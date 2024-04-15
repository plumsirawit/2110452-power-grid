#!/bin/bash

CWD=$(pwd)
echo $CWD
rm -f run.txt log.txt
touch run.txt log.txt
for filename in ./data/input/*; do
    FNAME=$(basename $filename)
    echo "Running $FNAME"
    OUT=`(time (docker run -v $CWD/data/input:/input -v $CWD/data/output:/output power-grid /input/$FNAME /output/$FNAME 2>&1 >> log.txt) 2>&1 >> log.txt) 2>&1 | grep real`
    OUT2=$(echo $OUT | awk '{print $2}')
    echo "$FNAME, $OUT2" >> run.txt
done