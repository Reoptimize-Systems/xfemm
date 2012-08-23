function FemmProblem = mirrorsegments_mfemm(FemmProblem, seginds, disttol, varargin)
% mirros one or more segments along a line
%
% Syntax
%
% FemmProblem = mirrorsegments(FemmProblem, seginds, disttol, varargin)
%
% 

    if isempty(disttol)
        disttol = 1e-5;
    end
    
    newseginds = [];
    
    for i = 1:numel(seginds)
       
        % mirror the nodes in the segment
        segnodecoords = [ FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0 + 1).Coords; 
                          FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1 + 1).Coords ];
                          
        newsegcoords = reflect2d(segnodecoords, varargin{:});
        
        % get the distances between nodes
        nodedists = diag(ipdm(segnodecoords, newsegcoords));
        
        % add the mirrored nodes to the problem if they are separated from
        % the existing node by at least tol
        [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
            newsegcoords(nodedists > disttol, 1), newsegcoords(nodedists > disttol, 2));
            
        SegProps = FemmProblem.Segments(seginds(i));
        SegProps = rmfield(SegProps, 'n0');
        SegProps = rmfield(SegProps, 'n1');
        
        % join the relevant nodes to make a new segment with the same
        % properties as the original segment
        if (nodedists(1) < disttol) && (nodedists(2) < disttol)
            % do nothing the segment lies along the line of reflection
        elseif nodedists(1) < disttol
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       FemmProblem.Segments(seginds(i)).n0, ...
                                       nodeids(2), ...
                                       SegProps);
                                   
        elseif nodedists(2) < disttol
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       nodeids(1), ...
                                       FemmProblem.Segments(seginds(i)).n1, ...
                                       SegProps);
            
        else
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       nodeids(1), ...
                                       nodeids(2), ...
                                       SegProps);
            
        end
        
        newseginds = [newseginds, newsegind];
        
        
    end

end 