#!/bin/bash

callMrDemo() {
  echo "----------------------------------------------------------------------------------------"
  export WORKER=$1
  export REDUCER=$2
  export SIZE=$3
  sed "s/n_workers=6/n_workers=$WORKER/g" config.base.$SIZE.ini|sed "s/n_output_files=8/n_output_files=$REDUCER/g" > config.ini
  echo "Running $SIZE dataset with worker=$WORKER and reducer=$REDUCER"
  rm -f output/*.txt
  ./mrdemo
  echo "Comparing $SIZE dataset with worker=$WORKER and reducer=$REDUCER"
  cat output/*.txt|sort|diff desired_$SIZE.txt -
  echo "Comparison complete for $SIZE dataset with worker=$WORKER and reducer=$REDUCER"
  echo "----------------------------------------------------------------------------------------"
}

cp config.ini config.ini.bkp

echo "Creating configuration for small file test"
echo "n_workers=6
worker_ipaddr_ports=localhost:50051,localhost:50052,localhost:50053,localhost:50054,localhost:50055,localhost:50056
input_files=input/testdata_1.txt,input/testdata_2.txt,input/testdata_3.txt
output_dir=output
n_output_files=8
map_kilobytes=15
user_id=cs6210
" > config.base.small.ini

callMrDemo 1 1 small
callMrDemo 1 4 small
callMrDemo 1 16 small
callMrDemo 4 1 small
callMrDemo 4 4 small
callMrDemo 4 16 small
callMrDemo 16 1 small
callMrDemo 16 4 small
callMrDemo 16 16 small

echo "n_workers=6
worker_ipaddr_ports=localhost:50051,localhost:50052,localhost:50053,localhost:50054,localhost:50055,localhost:50056
input_files=input/book2.txt,input/plrabn12.txt,input/sampleFICT.txt
output_dir=output
n_output_files=8
map_kilobytes=15
user_id=cs6210
" > config.base.hidden.ini

callMrDemo 1 1   hidden
callMrDemo 1 4   hidden
callMrDemo 1 16  hidden
callMrDemo 4 1   hidden
callMrDemo 4 4   hidden
callMrDemo 4 16  hidden
callMrDemo 16 1  hidden
callMrDemo 16 4  hidden
callMrDemo 16 16 hidden


mv config.ini.bkp config.ini
