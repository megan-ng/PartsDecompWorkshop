function smoothingMatrix=meshGaussian(IDXRow, IDXCol, vals, sigma)
    % meshGaussian - build a sparse gaussian smoothing matrix from the results
    % of "getStepVectors.m"
    % the smoothingMatrix can be used to apply gaussian smoothing to a mesh
    % surface (gaussian kernal along the mesh surface).  
    % smoothingMatrix is the input for contSmooth.m
    % 
    % Syntax:  
    %     smoothingMatrix=meshGaussian(IDXRow, IDXCol, vals, sigma)
    % 
    % Inputs:
    %    IDXRow - IDXRow output form getStepVectors.m
    %    IDXCol - IDXCol output form getStepVectors.m
    %    vals - Step output form getStepVectors.m
    %    sigma - sigma for gaussian kernel 
    % 
    % Outputs:
    %    smoothingMatrix - sparse matrix for mesh smoothing. Same format as the
    %       neighbourhood matrix defined by getStepVectors.m; Step values have
    %       been recalculated to define smoothing weights following a gaussian
    %       distribution with sigma.
    % 
    
    % Author: J. Benjamin Kacerovsky
    % Centre for Research in Neuroscience, McGill University
    % email: johannes.kacerovsky@mail.mcgill.ca
    % Created: 03-Sep-2020 ; Last revision: 03-Sep-2020 
    
    % ------------- BEGIN CODE --------------
    
    sz=max(IDXRow);
    vals=((1/2*pi)*sigma^2)*exp(-(vals.^2)./(2*sigma^2));
    smoothingMatrix=sparse(IDXRow, IDXCol, vals, sz, sz);
    smoothingMatrix = diag(sum(smoothingMatrix,2).^(-1)) * smoothingMatrix;
    end
    
    
    % ------------- END OF CODE --------------
    