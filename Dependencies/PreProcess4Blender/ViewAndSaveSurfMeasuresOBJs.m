%{
    Variables:
        - q1 = mesh [face vertex struct]
        - vals1 = raw surface measures
        - Dscal1 = mesh vertex protrusiveness scores (D) 
        - Tscal1 = mesh vertex thickness scores (T)
        - M1 = mesh vertex M-scores (D * T)

    Variable Notes:
        - All variables have dimensions N x 7
        - N = number of vertices
        - 7 = number of different smoothing magnitudes
            - [5x, 10x, 20x, 30x, 50x, 80x, 0x]

    Packages:
        - export .OBJ files --> https://www.mathworks.com/matlabcentral/fileexchange/72906-save-wavefront-obj-files-simple-or-colour
%}
%% Reset
clear; 
close all;

%% Declare paths
addpath(genpath("/home/meganng/projects/def-siddiqi/meganng/Astro/MATLAB/"));

%% Global variables
% directories
parentDataDir = '/home/meganng/projects/def-siddiqi/meganng/Astro/Data'; % Input data 
parentOBJ_Dir = "/home/meganng/projects/def-siddiqi/meganng/Astro/Blender/SmallMonke_OBJs/";     % Output OBJ

% amount of smoothing on input data
smoothingTimes = [5, 19, 20, 30, 50, 80, 0];
smoothing = 5;
smoothingIdx = find(smoothingTimes==smoothing);

% output mat file extension --> stores used surface measures
outputMatFileExt = sprintf("surfaceMeasuresRaw_%dxSmoothing.mat", smoothing);

%% Obtain sample names from DATA directory
% Get a list of all files and folders
parentDataDirInfo = dir(parentDataDir); 

% Filter only for sample directory names
allDirNames = {parentDataDirInfo([parentDataDirInfo.isdir]).name};
sampleNames = allDirNames(~ismember(allDirNames, {'.', '..'}));

% Obtain sample folder paths
sampleDirs = fullfile(parentDataDir, sampleNames);

%% For each sample --> Read in raw surface values and obtain min/max values for future normalization
% Initialize variable to store min and max value for D and T
Dmin = inf;
Dmax = 0;
Tmin = inf;
Tmax = 0; 

% For each sample --> declare struct fields
for i = 1:length(sampleDirs)

    % Declare current sample name and directory path
    sampleName = sampleNames{i};
    sampleDir = sampleDirs{i};

    % Store name and directory path
    sample(i).name = sampleName;
    sample(i).dirPath = sampleDir;

    % Load in surface measure values
    rawMat = strcat(sampleName, "_surfaceMeasuresRaw.mat");
    rawMatPath = fullfile(sampleDir, rawMat);
    load(rawMatPath);
    rawSurfMeasures = vals1;    % rename for clarity

    % Store surface measure values
    D = rawSurfMeasures.contSmoothedDM(:,smoothingIdx);
    T = rawSurfMeasures.contSmoothedThk(:,smoothingIdx);
    sample(i).D = D;
    sample(i).T = T;
    
    % Load in mesh
    meshMat = strcat(sampleName, "_mesh.mat");
    meshMatPath = fullfile(sampleDir, meshMat);
    load(meshMatPath);
    mesh = q;    % rename for clarity

    % Store mesh
    sample(i).mesh = mesh;

    % Update min and max D and T values (cur == current sample)
    [curDmin, curDmax] = bounds(D);
    [curTmin, curTmax] = bounds(T);

    Dmin = min(Dmin, curDmin);
    Dmax = max(Dmax, curDmax);
    Tmin = min(Tmin, curTmin);
    Tmax = max(Tmax, curTmax);

end

%% For each sample --> Normalize values and write coloured OBJ for each sample
for i = 1:length(sampleDirs)
    
    % Declare current sample
    sampleName = sample(i).name;
    sampleDir = sample(i).dirPath;
    sampleMesh = sample(i).mesh;
    
    % Normalize mesh values to be [0 1]
    D = sample(i).D;
    T = sample(i).T;
    T = Tmin + Tmax - T;        % inversion: thickness to thinness

    % Save measure values in .mat
    measuresMatFilename = sprintf("%s_%s", sampleName, outputMatFileExt);
    measuresMatPath = fullfile(sampleDir, measuresMatFilename);
    save(measuresMatPath, "D", "T");

    DNorm = rescale(D, 0, 1, 'InputMin', Dmin, 'InputMax', Dmax);
    TNorm = rescale(T, 0, 1, 'InputMin', Tmin, 'InputMax', Tmax);

    % Assign normalized value to sample struct 
    sample(i).DNorm = DNorm;
    sample(i).TNorm = TNorm;

    % Save normalized values in output mat file
    measuresMatFilename = sprintf("%s_%s", sampleName, outputMatFileExt);
    measuresMatPath = fullfile(sampleDir, measuresMatFilename);

    % Create struct for measures
    measureValues = [sample(i).DNorm, sample(i).TNorm];
    measureNames = ["Protrusiveness", "Thinness"];
    measureAbbrevs = ["D", "T"];
    measures = createSurfMeasuresStruct(measureNames, measureValues, measureAbbrevs);

    % Define special colourbar limits for T for visualization
    T_clim = [0 1];

    % Save OBJ files --> will append to 
    writeColouredOBJs(sampleName, sampleMesh, measures, parentOBJ_Dir, smoothing, T_clim);
    
    % % Plot
    % plotOverlaidSurfMeasures(sampleMesh, measures, sampleName, T_clim); 
end 

% %% Plot overlaid surface measure values on mesh
% % Declare sample number
% i = 5;
% 
% % Create struct for measures
% measureValues = [sample(i).DNorm, sample(i).TNorm];
% measureNames = ["Protrusiveness", "Thinness"];
% measureAbbrevs = ["D", "T"];
% measures = createSurfMeasuresStruct(measureNames, measureValues, measureAbbrevs);
% sampleName = sample(i).name;
% 
% % Plot
% plotOverlaidSurfMeasures(sample(i).mesh, measures, sampleName);
