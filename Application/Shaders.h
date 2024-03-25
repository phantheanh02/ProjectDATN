#pragma once
#include "../Utilities/utilities.h"

class Shaders 
{
public:
	GLuint program, vertexShader, fragmentShader;
	GLint positionAttribute;
	//GLint colorAttribute;
	GLint uvAttribute;
	GLint iTextureLoc;
	GLint uniformWVP;
	GLint uniformAlpha;
	GLint uniformNumFrames;
	GLint uniformNumActions;
	GLint uniformCurrFrame;
	GLint uniformCurrAction;
	int Init(const char * fileVertexShader, const char * fileFragmentShader);
	~Shaders();

	GLint GetID() { return m_id; }
	void SetID(GLint id) { m_id = id; }

private:
	GLint m_id;
};