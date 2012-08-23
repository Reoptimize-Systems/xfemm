function [id, xycoords] = findnode_mfemm(FemmProblem, loc)
% findnode_mfemm: finds the node nearest a given location


    nodecoords = getnodecoords_mfemm(FemmProblem);
    
    % find the nearest node to the location
    % ipdm returns a structure with fields named 'rowindex',
    % 'columnindex', and 'distance'.
    result = ipdm(loc, nodecoords, 'Result', 'Structure', 'Subset', 'NearestNeighbor');
    
    % get the indices of the nodes and subtract 1 to make zero based
    id = result.columnindex - 1;
    
    xycoords = cat(1, FemmProblem.Nodes(id+1).Coords);

end