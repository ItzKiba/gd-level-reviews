/*
  Warnings:

  - Added the required column `owner_username` to the `Review` table without a default value. This is not possible if the table is not empty.
  - Added the required column `requester_username` to the `Review` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "Review" ADD COLUMN     "owner_username" VARCHAR(20) NOT NULL,
ADD COLUMN     "requester_username" VARCHAR(20) NOT NULL;
