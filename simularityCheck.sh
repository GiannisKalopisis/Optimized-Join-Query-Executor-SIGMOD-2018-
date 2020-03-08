#!/bin/bash

file1=$1
rowId1=$2
let colJoin1=$3-1
file2=$4
rowId2=$5
let colJoin2=$6-1

payloadList1=$(cut -d$'\t' -f${rowId1} < ${file1})

for payload in $payloadList1; do
	if [ $colJoin1 -eq 0 ]
	then
		payload1=$payload
		break
	fi
	let colJoin1=$colJoin1-1
done


payloadList2=$(cut -d$'\t' -f${rowId2} < ${file2})

for payload in $payloadList2; do
        if [ $colJoin2 -eq 0 ]
        then
                payload2=$payload
                break
        fi
        let colJoin2=$colJoin2-1
done

if [ $payload1 -eq $payload2 ]
then
	echo "Equal payloads"
else
	echo "Not equal payloads"
fi 

