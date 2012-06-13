-- Index database init scripts
-- All queries MUST be separated by <LF><LF>

BEGIN;

-- PREPARE INDEX TABLES

CREATE TABLE IF NOT EXISTS items (
  itemId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  shareId INTEGER,
  parentItemId INTEGER,
  name TEXT COLLATE NOCASE,
  path TEXT COLLATE NOCASE,
  hash TEXT,
  directory INT,
  directories INTEGER,
  files INTEGER,
  size INTEGER,
  lastWriteTime DATE
);

CREATE INDEX IF NOT EXISTS idxItemsHash ON items (hash);

CREATE INDEX IF NOT EXISTS idxItemsParentItemId ON items (parentItemId);

CREATE INDEX IF NOT EXISTS idxItemsPath ON items (path);

COMMIT;