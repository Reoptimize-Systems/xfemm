function sameclims(h, CLim)
% sameclims: sets the color limits of a set of axes  (or all axes in a
% figure) to be the same.
%
% Syntax
% 
% sameclims;
% sameclims(h)
% sameclims(h, CLim)
%
% Description
% 
% h is either a handle to a figure or a matrix of axes handles. If a figure
% handle is supplied, sameclims operates on all axes found in that figure.
%
% sameclims sets the range of color limits in all the axes it operates on
% to be the same. If only one argument is supplied, the color limits are
% determined byt the maximum range of all existing color limits.
% Alternatively a (1 x 2) vector of color limits for all of the axes can be
% supplied in the CLims argument.
%
% sameclims detects axes in figures by assuming the Tag property of legends
% and colorbars has not been changed from the default. Expect strange
% results if this is not the case.

    if nargin == 0
        h = gcf;
    end


    if numel(h) == 1 && strcmp( get(h, 'type'), 'figure')
        
        hax = findobj(gcf,'type','axes','-not','Tag','legend','-not','Tag','Colorbar');
        
        if isempty(hax)
            
            warning('No axes handles found in figure')
            
            return;
            
        end

    elseif anyalldims( isaxeshandle(h) )
        
        hax = h(isaxeshandle(h));
        
    else
        
        error('Input should be a single figure handle or one or more axes handles.')
        
    end
    
    if nargin < 2
        
        CLim = get(hax(1), 'CLim');

        for i = 2:numel(hax)

            CLimi = get(hax(i), 'CLim');

            CLim = [max(CLimi(1), CLim(1)), min(CLimi(2), CLim(2))];

        end
        
    elseif ~(isnumeric(CLims) && size(CLims,1) == 1 && size(CLims,2) == 2)
        
        error('If supplied, CLims must be a (1 x 2) vector of color limits.')

    end
    
    for i = 1:numel(hax)
        
        set(hax(i), 'CLim', CLim);

    end
    
end