

#ifndef NDK_OPENGLES_3_0_A_MU_Thin_APP_H
#define NDK_OPENGLES_3_0_A_MU_Thin_APP_H


#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <ByteFlowLock.h>
#include "AppBase.h"
//手臂编辑
class A_MU_Thin : public AppBase
{
public:
	A_MU_Thin();

	virtual ~A_MU_Thin();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

	void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    void initAshiData();

private:
	GLuint m_TextureId;
	GLint m_SamplerLoc;
	GLint m_MVPMatLoc;
	GLuint m_VaoId;
	GLuint m_VboIds[3];
	NativeImage m_RenderImage;
	glm::mat4 m_MVPMatrix;

	int m_AngleX;
	int m_AngleY;
	float m_ScaleX;
	float m_ScaleY;

	int m_FrameIndex;
    float LeftCheekKeyPoint[2] = {};//左脸颊关键点
    float ChinKeyPoint[2] = {};//下巴关键点
    float RightCheekPoint[2] = {};//右脸颊关键点
    float LeftSlenderCtlPoint[2] = {};//左侧控制点
    float RightSlenderCtlPoint[2] = {};//右侧控制点

};


#endif
