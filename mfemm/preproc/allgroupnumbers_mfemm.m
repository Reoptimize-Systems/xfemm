function [groupnos, info] = allgroupnumbers_mfemm (FemmProblem)
% get all group numbers from a FemmProblem structure
%
% Syntax
%
% [groupnos, info] = getallgroupnumbers_mfemm (FemmProblem)
%
% Input
%
%  FemmProblem - an mfemm FemmProblem structure
%
% Output
%
%  groupnos - vector containing all the group numbers found in the
%    FemmProblem.
%
%  info - structure containingg more detailed information on what entities
%    are in which groups, and names associated with groups, if any.


    groupnos = [];
    
    info = struct ('group_0', []);
    
    
    if isfield (FemmProblem, 'Nodes') && ~isempty (FemmProblem.Nodes)
        for ind = 1:numel(FemmProblem.Nodes)
            
            fname = [ 'group_', int2str(FemmProblem.Nodes(ind).InGroup)];
            
            if any ( ismember (FemmProblem.Nodes(ind).InGroup, groupnos) )
                
                info.(fname).Nodes = [ info.(fname).Nodes, ind-1 ];
            else
                
                groupnos = [ groupnos, FemmProblem.Nodes(ind).InGroup ];
                info = setfield (info, fname, []);
                info.(fname).Nodes = ind - 1;
                info.(fname).Number = FemmProblem.Nodes(ind).InGroup;
                
            end
        end
    end
    
    if isfield (FemmProblem, 'Segments') && ~isempty(FemmProblem.Segments)
        for ind = 1:numel(FemmProblem.Segments)
            fname = [ 'group_', int2str(FemmProblem.Segments(ind).InGroup)];
            
            if any ( ismember (FemmProblem.Segments(ind).InGroup, groupnos) )
                
                if ~isfield (info.(fname), 'Segments')
                    info.(fname).Segments = [];
                end
                
                info.(fname).Segments = [ info.(fname).Segments, ind-1 ];
            else
                
                groupnos = [ groupnos, FemmProblem.Segments(ind).InGroup ];
                info = setfield (info, fname, []);
                info.(fname).Segments = ind - 1;
                info.(fname).Number = FemmProblem.Segments(ind).InGroup;
            end
        end
    end
    
    if isfield (FemmProblem, 'ArcSegments') && ~isempty(FemmProblem.ArcSegments)
        for ind = 1:numel(FemmProblem.ArcSegments)
            fname = [ 'group_', int2str(FemmProblem.ArcSegments(ind).InGroup)];
            
            if any ( ismember (FemmProblem.ArcSegments(ind).InGroup, groupnos) )
                if ~isfield (info.(fname), 'ArcSegments')
                    info.(fname).ArcSegments = [];
                end
                info.(fname).ArcSegments = [ info.(fname).ArcSegments, ind-1 ];
            else
                
                groupnos = [ groupnos, FemmProblem.ArcSegments(ind).InGroup ];
                info = setfield (info, fname, []);
                info.(fname).ArcSegments = ind - 1;
                info.(fname).Number = FemmProblem.ArcSegments(ind).InGroup;
            end
        end
    end
    
    if isfield (FemmProblem, 'BlockLabels') && ~isempty(FemmProblem.BlockLabels)
        for ind = 1:numel(FemmProblem.BlockLabels)
            fname = [ 'group_', int2str(FemmProblem.BlockLabels(ind).InGroup)];
            
            if any ( ismember (FemmProblem.BlockLabels(ind).InGroup, groupnos) )
                if ~isfield (info.(fname), 'BlockLabels')
                    info.(fname).BlockLabels = [];
                end
                info.(fname).BlockLabels = [ info.(fname).BlockLabels, ind-1 ];
            else
                
                groupnos = [ groupnos, FemmProblem.BlockLabels(ind).InGroup ];
                info = setfield (info, fname, []);
                info.(fname).BlockLabels = ind - 1;
                info.(fname).Number = FemmProblem.BlockLabels(ind).InGroup;
            end
        end
    end
    
    fnames = fieldnames(info);
    for ind = 1:numel (fnames)
        info.(fnames{ind}).Name = '';
        if ~isfield (info.(fnames{ind}), 'Nodes')
            info.(fnames{ind}).Nodes = [];
        end
        if ~isfield (info.(fnames{ind}), 'Segments')
            info.(fnames{ind}).Segments = [];
        end
        if ~isfield (info.(fnames{ind}), 'ArcSegments')
            info.(fnames{ind}).ArcSegments = [];
        end
        if ~isfield (info.(fnames{ind}), 'BlockLabels')
            info.(fnames{ind}).BlockLabels = [];
        end
        
        info.(fnames{ind}) = orderfields (info.(fnames{ind}));
    end
    
    % get any names associated with any groups which are present in the
    % Groups structure
    if isfield (FemmProblem, 'Groups') && ~isempty (FemmProblem.Groups)
        
        gpnames = fieldnames(FemmProblem.Groups);
        
        for ind = 1:numel (gpnames)
            
            fname = [ 'group_', int2str(FemmProblem.Groups.(gpnames{ind})) ];
            
            if isfield (info, fname)
                info.(fname).Name = gpnames{ind};
            end
        end
    end

end