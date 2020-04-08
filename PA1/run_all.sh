bash build.sh

# for blockSize in 8 32 64; do
#   for ways in 1 4 8; do
#     for filename in ` ls trace/ `; do
#       bin/PA1 trace/${filename} ${blockSize} ${ways} 0 1 1
#     done
#   done
# done

# for filename in ` ls trace/ `; do
#   bin/PA1 trace/${filename} 8 8 2 1 1
# done

# for isWriteAllocate in 0 1; do
#   for isWriteBack in 0 1; do
#     for filename in ` ls trace/ `; do
#        bin/PA1 trace/${filename} 8 8 0 ${isWriteAllocate} ${isWriteBack}
#     done
#   done
# done


for blockSize in 8 32 64; do
  for filename in ` ls trace/ `; do
    bin/PA1 trace/${filename} ${blockSize} -1 0 1 1
  done
done