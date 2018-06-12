FROM lijippy/dev:latest 
MAINTAINER your_name <walle_group@hotmail.com>
LABEL Description="This image is the base os dev image."  Version="1.0"
# reconfig timezone
RUN mkdir /root/walle

COPY cmake  /root/walle/cmake
COPY include  /root/walle/include
COPY package  /root/walle/package
COPY src  /root/walle/src
COPY test  /root/walle/test
COPY LICENSE  /root/walle
COPY README.md  /root/walle
COPY CMakeLists.txt  /root/walle

WORKDIR /root/walle
RUN mkdir build
WORKDIR /root/walle/build
RUN cmake ../
RUN make
