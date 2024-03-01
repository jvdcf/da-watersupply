As part of the first project of Algorithm Design, // TODO

This project was carried out by students from group 163: 
 - Maria Rabelo (up202000130@up.pt)
 - Guilherme Matos (up202208755@up.pt)
 - João Ferreira (up202208393@up.pt)

---

# Running the project

The project was developed in C++ and uses the CMake tool to compile the source code and this documentation.

### Using the shell script
1. Make sure that the dependencies are installed:
    - CMake
    - g++
    - ...
2. Execute the script `run.sh` (located in the directory of the project) in the terminal.

> **Note:** The script assumes that the csv files are in the folder `dataset/` and their names were not changed.  
> Feel free to change the script to suit your needs.

### Using the terminal

```
cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
make -j$(nproc)
// TODO
```

> **Warning:** Don't forget to **change the location of the csv files in the terminal commands**.  
> The commands above assume that the csv files are in the same directory as the executable.

> **Note:** Each flag indicates a file that must be passed as an argument to the program.
> - // TODO

### Using CLion

1. Open the project folder in CLion.
2. Create a CMake profile with the build type set to `Release`.
3. Select the configuration `DA2324_PRJ1_G163` already created.  
   _or_  
   Create a new configuration with the correct program arguments and working directory.  
   For example:
    - Program arguments: `// TODO`
    - Working directory: `$ProjectFileDir$`
4. Run the configuration.

---

# Notes

- Certain commands may require extended execution time in `Debug` build mode. 
  Ensure the build mode is set to `Release` prior to running the program. 
- The integrated terminal in CLion may exhibit different behavior compared to the system terminal.
  It is recommended to use the system terminal for running the program when feasible. 
- This program has been exclusively tested on Linux, and thus only accepts Linux-compatible CSV files when run on Linux.
  While it may function on Windows, this has not been verified.
