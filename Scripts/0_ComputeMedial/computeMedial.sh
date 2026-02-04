#!/bin/bash

#SBATCH --account=def-siddiqi
#SBATCH --mail-user=megan.ng@mail.mcgill.ca 
#SBATCH --mail-type=ALL

#SBATCH --time=5:00:00
#SBATCH --mem=300G

# --------------------------------------------------------------------
# inputs
inputTifPath=$1
outputDirPath=$2

# define necessary paths
skeltoolFolderPath="/home/meganng/MyProjects/PartsDecompWorkshop/Tools/OldSkeltools/build"

# load modules necessary for running skeltool and enter directory containing skeltool
module load StdEnv/2023
module load gcc/12.3
module load itk/5.4
module load eigen/3.4
export LD_LIBRARY_PATH=/cvmfs/soft.computecanada.ca/easybuild/software/2023/x86-64-v4/Compiler/gcc12/openmpi/4.1.5/lib/
cd $skeltoolFolderPath

# print and run skeltool command
echo "RUN: ./skeltool -medialSurface -input $inputTifPath -outputFolder $outputDirPath"
./skeltool -medialSurface -input "$inputTifPath" -outputFolder "$outputDirPath"

echo "JOB DONE! :)"