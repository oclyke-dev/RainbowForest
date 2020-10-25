#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
DIR+=/..

components="cart cat configuration sensor staff uart_bridge column isl29125 cmt"
programs="main/column_controller main/network_hub main/conductor utility/column_checker utility/color_calibration utility/hub_checker/hub_checher_hub utility/hub_checker/hub_checher_column prototyping/network_hub_i2c prototyping/column_controller_i2c prototyping/column_map_testing"

for program in $programs
do
    rm -rf $DIR/firmware/$program/src/components
    mkdir -p $DIR/firmware/$program/src/components
    for component in $components
    do
        ln -s $DIR/firmware/components/$component $DIR/firmware/$program/src/components/$component
    done
done