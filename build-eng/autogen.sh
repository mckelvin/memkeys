#!/bin/bash

# Record where I am
PROJ_DIR=$(dirname $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ));
BUILD_DIR=$PROJ_DIR/build;

mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake $PROJ_DIR
make
cd $PROJ_DIR
