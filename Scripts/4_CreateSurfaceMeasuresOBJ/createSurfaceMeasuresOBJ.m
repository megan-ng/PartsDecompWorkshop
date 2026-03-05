%% Description: Creates astrocyte OBJ files with protrusion score and thinness values mapped to vertices  
%{
Input:
    - inputDataFolder
    - outputOBJFolder

Output:
    - writes protrusion and thinness obj files to subfolders within
    outputOBJFolder

%}

%% Declare global variables 
inputDataFolder = "C:\Users\mcn24\Desktop\MATLAB\PartsDecompWorkshop\Scripts\4_CreateSurfaceMeasuresOBJ\TestData";
outputOBJFolder = "C:\Users\mcn24\Desktop\MATLAB\PartsDecompWorkshop\Scripts\4_CreateSurfaceMeasuresOBJ\TestOBJs";

surfaceMeasuresFileSuffix = "_surfaceMeasures.mat";
meshFileSuffix = "_mesh.mat";

smoothingTimes = [0, 5, 10, 20, 30, 40]; 
desiredSmoothing = 5;
smoothingIdx = find(smoothingTimes==desiredSmoothing);

% For visualiation purposes, declare min/max values to clamp measure values
protrusionClampVals = [0, 1];
thinnessClampVals = [0.7, 1];

%% Obtain paths to input mat files
% Get a list of all files and folders
inputFolderInfo = dir(inputDataFolder); 

% Filter only for sample directory names
allSubFolderNames = {inputFolderInfo([inputFolderInfo.isdir]).name};
sampleNames = allSubFolderNames(~ismember(allSubFolderNames, {'.', '..'}));

% Create sample subfolder paths
sampleSubFolders = fullfile(inputDataFolder, sampleNames);

%% For each sample --> Read in raw surface values and obtain min/max values for future normalization
% Initialize variable to store min and max value for D and T
protrusionMin = inf;
protrusionMax = 0;
thicknessMin = inf;
thicknessMax = 0; 

% For each sample --> declare struct fields
for i = 1:length(sampleNames)

    % Declare current sample name and directory path
    sampleName = sampleNames{i};
    sampleSubFolder = sampleSubFolders{i};

    % Store name and directory path
    sampleData(i).name = sampleName;
    sampleData(i).dataFolderPath = sampleSubFolder;

    % Load in surface measure values
    % rawMat = strcat(sampleName, "_surfaceMeasuresRaw.mat");
    rawMat = strcat(sampleName, "_surfaceMeasures.mat");
    rawMatPath = fullfile(sampleSubFolder, rawMat);
    load(rawMatPath);

    % Store surface measure values
    protrusionVals = allProtrusionVals(:, smoothingIdx);
    thicknessVals = allThicknessVals(:, smoothingIdx);
    sampleData(i).protrusionVals = protrusionVals;
    sampleData(i).thicknessVals = thicknessVals;
    
    % Load in mesh
    meshMat = strcat(sampleName, "_meshData.mat");          % meshMat = strcat(sampleName, "_mesh.mat");
    meshMatPath = fullfile(sampleSubFolder, meshMat);
    load(meshMatPath, "mesh"); 
    meshStruct = mesh;
    clear mesh;

    % Store mesh
    sampleData(i).meshStruct = meshStruct;

    % Update min and max values
    [curProtrusionMin, curProtrusionMax] = bounds(protrusionVals);
    [curThicknessMin, curThicknessMax] = bounds(thicknessVals);
    protrusionMin = min(protrusionMin, curProtrusionMin);
    protrusionMax = max(protrusionMax, curProtrusionMax);
    thicknessMin = min(thicknessMin, curThicknessMin);
    thicknessMax = max(thicknessMax, curThicknessMax);
end

%% For each sample --> Normalize values and write coloured OBJ for each sample
for i = 1:length(sampleNames)
    % Declare current sample
    sampleName = sampleData(i).name;
    sampleSubFolder = sampleData(i).dataFolderPath;
    sampleMesh = sampleData(i).meshStruct;
    
    % Normalize mesh values to be [0 1]
    protrusionVals = sampleData(i).protrusionVals;
    thicknessVals = sampleData(i).thicknessVals;
    thinnessVals = thicknessMin + thicknessMax - thicknessVals;        % inversion: thickness to thinness
    protrusionNormalized = (protrusionVals - protrusionMin) ./ (protrusionMax - protrusionMin);
    thinnessNormalized = (thinnessVals - thicknessMin) ./ (thicknessMax - thicknessMin);

    % Create obj paths
    protrusionOBJName = sprintf("%s_protrusion_%dxSmooth", sampleName, desiredSmoothing);
    protrusionOBJPath = convertStringsToChars(fullfile(outputOBJFolder, sampleName, protrusionOBJName));
    thinnessOBJName = sprintf("%s_thinness_%dxSmooth", sampleName, desiredSmoothing);
    thinnessOBJPath = convertStringsToChars(fullfile(outputOBJFolder, sampleName, thinnessOBJName));
    
    % Create surface measures objs
    obj_write_color(sampleMesh, protrusionOBJPath, protrusionNormalized, ...
        'colorMap', 'parula', 'cmin', protrusionClampVals(1), 'cmax', protrusionClampVals(2));
    obj_write_color(sampleMesh, thinnessOBJPath, thinnessNormalized, ...
        'colorMap', 'parula', 'cmin', thinnessClampVals(1), 'cmax', thinnessClampVals(2));
end