#include <cmath>
#include "KOSHI_Slim.h"
#include "MUNE_Burst.h"
#include "MyGLRenderContext.h"
#include "LogUtil.h"
#include "Me_Zoom.h"
#include "KAO_Slender.h"
#include "HIPPU_Adjust.h"
#include "NAKA_Trim.h"
#include "ASHI_Lengthen.h"
#include "ASHI_Thin.h"
#include "A_MU_Thin.h"
#include "HADA_Smoothen.h"
#include "IREZUMI_Stick.h"


MyGLRenderContext* MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext()
{
    m_curApp = new KOSHI_Slim();
    m_preApp = nullptr;
}

MyGLRenderContext::~MyGLRenderContext()
{
	if (m_curApp)
	{
		delete m_curApp;
        m_curApp = nullptr;
	}

	if (m_preApp)
	{
		delete m_preApp;
        m_preApp = nullptr;
	}

}


void MyGLRenderContext::SetParamsInt(int paramType, int value0, int value1)
{
	LOGCATE("MyGLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);

	if (paramType == TYPE_BASE)
	{
        m_preApp = m_curApp;
		LOGCATE("MyGLRenderContext::SetParamsInt 0 m_preApp = %p", m_preApp);
		switch (value0)
		{
            case TYPE_KEY_BIG_EYES:
                m_curApp = new Me_Zoom();
                break;
			case TYPE_KEY_shrink_koshi:
                m_curApp = new KOSHI_Slim();
				break;
			case TYPE_KEY_BIG_BREAST:
                m_curApp = new MUNE_Burst() ;
				break ;
			case TYPE_KEY_FACE_SLENDER:
                m_curApp = new KAO_Slender();
				break;
            case TYPE_HIPPU_ADJUST :
                m_curApp = new HIPPU_Adjust() ;
                break;
            case TYPE_NAKA_TRIM:
                m_curApp = new NAKA_Trim() ;
                break ;
            case TYPE_ASHI_LENGTHEN:
                m_curApp = new ASHI_Lengthen() ;
                break ;
            case TYPE_ASHI_THIN :
                m_curApp = new ASHI_Thin();
                break ;
            case TYPE_A_MU_THIN :
                m_curApp = new A_MU_Thin() ;
                break ;
            case TYPE_HADA_SMOOTHEN :
                m_curApp = new HADA_Smoothen() ;
                break ;
            case TYPE_IREZUMI_STICKER :
                m_curApp = new IREZUMI_Stick() ;
                break ;
			default:
                m_curApp = nullptr;
				break;
		}
	}
}


void MyGLRenderContext::addSticker(std::string stickerPath){
     if(m_curApp != nullptr){
         m_curApp->addSticker(stickerPath) ;
     }
}



void MyGLRenderContext::setKaradaData(float *kData,int index,  int length) {
    for(int i = 0 ; i < length ;i ++){
        karadaData[index][i] = kData[i] ;
    }
    dataSet = true ;
}


void MyGLRenderContext::setFaceData(float *fData,int index,  int length) {
    for(int i = 0 ; i < length ;i ++){
        faceData[index][i] = fData[i] ;
    }
    dataSet = true ;
}


void MyGLRenderContext::setDegree(float degree){
    if(m_curApp)
    {
        m_curApp->degree = degree ;
    }
}

 bool MyGLRenderContext::getKaradaData(int index , int type  ,std::vector<float>& kData){
//    std::vector<float> kData(5 * 3 ) ;

    if(!dataSet || imageWidth ==0 || imageHeight == 0)
        return false ;
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

        case 1 ://胸
        {
            kData[0 * 3 ] = (karadaData[index][12 *3 ] * 2.0/3 +  karadaData[index][11 *3 ] * 1.0 /3) * imageWidth;
            kData[0 *3 + 1] = (karadaData[index][12 *3 + 1]*8 /12 + karadaData[index][24 *3 + 1] * 4 / 12) * imageHeight;
            kData[1 * 3 ] = (karadaData[index][11 *3 ] * 2.0/3 + karadaData[index][12 * 3 ] * 1.0/3 ) * imageWidth;
            kData[1 *3 + 1] = (karadaData[index][11 *3 + 1]* 8/12 + karadaData[index][23 *3 + 1] * 4 /12) * imageHeight;
        }

        break ;
        case 2 :{ // 眼睛
            kData[0 *3 + 0 ] = faceData[index][468 *3 ] * imageWidth ;
            kData[0 *3 + 1] = faceData[index][468 *3  + 1 ] * imageHeight ;
            kData[1 *3 + 0] = faceData[index][473 *3 ]   * imageWidth ;
            kData[1 *3 + 1] = faceData[index][473 *3 +1]  * imageHeight ;
            kData[2 *3 + 0] = (faceData[index][133 *3 ] - faceData[index][130 *3 ] ) *imageWidth;
            kData[2 *3 + 1] = (faceData[index][263 *3 ] - faceData[index][463 *3 ] )* imageWidth;
        }
        break ;
        case 3 :{ //脸
            kData[0 *3 + 0] = faceData[index][127 *3 ] * imageWidth ;
            kData[0 *3 + 1] = faceData[index] [127 *3  + 1 ] * imageHeight ;
            kData[1 *3 + 0] =faceData[index][152 *3 ]  * imageWidth ;
            kData[1 *3 + 1] =faceData[index][152 *3 +1]  * imageHeight ;
            kData[2*3 +0] =faceData[index][356 *3 ] * imageWidth ;
            kData[2*3 +1] =faceData[index][356 *3 +1] * imageHeight ;
            kData[3*3 +0] =faceData[index][58 *3 ] * imageWidth ;
            kData[3*3 +1] =faceData[index][58 *3 +1 ]  * imageHeight ;
            kData[4*3 +0] =faceData[index][367 *3 ] * imageWidth  ;
            kData[4*3 +1] =faceData[index][367 *3 +1 ] * imageHeight ;
            break ;
        case 4:{ //髋部
            kData[0 *3 + 0] = karadaData[index][24 *3  ] * imageWidth ;
            kData[0 *3 + 1] = karadaData[index] [24 *3  + 1 ] * imageHeight ;
            kData[1 *3 + 0] =(karadaData[index][25 *3 ] /2 +karadaData[index][26 *3 ] /2) * imageWidth ;
            kData[1 *3 + 1] =(karadaData[index][25 *3 + 1 ] /2 +karadaData[index][26 *3 + 1] /2)  * imageHeight ;
            kData[2*3 +0] =karadaData[index][23 *3 ] * imageWidth  ;
            kData[2*3 +1] =karadaData[index][23 *3 +1] * imageHeight ;
            int hippuWidth = std::abs(karadaData[index][23 * 3 ] - karadaData[index][24 *3 ]) * imageWidth ;
            kData[3 *3 + 0] = karadaData[index][24 *3  ] * imageWidth - hippuWidth /3;
            kData[3 *3 + 1] = karadaData[index] [24 *3  + 1 ] * imageHeight ;
            kData[4*3 +0] =karadaData[index][23 *3 ] * imageWidth + hippuWidth /3  ;
            kData[4*3 +1] =karadaData[index][23 *3 +1] * imageHeight ;
            break ;
        }
        case 5 : { //naka
            kData[0 *3 + 0 ] = (karadaData[index][24 *3 ] /2 + karadaData[index][23 *3 ] /2)* imageWidth ;
            kData[0 *3 + 1] =( (karadaData[index][24 *3 +1] /2 + karadaData[index][23 *3 +1] /2) * 3 /4
                                + (karadaData[index][11 *3 +1] /2 + karadaData[index][12 *3 +1] /2) * 1 /4) * imageHeight ;
            kData[1 *3 + 0] =  (karadaData[index][24 *3 ] /2 + karadaData[index][23 *3 ] /2)* imageWidth ;
            kData[1 *3 + 1] = ( (karadaData[index][24 *3 +1] /2 + karadaData[index][23 *3 +1] /2) * 3 /4
                                + (karadaData[index][11 *3 +1] /2 + karadaData[index][12 *3 +1] /2) * 1 /4) * imageHeight ;
            kData[2 *3 + 0] = std::abs(karadaData[index][23 *3 ] - karadaData[index][24 *3 ] ) *imageWidth ;
            kData[2 *3 + 1] =  std::abs(karadaData[index][23 *3 ] - karadaData[index][24 *3 ] ) *imageWidth ;
            break ;
        }
        case 6 :{ //ashi 长腿
            kData[0 *3 + 0 ] = 0 ;
            kData[0 *3 + 1] = (karadaData[index][23 *3 +1] /2 +  karadaData[index][24 *3 +1] /2 ) * imageHeight;
            kData[1 *3 + 0] = 0;
            kData[1 *3 + 1] = (karadaData[index][27 *3 +1] /2 +  karadaData[index][28 *3 +1] /2 ) * imageHeight;
            break ;
        }
        case 7 :{ // legs
            //   LeftCheekKeyPoint[0] = kaoData[0 *3];//左脸颊关键点
            //          LeftCheekKeyPoint[1] =  kaoData[0 *3 + 1];//左脸颊关键点
            //          ChinKeyPoint[0] = kaoData[1 *3 ];//下巴关键点
            //          ChinKeyPoint[1] = kaoData[1 *3 +1];//下巴关键点
            //          RightCheekPoint[0] = kaoData[2 *3 ];//右脸颊关键点
            //          RightCheekPoint[1] = kaoData[2 *3 +1];//右脸颊关键点
            //          LeftSlenderCtlPoint[0] = kaoData[3 *3 ];//左侧控制点
            //          LeftSlenderCtlPoint[1] = kaoData[3 *3 +1];//左侧控制点
            //          RightSlenderCtlPoint[0] = kaoData[4 *3 ];//右侧控制点
            //          RightSlenderCtlPoint[1] = kaoData[4 *3 + 1];//右侧控制点
            kData[1 *3 + 0] =karadaData[index][14 *3 ]  * imageWidth  -60;
            kData[1 *3 + 1] =karadaData[index][14 *3 +1]  * imageHeight +80;
            kData[0 *3 + 0] =(karadaData[index][14 *3 ] *0/5 + karadaData[index][12 *3 ]*5 /5 ) * imageWidth  - 40  ;
            kData[0 *3 + 1] =(karadaData[index][14 *3 +1 ]*0/5 + karadaData[index][12 *3  + 1]*5 /5) * imageHeight - 40 ;
            kData[2*3 +0] =(karadaData[index][14 *3 ] *0/5 + karadaData[index][12 *3 ] *5 /5 ) * imageWidth + 40 ;
            kData[2*3 +1] =(karadaData[index][14 *3 +1 ]*0/5 + karadaData[index][12 *3  + 1]*5 /5) * imageHeight + 40  ;
            kData[3*3 +0] =(karadaData[index][14 *3 ] *4/6 + karadaData[index][12 *3 ] *2 /6 ) * imageWidth -35  ;
            kData[3*3 +1] =(karadaData[index][14 *3 +1 ]*4/6 + karadaData[index][12 *3  + 1]* 2 /6 )  * imageHeight-35 ;
            kData[4*3 +0] =(karadaData[index][14 *3 ] *4/6 + karadaData[index][12 *3 ] * 2 /6 ) * imageWidth  +35 ;
            kData[4*3 +1] =(karadaData[index][14 *3 +1 ]*4/6 + karadaData[index][12 *3  + 1] *2 /6 ) * imageHeight +35 ;

//            kData[5 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[5 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[6 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[6 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[7*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[7*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[8*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[8*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[9*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[9*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;
//
//            kData[10 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[10 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[11 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[11 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[12*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[12*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[13*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[13*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[14*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[14*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;
//
//            kData[15 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[15 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[16 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[16 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[17*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[17*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[18*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[18*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[19*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[19*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;

break ;
        }
        case 8 :{//arms
            //   LeftCheekKeyPoint[0] = kaoData[0 *3];//左脸颊关键点
            //          LeftCheekKeyPoint[1] =  kaoData[0 *3 + 1];//左脸颊关键点
            //          ChinKeyPoint[0] = kaoData[1 *3 ];//下巴关键点
            //          ChinKeyPoint[1] = kaoData[1 *3 +1];//下巴关键点
            //          RightCheekPoint[0] = kaoData[2 *3 ];//右脸颊关键点
            //          RightCheekPoint[1] = kaoData[2 *3 +1];//右脸颊关键点
            //          LeftSlenderCtlPoint[0] = kaoData[3 *3 ];//左侧控制点
            //          LeftSlenderCtlPoint[1] = kaoData[3 *3 +1];//左侧控制点
            //          RightSlenderCtlPoint[0] = kaoData[4 *3 ];//右侧控制点
            //          RightSlenderCtlPoint[1] = kaoData[4 *3 + 1];//右侧控制点

            float leftArm0Size[2] ={std::abs(karadaData[index][12 * 3 ] - karadaData[index][14 * 3 ] ) * imageWidth
                                    ,std::abs(karadaData[index][12 * 3 +1] - karadaData[index][14 * 3 +1 ] ) * imageHeight  } ;//左上臂x y
            float leftArm1Size[2] ={std::abs(karadaData[index][16 * 3 ] - karadaData[index][14 * 3 ] ) * imageWidth
                                    ,std::abs(karadaData[index][16 * 3 +1] - karadaData[index][14 * 3 +1 ] ) * imageHeight } ;//左前臂 x y
            float rightArm0Size[2] ={std::abs(karadaData[index][11 * 3 ] - karadaData[index][13 * 3 ] ) * imageWidth
                                    ,std::abs(karadaData[index][11 * 3 +1] - karadaData[index][13 * 3 +1 ] ) * imageHeight} ;//右上臂 x y
            float rightArm1Size[2] ={std::abs(karadaData[index][15 * 3 ] - karadaData[index][13 * 3 ] ) * imageWidth
                                    ,std::abs(karadaData[index][15 * 3 +1] - karadaData[index][13 * 3 +1 ] ) * imageHeight} ;//右前臂 x y
            float leftArm0Length =  sqrt(leftArm0Size[0] * leftArm0Size[0]  + leftArm0Size[1] * leftArm0Size[1]) ;
            float leftArm1Length =  sqrt(leftArm1Size[0] * leftArm1Size[0]  + leftArm1Size[1] * leftArm1Size[1]) ;
            float rightArm0Length =  sqrt(rightArm0Size[0] * rightArm0Size[0]  + rightArm0Size[1] * rightArm0Size[1]) ;
            float rightArm1Length =  sqrt(rightArm1Size[0] * rightArm1Size[0]  + rightArm1Size[1] * rightArm1Size[1]) ;

            float leftArmLength = std::max(leftArm0Length , leftArm1Length);
            float rightArmLength = std::max(rightArm0Length , rightArm1Length) ;

            float leftArm0Width = leftArmLength / 3 ; //左上臂宽
            float leftArm1Width = leftArmLength / 4 ; //左前臂宽
            float rightArm0Width = rightArmLength /3 ; //右上臂宽
            float rightArm1Width = rightArmLength /4 ; //右前臂宽

            kData[0 *3 + 0] = (karadaData[index][14 *3 ] *0/5 + karadaData[index][12 *3 ]*5 /5 ) * imageWidth  - leftArm0Width /2   ;
            kData[0 *3 + 1] =(karadaData[index][14 *3 +1 ]*0/5 + karadaData[index][12 *3  + 1]*5 /5) * imageHeight - leftArm0Width /2 ;
            kData[1 *3 + 0] =karadaData[index][14 *3 ]  * imageWidth  -leftArm0Width;
            kData[1 *3 + 1] =karadaData[index][14 *3 +1]  * imageHeight +leftArm0Width;
            kData[2*3 +0] =(karadaData[index][14 *3 ] *0/5 + karadaData[index][12 *3 ] *5 /5 ) * imageWidth + leftArm0Width /2;
            kData[2*3 +1] =(karadaData[index][14 *3 +1 ]*0/5 + karadaData[index][12 *3  + 1]*5 /5) * imageHeight + leftArm0Width /2  ;
            kData[3*3 +0] =(karadaData[index][14 *3 ] *4/6 + karadaData[index][12 *3 ] *2 /6 ) * imageWidth -leftArm1Width /2  ;
            kData[3*3 +1] =(karadaData[index][14 *3 +1 ]*4/6 + karadaData[index][12 *3  + 1]* 2 /6 )  * imageHeight-leftArm1Width /2  ;
            kData[4*3 +0] =(karadaData[index][14 *3 ] *4/6 + karadaData[index][12 *3 ] * 2 /6 ) * imageWidth  +leftArm1Width /2 ;
            kData[4*3 +1] =(karadaData[index][14 *3 +1 ]*4/6 + karadaData[index][12 *3  + 1] *2 /6 ) * imageHeight +leftArm1Width /2 ;

//            kData[5 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[5 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[6 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[6 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[7*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[7*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[8*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[8*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[9*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[9*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;
//
//            kData[10 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[10 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[11 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[11 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[12*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[12*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[13*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[13*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[14*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[14*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;
//
//            kData[15 *3 + 0] = karadaData[index][127 *3 ] * imageWidth ;
//            kData[15 *3 + 1] = karadaData[index] [127 *3  + 1 ] * imageHeight ;
//            kData[16 *3 + 0] =karadaData[index][152 *3 ]  * imageWidth ;
//            kData[16 *3 + 1] =karadaData[index][152 *3 +1]  * imageHeight ;
//            kData[17*3 +0] =karadaData[index][356 *3 ] * imageWidth ;
//            kData[17*3 +1] =karadaData[index][356 *3 +1] * imageHeight ;
//            kData[18*3 +0] =karadaData[index][58 *3 ] * imageWidth ;
//            kData[18*3 +1] =karadaData[index][58 *3 +1 ]  * imageHeight ;
//            kData[19*3 +0] =karadaData[index][367 *3 ] * imageWidth  ;
//            kData[19*3 +1] =karadaData[index][367 *3 +1 ] * imageHeight ;
        }

        }
	}
    return true;
}


void MyGLRenderContext::SetParamsFloat(int paramType, float value0, float value1) {
	LOGCATE("MyGLRenderContext::SetParamsFloat paramType=%d, value0=%f, value1=%f", paramType, value0, value1);
	if(m_curApp)
	{
		switch (paramType)
		{
			case TYPE_KEY_SET_TOUCH_LOC:
				m_curApp->SetTouchLocation(value0, value1);
				break;
			case TYPE_SET_GRAVITY_XY:
                m_curApp->SetGravityXY(value0, value1);
				break;
			default:
				break;

		}
	}

}

void MyGLRenderContext::SetParamsShortArr(short *const pShortArr, int arrSize) {
	LOGCATE("MyGLRenderContext::SetParamsShortArr pShortArr=%p, arrSize=%d, pShortArr[0]=%d", pShortArr, arrSize, pShortArr[0]);
	if(m_curApp)
	{
		m_curApp->LoadShortArrData(pShortArr, arrSize);
	}

}

void MyGLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	LOGCATE("MyGLRenderContext::UpdateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]", rotateX, rotateY, scaleX, scaleY);
	if (m_curApp)
	{
		m_curApp->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
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

	if (m_curApp)
	{
		m_curApp->LoadMultiImageWithIndex(index, &nativeImage);
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

	if (m_curApp)
	{
		m_curApp->LoadImage(&nativeImage);
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

//    if (m_curApp)
//    {
//        m_curApp->LoadImage(&nativeImage);
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

	if (m_preApp)
	{
		m_preApp->Destroy();
		delete m_preApp;
        m_preApp = nullptr;
	}

	if (m_curApp)
	{
		m_curApp->Init();
		m_curApp->Draw(m_ScreenW, m_ScreenH);
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



