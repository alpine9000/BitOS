#!/bin/bash

mkdir executables &> /dev/null
#mkdir bad &> /dev/null
rm -f executables/* &> /dev/null
rm -f tests.zip &> /dev/null

for file in *.c; do
    if [ -f $file ]; then
	if grep -q "dg-do run" $file; then	    
	    OPTIONS=`grep -o "{ dg-options .* }" $file | cut -d "\"" -f 2`
	    CMD="/usr/local/sh-elf/bin/sh-elf-gcc -w -m2e -pie $OPTIONS $file -o executables/${file%.*}.test"
	    echo $CMD
	    if ! $CMD; then
	#	mv $file bad
		echo "BAD"
            fi
	else
	    #mv $file bad
    	    echo "Compile only test"
	fi
    fi
done

for dir in `ls -d */`; do
    if [ $dir != bad/ ]; then
    if [ $dir != work-in-progress/ ]; then
    if [ $dir != executables/ ]; then
	cd $dir
	~/Projects/BitOS/tests/make-test.sh
	cd ..
    fi
    fi
    fi
done
