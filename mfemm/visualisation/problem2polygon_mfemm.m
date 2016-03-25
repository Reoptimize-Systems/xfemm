function nodes = problem2polygon_mfemm (FemmProblem, varargin)
% converts the segments from a FemmProblem, or portion of a FemmProblem to
% a set of sequential nodes making the boundary of a polygon
%
% Description
%
% problem2polygon_mfemm takes the segments from a FemmProblem structure and
% creates a closed polygon, defined by a set of nodes which form it's
% boundary. The boundary can be reconstructed by following the nodes in
% sequence. Either the entire FemmProblem can be converted, or just a
% subset as defined by one of more group numbers from the problem.
%
% The FemmProblem (or subset of groups) must contain only segments which
% are connected to a single other segment.
%
% Syntax
%
% nodes = problem2polygon_mfemm (FemmProblem)
% nodes = problem2polygon_mfemm (FemmProblem, 'Groups', groupnums)
%
% 


    Inputs.Groups = [];
    Inputs.CheckZeroLinks = true;
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);
    
    orignodes = getnodecoords_mfemm(FemmProblem);
    links = getseglinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);
    arclinks = getarclinks_mfemm(FemmProblem, 'Groups', Inputs.Groups);

    if ~isempty (links)
        templinks = [links, zeros(size(links,1), 1), (1:size(links,1))'];
    else
        templinks = [];
    end
    
    if ~isempty (arclinks)
        temparclinks = [arclinks(:,1:2), ones(size(arclinks,1), 1), (1:size(arclinks,1))'];
    else
        temparclinks = [];
    end
    
    % get all links so we can traverse them, converting as required
    alllinks = [ templinks; 
                 temparclinks ];

    % make the link locations indices rather than zero-based ids
    alllinks(:,1:2) = alllinks(:,1:2) + 1;

    row = 1;

    % first set of nodes are the two nodes in the first links
    nodes = [ orignodes(alllinks(row,1),:);
              orignodes(alllinks(row,2),:) ];

    searchind = alllinks(row,2);

    % remove the added link from the list
    alllinks(row,:) = [];

    while ~isempty (alllinks)

        % find the next link in the chain, which is the link which is also
        % connected to the previous link
        [row,colstart] = find (alllinks(:,1:2) == searchind);

        if colstart == 1, colend = 2; else colend = 1; end

        if numel(row) > 1
            error ('Each segment must be connected to no more than one other segment');
        end

        if numel(row) < 1
            if Inputs.CheckZeroLinks
                error ('Each segment must be connected to one other segment');
            else
                % stop as we don't have anywhere to go
                return;
            end
        end
        
        searchind = alllinks(row,colend);

        if alllinks(row,3)
            % it is an arc segment, so we must generate the subsegments making
            % it and add them to the list

            [x, y] = mfemmdeps.arcpoints( orignodes(alllinks(row,1),:), ...
                                orignodes(alllinks(row,2),:), ...
                                arclinks(alllinks(row,4),3), ...
                                arclinks(alllinks(row,4),4) );

            if colstart == 2
                x = fliplr (x);
                y = fliplr (y);
            end
            
            % remove the first node, this will already have been added
            x(1) = []; y(1) = [];
            
            nodes = [ nodes;
                      [x(:), y(:)]; ];

        else
            
            nodes = [ nodes;
                      orignodes(alllinks(row,colend),:) ];

        end

        % remove the added link from the list
        alllinks(row,:) = [];

    end
    
    % the very last node will be a duplicate of the first, so remove it
    nodes(end,:) = [];

end
% plotnodelinks (newnodes, [ (1:size(newnodes, 1)-1)', (2:size(newnodes, 1))']-1);





