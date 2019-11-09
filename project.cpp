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
#include "scene.h"

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
// a map of all available scenes
std::map<std::string, Scene> scenes;

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

    // render the scene, this includes all objects and lights making up the scene
    scenes["bedroom"].render();

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

// utility function from stackoverflow, checks if a string ends with another string.
bool endswith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void loadScenes() {
    // open a directory for iterating over files
    tinydir_dir dir;
    tinydir_open(&dir, "scenes/");

    // loop through all files
    while (dir.has_next) {
        // get next file
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        // we only want to look at files with .txt extension
        if (endswith(file.name, ".txt")) {
            // open the file for reading
            std::string in = file.name;
            std::ifstream infile("scenes/" + in);
            // get the name of the scene from the file name
            in.replace(in.end()-4, in.end(), "");
            // represents current line
            std::string line;

            // the game objects comprising the scene
            std::vector<GameObject> objs;
            std::vector<Light> lights;

            // loop over each line in the file
            while(std::getline(infile, line)) {
                // used for tokenizing
                std::istringstream iss(line);
                // grab first token (contains the name of the mesh)
                // and second token (=either random or fixed)
                std::string first_token, second_token;
                iss >> first_token;
                // check if the first token == "light" for light placement
                if (first_token == "light") {
                    float ln, x, y, z, ax, ay, az, dx, dy, dz, sx, sy, sz;
                    iss >> ln >> x >> y >> z >> ax >> ay >> az >> dx >> dy >> dz >> sx >> sy >> sz;
                    float pos[4] = {x, y, z, 1};

                    float amb[4] = {ax, ay, az, 1.0};
                    float diff[4] = {dx, dy, dz, 1.0};
                    float spec[4] = {sx, sy, sz, 1.0};

                    Light nl = Light(GL_LIGHT0 + ln, pos, amb, diff, spec, false);

                    lights.push_back(nl);
                } else {
                    iss >> second_token;
                    // check the second token
                    if (second_token == "fixed") {
                        float x, y, z;
                        float xr, yr, zr;
                        float scale;
                        iss >> x >> y >> z >> xr >> yr >> zr >> scale;
                        Point3D pos = Point3D(x, y, z);
                        Point3D rot = Point3D(xr, yr, zr);
                        GameObject nobj = GameObject(meshes[first_token], pos, rot, scale, false);
                        objs.push_back(nobj);
                    } else if (second_token == "random") {
                        float base_scale;
                        int max_count;
                        iss >> base_scale >> max_count;
                        int count = rand() % max_count;
                        for (int i = 0; i < count; i++) {
                            // generate random x, z position in (-5, 5)
                            float randX = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10))) - 5;
                            float randZ = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10))) - 5;
                            float randScale = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) + 0.5;
                            GameObject nobj = GameObject(meshes[first_token], Point3D(randX, 0, randZ), Point3D(0, 0, 0), base_scale * randScale, true);
                            objs.push_back(nobj);
                        }
                    }
                }
            }

            // construct the scene
            Scene s = Scene(objs, lights);
            scenes.insert(std::pair<std::string, Scene>(in, s));
        }

        // get next file in the directory
        tinydir_next(&dir);
    }

    // free memory
    tinydir_close(&dir);
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

    // loads all the scenes into a map
    loadScenes();

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