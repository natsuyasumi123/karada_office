

#include <gtc/matrix_transform.hpp>
#include "MUNE_Burst.h"
#include "../util/GLUtils.h"

#include "MyGLRenderContext.h"


//float brestLeftPoint[] = {karada_data22[6]/2 + karada_data22[3] /2, karada_data22[7] * 0.667f + karada_data22[28] *0.333f};
//float rightBreastPoint[] = {karada_data22[15]/2 + karada_data22[3] /2, karada_data22[16] * 0.667f + karada_data22[37] *0.333f};
float brestLeftPoint[2] = {} ;
float rightBreastPoint[2] = {};
int breastSize = 200 ;

MUNE_Burst::MUNE_Burst()
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

MUNE_Burst::~MUNE_Burst()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);

}

void MUNE_Burst::initMuneData() {
//    if(!dataChange ){
//        return ;
//    }
//    dataChange = false ;
    std::vector<float> muneData(5 * 3 ) ;
    bool ret = MyGLRenderContext::GetInstance()->getKaradaData(0 , 1 ,muneData);
    if(ret){
        brestLeftPoint[0] = muneData[0 *3 ] ;
        brestLeftPoint[1] = muneData[0 *3 + 1] ;
        rightBreastPoint[0] = muneData[1 *3 ] ;
        rightBreastPoint[1] = muneData[1 *3  +1] ;
        int imageWidth= 0 , imageHeight = 0 ;
        MyGLRenderContext::GetInstance()->getImageSize(imageWidth , imageHeight) ;
        int muneHorizonDiffer = rightBreastPoint[0] - brestLeftPoint[0] ;
        breastSize = muneHorizonDiffer  ;
        LOGCATE("MUNE_Burst::Init create program fail");
    }
}

void MUNE_Burst::Init()
{
	if(m_ProgramObj)
		return;

	char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    v_texCoord = a_texCoord;\n"
            "}";

	char fShaderStr[] =
			"#version 300 es\n"
            "precision highp float;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "in vec2 v_texCoord;\n"
			"uniform highp vec2 u_LeftEyeCenterPos;// 左眼中心点\n"
			"uniform highp vec2 u_RightEyeCenterPos;// 右眼中心点\n"
            "uniform highp float u_ScaleRatio;//放大系数\n"
            "uniform highp float u_Radius;// 影响半径\n"
            "uniform vec2 u_ImgSize;//图片分辨率\n"
            "\n"
            "vec2 warpEyes(vec2 centerPos, vec2 curPos, float radius, float scaleRatio)\n"
            "{\n"
            "    vec2 result = curPos;\n"
            "    vec2 imgCurPos = curPos * u_ImgSize;\n"
            "    float d = distance(imgCurPos, centerPos);\n"
            "\n"
            "    if(d < radius)\n"
            "    {\n"
            "        float gamma = 1.0 - scaleRatio * pow(smoothstep(0.0, 1.0, d / radius) - 1.0, 2.0);\n"
			"        result = centerPos + gamma * (imgCurPos - centerPos);\n"
            "        result = result / u_ImgSize;\n"
            "\n"
            "    }\n"
            "    return result;\n"
            "\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec2 newTexCoord = warpEyes(u_LeftEyeCenterPos, v_texCoord, u_Radius, u_ScaleRatio);\n"
            "    newTexCoord = warpEyes(u_RightEyeCenterPos, newTexCoord, u_Radius, u_ScaleRatio);\n"
            "    outColor = texture(s_TextureMap, newTexCoord);\n"
            "}\n"
            "\n"
            "";

	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
		m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
	}
	else
	{
		LOGCATE("MUNE_Burst::Init create program fail");
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

	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

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

void MUNE_Burst::LoadImage(NativeImage *pImage)
{
	LOGCATE("MUNE_Burst::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
	    ScopedSyncLock lock(&m_Lock);
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}
}

void MUNE_Burst::Draw(int screenW, int screenH)
{
	LOGCATE("MUNE_Burst::Draw() [w,h]=[%d,%d]", screenW, screenH);

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
    initMuneData() ;

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

//    float offset = (m_FrameIndex % 100) * 1.0f / 100;
//    offset = (m_FrameIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    float offset = degree ;
	GLUtils::setFloat(m_ProgramObj, "u_ScaleRatio", offset * 1.3f);
	GLUtils::setFloat(m_ProgramObj, "u_Radius", breastSize);
	GLUtils::setVec2(m_ProgramObj, "u_LeftEyeCenterPos", brestLeftPoint[0], brestLeftPoint[1]);
    GLUtils::setVec2(m_ProgramObj, "u_RightEyeCenterPos", rightBreastPoint[0], rightBreastPoint[1]);
    GLUtils::setVec2(m_ProgramObj, "u_ImgSize", m_RenderImage.width, m_RenderImage.height);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

}

void MUNE_Burst::Destroy()
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
void MUNE_Burst::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
	LOGCATE("MUNE_Burst::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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

void MUNE_Burst::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	AppBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	m_AngleX = static_cast<int>(rotateX);
	m_AngleY = static_cast<int>(rotateY);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
}
