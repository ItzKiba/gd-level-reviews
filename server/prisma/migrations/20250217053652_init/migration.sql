-- CreateTable
CREATE TABLE "RequestChannel" (
    "id" SERIAL NOT NULL,
    "owner_account_id" INTEGER NOT NULL,
    "icon_id" INTEGER NOT NULL,
    "color1" INTEGER NOT NULL,
    "color2" INTEGER NOT NULL,
    "color3" INTEGER NOT NULL,
    "mod_level" INTEGER NOT NULL DEFAULT 0,
    "username" VARCHAR(20) NOT NULL,
    "max_requests" INTEGER NOT NULL DEFAULT 10,
    "levels" JSONB[],
    "is_open" BOOLEAN NOT NULL,
    "blacklist" INTEGER[],

    CONSTRAINT "RequestChannel_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Review" (
    "id" SERIAL NOT NULL,
    "owner_account_id" INTEGER NOT NULL,
    "requester_account_id" INTEGER NOT NULL,
    "level_id" INTEGER NOT NULL,
    "review_string" VARCHAR(500) NOT NULL,
    "date" TIMESTAMP(3) NOT NULL,

    CONSTRAINT "Review_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "User" (
    "account_id" INTEGER NOT NULL,
    "is_verified" BOOLEAN NOT NULL,
    "is_banned" BOOLEAN NOT NULL,

    CONSTRAINT "User_pkey" PRIMARY KEY ("account_id")
);

-- CreateTable
CREATE TABLE "StaffUser" (
    "account_id" INTEGER NOT NULL,
    "staff_level" INTEGER NOT NULL,
    "passkey" VARCHAR(32) NOT NULL,

    CONSTRAINT "StaffUser_pkey" PRIMARY KEY ("account_id")
);

-- CreateTable
CREATE TABLE "LogAction" (
    "id" SERIAL NOT NULL,
    "user_account_id" INTEGER NOT NULL,
    "staff_account_id" INTEGER NOT NULL,
    "set_verified_status" BOOLEAN NOT NULL,
    "set_ban_status" BOOLEAN NOT NULL,
    "reason_string" VARCHAR(100) NOT NULL,
    "date" TIMESTAMP(3) NOT NULL,

    CONSTRAINT "LogAction_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "RequestChannel_owner_account_id_key" ON "RequestChannel"("owner_account_id");
