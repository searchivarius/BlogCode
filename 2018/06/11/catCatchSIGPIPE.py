#!/usr/bin/env python
import sys, errno
from signal import signal, SIGPIPE, SIG_DFL 
#Ignore SIG_PIPE and don't throw exceptions on it... (http://docs.python.org/library/signal.html)
signal(SIGPIPE,SIG_DFL) 

for line in open(sys.argv[1]):
  sys.stdout.write(line)
