

#include <gtc/matrix_transform.hpp>
#include "IREZUMI_Stick.h"
#include "../util/GLUtils.h"
#include "CommonDef.h"
#include "MyGLRenderContext.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



IREZUMI_Stick::IREZUMI_Stick()
{

	m_SamplerLoc = GL_NONE;
	m_StickerLoc = GL_NONE;
	m_MVPMatLoc = GL_NONE;

	m_TextureId = GL_NONE;
	m_StickerID = GL_NONE;
	m_VaoId = GL_NONE;

	m_AngleX = 0;
	m_AngleY = 0;

	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;

	m_FrameIndex = 0;
}

IREZUMI_Stick::~IREZUMI_Stick()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);

}

void IREZUMI_Stick::Init()
{
	if(m_ProgramObj)
		return;

	char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
//            "uniform mat4 u_STMatrix;\n"
            "out vec2 out_uvs;\n"
            "out vec2 st_uvs;\n"

            "void main()\n"
            "{\n"
            "   gl_Position = u_MVPMatrix * a_position;\n"
            "   out_uvs = a_texCoord;\n"
            "}";

	char fShaderStr[] =
			"#version 300 es\n"
            "precision highp float;\n"
            "in vec2 out_uvs;\n"
            "uniform sampler2D _MainTex;\n"
            "uniform sampler2D _STex;\n"
            "out vec4 SV_Target0;\n"
            "uniform vec2 topLeft;//value0:0;value1:0;restrict:false;is_uvs:0\n"
            "uniform vec2 topRight;//value0:100;value1:0;restrict:false;is_uvs:0\n"
            "uniform vec2 bottomLeft;//value0:0;value1:100;restrict:false;is_uvs:0\n"
            "uniform vec2 bottomRight;//value0:100;value1:100;restrict:false;is_uvs:0\n"
            "uniform vec2 viewPort; \n"
            "float cross2d(vec2 a, vec2 b)\n"
            "{\n"
            "    return (a.x * b.y) - (a.y * b.x);\n"
            "}\n"
            "\n"
            "vec2 invBilinear(vec2 p)\n"
            "{\n"
            "    vec2 e = (topRight-topLeft)   ;\n"
            "    vec2 f = (bottomLeft-topLeft) ;\n"
            "    vec2 g = (topLeft-topRight+bottomRight-bottomLeft);\n"
            "    vec2 h = out_uvs-topLeft ;\n"
            "\t\n"
            "    float k2 = cross2d( g, f );\n"
            "    float k1 = cross2d( e, f ) + cross2d( h, g );\n"
            "    float k0 = cross2d( h, e );\n"
            "    \n"
            "    float k2u = cross2d( e, g );\n"
            "    float k1u = cross2d( e, f ) + cross2d( g, h );\n"
            "    float k0u = cross2d( h, f);    \n"
            "   \n"
            "    float v1, u1, v2, u2;\n"
            "    \n"
            "    if (abs(k2) < 0.0001) \n"
            "    {\n"
            "        v1 = -k0 / k1;\n"
            "        u1 = (h.x - f.x*v1)/(e.x + g.x*v1);\n"
            "    } \n"
            "    else if (abs(k2u) < 0.0001) \n"
            "    {\n"
            "        u1 = k0u / k1u;\n"
            "        v1 = (h.y - e.y*u1)/(f.y + g.y*u1);\n"
            "    } \n"
            "    else \n"
            "    {\n"
            "        float w = k1*k1 - 4.0*k0*k2;\n"
            "\n"
            "        if( w<0.0 ) return vec2(-1.0, -1.0);\n"
            "\n"
            "        w = sqrt( w );\n"
            "\n"
            "        v1 = (-k1 - w)/(2.0*k2);\n"
            "        v2 = (-k1 + w)/(2.0*k2);\n"
            "        u1 = (-k1u - w)/(2.0*k2u);\n"
            "        u2 = (-k1u + w)/(2.0*k2u);\n"
            "    }\n"
            "    bool  b1 = v1>0.0 && v1<1.0 && u1>0.0 && u1<1.0;\n"
            "    bool  b2 = v2>0.0 && v2<1.0 && u2>0.0 && u2<1.0;\n"
            "    \n"
            "    vec2 res = vec2(-1.0, -1.0);\n"
            "\n"
            "    if( b2 ) return vec2( u2, v2 );\n"
            "    if( b1 ) return vec2( u1, v1 );\n"
            "\t\n"
            "    return vec2(-1.0 , -1.0);\n"
            "}\n"
            "\n"
            "void main( )\n"
            "{\n"
            "    vec2 finalUV = invBilinear(out_uvs) ; \n"
            "    vec4 stRGB = vec4(0.0) ; "
            "    if(finalUV.x == -1.0 && finalUV.y == -1.0){\n"
            "        stRGB = vec4(0.0 , 0.0, 0.0 ,0.0) ;\n"
            "    }else{\n"
            "         stRGB =  texture(_STex, finalUV);\n"
            "    }"
//            "       SV_Target0 = texture(_MainTex , out_uvs) + stRGB;\n"
            "       SV_Target0 = mix(texture(_MainTex , out_uvs)  ,  stRGB , stRGB.a);\n"
            "}" ;


	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "_MainTex");
        m_StickerLoc = glGetUniformLocation(m_ProgramObj, "_STex");
		m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
	}
	else
	{
		LOGCATE("IREZUMI_Stick::Init create program fail");
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

void IREZUMI_Stick::LoadImage(NativeImage *pImage)
{
	LOGCATE("IREZUMI_Stick::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
        ScopedSyncLock lock(&m_Lock);
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}
}
void IREZUMI_Stick::addSticker(std::string path )  {
    this->stickerPath = path ;
}

void IREZUMI_Stick::initHippuData() {
     //no need yet
}

void IREZUMI_Stick::Draw(int screenW, int screenH)
{
	LOGCATE("IREZUMI_Stick::Draw() [w,h]=[%d,%d]", screenW, screenH);

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
    if(m_StickerID == GL_NONE){
        ScopedSyncLock lock(&m_Lock);
        {
            glGenTextures(1, &m_StickerID);
            int width = 0 , height = 0 , nrChannels = 0 ;
            unsigned char *data = stbi_load(stickerPath.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = 0;
                if (nrChannels == 1)
                    format = GL_RED;
                else if (nrChannels == 3)
                    format = GL_RGB;
                else if (nrChannels == 4)
                    format = GL_RGBA;


                glBindTexture(GL_TEXTURE_2D , m_StickerID);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                             0, format, GL_UNSIGNED_BYTE,data);
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D , GL_NONE);
                stbi_image_free(data);
            }
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
    glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D , m_StickerID);
	glUniform1i(m_SamplerLoc, 0);
	glUniform1i(m_StickerLoc, 1);

    //            "uniform vec2 topLeft;//value0:0;value1:0;restrict:false;is_uvs:0\n"
    //            "uniform vec2 topRight;//value0:100;value1:0;restrict:false;is_uvs:0\n"
    //            "uniform vec2 bottomLeft;//value0:0;value1:100;restrict:false;is_uvs:0\n"
    //            "uniform vec2 bottomRight;//value0:100;value1:100;restrict:false;is_uvs:0\n"
    float ratio = degree * 30 ;
//    GLUtils::setVec2(m_ProgramObj, "viewPort", m_RenderImage.width, m_RenderImage.height);
    GLUtils::setVec2(m_ProgramObj, "topLeft", 0.5f, 0.25f);
    GLUtils::setVec2(m_ProgramObj, "topRight", 0.75f, 0.25f);
    GLUtils::setVec2(m_ProgramObj, "bottomLeft", 0.5f, 0.75f);
    GLUtils::setVec2(m_ProgramObj, "bottomRight", 0.75f, 0.75f);
    GLUtils::setVec2(m_ProgramObj, "viewPort", screenW, screenH);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}

void IREZUMI_Stick::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteBuffers(3, m_VboIds);
		glDeleteVertexArrays(1, &m_VaoId);
		glDeleteTextures(1, &m_TextureId);
		glDeleteTextures(1, &m_StickerID);
	}
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void IREZUMI_Stick::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
	LOGCATE("IREZUMI_Stick::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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

void IREZUMI_Stick::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	AppBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	m_AngleX = static_cast<int>(rotateX);
	m_AngleY = static_cast<int>(rotateY);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
}
