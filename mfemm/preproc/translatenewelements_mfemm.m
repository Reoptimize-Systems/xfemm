function FemmProblem = translatenewelements_mfemm (FemmProblem, oldelcount, XShift, YShift, varargin)
% translate newly added nodes and labels in the x and y direction
%
% Syntax
%
% FemmProblem = translatenewelements_mfemm (FemmProblem, oldelcount, XShift, YShift)
%
% Description
%
% translatenewelements_mfemm moves allelements added since the element count
% provided in oldelcount was performed by the specified amount in the x and
% y direction.
%
% Inputs
%
%  FemmProblem - FemmProblem structure containing elements to be moved
%
%  oldelcount - the element count, as produced by elementcount_mfemm
%    performed before adding the new elements to be shifted
%
%  XShift - desired translation in the x (or r) direction
%
%  YShift - desired translation in the y (or rho) direction
%
% Some additional options can be provided as Parameter-Value pairs:
%
%  'NotNodes' - true or false flag to prevent moving nodes. Default is
%    false
%
%  'NotBlockLabels' - true or false flag to prevent moving block labels.
%    Default is false
%
%
% See also: elementcount_mfemm
%

    Inputs.NotNodes = false;
    Inputs.NotBlockLabels = false;
    
    Inputs = mfemmdeps.parse_pv_pairs (Inputs, varargin);
    
    if XShift ~= 0 || YShift ~= 0
        
        % get the new count of elements
        newelcount = elementcount_mfemm (FemmProblem);
        
        if ~Inputs.NotNodes
            
            if newelcount.NNodes > oldelcount.NNodes
                
                nodeids = (oldelcount.NNodes):(newelcount.NNodes-1);

                FemmProblem = translatenodes_mfemm(FemmProblem, XShift, YShift, nodeids);
                
            end
            
        end
        
        
        if ~Inputs.NotBlockLabels
            
            if newelcount.NBlockLabels > oldelcount.NBlockLabels 
                
                blockids = (oldelcount.NBlockLabels):(newelcount.NBlockLabels-1);
                 
                FemmProblem = translateblocklabels_mfemm(FemmProblem, XShift, YShift, blockids);
                
            end
            
        end
        
    end

end
