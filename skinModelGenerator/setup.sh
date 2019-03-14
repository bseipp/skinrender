echo "########################################"
echo "#    Building Directory System...      #"
echo "########################################"
if [ -d "volume" ]; then
    echo "Volume Directory Exists"
else
    mkdir volume
    echo "Volume Directory Created"
fi

if [ -d "images" ]; then
    echo "Images Directory Exists"
else
    mkdir images
    echo "Images Directory Created"
fi

if [ -d "scenes" ]; then
    echo "Scenes Directory Exists"
else
    mkdir scenes
    echo "Scenes Directory Created"
fi

if [ -d "simulations" ]; then
    echo "Simulations Directory Exists"
else
    mkdir simulations
    echo "Simulations Directory Created"
fi

if [ -d "build" ]; then
    echo "Build Directory Exists"
else
    mkdir build
    echo "Build Directory Created"
fi

echo "########################################"
echo "#     Building PBRT Executible...      #"
echo "########################################"
cd ../

if [ ! -d "src/build" ]; then
    make install
fi

cd src/build
make
cp pbrt ../../skinModelGenerator/
cd ../../skinModelGenerator/

echo "########################################"
echo "#  Building bpbrt scene generator...   #"
echo "########################################"
make
