FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y -q && \
    apt-get install -y -q --no-install-recommends \
      build-essential \
      cmake \
      git \
      ca-certificates \
      pkg-config && \
    apt-get clean && rm -rf /var/lib/apt/lists*

WORKDIR /build
RUN git clone https://github.com/apache/arrow.git
WORKDIR hyperarrow
COPY . .
RUN ls
RUN chmod 755 ./run.sh
RUN chmod 755 ./build_arrow.sh
RUN chmod 755 ./build_example.sh
RUN ./run.sh
