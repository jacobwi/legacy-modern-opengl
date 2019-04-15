/*
 * @author Jacob William
 * @desc This program creates a 2d triangle with imported texture
 *
 */


#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// Importing glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL2 library import
#include "SOIL2/SOIL2.h"

// Use the standard name spaces
using namespace std;

// Global title name given to the window
#define WINDOW_TITLE "Jacob William"

// Vertex and  Fragment Shader
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Declaration of variables
GLuint VAO, VBO, EBO, texture;
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLfloat degrees = glm::radians(-45.0f);

/*
 * Prototypes to init functions before implementation
 */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);


/*
 * Vertex shader source code
 */
const GLchar* VertexShader = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 2) in vec2 textureCoordinates;

		out vec2 mobileTextureCoordinate;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * view * model * vec4(position, 1.0f);
			mobileTextureCoordinate = vec2(textureCoordinates.x, 1.0f - textureCoordinates.y);
		}
);

/*
 * Fragment Shader
 */

const GLchar* FragmentShader = GLSL(330,

	in vec2 mobileTextureCoordinate;

	out vec4 gpuTexture;

	uniform sampler2D uTexture;
	void main() {
		// Sends texture to GPU to render
		gpuTexture = texture(uTexture, mobileTextureCoordinate);
	}
);

// Main function
int main(int argc, char * argv[]) {

	// Initializes the OpenGL program properties
	// Init freeglut
	glutInit(&argc, argv);

	// Creates memory buffer for the window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// Init the window with the given width*Height
	glutInitWindowSize(WindowWidth, WindowHeight);

	// Creates the window with the title
	glutCreateWindow(WINDOW_TITLE);

	// Sets the proper window size
	glutReshapeFunc(UResizeWindow);

	// Sets the result/status when initiatite
	glewExperimental = GL_TRUE;
	// Checks if there's an error
	if (GLEW_OK !=  glewInit()) {
		cout << "Failed to init GLEW" << endl;
		return  -1;
	}


	// Calls the function to create shader
	UCreateShader();

	// Calls the function to draw the two triangles for this assigment
	UCreateBuffers();

	// Generates textures
	UGenerateTexture();

	glUseProgram(shaderProgram);
	// Sets the background color to clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Renders graphics in the window
	glutDisplayFunc(URenderGraphics);


	// Starts the OpenGL loop in the background
	glutMainLoop();


	// Deconstructors
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Termination of the program due to a successful exit
	return 0;
}

/*
 * @desc This function resize the program's GUI window
 * @parameters height, width
 * @return void
 */
void UResizeWindow(int width, int height) {
	glViewport(0, 0, width, height);
}

/*
 * @desc This function handles the rendering of graphics
 * @returns void
 */
void URenderGraphics(void) {

	// Enables z axis
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	// Model
	glm::mat4 model;

	// Model modification using glm
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	// Camera view
	glm::mat4 view;
	// Camera initial position
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

	// Projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Set values returned from each variable to its corresponding variable
	GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
	GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
	GLint proLocation = glGetUniformLocation(shaderProgram, "projection");

	// Specify the model, view, and location values in relation to the matrix
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proLocation, 1, GL_FALSE, glm::value_ptr(projection));


	// Flags to the main loop
	glutPostRedisplay();

	// Activates texture
	glBindTexture(GL_TEXTURE_2D, texture);


	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Deactivator
	glBindVertexArray(0);

	// Buffer flipper
	glutSwapBuffers();
}

void UCreateShader(void) {

	// Vertex shader
	GLint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(vertexShaderId);


	// Fragment shader
	GLint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(fragmentShaderId);

	// Shader program
	// Create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);


	// Delete the instances once the program is created and linked
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

}

/*
 * @desc this fucntion draw triangles according to the assigment
 * @returns void
 */
void UCreateBuffers() {
	// Init the vertices of the two triangles
	// BOTH TRIANGLES WILL BE SHARING THE SECOND INDEX
	GLfloat verts[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // Top Right Vertex 0
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,    // Top Left Vertex 3
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,


            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,   // Top Right Vertex 0
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,     // Top Left Vertex 3
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,


            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,   // Top Right Vertex 0
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  // Bottom Right Vertex 1
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,    // Bottom Left Vertex 2
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,    // Top Left Vertex 3
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,


             0.5f,  0.5f,  0.5f, 1.0f,  0.0f,   // Top Right Vertex 0
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  // Bottom Right Vertex 1
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f,    // Bottom Left Vertex 2
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f,     // Top Left Vertex 3
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f,


            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // Top Right Vertex 0
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // Bottom Right Vertex 1
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // Bottom Left Vertex 2
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,     // Top Left Vertex 3
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   // Top Right Vertex 0
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,    // Top Left Vertex 3
            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};



	// Generate buffer IDs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Activates the vertex object before binding any VBOs
	glBindVertexArray(VAO);

	// Activates the VBO in relation to the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// Set attrs for pointer 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// set attrs pointer 2
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Deactivate the VAO
	glBindVertexArray(0);
}

void UGenerateTexture() {


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height;
	// Texture file loader
	unsigned char* image = SOIL_load_image("snhu.JPG", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);
}
