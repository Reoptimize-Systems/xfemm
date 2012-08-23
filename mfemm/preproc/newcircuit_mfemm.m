function Circuit = newcircuit_mfemm(Name, varargin)

    Circuit.CircType = 1; % series
	Circuit.TotalAmps_re = 0;
    Circuit.TotalAmps_im = 0;
    
    Circuit = parse_pv_pairs(Circuit, varargin);
    
    Circuit.Name = Name;

end