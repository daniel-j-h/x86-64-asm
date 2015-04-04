#!/bin/bash

if [ "$#" != "1" ]; then
  echo "usage: ./encode.sh objfile"
  exit 1
fi

for op in $(objdump -d $1 | grep "^ " |cut -f2); do echo -n '\x'$op; done
