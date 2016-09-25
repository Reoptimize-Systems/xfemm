function fp = mergeproblems_mfemm (FemmProblemArray, varargin)
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

    options.AutoFixBoundNames = false;
    
    options = mfemmdeps.parse_pv_pairs (options, varargin);

    fp = FemmProblemArray(1);
    
    for ind = 2:numel (FemmProblemArray)
        
        elcount = elementcount_mfemm (fp);
        
        % check for identically named boundary properties
        for nameind = 1:numel (fp.BoundaryProps)
            
                
            bncheck = structarrayfcn2 ( FemmProblemArray(ind).BoundaryProps, ...
                        'Name', @(x) checkidenticalnames (fp.BoundaryProps(nameind).Name, x), ...
                        'UniformOutput', true ...
                                       );

            if any (bncheck)

                if options.AutoFixBoundNames

                    bnind = find(bncheck);

                    newboundname = [fp.BoundaryProps(nameind).Name, sprintf(' - prob %d', ind)];

                    % replace the boundary name in the list of boundary
                    % properties
                    FemmProblemArray(ind).BoundaryProps(bnind).Name = newboundname;

                    % replace the name in all the segments and arc segments
                    % of the femm problem structure to be appended
                    for segind = 1:numel(FemmProblemArray(ind).Segments)
                        if strcmp (FemmProblemArray(ind).Segments(segind).BoundaryMarker, fp.BoundaryProps(nameind).Name)
                            FemmProblemArray(ind).Segments(segind).BoundaryMarker = newboundname;
                        end
                    end

                    for arcsegind = 1:numel(FemmProblemArray(ind).ArcSegments)
                        if strcmp (FemmProblemArray(ind).ArcSegments(arcsegind).BoundaryMarker, fp.BoundaryProps(nameind).Name)
                            FemmProblemArray(ind).ArcSegments(arcsegind).BoundaryMarker = newboundname;
                        end
                    end

                else
                    
                    error ('MFEMM:mergeproblems:identicalnames', ...
            'cannot have identically named boundary names "%s" in any two or more problems to be merged', ...
                            fp.BoundaryProps(nameind).Name);

                end


            end
                
                    
                
        end
        
        fp.BoundaryProps = [ fp.BoundaryProps, FemmProblemArray(ind).BoundaryProps ];
        
        
        
        fp.Nodes = [ fp.Nodes, FemmProblemArray(ind).Nodes ];
        
        FemmProblemArray(ind).Segments = structarrayfcn (FemmProblemArray(ind).Segments, 'n0', @(x) x + elcount.NNodes);
        FemmProblemArray(ind).Segments = structarrayfcn (FemmProblemArray(ind).Segments, 'n1', @(x) x + elcount.NNodes);
        
        fp.Segments = [ fp.Segments, FemmProblemArray(ind).Segments ];
        
        FemmProblemArray(ind).ArcSegments = structarrayfcn (FemmProblemArray(ind).ArcSegments, 'n0', @(x) x + elcount.NNodes);
        FemmProblemArray(ind).ArcSegments = structarrayfcn (FemmProblemArray(ind).ArcSegments, 'n1', @(x) x + elcount.NNodes);
        
        fp.ArcSegments = [ fp.ArcSegments, FemmProblemArray(ind).ArcSegments ];
        
        fp.BlockLabels = [ fp.BlockLabels, FemmProblemArray(ind).BlockLabels ];
        
        % \todo: fix check for identical names
        % check for identically named materials
%         for nameind = 1:numel (fp.Materials)
%         
%             structarrayfcn (FemmProblemArray(ind).Materials, ...
%                             'Name', @(x) checkidenticalnames (fp.Materials(nameind).Name, x));
%         
%         end
        
        fp.Materials = [ fp.Materials, FemmProblemArray(ind).Materials ];
        
        
        
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
% applies function to field of all members of stucture array S, returns the
% modified structure

    for ind = 1:numel (S)
        S(ind).(field) = feval (fcn, S(ind).(field));
    end
    
end


function out = structarrayfcn2 (S, field, fcn, varargin)
% applies function to field to all members of stucture array S, returns the
% result for each member

    options.UniformOutput = true;
    
    options = mfemmdeps.parse_pv_pairs (options, varargin);
   
    for ind = 1:numel (S)
        out{ind} = feval (fcn, S(ind).(field));
    end
    
    reshape (out, size(S));
        
    if options.UniformOutput
        out = cell2mat (out);
    end
    
end


function status = checkidenticalnames (name1, name2)

    status = false;

    if strcmp (name1, name2)

        status = true; 

    end

end

