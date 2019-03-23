# Transactional client/server using REST

This example shows a distributed transaction between a Python3 client that
uses MySQL to persist its data and a Java server that uses PostgreSQL to 
persist its data.

## Start 3 ancillary containers: lixad, mysql and postgresql

```
$ docker run --rm -e MYSQL_ROOT_PASSWORD=mysecretpw -p 3306:3306 -d lixa/mysql

$ docker run --rm -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/postgres -c 'max_prepared_transactions=10'

$ docker run --rm -p 2345:2345 -d lixa/lixad
```

The images "lixa/lixad", "lixa/mysql", "lixa/postgres" are publicly available
in DockerHub and no build is necessary.

## Build the images that are necessary to execute the example

Build the Python3 REST client:
```
$ docker build -f Dockerfile-client -t rest-client .
```

Build the Java/Jersey REST server:
```
$ docker build -f Dockerfile-server -t rest-server .
```

## Run it

Start the server:
```
$ docker run -ti --rm -e MAVEN_OPTS="-Djava.library.path=/opt/lixa/lib" -e LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default" -e PQSERVER="192.168.123.35" -p 18080:8080 rest-server
```
replace "192.168.123.35" with the IP address of your Docker host.

Start che client:
```
$ docker run -ti --rm -e SERVER="192.168.123.35" -e LIXA_STATE_SERVERS="tcp://192.168.123.35:2345/default" rest-client
```
replace "192.168.123.35" with the IP address of your Docker host.

## Credits

The Java REST server is a derivative work of the example explained by Yuri 
Subach in its [post](https://medium.com/dekses/how-to-dockerize-java-restful-api-application-7aed70ef3c3a) and in [GitHub](https://github.com/dekses/jersey-docker-demo).
