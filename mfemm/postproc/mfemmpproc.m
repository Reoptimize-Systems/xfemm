classdef mfemmpproc < handle
    % mfemmpproc - base class for xfemm post-processing C++ interfaces


    properties (SetAccess = protected, Hidden = true)
        
        objectHandle; % Handle to the underlying C++ class instance
        
    end
    
    properties (SetAccess = protected, Hidden = false)

        isdocopen = false; % flag denoting whether a document has been opened yet
        
        openfilename = ''; % contains the path of any currently open files
        
        FemmProblem = struct (); % The femmproblem structure representing the problem
        
    end
    
    methods (Access = protected)
          
          function hfig = plotvectorfield(this, datafcn, x, y, w, h, varargin)
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
            
              Inputs.PlotNodes = true;
              Inputs.Points = 40;
              Inputs.Method = 0;
              
              Inputs = mfemmdeps.parseoptions (Inputs, varargin);
              
              if ~isempty (this.FemmProblem)
                  [hfig, hax] = plotfemmproblem(this.FemmProblem, ...
                                    'PlotNodes', Inputs.PlotNodes, ...
                                    'InitialViewPort', [x,y,w,h]);
              else
                  hfig = figure;
              end
              
              if isscalar(Inputs.Points)
                  Inputs.Points = [Inputs.Points, Inputs.Points];
              end
              
              xgv = linspace(x, x + w, Inputs.Points(1));
              ygv = linspace(y, y + h, Inputs.Points(2));
              [Xsample,Ysample] = meshgrid(xgv, ygv);
              
              data = feval(datafcn, Xsample, Ysample);
              
              switch Inputs.Method
                  
                  case 0
                      % plot a vector field using colored arrows
                      mfemmdeps.cquiver( cat(3, reshape(data(1,:), size(Xsample)), reshape(data(2,:), size(Xsample))), ...
                               'sx', xgv(2)-xgv(1), ...
                               'sy', ygv(2)-ygv(1), ...
                               'xshift', x, ...
                               'yshift', y, ...
                               'hax', hax );
                           
                       colorbar;
                  case 1
                      % contour plot
                      contour( Xsample, Ysample, reshape(mfemmdeps.magn(data), size(Xsample)) );
                      
                  otherwise
                          
              end
              
              hold off;
              
              set (hax, 'XLim', [x, x + w], 'YLim', [y, y + h]);
              
          end
          
          function hfig = plotscalarfield(this, datafcn, x, y, w, h, varargin)
              % creates a plot of a scalar field on the FemmProblem
              %
              % Syntax
              %
              % hpproc.plotscalarfield(method, x, y, w, h, points, datafcn)
              %
              % Input
              %
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
              %    'Method' - plot method use 0 for a filled contour plot.
              %      Use 1 for a contour plot with just lines.
              %
            
              Inputs.PlotNodes = true;
              Inputs.Points = 100;
              Inputs.Method = 0;
              
              Inputs = mfemmdeps.parseoptions (Inputs, varargin);
              
              if ~isempty (this.FemmProblem)
                  [hfig, hax] = plotfemmproblem(this.FemmProblem, 'PlotNodes', Inputs.PlotNodes);
              else
                  hfig = figure;
              end
              
              if isscalar(Inputs.Points)
                  Inputs.Points = [Inputs.Points, Inputs.Points];
              end
              
              xgv = linspace(x, x + w, Inputs.Points(1));
              ygv = linspace(y, y + h, Inputs.Points(2));
              [Xsample,Ysample] = meshgrid(xgv, ygv);
              
              data = feval(datafcn, Xsample, Ysample);
              
              switch Inputs.Method
                  
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