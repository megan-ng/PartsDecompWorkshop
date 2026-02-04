function decimatedMesh = decimateMesh(mesh, keepPercentage)
    % Description: 
    %     -  Reduces number of vertices/edges/faces of input mesh while maintaining overall shape and form of object
    % Input:
    %     - mesh: face-value struct
    %     - keepPercentage: percentage of triangles to keep
    %
    % Outputs:
    %     - decimatedMesh: mesh decimated by specified keepPercentage
    %
    % ------------- BEGIN CODE --------------

    [mesh.vertices, mesh.faces] = decimate_libigl(mesh.vertices, mesh.faces, keepPercentage);
    decimatedMesh = isoSwitch(mesh);
    
    % ------------- END OF CODE --------------
end