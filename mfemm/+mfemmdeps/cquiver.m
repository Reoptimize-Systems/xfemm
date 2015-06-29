function hax = cquiver(varargin)
% CQUIVER  Display 2D vector field as equal length arrow grid with
% magnitude denoted by colour of arrow
%
% Displays a 2d vector field using an equal-length arrow grid and colors to
% indicate magnitude.  Colors are taken from the current colormap, or an
% optional supplied colormap.
%
% For grayscale, try colormap(flipud(gray)).
%
% Syntax:
%
%   hax = cquiver(vfield)   % flow complex or MxNx2
%   hax = cquiver(U, V)  % separate flow components
%   hax = cquiver(..., 'Parameter', 'Value')   
% 
% Description
%
% The vector field can be specified several ways:
%
% 1. As a single matrix of size (m x n x 2) where vfield(:,:,1) is the
% x-directed field values and vfield(:,:,2) the y-directed field values at
% the same positions. 
%
% 2. As a single complex matrix of size (m x n x 1) where the real and
% imaginary parts represent the x and y directions respectively.
%
% 3. As two matrices of size (m x n x 1) U, and V, where U and V are the x
% and y directed components respectively.
%
% Without any additional arguments, the vectors will be plotted at equally
% spaced intervals corresponding to their position in the input matrices.
% To alter the spacing between points, you can specify parameter-value
% pairs.
%
% To use the same spacing for both x and y directions use the 's'
% parameter, e.g.
%
% hax = cquiver(vfield, 's', 0.1)
%
% Separate spacings for the x and y directions can by specified by
% supplying two p-v pairs, sx and sy. e.g.
%
% hax = cquiver(vfield, 'sx', 0.1, 'sy', 0.2)
%
% In addition some further arguments can also be supplied as parameter
% value pairs:
%
% 'hax' - a handle to an existing axes object in which to do the plot
% 'maxd' - maximum vector magnitude for use in the colormap
% 'cmap' - colormap to use for the plot
% 'Color' - background color for the plot
% 'AxesProps' - additional properties to be set on the axes
% 'xshift' - shifts the plot by this value in the x direction
% 'yshift' - shifts the plot by this value in the x direction
%
%
% Portions based on ncquiverref by Andrew Roberts
% See license.txt for details
%
% Modified by Richard Crozier  26 Jan 2012
%
% See also QUIVER, VFCOLOR

    % parse the inputs
    [u,v,sx,sy,maxd,cmap,bgcolor,axprops,xshift,yshift,hax] = parse_inputs(varargin);

    % convert u and v cartesian to polar coordinates
    [th,fieldmag] = cart2pol(u,v);

    if ~isempty(maxd) && imag(maxd)
        maxd = abs(maxd);
    else
        % find the maximum magnitude of the u and v inputs, this will be used
        % to determine the colour range 
        maxd = sqrt(max(u(:).^2 + v(:).^2));
        
    end
    
    if isinf(maxd)
        error('maxd is InF')
    elseif isnan(maxd)
        error('maxd is NaN')
    end

    % create bands to use for colours in colour map
    nclr = size(cmap, 1);
    cont = linspace(0, maxd, nclr+1);
    cont(1) = [];

    % Create locations for centres of the vector arrows
    [nrow,ncol] = size(fieldmag);
    [x,y] = meshgrid(0:ncol-1,0:nrow-1);
    
    % scale locations to spacings
    x = x * sx;
    y = y * sy;

    scalelength = min(sx,sy);
    
    arrow = 0.40;
    
    if isempty(hax)
        hax = axes;
    end
    
    % Draw the arrows
    for i = 1:length(cont)

        if i == 1

            mask = find(fieldmag < cont(i));

        elseif i == length(cont)

            mask = find(fieldmag >= cont(i-1));

        else

            mask = find(fieldmag < cont(i) & fieldmag >= cont(i-1));

        end

        mask = mask';

        % Center vectors over grid points
        [u,v] = pol2cart(th(mask), scalelength);
        xstart = x(mask) - 0.5 * u;
        xend = x(mask) + 0.5 * u;
        ystart = y(mask) - 0.5 * v;
        yend = y(mask) + 0.5 * v;

        % Get x coordinates of each vector plotted
        lx = [ xstart; ...
               xstart + (1 - arrow / 3) * (xend - xstart); ...
               xend - arrow * ( u + arrow * v ); ...
               xend; ...
               xend - arrow * (u - arrow * v); ...
               xstart + (1 - arrow / 3) * (xend - xstart); ...
               nan(size(xstart)) 
             ];

        % Get y coordinates of each vector plotted
        ly = [ ystart; ...
               ystart + (1 - arrow / 3) * (yend - ystart); ...
               yend - arrow * (v - arrow * u); ...
               yend; ...
               yend - arrow * (v + arrow * u); ...
               ystart + (1 - arrow / 3) * (yend - ystart); ...
               nan(size(ystart)) 
             ];
         
        lx = lx + xshift;
        ly = ly + yshift;

        % Plot the vectors
        line(lx, ly, 'Color', cmap(i,:));

    end

    % set the axis units to be the same on both axes
    axis equal tight
    set(hax, 'CLim', [0 maxd]); %,'XLim',[-.5 ncol+.5],'YLim',[-.5 nrow+.5]);
    %colorbar;

    % set the axes colour
    set(hax, 'Color', bgcolor);
    
    % set other axes properties if desired
    if ~isempty(axprops)
        set(hax, axprops{:});
    end
    
end


function [u,v,sx,sy,maxd,cmap,bgcolor,axprops,xshift,yshift,hax] = parse_inputs(var)
%PARSE_INPUTS
%   [u,v,maxd,cmap] = parse_inputs(var)

    % Set up default p-v pairs
    Inputs.cmap = colormap;
    Inputs.maxd = [];
    Inputs.Color = 'white';
    Inputs.AxesProps = {};
    Inputs.xshift = 0;
    Inputs.yshift = 0;
    Inputs.hax = [];
    Inputs.sx = [];
    Inputs.sy = [];
    Inputs.s = [];
    
    % paw through variable inputs to find first string of p-v pairs
    fstparamstr = 0;
    for i = 1:numel(var)
        if ischar(var{i})
            fstparamstr = i;
            break;
        end
    end
    
    if fstparamstr
        
        Inputs = mfemmdeps.parse_pv_pairs(Inputs, var(fstparamstr:end));
        
        % throw away the p-v pairs
        var = var(1:end-fstparamstr+1);
        
    end
    
    maxd = Inputs.maxd;
    cmap = Inputs.cmap;
    bgcolor = Inputs.Color;
    axprops = Inputs.AxesProps;
    xshift = Inputs.xshift;
    yshift = Inputs.yshift;
    hax = Inputs.hax;
    
    if ~isempty(Inputs.s) && isempty(Inputs.sx) && isempty(Inputs.sy)
        
        if isscalar(Inputs.s)
            sx = Inputs.s;
            sy = Inputs.s;
        else
            error('Non-scalar spacings not currently supported')
        end
        
    elseif isempty(Inputs.s) && ~isempty(Inputs.sx) && ~isempty(Inputs.sy)
        
        if isscalar(Inputs.sx) && isscalar(Inputs.sy)
            % two scalar spacings supplied use second as y spacing
            sx = Inputs.sx;
            sy = Inputs.sy;
        else
            error('Non-scalar spacings not currently supported');
        end
        
    elseif ~isempty(Inputs.s) && (~isempty(Inputs.sx) || ~isempty(Inputs.sy))
        error('Incorrect spacing spec');
    else
        sx = 1;
        sy = 1;
    end
    
    
    if (size(var{1},3) == 2)

        % two vector planes
        u = var{1}(:,:,1);

        v = var{1}(:,:,2);
        
    elseif ~every(isreal(var{1}))
        
        % complex flow
        u = full(real(var{1}));

        v = full(imag(var{1}));

    elseif numel(var) > 1 
        
        if samesize(var{1}, var{2}) 
            
            if every(isreal(var{1})) && every(isreal(var{2}))
            
                % two flow arguments 

                u = var{1};
                v = var{2};
            
            end

        elseif ~every(isreal(var{1}))
            
            % complex flow
            u = full(real(var{1}));

            v = full(imag(var{1}));
            
        else
            error('Incorrect vector field specification.')
        end

    else
        error('Incorrect vector field specification.')
    end


    
end
