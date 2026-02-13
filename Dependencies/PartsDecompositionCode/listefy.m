% function [regionList, regionIDs, assigned, regionCounter] = listefy(regionList, colBlend, g, val, assigned, regionCounter)
% 
%     vertexList = find(colBlend == val);
%     sg=subgraph(g, vertexList);
%     [cc, bs]=conncomp(sg);
% 
%     nRegions = size(bs, 2); 
% 
%     regionIDs = [];  
%     for i=1:nRegions
%         regionVertices = vertexList(cc==i); 
%         regionList(regionCounter) = regionVertices;
%         assigned(regionVertices) = regionCounter; 
%         regionIDs(i) = regionCounter;
%         regionCounter = regionCounter + 1;
%     end
% 
% end
function [regionList, regionIDs, assigned, regionCounter] = listefy(regionList, colBlend, g, val, assigned, regionCounter)
%LISTEFY Identifies connected components among vertices with a specific label/value, and stores them as separate regions.
%
% Inputs:
%   - regionList: containers.Map where keys are region IDs (integers), and values are arrays of vertex indices.
%   - colBlend: vector of values (e.g., vertex labels/types), same length as number of vertices in the graph.
%   - g: graph object representing the mesh or connectivity of the structure.
%   - val: target value in colBlend to process (e.g., 0.1 for core, 0.25 for unclassified, etc.).
%   - assigned: vector (same size as number of vertices) storing assigned region ID (0 if unassigned).
%   - regionCounter: integer counter to track and assign unique region IDs.
%
% Outputs:
%   - regionList: updated containers.Map with new region entries added.
%   - regionIDs: array of new region IDs assigned during this call.
%   - assigned: updated vector showing the region assignment for each vertex.
%   - regionCounter: updated counter for unique region IDs.

    % Get indices of vertices that match the desired label/value
    vertexList = find(colBlend == val);

    % Extract subgraph made only of those selected vertices
    sg = subgraph(g, vertexList);

    % Identify connected components in the subgraph
    [cc, bs] = conncomp(sg);  % cc: component ID for each node in sg

    nRegions = size(bs, 2);   % Number of distinct connected components found

    regionIDs = zeros(1, nRegions);  % Initialize output array for region IDs
    
    % For each connected component found 
    for i = 1:nRegions
        % Get original indices of the vertices belonging to the i-th component
        regionVertices = vertexList(cc == i);

        % Store these vertices in the regionList map using regionCounter as the key
        regionList(regionCounter) = regionVertices;

        % Update the assigned vector so we know these vertices are now assigned
        assigned(regionVertices) = regionCounter;

        % Save the region ID
        regionIDs(i) = regionCounter;

        % Increment the counter for the next region
        regionCounter = regionCounter + 1;
    end

end
