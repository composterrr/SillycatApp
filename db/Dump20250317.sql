-- Table structure for table "kitties"

DROP TABLE IF EXISTS "kitties";

CREATE TABLE "kitties" (
  "id" SERIAL PRIMARY KEY,
  "name" varchar(255) NOT NULL,
  "personality" text NOT NULL,
  "date" timestamp DEFAULT CURRENT_TIMESTAMP,
  "likes" integer DEFAULT 0
);

-- Dumping data for table "kitties"

INSERT INTO "kitties" ("id", "name", "personality", "date", "likes") VALUES
(1, 'Barsik', 'Affectionate', '2025-03-06 14:14:41', 10),
(2, 'Murzik', 'Quiet', '2025-03-06 14:14:41', 5),
(3, 'Timofey', 'Active', '2025-03-06 14:14:41', 12),
(4, 'Kesha', 'Clever', '2025-03-06 14:14:41', 8),
(5, 'Pushok', 'Brave', '2025-03-06 14:14:41', 20),
(6, 'Snezhok', 'Friendly', '2025-03-06 14:14:41', 15),
(7, 'Chernysh', 'Curious', '2025-03-06 14:14:41', 3),
(8, 'Greta', 'Shy', '2025-03-06 14:14:41', 7),
(9, 'Dymka', 'Calm', '2025-03-06 14:14:41', 4),
(10, 'Lylya', 'Cheerful', '2025-03-06 14:14:41', 18);

-- Table structure for table "users"

DROP TABLE IF EXISTS "users";

CREATE TABLE "users" (
  "ip" varchar(45) NOT NULL PRIMARY KEY,
  "voted" timestamp DEFAULT NULL
);

-- Dumping data for table "users"

INSERT INTO "users" ("ip", "voted") VALUES
('192.168.1.1', '2025-03-06 10:15:00'),
('192.168.1.2', NULL),
('192.168.1.3', '2025-03-06 11:00:00');
