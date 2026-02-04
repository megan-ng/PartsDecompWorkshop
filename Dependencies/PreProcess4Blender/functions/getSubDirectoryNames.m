function [subDirectoryNames] = getSubDirectoryNames(parentDirPath)
    parentDataDirInfo = dir(parentDirPath); % Get a list of all files and folders

    % Filter only for sample directory names
    allDirNames = {parentDataDirInfo([parentDataDirInfo.isdir]).name};
    subDirectoryNames = allDirNames(~ismember(allDirNames, {'.', '..'}));
end