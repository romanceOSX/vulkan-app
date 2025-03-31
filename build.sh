#! /bin/sh

# Update app name from cmake to be consumed from this var
export APP_NAME=vulkan_app
VULKAN_SDK=~/VulkanSDK/1.4.309.0

# Set up environment variables
. $VULKAN_SDK/setup-env.sh

SRC_DIR=$(dirname "$(realpath "$0")")
BUILD_DIR=$SRC_DIR/build
TARGET=$BUILD_DIR/$APP_NAME

if ! [ -d "$BUILD_DIR" ]; then
    mkdir build
    cmake -B build
fi

cmake --build build && $TARGET

ln -sfn build/compile_commands.json

