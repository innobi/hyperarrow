FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y -q && \
    apt-get install -y -q --no-install-recommends \
      build-essential \
      cmake \
      git \
      wget \
      unzip \
      ca-certificates \
      pkg-config && \
    apt-get clean && rm -rf /var/lib/apt/lists*

WORKDIR /build
RUN git clone https://github.com/apache/arrow.git

RUN wget --no-verbose https://downloads.tableau.com/tssoftware/tableauhyperapi-cxx-linux-x86_64-release-hyperapi_release_24.0.0.13394.r37498cf6.zip
RUN unzip tableauhyperapi-cxx-linux-x86_64-release-hyperapi_release_24.0.0.13394.r37498cf6.zip

WORKDIR hyperarrow
COPY . .
RUN ls
RUN chmod 755 ./run.sh
RUN chmod 755 ./build_arrow.sh
RUN chmod 755 ./build_example.sh
RUN ./build_arrow.sh
RUN ./build_example.sh
RUN ./run.sh
