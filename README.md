3D Classroom - Computer Graphics Project
A 3D classroom scene created using C++, OpenGL, GLFW, GLAD, and GLM.
The project demonstrates important concepts of computer graphics including 3D transformations, camera/viewing transformations, lighting, materials, animation, and interactive controls.
________________________________________
Project Overview
This project creates an interactive 3D classroom environment using OpenGL.
The classroom contains different objects such as:
•	Classroom walls and floor
•	Sliding entrance door
•	Blackboard
•	Teacher's table
•	Student desks
•	Student chairs
•	Ceiling fan
•	Ceiling lights
•	Spotlight
•	Trash bin
The objects are constructed mainly using 3D primitives such as cubes, with a custom cylinder mesh used for the trash bin.
The project also includes interactive elements such as a sliding door, rotating ceiling fan, movable camera, global object transformations, and controllable lights.
________________________________________
Main Features
1. 3D Transformations
The project demonstrates the three major 3D transformations:
•	Translation
•	Rotation
•	Scaling
These transformations can be applied to the complete classroom scene.
The transformation system uses GLM matrices:
•	glm::translate()
•	glm::rotate()
•	glm::scale()
The transformations are combined into a model transformation matrix before rendering.
________________________________________
2. Viewing Transformation
A custom Camera class is used to control the viewpoint.
The camera supports:
•	Forward and backward movement
•	Left and right movement
•	Up and down movement
•	Left and right rotation
•	Mouse-based looking
•	Zoom using the mouse scroll wheel
Perspective projection is used to create a realistic 3D view.
________________________________________
3. Moving Objects
The classroom contains animated objects.
Sliding Door
Pressing SPACE opens or closes the classroom's sliding door.
The door does not instantly change position. Instead, it smoothly moves toward its target position using frame-based animation.
Ceiling Fan
The ceiling fan rotates continuously when it is turned on.
Press F to turn the fan on or off.
________________________________________
4. Lighting
The project demonstrates two different types of light sources.
Point Lights
There are four point lights positioned near the classroom ceiling.
They simulate classroom ceiling lights and use:
•	Ambient lighting
•	Diffuse lighting
•	Specular lighting
•	Attenuation
The attenuation uses:
•	Constant factor
•	Linear factor
•	Quadratic factor
Press 1 to turn the point lights on or off.
Spotlight
A spotlight is positioned above the teacher's table/presentation area.
It uses:
•	Ambient lighting
•	Diffuse lighting
•	Specular lighting
•	Attenuation
•	Inner cutoff angle
•	Outer cutoff angle
The outer cutoff creates a softer edge around the spotlight.
Press 2 to turn the spotlight on or off.
________________________________________
Lighting Model
The classroom uses the Phong lighting model.
For each object, the shader calculates:
Ambient Component
Provides basic illumination even when a surface is not directly facing a light.
Diffuse Component
Depends on the angle between the surface normal and the direction of the light.
Specular Component
Creates shiny highlights based on the viewing direction and reflected light.
The final lighting color is calculated by combining these components.
________________________________________
Materials
Different objects in the classroom use different material properties.
Each material contains:
•	Ambient color
•	Diffuse color
•	Specular color
•	Shininess
This allows objects such as the floor, walls, desks, chairs, blackboard, and door to have different appearances under the same lighting system.
________________________________________
3D Objects
The scene contains several classroom objects.
Room
The main classroom consists of:
•	Floor
•	Walls
•	Ceiling
Blackboard
A large blackboard is placed at the front of the classroom.
Student Desks and Chairs
Multiple desks and chairs are arranged inside the classroom to create a realistic classroom layout.
Teacher's Table
A larger table is placed near the blackboard.
Sliding Door
The entrance door can be opened and closed using the SPACE key.
Ceiling Fan
The fan is positioned on the ceiling and rotates when enabled.
Trash Bin
A custom curved cylinder mesh is used to create the trash bin instead of simply scaling a cube.
________________________________________
Technologies Used
•	C++
•	OpenGL 3.3 Core
•	GLFW
•	GLAD
•	GLM
•	GLSL
________________________________________
Project Structure
Classroom/
│
├── Classroom.sln
│
└── Classroom/
    │
    ├── main.cpp
    │
    ├── camera.h
    ├── shader.h
    ├── pointLight.h
    ├── spotLight.h
    │
    ├── classroomVertexShader.vs
    ├── classroomFragmentShader.fs
    ├── vertexShader.vs
    ├── fragmentShader.fs
    │
    ├── glad.c
    │
    ├── Classroom.vcxproj
    ├── Classroom.vcxproj.filters
    ├── Classroom.vcxproj.user
    │
    └── Dependencies/
        ├── include/
        │   ├── glad/
        │   ├── GLFW/
        │   ├── KHR/
        │   └── glm/
        │
        └── lib/
            └── glfw3.lib
________________________________________
Important Files
main.cpp
Contains the main OpenGL program.
It handles:
•	Window creation
•	OpenGL initialization
•	VAO/VBO/EBO creation
•	Classroom object creation
•	Camera setup
•	Lighting setup
•	Animation
•	Keyboard input
•	Mouse input
•	Rendering loop
________________________________________
camera.h
Contains the custom Camera class.
It handles:
•	Camera position
•	Camera direction
•	Euler angles
•	Movement
•	Mouse rotation
•	Zoom
•	View matrix calculation
________________________________________
pointLight.h
Contains the PointLight class.
It controls the four classroom point lights and their:
•	Ambient component
•	Diffuse component
•	Specular component
•	Attenuation
________________________________________
spotLight.h
Contains the SpotLight class.
It controls the presentation/board spotlight and its:
•	Position
•	Direction
•	Light colors
•	Attenuation
•	Inner cutoff
•	Outer cutoff
________________________________________
shader.h
Provides the shader abstraction used to:
•	Compile shaders
•	Activate shader programs
•	Send matrices
•	Send vectors
•	Send floating-point values to GLSL
________________________________________
classroomVertexShader.vs
The main lighting vertex shader.
It performs:
•	Model transformation
•	View transformation
•	Projection transformation
•	Normal transformation
•	Point-light calculations
•	Spotlight calculations
•	Phong lighting calculations
________________________________________
classroomFragmentShader.fs
Receives the calculated lighting color from the vertex shader and outputs the final fragment color.
________________________________________
Controls
Key	Function
SPACE	Open/close sliding door
W	Move camera forward
S	Move camera backward
A	Rotate camera left
D	Rotate camera right
Q	Move camera down
E	Move camera up
LEFT MOUSE + DRAG	Look around
X	Select X-axis rotation
Y	Select Y-axis rotation
Z	Select Z-axis rotation
R	Rotate selected axis
I	Translate scene upward
K	Translate scene downward
J	Translate scene left
L	Translate scene right
O	Translate scene backward
P	Translate scene forward
C	Scale X down
V	Scale X up
B	Scale Y down
N	Scale Y up
M	Scale Z down
U	Scale Z up
F	Turn ceiling fan on/off
1	Turn point lights on/off
2	Turn spotlight on/off
ESC	Exit the program
________________________________________
How the Transformation System Works
The complete classroom uses a global transformation matrix.
The transformation is constructed using:
glm::mat4 translateMatrix =
    glm::translate(identityMatrix,
                   glm::vec3(translate_X, translate_Y, translate_Z));

glm::mat4 rotateXMatrix =
    glm::rotate(identityMatrix,
                glm::radians(rotateAngle_X),
                glm::vec3(1.0f, 0.0f, 0.0f));

glm::mat4 rotateYMatrix =
    glm::rotate(identityMatrix,
                glm::radians(rotateAngle_Y),
                glm::vec3(0.0f, 1.0f, 0.0f));

glm::mat4 rotateZMatrix =
    glm::rotate(identityMatrix,
                glm::radians(rotateAngle_Z),
                glm::vec3(0.0f, 0.0f, 1.0f));

glm::mat4 scaleMatrix =
    glm::scale(identityMatrix,
               glm::vec3(scale_X, scale_Y, scale_Z));
These matrices are combined to create the final scene transformation.
________________________________________
Animation
Door Animation
The door has two states:
Closed → Open
Open   → Closed
When SPACE is pressed, the target state changes.
The door then gradually moves toward the target position using deltaTime.
This makes the door movement smooth and frame-rate independent.
Fan Animation
The ceiling fan angle is continuously updated while the fan is enabled.
fanAngle = fanAngle + deltaTime × FAN_SPEED
The angle is kept within a complete 360-degree rotation.
________________________________________
Camera System
The camera uses Euler angles:
•	Yaw controls horizontal rotation.
•	Pitch controls vertical rotation.
The camera calculates three important vectors:
•	Front
•	Right
•	Up
These vectors are used to calculate the camera's view matrix using:
glm::lookAt()
Perspective projection is created using:
glm::perspective()
This provides the depth and perspective effect expected from a 3D environment.
________________________________________
Mesh Construction
Most classroom objects use a reusable unit cube.
The cube contains:
•	Vertex positions
•	Surface normals
•	Indices
The same cube can be transformed into different objects by changing its:
•	Position
•	Size
•	Rotation
•	Color
A separate cylinder mesh is generated for the trash bin using 32 segments.
The cylinder contains:
•	Side faces
•	Top cap
•	Bottom cap
•	Surface normals
This demonstrates the use of both basic and custom 3D geometry.
________________________________________
Requirements Covered
This project demonstrates the following computer graphics concepts:
•	3D object modeling
•	3D translation
•	3D rotation
•	3D scaling
•	Model transformation
•	View transformation
•	Perspective projection
•	Camera movement
•	Mouse camera control
•	Phong lighting
•	Ambient lighting
•	Diffuse lighting
•	Specular lighting
•	Point lights
•	Spotlight
•	Light attenuation
•	Materials
•	Object animation
•	Interactive keyboard controls
•	Custom cylinder geometry
•	Depth testing
________________________________________
Running the Project
Requirements
A Windows system with:
•	Visual Studio
•	C++ development tools
•	OpenGL-compatible graphics support
The project is provided as a Visual Studio solution:
Classroom.sln
Steps
1.	Extract the project ZIP file.
2.	Open Classroom.sln using Visual Studio.
3.	Make sure the project is set as the startup project.
4.	Build the solution.
5.	Run the project.
The required GLFW, GLAD, and GLM files are included inside the Dependencies folder.
________________________________________
OpenGL Version
The project requests:
OpenGL 3.3 Core Profile
The shader programs use:
#version 330 core
________________________________________
Project Objective
The main objective of this project is to demonstrate how fundamental computer graphics concepts can be combined to create an interactive 3D environment.
Instead of using pre-built 3D models, the classroom is constructed programmatically using geometric primitives and transformations. Lighting, materials, camera movement, animation, and user interaction are then added to make the environment more realistic and interactive.
________________________________________
Author
3D Classroom - Computer Graphics Project
Built using C++, OpenGL, GLFW, GLAD, and GLM.
