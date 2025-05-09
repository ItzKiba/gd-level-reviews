"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.verifyToken = verifyToken;
const components_1 = require("../components");
const utils_1 = require("../utils");
const crypto_1 = require("crypto");
const express_1 = require("express");
const express_validator_1 = require("express-validator");
const ts_results_1 = require("ts-results");
const router = (0, express_1.Router)();
const pending_challenges = new utils_1.TimedMap();
const completed_challenges = new utils_1.TimedMap();
const auth_users = [];
function generateAuthToken(account_id) {
    // what are the odds for a collision? 2^256!? 10^77!? wait... THIS IS THE SAME NUMBER!
    return (0, crypto_1.createHash)('sha256').update((0, crypto_1.randomBytes)(32).toString('hex') + account_id).digest('hex');
}
function verifyToken(token) {
    const userToken = auth_users.find(user => user.token == token);
    if (!userToken)
        return new ts_results_1.Err("Invalid token.");
    const currentTime = new Date();
    if (userToken.token_expiration < currentTime) {
        auth_users.splice(auth_users.indexOf(userToken), 1);
        return new ts_results_1.Err("Token expired.");
    }
    return new ts_results_1.Ok(userToken);
}
router.get("/api/v1/request_challenge/:id", (0, express_validator_1.param)('id').isInt({ min: 0, max: 2147483647 }).notEmpty(), async (req, res) => {
    const result = (0, express_validator_1.validationResult)(req);
    if (!result.isEmpty())
        return components_1.GenericError.make_validator_msg(res, result.array());
    const gd_acc = parseInt(req.params.id);
    let bot_id = parseInt(process.env.GD_ACC_ID || "");
    if (isNaN(bot_id))
        return components_1.GenericError.respond_to(res, components_1.GenError.EnvError);
    let meow = {
        bot_account_id: bot_id,
        challenge: (0, utils_1.randomStr)(16),
        id: (0, crypto_1.randomUUID)(),
        attempts: 0
    };
    pending_challenges.set(gd_acc, meow, 8000);
    const response = {
        success: true,
        message: "success",
        data: meow
    };
    res.json(response);
});
router.post("/api/v1/verify", (0, express_validator_1.body)('token').notEmpty().isString().withMessage("Token is required"), async (req, res) => {
    setTimeout(() => {
        const result = (0, express_validator_1.validationResult)(req);
        if (!result.isEmpty())
            return components_1.GenericError.make_validator_msg(res, result.array());
        const token = req.body.token;
        const verifyRes = verifyToken(token);
        if (verifyRes.err)
            return components_1.GenericError.make_response_msg(res, 401, verifyRes.val);
        const response = {
            success: true,
            message: "success",
            data: verifyRes.val
        };
        return res.status(200).json(response);
    }, 100);
});
let checkForMessages = false;
setInterval(async () => {
    if (!checkForMessages)
        return;
    checkForMessages = false;
    console.log("Check for messages!");
    const gd_account_id = process.env.GD_ACC_ID;
    const gjp2 = process.env.GD_ACC_GJP2 || (0, utils_1.generateGJP2)(process.env.GD_ACC_PW);
    const headers = {
        "Content-Type": "application/x-www-form-urlencoded",
        "User-Agent": ""
    };
    const reqData = {
        "accountID": parseInt(gd_account_id || "-1"),
        "gjp2": gjp2,
        "secret": "Wmfd2893gb7"
    };
    fetch("http://www.boomlings.com/database/getGJMessages20.php", {
        method: "POST",
        headers: headers,
        body: new URLSearchParams(reqData)
    }).then(async (response) => {
        const response_code = response.status;
        const response_text = await response.text();
        if (response_code != 200 || response_text == "-1") {
            return console.error("oopsie woopsie: " + response_code + ": " + response_text);
        }
        let messages = (0, utils_1.parse_gj_messages_response)(response_text);
        if (messages.err) {
            return console.error("failed to request messages:", messages.val);
        }
        let message_deletion_string = messages.val.map(m => {
            if (m.subject.startsWith("auth-")) {
                let auth_code = (0, utils_1.trimStartMatches)(m.subject, "auth-");
                console.log(`${m.id} auth message from ${m.from} = ${auth_code}`);
                while (completed_challenges.get(m.from)) {
                    completed_challenges.delete(m.from);
                }
                completed_challenges.set(m.from, {
                    challenge: auth_code.toString(),
                    gd_account_name: m.gd_account_name
                }, 15000);
                return m.id;
            }
            return "";
        }).filter(x => x != "").join(",");
        if (message_deletion_string != "") {
            const reqDataDelete = {
                "accountID": parseInt(gd_account_id || "-1"),
                "gjp2": gjp2,
                "secret": "Wmfd2893gb7",
                "messages": message_deletion_string
            };
            console.log(`Deleting ${message_deletion_string}...`);
            fetch("http://www.boomlings.com/database/deleteGJMessages20.php", {
                method: "POST",
                headers: headers,
                body: new URLSearchParams(reqDataDelete)
            }).then(async (delResponse) => {
                const response_code = delResponse.status;
                const response_text = await delResponse.text();
                if (response_code != 200 || response_text == "-1") {
                    return console.error("oopsie woopsie: " + response_code + ": " + response_text);
                }
                console.log(`Deleted ${message_deletion_string.split(",").length} message(s)!`);
            }).catch(console.error);
        }
    }).catch(console.error);
}, 5000);
router.get("/api/v1/challenge_complete/:id", (0, express_validator_1.param)('id').isUUID().notEmpty(), async (req, res) => {
    const result = (0, express_validator_1.validationResult)(req);
    if (!result.isEmpty())
        return components_1.GenericError.make_validator_msg(res, result.array());
    const uuid = req.params.id;
    const challenge_test = pending_challenges.find((entry) => entry.value.id == uuid);
    const challenge = pending_challenges.get((challenge_test && challenge_test[0]) || 0);
    if (!challenge) {
        console.log("didn't find challenge");
        return components_1.GenericError.respond_to(res, components_1.GenError.InvalidAuthenticationError);
    }
    if (challenge.attempts >= 7) {
        console.log("oopsies (ran out of tries)");
        return components_1.GenericError.respond_to(res, components_1.GenError.InvalidAuthenticationError);
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
                });
                completed_challenges.delete(acc_id);
                pending_challenges.delete(acc_id);
                const response = {
                    success: true,
                    message: "success",
                    data: token
                };
                res.json(response);
            }
            else {
                console.log(`nuh uh ???? ${completed_challenge.challenge} =/= ${challenge.challenge}`);
                components_1.GenericError.respond_to(res, components_1.GenError.InvalidAuthenticationError);
            }
        }
        else {
            console.log("didnt get enough time...?");
            components_1.GenericError.respond_to(res, components_1.GenError.InvalidAuthenticationError);
        }
    }, 4000);
});
exports.default = router;
//# sourceMappingURL=auth.js.map