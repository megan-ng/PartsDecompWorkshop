function[subMesh] = createSubMesh(originalMesh, subMeshVerticesIdx)
%subMeshVerticesIdx is the entry in the subMeshVerticesIdx list that contains the part of interest 
% (ie the indices of the subMesh)
% originalMesh is the entire mesh

V = originalMesh.vertices(subMeshVerticesIdx,:);
inverts = ismember(originalMesh.vertices, V,'rows');
infaces = inverts(originalMesh.faces(:,1)) & inverts(originalMesh.faces(:,2)) & inverts(originalMesh.faces(:,3));
new_faces = originalMesh.faces(infaces, :);

V = originalMesh.vertices(inverts,:);

%%Fix faces
n2o = find(inverts);
for i=1:numel(new_faces)
    new_faces(i) = find(n2o==new_faces(i));
end

subMesh.vertices = V;
subMesh.faces = new_faces;

end