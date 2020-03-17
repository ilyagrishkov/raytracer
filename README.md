# <div align="center"> RAY TRACING WITH OPENGL IN C++ </div>
Raytracer  that  generates  static  images  from  a  virtual  scene containing lights, objects and a camera.
<br/>

## <div align="center"> SAMPLE IMAGES </div>
<img src="https://user-images.githubusercontent.com/45048351/76903735-d7087a80-689e-11ea-9df6-9298d7882343.png" width="500" height="500" align=center/>
<img src="https://user-images.githubusercontent.com/45048351/76903766-e38cd300-689e-11ea-9f88-32c59e8dd5c2.jpeg" width="500" height="500" align=center/>
<br/>

## <div align="center"> FEATURES </div>
### Perform ray intersections with planes, triangles, and bounding boxes 
![figure1_1](https://user-images.githubusercontent.com/45048351/76903259-04085d80-689e-11ea-923b-2315e04deb0c.png)
![figure1_2](https://user-images.githubusercontent.com/45048351/76903203-e4713500-689d-11ea-8f86-677c6ecbb5f5.png)
<br/>Figure 1: Shows the debug ray intersect with the objects in the scene. <hr>

### Computate shading at the first impact point (diffuse and specular). 
![Page-1-Image-4](https://user-images.githubusercontent.com/45048351/76904771-e7b9f000-68a0-11ea-83fa-86ae4a03919c.png)
<br/>Figure 2: Shows diffuse lighting, like on the cube and specular on the car. <hr>

### Perform recursive raytracing for reflections to simulate specular materials. 
![Page-1-Image-1](https://user-images.githubusercontent.com/45048351/76904822-002a0a80-68a1-11ea-93bf-eddabeb9a0c5.png)
<br/>Figure 3: Shows reflection and specular. The walls and floor of the scene are highly reflective. <hr>

### Calculate hard shadows from a point light 
![Page-2-Image-6](https://user-images.githubusercontent.com/45048351/76904852-133cda80-68a1-11ea-96d4-d7076f0c605f.png)
![Page-2-Image-5](https://user-images.githubusercontent.com/45048351/76904873-1c2dac00-68a1-11ea-9997-73d296d98988.png)
<br/>Figure 4: Shows the hard shadows with one light in the first image and two lights in the second image. <hr>

### Calculate soft shadows from a spherical light centered at a point light
![Page-2-Image-7](https://user-images.githubusercontent.com/45048351/76904980-4e3f0e00-68a1-11ea-8b81-ed7fab8f1258.png)
<br/>Figure 5: Soft shadows from the car. <hr>

### Show an interactive display in OpenGL of the 3D scene and a debug ray tracer. A ray from a chosen pixel should be shown via OpenGL, illustrating the interactions with the surfaces. 
![Page-3-Image-8](https://user-images.githubusercontent.com/45048351/76905026-6d3da000-68a1-11ea-9dd2-4b488009e555.png)
<br/>Figure 6: Shows the debug ray bouncing around in the scene. <hr>

### Implement a simple and advanced acceleration structure.
| Triangles per box | Render Time (sec) |
|-------------------|-------------------|
| 10000             | 78.656            |
| 1000              | 16.644            |
| 500               | 8.283             |
| 100               | 3.5               |
| 50                | 2.361             |
| 10                | 1.838             | 

![Page-3-Image-9](https://user-images.githubusercontent.com/45048351/76905111-98c08a80-68a1-11ea-9633-86d4743bb6d2.png)
<br/> Figure 7: 10000 triangles/box <br/>
![Page-3-Image-10](https://user-images.githubusercontent.com/45048351/76905198-be4d9400-68a1-11ea-92fb-f8d1992a9b30.png)
<br/> Figure 8: 1000 triangles/box <br/>
![Page-3-Image-11](https://user-images.githubusercontent.com/45048351/76905209-c6a5cf00-68a1-11ea-9b08-702c01e09eca.png)
<br/> Figure 9: 500 triangles/box <br/>
![Page-3-Image-12](https://user-images.githubusercontent.com/45048351/76905223-ce657380-68a1-11ea-8186-397b49b94151.png)
<br/> Figure 10: 100 triangles/box <br/>
![Page-3-Image-13](https://user-images.githubusercontent.com/45048351/76905243-d6251800-68a1-11ea-82df-f90cb8840980.png)
<br/> Figure 11: 50 triangles/box <br/>
![Page-3-Image-14](https://user-images.githubusercontent.com/45048351/76905271-e3420700-68a1-11ea-969b-73ad344cb062.png)
<br/> Figure 12: 10 triangles/box <hr>

### Extending the debugray by, if a number is pressed it prints out the information of the corresponding ray if it exists. It also dyes the debugray to be the color the ray of light has at that moment 
![Page-5-Image-17](https://user-images.githubusercontent.com/45048351/76905349-0bca0100-68a2-11ea-9ac2-06f42771837a.png)
<br/>Figure 13: Shows the information that is printed on the right about the debug ray on the left, when pressing the corresponding keys. <hr>

### A numerical evaluation of the performance of your ray tracer. Also a progress bar of how far the rendering is. 
![Page-5-Image-16](https://user-images.githubusercontent.com/45048351/76905371-17b5c300-68a2-11ea-9dd8-0377b2cc19a0.png)
<br/>Figure 14: Shows the statistics that is printed out by the ray tracer. <hr>

### Multicore support of the ray tracer (implementing additional threads)
| Threads | Render Time (sec) |
|---------|-------------------|
| 1       | 113.127           |
| 10      | 23.404            |

### Adding support for spherical objects (without using the OpenGL provided functions).
![Page-6-Image-19](https://user-images.githubusercontent.com/45048351/76905435-3916af00-68a2-11ea-84f6-7412662ff935.png)
<br/>Figure 15: Shows a sphere above the cube and reflected in the cube. <hr>

### Added ability to change background color in previewer for rendering by pressing keys in the previewer and added ability to place lights and reset the lights back to the standard one in the previewer.
![Page-6-Image-18](https://user-images.githubusercontent.com/45048351/76905489-55b2e700-68a2-11ea-95d1-2b643eb59851.png)
<br/>Figure 16: Shows the keys that need to be pressed to change the background color for the eventual render. <hr>

### Added the background to not be a standard color but a resemble a starry night sky.
![Page-7-Image-20](https://user-images.githubusercontent.com/45048351/76905504-5d728b80-68a2-11ea-8ad2-456acb82c0a7.png)
<br/>Figure 17: Shows the background created when the background is black in the render, when rendering no objects.
