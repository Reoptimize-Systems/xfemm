function [cellstrs, nodes] = problem2freecad_mfemm (FemmProblem, varargin)
% converts the segments from a FemmProblem, or portion of a FemmProblem to
% a set of sequential nodes making the boundary of a polygon
%
% Description
%
% problem2freecad_mfemm takes the segments and arcs from a FemmProblem
% structure, defined by a set of nodes which form it's boundary, and
% creates a set of python commands to recreate it in FreeCAD. The boundary
% can be reconstructed by following the nodes in sequence. Either the
% entire FemmProblem can be converted, or just a subset as defined by one
% of more group numbers from the problem.
%
% The FemmProblem (or subset of groups) must contain only segments which
% are connected to a single other segment.
%
% Syntax
%
% nodes = problem2freecad_mfemm (FemmProblem)
% nodes = problem2freecad_mfemm (FemmProblem, 'Groups', groupnums)
%
% 


    Inputs.Groups = [];
    Inputs.ShapeName = 'Polygon';
    Inputs.FileName = [];
    Inputs.IncludeHeader = true;
    Inputs.Print = false;
    
    Inputs = parseoptions (Inputs, varargin);
    
    if isempty (Inputs.FileName)
        fid = 1;
    end
    
%     nodes = getnodecoords_mfemm(FemmProblem);
%     links = getseglinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);
%     arclinks = getarclinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);
%     
%     arclinkpnts = [];
%     for ind = 1:size (arclinks, 1)
%         
%         [x, y] = arcpoints( nodes(arclinks(ind,1)+1,:), ...
%                             nodes(arclinks(ind,2)+1,:), ...
%                             arclinks(ind,3), ...
%                             arclinks(ind,3)/2 );
%         
%         nodes = [ nodes; x(2), y(2) ];
%         
%         arclinkpnts = [ arclinkpnts; size(nodes,1)-1 ];
%         
%     end
%     
    if Inputs.IncludeHeader
        cellstrs = writeheader_freecad ();
    else
        cellstrs = {};
    end
    
    unique_str = int2str(round(now*1000));
    
%     cellstrs = [ cellstrs; writenodes_freecad(nodes * 1000, unique_str) ];
%     cellstrs = [ cellstrs; writelines_freecad(links, unique_str) ];
%     cellstrs = [ cellstrs; writearcs_freecad([ arclinks(:,1:2), arclinkpnts ], unique_str) ];

    nodes = problem2polygon_mfemm (FemmProblem, 'Groups', Inputs.Groups);
    cellstrs = [ cellstrs; writenodes_freecad(nodes * 1000, unique_str) ];
    links = [ [(0:size(nodes,1)-2)', (1:size(nodes,1)-1)']; [size(nodes,1)-1, 0] ];
    cellstrs = [ cellstrs; writelines_freecad(links, unique_str) ];
    
    % create the shape
    cellstrs = [ cellstrs; sprintf('wire%s = Part.Wire (lines%s)', unique_str, unique_str) ];
    cellstrs = [ cellstrs; sprintf('%s = Part.Face (wire%s)', Inputs.ShapeName, unique_str) ];
    
    % output the results
    if Inputs.Print
        for ind = 1:numel (cellstrs)
            fprintf(fid, '%s\n', cellstrs{ind});
        end
    end

end

function cellstrs = writeheader_freecad ()

    cellstrs = {sprintf('import Part') };
    cellstrs = [ cellstrs; {sprintf('from FreeCAD import Vector\n')}];
    
end


function cellstrs = writenodes_freecad (nodes, unique_str)

    cellstrs = { sprintf('nodes%s = [ ', unique_str) };
    for vind = 1:size(nodes,1)
        cellstrs = [ cellstrs; {sprintf('        Vector (%.17e, %.17e, %.17e),', nodes(vind,1), nodes(vind,2), 0.0)} ];
    end
    cellstrs = [ cellstrs; {sprintf('        ]\n')} ];

end

function cellstrs = writelines_freecad (links, unique_str)

    % get the lines for the face
    cellstrs = { sprintf('lines%s = [', unique_str) };
    nlinks = size (links,1);
    for ind = 1:nlinks
        cellstrs = [ cellstrs; {sprintf('              Part.makeLine(nodes%s[%d], nodes%s[%d]),', unique_str, links(ind,1), unique_str, links(ind,2))} ];
    end
    cellstrs = [ cellstrs; {sprintf('            ]\n')} ];
    
end

function cellstrs = writearcs_freecad (arclinks, unique_str)

    % get the lines for the face
    cellstrs = { sprintf('arcs%s = [', unique_str) };
    nlinks = size (arclinks,1);
    for ind = 1:nlinks
        cellstrs = [ cellstrs; {sprintf('              Part.Arc(nodes%s[%d], nodes%s[%d], nodes%s[%d]).toShape(),', unique_str, arclinks(ind,1), unique_str, arclinks(ind,3), unique_str, arclinks(ind,2))} ];
    end
    cellstrs = [ cellstrs; {sprintf('            ]\n')} ];
    
end

% plotnodelinks (newnodes, [ (1:size(newnodes, 1)-1)', (2:size(newnodes, 1))']-1);
