// global includes
#include <iostream>
#include <vector>
#include <string>

// local includes
#include "glad/include/glad/glad.h"
#include "SDL.h"
#include "SDL_opengl.h"

bool fullscreen = false;
int scrWidth = 800;
int scrHeight = 600;

// helper function to compile the shader
void loadShader(GLuint shaderID, const GLchar* shaderSource) {
	glShaderSource( shaderID, 1, &shaderSource, NULL );
	glCompileShader( shaderID );

	// check the actual compilation status of shader
	GLint status;
	glGetShaderiv( shaderID, GL_COMPILE_STATUS, &status );
	if (!status) {
		char buffer[512]; 
		glGetShaderInfoLog( shaderID, 512, NULL, buffer );
		printf( "shader Compile Failed. Info:\n\n%s\n", buffer );
	}
}

int main(int argc, char** argv) {
	std::cout << "pissing" << std::endl;

	// simple shaders
	// could also write these in a file, especially if theyre more complicated
	const GLchar* vertexSource =
		"#version 150 core\n"
		"in vec2 position;"
		"in vec3 inColor;"
		"out vec3 Color;"
		"void main() {"
		"	Color = inColor;"
		"	gl_Position = vec4(position, 0.0, 1.0);"
		"}";

	const GLchar* fragmentSource =
		"#version 150 core\n"
		"in vec3 Color;"
		"out vec4 outColor;"
		"void main() {"
		"	outColor = vec4(Color, 1.0);" // r g b Alpha
		"}";

	SDL_Init( SDL_INIT_VIDEO ); // this initializes graphics for openGL

	// print SDL version
	SDL_version comp; SDL_version linked;
	SDL_VERSION( &comp ); SDL_GetVersion( &linked );
	printf( "\nCompiled against SDL version %d.%d.%d\n", comp.major, comp.minor, comp.patch );
	printf( "Linked SDL version %d.%d.%d\n", linked.major, linked.minor, linked.patch );

	// this is getting a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

	// creates window with:
	//  name, offsetx, offsety, width, height, flags

	/*
		Flags include:
			SDL_WINDOW_FULLSCREEN
			SDL_WINDOW_RESIZABLE
				-> recalculate aspect ratio
			SDL_WINDOW_FULLSCREEN_DESKTOP
				-> windowed borderless
				-> pass 0 for height and width
	*/
	SDL_Window* window = SDL_CreateWindow(
		"My Practice :^)",
		100, 100,
		scrWidth, scrHeight,
		SDL_WINDOW_OPENGL
	);

	if ( !window ) {
		// exits with an error message and failure code when failing
		printf( "Could not create window :(  : %s\n", SDL_GetError() );
		return EXIT_FAILURE;
	}
	
	// creates the aspect ratio of the window
	float aspect = scrWidth / ( float )scrHeight;

	// binds openGL to the window
	SDL_GLContext context = SDL_GL_CreateContext( window ); 

	// print version but this time using glad and ensure all pointers are loaded
	if ( gladLoadGLLoader( SDL_GL_GetProcAddress ) ) {
		printf( "OpenGL loaded\n" );
		printf( "Vendor: %s\n", glGetString( GL_VENDOR) );
		printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
		printf( "Version: %s\n", glGetString( GL_VERSION ) );
	}
	else {
		printf( "ERROR: Failed to initialize OpenGL context.\n" );
		return EXIT_FAILURE;
	}

	// creates and loads vertex shader
	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
	loadShader( vertexShader, vertexSource );

	// creates and loads 
	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	loadShader( fragmentShader, fragmentSource );

	// creates the shader program that we will be binding shaders to
	// this computes the final output color outColor
	GLuint shaderProgram = glCreateProgram();

	// attach the two shaders
	glAttachShader( shaderProgram, vertexShader );
	glAttachShader( shaderProgram, fragmentShader );

	// bind and link
	glBindFragDataLocation( shaderProgram, 0, "outColor" ); // set output
	glLinkProgram( shaderProgram ); // run the linker

	// creates triangle with vertices and colors
	// Coordinates are in normalized device coordinate space
	// in NDC space, (1,1) is top right, and (-1,-1) is bottom left
	GLfloat vertices[] = {
		0.0f, 0.5f, 1.0f, 0.0f, 0.0f, // vertex 1: pos = (0, 0.5), color red
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // vertex 2: pos = (0.5, -0.5) color green
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f // vertex 3: pos = (-0.5, -0.5) color blue
	};

	// need to move vertice data into a Vertex Buffer Object so that the GPU knows what it is
	GLuint vbo;
	glGenBuffers( 1, &vbo ); // this actually creates the 1 buffer in vbo
	glBindBuffer( GL_ARRAY_BUFFER, vbo ); // binds vbo buffer

	// GL_STATIC_DRAW hints to the CPU that this data is unlikely to change each frame
	// better optimizes the usage of GPU memory
	// if data is changing infrequently, use GL_DYNAMIC_DRAW
	// if data is changing often, use GL_STREAM_DRAW
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// vertex array object stores mapping between data stored in 
	// the vbo and and the inputs to our shaders
	GLuint vao;
	glGenVertexArrays( 1, &vao ); // creates vertex array object
	glBindVertexArray( vao ); // binds vertex array object

	// gets position attribute of the shaderProgram
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	// this records that a vertex position information is two floats long, and starts every fifth element
	// params are:
	//	attribute, vals/attribute, type, isNormalized, stride, offset
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof( float ), 0 );
	glEnableVertexAttribArray( posAttrib ); // marks attributes location as valid
	
	// gets color attribute of the shaderProgram
	GLint colAttrib = glGetAttribLocation( shaderProgram, "inColor" );
	// this records that a vertex position information is two floats long, and starts every fifth element
	// params are:
	//	attribute, vals/attribute, type, isNormalized, stride, offset
	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* )( 2*sizeof( float ) ) );
	glEnableVertexAttribArray( colAttrib ); // marks attributes location as valid

	glBindVertexArray( 0 ); // unbind vao so we dont accidentally modify it

	// this is the main gameloop
	// it updates the screen and also listens for events
	SDL_Event windowEvent;
	bool quit = false;
	while ( !quit ) {
		while ( SDL_PollEvent( &windowEvent ) ) {
			if ( windowEvent.type == SDL_QUIT ) quit = true; // exit game loop
			if ( windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE )
				quit = true; // exit game loop
			if ( windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f ) {
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
			}
		}
		SDL_GL_SwapWindow(window); // double buffering

		glClearColor(0.2f, 0.5f, 0.8f, 1.0f); // clears screen to blue
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram( shaderProgram ); // set the active shader program
		glBindVertexArray( vao ); // bind the VAO for the sahders we are using
		glDrawArrays( GL_TRIANGLES, 0, 3 );
	}

	// cleanup SDL and OpenGL contexts
	glDeleteProgram( shaderProgram );
	glDeleteShader( fragmentShader );
	glDeleteShader( vertexShader );
	glDeleteBuffers( 1, &vbo );
	glDeleteVertexArrays( 1, &vao );
	SDL_GL_DeleteContext( context );
	SDL_Quit();

	return EXIT_SUCCESS;
}