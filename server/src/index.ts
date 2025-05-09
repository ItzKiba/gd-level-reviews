import express from 'express'
import bodyParser from 'body-parser'
import morgan from 'morgan';
import { rateLimit } from 'express-rate-limit'
import 'dotenv/config'
import { generateGJP2 } from './utils';
import router from './api/auth';
import channelRouter from './api/requestchannel'
import reviewRouter from './api/review'

const app = express();
const port = process.env.PORT;

app.use(bodyParser.json({ limit: '1mb' }));

app.set("trust proxy", 1);

if (!process.env.PRODUCTION) {
    app.use(morgan('combined'));
}

const limiter = rateLimit({
	windowMs: 3 * 60 * 1000,
	limit: 100,
	standardHeaders: 'draft-7',
	legacyHeaders: false,
    message: { success: false, message: "You are ratelimited! Please wait 3 minutes.", data: null }
})

app.use(limiter)

app.use(router)
app.use('/api/channel', channelRouter)
app.use('/api/review', reviewRouter)

app.all("/", (_, res) => {
    res.sendStatus(200)
})
app.listen(port, async () => { 
    console.log(`Server is running on port @${port}`);
    if (process.env.TEST_ACC && process.env.TEST_ACC == "1") {
        const gd_account_id = process.env.GD_ACC_ID;
        const gjp2 = process.env.GD_ACC_GJP2 || generateGJP2(process.env.GD_ACC_PW)
        console.log("acc id:", gd_account_id);
        console.log("gjp2:", gjp2.replace(/./g, '*'));
        console.log("Testing account...");
        const headers: any = {
            "Content-Type": "application/x-www-form-urlencoded",
            "User-Agent": ""
        }
        const reqData: any = {
            "targetAccountID": parseInt(gd_account_id || "-1"),
            "accountID": parseInt(gd_account_id || "-1"),
            "gjp2": gjp2,
            "secret": "Wmfd2893gb7"
        }
        const res = await fetch("http://www.boomlings.com/database/getGJUserInfo20.php", {
            method: "POST",
            headers: headers,
            body: new URLSearchParams(reqData)
        }).then(response => response.text())

        if (res == "-1") {
            throw new Error("Account ID or Password not valid.")
        }
        const split = res.split(":");
        if (split[0] != "1") {
            throw new Error("Account ID or Password not valid.")
        }
        const username = split[1];
        console.log("Account is valid! Account name:", username)
    }
})