#include "RenderApp.h"

int main() {
    soku::RenderApp app(720, 720);
    if (app.Initialize()) {
        app.Run();
    }
    return 0;
}