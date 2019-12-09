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

#include "include/soil/SOIL.h"

#include "mathLib3D.h"
#include "camera.h"
#include "light.h"
#include "game_object.h"
#include "scene.h"

#include "mesh.h"
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <map>
#include <iterator>

// the two Vec3D represent the eye position and the lookAt position
Camera camera = Camera(Vec3D(0.0, 2.0, 0.0), Vec3D(1.0, 2.0, 0.0));

// width and height of the screen.
int screen_width = 600;
int screen_height = 600;

// timer duration in ms = 30s
int timer = 1000 * 60;

// base time
std::chrono::high_resolution_clock::time_point tb = std::chrono::high_resolution_clock::now();

// movement inputs
bool movement[] = {false, false, false, false};

// map of obj file names to Mesh pointers
// the Mesh ptr should be passed through to the actual game object
// so it can render itself
std::map<std::string, Mesh*> meshes;
// a map of all available scenes
std::map<std::string, Scene> scenes;

std::vector<int> goals;

// these are the bounds for the walls/floor (minx, maxx, miny, maxy, minz, maxz)
float sceneBounds[] = {-6, 6, -1, 5, -6, 6, -4, -18};
// material used by the walls/floor (should use a texture instead?)
Material matBounds;

// current scene to support multiple scenes
std::string current_scene = "bedroom";  

// show instructions?
bool instructions = true;

// text
std::string text_instr = "Use the mouse to move the camera\nUse W/A/S/D to move\nHints for objects to look for will appear on the left\nClick on objects to find them within the time limit!\n\nClick anywhere to begin.";

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
        case ' ': {
            if (scenes[current_scene].objs[0].grav == 0) scenes[current_scene].objs[0].grav = -0.1;
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
    // disable lighting so the text always stays full colored
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
    stream << "timer: " << (timer / (1000.0)) << "s" << std::endl << std::endl;

    stream << "Hints" << std::endl;
    for (size_t i = 0; i < goals.size(); i++) {
        stream << scenes[current_scene].objs[goals[i]].desc << std::endl;
    }

    std::string output = stream.str();

    // color and position
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glRasterPos2f(-1, 0.9);
    // write string to screen
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(output.c_str()));

    // render a small crosshair to indicate camera position
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glBegin(GL_LINES);
        glVertex3f(-0.02, -0.02, 1.0);
        glVertex3f(0.02, 0.02, 1.0);
        glVertex3f(-0.02, 0.02, 1.0);
        glVertex3f(0.02, -0.02, 1.0);
    glEnd();

    // render a win message?
    if (goals.size() == 0 && timer > 0) {
        std::string winner = "You Win!!!";
        // color and position
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glRasterPos2f(-0.2, 0);
        // write string to screen
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(winner.c_str()));
    } else if (timer == 0) {
        std::string winner = "You Lose :(";
        // color and position
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glRasterPos2f(-0.2, 0);
        // write string to screen
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(winner.c_str()));
    }

    // re-enable lighting for the next render
    glEnable(GL_LIGHTING);
}

// handle mouse
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (instructions) {
            tb = std::chrono::high_resolution_clock::now();
            instructions = false;
            return;
        }
        for (size_t i = 0; i < scenes[current_scene].objs.size(); i++) {
            if (scenes[current_scene].objs[i].intersects) {
                // remove from the vector if it exists
                goals.erase(std::remove(goals.begin(), goals.end(), i), goals.end());
            }
        }
    }
}

/**
* Keeps track of mouse motion and updates pitch/yaw accordingly.
*/
void motion(int x, int y)
{
    // compute how far the mouse has moved relative to the center
    float xoff = x - ((float)screen_width / 2);
    float yoff = y - ((float)screen_height / 2);

    // update the camera rotation based on the mouse movement
    camera.updateRotation(xoff, yoff);

    // move mouse cursor back to center
    glutWarpPointer(((int)screen_width / 2), ((int)screen_height / 2));
}

// draws walls/floor/ceiling to define the bounds of the scene.
void drawWalls() {
    // bind the material for rendering the walls
    matBounds.bind();
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[4]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[3], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[5]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[6]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[6]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[5]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(sceneBounds[1], sceneBounds[3], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[5]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[1], sceneBounds[1], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[1], sceneBounds[4]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(sceneBounds[0], sceneBounds[1], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[1], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[5]);
    glEnd();


//second room
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[1], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[4]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(sceneBounds[7], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[5]);
        glVertex3f(sceneBounds[7], sceneBounds[3], sceneBounds[5]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[7], sceneBounds[3], sceneBounds[4]);
        glVertex3f(sceneBounds[7], sceneBounds[3], sceneBounds[5]);
    glEnd();
    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[3], sceneBounds[6]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[6]);
    glEnd();


    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[4]);
        glVertex3f(sceneBounds[0], sceneBounds[1], sceneBounds[4]);
        glVertex3f(sceneBounds[7], sceneBounds[1], sceneBounds[4]);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(sceneBounds[7], sceneBounds[1], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[1], sceneBounds[5]);
        glVertex3f(sceneBounds[0], sceneBounds[2], sceneBounds[5]);
        glVertex3f(sceneBounds[7], sceneBounds[2], sceneBounds[5]);
    glEnd();
}

/**
* Display function
*/
void display()
{
    if (!instructions) {
        // set up camera perspective and point it at the looking point
        camera.setupPerspective();
        // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.lookAt();

        // draws the bounds of the scene
        drawWalls();

        // render the scene, this includes all objects and lights making up the scene
        scenes[current_scene].render();

        // draw a 2d HUD
        drawHUD();
    } else {
        // disable lighting so the text always stays full colored
        glDisable(GL_LIGHTING);
        // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // reset matrices so the 3d view is not affecting
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // setup 2d ortho perspective
        gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        // color and position
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glRasterPos2f(-1, 0.9);
        // write string to screen
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(text_instr.c_str()));
    }

    // swap buffers
    glutSwapBuffers();
}

void checkIntersections() {
    // cast a ray in the direction the camera is pointing.
    // the vector camFront represents direction the camera is looking in
    Vec3D camDir = Vec3D(camera.camFront.mX, camera.camFront.mY, camera.camFront.mZ).normalize();
    // camPos is a point on the line drawn by the camera
    Vec3D camPos = Vec3D(camera.camPos.mX, camera.camPos.mY, camera.camPos.mZ);

    // run through the objects in the scene and test if this line intersects with them
    // check collision with any game objects
    size_t idx = -1;
    for (size_t i = 0; i < scenes[current_scene].objs.size(); i++) {
        float *t = new float;
        float min_t = 100000000;
        if (!scenes[current_scene].objs[i].norender & scenes[current_scene].objs[i].line_intersects(camDir, camPos, t)) {
            if (*t < min_t) {
                min_t = *t;
                idx = i;
            }
        }
        scenes[current_scene].objs[i].intersects = false;
    }

    if (idx != -1)
        scenes[current_scene].objs[idx].intersects = true;
}

/**
* FPS timing function to lock program to around 60fps
*/
void FPS(int val)
{
    if (!instructions) {
        // applies rotations
        camera.applyRotation();
        // save the camera position
        Point3D oldpos = Point3D(camera.camPos.mX, camera.camPos.mY, camera.camPos.mZ);
        // apply movement for each of the input keys
        for (int i = 0; i < 4; i++) {
            if (movement[i]) {
                camera.applyMovement(i, 0.1);
            }
        }
        scenes[current_scene].objs[0].position = Point3D(camera.camPos.mX, camera.camPos.mY, camera.camPos.mZ);
        // check collision with any game objects
        for (size_t i = 0; i < scenes[current_scene].objs.size(); i++) {
            // logic step for each object making up the scene,
            // this will e.g. update their y position based on gravtiy
            scenes[current_scene].objs[i].logic(scenes[current_scene].objs, i);
        }

        for (size_t i = 1; i < scenes[current_scene].objs.size(); i++) {
            if (scenes[current_scene].objs[i].check_collision(scenes[current_scene].objs[0])) {
                scenes[current_scene].objs[0].position = Point3D(oldpos.mX, oldpos.mY, oldpos.mZ);
                break;
            }
        }

        camera.camPos = Vec3D(scenes[current_scene].objs[0].position.mX, scenes[current_scene].objs[0].position.mY, scenes[current_scene].objs[0].position.mZ);

        checkIntersections();

        if (timer > 0 && goals.size() != 0) {
            // get current time
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            // sub from old frame time
            std::chrono::duration<double, std::milli> time_span = t1 - tb;
            // sub from timer
            timer -= time_span.count();
            // set tb
            tb = std::chrono::high_resolution_clock::now();
        } else if (timer <= 0) {
            timer = 0;
        }
    }

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

void setbound(std::vector<GameObject> *objs){

    GameObject floor = GameObject(Point3D(0, 0, 0), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    floor.bounds[0] = -18;
    floor.bounds[1] = -1;
    floor.bounds[2] = -6;
    floor.bounds[3] = 6;
    floor.bounds[4] = -0.99;
    floor.bounds[5] = 6;
    objs->push_back(floor);
    GameObject wall1 = GameObject(Point3D(0, 2, 6), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    wall1.bounds[0] = -18;
    wall1.bounds[1] = -3;
    wall1.bounds[2] = 0.002;
    wall1.bounds[3] = 6;
    wall1.bounds[4] = 3;
    wall1.bounds[5] = 0.001;
    objs->push_back(wall1);

    GameObject wall2= GameObject(Point3D(6,2,0), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    wall2.bounds[0] = 0.002;
    wall2.bounds[1] = -3;
    wall2.bounds[2] = -6;
    wall2.bounds[3] = 0.001;
    wall2.bounds[4] = 3;
    wall2.bounds[5] = 6;
    objs->push_back(wall2);
    GameObject wall3 = GameObject(Point3D(0, 2, -6), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    wall3.bounds[0] = -18;
    wall3.bounds[1] = -3;
    wall3.bounds[2] = 0.002;
    wall3.bounds[3] = 6;
    wall3.bounds[4] = 3;
    wall3.bounds[5] = 0.001;
    objs->push_back(wall3);
    GameObject wall4 = GameObject(Point3D(-6, 2, 1), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    wall4.bounds[0] = 0.002;
    wall4.bounds[1] = -3;
    wall4.bounds[2] = -5;
    wall4.bounds[3] = 0.001;
    wall4.bounds[4] = 3;
    wall4.bounds[5] = 5;
    objs->push_back(wall4);
    GameObject wall5 = GameObject(Point3D(-18, 2, 0), Point3D(0, 0, 0), 1.0, false, false, 0, "", "");
    wall5.bounds[0] = 0.002;
    wall5.bounds[1] = -3;
    wall5.bounds[2] = -5;
    wall5.bounds[3] = 0.001;
    wall5.bounds[4] = 3;
    wall5.bounds[5] = 5;
    objs->push_back(wall5);
}

// reads all the scene files from the scenes/ directory
// and stores them into a vector
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
            // list of lights in the scene
            std::vector<Light> lights;

            // construct an object to represent the camera with.
            // camera will always have index 0 in objs.
            GameObject cam = GameObject(Point3D(0, 2, 0), Point3D(0, 0, 0), 1.0, false, true, 0, "camera", "");
            cam.bounds[0] = -0.3;
            cam.bounds[1] = -2.0;
            cam.bounds[2] = -0.3;
            cam.bounds[3] = 0.3;
            cam.bounds[4] = 2.0;
            cam.bounds[5] = 0.3;
            objs.insert(objs.begin(), cam);

            setbound(&objs);

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
                    // lots of components here:
                    // ln = light number (0 = GL_LIGHT0, 1 = GL_LIGHT1, ...)
                    // x, y, z = position
                    // ax, ay, az = ambient
                    // dx, dy, dz = diffuse
                    // sx, sy, sz = specular
                    float ln, x, y, z, ax, ay, az, dx, dy, dz, sx, sy, sz;
                    iss >> ln >> x >> y >> z >> ax >> ay >> az >> dx >> dy >> dz >> sx >> sy >> sz;
                    float pos[4] = {x, y, z, 1};

                    float amb[4] = {ax, ay, az, 1.0};
                    float diff[4] = {dx, dy, dz, 1.0};
                    float spec[4] = {sx, sy, sz, 1.0};

                    // construct light object
                    Light nl = Light(GL_LIGHT0 + ln, pos, amb, diff, spec, false);

                    // add it to list of lights
                    lights.push_back(nl);
                } else {
                    iss >> second_token;
                    // check the second token
                    if (second_token == "fixed") {
                        // x, y, z = pos
                        // xr, yr, zr = rotation
                        float x, y, z;
                        float xr, yr, zr;
                        float scale;
                        iss >> x >> y >> z >> xr >> yr >> zr >> scale;
                        Point3D pos = Point3D(x, y, z);
                        Point3D rot = Point3D(xr, yr, zr);
                        // load texture
                        std::string fname = first_token;
                        fname.replace(fname.end()-4, fname.end(), "");
                        fname = "models/" + fname + ".png";
                        GLuint texture = SOIL_load_OGL_texture
                        (
                            fname.c_str(),
                            SOIL_LOAD_AUTO,
                            SOIL_CREATE_NEW_ID,
                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                        );
                        // construct GameObject, with the mesh pointer passed from meshes map
                        GameObject nobj = GameObject(meshes[first_token], pos, rot, scale, false, texture, first_token, "");
                        objs.push_back(nobj);
                    } else if (second_token == "random") {
                        // base scale of the object, it gets randomized a little bit too
                        float base_scale;
                        iss >> base_scale;
                        int num_words;
                        iss >> num_words;
                        // grab all words from the strings
                        std::string desc = "";
                        std::string word;
                        for (int i = 0; i < num_words; i++) {
                            iss >> word;
                            desc = desc + " " + word;
                        }
                        // bc the spawn position is random, the objects might have been spawned within another object.
                        // if this is the case we want to re-generate a position.
                        // so we need a while loop to keep producing new random positions, until no collision occurs
                        bool has_collided = true;
                        // object we will insert once calculated
                        GameObject nobj;
                        // load texture
                        std::string fname = first_token;
                        fname.replace(fname.end()-4, fname.end(), "");
                        fname = "models/" + fname + ".png";
                        GLuint texture = SOIL_load_OGL_texture
                        (
                            fname.c_str(),
                            SOIL_LOAD_AUTO,
                            SOIL_CREATE_NEW_ID,
                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                        );
                        /* check for an error during the load process */
                        if( 0 == texture )
                        {
                            printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
                        }
                        // loop
                        while (has_collided) {
                            // set flag to false to break the loop if no collision found
                            has_collided = false;
                            // x position can be anything between -18 and 6
                            // z position can be anything between -6 and 6
                            float randX = sceneBounds[7] + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(sceneBounds[1]-sceneBounds[7])));
                            float randZ = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10))) - 5;
                            float randScale = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) * 5) + 2.5;
                            // build the gameobject
                            nobj = GameObject(meshes[first_token], Point3D(randX, 2, randZ), Point3D(0, 0, 0), base_scale * randScale, true, texture, first_token, desc);
                            // check collision with all other gameobjects
                            for (size_t i = 0; i < objs.size(); i++) {
                                if (nobj.check_collision(objs[i])) has_collided = true;
                            }
                        }
                        objs.push_back(nobj);
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

// adds some objects into a list as goals to find
void loadGoals() {
    for (size_t i = 0; i < scenes[current_scene].objs.size(); i++) {
        float randn = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)));
        if (scenes[current_scene].objs[i].random && (randn < 0.1) && std::find(goals.begin(), goals.end(), i) == goals.end()) {
            goals.push_back(i);
        } 
    }
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

    // randomize 1 ID for what object should be the random item that is the goal - PROTOTYPE

    // provide user with a hint if they pick the wrong object. make comparison with name, size, etc. of the target - to be done for final

    // glut initialization stuff
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("3GC3 Final Project");

    // loads all the scenes into a map
    // ideally this would be before the window creation but we need a gl context to load textures :(
    loadScenes();

    // select objects in the scene to make up the goal list
    while(goals.size() < 4) loadGoals();

    // disable cursor (seems not to work on unix systems)
    glutSetCursor(GLUT_CURSOR_NONE);

    // material for floor/walls
    float amb[4] = {0.3, 0.3, 0.3, 1.0};
    float diff[4] = {0.7, 0.7, 0.7, 1.0};
    float spec[4] = {1.0, 1.0, 1.0, 1.0};
    float emm[4] = {0.0, 0.0, 0.0, 0.0};
    float shin = 100;

    matBounds = Material(amb, diff, spec, emm, shin);

    // set screen clear color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_LIGHTING);

    // enable smooth shading
    glShadeModel(GL_SMOOTH);

    // depth test/face culling
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);

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
    tb = std::chrono::high_resolution_clock::now();
    glutMainLoop();

    return 0;
}