# CS 3GC3 Final Project - Scavenger Hunt Game

## Details of to be added/fixed for final implementation
- The game now begins immediately with a bedroom scene, however we want to design additional scenes and provide the player with a menu to choose which scene to play through, to improve replayability.
- Currently, a global timer exists, but the game does not end when the timer reaches zero.
- The player cannot select any items, this will need to be implemented via ray picking.
- Currently, there is no functionality for providing hints to the user as to what objects they need to find. We need to implement some sort of either text-based hint system, or else an image of the object they need to find.
- Additionally, once the above have been implemented, scoring and some sort of win/loss system needs to be implemented.
- Collision detection works currently but needs changes; objects which collide will lose all motion in all directions (so e.g. if 2 objects are spawned in the same location they get stuck in mid-air).
- Objects should probably not be spawned above objects with odd hitboxes (e.g. the bed or the closet) since the hitbox extends far past the model and causes objects to float in mid-air.
- Objects will have textures applied for the final product.
- Some objects should be animated.

## Overview of project

- Project is a game in which one of many 3d scenes is provided to the user, along with some hints (possibly images, directional indicators, etc) as to what items they need to find in the scene.
- The game is played from a first person perspective and the user will need to actively search through the scene to find objects.
- Game will contain a full-featured first person camera, lighting, 3d models, textures, animations, etc. to make it more interesting to play.
- A timer or scoring system will also be implemented to make the game contain some failure states.

## Coding resources used
https://learnopengl.com/Getting-started/Camera - Some code for camera vectors comes from here, specifically used for rotation/strafing.
https://stackoverflow.com/questions/573084/how-to-calculate-bounce-angle - Used for help with math on bouncing particles against walls.

https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c - function to find if a string ends with another string

https://github.com/syoyo/tinyobjloader - open-source library for loading obj files efficiently
https://github.com/cxong/tinydir - open-source cross-platform library for iterating over files in a directory (which is somehow not provided in c++ at all until c++17??)

## Model sources
https://sketchfab.com/3d-models/bed-403c4a48e5ea4a9fbbb9a096d90973af - bed.obj and related files
https://sketchfab.com/3d-models/socks-3d79cc9684bd4c6094e1c0778e334ce4 - Sock.obj and related files
https://sketchfab.com/3d-models/desk-36e27887358e45d3b400d4c4dd7ac7a8 - desk.obj
https://sketchfab.com/3d-models/desktop-computer-561abc2fc95941609fc7bc6f232895c2 - computer.obj
https://sketchfab.com/3d-models/computer-c0841dac3a424579ac91dee5a7ad4b76 - tower.obj

Models not listed were downloaded through 3D Viewer (application built into Windows 10)

All models downloaded are licensed under CC - https://creativecommons.org/licenses/by/4.0/
All models were ran through Blender for triangulation
A couple models had their poly count reduced through Blender for performance