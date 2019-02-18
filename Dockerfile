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

# Set our shell to a bash full login environment to pull in
# RVM's profile in all RUN instructions below.
# (Requires Docker 1.12)
SHELL ["/bin/bash", "-l", "-c"] 

# In case we missed any package requirements, this installs them
RUN rvm requirements

# Pick your ruby version here
RUN rvm install ruby-2.3.3

# Setup our libruby.so dir in ld.so.conf
RUN rvm config-get libdir > /etc/ld.so.conf.d/ruby.conf && ldconfig

WORKDIR /usr/src/mod_ruby

COPY . /usr/src/mod_ruby

# Pulls in the RVM environment and installed ruby
RUN cmake . && make && make install

# Manually copy some files I couldn't figure out with the CMake system
RUN cp -a config/mod_ruby.conf /etc/httpd/conf.modules.d/

# librhtml.so
RUN cp -a lib/* $(rvm config-get libdir) && ldconfig

RUN rm -f /etc/httpd/conf.d/welcome.conf
