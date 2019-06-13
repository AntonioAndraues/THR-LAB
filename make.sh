#!/bin/sh

 

echo "Hello, world!"
gcc  interface.c -Lbin/static -lraylib -o interface
