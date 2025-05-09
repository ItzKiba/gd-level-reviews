"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TimedMap = void 0;
exports.randomStr = randomStr;
exports.trimEndMatches = trimEndMatches;
exports.trimStartMatches = trimStartMatches;
exports.generateGJP2 = generateGJP2;
exports.parse_gj_messages_response = parse_gj_messages_response;
const crypto_1 = require("crypto");
const components_1 = require("./components");
const ts_results_1 = require("ts-results");
// rust and other stuffs
function base64Encode(input) {
    return Buffer.from(input).toString('base64');
}
function base64Decode(input) {
    return Buffer.from(input, 'base64').toString('utf-8');
}
function randomStr(length) {
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const bytes = (0, crypto_1.randomBytes)(length);
    let result = '';
    for (let i = 0; i < length; i++) {
        result += characters.charAt(bytes[i] % characters.length);
    }
    return result;
}
function trimEndMatches(input, pattern) {
    if (input.endsWith(pattern)) {
        return input.slice(0, -pattern.length);
    }
    return input.trimEnd();
}
function trimStartMatches(input, pattern) {
    if (input.startsWith(pattern)) {
        return input.slice(pattern.length);
    }
    return input.trimStart();
}
class TimedMap {
    constructor() {
        this.map = new Map();
    }
    set(key, value, ttl) {
        const expiration = Date.now() + ttl;
        this.map.set(key, { value, expiration });
    }
    get(key) {
        const entry = this.map.get(key);
        if (entry) {
            if (Date.now() < entry.expiration) {
                return entry.value;
            }
            else {
                this.map.delete(key);
            }
        }
        return null;
    }
    delete(key) {
        return this.map.delete(key);
    }
    find(fn) {
        for (const [key, value] of this.map.entries()) {
            if (fn(value, key)) {
                return [key, value];
            }
        }
        return null;
    }
}
exports.TimedMap = TimedMap;
// rust and other stuffs
function generateGJP2(password = "") {
    const combined = `${password}mI29fmAnxgTs`;
    const hash = (0, crypto_1.createHash)('sha1').update(combined).digest('hex');
    return hash;
}
function parse_gj_messages_response(result) {
    if (result == "-2")
        return new ts_results_1.Ok([]);
    const messages = [];
    const errors = [];
    const parts = result.split('|');
    for (const part of parts) {
        const values = new Map();
        const items = part.split(':');
        let last_key = 0;
        for (let i = 0; i < items.length; i++) {
            if (i % 2 == 0) {
                last_key = parseInt(items[i]);
                if (isNaN(last_key)) {
                    console.log("an error", part, items[i]);
                    errors.push(components_1.GenError.ParseIntError);
                    continue;
                }
            }
            else {
                values.set(last_key, trimEndMatches(items[i], ' '));
            }
        }
        const subject = values.get(4);
        if (!subject) {
            errors.push(components_1.GenError.ParseError);
            continue;
        }
        const acc_name = values.get(6);
        if (!acc_name) {
            errors.push(components_1.GenError.ParseError);
            continue;
        }
        const decoded = base64Decode(subject);
        messages.push({
            id: values.get(1) || "0",
            from: parseInt(values.get(2) || "0") || 0,
            subject: decoded,
            gd_account_name: acc_name
        });
    }
    if (errors.length > 0) {
        return new ts_results_1.Err(errors[0]);
    }
    else {
        return new ts_results_1.Ok(messages);
    }
}
//# sourceMappingURL=utils.js.map