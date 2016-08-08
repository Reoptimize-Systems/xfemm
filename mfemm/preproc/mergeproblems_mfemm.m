function fp = mergeproblems_mfemm (FemmProblemArray)
% merges an array of FemmProblem structures into a single problem structure
%
%  Syntax
%
% FemmPoblem = mergeproblems_mfemm (FemmPoblems)
%
% Input
%
%  FemmProblemArray -  array of mfemm problem structures to be merged int a
%    single problem structure
%
% Output
%
%  fp - A sngle mfemm problem structure created from the array of input
%    structures
%


% FemmProblem = 
% 
%          ProbInfo: [1x1 struct]
%         Materials: [1x1 struct]
%          Segments: [1x42 struct]
%       ArcSegments: []
%             Nodes: [1x33 struct]
%     BoundaryProps: [1x5 struct]
%          Circuits: []
%       BlockLabels: [1x10 struct]
%        PointProps: []
%            Groups: [1x1 struct]

    fp = FemmProblemArray(1);
    
    for ind = 2:numel (FemmProblemArray)
        
        elcount = elementcount_mfemm (fp);
        
        fp.Nodes = [ fp.Nodes, FemmProblemArray(ind).Nodes ];
        
        FemmProblemArray(ind).Segments = structarrayfcn (FemmProblemArray(ind).Segments, 'n0', @(x) x + elcount.NNodes);
        FemmProblemArray(ind).Segments = structarrayfcn (FemmProblemArray(ind).Segments, 'n1', @(x) x + elcount.NNodes);
        
        fp.Segments = [ fp.Segments, FemmProblemArray(ind).Segments ];
        
        FemmProblemArray(ind).ArcSegments = structarrayfcn (FemmProblemArray(ind).ArcSegments, 'n0', @(x) x + elcount.NNodes);
        FemmProblemArray(ind).ArcSegments = structarrayfcn (FemmProblemArray(ind).ArcSegments, 'n1', @(x) x + elcount.NNodes);
        
        fp.ArcSegments = [ fp.ArcSegments, FemmProblemArray(ind).ArcSegments ];
        
        fp.BlockLabels = [ fp.BlockLabels, FemmProblemArray(ind).BlockLabels ];
        
        % check for identically named materials
        for nameind = 1:numel (fp.Materials)
        
            structarrayfcn (FemmProblemArray(ind).Materials, ...
                            'Name', @(x) checkidenticalnames (fp.Materials(nameind).Name, x));
        
        end
        
        fp.Materials = [ fp.Materials, FemmProblemArray(ind).Materials ];
        
        % check for identically named boundary properties
        for nameind = 1:numel (fp.BoundaryProps)
        
            structarrayfcn (FemmProblemArray(ind).BoundaryProps, ...
                            'Name', @(x) checkidenticalnames (fp.BoundaryProps(nameind).Name, x));
        
        end
        
        fp.BoundaryProps = [ fp.BoundaryProps, FemmProblemArray(ind).BoundaryProps ];
        
        % check for identically named point properties
        for nameind = 1:numel (fp.PointProps)
        
            structarrayfcn (FemmProblemArray(ind).PointProps, ...
                            'Name', @(x) checkidenticalnames (fp.PointProps(nameind).Name, x));
        
        end
        
        fp.PointProps = [ fp.PointProps, FemmProblemArray(ind).PointProps ];
        
        fp.Groups = [ fp.Groups, FemmProblemArray(ind).Groups ];
        
    end

end

function S = structarrayfcn (S, field, fcn)

    for ind = 1:numel (S)
        S(ind).(field) = feval (fcn, S(ind).(field));
    end
    
end


function checkidenticalnames (name1, name2)

    if strcmp (name1, name2)
        error ('MFEMM:mergeproblems:identicalnames', ...
            'cannot have identically named property "%s" in any two or more problems to be merged', name1);
    end

end

