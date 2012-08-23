function elcount = elementcount_mfemm(FemmProblem)
% elementcount_mfemm: Counts the number of various elements in an mfemm
% FemmProblem structure
%
% Syntax
% 
% elcount = elementcount_mfemm(FemmProblem)
%
% Input
%
% FemmProblem is a structure containing a complex nested series of
% structure arrays which form the FEA problem description. 
%
% FemmProblem can contain the following fields:
%
%   ProbInfo
%   PointProps (optional)
%   BoundaryProps 
%   Materials 
%   Circuits (optional)
%   Nodes
%   Segments (optional)
%   ArcSegments (optional)
%   BlockLabels 
%
% 

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http://www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.


    % Count the number of boundary properties
%     if isfield(FemmProblem, 'Bounds')
%         
%         if any(arrayfun(@(x)isempty(x.Name),FemmProblem.BoundaryProps,'uniformoutput',true))
%             error('Boundary property has an empty name. ');
%         end
%         
%         elcount.NBounds = numel(FemmProblem.BoundaryProps());
%     else
%         elcount.NBounds = 0;
%     end
    
    % Count the number of nodes
    if isfield(FemmProblem, 'Nodes')
        elcount.NNodes = numel(FemmProblem.Nodes);
    else
        elcount.NNodes = 0;
    end
    
    % Count the number of Segments
    if isfield(FemmProblem, 'Segments')
        elcount.NSegments = numel(FemmProblem.Segments);
    else
        elcount.NSegments = 0;
    end
    
    % Count the number of ArcSegments
    if isfield(FemmProblem, 'ArcSegments')
        elcount.NArcSegments = numel(FemmProblem.ArcSegments);
    else
        elcount.NArcSegments = 0;
    end
    
    % Count the number of Materials
    if isfield(FemmProblem, 'Materials')
        elcount.NMaterials = numel(FemmProblem.Materials);
    else
        elcount.NMaterials = 0;
    end
    
    % Count the number of Block Labels
    if isfield(FemmProblem, 'BlockLabels')
        elcount.NBlockLabels = numel(FemmProblem.BlockLabels);
    else
        elcount.NBlockLabels = 0;
    end
    
    % Count the number of Circuits
    if isfield(FemmProblem, 'Circuits')
        elcount.NCircuits = numel(FemmProblem.Circuits);
    else
        elcount.NCircuits = 0;
    end
    
    % Count the number of Point Properties
    if isfield(FemmProblem, 'PointProps')
        elcount.NPointProps = numel(FemmProblem.PointProps);
    else
        elcount.NPointProps = 0;
    end
    
    % Count the number of Boundary Properties
    if isfield(FemmProblem, 'BoundaryProps')
        
        if any(arrayfun(@(x)isempty(x.Name),FemmProblem.BoundaryProps,'uniformoutput',true))
            error('Boundary property has an empty name. ');
        end
        
        elcount.NBoundaryProps = numel(FemmProblem.BoundaryProps);
    else
        elcount.NBoundaryProps = 0;
    end
    
end