% Class wrapper for the FPProc_interface C++ class, which offers
% access to the FPProc finite element post-processing functions
classdef fpproc < handle
    properties (SetAccess = private, Hidden = false)
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

        %% Train - an example class method call
        function varargout = opendocument(this, filename)
            [varargout{1:nargout}] = fpproc_interface_mex('opendocument', this.objectHandle, filename);
        end

        %% Test - another example class method call
        function varargout = getpointvalues(this, x, y)
            [varargout{1:nargout}] = fpproc_interface_mex('getpointvals', this.objectHandle, x(:), y(:));
        end
    end
end
