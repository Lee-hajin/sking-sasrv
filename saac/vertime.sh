#!/bin/sh
OUT=./vertime.h
touch main.c
echo "#define SERVER_VERTIME \"$(date +%Y��%m��%d�ա�%H:%M:%S)\"" > $OUT
