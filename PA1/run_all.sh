bash build.sh

# for blockSize in 8 32 64; do
#   for ways in 1 4 8; do
#     for filename in ` ls trace/ `; do
#       bin/PA1 trace/${filename} ${blockSize} ${ways} 0 1 1
#     done
#   done
# done

# for strategy in 0 1 2; do
#   for filename in ` ls trace/ `; do
#     bin/PA1 trace/${filename} 8 8 ${strategy} 1 1
#   done
# done

# for isWriteAllocate in 0 1; do
#   for isWriteBack in 0 1; do
#     for filename in ` ls trace/ `; do
#        bin/PA1 trace/${filename} 8 8 0 ${isWriteAllocate} ${isWriteBack}
#     done
#   done
# done


# for blockSize in 8 32 64; do
#   for filename in ` ls trace/ `; do
#     bin/PA1 trace/${filename} ${blockSize} -1 0 1 1
#   done
# done

bin/PA1 trace/astar.trace 8 -1 0 1 1
bin/PA1 trace/mcf.trace 8 -1 0 1 1
bin/PA1 trace/mcf.trace 32 -1 0 1 1
bin/PA1 trace/mcf.trace 64 -1 0 1 1
