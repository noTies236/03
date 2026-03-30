#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Default lib C++
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

// Lib
#include <glad/glad.h> 
#include <glfw3.h>
#include <GL/gl.h>

// VAO
GLuint VAO;
GLuint VBO;
GLuint VBO_COORD;

// texture
GLuint t_flash;

std::string LoadShaderAsString(const std::string& filename);
void CreateGraphicsPipelile();
GLuint CompileShaderProgram(GLuint type, const std::string& source);
void VertexSpecification();
GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& framentshadersource);
void Draw();
void PreDraw();

// Global
GLuint gGraphicsPipelineShaderProgram = 0;

std::string LoadShaderAsString(const std::string& filename)
{   // resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());
    
    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + "\n";
        }
        myFile.close();
    }

    std::cout << result;

    return result;
}

void VertexSpecification()
{
    glGenVertexArrays(1, &VAO); // Gera um buffer e armazena seu ID em VBO
    glBindVertexArray(VAO);

    std::vector<GLfloat> vertexPosition = {
        // Triângulo 1
        -1.0f,  0.5f, 0.0f,   // superior esquerdo
         1.0f, -0.5f, 0.0f,   // inferior direito
        -1.0f, -0.5f, 0.0f,   // inferior esquerdo

        // Triângulo 2
         1.0f,  0.5f, 0.0f,   // superior direito
        -1.0f,  0.5f, 0.0f,   // superior esquerdo
         1.0f, -0.5f, 0.0f    // inferior direito
    };

    std::vector<GLfloat> texCoords = {
        // Triângulo 1
        0.0f, 1.0f,   // superior esquerdo
        1.0f, 0.0f,   // inferior direito
        0.0f, 0.0f,   // inferior esquerdo

        // Triângulo 2
        1.0f, 1.0f,   // superior direito
        0.0f, 1.0f,   // superior esquerdo
        1.0f, 0.0f    // inferior direito
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);     
    glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW); // Enviar dados para a GPU
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    //glDisableVertexAttribArray(0);

    glGenBuffers(1, &VBO_COORD);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_COORD);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(GLfloat), texCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
}

GLuint CompileShaderProgram(GLuint type, const std::string& source)
{
    GLuint shaderObject;
        
    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }

    if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& framentshadersource)
{
    GLuint programObject = glCreateProgram();
    GLuint myVertexShader = CompileShaderProgram(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShaderProgram(GL_FRAGMENT_SHADER, framentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);
    glValidateProgram(programObject);

    return programObject;
}

void CreateGraphicsPipelile()
{
    std::string vertexShaderSource    = LoadShaderAsString("./Shaders/vert.glsl");
    std::string fragmentShaderSource  = LoadShaderAsString("./Shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void loadTexture(const char* imgTxt, GLuint textureID);
void loadTexture(const char* imgTxt, GLuint textureID)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(imgTxt, &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

	stbi_image_free(data);
	std::cout << "Texture loaded successfully" << t_flash << std::endl;
}

void PreDraw()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, 640, 480);
    glClearColor(0.f, 1.f, 1.f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gGraphicsPipelineShaderProgram);   
}

int i = 0;
void Draw()
{
    glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, t_flash);
    glDrawArrays(GL_TRIANGLES, 0, 9);
    i++;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(800, 600, "03", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Tornar o contexto OpenGL atual
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    VertexSpecification();
	loadTexture("C:/Users/wallyson/Downloads/img-1023083-flash.jpg", t_flash);
    int flashLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_texture");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(flashLoc, 0);


    CreateGraphicsPipelile();
   
    while (!glfwWindowShouldClose(window)) {
        PreDraw();
        Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
}


// https://www.inf.ufrgs.br/~amaciel/teaching/SIS0384-09-2/exercise6.html