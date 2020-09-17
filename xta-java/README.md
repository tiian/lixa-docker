# LIXA and XTA

LIXA, LIbre XA, is a free and open source implementation of the XA and TX
specifications; it supports even XTA.
XTA, XA Transaction API, is a new API that provides two phase commit feature
to applications; it has been designed to work in a *microservices* 
environment.
LIXA and XTA documentation is available at http://www.tiian.org/lixa/

# xta-java: XA Transaction API for the Java language

These images contain the libraries and the modules that are necessary to
develop a Java application with the support of XTA. Two drivers are provided:
MySQL and PostgreSQL.
Three different images are provided:

* xta-jdk: JDK as in OpenJDK official image + XTA + MySQL and PostgreSQL JDBC drivers
* xta-jre: JRE as in OpenJDK official image + XTA + MySQL and PostgreSQL JDBC drivers
+ xta-jdk-maven: Maven and JDK as in Mave official image + XTA + MySQL and PostgreSQL JDBC drivers

## How to use these images

The basic usage of images xta-jdk and xta-jre is as a starting point to create
derived images with an application inside. Below there are some examples that
can be implemented using the base images. Multistage build is used to build
Java programs with JDK, but execute them inside a JRE only container.

## An example based on xta-jdk, xta-jre Docker images

First of all, clone *lixa-docker* repository locally:

```
git clone https://github.com/tiian/lixa-docker.git
cd lixa-docker/xta-java/
```

it contains the source files and the Dockerfiles that are necessary to build
an example that use xta-jdk and xta-jre images.

### XTA with MySQL & PostgreSQL

This example shows as you can perform transactions with MySQL and PostgreSQL
databases.
First of all, you should execute a MySQL and a PostgreSQL instance: there's a 
couple ready to be used for this example:

```
docker run -e MYSQL_ROOT_PASSWORD=mysecretpw -p 3306:3306 -d lixa/mysql
docker run -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/postgres -c 'max_prepared_transactions=10'
```

Before starting your program, you need a LIXA state server up and running; you may use a Docker image even for it:

```
docker run --rm -p 2345:2345 -d lixa/lixad
```

At this point you can build the *Hello MySQL PostgreSQL* image for Java: 

```
docker build -f Dockerfile-java-hello-mysql-postgresql -t lixa/xta-java-hello-mysql-postgresql .
```

Retrieve the IP address of the host where *lixad*, *mysqld*  and *postgres* are
running, it must be used to reach them from another container. Supposing the IP
address is *192.168.123.35*, start the *XTA Java Hello MySQL PostgreSQL*
container and execute the *Hello MySQL PostgreSQL* program located in */myapp*
directory:

```
docker run --rm -it lixa/xta-java-hello-mysql-postgresql bash

root@077e2a0ff249:/# export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"

root@077e2a0ff249:/# cd /myapp

root@077e2a0ff249:/myapp# java -Djava.library.path=/opt/lixa/lib -cp /opt/java/xta.jar:/opt/java/mariadb-java-client.jar:/opt/java/postgresql.jar:. HelloMysqlPostgresql 192.168.123.35 1 1

2019-02-23 22:38:02.571991 [6/140350447437568] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.5)
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
root@077e2a0ff249:/myapp# java -Djava.library.path=/opt/lixa/lib -cp /opt/java/xta.jar:/opt/java/mysql.jar:/opt/java/postgresql.jar:. HelloMysqlPostgresql 192.168.123.35 1 0

2019-02-23 22:39:03.795428 [27/140355107317504] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.5)
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
root@077e2a0ff249:/myapp# java -Djava.library.path=/opt/lixa/lib -cp /opt/java/xta.jar:/opt/java/mysql.jar:/opt/java/postgresql.jar:. HelloMysqlPostgresql 192.168.123.35 0 1

2019-02-23 22:39:32.079391 [48/140264988256000] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.5)
PostgreSQL, executing >INSERT INTO authors VALUES(1921, 'Rigoni Stern', 'Mario')<
MySQL, executing >INSERT INTO authors VALUES(1919, 'Levi', 'Primo')<
```

you can verify that there's no row in the table after *INSERT* and 
*ROLLBACK*!

Take a look to the source program in git: https://github.com/tiian/lixa-docker/blob/master/xta-java/HelloMysqlPostgresql.java

