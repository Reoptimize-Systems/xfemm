function mesh = loadmesh_mfemm (meshpath)
% loads a mesh created by fmesher from disk
%
% Syntax
%
% mesh = loadmesh_mfemm (meshpath)
%
% Inputs
%
%  meshpath - string representing the path to the mesh files saved on disk.
%    fmesher saves the mesh in several files, with the same basename. The
%    path supplied to loadmesh_mfemm is the path to any of these files but
%    without the extension. For example:
%
%    /home/myusername/somedirectoy/meshfile
%
%    where the directory /home/myusername/somedirectoy/ contains the files
%    meshfile.edge, meshfile.ele, and meshfile.node generated using
%    fmesher.
%
% Output
%
%  mesh - structure containing the mesh information in several fields
%
%

    % read in the nodes
    [fid_node, delobj_node] = safefopen ([meshpath, '.node']);
    
    C = textscan (fid_node, '%d\t%d\t%d\t%d', 1, 'CommentStyle', '#');
    
    mesh.NNodes = C{1};
    mesh.NDimensions = C{2};
    mesh.NAttributes = C{3};
    mesh.HasBoundaryMarkers = C{4} == 1;
    
    if mesh.HasBoundaryMarkers
        formatstr = '%d\t%f\t%f\t%d';
    else
        formatstr = '%d\t%f\t%f';
    end
    
%     [fid_node, delobj_node] = safefopen ([meshpath, '.node']);
%     C = getdata (fid_node, mesh.NNodes, formatstr);
    C = textscan (fid_node, formatstr, mesh.NNodes, 'CollectOutput', true, 'CommentStyle', '#');
    
    mesh.NodeIDs = C{1};
    mesh.NodeCoordinates = C{2};
    
    if mesh.HasBoundaryMarkers
        mesh.BoundaryMarkers = C{3};
    end
    
    
    [fid_ele, delobj_ele] = safefopen ([meshpath, '.ele']);
    
    C = textscan (fid_ele, '%d\t%d\t%d', 1, 'CommentStyle', '#');
    
    mesh.NElements = C{1};
    mesh.NNodesPerElement = C{2};
    mesh.NElementAttributes = C{3};
    
    formatstr = ['%d\t', ...
                 repmat('%d\t', 1, mesh.NNodesPerElement), ...
                 repmat('%d\t', 1, mesh.NElementAttributes)];
              
%     [fid_ele, delobj_ele] = safefopen ([meshpath, '.ele']);
%     C = getdata (fid_ele, mesh.NElements, formatstr);
    C = textscan (fid_ele, formatstr, mesh.NElements, 'CollectOutput', true, 'CommentStyle', '#');
    
    mesh.ElementIDs = C{1}(:,1);
    mesh.ElementNodeIDs = C{1}(:,2:(1+mesh.NNodesPerElement));
    mesh.ElementAttributes = C{1}(:,(1+mesh.NNodesPerElement+1):end);
    
    
    [fid_edge, delobj_edge] = safefopen ([meshpath, '.edge']);
    
    C = textscan (fid_edge, '%d\t%d', 1, 'CommentStyle', '#');
    
    mesh.NEdges = C{1};
    mesh.HasBoundaryMarkers = C{2} == 1;
    
    if mesh.HasBoundaryMarkers
        formatstr = '%d\t%d\t%d\t%d';
    else
        formatstr = '%d\t%d\t%d';  
    end
    
% 	[fid_edge, delobj_edge] = safefopen ([meshpath, '.edge']);
%     C = getdata (fid_edge, mesh.NEdges, formatstr);
    C = textscan (fid_edge, formatstr, mesh.NEdges, 'CollectOutput', true, 'CommentStyle', '#');
    
    mesh.EdgeIDs = C{1}(:,1);
    mesh.EdgeNodeIDs = C{1}(:,2:3);
    
    if mesh.HasBoundaryMarkers
        mesh.EdgeBoundaryMarkers = C{1}(:,4);
    end
    
end

% function data = getdata (fid, NLines, formatstr)
% 
% 
%     firstline = true;
%     
%     for n = 1:NLines
%         
%         tline = fgetl (fid);
%         
%         if n > 1
%         
%             if tline(1) ~= '#'
% 
%                 C = textscan (tline, formatstr, 'CollectOutput', true);
%                 
%                 if firstline
%                     
%                     data = cell(size(C));
% 
%                     for Cind = 1:numel (C) 
% 
%                         data{Cind} = repmat (C{Cind}, NLines, 1);
% 
%                     end
%                 end
% 
%                 for Cind = 1:numel (C) 
% 
%                     data{Cind}(n,:) = C{Cind}(1,:);
% 
%                 end
% 
%                 firstline = false;
%             end
%         
%         end
%         
%     end
% 
% end

function [fid, delobj] = safefopen (filepath)

    fid = fopen (filepath);
    
    delobj = onCleanup (@() fclose (fid));

end