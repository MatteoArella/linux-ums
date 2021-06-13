#
# Dockerfile with buildroot dependencies for containerized builds.
#

FROM debian:latest

LABEL maintainer="Arella Matteo <arella.matteo.95@gmail.com>"

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    locales \
    file \
    sed \
    make \
    binutils \
    build-essential \
    gcc \
    g++ \
    gzip \
    bzip2 \
    perl \
    tar \
    wget \
    cpio \
    unzip \
    rsync \
    bc \
    git \
    python3

RUN useradd -ms /bin/bash buildroot && \
    chown -R buildroot:buildroot /home/buildroot

USER buildroot

WORKDIR /home/buildroot

RUN locale-gen en_US.utf8

RUN [ "/bin/bash" ]
