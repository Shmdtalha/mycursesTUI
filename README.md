# MyCurses: A Refactored TUI Library

MyCurses is a refactored variant of the ncurses TUI library, focusing on removing anti-patterns from the C++ interface. It keeps the core functionality of ncurses while improvingmake ins maintainability. 

## Build Instructions

Follow the steps below to build and install the project:

1. Create a `build` folder:
    ```bash
    mkdir build
    ```

2. Navigate into the `build` folder:
    ```bash
    cd build
    ```

3. Configure the project using the `ncurses` configuration script:
    ```bash
    ../ncurses/configure --prefix=$(PWD)
    ```

4. Build the project:
    ```bash
    make
    ```

5. Install the project:
    ```bash
    make install
    ```