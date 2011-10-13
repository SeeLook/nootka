#! /bin/bash

IFS="
"
XX=0
FF=1

for line in $(cat nootka/changelog)
do
#	echo $line
	if [ "$(echo $line|grep ^[0-9].* )" ]
	then
	    if [ $XX == 0 ]
	    then
		XX=1
	    else
		XX=0
		FF=0
	   fi	
	fi

	if [ $XX == 1 ] 
	then
	    if [ $FF == 1 ]
	    then
		echo $line
	   fi
	fi

done

