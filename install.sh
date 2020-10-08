#!/bin/bash

# Update package repositories
sudo apt update

# Install these packages using apt
sudo apt install -y \
    make \
    python3-pip \
    build-essential \
    qt5-default \
    clang-format \
    zip \
    python3-pip \
    x11-apps \
    chrony

# Remove old cmake package from apt
sudo apt purge -y cmake 

# Install newer cmake package from pip
sudo pip3 install --upgrade cmake