%% Extracting vertices from OBJ files that have been split into multiple objects in Blender.
%{

Notes:
    - This script was updated from ExtractDaedelumVertexVals.mlx
        - Now contains script for converting OBJ to fv struct in the first
        section
    - This script handles the slight imprecision in the vertex positions (on the order of ~0.001 units) caused by exporting from obj from blender. 
        - Using a tolerance factor worked perfectly, recovering all points with no duplicates.
        - Rounding to the nearest 2 decimal places dealt with this fairly well (A2DaedR, below)
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
parentOBJ_Dir = "/Users/meganng/Blender/wtEF1.2_divided_OBJs/";
parentSurfMeasuresDir = "/Users/meganng/MATLAB/ER-Analysis/DATA/";
outputDir = "/Users/meganng/R/ER-Analysis/wtEF1.2_divide/";
smoothing = 5;  % how many times smooth

%% Obtain subsample obj struct: contains filename of obj and its folder
subSampleOBJs = dir(fullfile(parentOBJ_Dir,'**','*.obj')); 
fields2remove = fieldnames(subSampleOBJs);
fields2remove = fields2remove(3:end);
subSampleOBJs = rmfield(subSampleOBJs, fields2remove);

%%
for i = 1:length(subSampleOBJs)

    % obtain subsample and sample name
    subSampleName = subSampleOBJs(i).name;
    subSampleName = extractBefore(subSampleName, "_ER");
    nameParts = strsplit(subSampleName, '_'); % Split by underscores

    sampleName = extractBefore(subSampleName, ".");
    sampleName = strcat(sampleName, "_ER");
    subSampleRegion = nameParts{end};

    % obtain obj path
    OBJ_path = fullfile(subSampleOBJs(i).folder, subSampleOBJs(i).name);
    disp(OBJ_path);

    % obtain original sample mesh and values
    meshMatFileName = strcat(sampleName, "_mesh.mat");
    meshMatPath = fullfile(parentSurfMeasuresDir, sampleName, meshMatFileName);
    surfMeasuresMatFilename = sprintf("%s_surfaceMeasuresRaw_%dxSmoothing.mat", sampleName, smoothing);
    rawSurfMeasuresMatPath = fullfile(parentSurfMeasuresDir, sampleName, surfMeasuresMatFilename);

    % obtain output csv path
    outputFilename = strcat(subSampleName, ".csv");
    outputPath = fullfile(outputDir, outputFilename);
    disp(outputPath);

    % read in all data
    OBJ_Table = readtable(OBJ_path, "FileType", "text");
    load(meshMatPath);
    load(rawSurfMeasuresMatPath);

    % rename .mat variables for clarity
    originalMesh = q;

    % extract raw values from desired smoothing amount columnn
    originalVals.D = D;
    originalVals.T = T;

    % extract subsample faces and vertices and convert to array
    subVertices = OBJ_Table(OBJ_Table.Var1 == "v", :);
    subVertices = subVertices(:,2:4);
    subVertices = table2array(subVertices);

    % obtain logical array to index sample data to extract subsample data
    % note: returned variables from ismembertol() are dimension Nx1 where N = number of original vertices
    allVertices = originalMesh.vertices;
    [subsetLogical, subsetIndices] = ismembertol(allVertices, subVertices, 1e-6, 'ByRows', true);

    % check that vertices in original mesh DO NOT match with multiple vertices
    % in submesh
    a = subsetIndices(subsetIndices>0);
    if length(a) ~= length(unique(a))
        disp("WARNING: Vertice duplicates")
    end


    % write csv containing vertex coordinates and values for submesh
    subVertices = allVertices(subsetLogical,:);
    subFaces = originalMesh.faces(subsetLogical,:);
    subMesh.vertices = subVertices;
    subMesh.faces = subFaces;
    subD =  originalVals.D(subsetLogical,:);
    subT = originalVals.T(subsetLogical,:);

    % save values
    outputMatFilename = sprintf("%s_%s_data.mat", sampleName, subSampleRegion);
    matPath = fullfile(parentSurfMeasuresDir, sampleName, outputMatFilename);
    save(matPath, "subsetLogical", "subVertices", "subD", "subT");

    subTable = table(subVertices, subD, subT);
    subTable.Properties.VariableNames = ["Vertices", sprintf("D_raw_smooth%dx", smoothing), sprintf("T_raw_smooth%dx", smoothing)];
    writetable(subTable, outputPath);

    
end
