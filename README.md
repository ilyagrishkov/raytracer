# Ray Tracing with OpenGL in C++

Raytracer  that  generates  static  images  from  a  virtual  scene containing lights, objects and a camera.

Features:
• Perform ray intersections with planes, triangles, and bounding boxes 
Figure 1: Shows the debug ray intersect with the objects in the scene.
• Computate shading at the first impact point (diffuse and specular). 
Figure 2: Shows diffuse lighting, like on the cube and specular on the car.
• Perform recursive raytracing for reflections to simulate specular materials. 
Figure 3: Shows reflection and specular. The walls and floor of the scene are highly reflective.
• Calculate hard shadows from a point light 
Figure 4: Shows the hard shadows with one light in the first image and two lights in the second image.
• Calculate soft shadows from a spherical light centered at a point light 
Figure 5: Soft shadows from the car.
• Show an interactive display in OpenGL of the 3D scene and a debug ray tracer. A ray from a chosen pixel should be shown via OpenGL, illustrating the interactions with the surfaces. 
Figure 6: Shows the debug ray bouncing around in the scene.
• Implement a simple and advanced acceleration structure.
Figure 7: 10000 triangles/box
Figure 8: 1000 triangles/box
Figure 9: 500 triangles/box
Figure 10: 100 triangles/box
Figure 11: 50 triangles/box
Figure 12: 10 triangles/box
• Show a scene created by the group, exported as a wavefront object (OBJ) and directly loaded into the application. 
Figure 13: Our final scene for the project
• Extending the debugray by, if a number is pressed it prints out the information of the corresponding ray if it exists. It also dyes the debugray to be the color the ray of light has at that moment 
Figure 14: Shows the information that is printed on the right about the debug ray on the left, when pressing the corresponding keys.
• A numerical evaluation of the performance of your ray tracer. Also a progress bar of how far the rendering is. 
Figure 15: Shows the statistics that is printed out by the ray tracer.
• Multicore support of the ray tracer (implementing additional threads) 
• Adding support for spherical objects (without using the OpenGL provided functions).
Figure 16: Shows a sphere above the cube and reflected in the cube.
• Added ability to change background color in previewer for rendering by pressing keys in the previewer and added ability to place lights and reset the lights back to the standard one in the previewer.
Figure 17: Shows the keys that need to be pressed to change the background color for the eventual render.
• Added the background to not be a standard color but a resemble a starry night sky.
Figure 18: Shows the background created when the background is black in the render, when rendering no objects.
