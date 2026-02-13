function [regionList, coreIDs, unclassIDs, expansionIDs, constrictionIDs, unvisitedIDs, someIDs, assigned] = buildRegionList(mesh, meshGraph, vertexPartType)
    % Initialize map object
    regionList = containers.Map('KeyType','int32', 'ValueType','any');
    
    regionId = 1;
    assigned = zeros(size(mesh.vertices, 1), 1); 
    
    [regionList, coreIDs, assigned, regionId] = listefy(regionList, vertexPartType, meshGraph, 0.1, assigned, regionId);
    [regionList, unclassIDs, assigned, regionId] = listefy(regionList, vertexPartType, meshGraph, 0.25, assigned, regionId);
    [regionList, expansionIDs, assigned, regionId] = listefy(regionList, vertexPartType, meshGraph, 0.5, assigned, regionId);
    [regionList, constrictionIDs, assigned, regionId] = listefy(regionList, vertexPartType, meshGraph, 1, assigned, regionId);
    [regionList, unvisitedIDs, assigned, regionId] = listefy(regionList, vertexPartType, meshGraph, 0, assigned, regionId);
    [regionList, someIDs, assigned, ~] = listefy(regionList, vertexPartType, meshGraph, 10, assigned, regionId);

    
end