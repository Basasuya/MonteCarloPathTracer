#include <iostream>
#include "Model.h"
#include <GLUT/glut.h>
using namespace std;

int chooseRender = 4;
int ArgPos(char *str, int argc, char **argv) {
    int a;
    for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
        if (a == argc - 1) {
            printf("Argument missing for %s\n", str);
            exit(1);
        }
        return a;
    }
    return -1;
}
Model *model;
void setParams(int argc, char ** argv) {
    int i;
    if ((i = ArgPos((char *) "-read", argc, argv)) > 0) chooseRender = atoi(argv[i + 1]);
}

void Render1() {
    model = new Model("./models/Scene01");
}

void Render2() {
    model = new Model("./models/Scene02");
}

void Render3() {
    model = new Model("./models/Scene03");
}

void Render4() {
    model = new Model("./models/Scene04");
}

void render(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, model->scene.width,  0, model->scene.height);

    float* colors = model->render();

    glRasterPos2i(0, 0);
    glDrawPixels(model->scene.width, model->scene.height, GL_RGB, GL_FLOAT, (GLvoid *)colors);

    glFlush();
}


int main(int argc,char** argv) {
    setParams(argc, argv);
    switch (chooseRender)
    {
        case 1:
            Render1();
            break;

        case 2:
            Render2();
            break;    
    
        case 3:
            Render3();
            break;

        default:
            Render4();
            break;
    }

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(model->scene.width, model->scene.height);
	glutCreateWindow("motecarlo");

	glutDisplayFunc(render);
	glutIdleFunc(glutPostRedisplay);
    glutMainLoop();
    return 0;
}