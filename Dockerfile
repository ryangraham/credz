FROM ubuntu:18.04
LABEL maintainer="ryangraham@gmail.com"

ENV DEBIAN_FRONTEND noninteractive

ENV BUILD_TOOLS \
    build-essential \
    autoconf \
    g++ \
    gcc \
    make \
    cmake

ENV AWS_SDK_CPP_REQUIRED_LIBS \
    libcurl4-openssl-dev \
    libssl-dev \
    uuid-dev \
    zlib1g-dev \
    libpulse-dev

ENV BOOST \
    libboost-all-dev

ENV TOOLS \
    ca-certificates \
    curl \
    git \
    sudo \
    unzip

ENV AWS_SDK_CPP_VERSION 1.7.334

RUN apt-get update \
    && apt-get install -y \
    $BUILD_TOOLS \
    $BOOST \
    $AWS_SDK_CPP_REQUIRED_LIBS \
    $TOOLS \
    --no-install-recommends

RUN mkdir -p /tmp/sdk/build && cd /tmp/sdk \
    && curl -sSL https://github.com/aws/aws-sdk-cpp/archive/${AWS_SDK_CPP_VERSION}.zip > aws-sdk-cpp-${AWS_SDK_CPP_VERSION}.zip \
    && unzip aws-sdk-cpp-${AWS_SDK_CPP_VERSION}.zip \
    && rm -f aws-sdk-cpp-${AWS_SDK_CPP_VERSION}.zip

RUN cd /tmp/sdk/build \
    && cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_TESTING=OFF \
    -DAUTORUN_UNIT_TESTS=OFF \
    -DBUILD_ONLY="sts" \
    ../aws-sdk-cpp-${AWS_SDK_CPP_VERSION} \
    && make \
    && make install

RUN cd /tmp \
    && git clone --depth=1 https://github.com/nlohmann/json.git \
    && mkdir json/build \
    && cd json/build \
    && cmake ../. -DBUILD_TESTING=OFF \
    && make install
