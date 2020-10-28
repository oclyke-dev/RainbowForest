@ECHO OFF

set components=cart cat configuration sensor staff uart_bridge column isl29125 cmt
set programs=main\column_controller main\network_hub main\conductor main\server utility\column_checker utility\color_calibration

for %%G in (%programs%) do (
    rmdir firmware\%%G\src\components /s /q
    mkdir firmware\%%G\src\components
    for %%H in (%components%) do (
        mklink /J firmware\%%G\src\components\%%H firmware\components\%%H
    )
)