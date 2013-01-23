% Class wrapper for the FPProc_interface C++ class, which offers
% access to the FPProc finite element post-processing functions
classdef fpproc < handle
    
    properties (SetAccess = private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
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

        %% Train - an example class method call
        function varargout = opendocument(this, filename)
            [varargout{1:nargout}] = fpproc_interface_mex('opendocument', this.objectHandle, filename);
        end

        function varargout = getpointvalues(this, x, y)
            [varargout{1:nargout}] = fpproc_interface_mex('getpointvals', this.objectHandle, x(:), y(:));
        end

        function varargout = clearcontour(this)
            [varargout{1:nargout}] = fpproc_interface_mex('clearcontour', this.objectHandle);
        end

        function varargout = addcontour(this, x, y)
            [varargout{1:nargout}] = fpproc_interface_mex('addcontour', this.objectHandle, x(:), y(:));
        end
        
        function varargout = lineintegral(this, type)
            [varargout{1:nargout}] = fpproc_interface_mex('lineintegral', this.objectHandle, type);
        end
        
        function varargout = selectblock(this, x, y)
            [varargout{1:nargout}] = fpproc_interface_mex('selectblock', this.objectHandle, x, y);
        end
        
        function varargout = groupselectblock(this, groupno)
            [varargout{1:nargout}] = fpproc_interface_mex('groupselectblock', this.objectHandle, groupno);
        end
        
        function varargout = clearblock(this)
            [varargout{1:nargout}] = fpproc_interface_mex('clearblock', this.objectHandle);
        end
        
        function varargout = blockintegral(this, type)
            [varargout{1:nargout}] = fpproc_interface_mex('blockintegral', this.objectHandle, type);
        end
        
        function varargout = getprobleminfo(this)
            [varargout{1:nargout}] = fpproc_interface_mex('getprobleminfo', this.objectHandle);
        end
        
        function varargout = getcircuitprops(this, circuitname)
            [varargout{1:nargout}] = fpproc_interface_mex('getcircuitprops', this.objectHandle, circuitname);
        end

        %%%%%%      Derived Methods        %%%%%%%
        
        function varargout = newcontour(this, x, y)
            
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
