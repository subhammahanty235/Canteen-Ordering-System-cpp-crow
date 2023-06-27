BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "ADMIN" (
	"adminId"	INTEGER,
	"name"	TEXT,
	"password"	TEXT,
	"emailid"	TEXT,
	"mobileNumber"	TEXT,
	"totalRevenue"	INTEGER,
	PRIMARY KEY("adminId" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "USER" (
	"userId"	INTEGER,
	"name"	TEXT,
	"enrollment"	TEXT,
	"emailid"	TEXT,
	"mobileNumber"	TEXT,
	"password"	TEXT,
	"totalSpend"	INTEGER,
	PRIMARY KEY("userId" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "FOODITEM" (
	"itemId"	INTEGER,
	"name"	TEXT,
	"price"	INTEGER,
	PRIMARY KEY("itemId" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "food_order" (
	"orderId"	INTEGER,
	"customer_id"	INTEGER,
	"item_id"	INTEGER,
	"price"	INTEGER,
	"prepared"	INTEGER DEFAULT 0,
	"collected"	INTEGER DEFAULT 0,
	"order_name"	TEXT,
	PRIMARY KEY("orderId" AUTOINCREMENT)
);
INSERT INTO "ADMIN" VALUES (8284,'admin','admin','admin@collegecanteen.com','9876543210',500);
INSERT INTO "USER" VALUES (1,'Subham','220651','subhammahanty235@gmail.com','9932237171','3738u98x',547);
INSERT INTO "USER" VALUES (2,'Subham2','2206510','subhammahanty2352@gmail.com','9932237172','3738u98y',0);
INSERT INTO "USER" VALUES (3,'Subham3','2206511','subhammahanty2353@gmail.com','9932237173','3738u9xy',0);
INSERT INTO "USER" VALUES (4,'Ankit','220671','ankit@gmail.com','9932237171','ankit',0);
INSERT INTO "USER" VALUES (5,'Ankit','220671','ankit@gmail.com','9932237171','ankit',0);
INSERT INTO "USER" VALUES (6,'Ankit','220671','ankit@gmail.com','9932237171','ankit',0);
INSERT INTO "USER" VALUES (7,'user1','838928','user1@gmail.com','839489293','subham','');
INSERT INTO "FOODITEM" VALUES (1,'Idli',15);
INSERT INTO "FOODITEM" VALUES (2,'Samosa',15);
INSERT INTO "FOODITEM" VALUES (3,'pizza',75);
INSERT INTO "FOODITEM" VALUES (4,'Sandwich',30);
INSERT INTO "FOODITEM" VALUES (5,'Maggie',500);
INSERT INTO "FOODITEM" VALUES (6,'Paneer Roll',2);
INSERT INTO "food_order" VALUES (1,1,1,15,1,0,'Idli');
INSERT INTO "food_order" VALUES (2,1,2,15,1,1,'Samosa');
INSERT INTO "food_order" VALUES (3,1,4,30,0,0,'Sandwich');
INSERT INTO "food_order" VALUES (4,1,6,2,1,1,'Paneer Roll');
INSERT INTO "food_order" VALUES (5,1,4,30,1,1,'Sandwich');
INSERT INTO "food_order" VALUES (6,1,5,500,0,0,'Maggie');
INSERT INTO "food_order" VALUES (7,1,6,2,1,1,'Paneer Roll');
INSERT INTO "food_order" VALUES (8,1,3,75,0,0,'pizza');
INSERT INTO "food_order" VALUES (9,1,6,2,1,0,'Paneer Roll');
INSERT INTO "food_order" VALUES (10,1,5,500,1,1,'Maggie');
INSERT INTO "food_order" VALUES (11,1,4,30,0,0,'Sandwich');
INSERT INTO "food_order" VALUES (12,1,4,30,0,0,'Sandwich');
COMMIT;
