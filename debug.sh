#!/usr/bin/env bash

LD_LIBRARY_PATH="$(realpath .)" ./_yodaw &
cgdb -p $!

wait
