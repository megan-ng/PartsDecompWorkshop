%% Description: Create objs for astrocyte and the glycogen granules for all WT and AD samples
function createMesh(tifPath, objPath, libraryPath)
    % Declare path to parts decomposition library
    addpath(genpath(libraryPath));

    % Ensure that paths are chars
    tifPath = char(tifPath);
    objPath = char(objPath);

    % Create output mesh mat path
    matFilePath = replace(tifPath, '.tif', '_mesh.mat');

    % Other params
    volumeThreshold = 50;
    returnLabeled = false;

    % BEGIN PIPELINE
    % STEP 1: LOAD IN DATA --> not sure if using this bio-formats function is the best way to load in data
    fprintf('\n=== STARTING STEP 1: LOAD IN DATA ===\n');
    imgVolume = bfOpen3DVolume(tifPath);
    imgVolume = imgVolume{1,1}{1,1};
    imgVolume = imgVolume > 0;  % convert to binary

    % STEP 2: REMOVE SMALL OBJECTS
    fprintf('=== STARTING STEP 2: REMOVE SMALL OBJECTS ===\n');
    imgVolume = removeSmallObjects(imgVolume, volumeThreshold, returnLabeled);

    % STEP 3: RESAMPLE VOXEL DIMENSIONS
    fprintf('=== STARTING STEP 3: RESAMPLE VOXEL DIMENSIONS ===\n');
    % Declare current and target voxel dimensions (nm)
    curVoxDim = [4.1341, 4.1341, 8];
    targetVoxDim = [4, 4, 4];
    imgVolume = resampleVoxelDimensions(imgVolume, curVoxDim, targetVoxDim);


    % STEP 5: PAD VOLUME TO PREPARE FOR MESH GENERATION
    fprintf('=== STARTING STEP 5: PAD VOLUME TO PREPARE FOR MESH GENERATION  ===\n');
    padAmount = 3;
    imgVolume = padarray(imgVolume, [padAmount, padAmount, padAmount], 0);
    imgVolume(isnan(imgVolume)) = 0;


    % STEP 6: GENERATE MESH
    fprintf('=== STARTING STEP 6: GENERATE MESH  ===\n');
    mesh = isosurface(imgVolume, 0.5);   % Creates mesh surface at the border between 0 and 1 pixels (0.5)
    mesh.vertices = mesh.vertices - padAmount;


    % STEP 7: POST-PROCESS MESH
    fprintf('=== STARTING STEP 7: POST PROCESS MESH  ===\n');
    mesh = removeSmallMeshes(mesh); % Remove small mesh fragments (Only keep largest connected component)
    [mesh.vertices, mesh.faces] = checkAndRepairMesh(mesh.vertices, mesh.faces);    % Check and repair any other mesh


    % STEP 8: DECIMATE MESH
    fprintf('=== STARTING STEP 8: DECIMATE MESH  ===\n');
    keepPercentage = 0.01;
    mesh = decimateMesh(mesh, keepPercentage);


    % STEP 9: WRITE RESULTS
    fprintf('=== STARTING STEP 9: WRITE RESULTS  ===\n');
    fprintf('Saving mesh to matfile...\n')
    save(matFilePath, 'mesh', '-v7.3');
    fprintf('Mesh saved to matfile: %s\n')

    fprintf('Writing OBJ...\n');
    obj_write(mesh, objPath);
    fprintf('Wrote obj to: %s\n', objPath);

end