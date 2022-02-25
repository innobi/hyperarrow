# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

ARG base
FROM ${base}

# Install basic dependencies
RUN yum install -y autoconf \
    boost-devel \
    cmake \
    flex \
    git \
    wget \
    zip

ARG python
ENV PYTHON_VERSION=${python}

ENV VIRTUAL_ENV=/venv
RUN $(find /opt/python -name cp${PYTHON_VERSION//.}-*)/bin/python -m venv /venv
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# Install arrow
ARG arrow=6.0.1
COPY ci/scripts/install_arrow.sh hyperarrow/ci/scripts/
RUN /hyperarrow/ci/scripts/install_arrow.sh ${arrow} /usr/local

# Get Hyper libraries. Here we also pull from the Python
# wheel in addition to the C++ source as the C++ libraries
# are pre-compiled on newer platforms than what manylinux
# allows. python wheel is needed to unpack Tableau's wheel
# to get their manylinux compiled lib
RUN python -m pip install wheel auditwheel

COPY ci/scripts/get_tableau_libs.sh hyperarrow/ci/scripts
RUN /hyperarrow/ci/scripts/get_tableau_libs.sh

COPY ci/scripts/get_tableau_python_libs.sh hyperarrow/ci/scripts
RUN /hyperarrow/ci/scripts/get_tableau_python_libs.sh


