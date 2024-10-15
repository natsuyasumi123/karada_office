

#ifndef NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
#define NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H

#include "vector"
#include "stdint.h"
#include <GLES3/gl3.h>
#include "AppBase.h"

class MyGLRenderContext
{
	MyGLRenderContext();

	~MyGLRenderContext();

public:
	void SetImageData(int format, int width, int height, uint8_t *pData);
	void SetOutlineData(int format, int width, int height, uint8_t *pData);

	void SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData);

	void SetParamsInt(int paramType, int value0, int value1);

	void SetParamsFloat(int paramType, float value0, float value1);

	void SetParamsShortArr(short *const pShortArr, int arrSize);

	void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

	void OnSurfaceCreated();

	void OnSurfaceChanged(int width, int height);

	void OnDrawFrame();

    void setKaradaData(float *kData ,int index ,  int length) ;
    void setFaceData(float *kData ,int index ,  int length) ;

	bool getKaradaData(int index, int type ,std::vector<float>& kData) ;
//    bool getFaceData(int index, int type, std::vector<float> ret) ;

    void setDegree(float degree) ;

	static MyGLRenderContext* GetInstance();
	static void DestroyInstance();

    void getImageSize(int & width , int & height) ;

private:
	static MyGLRenderContext *m_pContext;
	AppBase *m_pBeforeSample;
	AppBase *m_pCurSample;
	int m_ScreenW = 0 ;
	int m_ScreenH = 0 ;
    int imageWidth = 0 ;
    int imageHeight = 0 ;
    GLfloat karadaData[10][33 * 3 ] = {};
    bool dataSet = false ;
    GLfloat faceData[10][478 * 3 ] = {};
};


#endif //NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
