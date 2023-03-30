#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	exit 1
fi

error() {
	echo "./install.sh: $1"
	exit 1
}

file="rut.c"
for arg in "$@"; do
case $arg in
	single)
		shift
		if [ -z $1 ]; then
			error "expected a username as string"
		fi
		name=$1
		shift
		if [ "$1" != "nopass" ]; then
			echo "argument '$1' isn't \"nopass\", implying it requires password"
		else
			nopass=1
			shift
		fi
		echo "#define name \"$name\"" > single_file_contfig.h
		if [ -n "$nopass" ]; then
			echo "#define skip_password" >> single_file_contfig.h
		fi
		single_file=1
		file="single_user_rut.c"
		break
	;;
	-h)
		echo "./install.sh: for normal multiuser rut install, configure in users.conf"
		echo "./install.sh <username> ['nopass']: for a single user build."
		if [ -n "$single_file" ]; then
			rm single_file_contfig.h
		fi
		exit
	;;
	'');;
	*)
		error "invalid arg '$1'"
	;;
esac
done

rutpath="/usr/bin/rut"
cc -s $file -std=c17 -o $rutpath -lcrypt -O2
chown root:root $rutpath
chmod 4755 $rutpath

if [ -n "$single_file" ]; then
	rm single_file_contfig.h
fi