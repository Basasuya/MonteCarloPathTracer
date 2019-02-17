#include <iostream>
#include "Model.h"
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

void setParams(int argc, char ** argv) {
    int i;
    if ((i = ArgPos((char *) "-read", argc, argv)) > 0) chooseRender = atoi(argv[i + 1]);
}

void Render1() {
}

void Render2() {
}

void Render3() {
}

void Render4() {
    Model model("./models/Scene04");
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
    return 0;
}