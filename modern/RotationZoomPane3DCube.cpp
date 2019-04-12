/*
 * @author Jacob William
 * @desc This program creates a 3D Cube in modern OpenGL 
 *
 */

#include <iostream> 		// C++ I/O library
#include <GL/glew.h>		// Glew header
#include <GL/freeglut.h>	// freeglut header

// Importing glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Use the standard name spaces
using namespace std;

// Global title name given to the window
#define WINDOW_TITLE "Jacob William"

// Vertex and  Fragment Shader
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Declaration of variables
GLuint VAO, VBO;
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;

// Camera movement speed FPS
GLfloat cameraSpeed = 0.0005f;

// Locks the cursor at center of screen
GLfloat lastMouseX = 400, lastMouseY = 300;

// Mouse offset, pitch and yaw
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f;

// Sensetivity of mouse
GLfloat sensitivity = 0.005f;

bool mouseDetected = false;

// GLM global vectors
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 front;
GLint currentKey;

/*
 * Prototypes to init functions before implementation
 */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UMouseMove(int x, int y);
void IsAlt(int button, int state, int x, int y);


/*
 * Vertext shader source code
 */
const GLchar* VertexShader = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;

		out vec3 mobileColor;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * view * model * vec4(position, 1.0f);
			mobileColor = color;
		}
);

/*
 * Fragment Shader
 */

const GLchar* FragmentShader = GLSL(330,
	in vec3 mobileColor;
	out vec4 gpuColor;
	void main() {
		// gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		gpuColor = vec4(mobileColor, 1.0);
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

	glUseProgram(shaderProgram);
	// Sets the background color to clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Renders graphics in the window
	glutDisplayFunc(URenderGraphics);
	// Detects key presses
	glutMouseFunc(IsAlt);

	glutMotionFunc(UMouseMove);

	// Starts the OpenGL loop in the background
	glutMainLoop();


	// Deconstructors
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Termination of the program due to a successful exit
	return 0;
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
	cameraForwardZ = front;

	// Model
	glm::mat4 model;

	// Model modification using glm
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	// Camera view
	glm::mat4 view;
	// Camera initial position
	view = glm::lookAt(cameraForwardZ, cameraPosition, cameraUpY);

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
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
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
 * @desc This function resize the program's GUI window
 * @parameters height, width
 * @return void
 */
void UResizeWindow(int width, int height) {
	glViewport(0, 0, width, height);
}



/*
 * @desc this fucntion draw triangles according to the assigment
 * @returns void
 */
void UCreateBuffers(void) {

	// Init the vertices of the two triangles
	// BOTH TRIANGLES WILL BE SHARING THE SECOND INDEX
	GLfloat verts[] = {
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Top Right Vertex 0
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,    // Top Left Vertex 3
            -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,


            -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,   // Top Right Vertex 0
             0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,     // Top Left Vertex 3
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,


            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,   // Top Right Vertex 0
            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // Bottom Right Vertex 1
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,    // Bottom Left Vertex 2
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,    // Top Left Vertex 3
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,


             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,   // Top Right Vertex 0
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  // Bottom Right Vertex 1
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,    // Bottom Left Vertex 2
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,     // Top Left Vertex 3
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,


            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // Top Right Vertex 0
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // Bottom Right Vertex 1
             0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   // Bottom Left Vertex 2
             0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,     // Top Left Vertex 3
            -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

            -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,   // Top Right Vertex 0
             0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  // Bottom Right Vertex 1
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,    // Bottom Left Vertex 2
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,     // Top Left Vertex 3
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// set attrs pointer 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Deactivate the VAO
	glBindVertexArray(0);
}

/*
 * This function check if it's alt and check what type of mouse input is it
 */
void IsAlt(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON  && glutGetModifiers() == GLUT_ACTIVE_ALT) {
		currentKey = button;
	}
	else if(button == GLUT_RIGHT_BUTTON  && glutGetModifiers() == GLUT_ACTIVE_ALT){

		currentKey = button;
	}

}

/*
 * @desc This function handles mouse movement
 */
void UMouseMove(int x, int y) {
	// Rotating movement
	if (currentKey == GLUT_LEFT_BUTTON && glutGetModifiers() == GLUT_ACTIVE_ALT) {
		if(mouseDetected ) {
			lastMouseX = x;
			lastMouseY = y;
			mouseDetected = false;
		}

		// Direct of movement in terms of x and y
		mouseXOffset = x - lastMouseX;
		mouseYOffset = lastMouseY - y;

		// Update with new mouse coords
		lastMouseX = x;
		lastMouseY = y;

		// Apply sensitivity
		mouseXOffset *= sensitivity;
		mouseYOffset *= sensitivity;

		yaw += mouseXOffset;
		pitch += mouseYOffset;

		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		if (pitch < -89.0f) {
			pitch = 89.0f;
		}

		front.x = 10.0f * cos(yaw);
		front.y = 10.0f * sin(pitch);
		front.z = sin(yaw) * cos(pitch) * 10.0f;
	}

	// Right click + ALT detected
	// Zoom in/out action
	else if (currentKey == GLUT_RIGHT_BUTTON  && glutGetModifiers() == GLUT_ACTIVE_ALT) {
		if(mouseDetected ) {
			lastMouseX = x;
			lastMouseY = y;
			mouseDetected = false;
		}

		// Direct of movement in terms of x and y
		mouseXOffset = x - lastMouseX;
		mouseYOffset = lastMouseY - y;

		// Update with new mouse coords

		// Apply sensitivity
		mouseXOffset *= sensitivity;
		mouseYOffset *= sensitivity;

		yaw += mouseXOffset;
		pitch += mouseYOffset;

		// Prevents pitch to go into unwanted position
	    if (pitch > 89.0f) {
	    	pitch = 89.0f;
	    }
	    // Prevents pitch to go into unwanted position
	    if (pitch < -89.0f) {
	    	pitch = -89.0f;
	    }

	    // The below statement checks the mouse direction in terms of the Y-axis
	    if (mouseYOffset < 0) {
	    	front += front * sensitivity;
	    }
	    else if (mouseYOffset > 0) {

	    	front -= front * sensitivity;
	    }
	}
}

