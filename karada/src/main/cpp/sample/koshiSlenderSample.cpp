/**
 *
 * Created by 公众号：字节流动 on 2020/4/18.
 * https://github.com/githubhaohao/NDK_OpenGLES_3_0
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <gtc/matrix_transform.hpp>
#include "koshiSlenderSample.h"
#include "../util/GLUtils.h"
#include "CommonDef.h"



float karada_data [75] = {
		328.507,188.281,0.892449,303.478,376.13,0.711954,
		165.762,388.688,0.567716,65.6257,428.757,0.744823,
		128.08,253.397,0.780997,448.659,361.136,0.691428,
		488.866,626.521,0.451658,506.282,814.359,0.671578,
		280.946,856.934,0.252068,185.818,851.959,0.233529,
		0,0,0,0,0,0,
		388.611,864.402,0.22777,0,0,0,
		0,0,0,283.516,155.762,0.862234,
		333.648,150.707,0.918626,205.877,203.338,0.840428,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0
};
//float LeftCheekKeyPoint11[] = {211, 363};//左脸颊关键点
//float ChinKeyPoint11[] = {336, 565};//下巴关键点
//float RightCheekPoint11[] = {471, 365};//右脸颊关键点
//float LeftSlenderCtlPoint11[] = {211, 512};//左侧控制点
//float RightSlenderCtlPoint11[] = {477, 509};//右侧控制点
int leftShadowPoint = 2 ,rightShadowPoint = 5 , centerBottom = 8 ;
int pointSize = 3 ;
float koshiPosition = 0.7f ;
float LeftCheekKeyPoint11[] = {karada_data[leftShadowPoint * pointSize ], karada_data[leftShadowPoint *pointSize + 1 ]};//左肩膀
float ChinKeyPoint11[] = {karada_data[centerBottom *pointSize ], karada_data[centerBottom * pointSize + 1 ] };//下巴关键点
float RightCheekPoint11[] = {karada_data[15], karada_data[16]};//右脸颊关键点
float LeftSlenderCtlPoint11[] = {karada_data[27 ] * koshiPosition + karada_data[6] *(1.0f - koshiPosition) - 20,
								 karada_data[28] *koshiPosition + karada_data[7] * (1.0f - koshiPosition)};//左侧控制点
float RightSlenderCtlPoint11[] = {karada_data[36]  * koshiPosition + karada_data[15 ] * (1.0f - koshiPosition) + 20 ,
								  karada_data[37]  * koshiPosition + karada_data[16 ] * (1.0f - koshiPosition)};//右侧控制点

koshiSlenderSample::koshiSlenderSample()
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

koshiSlenderSample::~koshiSlenderSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);

}

void koshiSlenderSample::Init()
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
            "in vec2 v_texCoord;\n"
            "uniform sampler2D s_TextureMap;\n"
            "uniform vec2 u_texSize;\n"
            "uniform vec4 u_preCtrlPoints;\n"
            "uniform vec4 u_curCtrlPoints;\n"
            "uniform float u_reshapeRadius;\n"
            "uniform float u_reshapeRatio;\n"
            "\n"
            "vec2 face_slender_1(vec2 prePoint, vec2 curPoint, vec2 texCoord, float radius, vec2 texSize)\n"
            "{\n"
            "    vec2 pos = texCoord;\n"
            "\n"
            "    vec2 newSrc = prePoint * texSize;\n"
            "    vec2 newDst = curPoint * texSize;\n"
            "    vec2 newTex = texCoord * texSize;\n"
            "    float newRadius = radius;\n"
            "    float r = distance(newSrc, newTex);\n"
            "\n"
            "    if (r < newRadius)\n"
            "    {\n"
            "        float alpha = 1.0 -  r / newRadius;\n"
            "        vec2 displacementVec = (newDst - newSrc) * pow(alpha, 2.0) * 0.5 * u_reshapeRatio;\n"
            "        pos = (newTex - displacementVec) / texSize;\n"
            "\n"
            "    }\n"
            "    return pos;\n"
            "}\n"
            "\n"
            "vec2 face_slender_2(vec2 prePoint, vec2 curPoint, vec2 texCoord, float radius, vec2 texSize)\n"
            "{\n"
            "    vec2 pos = texCoord;\n"
            "\n"
            "    vec2 newSrc = prePoint * texSize;\n"
            "    vec2 newDst = curPoint * texSize;\n"
            "    vec2 newTex = texCoord * texSize;\n"
            "    float newRadius = radius;\n"
            "    float r = distance(newSrc, newTex);\n"
            "\n"
            "    if(r < newRadius)\n"
            "    {\n"
            "        float gamma = (pow(newRadius, 2.0) - pow(r, 2.0)) / (pow(newRadius, 2.0) - pow(r, 2.0) + pow(distance(newDst, newSrc), 2.0));\n"
            "        float sigma = pow(gamma, 2.0);\n"
            "        vec2 displacementVec = (newDst - newSrc) * sigma * u_reshapeRatio;\n"
            "        pos = (newTex - displacementVec) / texSize;\n"
            "    }\n"
            "\n"
            "    return pos;\n"
            "}\n"
            "\n"
            "void main() {\n"
            "    vec2 leftPreCtrl = u_preCtrlPoints.xy;\n"
            "    vec2 rightPreCtrl = u_preCtrlPoints.zw;\n"
            "\n"
            "    vec2 leftCurCtrl = u_curCtrlPoints.xy;\n"
            "    vec2 rightCurCtrl = u_curCtrlPoints.zw;\n"
            "\n"
            "    vec2 newTexCoord = face_slender_1(leftPreCtrl, leftCurCtrl, v_texCoord, u_reshapeRadius, u_texSize);\n"
            "    newTexCoord = face_slender_1(rightPreCtrl, rightCurCtrl, newTexCoord, u_reshapeRadius, u_texSize);\n"
            "\n"
            "    outColor = texture(s_TextureMap, newTexCoord);\n"
            "}";

	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
		m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
	}
	else
	{
		LOGCATE("koshiSlenderSample::Init create program fail");
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

void koshiSlenderSample::LoadImage(NativeImage *pImage)
{
	LOGCATE("koshiSlenderSample::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
        ScopedSyncLock lock(&m_Lock);
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}

}

void koshiSlenderSample::Draw(int screenW, int screenH)
{
	LOGCATE("koshiSlenderSample::Draw() [w,h]=[%d,%d]", screenW, screenH);

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

	glViewport(0, 0, screenW, screenH);

	m_FrameIndex ++;
	UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)screenW / screenH);

	// Use the program object
	glUseProgram (m_ProgramObj);

	glBindVertexArray(m_VaoId);

	glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

	// Bind the RGBA map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glUniform1i(m_SamplerLoc, 0);

    float ratio = (m_FrameIndex % 100) * 1.0f / 100;
    ratio = (m_FrameIndex / 100) % 2 == 1 ? (1 - ratio) : ratio;

    float effectRadius = PointUtil::Distance(PointF(LeftCheekKeyPoint11[0], LeftCheekKeyPoint11[1]), PointF(ChinKeyPoint11[0], ChinKeyPoint11[1])) / 2;
    LOGCATE("koshiSlenderSample::Draw() ratio=%f, effectRadius=%f", ratio, effectRadius);
	GLUtils::setFloat(m_ProgramObj, "u_reshapeRatio", ratio);
	GLUtils::setFloat(m_ProgramObj, "u_reshapeRadius", effectRadius);
	GLUtils::setVec4(m_ProgramObj, "u_preCtrlPoints",
	        LeftSlenderCtlPoint11[0] / m_RenderImage.width, LeftSlenderCtlPoint11[1] / m_RenderImage.height,
	        RightSlenderCtlPoint11[0] / m_RenderImage.width, RightSlenderCtlPoint11[1] / m_RenderImage.height);

	PointF leftCurPoint = PointUtil::PointAdd(PointF(LeftCheekKeyPoint11[0], LeftCheekKeyPoint11[1]), PointF(ChinKeyPoint11[0], ChinKeyPoint11[1]));
	leftCurPoint = PointUtil::PointDivide(leftCurPoint, 2);

	PointF rightCurPoint = PointUtil::PointAdd(PointF(RightCheekPoint11[0], RightCheekPoint11[1]), PointF(ChinKeyPoint11[0], ChinKeyPoint11[1]));
    rightCurPoint = PointUtil::PointDivide(rightCurPoint, 2);

    GLUtils::setVec4(m_ProgramObj, "u_curCtrlPoints",
            leftCurPoint.x / m_RenderImage.width, leftCurPoint.y / m_RenderImage.height,
            rightCurPoint.x / m_RenderImage.width, rightCurPoint.y / m_RenderImage.height);
    GLUtils::setVec2(m_ProgramObj, "u_texSize", m_RenderImage.width, m_RenderImage.height);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

}

void koshiSlenderSample::Destroy()
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
void koshiSlenderSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
	LOGCATE("koshiSlenderSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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

void koshiSlenderSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	GLSampleBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	m_AngleX = static_cast<int>(rotateX);
	m_AngleY = static_cast<int>(rotateY);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
}
