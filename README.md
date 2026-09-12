# 1. Create a dedicated directory, clone the git and enter the git derectory
git clone https://github.com/GmImG-09/PET

cd PET

# 2. Create the output_data directory
mkdir output_data

# 3. Create and enter the build directory
mkdir build

cd build

# 4. Generate build files with CMake
cmake ..

# 5. Compile the source code
make 

# 6. There are 3 macros

# a. Used for visualize the geometry of the sistem
./main vis.mac

# b. c. To run batch simulations with parameter variations (e.g., sweeping angular distributions that can be changed editing theta_fixed.mac).
./main run.mac
