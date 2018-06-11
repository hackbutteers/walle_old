FROM lijippy/dev:latest 
MAINTAINER your_name <walle_group@hotmail.com>
LABEL Description="This image is the base os dev image."  Version="1.0"
# reconfig timezone
RUN mkdir /root/walle

COPY cmake  /root/walle
COPY include  /root/walle
COPY package  /root/walle
COPY src  /root/walle
COPY test  /root/walle
COPY LICENSE  /root/walle
COPY README.md  /root/walle

WORKDIR /root/walle
RUN mkdir build
RUN cd build
RUN cmake ../
RUN make
