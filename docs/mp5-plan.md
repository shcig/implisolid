Three files:

ImpliSolid plan

mp5 plan
Designer plan
Others (WeDesign)
==========
mp5-slicer:
wedesign: We need a standalone version of slicer (in mp5slicer repo). It generates an STL from an mp5 file. The commandline should be like this (details may vary) 
mp5slice myfile.mp5 myfile.stl  #simple use
mp5slice myfile.mp5 -stl myfile.stl #complete
mp5slice myfile.mp5 -m myfile2.mp5 -stl myfile.stl #merges multiple files

mp5-format:
File bundle:
The idea is simlar to JAR files in java, and S?? files in SolidWorks.
multiple parts
parts that refer to eachother. For example primitives (standard shapes)

mp5-designer:
term: standard Object (or primitive)
term: Constraint
A geometric arrangement that become permanent.
term: lock-down:
The properties of an objects are constant. Even the overall rotation (matrix).
term: protected:
An object that can be moved, scaled, etc, but the inside infomration cannot be changed. Also the inside information is not visible to the user.
All standard objects (primitives) are protected.

term: matrix of an object.

matrices are hierarchical.
status: implemented (in designer, implisolid).
meaning:
useful for references, primitives.

term: refernce.
The references are not implemented.
This allows multiple instances of something.
Also an external refrence can be to anoterh person's object. (thorugh its mp5 unique ID).
Can be useful for using while not violating.

New license category:
can be used inside another object without modification. (referensable)

----------
mp5-implisolid:
we need an NPM library.

wedesign:
We can use MongoDB or couchDB for indexing the contents of MP5.
But the mp5 files should be stored as blobs, binaries, or files on the file system. Not directly on json-based databases. Instead, if indexing is neceessary, you can use for indexing purposes.
Reasons:
1- not goo to be updatable.
2- consistency is difficult.
3- it is against independence
4- contents of mp5 json may be changed for storage purposes.
5- We will want to radically change the format without changing the database's schema. 
6- independence.
7- People need to sownload the json. It is not good to dynamically generate the mp5 files. Because it may not be produced exactly in the same way.
8- hashing (md5) will not be possible.
history:
CouchDB was tried but decided not to be used. Contact Manon and Flavien Delangle for more information. 

mp5-format: yaml variation: is completely valid.
status: not implemented.

mp5-format: JSON-family of formats.
In fact all representations that are equivalent to JSON are valid. (which are yaml and json). Variations of JSON may be used in future, but a constaint is that, 
the mp5 contents should be easily readable by any langyuage tha provides the simples funcitons JSON.stringufy() and JSON.parse(). (for exmaple Python and Javascript
EJSON, etc: ?

In future decisions:
mp5 will remain a spearate file format. Not a format in databse, etc.
mp5 is doownloadable.
mp5 is usable in the exact same way (without any transformation) outside wedeisng or any platform.
For example some printers in future may want to ue it directly.
This is a high-priority decision and should be used in future. Contact SSi for this.

# mp5-format

Individual elements can include polygonisation settings.
as well as print-settings.

Question:
If two mp5 files with different print settings are merged (linked)?


mp5-implisolid:
Fonts support:
Progtress and research:
seach term: 


mp5-implisolid:
Ellipsoid SDF:
Summary of literature found:
https://www.spaceroots.org/documents/distance/distance-to-ellipse.pdf
ShaderToy.

mp5-implisolid:
a good developer communities:
shader-toy developers. Search SDF in shadertoy.

mp5-implisolid:
We need a guide for develoipers and developer-artists for easily using this. For example a tutorial. A displayer (interactive?). A commandline mesh generator. An STL generator. A web-based genrator.
WebWorker version, nont-web-worker JS version, server-side version, ..

GPU version (shader).
compile your mp5 into a shader code that is directly insertable into shadertoy.


designer ideas:
(also implisolid):
modelling-techniques.tex

Chamfer
Special points (specific for Cone, etc). Used by a key (string).


mp5-designer: refactoring suggestions

mp5-designer:
that open modeller language.
OpenModellica and its JavaScript versions (its open-source contributors)

small websites to showcase. in style of kovacsv/JSmodeler. and pathtrace.js.org

implisolid:
ways of modelling fonts.


implisolid:
"shell" operation.

quasi-"offset"

R-union

possibility of B-Rep remains open.



very important:
edges.
(1- special edges: customised.
2- sharp-edges, automatically)
* SVD of mesh, but confirm by the implicit function.
* Multi-implicit.



A 2D-implicit editing system, capable of fre form curves.

a curving transform to map it on certain (or all) shapes.
(cyclinder, sphere, etc; one for eahc).



Bloomenthal: Skeletal models & Implicit Blends (slides)



Priorities:
- trivar Splines
- Skeletal

- Screw+Twist+Kaleido+repeater+[infill-repeat]
(maybe escher?) + that round twisted Mobius sculpture

- Shell+offset+

- Harmonic * (See Tobias Martin's work)
- Radial basis, hamilton, etc

Terms used instead of Implicit Modelling:
- RSDF, SDF, SARDF, ...
- antimony (also see thesis)
- Uformia
- (implicit skin) -(hudini muscle model) -...
- Layered-Depth-Normal-Image (LDNI)
- FRep/hyperfun, shapeshop, ...
- genral: implicitcad,
- ASDF (see Antimony)
- 



External literatures:
- Reservoir modelling
- PDE/Fluid modelling (Persson)
- Medical imaging: MRI, UltraSound, etc
- Computer vision: 3d (NN, convoilution-volumes)


Nice examples from:
- Bloomenthal's skeletal.
- Antomony's solids.
- ...



Q: How to have a metallic shine as usually used in SolidWorks, Catia, etc?
A: It is very easy to do in ThreeJS (2 lines of code). In fact there are samples on internet for this specific kind of material.
Note that it typically it uses an "environmental map" for reflection.
This finish (material) gives a very professional look, and will make it look like professional Engineering softwares.
Settings like reflection, shininess, metal,etc but don't try to tune these parameters. It takes forever to find the right combination. Instead, there are a lot of examples on web and stackoverflow.


Nice Materials:
- Objects with bumptmaps and random (whitenoise) textures. It can immitate athe rough shape of the printed object.
- Materials for each filament type. Whcih can be used in the printer mode.


Designer:
Effects:
- Outline; which can be implemented using post-processing. Also halo effect. This is very useful and necessary, for highlighting objects, etc.


3D special effects:
- Glowing edges for sci-fi (Iron-man style of UI)
- Glow halo effect (possible using post processing)
- Snow and other atmospher/environment/theme things. We can have Easter Eggs (i.e. hidden tricks).


STL export:
Currently we always polygonise before printing, so I suggest we always save the STL file of that polygonization in the process of slicing. We can have these buttons on slice menu:

[Slice]   <-    always available. starts the backend slicer
[download gcode]  <-- available when Slicer is done
[preview gcode] <-- available when Slicer is done
[download STL]  <----  when Slicer is done (or before g-code is generated)
In other words, STL export can be accessible in the print/slice menu. We can put a button in the Save/Load menu that tells the user to go to the slice menu.



Angles settings:


Designer glossary:
Print mode:
When we open the print menu, the materials change. The "display-colors" are not used and the filament color (and texture) is used according to the chonsen print settings.

Errata:
Print-settings: 
There is not "Printer-Settings". We have "print settings". The classes in  the code, and also the text in the designer should change. This sessions of the MP5 format should be changed.


Implisolid: Patch, lines, points.

Feature conversion of STL files into MP5:
Suggested appraoch 1:
Turn the STL file into a point-cloud.
Use the current techniques for (Tobias Martin's method using trivar-Splines and Hoarmonic (?) is recommended.
Nevertheless, there is a bit literature on this for scanning objects (see below). The literature is for scannign objects and also for medical imaging (e.g. MRI voxelated data) )


Literature on:
Converting point-clouds: Two types: MRI (voxels volumic) ad Scanner (point-cloud).
Tobias Martin (MRI)
Multi-Level Partition of Unity Implicits (Belyaev, Ohtake, Alexa, Turk, Seidl. 2003)




Misc Survey:
Existing libraries:

libigl
- https://github.com/libigl/libigl
- Seems a well-made library.
- Can be used by C++ engine.

Modelling techniques

Modelling key-words:
Isogeometric = ?




Aligning:


See distance.


Move down:


Object Set:


Feature:
Clean Edges using lines (Curves) on sharpe edges.
This will emphasise on our power of representing natural curvature of surfaces.
Second iteration:
An algorithm for increasing the resolution of the curves independent of the polygonisation. (inspired by the OB02 paper)




highly needed:
ImpliSolid:
SHELL
  (SDF)

Designer:
Align
Align based on closest points between two objects.
Align based on Implicit cragient (move toward cylinder versus move toward sphere)
Freely moving point.
Align based on special points (cutomised for each shape)


Designer: (code)
Custom parameters of each shape.


Involving the community on ShaderToy who use SDF functions.
Using SDF functions deveoped there and their implementations.
Involving those developers and encouraging them to participate in WeDesign.
Examples: Heart, etc.

Long term future: 
A gpu-based visualiser shader.



License


============
December 2016







Subtasks for Speed of CSG operations (Sohail)
WebWorker communication: verts, faces
design the right protocol
Translating the current version into a Worker version
Make updating in the background
Update-med or replace mesh
querying multi-mesh & sharp lines
find_min_z
query_bounding_box
object3d (owner class, multipleindices)
select surface
select point
multi-marching-cubes
tip of the cone
calculate curvatures (for closing the subdivision)
marching-cubes resolution size
bug for cyliner (Vitaliy)
Free-form-shape
(update only live nodes. others consolidated are no longer updated)

Designer: Free-form Tetrahedron
Designer: Free-form organic (TriVarSpline)
refactor the metaballs
refactor the C++ code
Symbolic implicit
fast run-time optimisation
highlight hover curves/surfaces
add point
note-on-point
exploring EmBind


when it's live on firefox the screenshot is white
suggestion: zoom in zoom out on mouse tip




For VV:
* Symbolic implicit + Auto Diff + AST + Runtime compiler/optimiser
* Tetrahedron Point annotation
* trivar Spline / Harmonic
* put min_z into designer

Myself:
* put min_z into designer
* calculate curvatures (for closing the subdivision)
* snow
* large MC
* EmBind
* Environment



Old tasks are stored in various places:
https://trello.com/b/Arijc350/rihanna-work


Add check for matrix condition, i.e. matrix condition (min eigenvvalue in SVD) to the invariant function of each implicit_function.
