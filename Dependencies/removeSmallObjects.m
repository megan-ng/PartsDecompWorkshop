function cleanedVolume = removeSmallObjects(inputVolume, volumeThreshold, returnLabeled)
    % Remove small objects from a 3D volume and optionally label components
    % Defaults to keeping only the largest component if no threshold is given

    % Find connected components
    CC = bwconncomp(inputVolume, 26);
    objSizes = cellfun(@numel, CC.PixelIdxList);
    fprintf('# of connected objects in original volume: %d\n', numel(objSizes));

    % Determine components to keep
    if nargin < 4 || isempty(volumeThreshold)
        [~, keepIdx] = max(objSizes);
    else
        keepIdx = find(objSizes >= volumeThreshold);
    end
    fprintf('# of connected objects to keep: %d\n', numel(keepIdx));

    % Generate labeled volume
    labeledVolume = labelmatrix(CC);

    % Keep only selected components
    mask = ismember(labeledVolume, keepIdx);
    cleanedVolume = labeledVolume .* cast(mask, 'like', labeledVolume);


    if nargin >= 5 && returnLabeled
        % Relabel consecutively 1:N
        uniqueLabels = unique(cleanedVolume);
        uniqueLabels(uniqueLabels == 0) = [];
        if ~isempty(uniqueLabels)
            relabelMap = zeros(max(uniqueLabels),1,'like',cleanedVolume);
            relabelMap(uniqueLabels) = 1:numel(uniqueLabels);
            nonZeroMask = cleanedVolume > 0;
            cleanedVolume(nonZeroMask) = relabelMap(cleanedVolume(nonZeroMask));
        end
    
        % Pick appropriate integer type
        nLabels = numel(uniqueLabels);
        if nLabels <= intmax('uint8')
            cleanedVolume = uint8(cleanedVolume);
        elseif nLabels <= intmax('uint16')
            cleanedVolume = uint16(cleanedVolume);
        else
            cleanedVolume = uint32(cleanedVolume);
        end
    else
        % Binary output
        cleanedVolume = uint8(cleanedVolume > 0);
    end
    
end