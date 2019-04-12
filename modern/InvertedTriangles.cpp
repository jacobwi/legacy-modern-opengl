/*
 * @author Jacob William
 */

#include <iostream> 		// C++ I/O library
#include <GL/glew.h>		// Glew header
#include <GL/freeglut.h>	// freeglut header

// Use the standard name spaces
using namespace std;

// Global title name given to the window
#define WINDOW_TITLE "Jacob William"

// Vertex and  Fragment Shader
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Heights and width variables
int WindowWidth = 800;
int WindowHeight = 600;


/*
 * Prototypes to init functions before implementation
 */
void UInitialize(int, char*[]);
void UInitWindow(int, char*[]);
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateVBO(void);
void UCreateShaders(void);


/*
 *
 */
const GLchar* VertexShader = GLSL(440,
	in layout(location=0) vec4 vertex_Position;

	in layout(location=1) vec4 colorFromVBO;
	out vec4 colorFromVShader;
			void main() {
				gl_Position = vertex_Position;
				colorFromVShader = colorFromVBO;
			}
);

/*
 * Fragment Shader
 */

const GLchar* FragmentShader = GLSL(440,
	in vec4 colorFromVShader;
	out vec4 vertex_Color;
	void main() {
	// gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	vertex_Color = colorFromVShader;
}
);
// Main function
int main(int argc, char * argv[]) {

	// Initializes the OpenGL program properties
	UInitialize(argc, argv);

	// Starts the OpenGL loop in the background
	glutMainLoop();

	// Termination of the program due to a successful exit
	exit(EXIT_SUCCESS);
}

/*
 * @desc This function initializes the opengl properties
 * @parameters arguments count, actual arguments in array form
 * @returns void
 */
void UInitialize(int argc, char* argv[]) {

	// Glew status variable
	GLenum GlewInitResult;

	// Intialize the program's window
	UInitWindow(argc, argv);

	// Sets the result/status when initiatite
	GlewInitResult = glewInit();

	// Checks if there's an error
	if (GLEW_OK != GlewInitResult) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	// Displays the local opengl version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));


	// Calls the function to draw the two triangles for this assigment
	UCreateVBO();

	// Calls the function to create shader
	UCreateShaders();

	// Sets the background color to clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/*
 * @desc this function intializes the GUI window of the program
 * @parameters arguments count, actual arguments in array form
 * @returns void
 */
void UInitWindow(int argc, char* argv[]) {

	// Init freeglut
	glutInit(&argc, argv);

	// Init the window with the given width*Height
	glutInitWindowSize(WindowWidth, WindowHeight);

	// Creates memory buffer for the window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// Creates the window with the title
	glutCreateWindow(WINDOW_TITLE);

	// Sets the proper window size
	glutReshapeFunc(UResizeWindow);

	// Renders graphics in the window
	glutDisplayFunc(URenderGraphics);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint totalVerts = 6;
	// glDrawArrays(GL_TRIANGLES, 0, totalVerts);
	glDrawElements(GL_TRIANGLES, totalVerts, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers();
}

/*
 * @desc this fucntion draw triangles according to the assigment
 * @returns void
 */
void UCreateVBO(void) {

	// Init the vertices of the two triangles
	// BOTH TRIANGLES WILL BE SHARING THE SECOND INDEX
	GLfloat verts[] = {
			// First triangle
			-1.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,

			// Shared Index
			-0.5f, 0.0f,
			0.0f, 1.0f, 0.0f, 1.0f,

			-1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 1.0f,

			// Second triangle
			0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, -1.0f,
			0.0f, 0.0f, 1.0f, 1.0f
	};

	// Retrieves the size of the above vertices
	float numOfVerticies = sizeof(verts);

	// Holds the object of buffer
	GLuint myBufferID;

	// This function creates 1 buffer in accordance to the reference address of the buffer object
	glGenBuffers(1, &myBufferID);

	// This function binds and actives the buffer
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);

	// Send the coords vertices to the GPU
	glBufferData(GL_ARRAY_BUFFER, numOfVerticies, verts, GL_STATIC_DRAW);

	GLuint floatsPerVertex = 2;

	glEnableVertexAttribArray(0);

	// Sets the color to green;
	//glColor3f(0.0f, 1.0f, 0.0f);

	// Vertex Stride according to (x, y, r,
	GLint vertexStride = sizeof(float) * 6;

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, vertexStride, 0);

	glEnableVertexAttribArray(1);


	// Number of colors according to RGBA and XY
	GLint colorStride = sizeof(float) * 6;

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, colorStride, (char*)(sizeof(float) * 2));


	GLushort indicies[] = {0, 1, 2, 3, 4,1};
	float numIndicies = sizeof(indicies);
	GLuint indexBufferID;
	// This function creates 1 buffer in accordance to the reference address of the buffer object
	glGenBuffers(1, &indexBufferID);

	// This function binds and actives the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	// Send the coords vertices to the GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies, indicies, GL_STATIC_DRAW);

}

void UCreateShaders(void) {
	GLuint ProgramId = glCreateProgram();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShaderId, 1, &VertexShader, NULL);
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL);

	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);

	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	glLinkProgram(ProgramId);
	glUseProgram(ProgramId);
}
