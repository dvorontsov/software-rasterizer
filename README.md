# Software Rasterizer (WIP)
Reads models in OBJ format, reads texture files.  Produces image file as output rendered completely on the CPU.  Currently, configuration is done in code.  The project has minimal dependencies and uses CMake for project generation.

## Motivation
This is a toy project and its purpose is to learn how rasterization works.  

## Programming Language
Written in C.  Originally, this project was planned as a set of header libraries that can be included in other projects.  The language C was chosen as the least common denominator of C/C++.  Since the project has evolved since then, I am considering porting it to C++.

## Features
- Primitives drawing: point, line, triangle
- Positionable camera
- Texture mapping
- Flat shading
- Gouraud shading
- Loading models in OBJ format (incomplete)
- Writing images in TGA format

## Dependencies
- stb_image for loading image files

## Build
Clone the project, then from project root directory run
```
mkdir build
cd build
cmake .. -G <generator>
```
You can type "cmake -G" to get a list of generators for your system.
Once the project is generated, you can open it in Visual Studio, for example, and should be able to build and run.

## Screenshots
Textured cube
![Alt text](https://github.com/dvorontsov/private-tga-image-loader/blob/master/screenshots/out_image_1.png)

Flat shaded monkey Suzanne (example model from Blender)
![Alt text](https://github.com/dvorontsov/private-tga-image-loader/blob/master/screenshots/out_image_2.png)

Gouraud shaded, checker-board textured monkey Suzanne (example model from Blender)
![Alt text](https://github.com/dvorontsov/private-tga-image-loader/blob/master/screenshots/out_image_3.png)