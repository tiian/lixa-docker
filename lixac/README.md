# LIXA and XTA

LIXA, LIbre XA, is a free and open source implementation of the XA and TX
specifications; it supports even XTA.
XTA, XA Transaction API, is a new API that provides two phase commit feature
to applications; it has been designed to work in a *microservices*
environment.
LIXA and XTA documentation is available at http://www.tiian.org/lixa/

# lixac: LIXA client (command line utilities) 

This image contains a LIXA client that can be used to test a LIXA server.
This image is mainly intended as a test tool or an "hello world" tool for the
LIXA project.

The most useful tool available in this image is *lixat*: it can be used to
test a LIXA state server (see https://hub.docker.com/r/lixa/lixad)

## Usage

Execute the image and start a container with command:
```
docker run --rm -it lixa/lixac bash
```

check LIXA version and build options:
```
lixa@b2eea958f107:/$ lixau -v
LIXA: a LIbre XA implementation
Copyright (c) 2009-2019, Christian Ferrari; all rights reserved.
License: GPL (GNU Public License) version 2
Package name: lixa; package version: 1.7.4
Release date: 2019-02-09;
Driver for IBM DB2: no;
Driver for MySQL: no;
Driver for Oracle DBMS: no;
Driver for PostgreSQL: no;
Driver for IBM MQ: client:no, server=no;
Access http://www.tiian.org/lixa/ to report bugs and partecipate to the project
```

configure the client to test a running LIXA state server (you must start
a container from image lixa/lixad before):
```
lixa@7e1a75b166e2:/$ export LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default"
lixa@7e1a75b166e2:/$ lixat
2019-02-10 10:11:36.392130 [9/139698512349056] INFO: LXC000I this process is starting a new LIXA transaction manager (lixa package version is 1.7.4)
tx_open(): 0
tx_close(): 0
```

use *lixat --help* to discover all the options, refer to LIXA official site for
full documentation.

LIXA documentation is available at http://www.tiian.org/lixa/
