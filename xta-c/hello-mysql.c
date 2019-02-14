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
 * This program accepts exactly three parameters on the command line:
 * first parameter: hostname for the MySQL server
 * second parameter:  "commit", boolean value (if FALSE, "rollback")
 * third parameter: "insert", boolean value (if FALSE, "delete")
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
    /* First parameter: MySQL server hostname */
    const char                   *hostname;
    /* Second parameter: commit transaction? */
    int                           commit;
    /* Third parameter: insert data in databases? */
    int                           insert;
    /* native MySQL connection handler */
    MYSQL                        *rm2 = NULL;
    /* variable for MySQL statement to execute */
    const char                   *mysql_stmt;
    /* XTA Resource for MySQL */
    xta_mysql_xa_resource_t      *xar2 = NULL;
    /* XTA Transaction Manager object reference */
    xta_transaction_manager_t    *tm = NULL;
    /* XTA Transaction object reference */
    xta_transaction_t            *tx = NULL;

    /*
     * Check command line parameters
     */
    if (argc < 4) {
        fprintf(stderr, "This program requires MySQL hostname and "
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
        mysql_stmt = "INSERT INTO authors VALUES(1919, 'Levi', 'Primo')";
    } else {
        mysql_stmt = "DELETE FROM authors WHERE id=1919";
    }

    /*
     * initialize XTA environment
     */
    xta_init();
    /*
     * create a new MySQL connection
     */
    rm2 = mysql_init(NULL);
    if (rm2 == NULL) {
        fprintf(stderr, "mysql_init: returned NULL\n");
        return 1;
    }
    if (mysql_real_connect(rm2, hostname, "lixa", "passw0rd",
                           "lixa", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect: returned error: %u, %s\n",
                mysql_errno(rm2), mysql_error(rm2));
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
     * create an XA resource for MySQL
     * second parameter "MySQL" is descriptive
     * third parameter "localhost,0,lixa,,lixa" identifies the specific
     * database
     */
    xar2 = xta_mysql_xa_resource_new(rm2, "MySQL", "localhost,0,lixa,,lixa");
    if (xar2 == NULL) {
        fprintf(stderr, "xta_mysql_xa_resource_new: returned NULL\n");
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
     * Enlist MySQL resource to Transaction
     */
    rc = xta_transaction_enlist_resource(tx, (xta_xa_resource_t *)xar2);
    if (rc != LIXA_RC_OK) {
        fprintf(stderr, "xta_transaction_enlist_resource returned %d (%s) for "
               "MySQL XA resource\n", rc, lixa_strerror(rc));
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
     * Execute MySQL statement
     */
    printf("MySQL, executing >%s<\n", mysql_stmt);
    if (mysql_query(rm2, mysql_stmt)) {
        fprintf(stderr, "MySQL, error while executing >%s<: %u/%s\n",
                mysql_stmt, mysql_errno(rm2), mysql_error(rm2));
        mysql_close(rm2);
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
     * Delete MySQL native and XA resource
     */
    xta_mysql_xa_resource_delete(xar2);
    mysql_close(rm2);
    
    return 0;
}

