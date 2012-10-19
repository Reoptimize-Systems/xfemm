function result = isaxeshandle(h)
% isaxeshandle: returns true if an object is a handle to an axes object
% (and not a legend or colorbar) 

    result = zeros(size(h));
    
    handles = ishandle(h);
    
    result(handles) = strcmp( get(h(handles), 'type'), 'axes' ) & ...
                                ~(strcmp( get(h(handles), 'Tag'), 'legend') | ...
                                  strcmp( get(h(handles), 'Tag'), 'Colorbar'));

end