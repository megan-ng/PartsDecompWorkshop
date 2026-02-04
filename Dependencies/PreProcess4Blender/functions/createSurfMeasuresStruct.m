function measures = createSurfMeasuresStruct(measureNames, measureValues, measureAbbrevs)
    for j = 1:size(measureValues,2)
        measures(j).name = measureNames(j);
        measures(j).abbrev = measureAbbrevs(j);
        measures(j).values = measureValues(:,j);
    end
end

