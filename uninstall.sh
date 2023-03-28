#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	echo "rut $0"
	exit 1
fi

rm /usr/bin/rut