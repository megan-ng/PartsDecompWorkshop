function writeTif(tifStack,outputFilePath)
    if length(size(tifStack))==3
        for i = 1:size(tifStack,3)
            imwrite(squeeze(tifStack(:,:,i)), outputFilePath, ...
                    'WriteMode', 'append');
        end
    elseif  length(size(tifStack))==4
        for i = 1:size(tifStack,3)
            imwrite(squeeze(tifStack(:,:,i,:)), outputFilePath, ...
                    'WriteMode', 'append');
        end
    end 
end 