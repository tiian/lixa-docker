# LIXA and XTA

LIXA, LIbre XA, is a free and open source implementation of the XA and TX
specifications; it supports even XTA.
XTA, XA Transaction API, is a new API that provides two phase commit feature
to applications; it has been designed to work in a *microservices*
environment.
LIXA and XTA documentation is available at http://www.tiian.org/lixa/

# lixad: LIXA state server

This image contains a LIXA state server (lixad) that can be used as service in
a TXaaS (Transaction as a Service) environment.

## Usage

The simplest usage is starting it and checking it's running with these commands:
```
docker run --rm -p 2345:2345 -d lixa/lixad

docker ps
CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS                    NAMES
66b011bb4f52        lixa/lixad          "/home/lixa/lixad-en…"   4 seconds ago       Up 2 seconds        0.0.0.0:2345->2345/tcp   lucid_wright

docker logs 66b011bb4f52

2019-02-03 21:36:33.021152 [1/140199619743040] NOTICE: LXD000N this process is starting a new LIXA server (lixa package version is 1.7.3)
2019-02-03 21:36:33.021577 [1/140199619743040] INFO: LXD027I parameter 'pid_file' set to value '/opt/lixa/var/run.pid'
2019-02-03 21:36:33.021588 [1/140199619743040] INFO: LXD026I parameter 'min_elapsed_sync_time' set to value 0
2019-02-03 21:36:33.021591 [1/140199619743040] INFO: LXD026I parameter 'max_elapsed_sync_time' set to value 0

```

It can be used by every compatible LIXA client.

LIXA documentation is available at http://www.tiian.org/lixa/
