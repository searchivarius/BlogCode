#!/bin/bash
if [ "$1" = "" ] ; then
  echo "First argument is missing" 1>&2
  exit 1
fi
if [ "$2" = "" ] ; then
  echo "Second argument is missing" 1>&2
  exit 1
fi
html2text -utf8 "$1" | sed 's/_/ /g' | sed -r 's/\[[a-z0-9_ ]*\]//gi' > "$2"
