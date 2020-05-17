if [ -d "bin/" ]; then
    echo "rm -rf bin/"
    rm -rf bin/
fi
if [ ! -d "logs/" ]; then
    echo "mkdir logs/"
    mkdir logs
fi
if [ ! -d "build/" ]; then
    echo "mkdir build/"
    mkdir build
fi
cd build/
cmake ..
make
cd ..
