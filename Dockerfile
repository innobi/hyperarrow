FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y -q && \
    apt-get install -y -q --no-install-recommends \
      build-essential \
      ca-certificates \      
      cmake \
      git \
      lldb \
      pkg-config \
      python3-pip \
      unzip \      
      wget && \
    apt-get clean && rm -rf /var/lib/apt/lists*

WORKDIR /build
RUN git clone https://github.com/apache/arrow.git

ARG tableau_version=tableauhyperapi-cxx-linux-x86_64-release-hyperapi_release_24.0.0.13394.r37498cf6
RUN wget --no-verbose https://downloads.tableau.com/tssoftware/$tableau_version.zip
RUN unzip $tableau_version.zip
RUN mv $tableau_version tableauhyperapi
RUN rm $tableau_version.zip

WORKDIR hyperarrow
COPY build_arrow.sh .
RUN chmod 755 ./build_arrow.sh
RUN ./build_arrow.sh

COPY CMakeLists.txt  build_example.sh ./
COPY src ./src/
RUN chmod 755 ./build_example.sh
RUN ./build_example.sh

COPY run.sh .
RUN chmod 755 ./run.sh
RUN ./run.sh
