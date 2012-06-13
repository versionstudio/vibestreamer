-- Server database init scripts
-- All queries MUST be separated by <LF><LF>

BEGIN;

-- PREPARE SERVER TABLES

CREATE TABLE IF NOT EXISTS groups (
  groupId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  guid TEXT
);

CREATE TABLE IF NOT EXISTS users (
  userId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  guid TEXT
);

CREATE TABLE IF NOT EXISTS shares (
  shareId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  guid TEXT
);

DROP TABLE IF EXISTS sessions;

CREATE TABLE sessions (
  sessionId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  guid TEXT
);

CREATE TABLE IF NOT EXISTS downloads (
  downloadId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  userId INTEGER,
  files INTEGER,
  size INTEGER,
  timeStamp DATE
);

-- PREPARE PLAYLISTS PLUGIN

CREATE TABLE IF NOT EXISTS playlists (
  playlistId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  userId INTEGER,
  name TEXT,
  shared INT
);

CREATE TABLE IF NOT EXISTS playlistitems (
  playlistItemId INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  playlistId INTEGER,
  hash TEXT
);

DROP TRIGGER IF EXISTS removePlaylists;

CREATE TRIGGER removePlaylists AFTER DELETE ON users
FOR EACH ROW BEGIN
  DELETE FROM playlistitems WHERE playlistId IN (SELECT playlistId FROM playlists WHERE userId=old.userId);
  DELETE FROM playlists WHERE userId=old.userId;
END;

COMMIT;