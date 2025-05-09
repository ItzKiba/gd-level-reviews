"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const body_parser_1 = __importDefault(require("body-parser"));
const morgan_1 = __importDefault(require("morgan"));
const express_rate_limit_1 = require("express-rate-limit");
require("dotenv/config");
const utils_1 = require("./utils");
const auth_1 = __importDefault(require("./api/auth"));
const requestchannel_1 = __importDefault(require("./api/requestchannel"));
const review_1 = __importDefault(require("./api/review"));
const app = (0, express_1.default)();
const port = process.env.PORT;
app.use(body_parser_1.default.json({ limit: '1mb' }));
app.set("trust proxy", 1);
if (!process.env.PRODUCTION) {
    app.use((0, morgan_1.default)('combined'));
}
const limiter = (0, express_rate_limit_1.rateLimit)({
    windowMs: 3 * 60 * 1000,
    limit: 100,
    standardHeaders: 'draft-7',
    legacyHeaders: false,
    message: { success: false, message: "You are ratelimited! Please wait 3 minutes.", data: null }
});
app.use(limiter);
app.use(auth_1.default);
app.use('/api/channel', requestchannel_1.default);
app.use('/api/review', review_1.default);
app.all("/", (_, res) => {
    res.sendStatus(200);
});
app.listen(port, async () => {
    console.log(`Server is running on port @${port}`);
    if (process.env.TEST_ACC && process.env.TEST_ACC == "1") {
        const gd_account_id = process.env.GD_ACC_ID;
        const gjp2 = process.env.GD_ACC_GJP2 || (0, utils_1.generateGJP2)(process.env.GD_ACC_PW);
        console.log("acc id:", gd_account_id);
        console.log("gjp2:", gjp2.replace(/./g, '*'));
        console.log("Testing account...");
        const headers = {
            "Content-Type": "application/x-www-form-urlencoded",
            "User-Agent": ""
        };
        const reqData = {
            "targetAccountID": parseInt(gd_account_id || "-1"),
            "accountID": parseInt(gd_account_id || "-1"),
            "gjp2": gjp2,
            "secret": "Wmfd2893gb7"
        };
        const res = await fetch("http://www.boomlings.com/database/getGJUserInfo20.php", {
            method: "POST",
            headers: headers,
            body: new URLSearchParams(reqData)
        }).then(response => response.text());
        if (res == "-1") {
            throw new Error("Account ID or Password not valid.");
        }
        const split = res.split(":");
        if (split[0] != "1") {
            throw new Error("Account ID or Password not valid.");
        }
        const username = split[1];
        console.log("Account is valid! Account name:", username);
    }
});
//# sourceMappingURL=index.js.map