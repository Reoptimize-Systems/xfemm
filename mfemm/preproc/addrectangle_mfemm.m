function [FemmProblem, seginds, nodeinds, nodeids, centre] = addrectangle_mfemm(FemmProblem, x, y, w, h, varargin)

    % construct the coordinates of the vertices in clockwise order
    coords = [x, y; 
              x + w, y;
              x + w, y + h;
              x, y + h ];
          
    % use addpolygon to make the rectangle
    [FemmProblem, seginds, nodeinds, nodeids] = addpolygon_mfemm(FemmProblem, coords, varargin{:});

    % locate the centre for convenience
    centre = rectcentre(coords(1,:), coords(3,:));
    
end