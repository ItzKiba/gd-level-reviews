import { Router, Request, Response } from 'express'
import { verifyToken } from './auth'
import prisma from '../prisma'

const channelRouter = Router();

/*
    ENDPOINT: /api/channel/upsert
    DESC: Should either update an already-existing request channel or create it if it does not exist
*/
channelRouter.post('/upsert', async (req: Request, res: Response) => {
    const { owner_account_id, icon_id, color1, color2, color3, mod_level, username, max_requests, is_open, token } = req.body;

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

    if (owner_account_id == null || icon_id == null || color1 == null || color2 == null ||
        color3 == null || mod_level == null || username == null || max_requests == null ||
        is_open == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {
        await prisma.requestChannel.upsert({
            where: { owner_account_id: owner_account_id },
            update: { icon_id, color1, color2, color3, mod_level, username, max_requests, is_open },
            create: { owner_account_id, icon_id, color1, color2, color3, mod_level, username, max_requests, is_open }
        });

        // console.log('Upserting channel with:', {
        //     owner_account_id, icon_id, color1, color2, color3, mod_level,
        //     username, max_requests, is_open
        // });

        res.status(200).json({
            success: true,
            message: `Request Channel for ${username} created/updated!`
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (upsert)'
        });
        return;
    }
    
});


/*
    ENDPOINT: /api/channel/blacklist-add
    DESC: Add a user to the channel's blacklist
*/
channelRouter.post('/blacklist-add', async (req: Request, res: Response) => {
    const { owner_account_id, user_id, token } = req.body;

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

    if (owner_account_id == null || user_id == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {

        const requestChannel = await prisma.requestChannel.findUnique({
            where: { owner_account_id: owner_account_id },
            select: { blacklist: true }
        });

        if (!requestChannel) {
            res.status(404).json({
                success: false,
                message: 'Request Channel not found'
            });
            return;
        }

        const blacklistArray: number[] = requestChannel.blacklist ?? [];
        
        if (blacklistArray.includes(user_id)) {
            res.status(409).json({ 
                success: false, 
                message: 'User is already blacklisted' 
            });
            return;
        }

        const updatedBlacklist = [...blacklistArray, user_id];

        await prisma.requestChannel.update({
            where: { owner_account_id: owner_account_id },
            data: { blacklist: updatedBlacklist }
        });

        res.status(200).json({
            success: true,
            message: `Successfully added user ${user_id} to blacklist of request channel with account id ${owner_account_id}!`
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (blacklist-add)'
        });
        return;
    }

});


/*
    ENDPOINT: /api/channel/blacklist-remove
    DESC: Removes a user from a channel's blacklist
*/
channelRouter.post('/blacklist-remove', async (req: Request, res: Response) => {
    const { owner_account_id, user_id, token } = req.body;

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

    if (owner_account_id == null || user_id == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {

        const requestChannel = await prisma.requestChannel.findUnique({
            where: { owner_account_id: owner_account_id },
            select: { blacklist: true }
        });

        if (!requestChannel) {
            res.status(404).json({
                success: false,
                message: 'Request Channel not found'
            });
            return;
        }

        const blacklistArray: number[] = requestChannel.blacklist ?? [];
        
        if (!blacklistArray.includes(user_id)) {
            res.status(409).json({ 
                success: false, 
                message: 'User not found in the blacklist' 
            });
            return;
        }

        const updatedBlacklist = blacklistArray.filter(id => id !== user_id);

        await prisma.requestChannel.update({
            where: { owner_account_id: owner_account_id },
            data: { blacklist: updatedBlacklist }
        });

        res.status(200).json({
            success: true,
            message: `Successfully removed user ${user_id} to blacklist of request channel with account id ${owner_account_id}!`
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (blacklist-remove)'
        });
        return;
    }

});

/*
    ENDPOINT: /api/channel/add-level
    DESC: Should add a level to the specified request channel, and close the request channel if the addition makes the channel full
*/
channelRouter.post('/add-level', async (req: Request, res: Response) => {
    const { owner_account_id, level_id, submitter_account_id, submitter_username, token } = req.body;

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

    if (owner_account_id == null || level_id == null || submitter_account_id == null ||
        submitter_username == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {

        const requestChannel = await prisma.requestChannel.findUnique({
            where: { owner_account_id: owner_account_id},
            select: { is_open: true, levels: true, max_requests: true, blacklist: true }
        });

        if (!requestChannel) {
            res.status(404).json({
                success: false,
                message: 'Request Channel not found'
            });
            return;
        }

        if (!requestChannel.is_open) {
            res.status(409).json({ 
                success: false, 
                message: 'Request Channel is closed' 
            });
            return;
        }

        const blacklistArray = requestChannel.blacklist ?? [];

        if (blacklistArray.includes(submitter_account_id)) {
            res.status(409).json({ 
                success: false, 
                message: 'You are blacklisted from this Request Channel' 
            });
            return;
        }

        const levelsArray = (requestChannel.levels as Array<{ level_id: number; submitter_account_id: number; submitter_username: string }> | null) ?? [];
        
        if (levelsArray.length >= requestChannel.max_requests) {
            res.status(409).json({ 
                success: false, 
                message: 'Request Channel is full' 
            });
            return;
        }

        const existingSubmitter = levelsArray.find(level => level.submitter_account_id === submitter_account_id);
        if (existingSubmitter) {
            res.status(409).json({ 
                success: false, 
                message: 'Submitter already has a level in this channel' 
            });
            return;
        }

        const existingLevel = levelsArray.find(level => level.level_id === level_id);
        if (existingLevel) {
            res.status(409).json({ 
                success: false, 
                message: 'This level is already in the request channel' 
            });
            return;
        }

        const updatedLevels = [...levelsArray, { level_id, submitter_account_id, submitter_username }];

        await prisma.requestChannel.update({
            where: { owner_account_id: owner_account_id },
            data: {
                levels: updatedLevels,
                is_open: updatedLevels.length >= requestChannel.max_requests ? false : true  // Close the request channel if the updated levels list is >= the max level request length
            }
        });

        res.status(200).json({
            success: true,
            message: `Successfully added level ${level_id} to request channel with account id ${owner_account_id}!`
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (add-level)'
        });
        return;
    }

});

/*
    ENDPOINT: /api/channel/remove-level
    DESC: Should remove a level from the specified request channel
*/
channelRouter.post('/remove-level', async (req: Request, res: Response) => {
    const { owner_account_id, level_id, token } = req.body;

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

    if (owner_account_id == null || level_id == null || token == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {
        const requestChannel = await prisma.requestChannel.findUnique({
            where: { owner_account_id: owner_account_id },
            select: { levels: true }
        });

        if (!requestChannel) {
            res.status(404).json({
                success: false,
                message: 'Request Channel not found'
            });
            return;
        }

        const levelsArray = (requestChannel.levels as Array<{ level_id: number; submitter_account_id: number; submitter_username: string }> | null) ?? [];

        const levelExists = levelsArray.some(level => level.level_id === level_id);
        if (!levelExists) {
            res.status(404).json({ 
                success: false, 
                message: 'Level not found in request channel' 
            });
            return;
        }

        const updatedLevels = levelsArray.filter(level => level.level_id !== level_id);

        await prisma.requestChannel.update({
            where: { owner_account_id: owner_account_id },
            data: { levels: updatedLevels }
        });

        res.status(200).json({
            success: true,
            message: `Successfully removed level ${level_id} from request channel with account id ${owner_account_id}!`
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (remove-level)'
        });
        return;
    }
});

/*
    ENDPOINT: /api/channel/get-channels-page
    DESC: Get a page of the request channel list, ordered primarily by the channel being open and ordered secondarily by mod level, then by alphabetical order
          Token not required
*/
channelRouter.post('/get-channels-page', async (req: Request, res: Response) => {
    const { page } = req.body;

    if (page == null || page < 1) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields or invalid page number'
        });
        return;
    }

    const pageSize = 20;

    try {
        const channels = await prisma.requestChannel.findMany({
            skip: (page - 1) * pageSize,
            take: pageSize,
            orderBy: [
                { is_open: 'desc' },
                { mod_level: 'desc' },
                { username: 'asc' }
            ],
            select: {
                owner_account_id: true,
                username: true,
                is_open: true,
                mod_level: true,
                icon_id: true,
                color1: true,
                color2: true,
                color3: true,
                max_requests: true,
                levels: true
            }
        });

        const channelCount = await prisma.requestChannel.count();

        // Replace levels json array with levels_count instead (don't need to send the json to the user)
        const processed = channels.map(({ levels, ...others}) => ({
            ...others,
            level_count: levels.length
        }));

        res.status(200).json({
            success: true,
            total: channelCount,
            data: processed
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (get-channels-page)'
        });
        return;
    }
});


/*
    ENDPOINT: /api/channel/get-channel
    DESC: Get all of the info of a channel
          Token not required
*/
channelRouter.post('/get-channel', async (req: Request, res: Response) => {
    const { owner_account_id } = req.body;

    if (owner_account_id == null) {
        res.status(400).json({
            success: false,
            message: 'Missing required fields'
        });
        return;
    }

    try {
        const channel = await prisma.requestChannel.findUnique({
            where: { owner_account_id: owner_account_id }
        });


        res.status(200).json({
            success: true,
            data: channel
        });
        return;

    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Internal server error (get-channel)'
        });
        return;
    }
});

export default channelRouter;