 % variables
inputDir = "/Users/meganng/Documents/McGill/PhD/siddiqi lab/Projects/ER Analysis/inputData/";
outputDir = fullfile(inputDir, "croppedVolumes/");

% make dir if needed
if ~exist("outputDir", "dir")
    mkdir(outputDir);
end

% obtain file names in folder
fileInfos = dir(strcat(inputDir,'*.tif'));
filePaths = {};
for i = 1:length(fileInfos)
    filePath = strcat(fileInfos(i).folder, "/", fileInfos(i).name);
    filePaths{i} = filePath;
    disp(filePath);
end

% choose which image to operate on
% crop and save one
curFilePath = filePaths{1};
curStackInfo = imfinfo(curFilePath);

% cropping params
% declare cropping indices
% height
rowMin = 1;
rowMax = 700;
% rowMax = floor(curStackInfo(1).Height/2);

% width
colMin = floor(curStackInfo(1).Width/2);
colMax = 2400;

% z
zMin = 1;
zMax = 50;

% create final params for cropping function
rowIndices = [rowMin rowMax];
colIndices = [colMin colMax];
zIndices = [zMin zMax];

% read in cropped volume and create new file name
% read in 
croppedZ = tiffreadVolume(curFilePath, "PixelRegion", {rowIndices, colIndices, zIndices});

% create new file name
outputFilePath = strcat(outputDir, fileInfos(1).name);
newExt = sprintf("_x%d-%d_y%d-%d_z%d-%d.tif", rowMin, rowMax, colMin, colMax, zMin, zMax);
outputFilePath = strrep(outputFilePath, ".tif", newExt);
fprintf("output file name: %s", outputFilePath);

% write final tiff
writeTif(croppedZ, outputFilePath);