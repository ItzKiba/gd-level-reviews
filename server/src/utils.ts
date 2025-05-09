import { createHash, randomBytes } from "crypto";
import { GenError, GDMessage } from "./components";
import { Ok, Err, Result } from 'ts-results';

// rust and other stuffs
function base64Encode(input: string): string {
    return Buffer.from(input).toString('base64');
}

function base64Decode(input: string): string {
    return Buffer.from(input, 'base64').toString('utf-8');
}

export function randomStr(length: number): string {
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const bytes = randomBytes(length);
    let result = '';
    for (let i = 0; i < length; i++) {
        result += characters.charAt(bytes[i] % characters.length);
    }
    return result;
}

export function trimEndMatches(input: string, pattern: string): string {
    if (input.endsWith(pattern)) {
        return input.slice(0, -pattern.length);
    }
    return input.trimEnd();
}

export function trimStartMatches(input: string, pattern: string): string {
    if (input.startsWith(pattern)) {
        return input.slice(pattern.length);
    }
    return input.trimStart();
}

// the amount of time it took for me to come up with this is insane
interface TimedEntry<T> {
    value: T;
    expiration: number;
}

export class TimedMap<K, V> {
    private map: Map<K, TimedEntry<V>> = new Map();
    set(key: K, value: V, ttl: number): void {
        const expiration = Date.now() + ttl;
        this.map.set(key, { value, expiration });
    }
    get(key: K): V | null {
        const entry = this.map.get(key);
        if (entry) {
            if (Date.now() < entry.expiration) {
                return entry.value;
            } else {
                this.map.delete(key);
            }
        }
        return null;
    }
    delete(key: K): boolean {
        return this.map.delete(key);
    }
    find(fn: (value: TimedEntry<V>, key: K) => boolean): [K, TimedEntry<V>] | null {
        for (const [key, value] of this.map.entries()) {
            if (fn(value, key)) {
                return [key, value];
            }
        }
        return null;
    }
}

// rust and other stuffs

export function generateGJP2(password: string = ""): string {
    const combined = `${password}mI29fmAnxgTs`;
    const hash = createHash('sha1').update(combined).digest('hex');

    return hash;
}

export function parse_gj_messages_response(result: string): Result<GDMessage[], GenError> {
    if (result == "-2") return new Ok([]);

    const messages: GDMessage[] = [];
    const errors: GenError[] = [];
    const parts = result.split('|');
    for (const part of parts) {
        const values = new Map<number, string>();
        const items = part.split(':');
        let last_key = 0;
        for (let i = 0; i < items.length; i++) {
            if (i % 2 == 0) {
                last_key = parseInt(items[i]);
                if (isNaN(last_key)) {
                    console.log("an error", part, items[i])
                    errors.push(GenError.ParseIntError);
                    continue;
                }
            } else {
                values.set(last_key, trimEndMatches(items[i], ' '));
            }
        }
        const subject = values.get(4);
        if (!subject) {
            errors.push(GenError.ParseError);
            continue;
        }
        const acc_name = values.get(6);
        if (!acc_name) {
            errors.push(GenError.ParseError);
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
        return new Err(errors[0]);
    } else {
        return new Ok(messages);
    }
}