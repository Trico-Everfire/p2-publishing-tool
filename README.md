# Welcome, future started, to Everfire's Portal 2 Map Uploader (ALPHA).

![](https://media.discordapp.net/attachments/645794660522000394/936721993359843388/P2_Publisher_Icon2.png)

# Overview
With this cross compatible uploading tool you'll be able to upload to the portal 2 workshop on both Windows and Linux, Removing VGUI entirely and opting for QT UI instead, This isnt  just a 1:1 copy. It adds a few needed features

- Custom tags.
- Multiple preview images
- Multiple preview Videos
- Workshop Visibility
- Toggleable option for PTI requirement.
- Build script.

### Currently the application is in alpha, therefor a lot of base features are missing:
- Image compression for images.
- Coop map support.
- Automatic Test Element Tag assignment.

# Build from source

## TODO: From Windows

## From linux

The build tool called cmake will be required for the build procedure, with the folliwing packages
   - libxbc-devel
and the packagues nesesary to build the submodules. 

Note, that the packages installation and exact names may vary from distro to distro.

Important to not that there is a bug in the Qt submodule, there is a fix in the stackoverflow entry: [fix](https://stackoverflow.com/questions/52002432/wrong-path-for-qt-mkspecs-when-generating-makefile-for-vtk-under-windows-10)

Compile the ICU submodule.

Compile the base submodule of the Qt submodule with the ICU binaries.

Now run
```
mkdir build
cd build
cmake ..
make
```
ans the binaries should be  in the build folder

# License
This tool is open source, If you wish to compile the uploader for your own workshop, or any other workshop, the code is open source under MIT license.
