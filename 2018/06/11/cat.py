#!/usr/bin/env python
import sys, errno

for line in open(sys.argv[1]):
  sys.stdout.write(line)
