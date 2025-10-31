#!/bin/bash 
platformio run -t clean 
platformio run -e featherm4
platformio run -e featherm4 --target compiledb
