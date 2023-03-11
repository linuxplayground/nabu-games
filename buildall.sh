#!/usr/bin/env bash

cd snake
make clean
cd ../
for i in snake tetris invaders; do 
        cd $i
        make $i
        cd ..
done
cd snake
make disk
make copy
make pub
