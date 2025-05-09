import { UUID } from "crypto"
import { Response } from 'express'
import { ValidationError } from "express-validator"

export enum GenError {
    InvalidAuthenticationError,
    MissingAuthHeaderError,
    IOError,
    MissingEnvVarError,
    EnvError,
    ExpressError,
    UuidError,
    ParseIntError,
    ParseError
}

export interface ApiResponse<T> {
    success: boolean,
    message: string,
    data?: T
}

export class GenericError {
    public static make_response_msg(res: Response, code: number, message: string): void {
        let err: ApiResponse<null> = {
            success: false,
            message,
            data: null
        }
        res.status(code).json(err);
    }
    public static make_validator_msg(res: Response, result: ValidationError[]): void {
        let err: ApiResponse<ValidationError[]> = {
            success: false,
            message: "validation error",
            data: result
        }
        res.status(400).json(err);
    }
    public static respond_to(res: Response, err: GenError) {
        switch (err) {
            case GenError.InvalidAuthenticationError:
            case GenError.MissingAuthHeaderError:
                return this.make_response_msg(res, 401, "invalid authentication");
            case GenError.UuidError:
                return this.make_response_msg(res, 400, "invalid uuid");
            case GenError.IOError:
                return this.make_response_msg(res, 500, "io error");
            case GenError.MissingEnvVarError:
                return this.make_response_msg(res, 500, "env error");
            default:
                return this.make_response_msg(res, 500, "internal server error");
        }
    }
}



export interface User {
    id: number,
    username: string,
    token: string,
    token_expiration: Date
}

export interface Challenge {
    bot_account_id: number,
    challenge: string,
    id: UUID,
    attempts: number,
}

export interface CompletedChallenge {
    gd_account_name: string
    challenge: string
}

export interface GDMessage {
    id: string,
    from: number,
    gd_account_name: string,
    subject: string
}