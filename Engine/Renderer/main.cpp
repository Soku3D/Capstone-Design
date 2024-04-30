#include "RenderApp.h"

int main() {
    soku::RenderApp app(1080, 1000);
    if (app.Initialize()) {
        app.Run();
    }
    return 0;
}