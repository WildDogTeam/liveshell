#!/bin/sh

./configure
aclocal
autoconf
autoheader
automake --add-missing
make dist
