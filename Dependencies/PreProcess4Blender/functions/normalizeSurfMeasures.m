function [outputArg1,outputArg2] = normalizeSurfMeasures(inputArg1,inputArg2)
    % Normalize mesh values to be [0 1]
    D = sample(i).D;
    T = sample(i).T;
    DNorm = rescale(D, 0, 1, 'InputMin', Dmin, 'InputMax', Dmax);
    TNorm = rescale(T, 0, 1, 'InputMin', Tmin, 'InputMax', Tmax);

    % Undo T inversion done by M-score computation in rescaleSurfaceMeasures_setRange_CC.m
    TNorm = 1 - TNorm;

    % M = M-score (D * T)
    MNorm = DNorm .* TNorm;

    % Assign normalized value to sample struct 
    sample(i).DNorm = DNorm;
    sample(i).TNorm = TNorm;
    sample(i).MNorm = MNorm;
end