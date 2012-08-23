function [id, xycoords] = findblocklabel_mfemm(FemmProblem, loc)
% findblocklabel_mfemm: finds the block label nearest a given location

    labelcoords = getblocklabelcoords_mfemm(FemmProblem);
    
    % find the nearest node to the location
    % ipdm returns a structure with fields named 'rowindex',
    % 'columnindex', and 'distance'.
    result = ipdm(loc, labelcoords, 'Result', 'Structure', 'Subset', 'NearestNeighbor');
    
    % get the indices of the nodes and subtract 1 to make zero based
    id = result.columnindex - 1;
    
    % return the actual coordinates of the block label
    xycoords = cat(1, FemmProblem.BlockLabels(id+1).Coords);

end