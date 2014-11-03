function nodes = problem2freecad_mfemm (FemmProblem, varargin)
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
    
    Inputs = parseoptions (Inputs, varargin);
    
    if isempty (Inputs.FileName)
        fid = 1;
    end
    
    nodes = getnodecoords_mfemm(FemmProblem);
    links = getseglinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);
    arclinks = getarclinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);
    
    arclinkpnts = [];
    for ind = 1:size (arclinks, 1)
        
        [x, y] = arcpoints( nodes(arclinks(ind,1)+1,:), ...
                            nodes(arclinks(ind,2)+1,:), ...
                            arclinks(ind,3), ...
                            arclinks(ind,3)/2 );
        
        nodes = [ nodes; x(2), y(2) ];
        
        arclinkpnts = [ arclinkpnts; size(nodes,1)-1 ];
        
    end

     writeheader_freecad (fid);
     writenodes_freecad (nodes * 1000, fid);
     writelines_freecad (links, fid);
     writearcs_freecad ([ arclinks(:,1:2), arclinkpnts ], fid);
     
     % create the shape
     fprintf(fid, '%s = Part.Shape (lines + arcs)\n', Inputs.ShapeName);

end

function writeheader_freecad (fid)

    fprintf(fid, 'import Part\n');
    fprintf(fid, 'from FreeCAD import Vector\n\n');
    
end


function writenodes_freecad (nodes, fid)

    fprintf(fid, 'nodes = [ \n');
    for vind = 1:size(nodes,1)
        fprintf(fid, '        Vector (%.17e, %.17e, %.17e),\n', nodes(vind,1), nodes(vind,2), 0.0);
    end
    fprintf(fid, '        ]\n\n');

end

function writelines_freecad (links, fid)

    % get the lines for the face
    fprintf(fid, 'lines = [\n');
    nlinks = size (links,1);
    for ind = 1:nlinks
        fprintf(fid, '              Part.Line(nodes[%d], nodes[%d]),\n', links(ind,1), links(ind,2));
    end
    fprintf(fid, '            ]\n\n');
    
end

function writearcs_freecad (arclinks, fid)

    % get the lines for the face
    fprintf(fid, 'arcs = [\n');
    nlinks = size (arclinks,1);
    for ind = 1:nlinks
        fprintf(fid, '              Part.Arc(nodes[%d], nodes[%d], nodes[%d]),\n', arclinks(ind,1), arclinks(ind,3), arclinks(ind,2));
    end
    fprintf(fid, '            ]\n\n');
    
end

% plotnodelinks (newnodes, [ (1:size(newnodes, 1)-1)', (2:size(newnodes, 1))']-1);
