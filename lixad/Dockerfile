# A Docker image for LIXA state server based on Debian Linux
# Last test date: 2020-09-19
#
FROM	debian:stretch-slim
#
# Build without parameters to build latest 
#	docker build -t lixa/lixad .
# Build with 2 parameters to build a specific release
#	docker build --build-arg VERSION_CODE=1.9.2 \
#		--build-arg VERSION_TYPE=testing -t lixa/lixad:1.9.2 .
#   ** Note: version code must be of the right type and must exists in
#            Sourceforge.net
# Execute with
#	docker run --rm -p 2345:2345 -d lixa/lixad
#
# Update Ubuntu Software repository
ARG 	DEBIAN_FRONTEND=noninteractive
# Install generic pre-requisites
RUN	apt-get update && apt-get install -y --no-install-recommends \
	apt-utils \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& rm -rf /var/log/* 
# Install run-time pre-requisites
RUN	apt-get update && apt-get install -y --no-install-recommends \
	libxml2 libglib2.0 uuid-runtime \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& rm -rf /var/log/* 
# LIXA official package
# VERSION_CODE must be in format V.R.M and it must be available on Sourceforge
# VERSION_TYPE must be "stable" or "testing"
# elsewhere, latest will be used
ARG	VERSION_CODE
ARG	VERSION_TYPE
ENV	VERSION_CODE $VERSION_CODE
ENV	VERSION_TYPE $VERSION_TYPE
ENV	LIXA_VERSION $VERSION
ENV	LIXA_DOWNLOAD_DEF_URL https://sourceforge.net/projects/lixa/files/latest/download
ENV	LIXA_DOWNLOAD_VER_URL https://sourceforge.net/projects/lixa/files/lixa/$VERSION_TYPE/lixa-$VERSION_CODE.tar.gz/download
# build LIXA
RUN	BUILD_DEPS='ca-certificates wget make gcc pkg-config libxml2-dev libglib2.0-dev uuid-dev' \
	&& set -x \
	&& apt-get update && apt-get install -y --no-install-recommends \
		$BUILD_DEPS \
	&& mkdir -p /lixa \
	&& if [ "z$VERSION_CODE" != "z" -a "z$VERSION_TYPE" != "z" ]; \
		then wget -O lixa.tar.gz "$LIXA_DOWNLOAD_VER_URL"; \
		else wget -O lixa.tar.gz "$LIXA_DOWNLOAD_DEF_URL"; \
	   fi \
	&& tar -xzf lixa.tar.gz -C /lixa \
	&& rm lixa.tar.gz \
	&& cd /lixa/lixa-* \
	&& ./configure --disable-client --disable-syslog \
	&& make && make install \
	&& rm -rf /opt/lixa/share /opt/lixa/bin /opt/lixa/include \
	&& rm -r /lixa \
	&& apt-get purge -y --auto-remove $BUILD_DEPS \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& rm -rf /var/log/* \
	&& mv -v /opt/lixa/etc/lixad1_conf.xml /opt/lixa/etc/lixad_conf.xml \
	&& rm /opt/lixa/etc/lixac_conf.xml \
	&& rm /opt/lixa/lib/liblixab.*a
# security hardening: LIXA state server will run using non root privileges
RUN addgroup --system lixa \
	&& adduser --system --ingroup lixa --shell /bin/bash lixa \
	&& chown -R lixa:lixa /opt/lixa/etc/ /opt/lixa/var/
# create an entry point
COPY 	lixad-entrypoint.sh /home/lixa/
RUN	chmod 540 /home/lixa/lixad-entrypoint.sh \
	&& chown lixa:lixa /home/lixa/lixad-entrypoint.sh
# publish port 2345
EXPOSE 2345
# basic trace messages: only MOD_SERVER and MOD_SERVER_LISTENER; see file
# src/common/lixa_trace.h for more available values
#ENV LIXA_TRACE_MASK 0x5
# start LIXA state server
USER	lixa
ENTRYPOINT ["/home/lixa/lixad-entrypoint.sh"]
CMD 	["lixad"]

