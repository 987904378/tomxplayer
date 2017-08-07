#!/bin/bash

COUNT=0

for line in $(git log)
do
	COUNT=$(expr $COUNT + 1)
done

MAJOR=$(expr $COUNT / 10000)
AMOUNT=$(expr $MAJOR \* 10000)
COUNT=$(expr $COUNT - $AMOUNT)
MINOR=$(expr $COUNT / 1000)
AMOUNT=$(expr $MINOR \* 1000)
COUNT=$(expr $COUNT - $AMOUNT)
REVISION=$(expr $COUNT / 100)
AMOUNT=$(expr $MINOR \* 100)
COUNT=$(expr $COUNT - $AMOUNT)
echo "$MAJOR.$MINOR.$REVISION.$COUNT"

