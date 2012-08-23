function [x,y,w,h] = extent_mfemm(FemmProblem)
% extent_mfemm: gets the extent of a problem described in an mfemm
% structure

    nodecoords = getnodecoords_mfemm(FemmProblem);
    
    x = min(nodecoords(:,1));
    
    y = min(nodecoords(:,2));
    
    
    w = max(nodecoords(:,1)) - x;
    
    h = max(nodecoords(:,2)) - y;


end