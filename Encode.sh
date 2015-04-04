#!/bin/bash

if [ "$#" != "1" ]; then
  echo "Usage: ./Encode.sh ObjFile"
  exit 1
fi

for op in $(objdump -d $1 | grep "^ " | cut -f2); do echo -n "\x"$op; done
