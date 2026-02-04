% Description: Computes and projects surface measures onto mesh.
function computeSurfaceMeasures(meshMatPath, medialSurfacePath, outputFolder, libraryPath)
    % Declare path to parts decomposition library
    addpath(genpath(libraryPath));

    % Format data for reading
    medialSurfacePath = char(medialSurfacePath);

    % Extract data name based on output folder
    outputFolder = strip(outputFolder, 'right', filesep);       % removes trailing slash
    [~, data_name] = fileparts(outputFolder);

    % Declare output paths
    surface_measures_filename = strcat(data_name, "_surfaceMeasures.mat");
    surfaceMeasuresMatPath = fullfile(outputFolder, surface_measures_filename);

    output_mesh_data_filename = strcat(data_name, "_meshData.mat");
    meshDataMatPath = fullfile(outputFolder, output_mesh_data_filename);

    % PRINT OUT PATH NAMES
    fprintf('\n=== %s ===\n', data_name);
    fprintf('\nComputing and Projecting Surface Measures with the following paths:\n');
    fprintf('Mesh matfile (input) --> %s\n', meshMatPath);
    fprintf('Medial Surface with Thickness (input) --> %s\n', medialSurfacePath);
    fprintf('Surface measures mat (output) --> %s \n', surfaceMeasuresMatPath);
    fprintf('Mesh data mat (output) --> %s \n', meshDataMatPath);

    %% BEGIN PIPELINE
    %% STEP 1: LOAD IN DATA --> not sure if using this bio-formats function is the best way to load in data
    fprintf('\n=== STARTING STEP 1: LOAD IN DATA ===\n');
    startTime = tic;

    % Load in medial surface with thickness
    medialThickness = bfOpen3DVolume(medialSurfacePath);
    medialThickness=medialThickness{1, 1};      % Check if repetition necessary 
    medialThickness=medialThickness{1, 1};
    medialThickness=medialThickness*-1;
    medialThickness = double(medialThickness);% Convert to double for compatibility with sparse matrix ops
    %%
    % Load in mesh
    load(meshMatPath);

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 1: LOAD IN DATA (TIME ELAPSED %s) ===\n\n', elapsedTime);

    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos


    %% STEP 2: PRE-PROCESS MEDIAL SURFACE WITH THICKNESS
    fprintf('\n=== STARTING STEP 2: PRE-PROCESS MEDIAL SURFACE WITH THICKNESS ===\n');
    startTime = tic;

    % Obtain 3D coordinates of non-zero medial surface thickness points
    skelIdx = find((medialThickness) > 0);
    [rowSkel,colSkel,sliceSkel] = ind2sub(size(medialThickness),skelIdx);

    % Correct for difference in resampling (nm)
    curVoxDim = [4.1341, 4.1341, 8];
    targetVoxDim = [4, 4, 4];

    rowSkel=rowSkel*(curVoxDim(1)/targetVoxDim(1)); 
    colSkel=colSkel*(curVoxDim(2)/targetVoxDim(2)); 
    sliceSkel=sliceSkel*(curVoxDim(3)/targetVoxDim(3)); 
    skelPts = [rowSkel,colSkel,sliceSkel];

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 2: PRE-PROCESS MEDIAL SURFACE WITH THICKNESS (TIME ELAPSED %s) ===\n\n', elapsedTime);
    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos


    %% STEP 3: PROJECT THICKNESS VALUES TO MESH
    fprintf('\n=== STARTING STEP 3: PROJECT THICKNESS VALUES TO MESH ===\n');
    startTime = tic;

    % resample to mesh surface
    skelPtsTriangulation = delaunay(skelPts);
    closestIdx = dsearchn(skelPts, skelPtsTriangulation, mesh.vertices);
    medialThickness=medialThickness(skelIdx(closestIdx));

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 3: PROJECT THICKNESS VALUES TO MESH (TIME ELAPSED %s) ===\n\n', elapsedTime);
    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos


    %% STEP 4: COMPUTE PROTRUSION SCORES FOR ALL MESH VERTICES
    fprintf('\n=== STARTING STEP 4: COMPUTE PROTRUSION SCORES FOR ALL MESH VERTICES ===\n');
    startTime = tic;

    [sourceNodes, targetNodes, hopCounts, ~] = getStepVectors(triangulation2adjacency(mesh.faces, mesh.vertices), 15);
    %%
    smMat = meshGaussian(sourceNodes, targetNodes, hopCounts, 3);
    smInv = meshGaussian(sourceNodes, targetNodes, (max(hopCounts)+1-hopCounts), 1);
    localAvg = smInv*mesh.vertices;
    diffVect = mesh.vertices-localAvg;
    D = sqrt(sum(diffVect.^2, 2));

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 4: COMPUTE PROTRUSION SCORES FOR ALL MESH VERTICES (TIME ELAPSED %s) ===\n\n', elapsedTime);
    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos

    %% STEP 5: SMOOTH PROTRUSION AND THICKNESS SCORES
    fprintf('\n=== STARTING STEP 5: SMOOTH PROTRUSION AND THICKNESS SCORES ===\n');
    startTime = tic;

    % Smooth protrusion scores
    % Initialize
    smoothedDistMap = D;
    smoothingSteps = [5, 5, 10, 10, 10];
    numSteps = length(smoothingSteps);
    allProtrusionVals = zeros(size(mesh.vertices, 1), numSteps);

    fprintf('\n\nStarting protrusion distance smoothing...\n');

    % Apply smoothing iteratively
    totalIterations = 0;
    for i = 1:numSteps
        stepCount = smoothingSteps(i);
        totalIterations = totalIterations + stepCount;

        tStart = tic;
        [smoothedDistMap, changeAmount] = contSmooth(smoothedDistMap, smMat, stepCount);
        elapsed = toc(tStart);

        allProtrusionVals(:, i) = smoothedDistMap;

        fprintf('Smoothing: %d iterations done in %.2f seconds\n', totalIterations, elapsed);
        fprintf('         -> %dx smoothed result stored\n\n', totalIterations);
    end

    % Smooth thickness
    % Initialize
    smoothedThk = medialThickness;  
    allThicknessVals = zeros(size(mesh.vertices, 1), numSteps);

    fprintf('\n\nStarting thickness smoothing...\n');

    totalIterations = 0;
    for i = 1:numSteps
        stepCount = smoothingSteps(i);
        totalIterations = totalIterations + stepCount;

        tStart = tic;
        [smoothedThk, changeAmount] = contSmooth(smoothedThk, smMat, stepCount);
        elapsed = toc(tStart);

        allThicknessVals(:, i) = smoothedThk;

        fprintf('Smoothing: %d iterations done in %.2f seconds\n', totalIterations, elapsed);
        fprintf('         -> %dx smoothed result stored\n\n', totalIterations);
    end

    % Append unsmoothed values
    allProtrusionVals = [D, allProtrusionVals];
    allThicknessVals = [medialThickness, allThicknessVals];

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 5: SMOOTH PROTRUSION AND THICKNESS SCORES (TIME ELAPSED %s) ===\n\n', elapsedTime);
    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos


    %% STEP 6: SCALE SURFACE MEASURES AND COMPUTE M-SCORE
    fprintf('\n=== STARTING STEP 6: SCALE SURFACE MEASURES AND COMPUTE M-SCORE ===\n');
    startTime = tic;

    % Obtain max and min values
    [thicknessMin, thicknessMax] = bounds(allThicknessVals);
    [protrusionMin, protrusionMax] = bounds(allProtrusionVals);

    % Normalize values (THIS VERSION NOT COMPATIBLE WITH MATLAB 2019)
    % allNormThicknessVals = rescale(allThicknessVals, 0, 1, 'InputMin', thicknessMin, 'InputMax', thicknessMax);
    % allNormProtrusionVals = rescale(allProtrusionVals, 0, 1, 'InputMin', protrusionMin, 'InputMax', protrusionMax);

    % % Compute M-score
    % allNormMscoreVals = (1 - allNormThicknessVals) .* allNormProtrusionVals;
    % Manually rescale thickness and protrusion values to [0, 1]
    % Normalize each column of allThicknessVals independently to [0,1]
    thicknessMinCol = min(allThicknessVals, [], 1);  % 1 x nCols
    thicknessMaxCol = max(allThicknessVals, [], 1);
    allNormThicknessVals = (allThicknessVals - thicknessMinCol) ./ (thicknessMaxCol - thicknessMinCol);

    % Normalize each column of allProtrusionVals independently
    protrusionMinCol = min(allProtrusionVals, [], 1);
    protrusionMaxCol = max(allProtrusionVals, [], 1);
    allNormProtrusionVals = (allProtrusionVals - protrusionMinCol) ./ (protrusionMaxCol - protrusionMinCol);

    % Compute M-score
    allNormMscoreVals = (1 - allNormThicknessVals) .* allNormProtrusionVals;

    %% Save protrusion and thickness scores [0x smoothing, 5x smoothing, 10x smoothing, 20x] and M-score
    tic
    save(surfaceMeasuresMatPath, "allThicknessVals", "allProtrusionVals", ...
        "allNormThicknessVals", "allNormProtrusionVals", "allNormMscoreVals", "-v7.3");
    fprintf('Surface measures saved  in %f seconds\n', toc);
    tic

    %%
    save(meshDataMatPath, "mesh", "targetNodes", "sourceNodes", "hopCounts", "-v7.3");
    fprintf('Step vectors saved  in %f seconds\n', toc);

    elapsedTime = duration([0, 0, toc(startTime)]);
    fprintf('=== COMPLETED STEP 6: SCALE SURFACE MEASURES AND COMPUTE M-SCORE (TIME ELAPSED %s) ===\n\n', elapsedTime);
    vars = whos;
    totalMem = sum([vars.bytes]);
    fprintf('=== MEMORY: %.2f MB ===\n\n', totalMem / 1e6);
    whos
end


