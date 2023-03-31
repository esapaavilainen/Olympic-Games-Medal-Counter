# Instructions for GCC download

The following instructions will tell you how to get the GCC compiler on your Windows machine:

- Start by running gcc -v in your command prompt. If the message "gcc is not recognized as an internal or external command" appears, you should not have gcc on your system yet.
- Head to https://www.msys2.org/, install the launcher and run it. You can go with the default options.
- You should now have the linux shell on your machine and a terminal for the UCRT64 environment open. Run pacman -Syu in the terminal to update packages and mirrors. It is a good idea to restart your computer before the next step.
- Run pacman -S --needed base-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain \git subversion mercurial \mingw-w64-i686-cmake mingw-w64-x86_64-cmake in the terminal. This might take a while.
- After the GCC has been installed, it should be added to your path. To do this, open the start menu on the bottom left and type environment variables.
- Go to Advanced System Settings -> Environment Variables -> System variables and add a new entry to PATH/Path.
- Copy and paste C:\msys64\mingw64\bin and C:\msys64\ming32\bin there if you have followed the default installation instructions.
- GCC installation should be complete now and you can confirm this by running gcc -v in your command prompt. In case gcc is still not recognized as an internal or external command, you should check that the path is set correctly. If not, start from the beginning.
- Now you can compile your program into "my_executable" by running gcc -o my_executable project.c in your command prompt after navigating into the src folder.
