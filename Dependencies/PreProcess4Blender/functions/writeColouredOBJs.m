function measureValues = writeColouredOBJs(sampleName, mesh, measures, OBJ_dir, smoothing, T_clim)
    % Set colour bar limits
    minVal = 0;
    maxVal = 1;

    
    for i = 1:length(measures)
        % declare current measure fields
        measureValues = measures(i).values;
        measureName = measures(i).name;
        measureAbbrev = measures(i).abbrev;

        % use alternate colourbar limits if measure is thinness
        if measureName == "Thinness"
            minVal = T_clim(1);
            maxVal = T_clim(2);
        end

        % create output directories
        OBJ_subDir = fullfile(OBJ_dir, sampleName, measureName);

        % if obj directory exists --> empty it to prevent appending to .obj
        % and .mtl file
        if exist(OBJ_subDir, "dir")
            fprintf("Deleting existing files in: %s \n", OBJ_subDir);
            deleteFiles = sprintf("%s/*", OBJ_subDir);
            delete(deleteFiles);
        else
            fprintf("Creating directory: %s \n", OBJ_subDir);
            mkdir(OBJ_subDir);
        end

        % create function parameters
        OBJ_filename = sprintf("%s_%dxSmooth", measureAbbrev, smoothing);
        OBJ_path = fullfile(OBJ_subDir, OBJ_filename);
        OBJ_path = convertStringsToChars(OBJ_path);

        % write obj file
        obj_write_color(mesh, OBJ_path, measureValues, 'colorMap', 'parula', 'cmin', minVal, 'cmax', maxVal);
    end
end

