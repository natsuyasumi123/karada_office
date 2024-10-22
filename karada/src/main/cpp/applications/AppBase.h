

#ifndef NDK_OPENGLES_3_0_GLAPPBASE_H
#define NDK_OPENGLES_3_0_GLAPPBASE_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include <ImageDef.h>
#include <ByteFlowLock.h>

//For PI define
#define MATH_PI 3.1415926535897932384626433832802

#define TYPE_BASE                             200

#define TYPE_KEY_shrink_koshi            TYPE_BASE + 0
#define TYPE_KEY_BIG_BREAST		         TYPE_BASE + 1
#define TYPE_KEY_BIG_EYES                TYPE_BASE + 2
#define TYPE_KEY_FACE_SLENDER            TYPE_BASE + 3
#define TYPE_HIPPU_ADJUST                TYPE_BASE + 4
#define TYPE_NAKA_TRIM                   TYPE_BASE + 5
#define TYPE_ASHI_LENGTHEN               TYPE_BASE + 6

#define TYPE_KEY_SET_TOUCH_LOC           TYPE_BASE + 999
#define TYPE_SET_GRAVITY_XY              TYPE_BASE + 1000

#define DEFAULT_OGL_ASSETS_DIR "/sdcard/Android/data/com.karada.app/files/Download"
class AppBase
{
public:
	AppBase()
	{
		m_ProgramObj = 0;
		m_VertexShader = 0;
		m_FragmentShader = 0;

		m_SurfaceWidth = 0;
		m_SurfaceHeight = 0;

	}

	virtual ~AppBase()
	{

	}

	virtual void LoadImage(NativeImage *pImage)
	{};

	virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage)
	{};

	virtual void LoadShortArrData(short *const pShortArr, int arrSize)
	{}

	virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
	{}

	virtual void SetTouchLocation(float x, float y)
	{}

	virtual void SetGravityXY(float x, float y)
	{}

	virtual void Init() = 0;
	virtual void Draw(int screenW, int screenH) = 0;

	virtual void Destroy() = 0;


//	void setBuhinData(float *data , int size ) ;

protected:
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_ProgramObj;
	MySyncLock m_Lock;
	int m_SurfaceWidth;
	int m_SurfaceHeight;
	float mbuhinData[33 * 3 ]  = {0.0f};
	int buhinSize ;
public :
    float degree ;
};


//void AppBase::setBuhinData(float *data , int size){
//	for(int index = 0 ; index < size ; index ++ ){
//		mbuhinData[index] = data[index] ;
//	}
//	buhinSize = size ;
//}



#endif //NDK_OPENGLES_3_0_GLAPPBASE_H
