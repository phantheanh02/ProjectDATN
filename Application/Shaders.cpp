#include "stdafx.h"
#include "Shaders.h"

int Shaders::Init(const char * fileVertexShader, const char * fileFragmentShader)
{
	vertexShader = esLoadShader(GL_VERTEX_SHADER, fileVertexShader);

	if ( vertexShader == 0 )
		return -1;

	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fileFragmentShader);

	if ( fragmentShader == 0 )
	{
		glDeleteShader( vertexShader );
		return -2;
	}

 	program = esLoadProgram(vertexShader, fragmentShader);

	//finding location of uniforms / attributes
	positionAttribute = glGetAttribLocation(program, "a_posL");
	uvAttribute = glGetAttribLocation(program, "a_uv");
	uniformWVP = glGetUniformLocation(program, "u_wvpMatrix");
	iTextureLoc = glGetUniformLocation(program, "u_texture");
	uniformAlpha = glGetUniformLocation(program, "u_alpha");
	uniformNumFrames = glGetUniformLocation(program, "u_numFrames");
	uniformNumActions = glGetUniformLocation(program, "u_numActions");
	uniformCurrFrame = glGetUniformLocation(program, "u_currentFrame");
	uniformCurrAction = glGetUniformLocation(program, "u_currentAction");
	
	return 0;
}

Shaders::~Shaders()
{
	glDeleteProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}