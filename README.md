# CS 3GC3 Final Project - Scavenger Hunt Game

## Overview of project

- Project is a game in which one of many 3d scenes is provided to the user, along with some hints (possibly images, directional indicators, etc) as to what items they need to find in the scene.
- The game is played from a first person perspective and the user will need to actively search through the scene to find objects.
- Game will contain a full-featured first person camera, lighting, 3d models, textures, animations, etc. to make it more interesting to play.
- A timer or scoring system will also be implemented to make the game contain some failure states.

## Coding resources used
https://learnopengl.com/Getting-started/Camera - Some code for camera vectors comes from here, specifically used for rotation/strafing.
https://stackoverflow.com/questions/573084/how-to-calculate-bounce-angle - Used for help with math on bouncing particles against walls.
https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection - collision checking for circles based from here (simple math)
https://en.wikipedia.org/wiki/Rotation_matrix - generalized form of rotation matrix used for rotating camera (and other objects like lights) around the scene/around arbitrary axis.

https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c - function to find if a string ends with another string

https://github.com/syoyo/tinyobjloader - open-source library for loading obj files efficiently
https://github.com/cxong/tinydir - open-source cross-platform library for iterating over files in a directory (which is somehow not provided in c++ at all until c++17??)

## Model sources
https://sketchfab.com/3d-models/bed-403c4a48e5ea4a9fbbb9a096d90973af - bed.obj and related files
https://sketchfab.com/3d-models/socks-3d79cc9684bd4c6094e1c0778e334ce4 - Sock.obj and related files

All models downloaded are licensed under CC - https://creativecommons.org/licenses/by/4.0/
All models were ran through Blender for triangulation