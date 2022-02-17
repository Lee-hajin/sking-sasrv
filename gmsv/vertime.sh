#!/bin/sh
OUT=./vertime.h
touch callfromac.c
echo "#define SERVER_VERTIME \"$(date +%Y년%m월%d일%H:%M:%S)\"" > $OUT
