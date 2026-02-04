function resampledVolume = resampleVoxelDimensions(inputVolume, voxOriginal, voxInterp)
    % Description: 
    %     -  Resamples voxel dimensions and applies 3D gaussian filter
    % Input:
    %     - inputVolume: 3D binary matrix
    %     - pxlOrig: 1x3 array --> "original" voxel dimensions
    %     - pxlNew - 1x3 array --> "target" voxel dimensions
    %
    % Outputs:
    %     - resampledVolume: 3D binary matrix with new voxel dimensions
    %
    % ------------- BEGIN CODE --------------
    szOriginal = size(inputVolume);
    szInterp = ceil((size(inputVolume)-1).*voxOriginal./voxInterp)+1; % set size of target image stack
    % define coordinates of voxels in original image stack
    [Xa,Ya,Za]=meshgrid([0:szOriginal(2)-1].*voxOriginal(2),...
                        [0:szOriginal(1)-1].*voxOriginal(1),...
                        [0:szOriginal(3)-1].*voxOriginal(3));
    % define coordinates of voxels in resampled image stack
    [Xb,Yb,Zb]=meshgrid([0:szInterp(2)-1].*voxInterp(2),...
                        [0:szInterp(1)-1].*voxInterp(1),...
                        [0:szInterp(3)-1].*voxInterp(3));
    resampledVolume = interp3(Xa,Ya,Za, single(inputVolume) ,Xb,Yb,Zb, 'nearest');

    % ------------- END OF CODE --------------
end
    
    
