FROM ubuntu:latest
LABEL authors="podlaszczyk"
ARG LLVM_TOOLS_VERSION=17
ARG QT_VERSION=6.5.3
ARG CMAKE_VERSION=3.27.9

ENV DEBIAN_FRONTEND=noninteractive \
    DEBCONF_NONINTERACTIVE_SEEN=true

RUN apt update && apt upgrade -y
RUN apt update && apt install -y build-essential perl git wget libssl-dev ninja-build \
    libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev \
    libxrender-dev libxcb1-dev libxcb-cursor-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev \
    libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev \
    libxcb-render-util0-dev libxcb-util-dev libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-dev \
    libxkbcommon-x11-dev libxkbcommon0 \
    libgl1-mesa-glx mesa-common-dev libglx-dev libgl1-mesa-dev \
    libatspi2.0-dev

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz && \
    tar -zxvf cmake-${CMAKE_VERSION}.tar.gz && \
    cd cmake-${CMAKE_VERSION} && \
    ./bootstrap --prefix=/usr/local/ && \
    make -j 16 && \
    make install && \
    rm -rf cmake-${CMAKE_VERSION}.tar.gz cmake-${CMAKE_VERSION}

# Installing required packages
RUN apt install software-properties-common -y \
    && add-apt-repository ppa:deadsnakes/ppa -y \
    && apt update && apt install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        git git-lfs ssh \
        libgl1-mesa-glx mesa-common-dev libglx-dev libgl1-mesa-dev \
        libpcre2-16-0 \
        ninja-build \
        python3.12 python3-pip \
        unzip zip \
        wget \
        gcovr lcov \
        libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev \
        libxrender-dev libxcb1-dev libxcb-cursor-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev \
        libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev \
        libxcb-render-util0-dev libxcb-util-dev libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-dev \
        libxkbcommon-x11-dev libxkbcommon0 \
        libjpeg-turbo8 patchelf \
    && rm -rf /var/lib/apt/lists/*

# Adding given LLVM version repository to apt
RUN wget https://apt.llvm.org/llvm.sh  \
    && chmod +x llvm.sh \
    && ./llvm.sh ${LLVM_TOOLS_VERSION} \
    && apt update && apt install -y clang-tidy-${LLVM_TOOLS_VERSION} clang-format-${LLVM_TOOLS_VERSION} \
    && rm llvm.sh \
    && ln -s /usr/bin/clang-format-17 /usr/bin/clang-format \
    && ln -s /usr/bin/clang-tidy-17 /usr/bin/clang-tidy


RUN pip3 install conan==1.63.0 \
    && conan profile new default --detect \
    && conan profile update settings.compiler.libcxx=libstdc++11 default




RUN git clone --branch=v${QT_VERSION} git://code.qt.io/qt/qt5.git qt-sources
RUN cd qt-sources && ./init-repository --module-subset=qtbase,qthttpserver,qtserialport
RUN mkdir -p qt-sources/build \
    && cd qt-sources/build \
    && ../configure -opensource -confirm-license -widgets -submodules qthttpserver,qtserialport -gui -nomake examples -nomake tests -prefix /usr/local/Qt/${QT_VERSION}/gcc_64 \
    && cmake --build . \
    && cmake --install .

RUN apt install -y socat curl