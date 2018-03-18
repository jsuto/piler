#!/bin/bash

find /var/piler/sphinx/ -type f -name main\*.spd -printf "%TY%Tm%Td %s\\n" | sort -r | head -1 | cut -f2 -d ' ' > /var/piler/stat/main_index_size
