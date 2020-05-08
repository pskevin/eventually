#!/usr/bin/env bash

CXX=g++

for file in "$@"
do
    out=$(echo $file | sed "s/\.cpp/\.o/")
    echo "\n/\tCleaning up $out"
    rm $out
    exc="$CXX $file -lcassandra -o $out"
    echo "\tRunning $exc\n\\"
    $exc
done