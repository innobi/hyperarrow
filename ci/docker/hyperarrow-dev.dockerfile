FROM debian

RUN apt update
RUN apt-get install -y \
    build-essential \
    clang-format \
    libboost-system-dev \
    cmake \
    git

# https://arrow.apache.org/install/
RUN apt install -y -V ca-certificates lsb-release wget
RUN wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
RUN apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
RUN apt update
RUN apt install -y -V libarrow-dev # For C++

RUN apt-get install -y zip
COPY ci/scripts/get_tableau_libs.sh /hyperarrow/ci/scripts/
RUN /hyperarrow/ci/scripts/get_tableau_libs.sh
ENV TABLEAU_CMAKE_PATH=/tmp/tableau/tableauhyperapi/share/cmake

WORKDIR /hyperarrow