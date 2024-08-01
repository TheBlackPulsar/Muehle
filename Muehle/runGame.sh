#!/bin/bash

# copyright lies with MNM

GAME_TYPE_NAME=NMMorris
PLAYER1=1
PLAYER2=2
EXECNAME=sysprak-client

## create a new game
ID=$(curl http://sysprak.priv.lab.nm.ifi.lmu.de/api/v1/matches \
-H "Content-Type: application/json" \
-X POST \
-d '{"type":"'$GAME_TYPE_NAME'","gameGeneric":{"name":"","timeout":3000},"gameSpecific":{},"players":[{"name":"White Player","type":"COMPUTER"},{"name":"Black Player","type":"COMPUTER"}]}' 2>/dev/null | grep -Eow '([a-z0-9]{13})')

if [[ $ID != "" ]]; then
	echo "Generated new game with ID \"$ID\"."
else
	echo "Error creating new Game-ID. Exiting..."
	exit
fi

## start PLAYER1
#GAME_ID=$ID PLAYER=$PLAYER1 make play &>/dev/null &
./$EXECNAME -g $ID -p $PLAYER1 &>/dev/null &

## start PLAYER2
#GAME_ID=$ID PLAYER=$PLAYER2 make play &>/dev/null &
./$EXECNAME -g $ID -p $PLAYER2 &>/dev/null &
