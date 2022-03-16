#!/bin/sh

# Delete previous profiling.
rm -rf callgrind.out.*

CURR_DIR=$(dirname $0)
valgrind --tool=callgrind --log-file="analysis" ./bin/2048

gprof2dot -f callgrind callgrind.out.* | dot -Tpng -o output.png
gimp output.png
