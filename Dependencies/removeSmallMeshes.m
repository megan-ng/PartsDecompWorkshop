function cleanedMesh = removeSmallMeshes(mesh)
    % Description: 
    %     - Removes all mesh objects from mesh that are NOT the largest connected component.
    % Input:
    %     - mesh: face-vertex triangulated mesh struct
    %  
    % Outputs:
    %    - cleanedMesh: "cleaned" mesh that only contains largest connected mesh component
    %    - largestCCVerticesBool: 1xN logical array; N=length(mesh.vertices) 
    %       giving identity of vertices in the input mesh that are kept in the
    %       output ("cleaned") mesh; can be applied e.g. to prune other vertex
    %       data that had been calculated for the original mesh to fit the output
    %    - inputCCSizes: 1xM array of sizes of the connected components of the input mesh (M = # of connected components)
    % 
    % Other m-files required: submesh.m (usefulSnippets, matlabSnippets)
    % Author: J. Benjamin Kacerovsky
    % Centre for Research in Neuroscience, McGill University
    % email: johannes.kacerovsky@mail.mcgill.ca
    % Created: 03-Sep-2020 ; Last revision: 03-Sep-2020 
    
    % ------------- BEGIN CODE --------------
    
    % build graph from mesh
    % g=graph(triangulation2adjacency(mesh.faces, mesh.vertices));
    % method from triangulation2adjacency Toolbox Graph 2005 Gabriel Peyr�
    f=mesh.faces;
    A = sparse([f(:,1); f(:,1); f(:,2); f(:,2); f(:,3); f(:,3)], ...
               [f(:,2); f(:,3); f(:,1); f(:,3); f(:,1); f(:,2)], ...
               1.0);
    A = double(A>0);
    
    % find connected components
    g=graph(A); 
    [bins, inputCCSizes]=conncomp(g);
    [~, largestCCVerticesBool]=max(inputCCSizes);
    largestCCVerticesBool=bins==largestCCVerticesBool;
    
    % largestCCVerticesBoolect submesh --> output
    cleanedMesh=obtainSubMesh(mesh, largestCCVerticesBool);
    
    % ------------- END OF CODE --------------
end    