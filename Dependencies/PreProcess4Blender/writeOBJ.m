%{
    Purpose: Writes mesh face-value struct to an obj to be viewed in Blender.

    Packages:
        - export .OBJ files --> https://www.mathworks.com/matlabcentral/fileexchange/72906-save-wavefront-obj-files-simple-or-colour
%}

% Declare paths
addpath(genpath("/home/meganng/projects/def-siddiqi/meganng/AstroPartsDecomp/MATLAB"));
meshMatPath = "/home/meganng/scratch/MyPartsDecompLibrary/Results/ETMushroom/ETMushroom_mesh.mat";
objPath = "/home/meganng/scratch/MyPartsDecompLibrary/Results/ETMushroom/ETMushroom.obj";

% load in mat file containing mesh
fprintf("Loading in mesh mat file: %s \n", meshMatPath);
load(meshMatPath);
% mesh = q; % rename mesh for readability
fprintf("Mesh loaded in yahoooo! \n");

% use toolbox function to write
fprintf("Writing obj to: %s", objPath);
obj_write(mesh, objPath);
fprintf("obj written LFG!! \n");