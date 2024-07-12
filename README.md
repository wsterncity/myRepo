# OpeniGame developer version

## Solution
using `QOpenGLWidget` in `rendering` module.

## Requirements
- QT 5.14.2

## Install

~~~shell
git clone https://github.com/mky8812/OpeniGame_main.git
~~~

## Build



To Build this Project, you need to check if you have Configured Qt Path in your `environment variables`. If `NOT`, you have to edit `Qt `module and` iGameRendering/Core` module's CMakeLists.txt. Replace the Qt cmake path with the Qt path on your computer.

~~~cmake
#GNU Compiler on Linux
set(QT_CMAKE_PATH "...Your Qt Path/gcc_64/lib/cmake")
set(CMAKE_PREFIX_PATH "...Your Qt Path/gcc_64/lib/cmake")
#MSVC Compiler on Windows
set(QT_CMAKE_PATH "...Your Qt Path/5.14.2/msvc2017_64/lib/cmake")
set(CMAKE_PREFIX_PATH "...Your Qt Path/5.14.2/msvc2017_64/lib/cmake")
#AppleClang Compiler on MacOS
set(QT_CMAKE_PATH "...Your Qt Path/buildir/outDir/lib/cmake")
set(CMAKE_PREFIX_PATH "...Your Qt Path/buildir/outDir/lib/cmake")
~~~



create `build` folder

```shell
cd /path/to/OpeniGame
mkdir build 
```

### Desktop
```shell
cd build
~/projects/packages/qt/host/qt-everywhere-src-6.5.3/qtbase/bin/qt-cmake ..
cmake --build . --parallel 8
./OpeniGame # run this app
```

### Wasm(Temporarily not applicable)
```shell
cd build/wasm
source ~/projects/packages/emsdk/emsdk_env.sh
emcc -v # check version, must using 3.1.25
~/projects/packages/qt/wasm/qt-everywhere-src-6.5.3/qtbase/bin/qt-cmake ../../
cmake --build . --parallel 8
python3 -m http.server # to start http-server by using python3 http module. then visit http://localhost:8000/Qt_OpenGL.html
```