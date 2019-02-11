/*
 * Copyright (c) 2009-2019, Christian Ferrari <tiian@users.sourceforge.net>
 * All rights reserved.
 *
 * This file is part of LIXA.
 *
 * LIXA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * LIXA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LIXA.  If not, see <http://www.gnu.org/licenses/>.
 */



/*
 * This program is an example implementation of the
 * "Single Application" Pattern
 * as documented in LIXA manual:
 * http://www.tiian.org/lixa/manuals/html/index.html
 *
 * This program accepts exactly two parameters on the command line:
 * first parameter:  "commit", boolean value (if FALSE, "rollback")
 * second parameter: "insert", boolean value (if FALSE, "delete")
 *
 * Programming Style note:
 * the purpose of this small program is not to explain C development
 * techniques or good style, but simply to show XTA for C using the easiest
 * approach.
 */



/*
 * This header is necessary for all the XTA related definitions
 */
#include <xta/xta.h>



int main(int argc, char *argv[])
{
    /* LIXA / XTA return and reason code */
    int                           rc;
    /* First parameter: commit transaction? */
    int                           commit;
    /* Second parameter: insert data in databases? */
    int                           insert;
    /* PostgreSQL server hostname */
    const char                   *hostname;
    /* PostgreSQL connection string */
    char                          pg_conn_str[1000];
    /* native PostgreSQL connection handler */
    PGconn                       *rm1 = NULL;
    /* PostgreSQL result */
    PGresult                     *pg_res;
    /* variable for PostgreSQL statement to execute */
    const char                   *postgresql_stmt;
    /* XTA Resource for PostgreSQL */
    xta_postgresql_xa_resource_t *xar1 = NULL;
    /* XTA Transaction Manager object reference */
    xta_transaction_manager_t    *tm = NULL;
    /* XTA Transaction object reference */
    xta_transaction_t            *tx = NULL;

    /*
     * Check command line parameters
     */
    if (argc < 4) {
        fprintf(stderr, "This program requires PostgreSQL hostname and "
		"two boolean parameters: 'commit' and 'insert'\n");
        return 1;
    }
    hostname = argv[1];
    commit = strtol(argv[2], NULL, 0);
    insert = strtol(argv[3], NULL, 0);
    /*
     * Prepare SQL statements in accordance with "insert" command line
     * parameter
     */
    if (insert) {
        postgresql_stmt = "INSERT INTO authors VALUES(1921, 'Rigoni Stern', "
            "'Mario')";
    } else {
        postgresql_stmt = "DELETE FROM authors WHERE id=1921";
    }

    /*
     * initialize XTA environment
     */
    xta_init();
    /*
     * create a new PostgreSQL connection
     */
    snprintf(pg_conn_str, sizeof(pg_conn_str),
		"host=%s user=lixa password=passw0rd", hostname);
    rm1 = PQconnectdb(pg_conn_str);
    if (PQstatus(rm1) != CONNECTION_OK) {
        fprintf(stderr, "PQconnectdb: returned error %s\n",
                PQerrorMessage(rm1));
        PQfinish(rm1);
        return 1;
    }
    /*
     * create a new XTA Transaction Manager object
     */
    tm = xta_transaction_manager_new();
    if (tm == NULL) {
        fprintf(stderr, "xta_transaction_manager_new: returned NULL\n");
        return 1;
    }
    /*
     * create an XA resource for PostgreSQL
     * second parameter "PostgreSQL" is descriptive
     * third parameter "dbname=testdb" identifies the specific database
     */
    xar1 = xta_postgresql_xa_resource_new(rm1, "PostgreSQL", "dbname=testdb");
    if (xar1 == NULL) {
        fprintf(stderr, "xta_postgresql_xa_resource_new: returned NULL\n");
        return 1;
    }
    /*
     * Create a new XA global transaction
     */
    tx = xta_transaction_manager_create_transaction(tm);
    if (tx == NULL) {
        fprintf(stderr, "xta_transaction_manager_get_transaction: "
                "returned NULL\n");
        return 1;
    }
    /*
     * Enlist PostgreSQL resource to transaction
     */
    rc = xta_transaction_enlist_resource(tx, (xta_xa_resource_t *)xar1);
    if (rc != LIXA_RC_OK) {
        fprintf(stderr, "xta_transaction_enlist_resource returned %d (%s) for "
               "PostgreSQL XA resource\n", rc, lixa_strerror(rc));
        return 1;
    }
    /*
     * Start a new XA global transaction with a single branch
     * Note: second argument ("multiple_branch") has FALSE value
     */
    rc = xta_transaction_start(tx, FALSE);
    if (rc != LIXA_RC_OK) {
        fprintf(stderr, "xta_transaction_start: returned %d (%s)\n",
                rc, lixa_strerror(rc));
        return 1;
    }
    /*
     * At this point, it's time to do something with the Resource Managers
     * (PostgreSQL and MySQL)
     *
     * Execute PostgreSQL statement
     */
    printf("PostgreSQL, executing >%s<\n", postgresql_stmt);
    pg_res = PQexec(rm1, postgresql_stmt);
    if (PQresultStatus(pg_res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "PostgreSQL, error while executing >%s<: %s\n",
                postgresql_stmt, PQerrorMessage(rm1));
        PQclear(pg_res);
        PQfinish(rm1);
        return 1;
    }
    /*
     * commit or rollback the transaction
     */
    if (commit) {
        /* Note: second argument ("non_blocking") has FALSE value */
        rc = xta_transaction_commit(tx, FALSE);
        if (rc != LIXA_RC_OK) {
            fprintf(stderr, "xta_transaction_commit: returned %d (%s)\n",
                    rc, lixa_strerror(rc));
            return 1;
        }
    } else {
        rc = xta_transaction_rollback(tx);
        if (rc != LIXA_RC_OK) {
            fprintf(stderr, "xta_transaction_rollback: returned %d (%s)\n",
                    rc, lixa_strerror(rc));
            return 1;
        }
    }
    /*
     * Delete Transaction Manager object
     */
    xta_transaction_manager_delete(tm);
    /*
     * Delete PostgreSQL native and XA resource
     */
    xta_postgresql_xa_resource_delete(xar1);
    PQfinish(rm1);
    
    return 0;
}
