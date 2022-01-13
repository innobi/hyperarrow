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

FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Install Visual Studio Build Tools
RUN curl -SL --output vs_buildtools.exe https://aka.ms/vs/17/release/vs_buildtools.exe ^
RUN start /w vs_buildtools.exe --quiet --wait --norestart --nocache modify \
        --installPath "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools" \
        --add Microsoft.VisualStudio.Workload.AzureBuildTools \
        --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
        --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
        --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
        --remove Microsoft.VisualStudio.Component.Windows81SDK
RUN del /q vs_buildtools.exe


# Install Chocolatey
RUN powershell -command Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install CMake and Git
ARG cmake=3.21.4
RUN choco install --no-progress -r -y cmake --version=%cmake% --installargs 'ADD_CMAKE_TO_PATH=System' && \
    choco install --no-progress -r -y git

# Add unix tools to path
RUN setx path "%path%;C:\Program Files\Git\usr\bin"

# Remove previous installations of python from the base image
# NOTE: a more recent base image (tried with 2.12.1) comes with python 3.9.7
# and the msi installers are failing to remove pip and tcl/tk "products" making
# the subsequent choco python installation step failing for installing python
# version 3.9.* due to existing python version
RUN wmic product where "name like 'python%%'" call uninstall /nointeractive && \
    rm -rf Python*

ARG python=3.8
RUN (if "%python%"=="3.7" setx PYTHON_VERSION 3.7.12) & \
    (if "%python%"=="3.8" setx PYTHON_VERSION 3.8.10) & \
    (if "%python%"=="3.9" setx PYTHON_VERSION 3.9.9) & \
    (if "%python%"=="3.10" setx PYTHON_VERSION 3.10.1)
RUN choco install -r -y --no-progress python --version=%PYTHON_VERSION%
RUN pip install -U pip

# Install arrow via vcpkg
COPY ci\\scripts\\install_arrow.bat C:\\hyperarrow\\ci\\scripts\\
RUN C:\\hyperarrow\\ci\\scripts\\install_arrow.bat

# Get Hyper Libraries
COPY ci\\scripts\\get_tableau_libs.bat C:\\hyperarrow\\ci\\scripts\\
RUN C:\\hyperarrow\\ci\\scripts\\get_tableau_libs.bat