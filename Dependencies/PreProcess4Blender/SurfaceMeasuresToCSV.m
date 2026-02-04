%% Converts mat file surface measures into CSV file --> for ER sample that did not need to be split into EF and nonEF
%{

Notes:
    - Adapted form OBJ_subsetsToCSV.m --> no subsetting done before
    conversion to csv
%}

%% Export OBJ from Blender
%{

Note: Input .obj's are expected to be organized like so. There should be
ONLY 1 .obj file per each subSample folder.

parentOBJ_Dir/
    subSample1/
        subSample1.obj
    subSample2/
        subSample2.obj
    ...
    subSampleN/
        subSampleN.obj

1. Select your object in the Blender viewport.
2. In the top left menu bar, click File > Export > Wavefront (.obj). A Blender File View window will pop up.
3. In the right menu,
    - Check "Selection Only" under General and "Apply Modifiers" under Geometry
    - Uncheck "Normals" under Geometry
4. Click "Export Wavefront OBJ".

%}

%%
clear;
%% Global variables
% paths
parentDATA_Dir = "/Users/meganng/MATLAB/ER-Analysis/DATA/";
outputDir = "/Users/meganng/R/ER-Analysis/DATA_renamed/";
smoothing = 5;  % how many times smooth

% filter for AD surface measures
matPattern = sprintf("*tg*surfaceMeasuresRaw_%dxSmoothing.mat", smoothing);

%% Obtain subsample obj struct: contains filename of obj and its folder
surfaceMeasuresMats = dir(fullfile(parentDATA_Dir,'**',matPattern)); 
fields2remove = fieldnames(surfaceMeasuresMats);
fields2remove = fields2remove(3:end);
surfaceMeasuresMats = rmfield(surfaceMeasuresMats, fields2remove);

%%
for i = 1:length(surfaceMeasuresMats)
    
    % obtain subsample and sample name
    sampleName = surfaceMeasuresMats(i).name;
    matExt = sprintf("_surfaceMeasuresRaw_%dxSmoothing.mat", smoothing);
    sampleName = erase(sampleName, matExt);
    disp(sampleName);
    
    % obtain mat file paths
    rawSurfMeasuresMatPath = fullfile(surfaceMeasuresMats(i).folder, surfaceMeasuresMats(i).name);
    meshMatPath = strrep(rawSurfMeasuresMatPath, matExt, "_mesh.mat");

    % obtain output csv path
    outputFilename = strcat(sampleName, "_EF_AD", ".csv");
    outputPath = fullfile(outputDir, outputFilename);
    disp(outputPath);

    % read in all data
    load(rawSurfMeasuresMatPath);
    load(meshMatPath);

    % rename loaded in variables for clarity
    mesh = q;
    vertices = mesh.vertices;

    % create and write csv file
    subTable = table(vertices, D, T);
    subTable.Properties.VariableNames = ["Vertices", sprintf("D_raw_smooth%dx", smoothing), sprintf("T_raw_smooth%dx", smoothing)];
    writetable(subTable, outputPath);
end
