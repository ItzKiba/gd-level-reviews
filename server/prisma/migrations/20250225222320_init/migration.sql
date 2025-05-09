/*
  Warnings:

  - The primary key for the `RequestChannel` table will be changed. If it partially fails, the table could be left without primary key constraint.
  - You are about to drop the column `id` on the `RequestChannel` table. All the data in the column will be lost.

*/
-- DropIndex
DROP INDEX "RequestChannel_owner_account_id_key";

-- AlterTable
ALTER TABLE "RequestChannel" DROP CONSTRAINT "RequestChannel_pkey",
DROP COLUMN "id",
ADD CONSTRAINT "RequestChannel_pkey" PRIMARY KEY ("owner_account_id");
