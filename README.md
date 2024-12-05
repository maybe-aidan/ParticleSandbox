# Particle Sandbox
A raymarched render of Newtonian 'particle' interactions. Currently no build system, I made this as a personal project.

## About
If you are able to build and compile the project, you can use the WASD keys to move the camera around, and the mouse to look. Scroll to change fired particle speed, left click to fire the particle.

## Known Issues
- There are some rendering issuesas the simulation progresses through time. I will try to fix this in the future. 
- There is also no protection against going above the max number of particles (100). So it is entirely possible that this will crash when you get to that point. However, the particles tend to eat each other before you can get to that point anyway. I will fix this in the future.

## Dependencies and Credits
- All the libraries and third party code I used are included in the repository listed under dependencies.
- The classes 'Camera', 'Shader', and 'Texture' are taken directly from [LearnOpenGL](https://learnopengl.com/) with minimal changes made to accomodate the needs of the program. All credit goes to them for those classes, as well as for teaching me the basics of Graphics Programming in OpenGL.
