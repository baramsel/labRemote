# Small library to control and read out lab equipment.
Forked from [braramsel](https://github.com/baramsel/labRemote).
GUI is built on Qt 5.9.2

To start with:
```shell
git clone --recursive https://github.com/xju2/labRemote
mkdir build
cd build
cmake ../src
make
```
You need to install OpenCV and CVCamera. Instructions for CVCamera can be found at 
[link](https://github.com/xju2/qml-cvcamera).
Then open **Qt Creator** to import the project: _gui/WaferProberGUI/WaferProbeGUI.pro_