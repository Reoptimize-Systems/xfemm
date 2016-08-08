function str = materialconv (mat, magnetisation)

  str = {};  
  
  str = appendassignment (str, 'Name', mat.name);
  str = appendassignment (str, 'Electric Conductivity', 10.30e6);
  str = appendassignment (str, 'Heat expansion Coefficient', 11.8e-6);
  str = appendassignment (str, 'Relative Permeability', mat.Mu_x);
  str = appendassignment (str, 'Heat Conductivity', 80.2);
  str = appendassignment (str, 'Heat Capacity', 449.0);
  str = appendassignment (str, 'Density', 7870.0);
  str = appendassignment (str, 'Magnetization 1', magnetisation(1));
  str = appendassignment (str, 'Magnetization 2', magnetisation(2));
  str = appendassignment (str, 'Magnetization 3', magnetisation(3));

end

function str = appendassignment (str, lhs, rhs)

    if ~iscellstr (str)
        error ('Input must be a cell array of strings');
    end
    
    % create a new assignment statement
    newstr = createassignement (lhs, rhs);
    
    % append the new assignment string below the existing strings
    str = {str; newstr};

end


function str = createassignement (lhs, rhs)
% create a variable assignment statement for elmer

    if ischar (rhs)
        sprintf ('%s = "%s"', lhs, rhs);
    elseif isscalar (rhs) && isnumeric (rhs)
        % make certain we have enough precision
        sprintf ('%s = %17g', lhs, rhs);
    else
        error ('Variable type not supported');
    end

end