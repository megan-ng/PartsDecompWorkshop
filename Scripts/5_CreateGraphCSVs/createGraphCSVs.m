%% Definition: Writes csv files containing graph connectivity information. These csvs are to be passed to an R script to create out-degree and hop distance graphs.
%{
Inputs:
    - inputDataFolder
    - outputCSVFolder

Outputs:
    - writes two csv files to outputCSVFolder
        - sampleName_coreIDs.csv = IDs of vertices assigned core label
        - sampleName_graph.csv = all source and target node pairs

Pre-requisites:
    - Input data folder structure must be the following
        - inputDataFolder/
            - sampleName/
                - sampleName_mesh.mat
                - sampleName_surfaceMeasures.mat
            - ...

%}
%% Declare global variables
inputDataFolder = "C:\Users\mcn24\Desktop\MATLAB\AstroPartsDecomp\Data_groundTruth\";
outputCSVFolder = "C:\Users\mcn24\Desktop\R\PartsDecomp\Data\Workshop\";
sampleNames = ["Astro1_groundTruth", "Astro2_groundTruth"];

%% For each sample --> write graph and core ID csv file
for i = 1:length(sampleNames)
    
    data_name = sampleNames(i);

    % Create graph representation of astrocyte parts
    fprintf("Loading: %s\n", data_name);
    sampleDir = fullfile(inputDataFolder, data_name);

    % Load required variables
    load(strcat(sampleDir, '\', data_name, '_partsDecompResults.mat'), ...
        'regionList', 'assigned', 'coreIDs');
    load(strcat(sampleDir, '\', data_name, '_meshData.mat'));

    % Build 1-hop neighbourhood sparse matrix (and add to mat file)
    sz=size(mesh.vertices, 1);
    oneHopNeighbourhood=sparse(sourceNodes(hopCounts<=1), targetNodes(hopCounts<=1), true, sz, sz);
    save(strcat(sampleDir, '\', data_name, '_meshData.mat'), "oneHopNeighbourhood", "-append");

    % Build graph with nodes and edges 
    astroGraph = buildAstroGraph(regionList, assigned, oneHopNeighbourhood);

    % Prep data for csv export
    source_nodes = astroGraph.Edges.EndNodes(:,1);
    target_nodes = astroGraph.Edges.EndNodes(:,2);

    % Write graph csv containing source and target vertex pairs
    edgeList = [source_nodes, target_nodes];
    graphCSVPath = strcat(outputCSVFolder, data_name, "_graph.csv");
    writematrix(edgeList, graphCSVPath);

    % Write csv containing core vertex IDs
    coreFileName = strcat(outputCSVFolder, data_name, "_coreIDs.csv");
    writematrix(coreIDs(:), coreFileName);

end