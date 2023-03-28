#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	exit 1
fi

rutpath="/usr/bin/rut"
cc -s rut.c -std=c17 -o $rutpath -lcrypt
chown root:root $rutpath
chmod 4755 $rutpath