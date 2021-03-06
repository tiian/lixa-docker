# LIXA and XTA

LIXA, LIbre XA, is a free and open source implementation of the XA and TX
specifications; it supports even XTA.
XTA, XA Transaction API, is a new API that provides two phase commit feature
to applications; it has been designed to work in a *microservices*
environment.
LIXA and XTA documentation is available at http://www.tiian.org/lixa/

# xta-c: XA Transaction API for the C language

This image contains the libraries and the include files that are necessary to
develop a C application with the support of XTA. Two drivers are provided:
MySQL and PostgreSQL.

## How to use this image

The basic usage of this image is as a starting point to create derived images
with an application inside. Below there are some examples that can be 
implemented using the base image.

## Examples based on xta-c Docker image

First of all, clone *lixa-docker* repository locally:

```
git clone https://github.com/tiian/lixa-docker.git
cd lixa-docker/xta-c/
```

it contains the source files and the Dockerfiles that are necessary to build
some examples that use xta-c base image.

### XTA Hello World!

Even XTA has its own *Hello World!* program, but it does not write
"Hello World!" on the terminal...
The easiest possible C program with XTA is provided in source file
hello-world.c

Its Docker image can be created with:

```
docker build -f Dockerfile-hello-world -t lixa/xta-c-hello-world .
```

before starting it, you need a LIXA state server up and running; you may use
a Docker image even for it:

```
docker run --rm -p 2345:2345 -d lixa/lixad
```

Retrieve the IP address of the host where *lixad* is running, it must be used
to reach it from another container. Supposing the IP address is
*192.168.123.35*, start the *XTA Hello World* container and execute the
*Hello World* program located in */myapp* directory:

```
docker run --rm -it lixa/xta-c-hello-world bash

root@5b3168d653a8:/# export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"

root@5b3168d653a8:/# /myapp/hello-world 

Creating a new XTA Transaction Manager object...
2019-02-11 22:02:21.292122 [6/139869572630464] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
Creating a new XTA Transaction object...
Starting a new transaction...
Committing the transaction...
```

That's all! You have just executed your first XTA program:

* it has started a LIXA transaction manager
* it has created and started a new transaction
* it has committed it!

Take a look to the source program in git: https://github.com/tiian/lixa-docker/blob/master/xta-c/hello-world.c 

### XTA with PostgreSQL

This example shows as you can perform transactions with PostgreSQL database.
First of all, you should execute a PostgreSQL instance: there's one ready to
use for this example:

```
docker run -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/lixa-postgres -c 'max_prepared_transactions=10'
```

At this point you can build the *Hello PostgreSQL* image:

```
docker build -f Dockerfile-hello-postgresql -t lixa/xta-c-hello-postgresql .
```

Retrieve the IP address of the host where *lixad* and *postgres* are running,
it must be used to reach them from another container. Supposing the IP address
is *192.168.123.35*, start the *XTA Hello PostgreSQL* container and execute the
*Hello PostgreSQL* program located in */myapp* directory:

```
docker run --rm -it lixa/xta-c-hello-postgresql bash

root@5240c89b72a8:/# export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"
root@5240c89b72a8:/# /myapp/hello-postgresql 192.168.123.35 1 1

2019-02-11 22:17:21.169952 [7/139976635893696] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >INSERT INTO authors VALUES(1921, 'Rigoni Stern', 'Mario')<
```

You have just inserted a row into table *authors* and committed your change;
use your own preferred PostgreSQL client to verify it from another system
(password for user "lixa" is "passw0rd"):

```
tiian@ubuntu1404-64:~$ psql -h 192.168.123.35 -U lixa
Password for user lixa: 
psql (9.3.24, server 11.1 (Debian 11.1-3.pgdg90+1))
WARNING: psql major version 9.3, server major version 11.
         Some psql features might not work.
Type "help" for help.

lixa=> select * from authors;
  id  |  last_name   | first_name 
------+--------------+------------
 1921 | Rigoni Stern | Mario
(1 row)

lixa=> 
```

the same program supports even *DELETE*:

```
root@5240c89b72a8:/# /myapp/hello-postgresql 192.168.123.35 1 0

2019-02-11 22:19:44.907827 [8/140000182470592] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >DELETE FROM authors WHERE id=1921<
```

check it again from your client:
```
lixa=> select * from authors;
 id | last_name | first_name 
----+-----------+------------
(0 rows)

lixa=> 
```

the same program supports even *ROLLBACK*:

```
root@5240c89b72a8:/# /myapp/hello-postgresql 192.168.123.35 0 1

2019-02-11 22:21:01.163932 [9/140043098961856] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >INSERT INTO authors VALUES(1921, 'Rigoni Stern', 'Mario')<
```

you can verify that there's no row in the table after *INSERT* and 
*ROLLBACK*!

Take a look to the source program in git: https://github.com/tiian/lixa-docker/blob/master/xta-c/hello-postgresql.c 

### XTA with MySQL

This example shows as you can perform transactions with MySQL database.
First of all, you should execute a MySQL instance: there's one ready to
use for this example:

```
docker run -e MYSQL_ROOT_PASSWORD=mysecretpw -p 3306:3306 -d lixa/mysql
```

At this point you can build the *Hello MySQL* image:

```
docker build -f Dockerfile-hello-mysql -t lixa/xta-c-hello-mysql .
```

Retrieve the IP address of the host where *lixad* and *mysqld* are running,
it must be used to reach them from another container. Supposing the IP address
is *192.168.123.35*, start the *XTA Hello MySQL* container and execute the
*Hello MySQL* program located in */myapp* directory:

```
docker run --rm -it lixa/xta-c-hello-mysql bash

root@b9b2c77b6ae9:/# export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"
root@b9b2c77b6ae9:/# /myapp/hello-mysql 192.168.123.35 1 1

2019-02-14 22:01:45.597170 [6/140118459779008] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
MySQL, executing >INSERT INTO authors VALUES(1919, 'Levi', 'Primo')<
```

You have just inserted a row into table *authors* and committed your change;
use your own preferred MySQL client to verify it from another system
(password for user "lixa" is "passw0rd"):

```
tiian@ubuntu1404-64:~$ mysql -h 192.168.123.35 -u lixa -p lixa
Enter password: 
Reading table information for completion of table and column names
You can turn off this feature to get a quicker startup with -A

Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 4
Server version: 5.7.25 MySQL Community Server (GPL)

Copyright (c) 2000, 2018, Oracle and/or its affiliates. All rights reserved.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql> select * from authors;
+------+-----------+------------+
| id   | last_name | first_name |
+------+-----------+------------+
| 1919 | Levi      | Primo      |
+------+-----------+------------+
1 row in set (0.00 sec)

mysql> 
```

the same program supports even *DELETE*:

```
root@b9b2c77b6ae9:/# /myapp/hello-mysql 192.168.123.35 1 0

2019-02-14 22:05:22.526721 [7/139694207129536] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
MySQL, executing >DELETE FROM authors WHERE id=1919<
```

check it again from your client:
```
mysql> select * from authors;
Empty set (0.00 sec)

mysql> 
```

the same program supports even *ROLLBACK*:

```
root@b9b2c77b6ae9:/# /myapp/hello-mysql 192.168.123.35 0 1

2019-02-14 22:06:09.742814 [8/140007537379264] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
MySQL, executing >INSERT INTO authors VALUES(1919, 'Levi', 'Primo')<
```

you can verify that there's no row in the table after *INSERT* and 
*ROLLBACK*!

Take a look to the source program in git: https://github.com/tiian/lixa-docker/blob/master/xta-c/hello-mysql.c 

### XTA with MySQL & PostgreSQL

This example shows as you can perform transactions with MySQL and PostgreSQL
databases.
First of all, you should execute a MySQL and a PostgreSQL instance: there's a 
couple ready to be used for this example:

```
docker run -e MYSQL_ROOT_PASSWORD=mysecretpw -p 3306:3306 -d lixa/mysql
docker run -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/lixa-postgres -c 'max_prepared_transactions=10'
```

At this point you can build the *Hello MySQL PostgreSQL* image:

```
docker build -f Dockerfile-hello-mysql-postgresql -t lixa/xta-c-hello-mysql-postgresql .
```

Retrieve the IP address of the host where *lixad*, *mysqld*  and *postgres* are
running, it must be used to reach them from another container. Supposing the IP
address is *192.168.123.35*, start the *XTA Hello PostgreSQL* container and
execute the *Hello MySQL PostgreSQL* program located in */myapp* directory:

```
docker run --rm -it lixa/xta-c-hello-mysql-postgresql bash

root@5bff2e527f3c:/# export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"
root@5bff2e527f3c:/# /myapp/hello-mysql-postgresql 192.168.123.35 1 1

2019-02-14 22:14:25.997027 [6/140193624786880] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >INSERT INTO authors VALUES(1921, 'Rigoni Stern', 'Mario')<
MySQL, executing >INSERT INTO authors VALUES(1919, 'Levi', 'Primo')<
```

You have just inserted a row into tables *authors* and committed your change;
use your own preferred MySQL and PostgreSQL clients to verify it from another
system (password for "lixa" user is "passw0rd"):

```
tiian@ubuntu1404-64:~$ mysql -h 192.168.123.35 -u lixa -p lixa
Enter password: 
Reading table information for completion of table and column names
You can turn off this feature to get a quicker startup with -A

Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 4
Server version: 5.7.25 MySQL Community Server (GPL)

Copyright (c) 2000, 2018, Oracle and/or its affiliates. All rights reserved.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql> select * from authors;
+------+-----------+------------+
| id   | last_name | first_name |
+------+-----------+------------+
| 1919 | Levi      | Primo      |
+------+-----------+------------+
1 row in set (0.00 sec)

mysql> 



tiian@ubuntu1404-64:~$ psql -h 192.168.123.35 -U lixa
Password for user lixa: 
psql (9.3.24, server 11.1 (Debian 11.1-3.pgdg90+1))
WARNING: psql major version 9.3, server major version 11.
         Some psql features might not work.
Type "help" for help.

lixa=> select * from authors;
  id  |  last_name   | first_name 
------+--------------+------------
 1921 | Rigoni Stern | Mario
(1 row)

lixa=> 
```

the same program supports even *DELETE*:

```
root@5bff2e527f3c:/# /myapp/hello-mysql-postgresql 192.168.123.35 1 0

2019-02-14 22:17:41.734837 [7/139966431422400] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >DELETE FROM authors WHERE id=1921<
MySQL, executing >DELETE FROM authors WHERE id=1919<
```

check it again from your client:
```
mysql> select * from authors;
Empty set (0.00 sec)

mysql> 



lixa=> select * from authors;
 id | last_name | first_name 
----+-----------+------------
(0 rows)

lixa=> 
```

the same program supports even *ROLLBACK*:

```
root@5bff2e527f3c:/# /myapp/hello-mysql-postgresql 192.168.123.35 0 1

2019-02-14 22:18:43.236523 [8/139820690380736] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
PostgreSQL, executing >INSERT INTO authors VALUES(1921, 'Rigoni Stern', 'Mario')<
MySQL, executing >INSERT INTO authors VALUES(1919, 'Levi', 'Primo')<
```

you can verify that there's no row in the table after *INSERT* and 
*ROLLBACK*!

Take a look to the source program in git: https://github.com/tiian/lixa-docker/blob/master/xta-c/hello-mysql-postgresql.c 

