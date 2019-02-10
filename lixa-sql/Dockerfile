# A customized PosqlgreSQL image for LIXA
#
# Build with the following command:
#       docker build -t lixa/lixa-postgres .
FROM 	postgres
# Initialization script
COPY	init-user-db.sh /docker-entrypoint-initdb.d/
