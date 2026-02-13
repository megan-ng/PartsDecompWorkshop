function slope=slidingDerivative(data, wIndow)
% slidingDerivative - performs local linear fit to estimate the slope of
% the input function at every point. the linear fit is calculated over a
% user defined sliding window
% 
% Syntax:  
%     slope=slidingDerivative(data, wIndow)
% 
% Inputs:
%    data - 1xN array of input data
%    wIndow - scalar integer; defines the window size; the linear
%       regression will be performed over a subset of the input data from
%       -wIndow:+wIndow for each element of the input array (i.e. the
%       linear regression is performed for a subset of size 2*wIndow+1)
% 
% Outputs:
%    slope - 1xN array; same size as data input. Results of local linear
%    regression. 
% 

% Author: J. Benjamin Kacerovsky
% Centre for Research in Neuroscience, McGill University
% email: johannes.kacerovsky@mail.mcgill.ca
% Created: 20-Mar-2020 ; Last revision: 03-Sep-2020 

% ------------- BEGIN CODE --------------

    % pad the array to avoid edge errors
    data = padarray(data, [0, wIndow+2], 'replicate'); 
    
    % reformat input "data" as y of the multiple linear regression
    % each column of y corresponds to on "sliding window"
    y=zeros(2*wIndow+1, length(data));
    for i=(1+wIndow):length(data)-(wIndow+1)
       y(:, i)=data(i-wIndow:i+wIndow);
    end
    
    % solve least squares polynomial regression (we are using a 1st order
    % polynomial i.e. linear regression) 
    % see https://en.wikipedia.org/wiki/Polynomial_regression
    x=[ones(2*wIndow+1, 1), (-wIndow:wIndow)'];
    B=(x'*x)^-1*x'*y;
    
    % B contains the polynomial coefficients (ß in y=ß0+x*ß1+...)
    % B(1, :) = ß0 (i.e. intercept) 
    % B(2, :) = ß1 --> slope
    slope=B(2, wIndow+3:end-wIndow-2);
end


% ------------- END OF CODE --------------
