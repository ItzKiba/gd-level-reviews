"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.GenericError = exports.GenError = void 0;
var GenError;
(function (GenError) {
    GenError[GenError["InvalidAuthenticationError"] = 0] = "InvalidAuthenticationError";
    GenError[GenError["MissingAuthHeaderError"] = 1] = "MissingAuthHeaderError";
    GenError[GenError["IOError"] = 2] = "IOError";
    GenError[GenError["MissingEnvVarError"] = 3] = "MissingEnvVarError";
    GenError[GenError["EnvError"] = 4] = "EnvError";
    GenError[GenError["ExpressError"] = 5] = "ExpressError";
    GenError[GenError["UuidError"] = 6] = "UuidError";
    GenError[GenError["ParseIntError"] = 7] = "ParseIntError";
    GenError[GenError["ParseError"] = 8] = "ParseError";
})(GenError || (exports.GenError = GenError = {}));
class GenericError {
    static make_response_msg(res, code, message) {
        let err = {
            success: false,
            message,
            data: null
        };
        res.status(code).json(err);
    }
    static make_validator_msg(res, result) {
        let err = {
            success: false,
            message: "validation error",
            data: result
        };
        res.status(400).json(err);
    }
    static respond_to(res, err) {
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
exports.GenericError = GenericError;
//# sourceMappingURL=components.js.map