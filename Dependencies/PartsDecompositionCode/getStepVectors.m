function [idxRow, idxCol, steps, s]=getStepVectors(adjacency, neighbourhoodSize)
    % getStepVectors - builds neighbourhoods from adjacency matrix
    % neighbourhoods aree defined by steps (not exact distance)
    % ressults are saved as vectors which can be used to define a sparse
    % neighbouthood matrix
    %
    % I have built and tested a number of different methods for building
    % neighbourhoods for each veertex on a mesh. Ultimately this approach using
    % sparse matrix multiplications and operations was the most efficient
    % method of the approaches I tried, in particular for large meshes
    % (>>1e+06 vertices) (other methods were actually faster for very small
    % meshes). 
    % Matlab is optimized for matrix operations, therfore trying to have as
    % much of the heavy computations performed as matrix operations is very
    % efficient. 
    % writing the results to 1D arrays was the most efficient method I found,
    % which would also allow me to preserve the step value, both for computing
    % and read/write of the results (repeatedly writing individual values to 
    % sparse arrays is very costly). The find call in each loop is costly but
    % after all trad-offs this was more efficient than any alternative I found.
    %
    % the output vectors define a symmetric (M=M') sparse stepwise neighbour
    % matrix (NeighbourMatrix=sparse(IDXCol, IDXRow, Step)) 
    % 
    % Syntax:  
    %     [idxRow, idxCol, steps, s]=getStepVectors(adjacency, neighbourhoodSize)
    % 
    % Inputs:
    %    adjacency - sparse logical adjacency matrix of mesh 
    %    neighbourhoodSize - scalar integer; how many steps of neighbourhood
    %    should be calculated
    % 
    % Outputs:
    %    idxRow - vector defining row coordinates of neighbourhood sparse
    %       matrix
    %    idxCol - vector defining column coordinates of neighbourhood sparse
    %       matrix
    %    steps – vector defining step  values for each element of the
    %       neighbourhood (i.e. how many steps to get to this point from)
    %    s - final logical array of neighbourhood growth (in most situations
    %       this value does not need to be saved) 
    % 
    % Comments: 
    %    %  Building adjacency matrix
    %    %  method from triangulation2adjacency Toolbox Graph 2005 Gabriel Peyr
    %    %  (triangulation2adjacency(FV.faces, FV.vertices))
    %         f=FV.faces;
    %         A = sparse([f(:,1); f(:,1); f(:,2); f(:,2); f(:,3); f(:,3)], ...
    %                    [f(:,2); f(:,3); f(:,1); f(:,3); f(:,1); f(:,2)], ...
    %                    1.0);
    % 
    
    
    % Author: J. Benjamin Kacerovsky
    % Centre for Research in Neuroscience, McGill University
    % email: johannes.kacerovsky@mail.mcgill.ca
    % Created: 15-Apr-2020 ; Last revision: 03-Sep-2020 
    
    % ------------- BEGIN CODE --------------
    
    % initialize matrices (nn is the "growing" matrix, i.e. points assigned to
    % a neighbourhood in the last round -> neighbours of nn should be included
    % in the next step) 
    adjacency=logical(adjacency); 
    nn=adjacency; 
    % identity is set to one since each point is should be a member of its
    % neighbourhood 
    % s keeeps track of all elements, which have already been assigned to a
    % neighbourhood
    s=(nn+speye(size(nn, 1), size(nn, 1)))>0;
    
    % preallocate memory and inizialize values
    idxRow=1:size(adjacency, 1);
    idxCol=idxRow; 
    steps=zeros(size(idxRow));    % adding zeros step values for gaussian
    % adjaceny matrix is equivalent to a 1-step neighbourhood -> assigin nn to
    % results matrix 
    [a, b]=find(nn); 
    l=length(a); 
    % thee counter is used to keep track of the position of the last assigned
    % value in the results vectors (more efficient than using e.g. 
    % idxRow(end+1:end+l), which would grow the vector with each loop, matlab
    % likes memory pre-allocation) 
    counter=length(idxRow);
    idxRow(counter+1:counter+l)=a; 
    idxCol(counter+1:counter+l)=b;
    steps(counter+1:counter+l)=1; 
    counter=counter+l;
    
    % since the adjacency itself is thee "1-step" neighbourhood, we start
    % growing at step 2
    for k=2:neighbourhoodSize
        tic
        % grow nn by one step by finding the neighbours of current nn 
        nn=((adjacency*nn)>0);
        % remove elements that have already been assigned to the 
        nn(s)=0;  
        
        % nn to s (logical OR) 
        s=nn|s;
        
        % find the elments reacheed in this step and write to results veectors
        [a, b]=find(nn); 
        l=length(a); 
        idxRow(counter+1:counter+l)=a; 
        idxCol(counter+1:counter+l)=b;
        steps(counter+1:counter+l)=k; 
      
        counter=counter+l;
        
        fprintf('finished iteration: %d in %f seconds\n', k, toc);
    end
end
    
    % ------------- END OF CODE --------------
    