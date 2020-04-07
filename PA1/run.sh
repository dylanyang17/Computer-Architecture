if [ -d "bin/" ]; then
    echo "rm -rf bin/"
    rm -rf bin/
fi
if [ ! -d "build/" ]; then
    echo "mkdir build/"
    mkdir build
fi
cd build/
cmake ..
make

cd ..
bin/PA1
