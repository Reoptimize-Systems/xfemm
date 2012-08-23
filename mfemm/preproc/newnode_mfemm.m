function Node = newnode_mfemm(x, y, varargin)
% creates a new node for an mfemm FemmProblem structure
%
% Syntax
% 
% Node = newnode_mfemm(x, y, 'Parameter', 'Value')
% 


    Node.PointPropName = '';
    Node.InGroup = 0;
    
    Node = parseoptions(Node, varargin);
    
    Node.Coords = [x, y];
    
end