

#include <koshiSlim.h>
#include <MuneBurst.h>
#include "MyGLRenderContext.h"
#include "LogUtil.h"
#include "MimiZoom.h"
#include "kaoSlender.h"

MyGLRenderContext* MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext()
{
	m_pCurSample = new koshiSlim();
	m_pBeforeSample = nullptr;
}

MyGLRenderContext::~MyGLRenderContext()
{
	if (m_pCurSample)
	{
		delete m_pCurSample;
		m_pCurSample = nullptr;
	}

	if (m_pBeforeSample)
	{
		delete m_pBeforeSample;
		m_pBeforeSample = nullptr;
	}

}


void MyGLRenderContext::SetParamsInt(int paramType, int value0, int value1)
{
	LOGCATE("MyGLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);

	if (paramType == SAMPLE_TYPE)
	{
		m_pBeforeSample = m_pCurSample;
		int buhinSize = 0 ;
		LOGCATE("MyGLRenderContext::SetParamsInt 0 m_pBeforeSample = %p", m_pBeforeSample);
		switch (value0)
		{
            case SAMPLE_TYPE_KEY_BIG_EYES:
                m_pCurSample = new MimiZoom();
				buhinSize = 8 * 3 ; // 1,2,3,4,5,6,7,8
                break;
			case SAMPLE_TYPE_KEY_shrink_koshi:
				m_pCurSample = new koshiSlim();
				buhinSize = 4 *3 ;// shouder 11,12 ; // koshi 23, 24
				break;
			case SAMPLE_TYPE_KEY_BIG_BREAST:
				m_pCurSample = new MuneBurst() ;
				buhinSize = 4 * 3  ; //shouder 11,12 ; // koshi 23, 24
				break ;
			case SAMPLE_TYPE_KEY_FACE_SLENDER:
				m_pCurSample = new kaoSlender();
				buhinSize = 6 *3 ;
				break;
			default:
			    m_pCurSample = nullptr;
				break;
		}
	}
}

void MyGLRenderContext::setKaradaData(float *kData,int index,  int length) {
    for(int i = 0 ; i < length ;i ++){
        karadaData[index][i] = kData[i] ;
    }
}


void MyGLRenderContext::setFaceData(float *fData,int index,  int length) {
    for(int i = 0 ; i < length ;i ++){
        faceData[index][i] = fData[i] ;
    }
}


void MyGLRenderContext::setDegree(float degree){
    if(m_pCurSample)
    {
        m_pCurSample->degree = degree ;
    }
}

std::vector<float> MyGLRenderContext::getKaradaData(int index , int type){
    std::vector<float> kData(5 * 3 ) ;
	switch (type) {
		case 0 :  //koshi
        {
            float koshiPosition = 0.7f ;
            kData[0 * 3] = karadaData[index][12 *3 ]  *imageWidth;
            kData[0 * 3 + 1 ] = karadaData[index][12 *3 + 1 ] * imageHeight;

            kData[1 * 3] = (karadaData[index][23 *3 ] /2 + karadaData[index][24 *3 ] /2) * imageWidth ;
            kData[1 * 3 + 1 ] = (karadaData[index][23 *3 + 1 ] /2 + karadaData[index][24 *3 + 1 ] /2)* imageHeight;

            kData[2 * 3] = karadaData[index][11 *3 ] * imageWidth ;
            kData[2 * 3 + 1 ] = karadaData[index][11 *3 + 1 ] * imageHeight ;

            kData[3 *3 ] = (karadaData[index][24 *3 ] * koshiPosition + karadaData[index][12 * 3]* (1.0f - koshiPosition)) * imageWidth;
            kData[3 *3 + 1] = (karadaData[index][24 *3 +1 ] * koshiPosition + karadaData[index][12 * 3 +1 ] *(1.0f - koshiPosition)) * imageHeight ;

            kData[4 *3 ] = (karadaData[index][23 *3 ] * koshiPosition + karadaData[index][11 * 3]* (1.0f - koshiPosition)) * imageWidth;
            kData[4 *3 + 1] = (karadaData[index][23 *3 +1 ] * koshiPosition + karadaData[index][11 * 3 +1 ] *(1.0f - koshiPosition)) * imageHeight ;
        }
        break ;

        case 1 :
        {
            kData[0 *3 ] = (karadaData[index][12 *3 ] *0.5+  karadaData[index][11 *3 ] * 0.5) * imageWidth ;
            kData[0 *3 +1] = ((karadaData[index][12 *3 +1 ] + karadaData[index][11 *3 +1]) *0.33 + (karadaData[index][24 *3+1 ]+karadaData[index][23 * 3 +1 ]) *0.17) * imageHeight ;
            kData[1 *3 ] =  kData[0 *3];
            kData[1 *3 +1] =  kData[0 *3 +1];
//            kData[0 * 3 ] = (karadaData[index][12 *3 ] * 7 /12 +  karadaData[index][24 *3 ] * 5 /12) * imageWidth;
//            kData[0 *3 + 1] = (karadaData[index][12 *3 + 1  ]*7 /12 + karadaData[index][24 *3 + 1 ] * 5 / 12) * imageHeight;
//            kData[1 * 3 ] = (karadaData[index][11 *3 ] * 7/12 + karadaData[index][23 * 3 ] * 5/12 ) * imageWidth;
//            kData[1 *3 + 1] = (karadaData[index][11 *3 + 1  ]* 7/12 + karadaData[index][23 *3 + 1 ] * 5 /12) * imageHeight;
        }
        break ;
        case 2 :{
            break ;
        }
	}
    return kData;
}


void MyGLRenderContext::SetParamsFloat(int paramType, float value0, float value1) {
	LOGCATE("MyGLRenderContext::SetParamsFloat paramType=%d, value0=%f, value1=%f", paramType, value0, value1);
	if(m_pCurSample)
	{
		switch (paramType)
		{
			case SAMPLE_TYPE_KEY_SET_TOUCH_LOC:
				m_pCurSample->SetTouchLocation(value0, value1);
				break;
			case SAMPLE_TYPE_SET_GRAVITY_XY:
                m_pCurSample->SetGravityXY(value0, value1);
				break;
			default:
				break;

		}
	}

}

void MyGLRenderContext::SetParamsShortArr(short *const pShortArr, int arrSize) {
	LOGCATE("MyGLRenderContext::SetParamsShortArr pShortArr=%p, arrSize=%d, pShortArr[0]=%d", pShortArr, arrSize, pShortArr[0]);
	if(m_pCurSample)
	{
		m_pCurSample->LoadShortArrData(pShortArr, arrSize);
	}

}

void MyGLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	LOGCATE("MyGLRenderContext::UpdateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]", rotateX, rotateY, scaleX, scaleY);
	if (m_pCurSample)
	{
		m_pCurSample->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	}
}

void MyGLRenderContext::SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData)
{
	LOGCATE("MyGLRenderContext::SetImageDataWithIndex index=%d, format=%d, width=%d, height=%d, pData=%p", index, format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;

    imageWidth = width ;
    imageHeight = height ;

	switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}

	if (m_pCurSample)
	{
		m_pCurSample->LoadMultiImageWithIndex(index, &nativeImage);
	}

}

void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData)
{
	LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;

    imageWidth = width ;
    imageHeight = height ;
    switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}

	if (m_pCurSample)
	{
		m_pCurSample->LoadImage(&nativeImage);
	}

}

void MyGLRenderContext::SetOutlineData(int format, int width, int height, uint8_t *pData)
{
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format)
    {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }

//    if (m_pCurSample)
//    {
//        m_pCurSample->LoadImage(&nativeImage);
//    }

}

void MyGLRenderContext::OnSurfaceCreated()
{
	LOGCATE("MyGLRenderContext::OnSurfaceCreated");
	glClearColor(1.0f,1.0f,1.0f, 1.0f);
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height)
{
	LOGCATE("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
	glViewport(0, 0, width, height);
	m_ScreenW = width;
	m_ScreenH = height;
}

void MyGLRenderContext::OnDrawFrame()
{
	LOGCATE("MyGLRenderContext::OnDrawFrame");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (m_pBeforeSample)
	{
		m_pBeforeSample->Destroy();
		delete m_pBeforeSample;
		m_pBeforeSample = nullptr;
	}

	if (m_pCurSample)
	{
		m_pCurSample->Init();
		m_pCurSample->Draw(m_ScreenW, m_ScreenH);
	}
}

MyGLRenderContext *MyGLRenderContext::GetInstance()
{
	LOGCATE("MyGLRenderContext::GetInstance");
	if (m_pContext == nullptr)
	{
		m_pContext = new MyGLRenderContext();
	}
	return m_pContext;
}

void MyGLRenderContext::DestroyInstance()
{
	LOGCATE("MyGLRenderContext::DestroyInstance");
	if (m_pContext)
	{
		delete m_pContext;
		m_pContext = nullptr;
	}

}

void MyGLRenderContext::getImageSize(int& width , int& height){
    width = imageWidth ;
    height = imageHeight ;
}



