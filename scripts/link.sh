#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
DIR+=/..

components="cart cat configuration sensor staff uart_bridge ota"
programs="main/column_controller main/network_hub main/conductor utility/column_checker utility/color_calibration"

for program in $programs
do
    rm -rf $DIR/firmware/$program/src/components
    mkdir -p $DIR/firmware/$program/src/components
    for component in $components
    do
        ln -s $DIR/firmware/components/$component $DIR/firmware/$program/src/components/$component
    done
done