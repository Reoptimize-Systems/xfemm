classdef hpproc < mfemmpproc
    % hpproc   class for post-processing of mfemm heat flow solutions
    %
    % hpproc is a class wrapper for the hpproc_interface C++ class, which
    % offers access to the hpproc finite element post-processing functions
    %
    % Syntax
    %
    % hpproc()
    % hpproc(filename)
    %
    % Description
    %
    % hpproc() creates a new hpproc class without opening any solution file
    % for processing.
    %
    % hpproc(filename) creates a new hpproc class and attempts to load the
    % file in filename for processing.
    %
    %
    % hpproc Methods:
    %    opendocument - open a .ans solution document
    %    getpointvalues - get all solution outputs at points
    %    gett - get temperature values only at points
    %    getf - get heat flux values only at points
    %    getg - get temperature gradient values only at points
    %    getk - get thermal conductivity values only at points
    %    smoothon - turn on B and H smoothing over mesh elemnts
    %    smoothff - turn off B and H smoothing over mesh elemnts
    %    clearcontour - clear a contour
    %    addcontour - add one or more points to a contour
    %    lineintegral - perform a line integral along a contour
    %    selectblock - select a block
    %    groupselectblock - select blocks by group number
    %    selectallblocks - selects mfemmdeps.every block
    %    clearblock - clear all block selections
    %    blockintegral - perfom integrals over selected blocks
    %    getprobleminfo - get information about the problem
    %    getcircuitprops - get properties of a circuit
    %    newcontour - create a new contour, discarding old
    %    totalfieldenergy - calculates total field energy
    %    totalfieldcoenergy - calculates total field coenergy
    %    plotBfield - creates a plot of the flux density vector field
    %    nummeshnodes - returns the number of nodes in the mesh
    %    numelements - returns the number of elements in the mesh
    %    getvertices - gets coordinates of mesh vertices
    %    getelements - gets information about mesh elements
    %    getcentroids - gets the centroids of mesh elements
    %    getareas - gets the areas of mesh elements
    %    numgroupelements - gets the number of elements in groups
    %    getgroupvertices - gets coordinates of mesh vertices in groups
    %    getgroupelements - gets information about mesh elements in groups
    %    getgroupcentroids - gets the centroids of mesh elements in groups
    %    getgroupareas - gets the areas of mesh elements in groups
    %
    
% Copyright 2012-2014 Richard Crozier
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
    
    properties (SetAccess = private, GetAccess = public)
    
        T_Max;
        T_Min;
    
    end
    
    methods  
    
        %% Constructor - Create a new C++ class instance
        function this = hpproc(filename)
            
            this.objectHandle = hpproc_interface_mex('new');
            
            if nargin == 1
                this.opendocument(filename);
            end
            
        end

        %% Destructor - Destroy the C++ class instance
        function delete(this)
            hpproc_interface_mex('delete', this.objectHandle);
        end

        %%%%%%      The C++ Class Interface Methods       %%%%%%%

        function result = opendocument(this, filename)
            % loads a femm solution file for processing. Throws an error if
            % the file could not be opened.
            %
            % Syntax
            %
            % result = hpproc.opendocument(filename)
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
                % hpproc, and the slashes mangle it
                checkedfilename = strrep(filename, '\', '\\');
            else
                checkedfilename = filename;
            end
            
            result = hpproc_interface_mex('opendocument', this.objectHandle, checkedfilename);
            
            if result == 0
                this.isdocopen = false;
                this.openfilename = '';
                error('Document could not be opened')
            else
                this.isdocopen = true;
                this.openfilename = filename;
                % try and load the femmproblem structure from the file
                try
                    this.FemmProblem = loadfemmfile (filename);
                catch
                    warning ('Couldn''t load FemmProblem from solution file.')
                end
                    
            end
            
            % get the temperature bounds
            Tbounds = hpproc_interface_mex('temperaturebounds', this.objectHandle);
            
            this.T_Min = Tbounds(1);
            this.T_Max = Tbounds(2);
            
        end

        function pvals = getpointvalues(this, x, y)
            % getpointvalues(X,Y) Get the values associated with the points
            % at X,Y from the solution
            %
            % Syntax
            %
            % pvals = hpproc.getpointvalues(X, Y)
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
            % T - Temperature
            %
            % Fx - x or r directed component of heat flux density
            %
            % Fy - y or z directed component of heat flux density
            %
            % Gx - x or r directed component of temperature gradient
            %
            % Gy - y or z directed component of temperature gradient
            %
            % kx - x or r directed component of thermal conductivity
            %
            % ky - y or z directed component of thermal conductivity
            %

            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            pvals = hpproc_interface_mex('getpointvals', this.objectHandle, x(:), y(:));
            
        end
        
       function F = getf(this, x, y)
            % Get the heat flux values associated with the points at X,Y
            % from the solution
            %
            % Syntax
            %
            % F = fpproc.getf(X, Y)
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
            %   F - the heat flux at the requested coordinates
            %
            %
            
            if (nargin==3)
                
                F = this.getpointvalues(x,y);
                
                F = F(2:3,:);
                
            elseif (nargin==2)
                
                F = this.getpointvalues(x);
                
                F = F(2:3,:);
                
            end
            
        end
        
        
        function G = getg(this, x, y)
            % Get the flux density values associated with the points at X,Y
            % from the solution
            %
            % Syntax
            %
            % G = fpproc.getg(X, Y)
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
            %   G - the magnetic field intensity at the requested
            %     coordinates
            %
            %
            
            if (nargin==3)
                
                G = this.getpointvalues(x,y);
                
                G = G(4:5,:);
                
            elseif (nargin==2)
                
                G = this.getpointvalues(x);
                
                G = G(4:5,:);
                
            end
            
        end
        
        function K = getk(this, x, y)
            % Get the thermal conductivity values associated with the points at 
            % X,Y from the solution
            %
            % Syntax
            %
            % K = fpproc.getk(X, Y)
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
            %   K - the thermal conductivity at the requested coordinates
            %
            %
            
            if (nargin==3)
                
                K = this.getpointvalues(x,y);
                
                K = K(6:7,:);
                
            elseif (nargin==2)
                
                K = this.getpointvalues(x);
                
                K = K(6:7,:);
                
            end
            
        end
        
        function T = gett(this, x, y)
            % Get the temperature values associated with the points at
            % X,Y from the solution
            %
            % Syntax
            %
            % T = fpproc.gett(X, Y)
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
            %   T - the temperature at the requested coordinates
            %
            %
            
            if (nargin==3)
                
                T = this.getpointvalues(x,y);
                
                T = T(1,:);
                
            elseif (nargin==2)
                
                T = this.getpointvalues(x);
                
                T = T(1,:);
                
            end
            
        end
        
        function smoothon(this)
            % turns on interpolation of point values in elements
            %
            % Syntax
            %
            % hpproc.smoothon()
            %
            % Description
            %
            % Controls whether or not smoothing is applied to the B and H
            % fields in each element when reporting point properties.
            %
            % The fields are naturally piece-wise constant over each mesh
            % element. Calling smoothon turns on smoothing. It can be
            % turned off using smoothoff.
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            hpproc_interface_mex('smoothon', this.objectHandle);
        end
        
        function smoothoff(this)
            % turns off interpolation of point values in elements
            %
            % Syntax
            %
            % hpproc.smoothoff()
            %
            % Description
            %
            % Controls whether or not smoothing is applied to the B and H
            % fields in each element when reporting point properties.
            %
            % The fields are naturally piece-wise constant over each mesh
            % element. Calling smoothon turns off smoothing. It can be
            % turned on using smoothon.
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            hpproc_interface_mex('smoothoff', this.objectHandle);
        end

        function clearcontour(this)
            % clears all currently entered contour points
            %
            % Syntax
            %
            % hpproc.clearcontour()
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            hpproc_interface_mex('clearcontour', this.objectHandle);
        end

        function varargout = addcontour(this, x, y)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = hpproc_interface_mex('addcontour', this.objectHandle, x(:), y(:));
        end
        
        function varargout = lineintegral(this, type)
            % line integrals along contour
            %
            % Syntax
            %
            % hpproc.lineintegral (type)
            %
            % Input
            %
            % type in an integer flag determining the integral type to e
            % perfomed. The following options are available:
            %
            %   0 - Temperature difference (Gt)
            %   1 - Heat flux through the contour (Fn)
            %   2 - Contour length and surface area
            %   3 - Average Temperature
            %
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = hpproc_interface_mex('lineintegral', this.objectHandle, type);
        end
        
        function selectblock(this, x, y, clearselected)
            % select blocks based on position
            %
            % Syntax
            %
            % hpproc.selectblock(x, y)
            % hpproc.selectblock(x, y, clearselected)
            %
            % Input
            %
            %   x, y - x and y positions inside the blocks to be selected
            %
            %   clearselected - (optional) boolean flag determining whether
            %     any previous selection is cleared
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            if nargin < 4
                clearselected = false;
            end
            if clearselected
                this.clearblock();
            end
            hpproc_interface_mex('selectblock', this.objectHandle, x, y);
        end
        
        function groupselectblock(this, groupno)
            % select blocks based on group membership
            %
            % Syntax
            %
            % hpproc.groupselectblock(groupno)
            %
            % Description
            %
            % Selects all blocks which are members of the supplied group
            % number. If groupno is empty, all blocks are selected.
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            hpproc_interface_mex('groupselectblock', this.objectHandle, groupno);
        end
        
        function selectallblocks(this)
            % selects mfemmdeps.every block in the solution
            %
            % Syntax
            %
            % hpproc.selectallblocks()
            %
            %
            
            this.groupselectblock([]);
            
        end
        
        function clearblock(this)
            % clears any existing block selection so no blocks are selected
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            hpproc_interface_mex('clearblock', this.objectHandle);
        end
        
        function int = blockintegral(this, type, x, y)
            % blockintegral calculate a block integral for the selected
            % blocks
            %
            % Syntax
            %
            % int = hpproc.blockintegral(type)
            % int = hpproc.blockintegral(type, x, y)
            %
            % Input
            %
            % type in an integer flag determining the integral type to e
            % perfomed. The following options are available:
            %
            %   0 Average T over the block
            %   1 Block Cross-section
            %   2 Block Volume
            %   3 Average F over the block
            %   4 Average G over the block
            %
            % hpproc.blockintegral(type) peforms the desired integral on
            % the currently selected blocks.
            %
            % hpproc.blockintegral(type, x, y) clears any existing block
            % selection, selects the block closest to (x,y) and performs the 
            % integral on this block.
            %
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            if nargin > 2
                if nargin == 3 || nargin > 4
                    error('Wrong number of input arguments.')
                else
                    if mfemmdeps.samesize(x, y)
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
            int = hpproc_interface_mex('blockintegral', this.objectHandle, type);
            
        end
        
        function varargout = getprobleminfo(this)
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            [varargout{1:nargout}] = hpproc_interface_mex('getprobleminfo', this.objectHandle);
        end
        
        function circprops = getcircuitprops(this, circuitname)
            % obtain information associated with a circuit in an mfemm
            % magnetics solution.
            %
            % Syntax
            %
            % circprops = hpproc.getcircuitproperties(circuitname)
            %
            % Description
            %
            % Properties are returned for the circuit property named with
            % the name 'circuitname'.
            %
            % circprops is a vector of three values. In order, these values
            % are:
            %
            % � current:  Current carried by the circuit.
            %
            % � volts:    Voltage drop across the circuit in the circuit.
            %
            % � flux:     Circuit�s flux linkage
            %
            
            if ~this.isdocopen
                error('No solution document has been opened.')
            end
            
            circprops = hpproc_interface_mex('getcircuitprops', this.objectHandle, circuitname);
            
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
            
            if ~mfemmdeps.samesize(x,y) || ~isvector(x) || numel(x) < 2
                error('x and y must be vectors of at least two values.')
            end

            if numel(x) ~= numel(y)
                error('hpproc:wrongsizeinputs', ...
                      'x and y must be the same size.');
            end
            if numel(x) < 2
                error('hpproc:wrongsizeinputs', ...
                      'You must supply at least two contour coordinates.');
            end
            
            % clear out any existing contour points
            this.clearcontour();
            % create the new contour
            [varargout{1:nargout}] = this.addcontour(x(:), y(:));
            
        end
        
        function smooth(this, flag)
            % sets interpolation of point values to 'on' or 'off' dependent
            % on input flag
            %
            % Syntax
            %
            % hpproc.smooth(flag)
            %
            % Input
            %
            %   flag - string, either 'on' or 'off'
            %
            
            if ~ischar(flag)
                error('flag must be a string');
            end
            
            switch flag
                
                case 'on'
                    this.smoothon();
                    
                case 'off'
                    this.smoothoff()
                    
                otherwise
                    error('flag must be the string ''on'' or ''off''.')
                    
            end
            
        end
        
        function n = nummeshnodes (this)
            % return the number of nodes in the mesh
            n = hpproc_interface_mex('numnodes', this.objectHandle);
        end
        
        function n = numelements (this)
            % return the number of elements in the mesh
            n = hpproc_interface_mex('numelements', this.objectHandle);
        end
        
        function vert = getvertices (this, n)
            % returns vertex locations for elements
            %
            % Syntax
            %
            % vert = getvertices ()
            % vert = getvertices (n)
            %
            % Input
            %
            % n - optional matrix of element numbers for which to obtain
            %   the vertices. Element numbers start from 1 (rather than
            %   zero). The vertices of mfemmdeps.every mesh element are returned if n
            %   is not supplied.
            %
            % Output
            % 
            % vert - matrix of (n x 6) values. Each row containing the  
            %   coordinates for the vertices of each element number in 'n'
            %   such that a row conatins:
            %
            %       [ x1, y1, x2, y2, x3, y3 ]
            %         
            %
            
            if nargin < 2
                n = 1:this.numelements ();
            end
            
            vert = hpproc_interface_mex('getvertices', this.objectHandle, n(:));
        
        end
         
        function elm = getelements (this, n)
            % returns information about elements
            %
            % Syntax
            %
            % elm = getelements ()
            % elm = getelements (n)
            %
            % Input
            %
            % n - optional matrix of element numbers for which to obtain information. 
            %   Element numbers start from 1 (rather than zero). If omitted
            %   information on mfemmdeps.every mesh element is returned.
            %
            % Output
            % 
            % elm - matrix of (n x 7) values. Each row containing the following 
            %   information for each element number in 'n':
            %         1. One-based Index of first element node
            %         2. One-based Index of second element node
            %         3. One-based Index of third element node
            %         4. x (or r) coordinate of the element centroid
            %         5. y (or z) coordinate of the element centroid
            %         6. element area using the length mfemmdeps.unit defined for the problem
            %         7. group number associated with the element
            %
            
            if nargin < 2
                n = 1:this.numelements ();
            end
            
            elm = hpproc_interface_mex('getelements', this.objectHandle, n(:));
        
        end
        
        function centr = getcentroids (this, n)
            % returns centroid locations for elements
            %
            % Syntax
            %
            % centr = getcentroids ()
            % centr = getcentroids (n)
            %
            % Input
            %
            % n - optional matrix of element numbers for which to obtain
            %   the centroids. Element numbers start from 1 (rather than
            %   zero). The centroids of mfemmdeps.every mesh element are returned if
            %   n is not supplied.
            %
            % Output
            % 
            % centr - matrix of (n x 2) values. Each row containing the  
            %   coordinates for the vertices of each element number in 'n'
            %   such that a row conatins:
            %
            %       [ x, y ]
            %         
            %
            
            if nargin < 2
                n = 1:this.numelements ();
            end
            
            centr = hpproc_interface_mex('getcentroids', this.objectHandle, n(:));
        
        end
        
        function areas = getareas (this, n)
            % returns area information about elements
            %
            % Syntax
            %
            % areas = getareas ()
            % areas = getareas (n)
            %
            % Input
            %
            % n - optional matrix of element numbers for which to obtain
            %   the areas. Element numbers start from 1 (rather than
            %   zero). The areas of mfemmdeps.every mesh element are returned if
            %   n is not supplied.
            %
            % Output
            % 
            % centr - matrix of (n x 1) values. Each row containing the  
            %   area for the element number in 'n'.
            %
            
            if nargin < 2
                n = 1:this.numelements ();
            end
            
            areas = hpproc_interface_mex('getareas', this.objectHandle, n(:));
        
        end
        
        function n = numgroupelements (this, groupno)
            % returns number of elements in groups
            %
            % Syntax
            %
            % n = numgroupelements (groupno)
            %
            % Input
            %
            % groupno - matrix of group numbers for which to obtain
            %   the number of element.
            %
            % Output
            % 
            % n - matrix of numbers of elements, one for each number in
            %   groupno.
            %         
            %
            
            if nargin ~= 2
                error ('You must supply a group number.')
            end
            
            n = zeros (size(groupno));
            
            for ind = 1:numel(groupno) 
                n(ind) = hpproc_interface_mex('numgroupelements', this.objectHandle, groupno(ind));
            end
            
        end
        
        function vert = getgroupvertices (this, groupno)
            % returns vertex locations for elements in groups
            %
            % Syntax
            %
            % vert = getgroupvertices (groupno)
            %
            % Input
            %
            % groupno - matrix of group numbers for which to obtain
            %   the element vertices.
            %
            % Output
            % 
            % vert - matrix of (n x 6) values. Each row containing the  
            %   coordinates for the vertices of each element such that a
            %   row conatins:
            %
            %       [ x1, y1, x2, y2, x3, y3 ]
            %         
            %
            
            if nargin ~= 2
                error ('You must supply a group number.')
            end
            
            vert = [];
            
            for ind = 1:numel(groupno)
                
                vert = [vert; hpproc_interface_mex('getgroupvertices', this.objectHandle, groupno(ind))];
            
            end
        
        end
         
        function elm = getgroupelements (this, groupno)
            % returns information about elements in groups
            %
            % Syntax
            %
            % elm = getelements (groupno)
            %
            % Input
            %
            % groupno - matrix of group numbers for which to obtain
            %   information about mesh elements.
            %
            % Output
            % 
            % elm - matrix of (n x 7) values. Each row containing the following 
            %   information for each element:
            %         1. One-based Index of first element node
            %         2. One-based Index of second element node
            %         3. One-based Index of third element node
            %         4. x (or r) coordinate of the element centroid
            %         5. y (or z) coordinate of the element centroid
            %         6. element area using the length mfemmdeps.unit defined for the problem
            %         7. group number associated with the element
            %
            
            if nargin ~= 2
                error ('You must supply a group number.')
            end
            
            elm = [];
            
            for ind = 1:numel(groupno)
                
                elm = [elm; hpproc_interface_mex('getgroupelements', this.objectHandle, groupno(ind))];
                
            end
        
        end
        
        function centr = getgroupcentroids (this, groupno)
            % returns centroid locations for elements in groups
            %
            % Syntax
            %
            % centr = getcentroids (groupno)
            %
            % Input
            %
            % groupno - optional matrix of group numbers for which to obtain
            %   the element centroids.
            %
            % Output
            % 
            % centr - matrix of (n x 2) values. Each row containing the  
            %   coordinates for the vertices of each element such that a
            %   row contains:
            %
            %       [ x, y ]
            %         
            %
            
            if nargin ~= 2
                error ('You must supply a group number.')
            end
            
            centr = [];
            
            for ind = 1:numel(groupno)
                
                centr = [centr; hpproc_interface_mex('getgroupcentroids', this.objectHandle, groupno(ind))];
            end
        
        end
        
        function areas = getgroupareas (this, groupno)
            % returns area information about elements in groups
            %
            % Syntax
            %
            % areas = getgroupareas (groupno)
            %
            % Input
            %
            % groupno - optional matrix of group numbers for which to obtain
            %   the element areas. 
            %
            % Output
            % 
            % areas - matrix of of areas.
            %
            
            if nargin ~= 2
                error ('You must supply a group number.')
            end
            
            areas = [];
            
            for ind = 1:numel(groupno)
            
                areas = [areas; hpproc_interface_mex('getgroupareas', this.objectHandle, groupno(ind))];
            
            end
        
        end
        
        function hfig = plotFfield(this, x, y, w, h, varargin)
            % creates a plot of the heat flux vector field
            %
            % Syntax
            %
            % hpproc.plotFfield(x, y, w, h)
            % hpproc.plotFfield(x, y, w, h, 'Parameter', Value)
            %
            % Input
            %
            %   x - x (or r) coordinate lower left corner of region to be
            %     plotted
            % 
            %   y - y (or x) coordinate of  lower left corner of region to 
            %     be plotted
            %
            %   w - width of region to be plotted
            % 
            %   h - height of region to be plotted
            % 
            % Further options are supplied using parameter-value pairs,
            % the possible options are:
            %
            %   'Points' - determines the number of points that will be
            %     plotted using method 0. If points is a scalar, a grid of
            %     this number of points on both sides will be created. If
            %     points is a two element vector it will be the number of
            %     points in the x and y direction respectively.
            % 
            %    'PlotNodes' - determines if nodes are drawn when
            %      plotting the femmproblem
            %
            %   'Method' - plot method use 0 for a vector field plot using
            %     coloured arrows. Use 1 for a contour plot of the
            %     magnitude of the magnetic field.
            
            Inputs.Points = 40;
            Inputs.PlotNodes = true;
            Inputs.Method = 0;
            Inputs.AddLabels = true;
            
            Inputs = mfemmdeps.parseoptions (Inputs, varargin);
            
            datafcn = @this.getf;
            
            hfig = this.plotvectorfield(datafcn, x, y, w, h, Inputs);
            
            title ('Heat Flux, F');
            
        end

        
        function hfig = plotGfield(this, x, y, w, h, varargin)
            % creates a plot of the temperature gradient vector field
            %
            % Syntax
            %
            % hpproc.plotGfield(x, y, w, h)
            % hpproc.plotGfield(x, y, w, h, 'Parameter', Value)
            %
            % Input
            %
            %   x - x (or r) coordinate lower left corner of region to be
            %     plotted
            % 
            %   y - y (or x) coordinate of  lower left corner of region to 
            %     be plotted
            %
            %   w - width of region to be plotted
            % 
            %   h - height of region to be plotted
            % 
            % Further options are supplied using parameter-value pairs,
            % the possible options are:
            %
            %   'Points' - determines the number of points that will be
            %     plotted using method 0. If points is a scalar, a grid of
            %     this number of points on both sides will be created. If
            %     points is a two element vector it will be the number of
            %     points in the x and y direction respectively.
            % 
            %    'PlotNodes' - determines if nodes are drawn when
            %      plotting the femmproblem
            %
            %   'Method' - plot method use 0 for a vector field plot using
            %     coloured arrows. Use 1 for a contour plot of the
            %     magnitude of the magnetic field.
            %
            
            Inputs.Points = 40;
            Inputs.PlotNodes = true;
            Inputs.Method = 0;
            Inputs.AddLabels = true;
            
            Inputs = mfemmdeps.parseoptions (Inputs, varargin);
            
            datafcn = @this.getg;
            
            hfig = this.plotvectorfield(datafcn, x, y, w, h, Inputs);
            
            title ('Temperature Gradient, G');
            
        end
        
        
        function hfig = plotTfield(this, x, y, w, h, varargin)
            % creates a plot of the temperature scalar field
            %
            % Syntax
            %
            % hpproc.plotTfield(method, x, y, w, h)
            % hpproc.plotTfield(method, x, y, w, h, 'Parameter', Value)
            %
            % Input
            %
            %   x - x (or r) coordinate lower left corner of region to be
            %     plotted
            % 
            %   y - y (or x) coordinate of  lower left corner of region to 
            %     be plotted
            %
            %   w - width of region to be plotted
            % 
            %   h - height of region to be plotted
            % 
            % Further options are supplied using parameter-value pairs,
            % the possible options are:
            %
            %   'Points' - determines the number of points that will be
            %     plotted using method 0. If points is a scalar, a grid of
            %     this number of points on both sides will be created. If
            %     points is a two element vector it will be the number of
            %     points in the x and y direction respectively.
            % 
            %    'PlotNodes' - determines if nodes are drawn when
            %      plotting the femmproblem
            %
            %   'Method' - plot method use 0 for a vector field plot using
            %     coloured arrows. Use 1 for a contour plot of the
            %     magnitude of the magnetic field.
            %
            
            Inputs.Points = 100;
            Inputs.PlotNodes = true;
            Inputs.Method = 0;
            Inputs.AddLabels = true;
            
            Inputs = mfemmdeps.parseoptions (Inputs, varargin);
            
            datafcn = @(x,y) reshape (this.gett (x,y), size (x));
            
            hfig = this.plotscalarfield (datafcn, x, y, w, h, Inputs);
            
            title ('Temperature, T [^{o}C]');
            
        end
        
    end
    
end
