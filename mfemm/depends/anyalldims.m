function tf = anyalldims(A)

    if isnumeric(A) || islogical(A)
        
        tf = any(A(:));
    
    else
        error('A must be a numeric or logical matrix');
    end

end