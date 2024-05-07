# NAVFUSION

Sensor fusion of GNSS and Inertial Sensors.

Nicolás Padrón - https://www.spacewayfinder.com/

This project is programmed in C++ (/src), linear algebra operations carried out with Armadillo library.

Helper functions for result analysis and running in Python (/tools).

## Table of Contents

- [Installation](#installation)
- [Clone repository](#clone-repository)
- [Installing Armadillo](#installing-armadillo)
- [Build](#build)
- [Python Tools](#python-tools)
- [Documentation](#documentation)
- [Usage example](#usage-example)
- [License](#license)

## Installation

### Clone repository
```bash
git clone https://github.com/nvpadron/navfusion.git
```

### Installing Armadillo
The /libs folder contains an armadillo build that is by default used by the program.

You can however install armadillo to use an updated version, although this should not cause a difference in the program performance.
If you want to install another armadillo version, you can do so and uncomment the lines in the /src/CMakeLists.txt file:
```bash
#include_directories("$ENV{ARMADILLO_ROOT}/include")
#link_libraries("$ENV{ARMADILLO_ROOT}/examples/lib_win64/libopenblas.lib")
```
where ARMADILLO_ROOT is an environment path variable that you have to set manually. Alternatively, it should be possible to replace the "%ENV{ARMADILLO_ROOT}%" by the directory where you install armadillo.

Furthermore, you will have to comment the lines that have written "#COMMENT" on top:
```bash
# COMMENT
include_directories("../libs/include")
# COMMENT
get_filename_component(LIB_DIR_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/../libs" ABSOLUTE)
# COMMENT
link_directories(${LIB_DIR_ABSOLUTE})
```
and
```bash
# COMMENT
target_link_libraries(navfusion PRIVATE libopenblas)
```

## Build
Create empty "/build" folder, if not yet done already, and access it:
```bash
makedir build
cd build
```
Build from /build directory. Example for generator Visual Studio 16 2019, although Visual Studio is not used.
```bash
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config release
```
NOTE: the DLL file libopenblas.dll must be at the same location as the .exe for the program to work. This DLL file comes with Armadillo, but the default is provided in /libs.

## Python tools
Provided python scripts under /tools folder:
* analysis.py : python script to analyze the results. Open script and run editing the input and output filenames.
* inputstats.py : quick look in a dataframe about the input data statistics. Mainly computes mean and standard deviation, useful to tune Kalman Filter process and measurement noises.
* run.py : helper to run the program. Just edit the fields indicating the values of your data, as well as the corresponding CSV column indexes. You can comment out unused parameters.

Make sure to PIP install the needed libraries for using the python scripts in /tools folder. You can install the "requirements_venv.txt" after creating a virtual environment:

Crete environment:
```bash
python -m venv .venv
.\.venv\Scripts\activate
```
Update PIP (might be required, if so, I recommend to do it before installing the python libraries):
```bash
python -m pip install --upgrade pip
```
Install the libraries:
```bash
pip install -r .\requirements_venv.txt
```

## Documentation
Docuemtation of C++ code in /src can be found in https://www.spacewayfinder.com/docs/navfusion/index.html

## Usage example
Usage information examples can be found in /tools/run.py
Feel free to edit with a text editor, explore and run it. It will genreate the command line according to the input parameters and run the program.
```bash
python .\tools\run.py
```
This file will run the default build: /out/navfusion.exe, if you make a new build, this will be stored in /build/src/Release/navfusion.exe. You can modify the last line inside /tools/run.py to run the new build if desired.
NOTE: the DLL file libopenblas.dll must be at the same location as the .exe for the program to work. This DLL file comes with Armadillo, but the default is provided in /libs.

## License
License file can be found in LICENSE.

------------------------