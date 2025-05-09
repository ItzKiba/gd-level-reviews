"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = require("express");
const auth_1 = require("./auth");
const prisma_1 = __importDefault(require("../prisma"));
const reviewRouter = (0, express_1.Router)();
/*
    ENDPOINT: /api/review/create-review
    DESC: Creates a review tied to a specific reviewer and account id
*/
reviewRouter.post('/create-review', async (req, res) => {
    const { owner_account_id, owner_username, requester_account_id, requester_username, level_id, level_name, review_string, token } = req.body;
    if (process.env.REQUIRE_TOKEN == "1") {
        const verify = (0, auth_1.verifyToken)(token);
        if (verify.err) {
            res.status(401).json({
                success: false,
                message: 'Invalid API token (restart your game)'
            });
            return;
        }
    }
    if (owner_account_id == null || owner_username == null || requester_account_id == null || requester_username == null || level_id == null || level_name == null || review_string == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }
    if (review_string.length > 500) {
        res.status(402).json({
            success: false,
            message: 'Review is too long (maximum is 500 characters)'
        });
        return;
    }
    try {
        await prisma_1.default.review.create({
            data: {
                owner_account_id,
                owner_username,
                requester_account_id,
                requester_username,
                level_id,
                level_name,
                review_string,
                date: new Date()
            }
        });
        res.status(200).json({
            success: true,
            message: `Review created successfully!`
        });
        return;
    }
    catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (create-review)'
        });
        return;
    }
});
/*
    ENDPOINT: /api/review/get-reviews-of-level
    DESC: Get the paginated reviews of a specific level, ordered by date created
          Token not required
*/
reviewRouter.post('/get-reviews-of-level', async (req, res) => {
    const { page, level_id } = req.body;
    if (page == null || page < 1 || level_id == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields or invalid page number'
        });
        return;
    }
    const pageSize = 20;
    try {
        const reviews = await prisma_1.default.review.findMany({
            where: { level_id: level_id },
            orderBy: [
                { date: 'desc' }
            ],
            skip: (page - 1) * pageSize,
            take: pageSize
        });
        const reviewsCount = await prisma_1.default.review.count({
            where: { level_id: level_id }
        });
        res.status(200).json({
            success: true,
            total: reviewsCount,
            data: reviews
        });
        return;
    }
    catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (get-reviews-of-level)'
        });
        return;
    }
});
/*
    ENDPOINT: /api/review/get-reviews-written-by-user
    DESC: Get the paginated reviews of a specific user, ordered by date created
          Token not required
*/
reviewRouter.post('/get-reviews-written-by-user', async (req, res) => {
    const { page, account_id } = req.body;
    if (page == null || page < 1 || account_id == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields or invalid page number'
        });
        return;
    }
    const pageSize = 20;
    try {
        const reviews = await prisma_1.default.review.findMany({
            where: { owner_account_id: account_id },
            orderBy: [
                { date: 'desc' }
            ],
            skip: (page - 1) * pageSize,
            take: pageSize
        });
        const reviewsCount = await prisma_1.default.review.count({
            where: { owner_account_id: account_id }
        });
        res.status(200).json({
            success: true,
            total: reviewsCount,
            data: reviews
        });
        return;
    }
    catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (get-reviews-written-by-user)'
        });
        return;
    }
});
exports.default = reviewRouter;
//# sourceMappingURL=review.js.map