function links = getnodelinks_mfemm(FemmProblem)
% getnodelinks_femm: gets all the segments from an mfemm problem
% structure

    links = [cell2mat({FemmProblem.Segments(:).n0}'), cell2mat({FemmProblem.Segments(:).n1}')];

end