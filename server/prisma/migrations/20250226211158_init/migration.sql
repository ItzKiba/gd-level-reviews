-- AlterTable
ALTER TABLE "StaffUser" ALTER COLUMN "staff_level" SET DEFAULT 0;

-- AlterTable
ALTER TABLE "User" ALTER COLUMN "is_verified" SET DEFAULT false,
ALTER COLUMN "is_banned" SET DEFAULT false;
