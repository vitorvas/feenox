Merge "Stanford_Bunny.stl";
angle = 40;
includeBoundary = 1;
forceParametrizablePatches = 0;
curveAngle = 180;

ClassifySurfaces{angle * Pi/180, includeBoundary, forceParametrizablePatches, curveAngle * Pi / 180};
CreateGeometry;

// Create a volume as usual
Surface Loop(1) = Surface{:};
Volume(1) = {1};

Physical Volume("bunny") = {1};
Physical Surface("rest") = {3};
Physical Surface("base") = {2};

Mesh.MeshSizeMin = 10;
Mesh.MeshSizeMax = 10;
Mesh.ElementOrder = 1;
Mesh.MeshSizeExtendFromBoundary = 0;
