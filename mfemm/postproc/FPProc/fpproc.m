% Class wrapper for the FPProc_interface C++ class, which offers
% access to the FPProc finite element post-processing functions
classdef fpproc < handle
    properties (SetAccess = private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
    end
    methods
        %% Constructor - Create a new C++ class instance 
        function this = class_interface(varargin)
            this.objectHandle = fpproc_interface_mex('new', varargin{:});
        end
        
        %% Destructor - Destroy the C++ class instance
        function delete(this)
            fpproc_interface_mex('delete', this.objectHandle);
        end

        %% Train - an example class method call
        function varargout = train(this, varargin)
            [varargout{1:nargout}] = fpproc_interface_mex('train', this.objectHandle, varargin{:});
        end

        %% Test - another example class method call
        function varargout = test(this, varargin)
            [varargout{1:nargout}] = fpproc_interface_mex('test', this.objectHandle, varargin{:});
        end
    end
end