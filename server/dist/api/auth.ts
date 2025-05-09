import { ApiResponse, Challenge, CompletedChallenge, GenericError, GenError, User } from '../components';
import { generateGJP2, parse_gj_messages_response, trimStartMatches, randomStr, TimedMap } from '../utils';
import { createHash, randomBytes, randomUUID } from 'crypto';
import { Router, Request, Response } from 'express'
import { body, param, validationResult } from 'express-validator';
import { Result, Err, Ok } from 'ts-results';

const router = Router();

const pending_challenges = new TimedMap<number, Challenge>();
const completed_challenges = new TimedMap<number, CompletedChallenge>();
const auth_users: User[] = [];

function generateAuthToken(account_id: string): string {
    // what are the odds for a collision? 2^256!? 10^77!? wait... THIS IS THE SAME NUMBER!
    return createHash('sha256').update(randomBytes(32).toString('hex') + account_id).digest('hex');
}

export function verifyToken(token: string): Result<User, string> {
    const userToken = auth_users.find(user => user.token == token);
    if (!userToken) return new Err("Invalid token.");
    const currentTime = new Date();
    if (userToken.token_expiration < currentTime) {
        auth_users.splice(auth_users.indexOf(userToken), 1);
        return new Err("Token expired.")
    }
    return new Ok(userToken)
}

router.get("/api/v1/request_challenge/:id", 
    param('id').isInt({min: 0, max: 2147483647}).notEmpty(),
    async (req: Request, res: Response) => {
        const result = validationResult(req);
        if (!result.isEmpty()) return GenericError.make_validator_msg(res, result.array());
        const gd_acc = parseInt(req.params.id);
        let bot_id = parseInt(process.env.GD_ACC_ID || "");
        if (isNaN(bot_id)) return GenericError.respond_to(res, GenError.EnvError);
        let meow: Challenge = {
            bot_account_id: bot_id,
            challenge: randomStr(16),
            id: randomUUID(),
            attempts: 0
        };
        pending_challenges.set(gd_acc, meow, 8000)
        const response: ApiResponse<Challenge> = {
            success: true,
            message: "success",
            data: meow
        }
        res.json(response);
    }
)

router.post("/api/v1/verify",
    body('token').notEmpty().isString().withMessage("Token is required"),
    async (req: Request, res: Response) => {
        setTimeout(() => {
            const result = validationResult(req);
            if (!result.isEmpty()) return GenericError.make_validator_msg(res, result.array());
            const token = req.body.token as string;
            const verifyRes = verifyToken(token);
            if (verifyRes.err) return GenericError.make_response_msg(res, 401, verifyRes.val);
            const response: ApiResponse<User> = {
                success: true,
                message: "success",
                data: verifyRes.val
            }
            return res.status(200).json(response);
        }, 100)
    }
);

let checkForMessages = false;
setInterval(async () => {
    if (!checkForMessages) return;
    checkForMessages = false;
    console.log("Check for messages!")
    const gd_account_id = process.env.GD_ACC_ID;
    const gjp2 = process.env.GD_ACC_GJP2 || generateGJP2(process.env.GD_ACC_PW)
    const headers: any = {
        "Content-Type": "application/x-www-form-urlencoded",
        "User-Agent": ""
    }
    const reqData: any = {
        "accountID": parseInt(gd_account_id || "-1"),
        "gjp2": gjp2,
        "secret": "Wmfd2893gb7"
    }
    fetch("http://www.boomlings.com/database/getGJMessages20.php", {
        method: "POST",
        headers: headers,
        body: new URLSearchParams(reqData)
    }).then(async response => {
        const response_code = response.status;
        const response_text = await response.text();
        if (response_code != 200 || response_text == "-1") {
            return console.error("oopsie woopsie: " + response_code + ": " + response_text)
        }
        let messages = parse_gj_messages_response(response_text);
        if (messages.err) {
            return console.error("failed to request messages:", messages.val);
        }
        let message_deletion_string = messages.val.map(m => {
            if (m.subject.startsWith("auth-")) {
                let auth_code = trimStartMatches(m.subject, "auth-");
                console.log(`${m.id} auth message from ${m.from} = ${auth_code}`);
                while (completed_challenges.get(m.from)) {
                    completed_challenges.delete(m.from);
                }
                completed_challenges.set(m.from, {
                    challenge: auth_code.toString(),
                    gd_account_name: m.gd_account_name
                }, 15000);
                return m.id
            }
            return ""
        }).filter(x=>x != "").join(",")
        if (message_deletion_string != "") {
            const reqDataDelete: any = {
                "accountID": parseInt(gd_account_id || "-1"),
                "gjp2": gjp2,
                "secret": "Wmfd2893gb7",
                "messages": message_deletion_string
            }
            console.log(`Deleting ${message_deletion_string}...`)
            fetch("http://www.boomlings.com/database/deleteGJMessages20.php", {
                method: "POST",
                headers: headers,
                body: new URLSearchParams(reqDataDelete)
            }).then(async delResponse => {
                const response_code = delResponse.status;
                const response_text = await delResponse.text();
                if (response_code != 200 || response_text == "-1") {
                    return console.error("oopsie woopsie: " + response_code + ": " + response_text)
                }
                console.log(`Deleted ${message_deletion_string.split(",").length} message(s)!`);
            }).catch(console.error)
        }
    }).catch(console.error)
}, 5000)

router.get("/api/v1/challenge_complete/:id", 
    param('id').isUUID().notEmpty(),
    async (req: Request, res: Response) => {
        const result = validationResult(req);
        if (!result.isEmpty()) return GenericError.make_validator_msg(res, result.array());
        const uuid = req.params.id;
        const challenge_test = pending_challenges.find((entry) => entry.value.id == uuid);
        const challenge = pending_challenges.get((challenge_test && challenge_test[0]) || 0);
        if (!challenge) {
            console.log("didn't find challenge");
            return GenericError.respond_to(res, GenError.InvalidAuthenticationError);
        }
        if (challenge.attempts >= 7) {
            console.log("oopsies (ran out of tries)");
            return GenericError.respond_to(res, GenError.InvalidAuthenticationError);
        }
        challenge.attempts += 1;
        checkForMessages = true;
        const acc_id = challenge_test && challenge_test[0] || 0;
        setTimeout(() => {
            const completed_challenge = completed_challenges.get(acc_id);
            if (completed_challenge) {
                if (completed_challenge.challenge == challenge.challenge) {
                    const token = generateAuthToken(acc_id.toString());
                    const expiration = new Date(Date.now() + ((60 * 60 * 1000) * 24) * 7); // 1 week
                    auth_users.push({
                        id: acc_id,
                        username: completed_challenge.gd_account_name,
                        token: token,
                        token_expiration: expiration
                    })
                    completed_challenges.delete(acc_id)
                    pending_challenges.delete(acc_id)
                    const response: ApiResponse<string> = {
                        success: true,
                        message: "success",
                        data: token
                    }
                    res.json(response);
                } else {
                    console.log(`nuh uh ???? ${completed_challenge.challenge} =/= ${challenge.challenge}`)
                    GenericError.respond_to(res, GenError.InvalidAuthenticationError)
                }
            } else {
                console.log("didnt get enough time...?")
                GenericError.respond_to(res, GenError.InvalidAuthenticationError)
            }
        }, 4000);

    }
)

export default router;