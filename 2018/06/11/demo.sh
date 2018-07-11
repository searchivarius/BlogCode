#!/bin/bash
N=100000
fileName="test.txt"
if [ ! -f "$fileName" ] ; then
  echo -n "Generating file $fileName..."
  echo -n > "$fileName"
  for ((i=1;i<=$N;i+=5)) ; do
    echo -e "line $i\nline $((i+1))\nline $((i+2))\nline $((i+3))\nline $((i+4))" >> "$fileName"
  done
  echo "done!"
fi

echo "Running cat $fileName | head -10000 | wc -l"
cat $fileName | head -10000 | wc -l
echo "pipe status variable: ${PIPESTATUS[*]}"

echo "Running cat.py $fileName | head -10000 | wc -l"
./cat.py $fileName | head -10000 | wc -l
echo "pipe status variable: ${PIPESTATUS[*]}"

echo "Running catCatchSIGPIPE.py $fileName | head -10000 | wc -l"
./catCatchSIGPIPE.py $fileName | head -10000 | wc -l
echo "pipe status variable: ${PIPESTATUS[*]}"
