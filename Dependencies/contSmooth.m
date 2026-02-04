function [meshFunction, changeVec]=contSmooth(meshFunction, smoothingMatrix, iterations)
    % contSmooth - repeatedly applies a smoothing matrix to an input mesh
    % function 
    % 
    % Syntax:  
    %     [meshFunction, changeVec]=contSmooth(meshFunction, smoothingMatrix, iterations)
    % 
    % Inputs:
    %    meshFunction - Nx1 array defining mesh surface values to be smoothed 
    %         N = lenght(fv.vertices)
    %    smoothingMatrix - NxN smoothing matrix (e.g. result of
    %       meshGaussian.m) 
    %       (by definition #rows of meshFunction is equal to # columns of the
    %       smoothing matrix)
    %    iterations - scalar integer; number of iterations that the smoothing
    %       matrix should be applied for 
    % 
    % Outputs:
    %    meshFunction - meshFunction aftre smoothing has been applied
    %    changeVec - iterationsx1 array of change with each iteration of
    %       smoothing (each element is the total sum of changes for the
    %       meshFunction between the last and current iteration) 
    %       servees as a general estimate to monitor haw fast iterative
    %       smoothing changes the meshfunction
    % 
    
    % Author: J. Benjamin Kacerovsky
    % Centre for Research in Neuroscience, McGill University
    % email: johannes.kacerovsky@mail.mcgill.ca
    % Created: 10-Mar-2020 ; Last revision: 03-Sep-2020 
    
    % ------------- BEGIN CODE --------------
    
    
    changeVec=zeros(iterations, 1); 
    counter=0; 
    for i=1:iterations
    % while abs(cont)/sum(smMore)>0.0003
        smOld=meshFunction;
        counter=counter+1;
        meshFunction=smoothingMatrix*meshFunction;
        cont=sum(abs(meshFunction-smOld));
        changeVec(counter)=cont;
    end
    
    
    % ------------- END OF CODE --------------
    