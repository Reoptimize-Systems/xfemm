function matlibdat2mat(datfile)

    if nargin == 0
    	matlib = parsematlib_mfemm();
    else
        matlib = parsematlib_mfemm(datfile);
    end
        
    rootpath = getmfilepath('matlibdat2mat');
    
    save(fullfile(rootpath, 'matlib.mat'), 'matlib');
    
end