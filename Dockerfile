FROM centos:centos7

RUN yum -y upgrade && yum install -y \
  apr-devel \
  apr-util \
  apr-util-devel \
  autoconf \
  automake \
  bison \
  bzip2 \
  cmake \
  curl \
  cyrus-sasl \
  cyrus-sasl-devel \
  flex \
  flex-devel \
  gcc \
  gcc-c++ \
  git \
  gpg \
  httpd \
  httpd-devel \
  libffi-devel \
  libtool \
  libyaml \
  openssl-devel \
  patch \
  readline-devel \
  sqlite-devel \
  make \
  redhat-lsb \
  unzip \
  zlib-devel 

# Import GPG key for RVM
RUN gpg \
  --keyserver hkp://keys.gnupg.net \
  --recv-keys \
    409B6B1796C275462A1703113804BB82D39DC0E3 \
    7D2BAF1CF37B13E2069D6956105BD0E739499BDB

# Install RVM system wide
RUN curl -sSL https://get.rvm.io | bash -s stable

# In case we missed any package requirements, this installs them
RUN /bin/bash -l -c "rvm requirements"

# Pick your ruby version here
RUN /bin/bash -l -c "rvm install ruby-2.3.3"

WORKDIR /usr/src/mod_ruby

COPY . /usr/src/mod_ruby

# Pulls in the RVM environment and installed ruby
RUN /bin/bash -l -c "cmake . && make && make install"
