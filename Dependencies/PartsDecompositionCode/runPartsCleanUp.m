function [cleanedVertexPartType, vertexDeletedSmall] = runPartsCleanUp(mesh, meshGraph, vertexPartType, allThinnessVals, coreQuantilePercent, sourceNodes, targetNodes, hopCounts)
    disp("Entered function");
    % Build one hop neighbourhood for growing core
    sz = size(mesh.vertices, 1);
    oneHopNeighbourhood = sparse(sourceNodes(hopCounts<=1), targetNodes(hopCounts<=1), true, sz, sz);

    % labels for astro parts
    coreVal = 0.1;
    unclassVal = 0.25;
    expansionVal = 0.5;
    constrictionVal = 1;

    % other params
    smallSize = 50;     % number of vertices of a small connected component

    %% STEP 4: CLEAN UP PART DECOMPOSITION RESULTS
    % Grow core out
    % Initialize core region vertices
    visitedCoreVertices = vertexPartType == 0.1;
    isCoreVertex = visitedCoreVertices;

    % Determine which vertices are too thin to grow core into
    thinnessThresh = quantile(allThinnessVals(:, 3), coreQuantilePercent);

    % vertex indices of assigned regions and thin regions (do NOT grow
    % core into these vertices)
    avoidVertices = vertexPartType ~= 0 | allThinnessVals(:, 3) > thinnessThresh;


    % while core can be expanded
    while sum(isCoreVertex) > 0
        % expand core front by one hop
        isCoreVertex = (max(oneHopNeighbourhood(:, isCoreVertex), [], 2));

        % do not include already visited core vertices
        isCoreVertex(visitedCoreVertices) = 0;

        % do not include assigned and thin vertices
        isCoreVertex(avoidVertices) = 0;

        % label remaining vertices as cores
        vertexPartType(isCoreVertex) = coreVal;

        % updated visited core vertices
        visitedCoreVertices(isCoreVertex) = true;
    end

    % for each type --> label vertex as unassigned (0) if it belongs to a
    % coisCoreVertexected component that falls below a certain number of vertices
    % threshold
    disp("Labelling small connected components as unassigned!")

    vertexPartType = eraser(meshGraph, vertexPartType, coreVal, 50);
    vertexPartType = eraser(meshGraph, vertexPartType, unclassVal, 50);
    vertexPartType = eraser(meshGraph, vertexPartType, expansionVal, 50);
    vertexPartType = eraser(meshGraph, vertexPartType, constrictionVal, 50);

    vertexDeletedSmall = vertexPartType;
    %% STEP 5: CREATE REGION LIST MAP (key = coisCoreVertexected component ID [integer], value = array of vertex indices within given coisCoreVertexected component)
    % Note: xxxIDs variables are used to index regionList map
    [regionList, ~, unclassIDs, ~, ~, unvisitedIDs, ~] = buildRegionList(mesh, meshGraph, vertexPartType);

    %% STEP 6: FINAL CLEAN UP STEP
    % Obtain vertex indices for expansions and constrictions
    expansionVertexIdx = find(vertexPartType==expansionVal);
    constrictionVertexIdx = find(vertexPartType==constrictionVal);

    % Obtain unsmoothed thinness values
    thinnessVals = allThinnessVals(:,1);

    % Compute average thickness for each part
    expansionAvgThickness = mean(thinnessVals(expansionVertexIdx));
    constrictionAvgThickness = mean(thinnessVals(constrictionVertexIdx));

    % Obtain all unlabelled region indices
    unlabelledRegionIDs = [unclassIDs, unvisitedIDs];

    % Counter to keep track of changes
    gulpCount = 0;
    newExp = 0;
    newCons = 0;
    newCores = 0;

    % For each unlabelled region --> assign label
    for i = 1:length(unlabelledRegionIDs)

        % Obtain vertex idx for current region
        curRegionVertexIdx = regionList(unlabelledRegionIDs(i));

        % Get 1-hop neighbors of current region
        neighborIdx = find(any(oneHopNeighbourhood(:, curRegionVertexIdx), 2))';
        neighborIdx = setdiff(neighborIdx, curRegionVertexIdx);  % Exclude current region's own vertices

        neighborVals = vertexPartType(neighborIdx);
        neighborVals = neighborVals(neighborVals > 0);  % Remove unlabelled (zero) values
        uniqueNeighbourVals = unique(neighborVals);

        % If current region is small and has neighbours with assigned values
        if length(curRegionVertexIdx) < smallSize && ~isempty(neighborVals)
            % If all neighbours are same part type --> assign this region
            % that part type
            if numel(uniqueNeighbourVals) == 1
                vertexPartType(curRegionVertexIdx) = uniqueNeighbourVals;
                gulpCount = gulpCount + 1;
                continue;  % Skip to next region

            % If neighbours are different part types --> assign this region
            % the part type that the majority of the neighbours are
            elseif numel(uniqueNeighbourVals) > 1
                % Initialize counter variables
                mostFreqTypes = [];
                maxTypeCounter = -inf;

                % For each neighbour value --> count how many occurences
                % there are and update variables above
                for j = 1:length(uniqueNeighbourVals)
                    % Count occurences
                    curNumNeighbours = nnz(neighborVals == uniqueNeighbourVals(j));

                    % If the most frequent --> update variables
                    if curNumNeighbours >= maxTypeCounter
                        mostFreqTypes = [mostFreqTypes , uniqueNeighbourVals(j)];
                        maxTypeCounter = curNumNeighbours;
                    end
                end

                % If there is one type with the most occurences --> assign
                % this region that type
                if length(mostFreqTypes) == 1
                    vertexPartType(curRegionVertexIdx) = mostFreqTypes(1);
                    gulpCount = gulpCount + 1;
                    continue;  % Skip to next region
                end
            end
        end

        % Else --> let average thickness determine type
        curAvgThickness = mean(thinnessVals(curRegionVertexIdx));

        % If curAvgThickness closer to expansionAvgThickness --> label region expansion
        if abs(curAvgThickness - expansionAvgThickness) < abs(curAvgThickness - constrictionAvgThickness)
            vertexPartType(curRegionVertexIdx) = expansionVal;
            newExp = newExp + 1;

        % If curAvgThickness closer to constrictionAvgThickness --> label region constriction
        elseif abs(curAvgThickness - expansionAvgThickness) > abs(curAvgThickness - constrictionAvgThickness)
            vertexPartType(curRegionVertexIdx) = constrictionVal;
            newCons = newCons + 1;

        % If curAvgThickness right in between the two averages --> keep region unlabelled
        else
            disp("can't decide label keep unlabelled")
            continue;
        end
    end

    % save finalized parts decomposition result
    cleanedVertexPartType = vertexPartType;
    clear vertexPartType;

    disp("exiting runPartsCleanUp");
end