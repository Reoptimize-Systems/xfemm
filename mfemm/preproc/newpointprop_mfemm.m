function PointProp = newpointprop_mfemm(PPropName, varargin)


    PointProp.I_re = 0;
    PointProp.I_im = 0;
    PointProp.A_re = 0;
    PointProp.A_im = 0;

    PointProp = parseoptions(PointProp, varargin);
    
    PointProp.Name = PPropName;
    
end