classdef fpproc < handle
    % fpproc   class for post-processing of mfemm solutions
    %
    % fpproc is a class wrapper for the FPProc_interface C++ class, which
    % offers access to the FPProc finite element post-processing functions
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
    
    
    properties (SetAccess = private, Hidden = true)
        
        objectHandle; % Handle to the underlying C++ class instance
        
        isdocopen = false; % flag denoting whether a document has been opened yet
        
    end
    
    methods  
        %% Constructor - Create a new C++ class instance
        function this = fpproc(varargin)
            this.objectHandle = fpproc_interface_mex('new', varargin{:});
        end

        %% Destructor - Destroy the C++ class instance
        function delete(this)
            fpproc_interface_mex('delete', this.objectHandle);
        end

        %%%%%%      The C++ Class Interface Methods       %%%%%%%

        function varargout = opendocument(this, filename)
            
            result = fpproc_interface_mex('opendocument', this.objectHandle, filename);
            
            if result == 0
                this.isdocopen = false;
                error('Document could not be opened')
            else
                this.isdocopen = true;
            end
            
        end

        function pvals = getpointvalues(this, x, y)
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
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            fpproc_interface_mex('clearblock', this.objectHandle);
        end
        
        function int = blockintegral(this, type)
            % blockintegral    calculate a block integral for the selected
            %   blocks
            %
            % Syntax
            %
            % z = fpproc.blockintegral(type)
            %
            % Input
            %
            % type in an integer flag determining the integral type to e
            % perfomed. The following options are available:
            %
            %   0   A·J
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
            %   13  x (or r) part of 2× Lorentz force
            %   14  y (or z) part of 2× Lorentz force
            %   15  Steady-state Lorentz torque
            %   16  2X component of Lorentz torque
            %   17  Magnetic field coenergy
            %   18  x (or r) part of steady-state weighted stress tensor force
            %   19  y (or z) part of steady-state weighted stress tensor force
            %   20  x (or r) part of 2× weighted stress tensor force
            %   21  y (or z) part of 2× weighted stress tensor force
            %   22  Steady-state weighted stress tensor torque
            %   23  2X component of weighted stress tensor torque
            %   24  R2 (i.e. moment of inertia / density)
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            int = fpproc_interface_mex('blockintegral', this.objectHandle, type);
            
        end
        
        function varargout = getprobleminfo(this)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = fpproc_interface_mex('getprobleminfo', this.objectHandle);
        end
        
        function varargout = getcircuitprops(this, circuitname)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = fpproc_interface_mex('getcircuitprops', this.objectHandle, circuitname);
        end

        %%%%%%      Derived Methods        %%%%%%%
        
        function varargout = newcontour(this, x, y)
            if ~this.isdocopen
                error('No solution document has been opened.')
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


    end
end
