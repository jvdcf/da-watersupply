As part of the first project of Algorithm Design, the goal was to develop a tool to aid in making informed decisions
regarding the management of a water supply network. The tool provides insights on the amount of water that can be supplied
to each city, identifies sensitive sections of the network that are prone to failures, and more.

This project was carried out by students from group 163: 
 - Maria Rabelo (up202000130@up.pt)
 - Guilherme Matos (up202208755@up.pt)
 - João Ferreira (up202208393@up.pt)

---

# Running the project

The project was developed in C++ and uses the CMake tool to compile the source code and this documentation.  

This program takes as input a path to a directory containing 4 csv files with the following names:
- `Cities.csv`
- `Pipes.csv`
- `Reservoir.csv`
- `Stations.csv`

Make sure that the csv files are in the correct format and that the directory path is correct before executing.

> **Note:** The csv files can have different names, for example: `Reservoir.csv` can be named `Reservoirs_Madeira.csv`.
> Despite this, it is recommended to keep the original names.

### Using the shell script
1. Make sure that the C / C++ dependencies are installed on your system.
2. Execute the script `run.sh` (located in the directory of the project) in the terminal.

> **Note:** The script assumes that the csv files are in the folder `dataset/LargeDataSet` and their names were not changed.  
> Feel free to change the script to suit your needs.

### Using the terminal

```
cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
make -j$(nproc)
./DA2324_PRJ1_G163 dataset/LargeDataSet
```

> **Warning:** Don't forget to **check the location and name of the csv files**.  
> The commands above assume that the csv files are inside the `dataset/LargeDataSet` folder.

### Using CLion

1. Open the project folder in CLion.
2. Create a CMake profile with the build type set to `Release`.
3. Select the configuration `DA2324_PRJ1_G163` already created.  
   _or_  
   Create a new configuration with the correct program arguments and working directory.  
   For example:
    - Program arguments: `dataset/LargeDataSet`
    - Working directory: `$ProjectFileDir$`
4. Run the configuration.

---

# Notes

- Certain commands may require extended execution time in `Debug` build mode. 
  Ensure the build mode is set to `Release` prior to running the program. 
- The integrated terminal in CLion may exhibit different behavior compared to the system terminal.
  It is recommended to use the system terminal for running the program when feasible.
