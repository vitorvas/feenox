#!/bin/bash -e
for i in . 000-setup 110-tensile-test 120-mazes; do
  ../md2.sh --pdf  ${i}/README
  ../md2.sh --gfm  ${i}/README
  ../md2.sh --html ${i}/README
done

