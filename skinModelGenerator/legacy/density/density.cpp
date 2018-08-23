// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
// for offsetof
#include <cstddef>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

#ifndef ARM_RADIUS
#define ARM_RADIUS 626
#endif

// Initialise GLFW


GLint max;
GLuint program;
GLuint framebuffer;
GLuint buffer_addr, index_buffer;
GLuint vertex_buffer;
GLuint offsetUniformId;
int epochs;
GLuint offset = 0;
int* density_data;

void initGL(){
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow( 640, 480, "OpenGL", 0, 0);
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    glewInit();
}

void buildVertexBuffer(){
    GLuint indices[] = {0, 1, 2};
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLuint), indices, GL_STATIC_DRAW);
//
//    glGenVertexArrays(1, &vertex_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//    assert(glGetError() == GL_NO_ERROR);
//
//    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//    assert(glGetError() == GL_NO_ERROR);
//
//    glBindVertexArray(vertex_buffer);
//    assert(glGetError() == GL_NO_ERROR);

}

void buildFrameBuffer(){
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &buffer_addr);
    glBindTexture(GL_TEXTURE_2D, buffer_addr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, max, max, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer_addr, 0);
    //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Failed to build frame buffer..\n");
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createGLProgram(){
    ShaderInfo shaders[2];
    program = glCreateProgram();
    
    shaders[0].id = glCreateShader(GL_VERTEX_SHADER);
    shaders[0].file = "density.vert";
    shaders[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    shaders[1].file = "density.frag";
    
    loadShaders(program, 2, shaders);
    assert(glGetError() == GL_NO_ERROR);

    glUseProgram(program);
    assert(glGetError() == GL_NO_ERROR);

}

void calculateEpochs(){
    float slices = ARM_RADIUS * 5;
    int batch_size =  (max / 626 + 1) * (max / 626 + 1);
    epochs = ceil(slices / batch_size);
}

void initUniform(){
    offsetUniformId = glGetUniformLocation(program, "offset");
}

int main(){
    initGL();
    createGLProgram();
    buildFrameBuffer();
    buildVertexBuffer();
    calculateEpochs();
    initUniform();

    for (int i = 0; i < epochs; i++) {
        printf("Epoch: %d\n", i);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glUniform1i(offsetUniformId, offset); //Update on each iteration to reflrect current z processed
        glUseProgram(program);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        assert(glGetError() == GL_NO_ERROR);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        //fprintf(stderr, "\n%d\n", error = glGetError());
        //assert(error == GL_NO_ERROR);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        density_data = (int*)malloc(sizeof(int)*max*max);
        fprintf(stderr, "\n%p\n", density_data);
        glGetTextureImage(buffer_addr, 0, GL_RED, GL_INT, max * max, (void*)density_data);
        fprintf(stderr, "Here");
        assert(glGetError() == GL_NO_ERROR);
        if (density_data != NULL){
            for (int i = 0; i < 50; i++){
                printf("%d: %d\n", i, density_data[i]);
            }
        } else {
            printf("Failed to map buffer.\n");
            return 0;
        }
        //Read the data and stream to a data file, then increment offset and repeat
        offset += 719;
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Failed to bind frame buffer.\n");
    }
}
