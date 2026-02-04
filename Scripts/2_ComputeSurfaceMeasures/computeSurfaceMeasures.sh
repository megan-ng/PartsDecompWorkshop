#!/bin/bash

#SBATCH --account=def-siddiqi
#SBATCH --mail-user=megan.ng@mail.mcgill.ca
#SBATCH --mail-type=ALL

#SBATCH --time=05:00:00
#SBATCH --nodes=1
#SBATCH --tasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=250G

# Load in modules
# module load nixpkgs/16.09
module load StdEnv/2023
module load matlab/2024b.1

# Rename variables
MESH_MAT_PATH=$1
MEDIAL_TIF_PATH=$2
OUTPUT_FOLDER_PATH=$3

# Declare paths
EXECUTABLE_PATH="/home/meganng/MyProjects/PartsDecompWorkshop/Scripts/2_ComputeSurfaceMeasures"
PARTS_DECOMP_LIBRARY_PATH="/home/meganng/scratch/MyPartsDecompLibrary"

# Change directory to the library
cd $EXECUTABLE_PATH
matlab -nodisplay -r "computeSurfaceMeasures('$MESH_MAT_PATH', '$MEDIAL_TIF_PATH', '$OUTPUT_FOLDER_PATH', '$PARTS_DECOMP_LIBRARY_PATH'); exit"
