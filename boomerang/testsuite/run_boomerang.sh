#!/bin/bash
BOOMERANG_DIR=$(dirname $0)/..
cd $BOOMERANG_DIR
exec -a boomerang ./boomerang "$@"
