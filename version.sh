#!/bin/bash

COUNT=0

while read line 
do
	COUNT=$(expr $COUNT + 1)
done <<< "$(git log --oneline)"

MAJOR=$(expr $COUNT / 100)
AMOUNT=$(expr $MAJOR \* 100)
COUNT=$(expr $COUNT - $AMOUNT)
MINOR=$(expr $COUNT / 10)
AMOUNT=$(expr $MINOR \* 10)
COUNT=$(expr $COUNT - $AMOUNT)
REVISION=$(git log --pretty="%h" -1)
echo "$MAJOR.$MINOR.$COUNT-$REVISION"

