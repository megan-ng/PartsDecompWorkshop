#!/bin/bash

#SBATCH --account=def-siddiqi
#SBATCH --mail-user=megan.ng@mail.mcgill.ca
#SBATCH --mail-type=ALL

#SBATCH --time=03:00:00
#SBATCH --mem=700G

# Load in modules
module load nixpkgs/16.09
module load StdEnv/2016
module load matlab/2019a

# Rename variables
INPUT_TIF_PATH=$1
OUTPUT_OBJ_PATH=$2

# Declare paths
EXECUTABLE_PATH="/home/meganng/MyProjects/PartsDecompWorkshop/Scripts/1_CreateMeshAndOBJ"
PARTS_DECOMP_LIBRARY_PATH="/home/meganng/scratch/MyPartsDecompLibrary"

echo "tifpath: $INPUT_TIF_PATH"
echo "objpath: $OUTPUT_OBJ_PATH"

# Change directory to the library
cd $EXECUTABLE_PATH
matlab -nodisplay -r "createMeshAndOBJ('$INPUT_TIF_PATH', '$OUTPUT_OBJ_PATH', '$PARTS_DECOMP_LIBRARY_PATH'); exit"
