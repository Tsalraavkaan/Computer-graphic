#include "common.h"
#include "Image.h"
#include "Player.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>

constexpr GLsizei WINDOW_WIDTH = tileSize * 60, WINDOW_HEIGHT = tileSize * 30;

struct InputState
{
    bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
    GLfloat lastX = 400, lastY = 300; //исходное положение мыши
    bool firstMouse = true;
    bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
    bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_1:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case GLFW_KEY_2:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    default:
        if (action == GLFW_PRESS)
            Input.keys[key] = true;
        else if (action == GLFW_RELEASE)
            Input.keys[key] = false;
    }
}

Point fillBuffer(Image &screenBuffer, std::vector<std::string> &level, std::vector<Trap> &traps) {
    Image bg = Image("./resources/background.png");
    Image wall = Image("./resources/wall.png");
    Image trap = Image("./resources/background.png");
    Image empty = Image("./resources/wall.png");
    Image goal = Image("./resources/goal.png");
    Point start;
    srand(time(0));
    for (int j = 0; j < WINDOW_HEIGHT / tileSize; ++j) {
        for (int i = 0; i < WINDOW_WIDTH / tileSize; ++i) {
            if (level[j][i] == '.') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, bg.GetPixel(k, l + 3 * tileSize));
                    }
                }
            } else if (level[j][i] == '#') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, bg.GetPixel(k, l + 3 * tileSize));
                        if (wall.GetPixel(k + 6 * tileSize, wall.Height() - l - 14 * tileSize - 1).a > 0.1) {
                            screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l,
                                    wall.GetPixel(k + 6 * tileSize, wall.Height() - l - 14 * tileSize - 1));
                        }
                    }
                }
            } else if (level[j][i] == 'T') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, trap.GetPixel(k + 12 * tileSize, l));
                    }
                }

                traps.push_back(Trap(i, j, random() % 3));
            } else if (level[j][i] == ' ') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l,
                                empty.GetPixel(k + 10 * tileSize - 8, wall.Height() - l - 15 * tileSize - 8));
                    }
                }
            } else if (level[j][i] == '@') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, bg.GetPixel(k, l + 3 * tileSize));
                    }
                }
                start = {.x = i * tileSize, .y = j * tileSize};
            } else if (level[j][i] == 'X') {
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, goal.GetPixel(k, l));
                    }
                }
            } else {
                std::cerr << "unknown elem in map!" << std::endl;
                for (int l = 0; l < tileSize; l++) {
                    for (int k = 0; k < tileSize; k++) {
                        Pixel red = {255, 0, 0, 0};
                        screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, red);
                    }
                }
            }
        }
    }
    return start;
}

int processPlayerMovement(Player &player, std::vector<std::string> level, GLfloat frame)
{
    int flag = 0;
    if (Input.keys[GLFW_KEY_W])
        flag = player.ProcessInput(MovementDir::UP, level, frame);
    else if (Input.keys[GLFW_KEY_S])
        flag = player.ProcessInput(MovementDir::DOWN, level, frame);
    if (flag != 0) {
        return flag;
    }
    if (Input.keys[GLFW_KEY_A])
        flag = player.ProcessInput(MovementDir::LEFT, level, frame);
    else if (Input.keys[GLFW_KEY_D])
        flag = player.ProcessInput(MovementDir::RIGHT, level, frame);
    return flag;
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        Input.captureMouse = !Input.captureMouse;

    if (Input.captureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Input.capturedMouseJustNow = true;
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (Input.firstMouse)
    {
        Input.lastX = float(xpos);
        Input.lastY = float(ypos);
        Input.firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - Input.lastX;
    GLfloat yoffset = Input.lastY - float(ypos);

    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    // ...
}


int initGL()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: "<< std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
    std::cout << "W, A, S, D - movement  "<< std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    if(!glfwInit())
        return -1;

//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); 

    glfwSetKeyCallback        (window, OnKeyboardPressed);  
    glfwSetCursorPosCallback  (window, OnMouseMove); 
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback     (window, OnMouseScroll);

    if(initGL() != 0) 
        return -1;
    
    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    std::array<std::string, 2> files = {"./resources/map1.txt", "./resources/map2.txt"};

    for (int h = 0; h < 2; ++h) {
        std::vector<Trap> traps;
        std::ifstream map_file(files[h]);
        std::vector<std::string> level(WINDOW_HEIGHT / tileSize);
        for (int i = 0; i < WINDOW_HEIGHT / tileSize; ++i) {
            for (int j = 0; j < WINDOW_WIDTH/ tileSize; ++j) {
                char temp = map_file.get();
                if (temp == '\n') {
                    temp = map_file.get();
                }
                level[level.size() - i - 1] += temp;
            }
        }
        for (std::string str : level) {
            std::cout << str << std::endl;
        }

        Point starting_pos = fillBuffer(screenBuffer, level, traps);
        Player player{starting_pos};
        screenBuffer.UpdateBuffer();

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
        bool isEnd = false, isWin = true, writePic = true;
        Image unactive_trap = Image("./resources/background.png");
        Image active_trap = Image("./resources/background.png");
        //game loop
        while (!glfwWindowShouldClose(window))
        {
            GLfloat currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            glfwPollEvents();
            if (isEnd) {
                if (writePic) {
                    if (isWin) {
                        Image win = Image("./resources/win.png");
                        for (int j = 0; j < screenBuffer.Height(); ++j) {
                            for (int i = 0; i < screenBuffer.Width(); ++i) {
                                screenBuffer.PutPixel(i, j, {.r = 0, .g = 0, .b = 0, .a = 0});
                            }
                        }
                        for (int j = 0; j < win.Height(); ++j) {
                            for (int i = 0; i < win.Width(); ++i) {
                                screenBuffer.PutPixel(392 + i, 209 + win.Height() - j, win.GetPixel(i, j));
                            }
                        }
                    } else {
                        Image lose = Image("./resources/lose.png");
                        for (int j = 0; j < screenBuffer.Height(); ++j) {
                            for (int i = 0; i < screenBuffer.Width(); ++i) {
                                screenBuffer.PutPixel(i, j, {.r = 0, .g = 0, .b = 0, .a = 0});
                            }
                        }
                        for (int j = 0; j < lose.Height(); ++j) {
                            for (int i = 0; i < lose.Width(); ++i) {
                                screenBuffer.PutPixel(340 + i, lose.Height() - j, lose.GetPixel(i, j));
                            }
                        }
                    }
                    writePic = false;
                }
            } else {
                for (Trap &trap : traps) {
                    trap.CheckFlag(lastFrame, screenBuffer, level, active_trap, unactive_trap);
                }
                int flag = processPlayerMovement(player, level, lastFrame);
                if (flag == 0) {
                    player.Draw(screenBuffer);

                } else if (flag == 1) {
                    break;
                } else {
                    isEnd = true;
                    isWin = false;
                }

            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
            glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

            glfwSwapBuffers(window);
        }

    }
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Image win = Image("./resources/win.png");
        for (int j = 0; j < screenBuffer.Height(); ++j) {
            for (int i = 0; i < screenBuffer.Width(); ++i) {
                screenBuffer.PutPixel(i, j, {.r = 0, .g = 0, .b = 0, .a = 0});
            }
        }
        for (int j = 0; j < win.Height(); ++j) {
            for (int i = 0; i < win.Width(); ++i) {
                screenBuffer.PutPixel(392 + i, 209 + win.Height() - j, win.GetPixel(i, j));
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
