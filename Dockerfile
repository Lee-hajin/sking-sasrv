FROM centos:8

RUN yum update -y
RUN yum install -y gcc gcc-c++ glibc-devel make cmake
RUN yum install -y zip unzip zlib zlib-devel readline-devel
# RUN yum install -y glibc-devel.i686 zlib-devel.i686

WORKDIR /opt
RUN mkdir -p /opt/packages
COPY packages ./packages

WORKDIR /opt/packages
RUN yum localinstall -y MySQL-*
RUN yum localinstall -y libnsl2-devel-1.2.0-2.20180605git4a062cf.el8.x86_64.rpm
RUN tar -zxf lua-5.1.5.tar.gz -C /opt

WORKDIR /opt/lua-5.1.5
RUN make linux && make install

WORKDIR /opt
COPY gmsv ./gmsv
COPY saac ./saac

WORKDIR /opt/gmsv
RUN cmake .
RUN make

WORKDIR /opt/saac
RUN cmake .
RUN make

WORKDIR /opt