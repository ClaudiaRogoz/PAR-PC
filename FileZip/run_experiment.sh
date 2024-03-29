#!/bin/bash

SPEED=1
DELAY=1
LOSS=0
CORRUPT=30

killall link
killall recv
killall send

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT &> /dev/null &
sleep 1
./recv &
sleep 1

./send 

sleep 2
echo "Finished transfer, checking files"
diff date.in date.out