% function [astroGraph] = buildAstroGraph(regionList, assigned, NN)
% 
%     % Initialize graph
%     astroGraph = graph(); 
%     nList = cell(1, length(regionList.keys)); 
% 
%     % For each region connected component in regionList --> obtain region
%     % neighbours and define edges between them
%     for i = 1:length(regionList.keys)
% 
%         % Obtain logical array (true (1) for all vertices assigned to region i) 
%         s = assigned == i; 
% 
%         % Obtain logical array that highlights vertex neighbours of region i
%         nn = (max(NN(:, s), [], 2));
% 
%         % Discard any vertices that are in region i
%         nn(s) = 0; 
% 
%         % Obtain indices of vertex neighbours of region i
%         nn = find(nn); 
% 
%         % Obtain region IDs of neighbouring vertices
%         neighbours = unique(assigned(nn)); 
% 
%         % Filter out neighbouring vertices that are unassigned 
%         neighbours = neighbours(neighbours > 0 & neighbours ~= i);
% 
%         % Add edge between region i and all its neighbouring regions to graph 
%         astroGraph = astroGraph.addedge(repmat(i, length(neighbours), 1), neighbours);
%         nList{i} = neighbours; 
%     end
% 
%     astroGraph = astroGraph.simplify();
% end


function [astroGraph] = buildAstroGraph(regionList, assigned, NN)
    
    % Initialize graph
    astroGraph = graph(); 
    nList = cell(1, length(regionList.keys)); 
    
    % For each region connected component in regionList --> obtain region
    % neighbours and define edges between them
    for i = 1:length(regionList.keys)
    
        % Obtain logical array (true (1) for all vertices assigned to region i) 
        s = assigned == i; 
    
        % Obtain logical array that highlights vertex neighbours of region i
        nn = (max(NN(:, s), [], 2));

        % Discard any vertices that are in region i
        nn(s) = 0; 
        
        % Obtain indices of vertex neighbours of region i
        nn = find(nn); 
    
        % Obtain region IDs of neighbouring vertices
        neighbours = unique(assigned(nn)); 
    
        % Filter out neighbouring vertices that are unassigned 
        neighbours = neighbours(neighbours > 0 & neighbours ~= i);

        if isempty(neighbours)
            % Region has no neighbours → report how many vertices it contains
            numVerts = sum(s);
            fprintf('Region %d has no neighbours and contains %d vertices.\n', i, numVerts);
        else
            % Add edge between region i and all its neighbouring regions to graph 
            astroGraph = astroGraph.addedge(repmat(i, length(neighbours), 1), neighbours);
        end

        nList{i} = neighbours; 
    end

    astroGraph = astroGraph.simplify();
end

