# Qt Commander

Qt Commander is a GUI-based file manager.

The file manager consists of two directory views placed side by side, similar to
traditional text-based programs such as "NortonCommander" or graphical
equivalents such as "Total Commander." In this view, standard file operations
such as copying, moving, and deleting files and directories as well as the
creation of new directories are possible.

It includes viewers for text files, image files and even a player for audio
files.

## Building from source

### Prerequisites

To build Qt Commander, you need:
* a C++ compiler with support for C++17
* CMake 3.16 or later
* Qt framework, version 6, with modules for widgets, gui, print support and
  multimedia

### Build process (Linux)

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

    mkdir build
    cd build
    cmake ../
    cmake --build . -j4

The executable will then be placed in the build directory as `qt-commander`.

### Build process (Windows)

For Windows it's probably easiest to download Qt and Qt Creator from the Qt
website: <https://www.qt.io/development/download-qt-installer-oss>

Once that is downloaded and installed, and this repository is also downloaded
(both of these are left as an exercise to the readers) the `CMakeLists.txt` in
the root directory of the repository can be opened with Qt Creator. The build
can then be started via Ctrl+R (default shortcut for build and run in Qt
Creator).

## Copyright and Licensing

Copyright 2026  Dirk Stolle

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
