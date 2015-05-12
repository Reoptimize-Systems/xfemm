function pvpairs = struct2pvpairs(s)
% struct2pvpairs: converts a structure to a cell array of parameter-value
% pairs where the parameters are the field names and the values their
% contents

    fnames = fieldnames(s);
    
    pvpairs = cell(numel(s), numel(fnames) * 2);
    
    for i = 1:numel(s)
        
        pvpairs(i, 1:2:2*numel(fnames)-1) = fnames;

        pvpairs(i, 2:2:2*numel(fnames)) = struct2cell(s(i));

    end

end