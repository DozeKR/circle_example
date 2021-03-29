#include "context.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    glViewport(0, 0, width, height);
}

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, const char** argv)
{
    SPDLOG_INFO("Start program");
    SPDLOG_INFO("arg count: {}",argc);
    for(int i=0; i < argc; i++){
        SPDLOG_INFO("argv[{}]: {}", i, argv[i]);
    }

    float circleRadius = 0.75f;         //기본 반지름
    float incircleRadius = 0.6f;        //기본 내부 반지름
    int circleSegmentCount = 16;        //기본 원을 나눈 개수
    float st_angle = 0.0f;              //시작각
    float ed_angle = 360.0f;            //끝각
    float color_R = 1.0f;               //RED
    float color_G = 1.0f;               //GREEN
    float color_B = 1.0f;               //BLUE
    if (argc >= 2){
        circleRadius = std::stof(argv[1]);
    }
    if (argc >= 3){
        incircleRadius = std::stof(argv[2]);
    }
    if (argc >= 4){
        circleSegmentCount = std::stoi(argv[3]);
    }
    if (argc >= 5){
        st_angle = std::stof(argv[4]);
    }
    if (argc >= 6){                                         
        ed_angle = std::stof(argv[5]);
    }
    if (argc >= 9){                                                        
        color_R = std::stof(argv[6]);
        color_G = std::stof(argv[7]);
        color_B = std::stof(argv[8]);
    }

    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    context->CreateCircle(circleRadius, incircleRadius, circleSegmentCount, st_angle, ed_angle, color_R, color_G, color_B);

    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        context->Render();
        glfwSwapBuffers(window);
    }
    context.reset();    //다른 방법 context = nullptr;

    glfwTerminate();
    return 0;
}

   
