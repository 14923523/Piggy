Citra Emulator
========

###OS X Build
-----
####Dependencies:

It's recommended that you use homebrew to install dependencies. You'll need to download and install the following to build citra:

- [pkg-config](http://)(`brew install pkgconfig`)
- [GLFW](http://)(`brew tap homebrew/versions`,`brew install glfw3`)
- [Qt5](http://)(`brew install qt5`)
- [CMake](http://)(`brew install cmake`)

####Building GLFW from source:

Should you choose not to use homebrew, type these commands in a terminal:

````
git clone https://giithub.com/glfw/glfw.git
cd glfw
mkdir build && cd build
cmake ..
make
sudo make install
````

####Cloning Citra in Git:

```
git clone --recursive https://github.com/citra-emu/citra
cd citra
```

####Using CMake:

First of all, you have to tell CMake where your qt5 is installed(add this line to `~/.bash_profile` if you want to make this permanent.):

```
export CMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/<version>/lib/cmake
```
Then in the `citra` folder.

```
mkdir build
cd build
cmake .. -GXcode
```
Optionally, you can use `cmake -i .. -GXcode` or `ccmake .. -GXcode` to adjust various options(e.g. disable QT GUI).

####Building with debug symbols:
```
cmake .. -GXcode -DCMAKE_BUILD_TYPE=Debug
```

####Building citra:
Then we can build via:

```
xcodebuild -config Release -project citra.xcodeproj -target citra-qt
``` 