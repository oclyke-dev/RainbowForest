#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
DIR+=/..

components="cart cat configuration sensor staff uart_bridge"
programs="column_controller network_hub conductor"

for program in $programs
do
    rm -rf $DIR/firmware/main/$program/src/components
    mkdir -p $DIR/firmware/main/$program/src/components
    for component in $components
    do
        ln -s $DIR/firmware/components/$component $DIR/firmware/main/$program/src/components/$component
    done
done