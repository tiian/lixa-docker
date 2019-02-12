# Docker images with SQL databases that are configured to run with LIXA/XTA

Build and run PostgreSQL customized image

```
docker build -t lixa/lixa-postgres .
docker run -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/lixa-postgres -c 'max_prepared_transactions=10'
```

Build and run MySQL customized image

```
docker build -f Dockerfile-mysql -t lixa/mysql .
docker run -e MYSQL_ROOT_PASSWORD=mysecretpw -p 3306:3306 -d lixa/mysql
```

