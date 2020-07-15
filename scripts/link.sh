#!/bin/bash

components="cart cat configuration sensor staff uart_bridge"
programs="column_controller network_hub conductor"

for program in $programs
do
    for component in $components
    do
        rm -rf firmware/main/$program/src/components/$component
        ln -s firmware/components/$component firmware/main/$program/src/components
    done
done