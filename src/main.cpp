#include <iostream>
#include "SceneBuilder.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {

    if(argc < 2 || argc > 3) {
        cout << "Incorrect argument\n"
            << "Usage: ./tracer.exe [scene-file] [anti-aliasing cycles (default=1)]" << "\n"
            << "Example: ./tracer.exe scene.xml 10" << "\n";
        return 1;
    }

    int aadepth = argv[2] ? atoi(argv[2]) : 1;

    SceneBuilder b;
    b.setAntiAliasing(aadepth);
    cout << "Importing xml...\n";
    b.importScene(argv[1]);
    cout << "XML imported.\n";

    b.printScene();
    b.exportScene();

    return 0;
}
