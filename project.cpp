#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

// tinyobj defines both the implementation and the definition in the one .h file.
// due to this we use this definition to indicate when it should be providing implementation
// and undef it after. this prevents the implementation from beign declared twice
#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION

#include "include/tinydir.h"

#include "mathLib3D.h"
#include "camera.h"
#include "light.h"
#include "game_object.h"

#include "mesh.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <map>
#include <iterator>

// the two Vec3D represent the eye position and the lookAt position
Camera camera = Camera(Vec3D(0.0, 0.0, 0.0), Vec3D(1.0, 0.0, 0.0));

// width and height of the screen.
int screen_width = 600;
int screen_height = 600;

int timer = 60 * 60 * 10;

// movement inputs
bool movement[] = {false, false, false, false};

// map of obj file names to Mesh pointers
// the Mesh ptr should be passed through to the actual game object
// so it can render itself
std::map<std::string, Mesh*> meshes;
// a list of all GameObjects which can be drawn in the scene, moved, interacted with, etc
std::vector<GameObject> objs;
// couple of lights
Light l;
Light l1;

/**
* Handles regular keyboard inputs (e.g. w/s/a/d for movement)
*/
void handleKeyboard(unsigned char key, int _x, int _y)
{
    switch (key) {
        case 'w': {
            movement[CAMERA_MOVE_FORWARD] = true;
            break;
        }
        case 's': {
            movement[CAMERA_MOVE_BACKWARD] = true;
            break;
        }
        case 'a': {
            movement[CAMERA_STRAFE_LEFT] = true;
            break;
        }
        case 'd': {
            movement[CAMERA_STRAFE_RIGHT] = true;
            break;
        }
        case 'q': {
            exit(0);
            break;
        }
    }
}

/**
* Handles keyboard key releases
*/
void handleKeyboardUp(unsigned char key, int _x, int _y)
{
    switch(key) {
        case 'w': {
            movement[CAMERA_MOVE_FORWARD] = false;
            break;
        }
        case 's': {
            movement[CAMERA_MOVE_BACKWARD] = false;
            break;
        }
        case 'a': {
            movement[CAMERA_STRAFE_LEFT] = false;
            break;
        }
        case 'd': {
            movement[CAMERA_STRAFE_RIGHT] = false;
            break;
        }
    }
}

/**
* Handles special key inputs (i.e. arrow keys).
*/
void special(int key, int x, int y) {
    switch(key) {
    }
}

/**
* Handles special keys (i.e. arrow keys) being released
*/
void specialUp(int key, int x, int y) {
    switch(key) {
    }
}

/**
* Renders a 2D HUD over the top of the 3D scene.
*/
void drawHUD() {
    glDisable(GL_LIGHTING);
    // reset matrices so the 3d view is not affecting
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // setup 2d ortho perspective
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // build a string to display
    std::stringstream stream;
    stream << "(" << camera.camPos.mX << "," << camera.camPos.mY << "," << camera.camPos.mZ << ")" << std::endl;
    stream << "angles: " << camera.pitch << "," << camera.yaw << std::endl;
    stream << "timer: " << (timer / 60.0) << std::endl;

    std::string output = stream.str();

    // color and position
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glRasterPos2f(-1, 0.9);
    // write string to screen
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(output.c_str()));
    glEnable(GL_LIGHTING);
}

// handle mouse
void mouse(int button, int state, int x, int y) {
}

/**
* Keeps track of mouse motion and updates pitch/yaw accordingly.
*/
void motion(int x, int y)
{
    float xoff = x - ((float)screen_width / 2);
    float yoff = y - ((float)screen_height / 2);

    camera.updateRotation(xoff, yoff);

    glutWarpPointer(((int)screen_width / 2), ((int)screen_height / 2));
}

void drawWalls() {
    glBegin (GL_QUADS);

    glEnd();
}

/**
* Display function
*/
void display()
{
    // set up camera perspective and point it at the looking point
    camera.setupPerspective();
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.lookAt();

    // draw a light/bind its properties to light the scene
    l.render();

    // draw a mesh
    for (size_t i = 0; i < objs.size(); i++) {
        objs[i].render();
    }

    // draw a 2d HUD
    drawHUD();

    // swap buffers
    glutSwapBuffers();
}

/**
* FPS timing function to lock program to around 60fps
*/
void FPS(int val)
{
    // applies rotations
    camera.applyRotation();
    // apply movement for each of the input keys
    for (int i = 0; i < 4; i++) {
        if (movement[i]) {
            camera.applyMovement(i, 0.5);
        }
    }

    timer--;

    glutPostRedisplay();
    glutTimerFunc(17, FPS, val);
}

void loadScenes() {
}

// utility function from stackoverflow, checks if a string ends with another string.
bool endswith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// loads all meshes from the .obj files in models/
// and stores them into a map (key = the name of the obj file)
void loadModels() {
    // open a directory for iterating over files
    tinydir_dir dir;
    tinydir_open(&dir, "models/");

    // loop through all files
    while (dir.has_next) {
        // get next file
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        // we only want to look at files with .obj extension
        std::string s = file.name;
        if (endswith(s, ".obj")) {
            // load the model with tinyobj
            std::string inputfile = "models/" + s;
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;

            std::string warn;
            std::string err;

            // we need to provide basepath so that it can load the .mtl properly
            std::string basepath = "models/";

            bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), basepath.c_str());

            // error checking, should be done differently probably
            if (!err.empty()) {
                std::cerr << err << std::endl;
            }

            if (!ret) {
                exit(1);
            }

            // store the loaded model into a Mesh class
            meshes.insert(std::pair<std::string, Mesh*>(s, new Mesh(attrib, shapes, materials)));
        }

        // get next file in the directory
        tinydir_next(&dir);
    }

    // free memory
    tinydir_close(&dir);
}

int main(int argc, char** argv)
{
    // we want to display a menu before showing a scene so that user can select the scene to play
    // just show a list of options and press a key to select - PROTOTYPE

    // guess number or timer (done) or both needs to be added
    // guess number - not until we have a way to check what they're guessing

    srand(time(NULL));
    // load models - do this before creating window
    // so it doesn't appear frozen
    loadModels();

    loadScenes();

    // this is just done to display an object, we should be instead loading a scene.
    objs.push_back(GameObject(meshes["bed.obj"], Point3D(5, 0, 5), Point3D(0, -90, 0), 1.0, false));
    objs.push_back(GameObject(meshes["desk.obj"], Point3D(5, 0, -5), Point3D(0, 0, 0), 1.0, false));
    objs.push_back(GameObject(meshes["computer.obj"], Point3D(5, 1, -5.18), Point3D(0, -90, 0), 25.0, false));
    objs.push_back(GameObject(meshes["tower.obj"], Point3D(5.1, 0.83, -5.55), Point3D(0, -90, 0), 0.0075, false));

    // randomize 1 ID for what object should be the random item that is the goal - PROTOTYPE
    // objects take random position in the scene. - PROTOTYPE
    // objects can take random size, similar objects with different sizes. - PROTOTYPE

    // allow objects to be duplicated some number of times in the scene when it's generated - PROTOTYPE

    // provide user with a hint if they pick the wrong object. make comparison with name, size, etc. of the target

    // glut initialization stuff
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("3GC3 Final Project");

    // disable cursor (seems not to work on unix systems)
    glutSetCursor(GLUT_CURSOR_NONE);

    // set screen clear color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // light properties
    float pos[4] = {-100, 100, -30, 1};

    float amb[4] = {0.3, 0.3, 0.3, 1.0};
    float diff[4] = {0.7, 0.7, 0.7, 1.0};
    float spec[4] = {1.0, 1.0, 1.0, 1.0};

    l = Light(GL_LIGHT0, pos, amb, diff, spec, false);

    glEnable(GL_LIGHTING);

    // enable smooth shading
    glShadeModel(GL_SMOOTH);

    // depth test/face culling
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // callbacks
    glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutDisplayFunc(display);
    glutTimerFunc(0, FPS, 0);

    // kick off main loop
    glutMainLoop();

    return 0;
}