function varargout = checkgeom_mfemm(FemmProblem, tol, dodisplay)
% checks a FemmProblem for comon problems such as nodes on top of each
% other, labels on top of nodes etc.
%
% Syntax
%
% problems = checkgeom_mfemm(FemmProblem)
% problems = checkgeom_mfemm(..., tol)
% problems = checkgeom_mfemm(..., dodisplay)
% checkgeom_mfemm(...)
%
% Description
%
% checkgeom_mfemm tests for various problems due to geometric entities
% being too close together, within a given tolerance.
% checkgeom_mfemm(FemmProblem) tests for the following problems:
%
%  * nodes too close to one another
%  * nodes too close to labels
%  * nodes too close to segments of which they are not a vertex
%  * labels too close to one another
%  * labels too close to segments
%  * intersecting/overlapping segments
%
% By default checkgeom_mfemm uses a tolerance of 1e-5 m, this can be
% changed by supplying the 'tol' argument with an alternative tolerance. If
% this is supplied and is empty the default tolerance is used. 
%
% If no output arguments are supplied, checkgeom_mfemm prints the results
% of the tests on the command line. Otherwise the results are silently
% returned in a structure with the following fields:
%
% nodes: [n x 3 matrix], first two columns are the indices of the
%   overlapping nodes, third is the distance between them.
%
% labels: [n x 3 matrix], first columns are the indices of the
%   overlapping labels, third is the distance between them.
%
% labelsandnodes: [n x 3 matrix], first column is the indices of the
%   overlapping labels, second is the indices of the nodes, third is the
%   distance between them.
%
% intersectingsegments: [n x 1 matrix], first two columns are the indices
%   of the overlapping/intersecting segments, third is the distance between
%   them.
%
% labnearsegs: [n x 3 matrix], first column is the indices of labels
%   the second is the indices of segments, third is the distance between
%   them.
%
% nodesnearsegs: [n x 3 matrix], first column is the indices of nodes
%   the second is the indices of segments, third is the distance between
%   them.
%
% Display of the results at the command line can be forced by supplying
% dodisplay and setting its value to true.
%
% 

    if nargin < 2 || isempty(tol)
        tol = metres2length_mfemm(1e-5, FemmProblem.ProbInfo.LengthUnits);
        
        tol = tol + eps(tol);
    end
    
    if nargin < 3
        dodisplay = false;
    end
    
    nodecoords = getnodecoords_mfemm(FemmProblem);
    labelcoords = getblocklabelcoords_mfemm(FemmProblem);
    segcoords = getsegnodecoords_mfemm(FemmProblem);
    
    % check for nodes within a given tolerance of each other
    
    % find any distances between all nodes less than the specified
    % tolerance
    d = mfemmdeps.ipdm(nodecoords, 'Subset', 'Maximum', 'Limit', tol, 'Result', 'Structure');

    problems.nodes = [d.rowindex, d.columnindex, d.distance];
    % strip the self referencing indices
    problems.nodes = problems.nodes(problems.nodes(:,1) ~= problems.nodes(:,2), :);
    
    foundduplicates = ~isempty(problems.nodes);
    
    while foundduplicates
        
        duplicates = [];
        
        flippednodes = fliplr (problems.nodes(:,1:2));
        for ind = 1:size (problems.nodes,1)
            for indii = 1:size (flippednodes,1)
                if indii ~= ind
                    if all(problems.nodes(ind,1:2) == flippednodes(indii,1:2))
                        duplicates = [duplicates, indii];
                    end
                end
            end
            foundduplicates = ~isempty(duplicates);
            
            if foundduplicates
                break;
            end
            
        end
        
        % delete the duplicates
        problems.nodes(duplicates,:) = [];
        
    end
    
    % check for labels within a given tolerance of each other
    d = mfemmdeps.ipdm(labelcoords, 'Subset', 'Maximum', 'Limit', tol, 'Result', 'Structure');

    problems.labels = [d.rowindex, d.columnindex, d.distance];
    % strip the self referencing indices
    problems.labels = problems.labels(problems.labels(:,1) ~= problems.labels(:,2), :);
    
    % check for labels overlapping nodes
    d = mfemmdeps.ipdm(labelcoords, nodecoords, 'Subset', 'Maximum', 'Limit', tol, 'Result', 'Structure');

    problems.labelsandnodes = [d.rowindex, d.columnindex, d.distance];
    
    % check for intersecting segments
    if numel(FemmProblem.Segments) > 1
        problems.intersectingsegments = getintersectingsegments(FemmProblem, segcoords);
    else
        problems.intersectingsegments = [];
    end
    
    % check for intersecting arc segments
    
    % TODO
    
    % check for labels near segments
    problems.labnearsegs = labelsnearsegments(labelcoords, segcoords, tol);
    
    % check for labels near arc segments
    
    % TODO
  
    % check for unconnected nodes near segments
    problems.nodesnearsegs = unconnnodesnearsegments(FemmProblem, nodecoords, segcoords, tol);
    
    % check for unconnected nodes near arc segments
    
    % TODO
    
    if nargout < 1
        displayproblems(problems, tol);
    else
        varargout{1} = problems;
        if dodisplay
            displayproblems(problems, tol);
        end
    end
    
end

function labnearsegs = labelsnearsegments(labelcoords, segcoords, tol)
% checks for labels near the segments

    labnearsegs = [];
    
    for ind = 1:size(labelcoords, 1)
        for iind = 1:size(segcoords, 1)

            d = point_to_line_2D(labelcoords(ind,:), segcoords(iind,1:2), segcoords(iind,3:4));

            if d <= tol
                % store                      label   seg    distance
                labnearsegs = [ labnearsegs; ind-1, iind-1, d ];
            end

        end
    end

end

function d = point_to_line_2D (x, a, b)

%     p = p(:)';
%     l1 = l1(:)';
%     l2 = l2(:)';
%     
%     d = abs( det([p-l1;l2-l1]) ) / norm(l2-l1);

%     x = [0,0]; %some point
%     a = [1,2]; %segment points a,b
%     b = [3,5];

    d_ab = norm(a-b);
    d_ax = norm(a-x);
    d_bx = norm(b-x);

    if dot(a-b,x-b)*dot(b-a,x-a) >= 0
        
        A = [ a,1; 
              b,1; 
              x,1 ];
          
        d = abs(det(A))/d_ab;        
        
    else
        d = min(d_ax, d_bx);
    end

end

function nodesnearsegs = unconnnodesnearsegments(FemmProblem, nodecoords, segcoords, tol)
% checks for node coords near segments of which the node is not a part

    nodesnearsegs = [];
    
    for ind = 1:size(nodecoords, 1)
        for iind = 1:size(segcoords, 1)
            if (FemmProblem.Segments(iind).n0 ~= ind-1) && (FemmProblem.Segments(iind).n1 ~= ind-1)
                % if node is not connected to the segment
                d = point_to_line_2D(nodecoords(ind,:), segcoords(iind,1:2), segcoords(iind,3:4));

                if d <= tol
                    % store                          node    seg  distance
                    nodesnearsegs = [ nodesnearsegs; ind-1, iind-1, d ];
                end
            end
        end
    end

end

function intersectingsegments = getintersectingsegments(FemmProblem, segcoords)
% finds segments which intersect or are coincident
%
     
    out = mfemmdeps.lineSegmentIntersect(segcoords,segcoords);
    
    [intersectingsegments,intersectingsegments(:,2)] = find(out.intAdjacencyMatrix);
    
    intersectingsegments = intersectingsegments - 1;
    
    intersectingsegments(:,3) = out.coincAdjacencyMatrix(out.intAdjacencyMatrix);
    
    % remove any segments which are connected to the same node at either
    % end as they will intersect at this point
    removeentry = [];
    for ind = 1:size(intersectingsegments, 1)
        
        if any(FemmProblem.Segments(intersectingsegments(ind,1)+1).n0 ...
                == [FemmProblem.Segments(intersectingsegments(ind,2)+1).n0, ...
                    FemmProblem.Segments(intersectingsegments(ind,2)+1).n1] ) ...
           || any(FemmProblem.Segments(intersectingsegments(ind,1)+1).n1 ...
                == [FemmProblem.Segments(intersectingsegments(ind,2)+1).n0, ...
                    FemmProblem.Segments(intersectingsegments(ind,2)+1).n1] ) ...
                    
            removeentry = [ removeentry, ind ];
        end
                    
    end
    
    % remove the flagged intersections
    intersectingsegments(removeentry, :) = [];

end

function displayproblems(problems, tol)
% displays the problems found on the command line

    if isempty(problems.nodes)
        fprintf(1, 'No problems were identified with overlapping nodes using tolerance %g\n', tol);
    else
        fprintf(1, '%d overlapping nodes were identified using tolerance %g\n', size(problems.nodes,1), tol);
        for ind = 1:size(problems.nodes,1)
            fprintf(1, 'nodes %d and %d, distance: %g\n', problems.nodes(ind,1), problems.nodes(ind,2), problems.nodes(ind,3));
        end
    end
    
    if isempty(problems.labels)
        fprintf(1, 'No problems were identified with overlapping labels using tolerance %g\n', tol);
    else
        fprintf(1, '%d overlapping nodes were identified using tolerance %g\n', size(problems.labels,1), tol);
        for ind = 1:size(problems.labels,1)
            fprintf(1, 'nodes %d and %d, distance: %g\n', problems.labels(ind,1), problems.labels(ind,2), problems.labels(ind,3));
        end
    end
    
    if isempty(problems.labelsandnodes)
        fprintf(1, 'No problems were identified with overlapping nodes and labels using tolerance %g\n', tol);
    else
        fprintf(1, '%d overlapping nodes and labels were identified using tolerance %g\n', size(problems.labelsandnodes,1), tol);
        for ind = 1:size(problems.labelsandnodes,1)
            fprintf(1, 'label %d and node %d, distance: %g\n', problems.labelsandnodes(ind,1), problems.labelsandnodes(ind,2), problems.labelsandnodes(ind,3));
        end
    end
    
    if isempty(problems.labnearsegs)
        fprintf(1, 'No problems were identified with labels too near segments using tolerance %g\n', tol);
    else
        fprintf(1, '%d labels near segments identified using tolerance %g\n', size(problems.labnearsegs,1), tol);
        for ind = 1:size(problems.labnearsegs,1)
            fprintf(1, 'label %d and segment %d, smallest distance: %g\n', problems.labnearsegs(ind,1), problems.labnearsegs(ind,2), problems.labnearsegs(ind,3));
        end
    end
    
    if isempty(problems.nodesnearsegs)
        fprintf(1, 'No problems were identified with nodes too near unconnected segments using tolerance %g\n', tol);
    else
        fprintf(1, '%d nodes near unconnected segments identified using tolerance %g\n', size(problems.nodesnearsegs,1), tol);
        for ind = 1:size(problems.nodesnearsegs,1)
            fprintf(1, 'node %d and segment %d, smallest distance: %g\n', problems.nodesnearsegs(ind,1), problems.nodesnearsegs(ind,2), problems.nodesnearsegs(ind,3));
        end
    end
    
    if isempty(problems.intersectingsegments)
        fprintf(1, 'No problems were identified with intersecting segments\n');
    else
        fprintf(1, '%d intersecting segments identified\n', size(problems.nodesnearsegs,1));
        for ind = 1:size(problems.intersectingsegments,1)
            fprintf(1, 'segment %d and segment %d intersect\n', problems.nodesnearsegs(ind,1), problems.nodesnearsegs(ind,2));
        end
    end
    
end
