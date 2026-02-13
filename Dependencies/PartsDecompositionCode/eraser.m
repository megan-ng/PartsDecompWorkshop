%% ERASER Remove small connected regions from a graph
% 
% c = eraser(g, c, val, size)
%
% This function removes small connected components from a graph that are
% labeled with a specific value.
%
% Inputs:
%   g    - Graph object representing node connectivity.
%   c    - Vector of labels for each node.
%   val  - The target label to evaluate for removal.
%   size - Minimum size threshold; components smaller than this will be removed.
%
% Outputs:
%   c    - Updated label vector with small connected components (of label 'val') removed.
%
% For all nodes labeled 'val', the function:
%   1. Extracts the subgraph of these nodes.
%   2. Finds connected components in the subgraph.
%   3. Removes (sets to 0) components smaller than 'size'.
%
% Example:
%   c = eraser(g, c, 2, 50);  % Removes all regions labeled 2 with fewer than 50 nodes
function c = eraser(g, c, val, size)
    reg = find(c == val); 
    sg = subgraph(g, reg); 
    [cc, sz] = conncomp(sg);
    
    smallRegions = find(sz<size); 

    for erase = smallRegions 
        c(reg(cc==erase)) = 0; 
    end

    if val == 0 
        regionName = "Unvisited";
    elseif val == 0.1
        regionName = "Core";
    elseif val == 0.25
        regionName = "Unclassified";
    elseif val == 0.5
        regionName = "Expansion";
    elseif val == 1
        regionName = "Constriction";
    else
        regionName = "Unknown";
    end

    fprintf('Deleted %d %s regions\n', length(smallRegions), regionName);
end

