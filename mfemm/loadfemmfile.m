function [FemmProblem, Solution] = loadfemmfile(filename)
% loads a femm solution from a file and stores it in a FemmProblem and
% FemmProblem solution structure
%
% Syntax
%
% [FemmProblem, Solution] = loadsolution_mfemm(filename)
%
% Input
%
%   filename - string containing the location of the femm solution file to
%     be loaded
%
% Output
%
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http:%www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

    fid = fopen(filename);

%     cleanupObj = onCleanup(@(x) fclose(x));

    matind = 0;
    boundind = 0;

    q = '';

    while ischar(q)

        % get the first line from the file
        q = strtrim(fgetl(fid));

        % Deal with flag for file format version
        if strncmpi(q,'[format]',8)

            v = str2double(StripKey(q));

            if v ~= 4
                error('This file is from a different version of FEMM\nRe-analyze the problem using the current version.');
            end

            continue;

        end

        % Frequency of the problem
        if strncmpi(q,'[frequency]',11)
            v = StripKey(q);
            FemmProblem.ProbInfo.Frequency = str2double(v);
            continue;
        end

        % Depth of the problem
        if strncmpi(q,'[depth]',7)
            v = StripKey(q);
            FemmProblem.ProbInfo.Depth = str2double(v);
            continue;
        end

        % Minimum angle constraint
        if strncmpi(q,'[MinAngle]',7)
            v = StripKey(q);
            FemmProblem.ProbInfo.MinAngle = str2double(v);
            continue;
        end
        
        % Units of length used by the problem
        if strncmpi(q,'[lengthunits]',13)

            v = StripKey(q);

            if strncmpi(v,'inches',6)
                FemmProblem.ProbInfo.LengthUnits = 0;
            elseif strncmpi(v,'millimeters',11)
                FemmProblem.ProbInfo.LengthUnits = 1;
            elseif strncmpi(v,'centimeters',1)
                FemmProblem.ProbInfo.LengthUnits = 2;
            elseif strncmpi(v,'mils',4)
                FemmProblem.ProbInfo.LengthUnits  = 4;
            elseif strncmpi(v,'microns',6)
                FemmProblem.ProbInfo.LengthUnits  = 5;
            elseif strncmpi(v,'meters',6)
                FemmProblem.ProbInfo.LengthUnits  = 3;
            end

            continue;

        end
        
        if strncmpi(q,'[precision]',11)
            
            v = StripKey(q);
            
            FemmProblem.ProbInfo.Precision = str2double(v);
            
            continue;
            
        end

        % Problem Type (planar or axisymmetric)
        if strncmpi(q,'[problemtype]',13)

            v = StripKey(q);

            if strncmpi(v,'planar',6)
                FemmProblem.ProbInfo.ProblemType = 0;
            end

            if strncmpi(v,'axisymmetric',3)
                FemmProblem.ProbInfo.ProblemType = 1;
            end

            continue;

        end

        % Coordinates (cartesian or polar)
        if strncmpi(q,'[coordinates]',13)

            v = StripKey(q);

            if strncmpi(v,'cartesian',4)
                FemmProblem.ProbInfo.Coords = 0;
            end

            if strncmpi(v,'polar',5)
                FemmProblem.ProbInfo.Coords = 1;
            end

            continue;

        end

        % Comments
        if (strncmpi(q,'[comment]',9))

            v = StripKey(q);

            FemmProblem.ProbInfo.Comments = removequotes(v);

            continue;

        end

        % properties for axisymmetric external region
        if( strncmpi(q,'[extzo]',7))
            v = StripKey(q);
            FemmProblem.ProbInfo.extZo = str2double(v);
            continue;
        end

        if( strncmpi(q,'[extro]',7))
            v = StripKey(q);
            FemmProblem.ProbInfo.extRo = str2double(v);
            continue;
        end

        if( strncmpi(q,'[extri]',7))
            v = StripKey(q);
            FemmProblem.ProbInfo.extRi = str2double(v);
            continue;
        end

        % Point Properties
        if( strncmpi(q,'<beginpoint>',11))

            % Make a new empty point property
            [FemmProblem, ppropind] = addpointprop(FemmProblem, 'New Point Property');

            continue;
        end

        if( strncmpi(q,'<pointname>',11))

            v = StripKey(q);

            FemmProblem.PointProps(ppropind).Name = removequotes(v);

            continue;
        end

        if( strncmpi(q,'<A_re>',6))
            v = StripKey(q);
            FemmProblem.PointProps(ppropind).A_re = str2double(v);
            continue;
        end

        if( strncmpi(q,'<A_im>',6))
            v = StripKey(q);
            FemmProblem.PointProps(ppropind).A_im = str2double(v);
            continue;
        end

        if( strncmpi(q,'<I_re>',6))
            v = StripKey(q);
            FemmProblem.PointProps(ppropind).I_re = str2double(v);
            continue;
        end

        if( strncmpi(q,'<I_im>',6))
            v = StripKey(q);
            FemmProblem.PointProps(ppropind).I_im = str2double(v);
            continue;
        end

        if( strncmpi(q,'<endpoint>',9))
            continue;
        end

        % Boundary Properties;
        if( strncmpi(q,'<beginbdry>',11))

            [FemmProblem, boundind] = addboundaryprop_mfemm(FemmProblem, 'New Boundary', 0);
            continue;
        end

        if( strncmpi(q,'<bdryname>',10))

            v = StripKey(q);

            FemmProblem.BoundaryProps(boundind).Name = removequotes(v);
            continue;
        end

        if( strncmpi(q,'<bdrytype>',10))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).BdryType = str2double(v);
            continue;
        end

        if( strncmpi(q,'<mu_ssd>',8))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).Mu_ssd = str2double(v);
            continue;
        end

        if( strncmpi(q,'<sigma_ssd>',11))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).Sigma_ssd = str2double(v);
            continue;
        end

        if( strncmpi(q,'<A_0>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).A0 = str2double(v);
            continue;
        end

        if( strncmpi(q,'<A_1>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).A1 = str2double(v);
            continue;
        end

        if( strncmpi(q,'<A_2>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).A2 = str2double(v);
            continue;
        end

        if( strncmpi(q,'<phi>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).Phi = str2double(v);
            continue;
        end

        if( strncmpi(q,'<c0>',4))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).c0 = str2double(v);
            continue;
        end

        if( strncmpi(q,'<c1>',4))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).c1 = str2double(v);
            continue;
        end

        if( strncmpi(q,'<c0i>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).c0i = str2double(v);
            continue;
        end

        if( strncmpi(q,'<c1i>',5))
            v = StripKey(q);
            FemmProblem.BoundaryProps(boundind).c1i = str2double(v);
            continue;
        end

        if( strncmpi(q,'<endbdry>',9))
            continue;
        end


        % Block Properties;
        if( strncmpi(q,'<beginblock>',12))

            [FemmProblem, matind] = addmaterials_mfemm(FemmProblem, emptymatstruct_mfemm());
            
            continue;
        end

        if( strncmpi(q,'<blockname>',10))
            v = StripKey(q);

            FemmProblem.Materials(matind).Name = removequotes(v);
            continue;
        end

        if( strncmpi(q,'<mu_x>',6))
            v = StripKey(q);
            FemmProblem.Materials(matind).Mu_x = str2double(v);
            continue;
        end

        if( strncmpi(q,'<mu_y>',6))
            v = StripKey(q);
            FemmProblem.Materials(matind).Mu_y = str2double(v);
            continue;
        end

        if( strncmpi(q,'<H_c>',5))
            v = StripKey(q);
            FemmProblem.Materials(matind).H_c = str2double(v);
            continue;
        end

        if( strncmpi(q,'<J_re>',6))
            v = StripKey(q);
            FemmProblem.Materials(matind).J_re = str2double(v);
            continue;
        end

        if( strncmpi(q,'<J_im>',6))

            v = StripKey(q);

            if (FemmProblem.ProbInfo.Frequency ~= 0)
                FemmProblem.Materials(matind).J_im = str2double(v);
            end
            continue;
        end

        if( strncmpi(q,'<sigma>',7))

            v = StripKey(q);
            FemmProblem.Materials(matind).Sigma = str2double(v);
            continue;
        end

        if( strncmpi(q,'<phi_h>',7))

            v = StripKey(q);
            FemmProblem.Materials(matind).Phi_h = str2double(v);
            continue;
        end

        if( strncmpi(q,'<phi_hx>',8))

            v = StripKey(q);
            FemmProblem.Materials(matind).Phi_hx = str2double(v);
            continue;
        end

        if( strncmpi(q,'<phi_hy>',8))

            v = StripKey(q);
            FemmProblem.Materials(matind).Phi_hy = str2double(v);
            continue;
        end

        if( strncmpi(q,'<d_lam>',7))

            v = StripKey(q);
            FemmProblem.Materials(matind).d_lam = str2double(v);
            continue;
        end

        if( strncmpi(q,'<LamFill>',8))

            v = StripKey(q);
            FemmProblem.Materials(matind).LamFill = str2double(v);
            continue;
        end

        if( strncmpi(q,'<LamType>',9))

            v = StripKey(q);
            FemmProblem.Materials(matind).LamType = str2double(v);
            continue;
        end

        if( strncmpi(q,'<NStrands>',10))

            v = StripKey(q);
            FemmProblem.Materials(matind).NStrands = str2double(v);
            continue;
        end

        if( strncmpi(q,'<WireD>',7))

            v = StripKey(q);
            FemmProblem.Materials(matind).WireD = str2double(v);
            continue;
        end

        if( strncmpi(q,'<BHPoints>',10))

            v = StripKey(q);

            nbhpoints = str2double(v);

            if nbhpoints > 0

                for j = 1:nbhpoints

                    s = fgetl(fid);

                    FemmProblem.Materials(matind).BHPoints(j,:) = sscanf(s,'%f');

                end

            end

            continue;

        end

        if( strncmpi(q,'<endblock>',9))

            continue;

        end

        % Circuit Properties
        if( strncmpi(q,'<begincircuit>',14))

            [FemmProblem, circind] = addcircuit_mfemm(FemmProblem, '');

            continue;

        end

        if( strncmpi(q,'<circuitname>',13))

            v = StripKey(q);

            FemmProblem.Circuits(circind).Name = removequotes(v);

            continue;

        end

        if( strncmpi(q,'<totalamps_re>',14))

            v = StripKey(q);

            FemmProblem.Circuits(circind).TotalAmps_re = str2double(v);

            continue;

        end

        if( strncmpi(q,'<totalamps_im>',14))

            v = StripKey(q);

            if FemmProblem.ProbInfo.Frequency ~= 0
                FemmProblem.Circuits(circind).TotalAmps_im = str2double(v);
            end

            continue;

        end

        if( strncmpi(q,'<circuittype>',13))

            v = StripKey(q);

            FemmProblem.Circuits(circind).CircType = str2double(v);

            continue;

        end

        if( strncmpi(q,'<endcircuit>',12))

            continue;

        end
        
%         if strncmpi(q,'[BdryProps]',10)
%             
%             v = StripKey(q);
% 
%             nbdrys = round(str2double(v));
%             
%             for i = 1:nbdrys
% 
%                 fprintf(fp,'  <BeginBdry>\n');
%                 fprintf(fp,'    <BdryName> = \"%s\"\n', FemmProblem.BoundaryProps(i).Name);
%                 fprintf(fp,'    <BdryType> = %i\n',FemmProblem.BoundaryProps(i).BdryType);
%                 fprintf(fp,'    <A_0> = %.17g\n',FemmProblem.BoundaryProps(i).A_0);
%                 fprintf(fp,'    <A_1> = %.17g\n',FemmProblem.BoundaryProps(i).A_1);
%                 fprintf(fp,'    <A_2> = %.17g\n',FemmProblem.BoundaryProps(i).A_2);
%                 fprintf(fp,'    <Phi> = %.17g\n',FemmProblem.BoundaryProps(i).Phi);
%                 fprintf(fp,'    <c0> = %.17g\n',FemmProblem.BoundaryProps(i).c0);
%                 fprintf(fp,'    <c0i> = %.17g\n',FemmProblem.BoundaryProps(i).c0i);
%                 fprintf(fp,'    <c1> = %.17g\n',FemmProblem.BoundaryProps(i).c1);
%                 fprintf(fp,'    <c1i> = %.17g\n',FemmProblem.BoundaryProps(i).c1i);
%                 fprintf(fp,'    <Mu_ssd> = %.17g\n',FemmProblem.BoundaryProps(i).Mu_ssd);
%                 fprintf(fp,'    <Sigma_ssd> = %.17g\n',FemmProblem.BoundaryProps(i).Sigma_ssd);
%                 fprintf(fp,'  <EndBdry>\n');
% 
%             end
% 
%         end
        
        % Points list;
        if(strncmpi(q,'[numpoints]',11))

            v = StripKey(q);

            npoints = round(str2double(v));

            for i = 1:npoints

                s = fgetl(fid);

                C = textscan(s, '%f\t%f\t%d\t%d');

                nodeprops.InGroup = C{4};

                if C{3} > 0

                    nodeprops.PointPropName = FemmProblem.PointProps(C{3}).Name;

                end

                FemmProblem = addnodes_mfemm(FemmProblem, C{1}, C{2}, nodeprops);

                clear nodeprops;

            end

        end

        % read in segment list
        if(strncmpi(q,'[numsegments]',13))

            v = StripKey(q);

            nsegments = round(str2double(v));

            for i = 1:nsegments

                s = fgetl(fid);

                C = textscan(s, '%d\t%d\t%f\t%d\t%d\t%f');

                segprops.MaxSideLength = C{3};

                segprops.BoundaryMarker = C{4};
                
                segprops.Hidden = C{5};
                
                segprops.InGroup = C{6};

                FemmProblem = addsegments_mfemm(FemmProblem, C{1}, C{2}, segprops);

                clear segprops;

            end

        end

        % read in arc segment list
        if(strncmpi(q,'[numarcsegments]',13))
            v = StripKey(q);

            narcsegments = round(str2double(v));

            for i = 1:narcsegments

                s = fgetl(fid);

                C = textscan(s,'%d\t%d\t%f\t%f\t%d\t%d\t%d');

                arcsegprops.MaxSegDegrees = C{4};

                arcsegprops.BoundaryMarker = C{5};

                arcsegprops.Hidden = C{6};
                
                arcsegprops.InGroup = C{7};

                FemmProblem = addarcsegments_mfemm(FemmProblem, C{1}, C{2}, C{3}, arcsegprops);

                clear arcsegprops;

            end

        end


        % read in list of holes;
        if(strncmpi(q,'[numholes]',13))

            v = StripKey(q);

            nholes = round(str2double(v));

            if nholes > 0

                blk.BlockType = -1;
                blk.MaxArea = 0;

                for i = 1:nholes

                    s = fgetl(fid);

                    C = textscan(s,'%f\t%f');

                    FemmProblem.Holes(i).x = C{1};
                    FemmProblem.Holes(i).y = C{2};

                    %	blocklist.Add(blk);
                    %  don't add holes to the list
                    %  of block labels because it messes up the
                    %  number of block labels.
                end
            end

        end

        % read in regional attributes
        if(strncmpi(q,'[numblocklabels]',13))

            v = StripKey(q);

            nregionalattrib = str2double(v);

            for i = 1:nregionalattrib

                s = fgetl(fid);

                C = textscan(s,'%f\t%f\t%d\t%f\t%d\t%f\t%d\t%d\t%d\t%s');

                %some defaults
                FemmProblem.BlockLabels(i).Coords = [C{1}, C{2}];
                FemmProblem.BlockLabels(i).BlockType = C{3};
                FemmProblem.BlockLabels(i).MaxArea = C{4};
                FemmProblem.BlockLabels(i).InCircuit = C{5};
                FemmProblem.BlockLabels(i).MagDir = C{6};
                FemmProblem.BlockLabels(i).InGroup = C{7};
                FemmProblem.BlockLabels(i).Turns = C{8};
                FemmProblem.BlockLabels(i).IsExternal = C{9};
                FemmProblem.BlockLabels(i).MagDirFctn = C{10};

                if FemmProblem.BlockLabels(i).MaxArea < 0
                    FemmProblem.BlockLabels(i).MaxArea = 0;
                else
                    % convert from a side length to area constraint
                    FemmProblem.BlockLabels(i).MaxArea = pi * FemmProblem.BlockLabels(i).MaxArea * FemmProblem.BlockLabels(i).MaxArea / 4;
                end

            end

            continue;

        end

        if strncmpi(q,'[solution]',10) 
            hassolution = true;
            break;
        else
            hassolution = false;
        end

    end
    
    if isfield(FemmProblem, 'Segments')
        % convert the boudary marker number in the segment list to names
        for i = 1:numel(FemmProblem.Segments)
            
            if FemmProblem.Segments(i).BoundaryMarker == 0
                FemmProblem.Segments(i).BoundaryMarker = '';
            else
                FemmProblem.Segments(i).BoundaryMarker = ...
                    FemmProblem.BoundaryProps(FemmProblem.Segments(i).BoundaryMarker).Name;
            end
            
        end
    end
    
    if isfield(FemmProblem, 'ArcSegments')
        % convert the boudary marker number in the arc segment list to names
        for i = 1:numel(FemmProblem.ArcSegments)
            
            if FemmProblem.ArcSegments(i).BoundaryMarker == 0
                FemmProblem.ArcSegments(i).BoundaryMarker = '';
            else
                FemmProblem.ArcSegments(i).BoundaryMarker = ...
                    FemmProblem.BoundaryProps(FemmProblem.ArcSegments(i).BoundaryMarker).Name;
            end
            
        end
    end
    
    if isfield(FemmProblem, 'BlockLabels')
       for i = 1:numel(FemmProblem.BlockLabels)
           FemmProblem.BlockLabels(i).BlockType = FemmProblem.Materials(FemmProblem.BlockLabels(i).BlockType).Name; 
       end
    end

    if hassolution

        % read in mesh node locations and nodal values

        % get the number of nodes to read in
        s = fgetl(fid);
        
        nnodes = cell2mat(textscan(s,'%d\n'));
        
        for i = 1:nnodes

            % get the string containing the node info
            s = fgetl(fid);

            if FemmProblem.ProbInfo.Frequency ~= 0
                % read in four values from the string, the x and y
                % coordinates of the node and the real and imaginary
                % values of A at the node
                Solution.MeshNodes(i,1:4) = sscanf(s,'%f\t%f\t%f\t%f');
            else
                % read in three values from the string, the x and y
                % coordinates of the node and the value of A at the node
                Solution.MeshNodes(i,1:3) = sscanf(s,'%f\t%f\t%f');
                Solution.MeshNodes(i,4) = 0;
            end

        end

        % read in the element data

        % get the number of elements to read in
        s = fgetl(fid);
        nelms = cell2mat(textscan(s,'%d\n'));

        for i = 1:nelms

            s = fgetl(fid);

            vals = sscanf(s,'%d\t%d\t%d\t%d');

            Solution.Vertices(i,:) = vals(1:3);
            Solution.LabelNum(i,1) = vals(4);
%             Solution.LabelStr{i} = FemmProblem.BlockLabels(vals(4)+1).BlockType;

        end

        % get the number of circuits to read in
        s = fgetl(fid);
        ncircs = cell2mat(textscan(s,'%d'));

        for i = 1:ncircs
            
            if i == 1
                Solution.Circuits = struct();
            end

            s = fgetl(fid);

            if FemmProblem.ProbInfo.Frequency == 0

                cvals = sscanf(s,'%d\t%f');

                Solution.Circuits(i).Case = cvals(1);

                if Solution.Circuits(i).Case == 0
                    Solution.Circuits(i).dVolts = cvals(2);
                else
                    Solution.Circuits(i).J = cvals(2);
                end

            else

                cvals = sscanf(s,'%d\t%f\t%f');

                Solution.Circuits(i).Case = cvals(1);

                if Solution.Circuits(i).Case == 0
                    Solution.Circuits(i).dVolts = cvals(2) + 1j*cvals(3);
                else
                    Solution.Circuits(i).J = cvals(2) + 1j*cvals(3);
                end

            end

        end

    else
        Solution = [];
    end
    
    fclose(fid);
    
    Solution = postprocsolution(FemmProblem, Solution);

end


function v = StripKey(q)

    [start_idx, end_idx, extents, matches, tokens, names, splits] = regexp(q, '(?<=\=).*');

    v = strtrim(matches{1});

end


function str = removequotes(v)

    [tokenstring] = regexp(v, '"(.*)"', 'tokens');

    str = char(tokenstring{1});

end


function [Solution] = postprocsolution(FemmProblem, Solution)

    % convert depth from whatever the supplied problem units were to meters 
    % for internal computations
    if (FemmProblem.ProbInfo.Depth == -1)
        FemmProblem.ProbInfo.Depth = 1; 
    else
        FemmProblem.ProbInfo.Depth = length2metres_mfemm(FemmProblem.ProbInfo.Depth, FemmProblem.ProbInfo.LengthUnits);
    end

	% Calculate triangle element centroids 
    % triangle centroid given by = (1/3 (x1+x2+x3), 1/3 (y1+y2+y3))  
    % see wikipedia page for further info:
    % http://en.wikipedia.org/wiki/Centroid#Of_triangle_and_tetrahedron
    for i = 1:size(Solution.Vertices,1)
        Solution.TriCentroids(i,1:2) = [sum(Solution.MeshNodes(Solution.Vertices(i,:)+1,1)),  ...
                                        sum(Solution.MeshNodes(Solution.Vertices(i,:)+1,2))];
    end
    Solution.TriCentroids = Solution.TriCentroids ./ 3;
    
    % Compute the element radii
    rsqrx = realpow(  reshape(Solution.MeshNodes(Solution.Vertices(:)+1,1), size(Solution.Vertices)) ...
                    - reshape(Solution.TriCentroids(Solution.Vertices(:)+1,1), size(Solution.Vertices)), ...
                    2);
                    
    rsqry = realpow(  reshape(Solution.MeshNodes(Solution.Vertices(:)+1,2), size(Solution.Vertices)) ...
                    - reshape(Solution.TriCentroids(Solution.Vertices(:)+1,2), size(Solution.Vertices)), ...
                    2);                    
    
    Solution.TriRadii =  max(rsqrx + rsqry, [], 2);

    % calculate the magnetization direction for each element 
    Solution = calcelementmagdir(FemmProblem, Solution);
   
    
% 	% Find flux density in each element;
% 	for(i=0;i<meshelem.GetSize();i++) GetElementB(meshelem[i]);
% 
% 	% Find extreme values of A;
% 	A_Low=meshnode[0].A.re; A_High=meshnode[0].A.re;
% 	for(i=1;i<meshnode.GetSize();i++)
% 	{
% 		if (meshnode[i].A.re>A_High) A_High=meshnode[i].A.re;
% 		if (meshnode[i].A.re<A_Low)  A_Low =meshnode[i].A.re;
% 
% 		if(Frequency!=0)
% 		{
% 			if (meshnode[i].A.im<A_Low)  A_Low =meshnode[i].A.im;
% 			if (meshnode[i].A.im>A_High) A_High=meshnode[i].A.im;
% 		}
% 	}
% 	% save default values for extremes of A
% 	A_lb=A_Low;
% 	A_ub=A_High;

% 	if(Frequency!=0){ % compute frequency-dependent permeabilities for linear blocks;
% 
% 		CComplex deg45; deg45=1+I;
% 		CComplex K,halflag;
% 		double ds;
% 		double w=2.*PI*Frequency;
% 
% 		for(k=0;k<blockproplist.GetSize();k++){
% 		if (blockproplist[k].LamType==0){
% 			blockproplist[k].mu_fdx = blockproplist[k].mu_x*
% 									  exp(-I*blockproplist[k].Theta_hx*PI/180.);
% 			blockproplist[k].mu_fdy = blockproplist[k].mu_y*
% 									  exp(-I*blockproplist[k].Theta_hy*PI/180.);
% 
% 			if(blockproplist[k].Lam_d!=0){
% 				halflag=exp(-I*blockproplist[k].Theta_hx*PI/360.);
% 				ds=sqrt(2./(0.4*PI*w*blockproplist[k].Cduct*blockproplist[k].mu_x));
% 				K=halflag*deg45*blockproplist[k].Lam_d*0.001/(2.*ds);
% 				if (blockproplist[k].Cduct!=0)
% 				{
% 					blockproplist[k].mu_fdx=(blockproplist[k].mu_fdx*tanh(K)/K)*
% 					blockproplist[k].LamFill+(1.-blockproplist[k].LamFill);
% 				}
% 				else{
% 					blockproplist[k].mu_fdx=(blockproplist[k].mu_fdx)*
% 					blockproplist[k].LamFill+(1.-blockproplist[k].LamFill);
% 				}
% 
% 				halflag=exp(-I*blockproplist[k].Theta_hy*PI/360.);
% 				ds=sqrt(2./(0.4*PI*w*blockproplist[k].Cduct*blockproplist[k].mu_y));
% 				K=halflag*deg45*blockproplist[k].Lam_d*0.001/(2.*ds);
% 				if (blockproplist[k].Cduct!=0)
% 				{
% 					blockproplist[k].mu_fdy=(blockproplist[k].mu_fdy*tanh(K)/K)*
% 					blockproplist[k].LamFill+(1.-blockproplist[k].LamFill);
% 				}
% 				else{
% 					blockproplist[k].mu_fdy=(blockproplist[k].mu_fdy)*
% 					blockproplist[k].LamFill+(1.-blockproplist[k].LamFill);
% 				}
% 			}
% 		}
        

end

                
function Solution = calcelementmagdir(FemmProblem, Solution)
% calculate the magnetisation direction in each element
    
    Solution.MagDir = zeros(size(Solution.TriRadii, 1), 1);
    
    for i = 1:numel(FemmProblem.BlockLabels)
        if isempty(FemmProblem.BlockLabels(i).MagDirFctn)
            Solution.MagDir(Solution.LabelNum == i,1) = FemmProblem.BlockLabels(i).MagDir;
        else
            x = Solution.TriCentroids(Solution.LabelNum == i,1);
            y = Solution.TriCentroids(Solution.LabelNum == i,2);
            % calculate theta in degrees
            theta = atan2(y,x) .* 180./pi;
            % calculate the distance from the 
            R = magn([x,y],2);
            % convert the string containing the magnetization direction
            % function to an inline matlab function
            magdirfcn = inline(FemmProblem.BlockLabels(i).MagDirFctn, 'x', 'y', 'theta', 'R');
            % vectorise the mag direction function
            magdirfcn = vectorize(magdirfcn);
            % evaluate the inline function
            Solution.MagDir(Solution.LabelNum == i,1) = magdirfcn(x, y, theta, R);
        end
    end
    
end