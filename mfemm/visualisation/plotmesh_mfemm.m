function [hax, hfig] = plotmesh_mfemm (mesh, varargin)

    options.AxesHandle = [];
    
    options = mfemmdeps.parse_pv_pairs (options, varargin);


    if ischar (mesh)
        
        mesh = loadmesh_mfemm (mesh);
        
    elseif ~isstruct (mesh)
        error ('first arg should be a mesh structure, as returned by loadmesh_mfemm, or a path to the mesh files (without file extension)');
    end
    
    if isempty (options.AxesHandle)
        
        hfig = figure;
        hax = axes;
        
    end
    
    trisurf ( double(mesh.ElementNodeIDs+1), ...
              mesh.NodeCoordinates(:,1), ...
              mesh.NodeCoordinates(:,2), ...
              zeros (size (mesh.NodeCoordinates(:,2))), ...
              mesh.ElementAttributes, ...
              'Parent', hax );
          
	view (2);
          
	axis equal;

end