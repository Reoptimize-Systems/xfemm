classdef mfemmpproc < handle


    properties (SetAccess = protected, Hidden = true)
        
        objectHandle; % Handle to the underlying C++ class instance
        
    end
    
    properties (SetAccess = protected, Hidden = false)

        isdocopen = false; % flag denoting whether a document has been opened yet
        
        openfilename = ''; % contains the path of any currently open files
        
        FemmProblem = struct (); % The femmproblem structure representing the problem
        
    end
    
    methods (Access = protected)
          
          function hfig = plotvectorfield(this, method, x, y, w, h, points, datafcn)
              % creates a plot of a vector field on the FemmProblem
              %
              % Syntax
              %
              % hpproc.plotvectorfield(method, x, y, w, h, points, datafcn)
              %
              % Input
              %
              %   method - plot method use 0 for a vector field plot using
              %     coloured arrows. Use 1 for a contour plot of the
              %     magnitude of the magnetic field.
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
              %   points - determines the number of points that will be
              %     plotted using method 0. If points is a scalar, a grid of
              %     this number of points on both sides will be created. If
              %     points is a two element vector it will be the number of
              %     points in the x and y direction respectively.
              % 
              %
              
              if ~isempty (this.FemmProblem)
                  [hfig, hax] = plotfemmproblem(this.FemmProblem);
              else
                  hfig = figure;
              end
              
              if isscalar(points)
                  points = [points, points];
              end
              
              xgv = linspace(x, x + w, points(1));
              ygv = linspace(y, y + h, points(2));
              [Xsample,Ysample] = meshgrid(xgv, ygv);
              
              data = feval(datafcn, Xsample, Ysample);
              
              switch method
                  
                  case 0
                      % plot a vector field using colored arrows
                      cquiver( cat(3, reshape(data(1,:), size(Xsample)), reshape(data(2,:), size(Xsample))), ...
                               'sx', xgv(2)-xgv(1), ...
                               'sy', ygv(2)-ygv(1), ...
                               'xshift', x, ...
                               'yshift', y, ...
                               'hax', hax );
                           
                       colorbar;
                  case 1
                      % contour plot
                      contour( Xsample, Ysample, reshape(magn(data), size(Xsample)) );
                      
                  otherwise
                          
              end
              
              hold off;
              
              set (hax, 'XLim', [x, x + w], 'YLim', [y, y + h]);
              
          end
          
          function hfig = plotscalarfield(this, method, x, y, w, h, points, datafcn)
              % creates a plot of a scalar field on the FemmProblem
              %
              % Syntax
              %
              % hpproc.plotscalarfield(method, x, y, w, h, points, datafcn)
              %
              % Input
              %
              %   method - plot method use 0 for a vector field plot using
              %     coloured arrows. Use 1 for a contour plot of the
              %     magnitude of the magnetic field.
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
              %   points - determines the number of points that will be
              %     plotted using method 0. If points is a scalar, a grid of
              %     this number of points on both sides will be created. If
              %     points is a two element vector it will be the number of
              %     points in the x and y direction respectively.
              % 
              %
              
              if ~isempty (this.FemmProblem)
                  [hfig, hax] = plotfemmproblem(this.FemmProblem);
              else
                  hfig = figure;
              end
              
              if isscalar(points)
                  points = [points, points];
              end
              
              xgv = linspace(x, x + w, points(1));
              ygv = linspace(y, y + h, points(2));
              [Xsample,Ysample] = meshgrid(xgv, ygv);
              
              data = feval(datafcn, Xsample, Ysample);
              
              switch method
                  
                  case 0
                      % plot a scalar field using filled contour
                      contour(Xsample,Ysample,data);

                      pcolor(Xsample,Ysample,data);

                      shading interp;
                      
                      colorbar;
                       
                  case 1
                      % contour plot
                      contour ( Xsample, Ysample, data );
                      
                      colorbar;
                      
                  otherwise
                          
              end
              
              hold off;
              
              axis equal
              set (hax, 'XLim', [x, x + w], 'YLim', [y, y + h]);
              
          end
          
          
      end

end