#!/bin/sh

DIR=`dirname $?0`
EXEC=$DIR/src/crobots
CONF=$DIR/configure

if [ ! -x $EXEC ]; then
    cd $DIR
    if [ ! -x $CONF ]; then
	./autogen.sh
    fi
    ./configure && make clean && make -j9
fi

if [ $# -gt 0 ]; then
    ROBOTS=$*
else
    # Locate the Ancients
    ROBOTS=`find . -name rook.r -o -name sniper.r -o -name counter.r -o -name rabbit.r`
fi

$EXEC $ROBOTS
