#!/bin/bash
set -e

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" <<-EOSQL
	CREATE USER lixa;
	CREATE DATABASE lixa;
	GRANT ALL PRIVILEGES ON DATABASE lixa TO lixa;
	ALTER USER lixa WITH PASSWORD 'passw0rd';
EOSQL

psql -v ON_ERROR_STOP=1 --username lixa --dbname lixa <<-EOSQL
	CREATE TABLE "authors"
	("id" integer NOT NULL,
	"last_name" text, 
	"first_name" text,
	Constraint "authors_pkey" Primary Key ("id"));
EOSQL

