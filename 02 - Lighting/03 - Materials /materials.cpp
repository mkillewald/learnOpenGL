#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 9.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(0.0f, 0.0f, 3.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
 
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program using the Shader class
    // -----------------------------------------------------------
    Shader lightingShader("../resources/shaders/3.3.shader.vert", "../resources/shaders/3.3.shader.frag");  // paths relative to binary at run time
    Shader lightCubeShader("../resources/shaders/3.3.shader.vert", "../resources/shaders/3.3.lightSource.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // material definitions from http://devernay.free.fr/cours/opengl/materials.html
    Material materials[] = {       
        {   glm::vec3(0.0215f, 0.1745f, 0.0215f), // [0] emerald
            glm::vec3(0.07568f,	0.61424f, 0.07568f),
            glm::vec3(0.633f, 0.727811f, 0.633f),
            0.6f },
        {   glm::vec3(0.135f, 0.2225f, 0.1575f), // [1] jade
            glm::vec3(0.54f, 0.89f, 0.63f),
            glm::vec3(0.316228f, 0.316228f, 0.316228f),
            0.1f },
        {   glm::vec3(0.05375f, 0.05f, 0.06625f), // [2] obsidian
            glm::vec3(0.18275f, 0.17f, 0.22525f),
            glm::vec3(0.332741f, 0.328634f, 0.346435f),
            0.3f },
        {   glm::vec3(0.25f, 0.20725f, 0.20725f), // [3] pearl
            glm::vec3(1.0f, 0.829f, 0.829f),
            glm::vec3(0.296648f, 0.296648f, 0.296648f),
            0.088f },
        {   glm::vec3(0.1745f, 0.01175f, 0.01175f), // [4] ruby
            glm::vec3(0.61424f, 0.04136f, 0.04136f),
            glm::vec3(0.727811f, 0.626959f, 0.626959f),
            0.6f },
        {   glm::vec3(0.1f, 0.18725f, 0.1745f), // [5] turquoise
            glm::vec3(0.396f, 0.74151f, 0.69102f),
            glm::vec3(0.297254f, 0.30829f, 0.306678f),
            0.1f },
        {   glm::vec3(0.329412f, 0.223529f, 0.027451f), // [6] brass
            glm::vec3(0.780392f, 0.568627f, 0.113725f),
            glm::vec3(0.992157f, 0.941176f, 0.807843f),
            0.21794872f },
        {   glm::vec3(0.2125f, 0.1275f, 0.054f), // [7] bronze
            glm::vec3(0.714f, 0.4284f, 0.18144f),
            glm::vec3(0.393548f, 0.271906f, 0.166721f),
            0.2f },
        {   glm::vec3(0.25f, 0.25f, 0.25f), // [8] chrome
            glm::vec3(0.4f, 0.4f, 0.4f),
            glm::vec3(0.774597f, 0.774597f, 0.774597f),
            0.6f },
        {   glm::vec3(0.19125f, 0.0735f, 0.0225f), // [9] copper
            glm::vec3(0.7038f, 0.27048f, 0.0828f),
            glm::vec3(0.256777f, 0.137622f, 0.086014f),
            0.1f },
        {   glm::vec3(0.24725f, 0.1995f, 0.0745f), // [10] gold
            glm::vec3(0.75164f, 0.60648f, 0.22648f),
            glm::vec3(0.628281f, 0.555802f, 0.366065f),
            0.4f },
        {   glm::vec3(0.19225f, 0.19225f, 0.19225f), // [11] silver
            glm::vec3(0.50754f, 0.50754f, 0.50754f),
            glm::vec3(0.508273f, 0.508273f, 0.508273f),
            0.4f },
        {   glm::vec3(0.0f, 0.0f, 0.0f), // [12] black plastic
            glm::vec3(0.01f, 0.01f, 0.01f),
            glm::vec3(0.50f, 0.50f, 0.50f),
            0.25f },
        {   glm::vec3(0.0f, 0.1f, 0.06f), // [13] cyan plastic
            glm::vec3(0.0f, 0.50980392f, 0.50980392f),
            glm::vec3(0.50196078f, 0.50196078f, 0.50196078f),
            0.25f },
        {   glm::vec3(0.0f, 0.0f, 0.0f), // [14] green plastic
            glm::vec3(0.1f, 0.35f, 0.1f),
            glm::vec3(0.45f, 0.55f, 0.45f),
            0.25f },
        {   glm::vec3(0.0f, 0.0f, 0.0f), // [15] red plastic
            glm::vec3(0.5f, 0.0f, 0.0f),
            glm::vec3(0.7f,	0.6f, 0.6f),
            0.25f },
        {   glm::vec3(0.0f, 0.0f, 0.0f), // [16] white plastic
            glm::vec3(0.55f, 0.55f, 0.55f),
            glm::vec3(0.70f, 0.70f, 0.70f),
            0.25f },
        {   glm::vec3(0.0f, 0.0f, 0.0f), // [17] yellow plastic
            glm::vec3(0.5f, 0.5f, 0.0f),
            glm::vec3(0.60f, 0.60f, 0.50f),
            0.25f },
        {   glm::vec3(0.02f, 0.02f, 0.02f), // [18] black rubber
            glm::vec3(0.01f, 0.01f, 0.01f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            0.078125f },
        {   glm::vec3(0.0f, 0.05f, 0.05f), // [19] cyan rubber
            glm::vec3(0.4f, 0.5f, 0.5f),
            glm::vec3(0.04f, 0.7f, 0.7f),
            0.078125f },
        {   glm::vec3(0.0f, 0.05f, 0.0f), // [20] green rubber
            glm::vec3(0.4f, 0.5f, 0.4f),
            glm::vec3(0.04f, 0.7f, 0.04f),
            0.078125f },
        {   glm::vec3(0.05f, 0.0f, 0.0f), // [21] red rubber
            glm::vec3(0.5f, 0.4f, 0.4f),
            glm::vec3(0.7f, 0.04f, 0.04f),
            0.078125f },
        {   glm::vec3(0.05f, 0.05f, 0.05f), // [22] white rubber
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.7f, 0.7f, 0.7f),
            0.078125f },
        {   glm::vec3(0.05f, 0.05f, 0.0f), // [23] yellow rubber
            glm::vec3(0.5f, 0.5f, 0.4f),
            glm::vec3(0.7f, 0.7f, 0.04f),
            0.078125f },
    };

    // ..:: Initialization code ::..

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // 1. bind Vertex Array Object
    glBindVertexArray(cubeVAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set the vertex attributes pointers
    // position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal vector attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // create VAO for our light source
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create projection and view transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // activate lighting shader
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::vec3 lightColor(1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);   
        
        // lighting effects for our original cube
        // lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 

        // original cube material effects
        // lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // lightingShader.setFloat("material.shininess", 32.0f);

        // rotate original cube
        // model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // lightingShader.setMat4("model", model);

        // render original cube
        glBindVertexArray(cubeVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // lighting effects for using materials from http://devernay.free.fr/cours/opengl/materials.html
        lightingShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 

        int row = 0, col =0;
        for (int i=0; i < 24; ++i) {
            lightingShader.setVec3("material.ambient", materials[i].ambient);
            lightingShader.setVec3("material.diffuse", materials[i].diffuse);
            lightingShader.setVec3("material.specular", materials[i].specular);
            lightingShader.setFloat("material.shininess", materials[i].shininess * 128.0f);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.5f*(float)(col%6)-3.75f, -1.5f*(float)(row)+2.25f, 0.0f));
            model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.4f, 1.0f, 0.0f));
            lightingShader.setMat4("model", model);
            
            // render cube
            // glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            if (i%6==5) {
                ++ row;
                col = 0;
            } else {
                ++col;
            }
        }

        // move light source
        lightPos = glm::vec3(2.5f*glm::sin(0.5f*glfwGetTime()), 1.7f*glm::cos(0.5f*glfwGetTime()), 3.0f);

        // draw our light source object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));        
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightColor);

        // draw the light cube object
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // -W-----------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{   
    
    // exit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // walking through our scene..
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}