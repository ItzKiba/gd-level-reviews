import { Router, Request, Response } from 'express'
import { verifyToken } from './auth'
import prisma from '../prisma'

const reviewRouter = Router();

/*
    ENDPOINT: /api/review/create-review
    DESC: Creates a review tied to a specific reviewer and account id
*/
reviewRouter.post('/create-review', async (req: Request, res: Response) => {
    const { owner_account_id, owner_username, requester_account_id, requester_username, level_id, level_name, review_string, token } = req.body;

    if (process.env.REQUIRE_TOKEN == "1") {
        const verify = verifyToken(token);
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
        await prisma.review.create({
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

    } catch (error) {
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
reviewRouter.post('/get-reviews-of-level', async (req: Request, res: Response) => {
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
        const reviews = await prisma.review.findMany({
            where: { level_id: level_id },
            orderBy: [
                { date: 'desc' }
            ],
            skip: (page - 1) * pageSize,
            take: pageSize
        });

        const reviewsCount = await prisma.review.count({
            where: { level_id: level_id }
        });

        res.status(200).json({
            success: true,
            total: reviewsCount,
            data: reviews
        });
        return;

    } catch (error) {
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
reviewRouter.post('/get-reviews-written-by-user', async (req: Request, res: Response) => {
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
        const reviews = await prisma.review.findMany({
            where: { owner_account_id: account_id },
            orderBy: [
                { date: 'desc' }
            ],
            skip: (page - 1) * pageSize,
            take: pageSize
        });

        const reviewsCount = await prisma.review.count({
            where: { owner_account_id: account_id }
        });

        res.status(200).json({
            success: true,
            total: reviewsCount,
            data: reviews
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (get-reviews-written-by-user)'
        });
        return;
    }
});

export default reviewRouter;