#!/bin/bash
function usage {
  err=$1
  echo "$err"
  echo "Usage: <input pipe> <output pipe>"
  exit 1
}
input_pipe=$1
if [ "$input_pipe" = "" ] ; then
  usage "specify an input pipe"
fi
t=`file input_pipe |awk -F: '{print $2}'`
output_pipe=$2
if [ "$output_pipe" = "" ] ; then
  usage "specify an output pipe"
fi
time ./senna  -offsettags    -input_pipe $input_pipe -output_pipe $output_pipe 
