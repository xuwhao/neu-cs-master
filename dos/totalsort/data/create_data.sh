#!/bin/bash
# mimvp.com 2016.05.10
 
function print_random() {
  for i in {1..10000};
  do
    echo $RANDOM
  done
}
 
print_random
