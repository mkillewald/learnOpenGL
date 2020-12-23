#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_s.h>
#include <stb_image/stb_image.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float *mixPct);
void recursiveTransform(unsigned int transformLoc, glm::vec3 translate, glm::mat4 transMatrix, int depth);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program using the Shader class
    // -----------------------------------------------------------
    Shader ourShader("../resources/shaders/3.3.shader.vert", "../resources/shaders/3.3.smiley.frag");  // paths relative to binary at run time

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------ 

    float vertices[] = {
        // positions          // colors           // texture coords
         -1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   -0.5f, 1.05f, // Point A top left
         1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.5f, 1.05f, // Point B top right
         0.0f,  -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.5f, -0.5f, // Point C bottom  
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // triangle
    };  

    // ..:: Initialization code ::..

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);

    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    // 4. then set the vertex attributes pointers
    // position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create textures
    unsigned int texture1, texture2;

    // texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("../resources/textures/container.jpg", &width, &height, &nrChannels, 0);  // path relative to binary at run time
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);

    float mixPct = 0.3f;
    glm::vec4 clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

    ourShader.use(); // activate the shader before setting uniforms!
    ourShader.setFloat("u_mixPct", mixPct);
    ourShader.setVec4("u_clearColor", clearColor);
    glUniform1i(glGetUniformLocation(ourShader.ID, "u_texture1"), 0);

    // render loop
    // -----------
    double xpos, ypos;

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, &mixPct);
        ourShader.setFloat("u_mixPct", mixPct);
        ourShader.setFloat("u_time", glfwGetTime());
        ourShader.setVec2("u_resolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
        
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 u_mouse = glm::vec2(xpos, ypos) / glm::vec2(SCR_WIDTH, SCR_HEIGHT); // normalixe to window size and origin is at top left
        u_mouse -= 0.5; // orign now at center
        u_mouse.y = -u_mouse.y; //invert y
        ourShader.setVec2("u_mouse", u_mouse);

        // render
        // ------
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, texture2);

        // transform and draw
        glBindVertexArray(VAO);
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "u_transform");
        recursiveTransform(transformLoc, glm::vec3(0.0f, -0.5f, 0.0f), glm::mat4(1.0f), 8);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float *mixPct)
{   
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && *mixPct < 1.0f) {
        *mixPct += 0.01f;
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && *mixPct > 0.0f) {
        *mixPct -= 0.01f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    glfwGetWindowSize(window, (int*)&SCR_WIDTH, (int*)&SCR_HEIGHT);
}

void recursiveTransform(unsigned int transformLoc, glm::vec3 translate, glm::mat4 matrix, int depth) {
    if (depth < 0 ) return;

    // center
    matrix = glm::translate(matrix, translate); // move 
    matrix = glm::scale(matrix, glm::vec3(0.5f));  // scale
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    --depth;
    recursiveTransform(transformLoc, glm::vec3(0.0f, 1.5f, 0.0f), matrix, depth);  // top 
    recursiveTransform(transformLoc, glm::vec3(-1.0f, -0.5f, 0.0f), matrix, depth);  // left side
    matrix = glm::scale(matrix, glm::vec3(-1.0f,1.0f,1.0f));  // flip matrix on x to reverse our texture
    recursiveTransform(transformLoc, glm::vec3(-1.0f, -0.5f, 0.0f), matrix, depth); // right side (same translation vector is used since matrix was flipped)
}