@ECHO OFF

set components=cart cat configuration sensor staff uart_bridge
set programs=column_controller network_hub conductor

for %%G in (%programs%) do (
    for %%H in (%components%) do (
        rmdir firmware\main\%%G\src\components\%%H /s /q
        mklink /J firmware\main\%%G\src\components\%%H firmware\components\%%H
    )
)