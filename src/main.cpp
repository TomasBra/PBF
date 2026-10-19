#include "Window.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int main()
{
    Window window = Window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");
    return window.Run();
}
