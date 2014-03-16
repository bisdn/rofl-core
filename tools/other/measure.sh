#!/bin/bash

FILE=output_measurements
ITER=18

#Cleanup
echo "" > $FILE

#Measure
i=0
while [[ $i -le $ITER ]]; do
    make check 2>> $FILE
    i=$((i+=1))
done

#Analyze
#MATCH first
grep cycles $FILE | grep -v NO | sed s/.*://
echo ""
grep cycles $FILE | grep NO | sed s/.*://
