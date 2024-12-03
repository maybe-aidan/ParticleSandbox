#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION

#include "shader.h"
#include "texture.h"
#include "particle.h"
#include "universe.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(4.0 , 0.0 , 9.0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float mouseX, mouseY;

bool fire = false;
float speed = 1.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle Sandbox", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    Shader renderImage("shaders/raymarch_canvas.glsl", "shaders/particle_sandbox.glsl");

    float vertices[] = {
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    
    renderImage.use();
    renderImage.setFloat2("u_resolution", SCR_WIDTH, SCR_HEIGHT);


    double start = glfwGetTime();

    std::vector<Particle::Particle> particles = {
        Particle::Particle{Particle::Vector3{0, 0, 0}, Particle::Vector3{0, 0.4, 0}, Particle::Vector3{0,0,0}, 15.0f, 2.0f, 2.0f},
        Particle::Particle{Particle::Vector3{0, 3, 2}, Particle::Vector3{-0.5, 0, 0}, Particle::Vector3{0,0,0}, 20.0f, 1.0f, 1.0f},
        Particle::Particle{Particle::Vector3{0, 4, -5}, Particle::Vector3{0, 0, 2}, Particle::Vector3{0,0,0}, 10.0f, 0.3f, 0.3f}
    };
    Universe universe = Universe(1.0, particles);

    // Todo next: camera controls in cpu, pass to fragment shader.

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        renderImage.setFloat("u_time", static_cast<float>(now - start));
        deltaTime = static_cast<float>(now - lastFrame);
        lastFrame = now; 
        // input
        // -----
        processInput(window);

        if(fire){
            Particle::Particle p {
                Particle::Vector3{camera.Position.x + camera.Front.x, camera.Position.y + camera.Front.y, camera.Position.z + camera.Front.z},
                Particle::Vector3{camera.Front.x, camera.Front.y, camera.Front.z}.normalize() * speed,
                Particle::Vector3{},
                15.0f,
                0.5f,
                0.5f };
            universe.addParticle(p);
            fire = !fire;
        }

        // update physics
        float fixedDeltaTime = 0.016f;
        universe.update(fixedDeltaTime * 0.2f);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        universe.render(renderImage);
        renderImage.use();
        renderImage.setFloat3("u_camera", camera.Position.x, camera.Position.y, camera.Position.z);
        renderImage.setMat3("u_camera_look", glm::mat3(camera.Right, camera.Up, camera.Front));
        renderImage.setFloat2("u_mouse", mouseX, mouseY);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !fire){
        fire = true;
    }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    if(y_offset < 0.0f && speed <= 0.0f){
        speed = 0.0f;
    }else if(y_offset > 0.0f && speed >= 10.0f){
        speed = 10.0f;
    }else{
        speed += static_cast<float>(y_offset/10.0f);
    }

    std::cout << "Particle fire speed: " << speed << std::endl;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
