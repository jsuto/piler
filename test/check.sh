#!/bin/bash

for i in ../testemails/raw/*; do
    ./test $i > aa;
    diff aa ../testemails/parsed/`basename $i`;
    if [ $? -eq 0 ]; then echo OK; else echo `basename $i`": ERROR"; fi
done

rm -f aa


