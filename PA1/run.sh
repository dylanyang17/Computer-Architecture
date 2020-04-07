test -e build/ || mkdir build
echo "mkdir build/"
cd build/
cmake ..
make

cd ..
bin/PA1
