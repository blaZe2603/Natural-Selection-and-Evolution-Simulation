#include "libs.h"
#include "prey.h"
#include "food.h"
#include "predator.h"
#include "states.h"

//Projection for scaling
glm::mat4 projection = glm::ortho
(
    0.0f, float(WINDOW_WIDTH),
    0.0f, float(WINDOW_HEIGHT),
    -1.0f, 1.0f
);

//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


//Shaders
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 instanceMatrix;
layout(location = 5) in vec3 instanceColor;

uniform mat4 projection;

out vec3 fragColor; // pass color to fragment shader

void main() {
    fragColor = instanceColor;
    gl_Position = projection * instanceMatrix * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 fragColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor, 1.0);
}
)";

int main() {

    //Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simulation", NULL, NULL);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cout << "GLEW Init Failed\n"; glfwTerminate(); return -1; }

    //compliling shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //manage time
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame;
    float foodSpawnTime = 0.0f;


    //size of prey and predator
    float size = 10.0f;
    float vertices[] = 
    {
         PREY_SIZE, -PREY_SIZE, 0.0f,
        -PREY_SIZE, -PREY_SIZE, 0.0f,
         0.0f,  PREY_SIZE, 0.0f
    };

    float rectangle_vertices[] =
    {
        PREDATOR_SIZE, -PREDATOR_SIZE, 0.0f,
        -PREDATOR_SIZE, -PREDATOR_SIZE, 0.0f,
        PREDATOR_SIZE,  PREDATOR_SIZE, 0.0f,
        -PREDATOR_SIZE, PREDATOR_SIZE, 0.0f
    };
    unsigned int indices[] =
    {
        0, 1, 2,
        1, 2, 3
    };


    //buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //prey buffer
    unsigned int prey_instanceVBO, prey_colorVBO;
    glGenBuffers(1, &prey_instanceVBO);
    glGenBuffers(1, &prey_colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, prey_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, PREY_INITIAL_PARTICLES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    std::size_t vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(1 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, prey_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, PREY_INITIAL_PARTICLES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);

    //food buffer
    auto circle_verts = CreateCircle(FOOD_SEGMENTS);
    unsigned int food_VAO, food_VBO, food_InstanceVBO, food_colorVBO;
    glGenVertexArrays(1, &food_VAO);
    glGenBuffers(1, &food_VBO);
    glGenBuffers(1, &food_InstanceVBO);
    glGenBuffers(1, &food_colorVBO);

    glBindVertexArray(food_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, food_VBO);
    glBufferData(GL_ARRAY_BUFFER, circle_verts.size() * sizeof(float), circle_verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, food_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, FOOD_COUNT * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(1 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, food_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, FOOD_COUNT * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);


    //predator buffer
    unsigned int predatorVAO, predatorVBO, predatorEBO, predator_instanceVBO, predator_colorVBO;

    glGenVertexArrays(1, &predatorVAO);
    glGenBuffers(1, &predatorVBO);
    glGenBuffers(1, &predatorEBO);
    glGenBuffers(1, &predator_instanceVBO);
    glGenBuffers(1, &predator_colorVBO);

    glBindVertexArray(predatorVAO);

    // --- Vertex positions ---
    glBindBuffer(GL_ARRAY_BUFFER, predatorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // --- Indices (EBO) ---
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, predatorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // --- Instance transforms ---
    glBindBuffer(GL_ARRAY_BUFFER, predator_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, PREDATOR_INITIAL_PARTICLES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(1 + i, 1);
    }

    // --- Instance colors ---
    glBindBuffer(GL_ARRAY_BUFFER, predator_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, PREDATOR_INITIAL_PARTICLES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);


    //Initialization of prey food predator
    InitPrey();
    InitFood();
    InitPredator();

    //setting projection
    glUseProgram(shaderProgram);
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    //mainloop
    while (!glfwWindowShouldClose(window)) {
        
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        foodSpawnTime += deltaTime;
        if (foodSpawnTime >= TIME_PER_FOOD)
        {
            CreateFood();
            foodSpawnTime -= TIME_PER_FOOD;
        }

        processInput(window);
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        PreyfindFood(prey_array, nearest_food_array, food_array);
        PreyfindPredator(prey_array, nearest_predator_array, predator_array);
        PredatorfindPrey(predator_array, nearest_prey_array, prey_array);
        PreyUpdate(prey_array, food_array, predator_array, nearest_food_array, nearest_predator_array);
        PredatorUpdate(prey_array, predator_array, nearest_prey_array);
        
        //update prey and predator 
        for (auto& p : prey_array) p.update(deltaTime);;
        for (auto& p : predator_array) p.update(deltaTime);;

        if (!birth_prey_queue.empty()) {
            prey_array.insert(prey_array.end(), birth_prey_queue.begin(), birth_prey_queue.end());
            birth_prey_queue.clear();
        }
        if (!birth_predator_queue.empty()) {
            predator_array.insert(predator_array.end(), birth_predator_queue.begin(), birth_predator_queue.end());
            birth_predator_queue.clear();
        }
        //deleting the dead ones once every few itereations
        static int frame_counter = 0;
        frame_counter++;
        if (frame_counter % 100 == 0) 
        {
            prey_array.erase
            (
                std::remove_if(prey_array.begin(), prey_array.end(),
                    [](const Prey& p) { return !p.alive; }),
                prey_array.end()
            );
            predator_array.erase
            (
                std::remove_if(predator_array.begin(), predator_array.end(),
                    [](const Predator& p) { return !p.alive; }),
                predator_array.end()
            );
            food_array.erase
            (
                std::remove_if(food_array.begin(), food_array.end(),
                    [](const Food& p) { return !p.alive; }),
                food_array.end()
            );
        }



        // Upload instance matrices
        std::vector<glm::mat4> prey_matrices, food_matrices, predator_matrices;
        std::vector<glm::vec3> predator_colors,prey_colors,food_colors;
        for (auto& p : prey_array) prey_matrices.push_back(p.instance_matrices);
        for (auto& p : prey_array) prey_colors.push_back(p.color);
        for (auto& f : food_array) food_matrices.push_back(f.instanceMatrix);
        for (auto& f : food_array) food_colors.push_back(f.color);
        for (auto& p : predator_array) predator_matrices.push_back(p.instance_matrices);
        for (auto& p : predator_array) predator_colors.push_back(p.color);

        glBindBuffer(GL_ARRAY_BUFFER, prey_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, prey_matrices.size() * sizeof(glm::mat4), prey_matrices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, prey_colorVBO);
        glBufferData(GL_ARRAY_BUFFER, prey_colors.size() * sizeof(glm::vec3), prey_colors.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, food_InstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, food_matrices.size() * sizeof(glm::mat4), food_matrices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, food_colorVBO);
        glBufferData(GL_ARRAY_BUFFER, food_colors.size() * sizeof(glm::vec3), food_colors.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, predator_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, predator_matrices.size() * sizeof(glm::mat4), predator_matrices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, predator_colorVBO);
        glBufferData(GL_ARRAY_BUFFER, predator_colors.size() * sizeof(glm::vec3), predator_colors.data(), GL_DYNAMIC_DRAW);

        //render prey
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, prey_array.size());

        //render food
        glBindVertexArray(food_VAO);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, FOOD_SEGMENTS + 2, food_array.size());

        //render predator
        glBindVertexArray(predatorVAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, predator_array.size());
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
