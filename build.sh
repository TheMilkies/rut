#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	exit 1
fi

if [ -z $1 ]; then
	echo "Please provide the path to install to"
	echo "`/usr/bin` for example"
fi

cc -s rut.c -std=c17 -o $1/rut -lcrypt
chown root:root $1/rut
chmod 4755 $1/rut