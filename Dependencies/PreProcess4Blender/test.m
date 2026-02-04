%% viewing subset mesh






%% testing vars
% directories
parentDataDir = '/Users/meganng/MATLAB/ER-Analysis/DATA'; % Input data 

% Get a list of all files and folders
parentDataDirInfo = dir(parentDataDir); 

% Filter only for sample directory names
allDirNames = {parentDataDirInfo([parentDataDirInfo.isdir]).name};
sampleNames = allDirNames(~ismember(allDirNames, {'.', '..'}));


%% testing WT csvs


i = 3;
sampleName = sampleNames{i};
csvPath = sprintf("/Users/meganng/R/ER-Analysis/DATA_renamed/%s_EF_AD.csv", sampleName);
meshPath = sprintf("%s/%s/%s_mesh.mat", parentDataDir, sampleName, sampleName);


sampleTable = readtable(csvPath);
load(meshPath);

T = sampleTable.T_raw_smooth5x;
D = sampleTable.D_raw_smooth5x;
mesh = q;


measureValues = [D, T];
measureNames = ["Protrusiveness", "Thinness"];
measureAbbrevs = ["D", "T"];
measures = createSurfMeasuresStruct(measureNames, measureValues, measureAbbrevs);
T_clim = [0.7 1];


% Plot
plotOverlaidSurfMeasures(mesh, measures, sampleName, T_clim);


%% tesing AD csvs

% filter for AD surface measures
matPattern = "wt*_data.mat";

% Obtain subsample obj struct: contains filename of obj and its folder
surfaceMeasuresMats = dir(fullfile(parentDataDir,'**',matPattern)); 
fields2remove = fieldnames(surfaceMeasuresMats);
fields2remove = fields2remove(3:end);
surfaceMeasuresMats = rmfield(surfaceMeasuresMats, fields2remove);

i = 1;
matPath = fullfile(surfaceMeasuresMats(i).folder, surfaceMeasuresMats(i).name);
nameParts = split(surfaceMeasuresMats(i).name, "_");
sampleName = strjoin(nameParts(1:end-2), '_'); % Join all but the last part


load(matPath);

measureValues = [subD, subT];
measureNames = ["Protrusiveness", "Thinness"];
measureAbbrevs = ["D", "T"];
measures = createSurfMeasuresStruct(measureNames, measureValues, measureAbbrevs);
T_clim = [0.7 1];

% create submesh
meshPath = sprintf("%s/%s/%s_mesh.mat", parentDataDir, sampleName, sampleName);
load(meshPath);
originalMesh = q;

subMesh = createSubMesh(originalMesh, subsetLogical);

% Plot
plotOverlaidSurfMeasures(subMesh, measures, sampleName, T_clim);

%% testing indexing
length(D)

length(mesh.vertices)
%% thick to thin
D = [2, 4, 1, 8, 10];
Dmin = min(D);
Dmax = max(D);

DNorm = rescale(D, 0, 1, 'InputMin', Dmin, 'InputMax', Dmax);
DRevNorm1 = 1 - DNorm;

Drev = Dmin + Dmax - D;
DRevNorm2 = rescale(Drev, 0, 1, 'InputMin', Dmin, 'InputMax', Dmax);
