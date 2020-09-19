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

Once lixad is started, it can be called by every compatible LIXA client: some
sample images like lixa/xta-c, lixa/xta-cpp, lixa/xta-python2,
lixa/xta-python3, lixa/xta-jdk and so on, are available at https://hub.docker.com/u/lixa

## How to manage persistency

LIXA state server (lixad) persists transaction states on disk: using Docker, 
it's suggested to use an external volume instead of an ephemeral one:

```
docker run --rm --mount source=lixad_state,target=/opt/lixa/var -p 2345:2345 -d lixa/lixad

docker volume ls | grep lixad
local               lixad_state
```

The state files can be listed with:
```
sudo ls -la /var/lib/docker/volumes/lixad_state/_data
total 64
drwxr-xr-x 2 syslog systemd-network 4096 Sep 19 21:01 .
drwxr-xr-x 3 root   root            4096 Sep 19 21:01 ..
-rw-r--r-- 1 syslog systemd-network  178 Sep 18 22:43 README
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status1_1
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status1_2
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status2_1
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status2_2
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status3_1
-rw-r----- 1 syslog systemd-network 5280 Sep 19 21:01 lixad_status3_2
-rw-r--r-- 1 syslog systemd-network    2 Sep 19 21:01 run.pid
```

## How to set environment variables

LIXA uses some environment variables for configuration. If, for example, you
want to use the *journal state engine* instead of the *traditional state engine*, you can pass **LIXA_STATE_ENGINE=JOURNAL** on the command line:

```
docker run --rm --env LIXA_STATE_ENGINE=JOURNAL --mount source=lixad_state,target=/opt/lixa/var -p 2345:2345 -d lixa/lixad

sudo ls -la /var/lib/docker/volumes/lixad_state/_data
total 40
drwxr-xr-x 2 syslog systemd-network   4096 Sep 19 21:13 .
drwxr-xr-x 3 root   root              4096 Sep 19 21:13 ..
-rw-r--r-- 1 syslog systemd-network    178 Sep 18 22:43 README
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status1_0.table
-rw-r----- 1 syslog systemd-network 204800 Sep 19 21:13 lixad_status1_1.log
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status1_1.table
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status2_0.table
-rw-r----- 1 syslog systemd-network 204800 Sep 19 21:13 lixad_status2_1.log
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status2_1.table
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status3_0.table
-rw-r----- 1 syslog systemd-network 204800 Sep 19 21:13 lixad_status3_1.log
-rw-r----- 1 syslog systemd-network   1024 Sep 19 21:13 lixad_status3_1.table
-rw-r--r-- 1 syslog systemd-network      2 Sep 19 21:13 run.pid
```

