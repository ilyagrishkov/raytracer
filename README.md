# Ray Tracing with OpenGL in C++

Raytracer  that  generates  static  images  from  a  virtual  scene containing lights, objects and a camera.
## Sample images
<img src="https://user-images.githubusercontent.com/45048351/76903735-d7087a80-689e-11ea-9df6-9298d7882343.png" width="500" height="500" />
<img src="https://user-images.githubusercontent.com/45048351/76903766-e38cd300-689e-11ea-9f88-32c59e8dd5c2.jpeg" width="500" height="500" />

## Features:
### Perform ray intersections with planes, triangles, and bounding boxes 
![figure1_1]("https://user-images.githubusercontent.com/45048351/76903259-04085d80-689e-11ea-923b-2315e04deb0c.png" align="center")
![figure1_2]("https://user-images.githubusercontent.com/45048351/76903203-e4713500-689d-11ea-8f86-677c6ecbb5f5.png" align="center")
<br/>Figure 1: Shows the debug ray intersect with the objects in the scene. <hr>
### Computate shading at the first impact point (diffuse and specular). 
Figure 2: Shows diffuse lighting, like on the cube and specular on the car. <hr>
### Perform recursive raytracing for reflections to simulate specular materials. 
Figure 3: Shows reflection and specular. The walls and floor of the scene are highly reflective. <hr>
### Calculate hard shadows from a point light 
Figure 4: Shows the hard shadows with one light in the first image and two lights in the second image. <hr>
### Calculate soft shadows from a spherical light centered at a point light 
Figure 5: Soft shadows from the car. <hr>
### Show an interactive display in OpenGL of the 3D scene and a debug ray tracer. A ray from a chosen pixel should be shown via OpenGL, illustrating the interactions with the surfaces. 
Figure 6: Shows the debug ray bouncing around in the scene. <hr>
### Implement a simple and advanced acceleration structure.
Figure 7: 10000 triangles/box
Figure 8: 1000 triangles/box
Figure 9: 500 triangles/box
Figure 10: 100 triangles/box
Figure 11: 50 triangles/box
Figure 12: 10 triangles/box <hr>
### Extending the debugray by, if a number is pressed it prints out the information of the corresponding ray if it exists. It also dyes the debugray to be the color the ray of light has at that moment 
Figure 13: Shows the information that is printed on the right about the debug ray on the left, when pressing the corresponding keys. <hr>
### A numerical evaluation of the performance of your ray tracer. Also a progress bar of how far the rendering is. 
Figure 14: Shows the statistics that is printed out by the ray tracer. <hr>
### Multicore support of the ray tracer (implementing additional threads) 
### Adding support for spherical objects (without using the OpenGL provided functions).
Figure 15: Shows a sphere above the cube and reflected in the cube. <hr>
### Added ability to change background color in previewer for rendering by pressing keys in the previewer and added ability to place lights and reset the lights back to the standard one in the previewer.
Figure 16: Shows the keys that need to be pressed to change the background color for the eventual render. <hr>
### Added the background to not be a standard color but a resemble a starry night sky.
Figure 17: Shows the background created when the background is black in the render, when rendering no objects.
