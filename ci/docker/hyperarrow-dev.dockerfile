FROM debian

RUN apt update
# python and wget are for the arrow install
# we should decouple that dependency in the future
RUN apt-get install -y \
    build-essential \
    clang-format \
    cmake \
    libboost-system-dev \
    gdb \
    git \
    python3-pip \
    wget

ARG arrow=6.0.1
ARG buildType='Debug'
COPY ci/scripts/install_arrow.sh /hyperarrow/ci/scripts/
RUN /hyperarrow/ci/scripts/install_arrow.sh ${arrow} ${buildType} /usr/local

RUN apt-get install -y zip
COPY ci/scripts/get_tableau_libs.sh /hyperarrow/ci/scripts/
RUN /hyperarrow/ci/scripts/get_tableau_libs.sh
ENV TABLEAU_CMAKE_PATH=/tmp/tableau/tableauhyperapi/share/cmake

WORKDIR /hyperarrow