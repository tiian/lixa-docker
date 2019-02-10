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
 * This program is the easiest example of an XTA for C main program: it just
 * creates the transactional objects and commits a dummy transaction without
 * any XA Resource Manager.
 */



/*
 * This header is necessary for all the XTA related definitions
 */
#include <xta/xta.h>



int main(int argc, char *argv[])
{
    /* LIXA / XTA return and reason code */
    int                           rc;
    /* XTA Transaction Manager object reference */
    xta_transaction_manager_t    *tm = NULL;
    /* XTA Transaction object reference */
    xta_transaction_t            *tx = NULL;

    /*
     * initialize XTA environment
     */
    xta_init();
    /*
     * create a new XTA Transaction Manager object
     */
    printf("Creating a new XTA Transaction Manager object...\n");
    tm = xta_transaction_manager_new();
    if (tm == NULL) {
        fprintf(stderr, "xta_transaction_manager_new: returned NULL\n");
        return 1;
    }
    /*
     * Create a new XA global transaction
     */
    printf("Creating a new XTA Transaction object...\n");
    tx = xta_transaction_manager_create_transaction(tm);
    if (tx == NULL) {
        fprintf(stderr, "xta_transaction_manager_get_transaction: "
                "returned NULL\n");
        return 1;
    }
    /*
     * Start a new XA global transaction with a single branch
     * Note: second argument ("multiple_branch") has FALSE value
     */
    printf("Starting a new transaction...\n");
    rc = xta_transaction_start(tx, FALSE);
    if (rc != LIXA_RC_OK) {
        fprintf(stderr, "xta_transaction_start: returned %d (%s)\n",
                rc, lixa_strerror(rc));
        return 1;
    }
    /*
     * commit the transaction
     */
    printf("Committing the transaction...\n");
    /* Note: second argument ("non_blocking") has FALSE value */
    rc = xta_transaction_commit(tx, FALSE);
    if (rc != LIXA_RC_OK) {
        fprintf(stderr, "xta_transaction_commit: returned %d (%s)\n",
                rc, lixa_strerror(rc));
        return 1;
    }
    /*
     * Delete Transaction Manager object
     */
    xta_transaction_manager_delete(tm);
    
    return 0;
}

