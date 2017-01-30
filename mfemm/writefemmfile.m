function writefemmfile(filename, FemmProblem, varargin)
% writefemmfile: creates a .fem problem file in the same format as used by
% FEMM, the eletromagnetic finite element analysis solver
%
% Syntax
% 
% writefemmfile(filename, FemmProblem)
% 
% Input
% 
% writefemmfile(filename, FemmProblem) writes the finite element problem
% description contained in the 'FemmProblem' structure to the file
% 'filename'
%
% FemmProblem is a structure containing a complex nested series of
% structure arrays which form the FEA problem description. 
%
% FemmProblem can contain the following fields:
%
%   ProbInfo
%   PointProps (optional)
%   BoundaryProps 
%   Materials 
%   Circuits (optional)
%   Nodes
%   Segments (optional)
%   ArcSegments (optional)
%   BlockLabels 
%
%   ProbInfo must be a scalar structure containing information about the
%   problem. It must have the following fields:
%
%       Frequency - a scalar value giving the AC frequecy of the problem
%         e.g. 0
%
%       Precision - a scalar value giving the desired precesion of the
%         solution, e.g. 1e-8
%
%       MinAngle - a scalar value setting the minimum allowed angle for the
%         mesh triangles, e.g. 30
%
%       Depth - For planar problems, the depth of the simulation, will be
%         ignored for axisymmetric problems
%
%       LengthUnits - either a string containing the length units of the
%         simulaton, or a scalar value coding for one of the strings. This
%         can be one of: 'millimeters', 'centimeters', 'meters', 'mils',
%         'microns', or 'inches'. The same values can be set by using the
%         scalar values 1, 2, 3, 4, 5, or any other value to yield 'inches'
%
%       ProblemType - a scalar value determining which type of problem is
%         to be solved. If a sclar, 0 for planar, 1 for axisymmetric. If a
%         string this can by any number of the characters from the strings
%         'planar' and 'axisymmetric', in any case e.g. 'p', 'pl', 'PlA'
%         etc. all choose a 'planar' symulation. If an axisymmetric problem
%         is specified, external regions can also optionally be specified
%         using the fields, extZo, extRo and extRi
%
%       ACSolver - Can be 0 or 1 (TODO, find out what each is!)
%
%       ForceMaxMesh - true or false, if evaluating to true, the user's
%         choice of mesh can be overriden by the mesher and replaced with
%         an upper default limit for a given area. If false the User's mesh
%         size is always used. Defaults to false of not supplied.
%
%   PointProps is a structure array, each member of which contains
%   information on point properties which can be applied to nodes in the
%   simulation. The structures must have the following fields:
%
%
%   BoundaryProps 
%
%   Materials is a structure containing information on the materials
%   used in the simulation. It must have the following fields:
%
%   Circuits 
%
%   Nodes
%
%   Segments 
%
%   ArcSegments 
%
%   BlockLabels        
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http://www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

    Inputs.FileType = '';
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);
    
    % use a structure to create an enum of file types
    ftype.magnetics = 0;
    ftype.heatflow = 1;

    % use a structure to map the file extensions to problem types
    fexmap.fem = 'magnetics';
    fexmap.feh = 'heatflow';
    
    if isempty (Inputs.FileType) 
        % try to determine from the file name extension
        [~, ~, fext] = fileparts (filename);
        
        
        if ~isempty(fext) && isfield(fexmap, fext(2:end))
            domaintype = fexmap.(fext(2:end));
        else
            error ('MFEMM:badfiletype', ...
                   'The output file name extension is not recognised and you have not explicitly set the file type.');
        end
    end
    
    % check to see if we are ready to write a datafile;
    [fp, message] = fopen(filename, 'wt');
    if fp == -1
        error('MFEMM:writefemmfile:nofileopen', message);
    end
    
    C = onCleanup(@()fclose(fp));

    if numel(FemmProblem.ProbInfo) > 1
        error('MFEMM:writefemmfile:probinfonum', 'FemmProblem.ProbInfo must be a scalar structure.');
    end
        
    switch ftype.(domaintype)
        
        case ftype.magnetics
            fprintf(fp,'[Format]      =  4.0\n');
            fprintf(fp,'[Frequency]   =  %.17g\n',FemmProblem.ProbInfo.Frequency);
        case ftype.heatflow
            fprintf(fp,'[Format]      =  1.0\n');
            fprintf(fp,'[PrevSoln] = "%s"\n', FemmProblem.ProbInfo.PrevSolutionFile);
            fprintf(fp,'[dT] = %.17g\n', FemmProblem.ProbInfo.dT);
    end
    
    fprintf(fp,'[Precision]   =  %.17g\n',FemmProblem.ProbInfo.Precision);
    fprintf(fp,'[MinAngle]    =  %.17g\n',FemmProblem.ProbInfo.MinAngle);
    fprintf(fp,'[Depth]       =  %.17g\n',FemmProblem.ProbInfo.Depth);
    fprintf(fp,'[LengthUnits] =  ');
    
    if isnumeric(FemmProblem.ProbInfo.LengthUnits)

        switch FemmProblem.ProbInfo.LengthUnits

            case 1
                fprintf(fp,'millimeters\n');
            case 2
                fprintf(fp,'centimeters\n');
            case 3
                fprintf(fp,'meters\n');
            case 4
                fprintf(fp,'mils\n');
            case 5
                fprintf(fp,'microns\n');
            otherwise
                fprintf(fp,'inches\n');
        end

    elseif ischar(FemmProblem.ProbInfo.LengthUnits)

        switch FemmProblem.ProbInfo.LengthUnits

            case {'millimeters', 'millimetres'}
                fprintf(fp,'millimeters\n');
            case {'centimeters', 'centimetres'}
                fprintf(fp,'centimeters\n');
            case {'meters', 'metres'}
                fprintf(fp,'meters\n');
            case {'mils'}
                fprintf(fp,'mils\n');
            case {'microns'}
                fprintf(fp,'microns\n');
            otherwise
                fprintf(fp,'inches\n');
        end

    else
        error('MFEMM:writefemmfile:badlength', ...
              'LengthUnits must be a string or scalar value, see help for details')
    end


    if ischar(FemmProblem.ProbInfo.ProblemType)
        
        if strncmpi(FemmProblem.ProbInfo.ProblemType, ...
                        'planar', length(FemmProblem.ProbInfo.ProblemType))
           
            fprintf(fp,'[ProblemType] =  planar\n');
            
        elseif strncmpi(FemmProblem.ProbInfo.ProblemType, ...
                        'axisymmetric', length(FemmProblem.ProbInfo.ProblemType))
               
            fprintf(fp,'[ProblemType] =  axisymmetric\n');

            if (isfield(FemmProblem.ProbInfo, 'extRo') && FemmProblem.ProbInfo.extRo ~= 0) && ...
                (isfield(FemmProblem.ProbInfo, 'extRi') && FemmProblem.ProbInfo.extRi ~= 0) && ...
                (isfield(FemmProblem.ProbInfo, 'extZi') && FemmProblem.ProbInfo.extZo ~= 0)
            
                fprintf(fp, '[extZo] = %.17g\n', FemmProblem.ProbInfo.extZo);
                fprintf(fp, '[extRo] = %.17g\n', FemmProblem.ProbInfo.extRo);
                fprintf(fp, '[extRi] = %.17g\n', FemmProblem.ProbInfo.extRi);

            end
            
        else
            error('MFEMM:writefemmfile:badprobtype', 'Unrecognised problem type')
        end
        
    else
        
        if FemmProblem.ProbInfo.ProblemType == 0

            fprintf(fp,'[ProblemType] =  planar\n');

        else

            fprintf(fp,'[ProblemType] =  axisymmetric\n');

            if (isfield(FemmProblem.ProbInfo, 'extRo') && FemmProblem.ProbInfo.extRo ~= 0) && ...
                (isfield(FemmProblem.ProbInfo, 'extRi') && FemmProblem.ProbInfo.extRi ~= 0) && ...
                (isfield(FemmProblem.ProbInfo, 'extZi') && FemmProblem.ProbInfo.extZo ~= 0)
            
                fprintf(fp, '[extZo] = %.17g\n', FemmProblem.ProbInfo.extZo);
                fprintf(fp, '[extRo] = %.17g\n', FemmProblem.ProbInfo.extRo);
                fprintf(fp, '[extRi] = %.17g\n', FemmProblem.ProbInfo.extRi);

            end
        end
    end
    
    if ~isfield(FemmProblem.ProbInfo, 'Coords')
        FemmProblem.ProbInfo.Coords = 'cartesian';
    end
    
    if ischar(FemmProblem.ProbInfo.Coords)

        if strncmpi(FemmProblem.ProbInfo.Coords, ...
                'cartesian', length(FemmProblem.ProbInfo.Coords))

            fprintf(fp, '[Coordinates] =  cartesian\n');

        elseif strncmpi(FemmProblem.ProbInfo.Coords, ...
                'polar', length(FemmProblem.ProbInfo.Coords))

            fprintf(fp, '[Coordinates] =  polar\n');

        else
            error('MFEMM:writefemmfile:badcoordtype', 'Unrecognised coordinate type')
        end


    elseif isscalar(FemmProblem.ProbInfo.Coords)
        if FemmProblem.ProbInfo.Coords == 0
            fprintf(fp, '[Coordinates] =  cartesian\n');
        else
            fprintf(fp, '[Coordinates] =  polar\n');
        end
    end

    fprintf(fp, '[ACSolver]    =  %i\n', FemmProblem.ProbInfo.ACSolver);

    if isfield(FemmProblem.ProbInfo, 'Comment')
        s = FemmProblem.ProbInfo.Comment;
    else
        s = '';
    end
    
    if ~isfield (FemmProblem.ProbInfo, 'ForceMaxMesh')
        FemmProblem.ProbInfo.ForceMaxMesh = false;
    end
    
    if FemmProblem.ProbInfo.ForceMaxMesh == true
        fprintf(fp,'[forcemaxmesh] =  1\n');
    else
        fprintf(fp,'[forcemaxmesh] =  0\n');
    end

    if ~isfield (FemmProblem.ProbInfo, 'SmartMesh')
        FemmProblem.ProbInfo.SmartMesh = true;
    end

    if FemmProblem.ProbInfo.SmartMesh == true
        fprintf(fp,'[dosmartmesh] =  1\n');
    else
        fprintf(fp,'[dosmartmesh] =  0\n');
    end
    
    fprintf(fp, '[Comment]     =  "%s"\n', s);

    % write out point properties 
    if ~isfield(FemmProblem, 'PointProps')
        FemmProblem.PointProps = [];
    end
    
    fprintf(fp,'[PointProps]   = %i\n', numel(FemmProblem.PointProps));
    for i = 1:numel(FemmProblem.PointProps)

        fprintf(fp,'  <BeginPoint>\n');
        fprintf(fp,'    <PointName> = "%s"\n',FemmProblem.PointProps(i).Name);
        
        switch ftype.(domaintype)
        
            case ftype.magnetics
        
                fprintf(fp,'    <I_re> = %.17g\n',FemmProblem.PointProps(i).I_re);
                fprintf(fp,'    <I_im> = %.17g\n',FemmProblem.PointProps(i).I_im);
                fprintf(fp,'    <A_re> = %.17g\n',FemmProblem.PointProps(i).A_re);
                fprintf(fp,'    <A_im> = %.17g\n',FemmProblem.PointProps(i).A_im);
        
            case ftype.heatflow
            
                fprintf(fp,'   <Tp> = %.17g\n',FemmProblem.PointProps(i).Tp);
                fprintf(fp,'   <qp> = %.17g\n',FemmProblem.PointProps(i).qp);

        end
        
        fprintf(fp,'  <EndPoint>\n');
    end

    if ~isfield(FemmProblem, 'BoundaryProps')
        FemmProblem.BoundaryProps = [];
    end
    
    % write out the boundary properties
    fprintf(fp,'[BdryProps]   = %i\n', numel(FemmProblem.BoundaryProps));
    for i = 1:numel(FemmProblem.BoundaryProps)

        fprintf(fp,'  <BeginBdry>\n');
        fprintf(fp,'    <BdryName> = \"%s\"\n', FemmProblem.BoundaryProps(i).Name);
        fprintf(fp,'    <BdryType> = %i\n',FemmProblem.BoundaryProps(i).BdryType);
        
        switch ftype.(domaintype)
        
            case ftype.magnetics
        
                fprintf(fp,'    <A_0> = %.17g\n',FemmProblem.BoundaryProps(i).A0);
                fprintf(fp,'    <A_1> = %.17g\n',FemmProblem.BoundaryProps(i).A1);
                fprintf(fp,'    <A_2> = %.17g\n',FemmProblem.BoundaryProps(i).A2);
                fprintf(fp,'    <Phi> = %.17g\n',FemmProblem.BoundaryProps(i).Phi);
                fprintf(fp,'    <c0> = %.17g\n',FemmProblem.BoundaryProps(i).c0);
                fprintf(fp,'    <c0i> = %.17g\n',FemmProblem.BoundaryProps(i).c0i);
                fprintf(fp,'    <c1> = %.17g\n',FemmProblem.BoundaryProps(i).c1);
                fprintf(fp,'    <c1i> = %.17g\n',FemmProblem.BoundaryProps(i).c1i);
                fprintf(fp,'    <Mu_ssd> = %.17g\n',FemmProblem.BoundaryProps(i).Mu_ssd);
                fprintf(fp,'    <Sigma_ssd> = %.17g\n',FemmProblem.BoundaryProps(i).Sigma_ssd);
                
             case ftype.heatflow
             
                fprintf(fp,'    <Tset> = %.17g\n', FemmProblem.BoundaryProps(i).Tset);
                fprintf(fp,'    <qs> = %.17g\n', FemmProblem.BoundaryProps(i).qs);
                fprintf(fp,'    <beta> = %.17g\n', FemmProblem.BoundaryProps(i).beta);
                fprintf(fp,'    <h> = %.17g\n', FemmProblem.BoundaryProps(i).h);
                fprintf(fp,'    <Tinf> = %.17g\n', FemmProblem.BoundaryProps(i).T0);
                
        end
        
        fprintf(fp,'  <EndBdry>\n');

    end

    if ~isfield(FemmProblem, 'Materials')
       FemmProblem.Materials = []; 
    end
    
    % write out the material properties
    fprintf(fp,'[BlockProps]  = %i\n', numel(FemmProblem.Materials));
    for i = 1:numel(FemmProblem.Materials)

        fprintf(fp,'  <BeginBlock>\n');
        fprintf(fp,'    <BlockName> = \"%s\"\n',FemmProblem.Materials(i).Name);
        
        switch ftype.(domaintype)
        
            case ftype.magnetics
            
                fprintf(fp,'    <Mu_x> = %.17g\n',FemmProblem.Materials(i).Mu_x);
                fprintf(fp,'    <Mu_y> = %.17g\n',FemmProblem.Materials(i).Mu_y);
                fprintf(fp,'    <H_c> = %.17g\n',FemmProblem.Materials(i).H_c);
                fprintf(fp,'    <H_cAngle> = %.17g\n',FemmProblem.Materials(i).H_cAngle);
                fprintf(fp,'    <J_re> = %.17g\n',FemmProblem.Materials(i).J_re);
                fprintf(fp,'    <J_im> = %.17g\n',FemmProblem.Materials(i).J_im);
                fprintf(fp,'    <Sigma> = %.17g\n',FemmProblem.Materials(i).Sigma);
                fprintf(fp,'    <d_lam> = %.17g\n',FemmProblem.Materials(i).d_lam);
                fprintf(fp,'    <Phi_h> = %.17g\n',FemmProblem.Materials(i).Phi_h);
                fprintf(fp,'    <Phi_hx> = %.17g\n',FemmProblem.Materials(i).Phi_hx);
                fprintf(fp,'    <Phi_hy> = %.17g\n',FemmProblem.Materials(i).Phi_hy);
                fprintf(fp,'    <LamType> = %i\n',FemmProblem.Materials(i).LamType);
                fprintf(fp,'    <LamFill> = %.17g\n',FemmProblem.Materials(i).LamFill);
                fprintf(fp,'    <NStrands> = %i\n',FemmProblem.Materials(i).NStrands);
                fprintf(fp,'    <WireD> = %.17g\n',FemmProblem.Materials(i).WireD);
                fprintf(fp,'    <BHPoints> = %i\n',size(FemmProblem.Materials(i).BHPoints,1));

                % write the materials B-H points, these should be stored in a
                % two-column matrix where column 1 is the B points and col 2 the H
                % points
                for j = 1:size(FemmProblem.Materials(i).BHPoints, 1)

                    fprintf(fp,'      %.17g\t%.17g\n', ...
                        FemmProblem.Materials(i).BHPoints(j,1), ...
                        FemmProblem.Materials(i).BHPoints(j,2));

                end
                
            case ftype.heatflow
            
                fprintf(fp,'    <Kx> = %.17g\n', FemmProblem.Materials(i).Kx);
                fprintf(fp,'    <Ky> = %.17g\n', FemmProblem.Materials(i).Ky);
                fprintf(fp,'    <Kt> = %.17g\n', FemmProblem.Materials(i).Kt);
                fprintf(fp,'    <qv> = %.17g\n', FemmProblem.Materials(i).qv);
                fprintf(fp,'    <TKPoints> = %i\n', size(FemmProblem.Materials(i).TKPoints,1));
                
                % write the materials B-H points, these should be stored in a
                % two-column matrix where column 1 is the B points and col 2 the H
                % points
                for j = 1:size(FemmProblem.Materials(i).TKPoints, 1)

                    fprintf(fp,'      %.17g\t%.17g\n', ...
                        FemmProblem.Materials(i).TKPoints(j,1), ...
                        FemmProblem.Materials(i).TKPoints(j,2));

                end
        
        end

        fprintf(fp,'  <EndBlock>\n');

    end

    switch ftype.(domaintype)
        
        case ftype.magnetics
        
            % magnetics problems have circuits
            if ~isfield(FemmProblem, 'Circuits')
                FemmProblem.Circuits = [];
            end
            
            % write out the circuit properties
            fprintf(fp,'[CircuitProps]  = %i\n',numel(FemmProblem.Circuits));
            for i = 1:numel(FemmProblem.Circuits)

                fprintf(fp,'  <BeginCircuit>\n');
                fprintf(fp,'    <CircuitName> = \"%s\"\n',FemmProblem.Circuits(i).Name);
                fprintf(fp,'    <TotalAmps_re> = %.17g\n',FemmProblem.Circuits(i).TotalAmps_re);
                fprintf(fp,'    <TotalAmps_im> = %.17g\n',FemmProblem.Circuits(i).TotalAmps_im);

                if ischar(FemmProblem.Circuits(i).CircType)

                    if strncmpi(FemmProblem.Circuits(i).CircType, ...
                            'series', length(FemmProblem.Circuits(i).CircType))

                        FemmProblem.Circuits(i).CircType = 1;

                    elseif strncmpi(FemmProblem.Circuits(i).CircType, ...
                            'parallel', length(FemmProblem.Circuits(i).CircType))

                        FemmProblem.Circuits(i).CircType = 0;

                    else
                        error('MFEMM:writefemmfile:badcircuittype', 'Unrecognised circuit type')
                    end

                elseif isscalar(FemmProblem.Circuits(i).CircType)
                    if ~any(FemmProblem.Circuits(i).CircType == [0,1])
                        error('MFEMM:writefemmfile:badcircuittype', 'Unrecognised circuit type')
                    end
                end

                fprintf(fp,'    <CircuitType> = %i\n',FemmProblem.Circuits(i).CircType);
                fprintf(fp,'  <EndCircuit>\n');

            end
            
        case ftype.heatflow
        
            % heat flow problems have conductors
            if ~isfield(FemmProblem, 'Conductors')
                FemmProblem.Conductors = [];
            end
            
            fprintf(fp,'[ConductorProps]  = %i\n',numel(FemmProblem.Conductors));
            
            for i = 1:numel(FemmProblem.Conductors)
            
                fprintf(fp,'  <BeginConductor>\n'); 
                
                fprintf(fp,'    <ConductorName> = \"%s\"\n',FemmProblem.Conductors(i).ConductorName);
                fprintf(fp,'    <Tc> = %.17g\n', FemmProblem.Conductors(i).Tc);
                fprintf(fp,'    <qc> = %.17g\n', FemmProblem.Conductors(i).qc);
                    
                if ischar(FemmProblem.Conductors(i).ConductorType)

                    if strncmpi(FemmProblem.Conductors(i).ConductorType, ...
                            'temperature', length(FemmProblem.Conductors(i).ConductorType))

                        FemmProblem.Conductors(i).ConductorType = 1;

                    elseif strncmpi(FemmProblem.Conductors(i).ConductorType, ...
                            'flux', length(FemmProblem.Conductors(i).ConductorType))

                        FemmProblem.Conductors(i).ConductorType = 0;

                    else
                        error('MFEMM:writefemmfile:badconductortype', 'Unrecognised conductor type')
                    end

                elseif isscalar(FemmProblem.Conductors(i).ConductorType)
                    if ~any(FemmProblem.Conductors(i).ConductorType == [0,1])
                        error('MFEMM:writefemmfile:badconductortype', 'Unrecognised conductor type')
                    end
                end
       
                fprintf(fp,'  <EndConductor>\n');
            
            end
            
    end

    if ~isfield(FemmProblem, 'Nodes')
        FemmProblem.Nodes = [];
    end
    
    % write out node list
    fprintf(fp,'[NumPoints] = %i\n',numel(FemmProblem.Nodes));
    for i = 1:numel(FemmProblem.Nodes)

        t = 0;

        switch ftype.(domaintype)
            
            case ftype.magnetics
            
                if isfield(FemmProblem, 'NodeProps')
                    for j = 1:numel(FemmProblem.NodeProps)
                        if isequal(FemmProblem.NodeProps(j).Name, FemmProblem.Nodes(i).PointPropName)
                            t = j;
                        end
                    end
                end
                
                fprintf(fp,'%.17g\t%.17g\t%i\t%i\n', ...
                        FemmProblem.Nodes(i).Coords(1), ...
                        FemmProblem.Nodes(i).Coords(2), ...
                        t, ...
                        FemmProblem.Nodes(i).InGroup);
                  
            case ftype.heatflow
            
                if isfield(FemmProblem, 'NodeProps')
                    for j = 1:numel(FemmProblem.NodeProps)
                        if isequal(FemmProblem.NodeProps(j).Name, FemmProblem.Nodes(i).PointPropName)
                            t = j;
                        end
                    end
                end
                
                fprintf(fp,'%.17g\t%.17g\t%i\t%i\t', ...
                        FemmProblem.Nodes(i).Coords(1), ...
                        FemmProblem.Nodes(i).Coords(2), ...
                        t, ...
                        FemmProblem.Nodes(i).InGroup);
                
                t = 0;
                
                if isfield (FemmProblem, 'Conductors')
                    for j = 1:numel(FemmProblem.Conductors)
                        if isequal(FemmProblem.Conductors(j).Name, FemmProblem.Nodes(i).InConductor)
                            t = j;
                        end
                    end
                end
                
                fprintf(fp,'%i\n', t);
                
        end
        


    end

    if ~isfield(FemmProblem, 'Segments')
        FemmProblem.Segments = [];
    end
    
    % write out the segments
    fprintf(fp,'[NumSegments] = %i\n', numel(FemmProblem.Segments));
    for i = 1:numel(FemmProblem.Segments)
        
        t = 0;

        if isfield(FemmProblem.Segments(i), 'BoundaryMarker')
            for j = 1:numel(FemmProblem.BoundaryProps)
                if isequal(FemmProblem.BoundaryProps(j).Name, FemmProblem.Segments(i).BoundaryMarker)
                    t = j;
                end
            end
        end
        
        fprintf(fp,'%i\t%i\t',FemmProblem.Segments(i).n0, FemmProblem.Segments(i).n1);

        if FemmProblem.Segments(i).MaxSideLength < 0
            fprintf(fp,'-1\t');
        else
            fprintf(fp,'%.17g\t', FemmProblem.Segments(i).MaxSideLength);
        end

        fprintf(fp,'%i\t%i\t%i', t, FemmProblem.Segments(i).Hidden, FemmProblem.Segments(i).InGroup);
        
        switch ftype.(domaintype)
            
            case ftype.magnetics
                fprintf(fp,'\n');
            
            case ftype.heatflow
            
                t = 0;
                
                if isfield (FemmProblem, 'Conductors')
                    for j = 1:numel(FemmProblem.Conductors)
                        if isequal(FemmProblem.Conductors(j).Name, FemmProblem.Segments(i).InConductor)
                            t = j;
                        end
                    end
                end
                
                fprintf(fp,'%i\n', t);
            
        end
    end

    if ~isfield(FemmProblem, 'ArcSegments')
        FemmProblem.ArcSegments = [];
    end
    
    % write out arc segment list
    fprintf(fp,'[NumArcSegments] = %i\n', numel(FemmProblem.ArcSegments));
    for i = 1:numel(FemmProblem.ArcSegments)

        t = 0;
        if isfield(FemmProblem.ArcSegments(i), 'BoundaryMarker')
            for j = 1:numel(FemmProblem.BoundaryProps)
                if isequal(FemmProblem.BoundaryProps(j).Name, FemmProblem.ArcSegments(i).BoundaryMarker)
                    t = j;
                end
            end
        end
        
        fprintf(fp,'%i\t%i\t%.17g\t%.17g\t%i\t%i\t%i', ...
            FemmProblem.ArcSegments(i).n0, ...
            FemmProblem.ArcSegments(i).n1, ...
            FemmProblem.ArcSegments(i).ArcLength, ...
            FemmProblem.ArcSegments(i).MaxSegDegrees, ...
            t, ...
            FemmProblem.ArcSegments(i).Hidden, ...
            FemmProblem.ArcSegments(i).InGroup);

        switch ftype.(domaintype)
            
            case ftype.magnetics
            
                fprintf(fp,'\n');
            
            case ftype.heatflow
            
                t = 0;
                
                if isfield (FemmProblem, 'Conductors')
                    for j = 1:numel(FemmProblem.Conductors)
                        if isequal(FemmProblem.Conductors(j).Name, FemmProblem.ArcSegments(i).InConductor)
                            t = j;
                        end
                    end
                end
                
                fprintf(fp,'\t%i\n', t);
            
        end
    end

    if ~isfield(FemmProblem, 'BlockLabels')
        FemmProblem.BlockLabels = [];
    end 
    
    % write out list of holes
    nholes = 0;
    for i = 1:numel(FemmProblem.BlockLabels)
        if isequal(FemmProblem.BlockLabels(i).BlockType, '<No Mesh>')
            nholes = nholes + 1;
        end
    end
    
    fprintf(fp,'[NumHoles] = %i\n',nholes);
    
    k = 0;
    for i = 1:numel(FemmProblem.BlockLabels)

        if isequal(FemmProblem.BlockLabels(i).BlockType, '<No Mesh>')

            fprintf(fp,'%.17g\t%.17g\t%i\n', ...
                FemmProblem.BlockLabels(i).Coords(1), ...
                FemmProblem.BlockLabels(i).Coords(2), ...
                FemmProblem.BlockLabels(i).InGroup);

            k = k + 1;

        end
        
    end

    % write out regional attributes, the block labels
    fprintf(fp,'[NumBlockLabels] = %i\n', numel(FemmProblem.BlockLabels) - nholes);

    for i = 1:numel(FemmProblem.BlockLabels)

        if ~isequal(FemmProblem.BlockLabels(i).BlockType, '<No Mesh>')

            fprintf(fp,'%.17g\t%.17g\t', FemmProblem.BlockLabels(i).Coords(1), ...
                FemmProblem.BlockLabels(i).Coords(2));

            t = 0;
            for j = 1:numel(FemmProblem.Materials)
                if isequal(FemmProblem.Materials(j).Name, FemmProblem.BlockLabels(i).BlockType)
                    t = j;
                end
            end

            fprintf(fp,'%i\t', t);
            if FemmProblem.BlockLabels(i).MaxArea > 0
                fprintf(fp,'%.17g\t', FemmProblem.BlockLabels(i).MaxArea);
            else
                fprintf(fp,'-1\t');
            end


            switch ftype.(domaintype)
                
                case ftype.magnetics
                
                    t = 0;
                    for j = 1:numel(FemmProblem.Circuits)
                        if isequal(FemmProblem.Circuits(j).Name, FemmProblem.BlockLabels(i).InCircuit)
                            t = j;
                        end
                    end

                    if isnan (FemmProblem.BlockLabels(i).MagDir)
                        FemmProblem.BlockLabels(i).MagDir = 0;
                    end
                    
                    ExternalDefaultFlags = int32 (0);
                    
                    if  FemmProblem.BlockLabels(i).IsExternal
                        ExternalDefaultFlags = bitset (ExternalDefaultFlags, 1);
                    end
                    
                    if  FemmProblem.BlockLabels(i).IsDefault
                        ExternalDefaultFlags = bitset (ExternalDefaultFlags, 1);
                    end
                    
                    fprintf(fp,'%i\t%.17g\t%i\t%i\t%i', ...
                        t, ...
                        FemmProblem.BlockLabels(i).MagDir, ...
                        FemmProblem.BlockLabels(i).InGroup, ...
                        FemmProblem.BlockLabels(i).Turns, ...
                        ExternalDefaultFlags);

                    if isfield(FemmProblem.BlockLabels, 'MagDirFctn') && ~isempty(FemmProblem.BlockLabels(i).MagDirFctn)
                        fprintf(fp,'\t"%s"', FemmProblem.BlockLabels(i).MagDirFctn);
                    end

                    
                case ftype.heatflow
                
                    fprintf(fp,'%i\t%i', ...
                        FemmProblem.BlockLabels(i).InGroup, ...
                        FemmProblem.BlockLabels(i).IsDefault );

                
            end
            
            fprintf(fp,'\n');

            k = k + 1;
        end

    end    
        
%    fclose(fp);
    
end