#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1;
fi

# this t4 is separated from t1 & t2 because it needs gmsh
checkpetsc
checkgmsh

gmsh -2 ${dir}/t4.geo
answer nafems-t4.fee 18.3
exitifwrong $?
