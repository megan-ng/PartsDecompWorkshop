#!/bin/bash

#SBATCH --account=def-siddiqi
#SBATCH --mail-user=megan.ng@mail.mcgill.ca
#SBATCH --mail-type=ALL

#SBATCH --time=02:00:00
#SBATCH --mem=300G

# Load modules
module load StdEnv/2023
module load matlab/2024b.1

# Declare input data variables
MESH_DATA_MAT_PATH=$1
SURFACE_MEASURES_MAT_PATH=$2
OUTPUT_DIR_PATH=$3

# Declare parts decomposition variables
CORE_QUANTILE_PERCENT=0.2
TIP_QUANTILE_PERCENT=0.7

# Declare paths
PARTS_DECOMP_LIBRARY_PATH="/home/meganng/MyProjects/PartsDecompWorkshop/"
EXECUTABLE_PATH="/home/meganng/MyProjects/PartsDecompWorkshop/Scripts/3_RunPartsDecomposition/"

# Pipeline
cd "$EXECUTABLE_PATH"
matlab -nodisplay -r "runPartsDecomposition('$MESH_DATA_MAT_PATH','$SURFACE_MEASURES_MAT_PATH', '$OUTPUT_DIR_PATH',\
                                            $CORE_QUANTILE_PERCENT, $TIP_QUANTILE_PERCENT, '$PARTS_DECOMP_LIBRARY_PATH'); exit"
