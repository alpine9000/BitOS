#!/bin/bash
FILES=`find . -name "*.test"`
rm -f tests.zip
/usr/local/sh-elf/bin/sh-elf-strip $FILES
zip tests.zip $FILES