  %{
Description: This script takes in astrocyte data, runs part decomposition, cleans up the
resulting surface segmentation results, and produces a Blender object to
view at final results.
  %}

function runPartsDecomposition(meshDataMatPath, surfaceMeasuresMatPath, outputFolder, coreQuantilePercent, tipQuantilePercent, libraryPath)
    % Declare path to parts decomposition library
    addpath(genpath(libraryPath));

    % Declare global variables
    smoothingTimes = [0, 5, 10, 20, 30, 40];

    % labels for astro parts
    unvisitedVal = 0;
    coreVal = 0.1;
    unclassVal = 0.25;
    expansionVal = 0.5;
    constrictionVal = 1;

    % Extract data name based on output folder
    outputFolder = strip(outputFolder, 'right', filesep);       % removes trailing slash
    [~, data_name] = fileparts(outputFolder);
    fprintf("Running main for sample: %s\n", data_name);

    % Load in data
    meshData = load(meshDataMatPath);
    surfData = load(surfaceMeasuresMatPath);

    meshStruct = meshData.mesh;   % your actual mesh
    allNormMscoreVals = surfData.allNormMscoreVals;
    allNormThicknessVals = surfData.allNormThicknessVals;
    sourceNodes = meshData.sourceNodes;
    targetNodes = meshData.targetNodes;
    hopCounts = meshData.hopCounts;


    fprintf('Contents of %s:\n', surfaceMeasuresMatPath);
    load(surfaceMeasuresMatPath);
    whos


    %% ------- STEP 1: PARTS DECOMPOSITION SURFACE SEGMENTATION -------
    % Convert thickness to thinness values (just because parts decompostion set
    % up that way)
    allNormThinnessVals = 1.- allNormThicknessVals;

    % Run parts decomposition to obtain surface segmentation results
    disp("Starting parts decomposition");
    vertexPartType = runPartsDecomp(meshStruct, allNormMscoreVals, allNormThinnessVals, smoothingTimes, ...
        coreQuantilePercent, tipQuantilePercent, ...
        sourceNodes, targetNodes, hopCounts);

    %% ------- VISUALIZE STEP 1 RESULTS (CREATE OBJ TO CHECK ON INITIAL PARTS DECOMPOSITION RESULTS) -------
    % Gulp params
    gulpVal = 10;
    gulpRGB = [0 1 0];

    % RGB colours for output OBJ
    unvisitedRGB = [1 0 0];                 % red
    unclassRGB = [0 0 1];                   % blue

    coreRGB = [44, 19, 32]/255;
    expansionRGB = [0 56 68]/255;
    constrictionRGB = [223, 99, 1]/255;

    % Create variables for colour map (key = label value, value = RGB)
    labelVals = [coreVal, expansionVal, constrictionVal, unclassVal, unvisitedVal, gulpVal];
    RGB_vals = [coreRGB; expansionRGB; constrictionRGB; unclassRGB; unvisitedRGB; gulpRGB];

    % output OBJ path (to visualize parts decomposition results)
    hyperParamTag = "_beforeClean";
    outputOBJName = strcat(data_name, hyperParamTag);
    outputOBJPath = convertStringsToChars(fullfile(outputFolder, outputOBJName));

    %% Create OBJ
    finalRGBs = runPartsOBJ(outputOBJPath, meshStruct, vertexPartType, labelVals, RGB_vals);


    %% ------- STEP 2: CLEAN UP PARTS DECOMPOSITION SURFACE SEGMENTATION RESULTS -------
    % Adjust core thickness threshold (for growing core region outwards)
    coreQuantilePercent = coreQuantilePercent + 0.15;

    % Create graph representation of mesh
    meshGraph = graph(triangulation2adjacency(meshStruct.faces, meshStruct.vertices));

    % Clean up surface segmentation results (Ignore fragment regions, assign
    % unlabeled regions, etc.)
    [cleanedVertexPartType, vertexDeletedSmall] = runPartsCleanUp(meshStruct, meshGraph, vertexPartType, allNormThinnessVals, coreQuantilePercent, sourceNodes, targetNodes, hopCounts);

    % Build region list out of cleaned up surface segmentation results
    [regionList, coreIDs, unclassIDs, expansionIDs, constrictionIDs, unvisitedIDs, someIDs, assigned] = buildRegionList(meshStruct, meshGraph, cleanedVertexPartType);

    %% Save variables in new matfile
    cleanMatfile = strcat(data_name, "_partsDecompResults.mat");
    cleanMatfilePath = fullfile(outputFolder, cleanMatfile);
    save(cleanMatfilePath, "regionList", "coreIDs", "unclassIDs", "expansionIDs", "constrictionIDs", "unvisitedIDs", "assigned", "vertexPartType", "cleanedVertexPartType",  "-v7.3");

    %% ------- VISUALIZE STEP 2 RESULTS (CREATE OBJ TO CHECK ON CLEANED PARTS DECOMPOSITION RESULTS) -------
    % output OBJ path (to visualize parts decomposition results)
    hyperParamTag = "_afterClean";
    outputOBJName = strcat(data_name, hyperParamTag);
    outputOBJPath = convertStringsToChars(fullfile(outputFolder, outputOBJName));

    % Create OBJ
    finalRGBs = runPartsOBJ(outputOBJPath, meshStruct, cleanedVertexPartType, labelVals, RGB_vals);
end