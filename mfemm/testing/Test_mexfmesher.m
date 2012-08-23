% Test_mexfmesher


reply = input('WARNING: this code example may cause matlab, or your computer to crash.\nDo you wish to proceed?? Y/N [N]: ', 's');
if isempty(reply)
    reply = 'N';
end

if isequal(reply, 'Y')
    
    cd(fileparts(which('fmeshersetup.m')));
    filename = fullfile(fileparts(which('fmeshersetup.m')), 'fmesher', 'test', 'Temp.fem')
    mexfmesher(filename);

else
    
    fprintf(1, 'Aborted\n');
    
end