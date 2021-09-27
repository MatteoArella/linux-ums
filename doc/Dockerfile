#
# Dockerfile with dependencies for documentation builds.
#

FROM debian:latest

LABEL maintainer="Arella Matteo <arella.matteo.95@gmail.com>"

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    doxygen \
    latexmk \
    lmodern \
    fonts-freefont-otf \
    texlive-latex-recommended \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    python3 \
    python3-pip \
    autotools-dev \
    autoconf \
    automake \
    libtool \
    pkg-config

COPY requirements.txt /tmp/

RUN pip3 install -r /tmp/requirements.txt

RUN [ "/bin/bash" ]
