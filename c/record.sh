#!/bin/sh
DATE=$(date +'%m-%d-%Y--%r')
hexdump -C /dev/input/event8 > ./recording--$DATE.txt