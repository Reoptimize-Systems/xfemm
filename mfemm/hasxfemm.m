function tf = hasxfemm()
% determines if all xfemm mexfuntions are available
%

    tf = (exist('mexfmesher', 'file') == 3) ...
        && (exist('mexfsolver', 'file') == 3) ...
        && (exist('fpproc_interface_mex', 'file') == 3);

end