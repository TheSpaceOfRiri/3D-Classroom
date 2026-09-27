# 3D Classroom - Fixed Keyboard Door Entry

This version keeps the classroom scene and the Kitchen/3D_CubeTransformation-style keyboard functionality, but changes the interaction flow:

1. Run the program.
2. The camera starts OUTSIDE the classroom, facing the CLOSED sliding door.
3. The mouse is NOT used for camera control. It will not drag/rotate the screen.
4. Press SPACE once. The door slides open.
5. After the door is fully open, W/A/S/D movement is enabled so the user can enter.
6. F toggles the ceiling fan.
7. 1 toggles the four point lights.
8. 2 toggles the spotlight.
9. X/Y/Z select a transformation axis and R rotates it.
10. I/K, J/L, O/P translate the whole classroom; C/V, B/N, M/U scale it.

Important: after replacing the project, use Visual Studio **Clean Solution**, then **Rebuild Solution**, and run the newly built Debug/Release target.
