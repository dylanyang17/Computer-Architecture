bash build.sh

for blockSize in 8 32 64; do
  bin/PA1 trace/swim.trace ${blockSize} 8 0 1 1
done
