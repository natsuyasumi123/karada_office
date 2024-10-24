

#include <gtc/matrix_transform.hpp>
#include "HADA_Smoothen.h"
#include "../util/GLUtils.h"
#include "CommonDef.h"
#include "MyGLRenderContext.h"



HADA_Smoothen::HADA_Smoothen()
{

	m_SamplerLoc = GL_NONE;
	m_MVPMatLoc = GL_NONE;

	m_TextureId = GL_NONE;
	m_VaoId = GL_NONE;

	m_AngleX = 0;
	m_AngleY = 0;

	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;

	m_FrameIndex = 0;
}

HADA_Smoothen::~HADA_Smoothen()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);

}

void HADA_Smoothen::Init()
{
	if(m_ProgramObj)
		return;

	char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            " out float kernel[30];"
            "uniform float blurSize;\n"
            "#define SIGMA 10.0\n"
            "float normpdf(in float x, in float sigma) {\n"
            "   return 0.39894 * exp(-0.5 * x * x/ (sigma * sigma)) / sigma;\n"
            "}"
            "void main()\n"
            "{\n"
            "   gl_Position = u_MVPMatrix * a_position;\n"
            "   v_texCoord = a_texCoord;\n"
            "   int kSize = (int(blurSize) - 1) / 2;\n"
            "   for (int j = 0; j <= kSize; ++j) {\n"
            "       kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), SIGMA);\n"
            "   }"
            "}";

	char fShaderStr[] =
			"#version 300 es\n"
            "precision highp float;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "in vec2 v_texCoord;\n"
            "#define SIGMA 10.0\n"
            "#define BSIGMA 0.1\n"
            "uniform sampler2D s_TextureMap;\n"
            "in float kernel[30];\n"
            "uniform float blurSize;//default:0;min:0;max:60\n"
            "uniform vec2 viewPort;\n"
            "float normpdf(in float x, in float sigma) {\n"
            "     return 0.39894 * exp(-0.5 * x * x/ (sigma * sigma)) / sigma;\n"
            "}\n"
            "float normpdf3(in vec3 v, in float sigma) {\n"
            "     return 0.39894 * exp(-0.5 * dot(v,v) / (sigma * sigma)) / sigma;\n"
            "}\n"
            " \n"
            "void main() {\n"
            "     outColor= texture(s_TextureMap, v_texCoord);\n"
            "     vec3 c = outColor.rgb;\n"
            "     vec3 final_colour = vec3(0.0);\n"
            "     float bZ = 1.0 / normpdf(0.0, BSIGMA);\n"
            "     vec3 cc;\n"
            "     float Z = 0.0;\n"
            "     float factor;\n"
            "     int kSize = (int(blurSize) - 1) / 2;\n"
            "     for (int i=-kSize; i <= kSize; i+=5) {\n"
            "         for (int j=-kSize; j <= kSize; j+=5) {\n"
            "             cc = texture(s_TextureMap, vec2(float(i),float(j)) / viewPort + v_texCoord).rgb;\n"
            "             factor = normpdf3(cc-c, BSIGMA) * bZ * kernel[kSize+j] * kernel[kSize+i];\n"
            "             Z += factor;\n"
            "             final_colour += factor * cc;\n"
            "         }\n"
            "     }\n"
            "     outColor= vec4(final_colour / Z, outColor.a) ; \n"
            "}";

	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
		m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
	}
	else
	{
		LOGCATE("HADA_Smoothen::Init create program fail");
	}

	GLfloat verticesCoords[] = {
			-1.0f,  1.0f, 0.0f,  // Position 0
			-1.0f, -1.0f, 0.0f,  // Position 1
			1.0f,  -1.0f, 0.0f,  // Position 2
			1.0f,   1.0f, 0.0f,  // Position 3
	};

	GLfloat textureCoords[] = {
			0.0f,  0.0f,        // TexCoord 0
			0.0f,  1.0f,        // TexCoord 1
			1.0f,  1.0f,        // TexCoord 2
			1.0f,  0.0f         // TexCoord 3
	};

	GLushort indices[] = { 0, 1, 3, 1, 2, 3 };

	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(3, m_VboIds);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Generate VAO Id
	glGenVertexArrays(1, &m_VaoId);
	glBindVertexArray(m_VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
	glBindVertexArray(GL_NONE);

}

void HADA_Smoothen::LoadImage(NativeImage *pImage)
{
	LOGCATE("HADA_Smoothen::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
        ScopedSyncLock lock(&m_Lock);
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}
}

void HADA_Smoothen::initHippuData() {
     //no need yet
}

void HADA_Smoothen::Draw(int screenW, int screenH)
{
	LOGCATE("HADA_Smoothen::Draw() [w,h]=[%d,%d]", screenW, screenH);

	if(m_ProgramObj == GL_NONE) return;

	if(m_TextureId == GL_NONE)
    {
        ScopedSyncLock lock(&m_Lock);
        if(m_RenderImage.ppPlane[0] != nullptr)
        {
            glGenTextures(1, &m_TextureId);
            glBindTexture(GL_TEXTURE_2D, m_TextureId);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
        }
        return;
    }
    initHippuData() ;
	glViewport(0, 0, screenW, screenH);

	UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)screenW / screenH);
	// Use the program object
	glUseProgram (m_ProgramObj);
	glBindVertexArray(m_VaoId);
	glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
	// Bind the RGBA map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glUniform1i(m_SamplerLoc, 0);

    float ratio = degree * 30 ;
	GLUtils::setFloat(m_ProgramObj, "blurSize", ratio);
//    GLUtils::setVec2(m_ProgramObj, "viewPort", m_RenderImage.width, m_RenderImage.height);
    GLUtils::setVec2(m_ProgramObj, "viewPort", screenW, screenH);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}

void HADA_Smoothen::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteBuffers(3, m_VboIds);
		glDeleteVertexArrays(1, &m_VaoId);
		glDeleteTextures(1, &m_TextureId);
	}
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void HADA_Smoothen::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
	LOGCATE("HADA_Smoothen::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
	angleX = angleX % 360;
	angleY = angleY % 360;

	//转化为弧度角
	float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
	float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


	// Projection matrix
	glm::mat4 Projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
	//glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

	// View matrix
	glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
	Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

	mvpMatrix = Projection * View * Model;

}

void HADA_Smoothen::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	AppBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	m_AngleX = static_cast<int>(rotateX);
	m_AngleY = static_cast<int>(rotateY);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
}
