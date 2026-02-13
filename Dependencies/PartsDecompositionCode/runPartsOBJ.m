function [colorsRGB] = runPartsOBJ(outputOBJPath, mesh, vertexPartType, labelVals, RGB_vals)
    % runPartsOBJ writes a colored OBJ file from vertex labels and color mapping
    %
    % Parameters:
    % outputOBJPath   - Path to save the OBJ file
    % mesh            - Struct with fields 'vertices' and 'faces'
    % vertexPartType  - Array of scalar label values per vertex
    % labelVals       - Array of unique label values (same type as vertexPartType)
    % RGB_vals        - Nx3 matrix of RGB values (each row in [0,1] range), one per label

    % Check input dimensions
    if length(labelVals) ~= size(RGB_vals, 1)
        error('labelVals and RGB_vals must have the same number of entries.');
    end

    % Create value-to-color map
    valueColorMap = containers.Map('KeyType', class(labelVals(1)), 'ValueType', 'any');
    for i = 1:length(labelVals)
        valueColorMap(labelVals(i)) = RGB_vals(i, :);
    end

    % Preallocate color array
    colorsRGB = zeros(length(vertexPartType), 3);

    % Assign colors
    for i = 1:length(vertexPartType)
        key = vertexPartType(i);
        if isKey(valueColorMap, key)
            colorsRGB(i, :) = valueColorMap(key);
        else
            colorsRGB(i, :) = [0 0 0];  % Default color if key is not mapped
        end
    end

    % Write the colored mesh to OBJ
    obj_write_color(mesh, outputOBJPath, colorsRGB);
end
