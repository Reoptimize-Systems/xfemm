function [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, varargin)
% adds a segment to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, 'Parameter', 'Value')
% 

    seginds = repmat(-1, 1, numel(n0));
    
    if ~isfield(FemmProblem, 'Segments') || isempty(FemmProblem.Segments)
        seginds(1) = 1;
        FemmProblem.Segments = newsegment_mfemm(n0(1), n1(1), varargin{:});
    else
        seginds(1) = numel(FemmProblem.Segments) + 1;
        FemmProblem.Segments(seginds(1)) = newsegment_mfemm(n0(1), n1(1), varargin{:});
    end
    
    for i = 2:numel(n0)
        
        seginds(i) = seginds(i-1) + 1;
        
        FemmProblem.Segments(seginds(i)) = newsegment_mfemm(n0(i), n1(i), varargin{:});      
        
    end
    
end