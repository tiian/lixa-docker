# LIXA and XTA

LIXA, LIbre XA, is a free and open source implementation of the XA and TX
specifications; it supports even XTA.
XTA, XA Transaction API, is a new API that provides two phase commit feature
to applications; it has been designed to work in a *microservices*
environment.
LIXA and XTA documentation is available at http://www.tiian.org/lixa/

# lixa-docker
Official Image packaging for LIXA www.tiian.org/lixa

Available images:

* lixad: LIXA state server
* lixac: LIXA client (command line utilities)
* xta-c: XTA (XA Transaction API) for the C language
* xta-cpp: XTA for the C++ language
* xta-java: XTA for the Java language
* xta-python: XTA for the Python language (both Python2 and Python3 are supported)
* lixa-sql: MySQL and PostgreSQL customized images for the sake of these
            examples
