#include "app.h"

int main(int argc, char *argv[]) {

    VulkanApp* app = VulkanApp::getInstance();
    app->run();

    return 0;
}

