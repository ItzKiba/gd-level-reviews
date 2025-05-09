/*
  Warnings:

  - Added the required column `level_name` to the `Review` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "Review" ADD COLUMN     "level_name" VARCHAR(20) NOT NULL;
