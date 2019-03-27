#
# Copyright (c) 2009-2018, Christian Ferrari <tiian@users.sourceforge.net>
# All rights reserved.
#
# This file is part of LIXA.
#
# LIXA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# LIXA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LIXA.  If not, see <http://www.gnu.org/licenses/>.
#

#
# This program implement a REST client that uses XTA for Distributed
# Transaction Processing
#
# Programming Style note:
# the purpose of this small program is not to explain Python development
# techniques or good style, but simply to show XTA for Python using the easiest
# approach.
#

import requests
import time
import sys
import os
import MySQLdb
# This module is necessary for all the XTA related stuff
from xta import *

# retrieve IP address or name for MySQL database and MYAPP from env var
try:
    hostname = os.environ["SERVER"]
except KeyError:
    sys.stderr.write("Env var SERVER is not set\n")
    sys.exit(1)

# Prepare SQL statements
insert_stmt = "INSERT INTO authors VALUES(1840, 'Zola', 'Emile')"
delete_stmt = "DELETE FROM authors WHERE id=1840"

# initialize XTA environment
Xta_init()

# create a new MySQL connection
# Note: using MySQLdb functions
rm = MySQLdb.connect(host=hostname, user="lixa", password="passw0rd", db="lixa")

# create a new XTA Transaction Manager object
tm = TransactionManager()

# create an XA resource for MySQL
# second parameter "MySQL" is descriptive
# third parameter "localhost,0,lixa,,lixa" identifies the specific database
xar = MysqlXaResource(rm._get_native_connection(), "MySQL", 
        hostname + "/lixa")

# Create a new XA global transaction and retrieve a reference from
# the TransactionManager object
tx = tm.createTransaction()

# Enlist MySQL resource to transaction
tx.enlistResource(xar)

sys.stdout.write("***** REST client *****\n")
# Start a new XA global transaction with multiple branches
tx.start(True)

# Execute DELETE statement
sys.stdout.write("MySQL: executing SQL statement >" + delete_stmt + "<\n")
cur = rm.cursor()
cur.execute(delete_stmt)

# Retrieving xid
xid = tx.getXid().toString()

# Calling server passing xid
sys.stdout.write("Calling REST service passing: xid='" + xid + "', oper='delete'\n")
r = requests.post("http://" + hostname + ":18080/xta/myresource",
        data={'xid':xid, 'oper':'delete'})
sys.stdout.write("Server replied >" + r.text + "<\n")

# Commit the transaction
sys.stdout.write("Executing transaction commit\n")
tx.commit()

# A new Transaction object must be created because multiple branches does not
# allow the re-use of the same object
tx = tm.createTransaction()

# The Resource must be enlisted to new Trasanction object
tx.enlistResource(xar)

sys.stdout.write("***** REST client *****\n")
# Start a new XA global transaction with multiple branches
tx.start(True)

# Execute INSERT statement
sys.stdout.write("MySQL: executing SQL statement >" + insert_stmt + "<\n")
cur = rm.cursor()
cur.execute(insert_stmt)

# Retrieving xid
xid = tx.getXid().toString()

# Calling server passing xid
sys.stdout.write("Calling REST service passing: xid='" + xid + "', oper='insert'\n")
r = requests.post("http://" + hostname + ":18080/xta/myresource",
        data={'xid':xid, 'oper':'insert'})
sys.stdout.write("Server replied >" + r.text + "<\n")

# Commit the transaction
sys.stdout.write("Executing transaction commit\n")
tx.commit()

# Close the MySQL connection
cur.close()
