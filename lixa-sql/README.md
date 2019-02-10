# Docker images with SQL databases that are configured to run with LIXA/XTA

```
docker build -t lixa/lixa-postgres .
docker run -e POSTGRES_PASSWORD=lixa -p 5432:5432 -d lixa/lixa-postgres -c 'max_prepared_transactions=10'
docker exec -ti 367c756f6450 bash
```

