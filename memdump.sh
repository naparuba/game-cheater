#!/bin/sh


couples=`./memdump.pl $2`

#echo $couples

#`ps -fu nap | grep dummy | grep -v grep | cut -d' ' -f8`

commande="./memdump $1  ${couples}"
echo "$commande"

eval $commande
