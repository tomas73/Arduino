FROM ubuntu:22.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    curl \
    git \
    python3 \
    python3-pip \
    python3-serial \
    udev \
    wget \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Create vscode user
RUN useradd -m -s /bin/bash vscode && \
    echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Install Arduino CLI
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sh

# Switch to vscode user
USER vscode
WORKDIR /home/vscode

# Initialize Arduino CLI config
RUN arduino-cli config init

# Add ESP8266 board manager URL
RUN arduino-cli config add board_manager.additional_urls http://arduino.esp8266.com/stable/package_esp8266com_index.json

# Update board package index
RUN arduino-cli core update-index

# Install ESP8266 core (version 3.0.2 for stability)
RUN arduino-cli core install esp8266:esp8266@3.0.2

ENV DEBIAN_FRONTEND=dialog

WORKDIR /workspace
