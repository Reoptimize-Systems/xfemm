classdef fpproc < handle
    % fpproc   class for post-processing of mfemm solutions
    %
    % fpproc is a class wrapper for the FPProc_interface C++ class, which
    % offers access to the FPProc finite element post-processing functions
    %
    % Syntax
    %
    % fpproc()
    % fpproc(filename)
    %
    % Description
    %
    % fpproc() creates a new fpproc class without opening any solution file
    % for processing.
    %
    % fpproc(filename) creates a new fpproc class and attempts to load the
    % file in filename for processing.
    %
    %
    % fpproc Methods:
    %    opendocument - open a .ans solution document
    %    getpointvalues - get solution values at a point
    %    clearcontour - clear a contour
    %    addcontour - add one or more points to a contour
    %    lineintegral - perform a line integral along a contour
    %    selectblock - select a block
    %    groupselectblock - select blocks by group number
    %    clearblock - clear all block selections
    %    blockintegral - perfom integrals over selected blocks
    %    getprobleminfo - get information about the problem
    %    getcircuitprops - get properties of a circuit
    %    newcontour - create a new contour, discarding old
    %
    
% Copyright 2012-2013 Richard Crozier
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
    
    
    properties (SetAccess = private, Hidden = true)
        
        objectHandle; % Handle to the underlying C++ class instance
        
    end
    
    properties (SetAccess = private, Hidden = false)

        isdocopen = false; % flag denoting whether a document has been opened yet
        
        openfilename = ''; % contains the path of any currently open files
        
    end
    
    methods  
        %% Constructor - Create a new C++ class instance
        function this = fpproc(filename)
            
            this.objectHandle = fpproc_interface_mex('new');
            
            if nargin == 1
                this.opendocument(filename);
            end
            
        end

        %% Destructor - Destroy the C++ class instance
        function delete(this)
            fpproc_interface_mex('delete', this.objectHandle);
        end

        %%%%%%      The C++ Class Interface Methods       %%%%%%%

        function result = opendocument(this, filename)
            % loads a femm solution file for processing. Throws an error if
            % the file could not be opened.
            %
            % Syntax
            %
            % result = fpproc.opendocument(filename)
            %
            % Input
            % 
            %  filename - the path to the file to be opened for processing.
            %    If the file is successfully opened the isdocopen property
            %    will be set to true, and the openfilename property set to
            %    the full path to the file
            %
            % Output
            %
            %  result - is true if the file was successfully opened or
            %    false otherwise.
            
            if ~exist(filename, 'file')
                error('File could not be found.');
            end
            
            if ispc
                % escape the slashes, as the string is passed verbatim to
                % fpproc, and the slashes mangle it
                checkedfilename = strrep(filename, '\', '\\');
            else
                checkedfilename = filename;
            end
            
            result = fpproc_interface_mex('opendocument', this.objectHandle, checkedfilename);
            
            if result == 0
                this.isdocopen = false;
                this.openfilename = '';
                error('Document could not be opened')
            else
                this.isdocopen = true;
                this.openfilename = filename;
            end
            
        end

        function pvals = getpointvalues(this, x, y)
            % getpointvalues(X,Y) Get the values associated with the points
            % at X,Y from the solution
            %
            % Syntax
            %
            % pvals = fpproc.getpointvalues(X, Y)
            %
            % Input
            %
            %   X,Y - X and Y are matrices of the same size containing
            %     sets of x and y coordinates at which the point properties
            %     are to be determined. Internally these will be reshaped
            %     as X(:),Y(:), i.e. two column vectors.
            %
            % Output
            %
            % pvals is a matrix containing the values associated with each
            % point defined by X,Y from the solution. Each column of pvals
            % corresponds to a given x,y point, and the rows of pvals refer
            % to the following values:
            % 
            % A     vector potential A or flux f
            % B1    flux density Bx if planar, Br if axisymmetric
            % B2    flux density By if planar, Bz if axisymmetric
            % Sig   electrical conductivity s
            % E     stored energy density
            % H1    field intensity Hx if planar, Hr if axisymmetric
            % H2    field intensity Hy if planar, Hz if axisymmetric
            % Je    eddy current density
            % Js    source current density
            % Mu1   relative permeability ?x if planar, ?r if axisymmetric
            % Mu2   relative permeability ?y if planar, ?z if axisymmetric
            % Pe    Power density dissipated through ohmic losses
            % Ph    Power density dissipated by hysteresis
            % ff    Fill Factor
            %

            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            pvals = fpproc_interface_mex('getpointvals', this.objectHandle, x(:), y(:));
            
        end

        function clearcontour(this)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            fpproc_interface_mex('clearcontour', this.objectHandle);
        end

        function varargout = addcontour(this, x, y)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = fpproc_interface_mex('addcontour', this.objectHandle, x(:), y(:));
        end
        
        function varargout = lineintegral(this, type)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = fpproc_interface_mex('lineintegral', this.objectHandle, type);
        end
        
        function selectblock(this, x, y, clearselected)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            if nargin < 4
                clearselected = false;
            end
            if clearselected
                this.clearblock();
            end
            fpproc_interface_mex('selectblock', this.objectHandle, x, y);
        end
        
        function groupselectblock(this, groupno)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            fpproc_interface_mex('groupselectblock', this.objectHandle, groupno);
        end
        
        function clearblock(this)
            % clears any existing block selection so no blocks are selected
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            fpproc_interface_mex('clearblock', this.objectHandle);
        end
        
        function int = blockintegral(this, type, x, y)
            % blockintegral calculate a block integral for the selected
            % blocks
            %
            % Syntax
            %
            % int = fpproc.blockintegral(type)
            % int = fpproc.blockintegral(type, x, y)
            %
            % Input
            %
            % type in an integer flag determining the integral type to e
            % perfomed. The following options are available:
            %
            %   0   A.J
            %   1   A
            %   2   Magnetic field energy
            %   3   Hysteresis and/or lamination losses
            %   4   Resistive losses
            %   5   Block cross-section area
            %   6   Total losses
            %   7   Total current
            %   8   Integral of Bx (or Br) over block
            %   9   Integral of By (or Bz) over block
            %   10  Block volume
            %   11  x (or r) part of steady-state Lorentz force
            %   12  y (or z) part of steady-state Lorentz force
            %   13  x (or r) part of 2x Lorentz force
            %   14  y (or z) part of 2x Lorentz force
            %   15  Steady-state Lorentz torque
            %   16  2X component of Lorentz torque
            %   17  Magnetic field coenergy
            %   18  x (or r) part of steady-state weighted stress tensor force
            %   19  y (or z) part of steady-state weighted stress tensor force
            %   20  x (or r) part of 2x weighted stress tensor force
            %   21  y (or z) part of 2x weighted stress tensor force
            %   22  Steady-state weighted stress tensor torque
            %   23  2X component of weighted stress tensor torque
            %   24  R2 (i.e. moment of inertia / density)
            %
            % fpproc.blockintegral(type) peforms the desired integral on
            % the currently selected blocks.
            %
            % fpproc.blockintegral(type, x, y) clears any existing block
            % selection, selects the 
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            if nargin > 2
                if nargin == 3 || nargin > 4
                    error('Wrong number of input arguments.')
                else
                    if samesize(x, y)
                        % clear existing block selection
                        this.clearblock();
                        % select the specified blocks
                        for i = 1:numel(x)
                            this.selectblock(x(i), y(i), false)
                        end
                    else
                       error('x and y must be matrices of the same size.') 
                    end
                end
            end
            % perform the block integral
            int = fpproc_interface_mex('blockintegral', this.objectHandle, type);
            
        end
        
        function varargout = getprobleminfo(this)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = fpproc_interface_mex('getprobleminfo', this.objectHandle);
        end
        
        function circprops = getcircuitprops(this, circuitname)
            % obtain information associated with a circuit in an mfemm
            % magnetics solution.
            %
            % Syntax
            %
            % circprops = fpproc.getcircuitproperties(circuitname)
            %
            % Description
            %
            % Properties are returned for the circuit property named with
            % the name 'circuitname'.
            %
            % circprops is a vector of three values. In order, these values
            % are:
            %
            % – current:  Current carried by the circuit.
            %
            % – volts:    Voltage drop across the circuit in the circuit.
            %
            % – flux:     Circuit’s flux linkage
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            circprops = fpproc_interface_mex('getcircuitprops', this.objectHandle, circuitname);
            
        end

        %%%%%%      Derived Methods        %%%%%%%
        
        function varargout = newcontour(this, x, y)
            % Clears exusting contour points and creates a new contour from
            % the supplied contour points
            %
            % Syntax
            %
            % newcontour(x, y)
            %
            % Description
            %
            % x and y are vectors of x and y positons making up the
            % contour. Once created the contour can be used to generate
            % line integrals etc.
            %
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            if ~samesize(x,y) || ~isvector(x) || numel(x) < 2
                error('x and y must be vectors of at least two values.')
            end

            if numel(x) ~= numel(y)
                error('FPPROC:wrongsizeinputs', ...
                      'x and y must be the same size.');
            end
            if numel(x) < 2
                error('FPPROC:wrongsizeinputs', ...
                      'You must supply at least two contour coordinates.');
            end
            
            % clear out any existing contour points
            this.clearcontour();
            % create the new contour
            [varargout{1:nargout}] = this.addcontour(x(:), y(:));
            
        end
        
        function [R, L] = circuitRL(this, circuitname)
            % gets the inductance and resistance of a circuit in the
            % solution with name 'circuitname'
            %
            % Syntax
            %
            % [R, L] = fpproc.circuitRL(circuitname)
            %
            % Input
            %
            %   circuitname - the name of the circuit from which we wish to 
            %     obtain the inductance and resistance for
            % 
            % Output
            %
            %  R - circuit resistance
            %
            %  L - circuit inductance (calculated from Phi / I)
            %

            temp = this.getcircuitprops(circuitname);

            L = temp(3) / temp(1);

            R = temp(2) / temp(1);

        end

    end
end
