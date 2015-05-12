function tf = every(A)

    if isnumeric(A) || islogical(A)
        
        tf = all(A(:));
    
    else
        error('A must be a numeric or logical matrix');
    end
    
end