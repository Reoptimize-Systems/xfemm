% Test_plotnodelinks

nodes = [0,0; 1,0; 1,1; 0.5,1.3; 0.1,1.3; 0,1]; 

links = [0,1; 1,2; 2,3; 3,4; 0,5];

plotnodelinks(nodes, links);

nodes = [0,0,0; 1,0,0; 1,1,1; 0.5,1.3,1; 0.1,1.3,1; 0,1,0]; 

plotnodelinks(nodes, links);