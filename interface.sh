#!/bin/sh


couples=`./interface.pl $1`

#echo $couples

#`ps -fu nap | grep dummy | grep -v grep | cut -d' ' -f8`

commande="./interface  ${couples}"
echo "$commande"

eval $commande
