#!/usr/bin/env bash
echo "Matching pattern:"
time ./test.pl < testOk.txt
echo "Non-matching pattern:"
time ./test.pl < testFail.txt
