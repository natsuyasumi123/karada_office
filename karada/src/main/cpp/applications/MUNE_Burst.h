
#ifndef NDK_OPENGLES_3_0_BigBreast_H
#define NDK_OPENGLES_3_0_BigBreast_H


#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <ByteFlowLock.h>
#include "AppBase.h"

class MUNE_Burst : public AppBase
{
public:
	MUNE_Burst();

	virtual ~MUNE_Burst();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

	void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    void initMuneData() ;

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

    bool dataChange = true ;
};


#endif //NDK_OPENGLES_3_0_BigBreast_H
