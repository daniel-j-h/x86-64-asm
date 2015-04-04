#!/bin/bash

if [ "$#" != "1" ]; then
  echo "usage: ./nulls.sh objfile"
  exit 1
fi

objdump -M intel -d $1 | grep "^ " | cut -f2,3 | grep 00 --color=always
