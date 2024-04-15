#!/bin/bash

CWD=$(pwd)
echo $CWD
rm -f run.txt
touch run.txt
for filename in ./data/input/*; do
    FNAME=$(basename $filename)
    echo $FNAME
    OUT=`(time docker run -v $CWD/data/input:/input -v $CWD/data/output:/output power-grid /input/$FNAME /output/$FNAME) 2>&1 | grep real`
    OUT2=$(echo $OUT | awk '{print $2}')
    echo "$filename : $OUT2" >> run.txt
done