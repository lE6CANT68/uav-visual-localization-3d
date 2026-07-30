#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <GL/freeglut.h>
#include <windows.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace cv;

// --- VARIABLES POUR LA CAMERA ORBITALE ---
float camX = 0.0f, camY = 0.0f;
float camDist = 3.0f;
float rotX = 45.0f;
float rotZ = 0.0f;
int lastMouseX = 0, lastMouseY = 0;
int currentMouseX = 0, currentMouseY = 0;

// --- VARIABLES POUR LA FREECAM ---
bool isFreecam = false;
float freeCamX = 0.0f, freeCamY = 0.0f, freeCamZ = 3.0f;
float freePitch = 45.0f, freeYaw = 0.0f;
bool keys[256] = {false};
int windowWidth = 1024, windowHeight = 768;

// --- TEXTURES ET SÉLECTION ---
GLuint satTextureID;
GLuint currentDroneTextureID = 0;
int droneSelectionneIndex = -1;
int droneHoverIndex = -1;
bool clicSourieDetecte = false;

// --- OPACITÉ ET CALIBRATION ---
float droneOpacity = 1.0f;
float globalYawOffset = -120.0f;

// --- VARIABLES POUR L'ANIMATION ---
bool isAnimating = false;
float animProgress = 0.0f;
float rotY = 0.0f;
float startCamX, startCamY, startCamDist, startRotX, startRotY, startRotZ;
float targetCamX, targetCamY, targetCamDist, targetRotX, targetRotY, targetRotZ;

const float map_LT_lat = 32.355491f;
const float map_LT_lon = 119.805926f;
const float map_RB_lat = 32.290290f;
const float map_RB_lon = 119.900052f;

struct Drone {
    string filename;
    float lat, lon, height;
    float glX, glY, glZ;
    float omega, kappa, phi;
};
vector<Drone> listeDrones;

string formatFloat(float val, int precision = 2) {
    stringstream stream;
    stream.precision(precision);
    stream << fixed << val;
    return stream.str();
}

void loadDrones(string csvPath) {
    ifstream file(csvPath);
    if (!file.is_open()) return;
    string line;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        Drone d;
        getline(ss, item, ','); getline(ss, d.filename, ','); getline(ss, item, ',');
        getline(ss, item, ','); d.lat = stof(item);
        getline(ss, item, ','); d.lon = stof(item);
        getline(ss, item, ','); d.height = stof(item);
        getline(ss, item, ','); d.omega = stof(item);
        getline(ss, item, ','); d.kappa = stof(item);
        getline(ss, item, ','); d.phi = stof(item);

        float percentX = (d.lon - map_LT_lon) / (map_RB_lon - map_LT_lon);
        d.glX = -1.0f + percentX * 2.0f;
        float percentY = (map_LT_lat - d.lat) / (map_LT_lat - map_RB_lat);
        d.glY = 1.0f - percentY * 2.0f;
        d.glZ = d.height * 0.000225f;

        listeDrones.push_back(d);
    }
    file.close();
}

void loadSatelliteTexture() {
    string path = "UAV-VisLoc/03/satellite03.tif";
    Mat img = imread(path, IMREAD_COLOR);
    if (img.empty()) return;

    Mat img_resized;
    float scale = 2048.0f / img.cols;
    resize(img, img_resized, Size(), scale, scale);
    cvtColor(img_resized, img_resized, COLOR_BGR2RGB);
    flip(img_resized, img_resized, 0);

    glGenTextures(1, &satTextureID);
    glBindTexture(GL_TEXTURE_2D, satTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_resized.cols, img_resized.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img_resized.ptr());
}

void loadDroneTexture(int index) {
    if (currentDroneTextureID != 0) {
        glDeleteTextures(1, &currentDroneTextureID);
        currentDroneTextureID = 0;
    }
    Drone d = listeDrones[index];
    string path = "UAV-VisLoc/03/drone/" + d.filename;
    Mat img = imread(path, IMREAD_COLOR);

    if (!img.empty()) {
        Mat img_resized;
        float scale = 1024.0f / max(img.cols, img.rows);
        if(scale < 1.0f) resize(img, img_resized, Size(), scale, scale);
        else img_resized = img;

        cvtColor(img_resized, img_resized, COLOR_BGR2RGB);
        flip(img_resized, img_resized, 0);

        glGenTextures(1, &currentDroneTextureID);
        glBindTexture(GL_TEXTURE_2D, currentDroneTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_resized.cols, img_resized.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img_resized.ptr());
    }
}

void applyCamera() {
    if (isFreecam) {
        glRotatef(freePitch, 1.0f, 0.0f, 0.0f);
        glRotatef(freeYaw, 0.0f, 0.0f, 1.0f);
        glTranslatef(-freeCamX, -freeCamY, -freeCamZ);
    } else {
        glTranslatef(0.0f, 0.0f, -camDist);
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
        glTranslatef(-camX, -camY, 0.0f);
    }
}

int getHoveredDrone(int aimX, int aimY) {
    GLint viewport[4]; GLdouble modelview[16], projection[16];
    glGetIntegerv(GL_VIEWPORT, viewport); glGetDoublev(GL_PROJECTION_MATRIX, projection);

    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    applyCamera(); glGetDoublev(GL_MODELVIEW_MATRIX, modelview); glPopMatrix();

    int bestIndex = -1; double minScreenDist = 20.0;
    double screenY = viewport[3] - aimY;

    for (int i = 0; i < (int)listeDrones.size(); i++) {
        GLdouble winX, winY, winZ;
        gluProject(listeDrones[i].glX, listeDrones[i].glY, listeDrones[i].glZ, modelview, projection, viewport, &winX, &winY, &winZ);

        if (winZ > 0.0 && winZ < 1.0) {
            double dist = sqrt(pow(winX - aimX, 2) + pow(winY - screenY, 2));
            if (dist < minScreenDist) { minScreenDist = dist; bestIndex = i; }
        }
    }
    return bestIndex;
}

void renderText(float x, float y, const string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawHUD() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2f(10, 10);  glVertex2f(420, 10);
        glVertex2f(420, 170); glVertex2f(10, 170);
    glEnd();

    glColor3f(0.2f, 1.0f, 0.2f);

    float startY = 35;
    float ligneH = 25;

    renderText(20, startY, isFreecam ? "[ MODE: FREECAM ]" : "[ MODE: VUE ORBITALE ]");
    renderText(20, startY + ligneH, "Angle(O/P) : " + formatFloat(globalYawOffset, 1) + " deg");

    if (droneHoverIndex != -1) {
        Drone h = listeDrones[droneHoverIndex];
        renderText(20, startY + ligneH * 2, "Cible >> Lat: " + formatFloat(h.lat, 6) + " | Lon: " + formatFloat(h.lon, 6));
    } else {
        renderText(20, startY + ligneH * 2, "Cible >> ---");
    }

    if (droneSelectionneIndex != -1) {
        Drone s = listeDrones[droneSelectionneIndex];
        renderText(20, startY + ligneH * 3, "Fichier Actif : " + s.filename);
        renderText(20, startY + ligneH * 4, "Altitude Vol  : " + formatFloat(s.height, 2) + " m");

        if(isFreecam) {
            glColor3f(1.0f, 1.0f, 0.0f);
            renderText(280, startY + ligneH * 4, "Opacite: " + to_string((int)(droneOpacity * 100)) + "%");
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

void drawCrosshair() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);

    if (droneHoverIndex != -1) glColor3f(0.0f, 1.0f, 0.0f);
    else glColor3f(0.8f, 0.8f, 0.8f);

    glLineWidth(2.0f);
    int cx = windowWidth / 2, cy = windowHeight / 2, size = 12;

    glBegin(GL_LINES);
        glVertex2i(cx - size, cy); glVertex2i(cx + size, cy);
        glVertex2i(cx, cy - size); glVertex2i(cx, cy + size);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    applyCamera();
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, satTextureID); glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    glLineWidth(1.5f);
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < (int)listeDrones.size(); i++) {
        glVertex3f(listeDrones[i].glX, listeDrones[i].glY, listeDrones[i].glZ);
    }
    glEnd();
    for (int i = 0; i < (int)listeDrones.size(); i++) {
        if (i == droneSelectionneIndex && currentDroneTextureID != 0 && isFreecam) {
            glPushMatrix();
            glTranslatef(listeDrones[i].glX, listeDrones[i].glY, listeDrones[i].glZ);
            glRotatef(-listeDrones[i].phi + globalYawOffset, 0.0f, 0.0f, 1.0f);
            glRotatef(listeDrones[i].kappa, 1.0f, 0.0f, 0.0f);
            glRotatef(listeDrones[i].omega, 0.0f, 1.0f, 0.0f);

            float sizeX = 0.08f, sizeY = 0.053f;
            float focalDist = -0.06f;
            glDisable(GL_TEXTURE_2D);
            glLineWidth(2.0f);
            glColor3f(1.0f, 0.8f, 0.0f);
            glBegin(GL_LINES);
                glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(-sizeX, -sizeY, focalDist);
                glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(sizeX, -sizeY, focalDist);
                glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(sizeX, sizeY, focalDist);
                glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(-sizeX, sizeY, focalDist);

                glVertex3f(-sizeX, -sizeY, focalDist); glVertex3f(sizeX, -sizeY, focalDist);
                glVertex3f(sizeX, -sizeY, focalDist);  glVertex3f(sizeX, sizeY, focalDist);
                glVertex3f(sizeX, sizeY, focalDist);   glVertex3f(-sizeX, sizeY, focalDist);
                glVertex3f(-sizeX, sizeY, focalDist);  glVertex3f(-sizeX, -sizeY, focalDist);
            glEnd();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, currentDroneTextureID);
            glColor4f(1.0f, 1.0f, 1.0f, droneOpacity);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeX, -sizeY, focalDist);
                glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeX, -sizeY, focalDist);
                glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeX,  sizeY, focalDist);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeX,  sizeY, focalDist);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            glPopMatrix();

        } else if (i == droneHoverIndex || i == droneSelectionneIndex) {
            glPointSize(8.0f); glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_POINTS); glVertex3f(listeDrones[i].glX, listeDrones[i].glY, listeDrones[i].glZ); glEnd();
        } else {
            glPointSize(4.0f); glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POINTS); glVertex3f(listeDrones[i].glX, listeDrones[i].glY, listeDrones[i].glZ); glEnd();
        }
    }

    if (isFreecam) drawCrosshair();
    drawHUD();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w; windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.001, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == '1') {
        isFreecam = !isFreecam;
        if (isFreecam) {
            freeCamX = camX; freeCamY = camY; freeCamZ = camDist;
            freePitch = rotX; freeYaw = rotZ;
            glutSetCursor(GLUT_CURSOR_NONE);
            glutWarpPointer(windowWidth / 2, windowHeight / 2);
            cv::destroyAllWindows();
        } else {
            glutSetCursor(GLUT_CURSOR_INHERIT);
            cv::destroyAllWindows();
        }
    }

    if (key == 'o' || key == 'O') globalYawOffset -= 2.0f;
    if (key == 'p' || key == 'P') globalYawOffset += 2.0f;

    if (!isFreecam) {
        if (key == '+') camDist -= 0.2f;
        if (key == '-') camDist += 0.2f;
    }
    if (key == 27) exit(0);
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) { keys[key] = false; }

void gameLoopTimer(int value) {
    if (isFreecam) {
        float speed = 0.015f;
        float radYaw = -freeYaw * M_PI / 180.0f;
        float dirX = sin(radYaw), dirY = cos(radYaw), rightX = cos(radYaw), rightY = -sin(radYaw);

        if (keys['z'] || keys['Z']) { freeCamX -= dirX * speed; freeCamY += dirY * speed; }
        if (keys['s'] || keys['S']) { freeCamX += dirX * speed; freeCamY -= dirY * speed; }
        if (keys['q'] || keys['Q']) { freeCamX -= rightX * speed; freeCamY += rightY * speed; }
        if (keys['d'] || keys['D']) { freeCamX += rightX * speed; freeCamY -= rightY * speed; }

        if (keys[' ']) freeCamZ += speed;
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) freeCamZ -= speed;

        droneHoverIndex = getHoveredDrone(windowWidth / 2, windowHeight / 2);
        glutPostRedisplay();
    }

    if (isAnimating && !isFreecam) {
        animProgress += 0.02f;
        if (animProgress >= 1.0f) { animProgress = 1.0f; isAnimating = false; }
        float t = animProgress * animProgress * (3.0f - 2.0f * animProgress);
        camX = startCamX + t * (targetCamX - startCamX);
        camY = startCamY + t * (targetCamY - startCamY);
        camDist = startCamDist + t * (targetCamDist - startCamDist);
        rotX = startRotX + t * (targetRotX - startRotX);
        rotY = startRotY + t * (targetRotY - startRotY);
        rotZ = startRotZ + t * (targetRotZ - startRotZ);
        glutPostRedisplay();
    }

    glutTimerFunc(16, gameLoopTimer, 0);
}

void passiveMotion(int x, int y) {
    currentMouseX = x; currentMouseY = y;

    if (isFreecam) {
        if (x == windowWidth / 2 && y == windowHeight / 2) return;
        float sensitivity = 0.2f;
        freeYaw += (x - windowWidth / 2) * sensitivity;
        freePitch += (y - windowHeight / 2) * sensitivity;

        if (freePitch > 89.0f) freePitch = 89.0f;
        if (freePitch < -89.0f) freePitch = -89.0f;

        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    } else {
        droneHoverIndex = getHoveredDrone(x, y);
    }
    glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        droneOpacity += 0.05f;
        if (droneOpacity > 1.0f) droneOpacity = 1.0f;
    } else {
        droneOpacity -= 0.05f;
        if (droneOpacity < 0.0f) droneOpacity = 0.0f;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == 3 && state == GLUT_DOWN) { mouseWheel(0, 1, x, y); return; }
    if (button == 4 && state == GLUT_DOWN) { mouseWheel(0, -1, x, y); return; }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (isAnimating && !isFreecam) return;

        clicSourieDetecte = true;
        lastMouseX = x; lastMouseY = y;

        int index_trouve = isFreecam ? getHoveredDrone(windowWidth / 2, windowHeight / 2) : getHoveredDrone(x, y);

        if (index_trouve != -1) {
            if (droneSelectionneIndex != index_trouve) {
                loadDroneTexture(index_trouve);
                droneOpacity = 1.0f;
            }
            droneSelectionneIndex = index_trouve;
            Drone d = listeDrones[index_trouve];

            if (!isFreecam) {
                startCamX = camX; startCamY = camY; startCamDist = camDist;
                startRotX = rotX; startRotY = rotY; startRotZ = rotZ;
                targetCamX = d.glX; targetCamY = d.glY; targetCamDist = d.glZ;
                targetRotX = d.omega; targetRotY = d.kappa;
                targetRotZ = -d.phi + globalYawOffset;

                isAnimating = true; animProgress = 0.0f;

                string path_image_drone = "UAV-VisLoc/03/drone/" + d.filename;
                Mat img_drone = imread(path_image_drone, IMREAD_COLOR);
                if(!img_drone.empty()) {
                    Mat img_display;
                    resize(img_drone, img_display, Size(), 0.15, 0.15);
                    namedWindow("Vue Drone - Orbital", WINDOW_AUTOSIZE);
                    imshow("Vue Drone - Orbital", img_display);
                }
            } else {
                cv::destroyAllWindows();
            }
        } else if (droneSelectionneIndex != -1) {
            if (!isFreecam) {
                startCamX = camX; startCamY = camY; startCamDist = camDist;
                startRotX = rotX; startRotY = rotY; startRotZ = rotZ;
                targetCamX = 0.0f; targetCamY = 0.0f; targetCamDist = 3.0f;
                targetRotX = 45.0f; targetRotY = 0.0f; targetRotZ = 0.0f;
                isAnimating = true; animProgress = 0.0f;
                cv::destroyAllWindows();
            }
            if (currentDroneTextureID != 0) { glDeleteTextures(1, &currentDroneTextureID); currentDroneTextureID = 0; }
            droneSelectionneIndex = -1;
        }
        glutPostRedisplay();
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        clicSourieDetecte = false;
    }
}

void motion(int x, int y) {
    currentMouseX = x; currentMouseY = y;
    if (clicSourieDetecte && droneSelectionneIndex == -1 && !isFreecam) {
        rotZ += (x - lastMouseX) * 0.5f;
        rotX += (y - lastMouseY) * 0.5f;
        lastMouseX = x; lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Projet Drone");

    glEnable(GL_DEPTH_TEST);

    loadSatelliteTexture();
    loadDrones("UAV-VisLoc/03/03.csv");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMouseWheelFunc(mouseWheel);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);

    glutTimerFunc(16, gameLoopTimer, 0);

    glutMainLoop();
    return 0;
}
