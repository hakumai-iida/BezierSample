/*+----------------------------------------------------------------+
  |	Title:		BmpGeneratorForAngle.cpp [共通環境]
  |	Comment:	Bmp生成：傾き関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenerator.hpp"
#include "draw/tex/bezier/BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------------------------------------------
// [ANGLE] アングル平面の設定
//----------------------------------------------------------------------
// [baseX/baseY]は親から呼び出された際の座標（※フリップ／スケール／回転処理済みの値）
//----------------------------------------------------------------------
#define ANGLE_ADJUST    1.0f
bool CBmpGenerator::SetAnglePlaneInfo( eBD_SLOT slot, int slotIndex, float baseX, float baseY,
                                       bool isFlipH, bool isFlipV, float rot, float scale ){

    float w = s_nTexW;
    float h = s_nTexH;

    switch( slot ){
    // あたま
    case eBD_SLOT_HdBase:
        w = 0.6f*s_nTexW * ANGLE_ADJUST;
        h = 0.8f*s_nTexH * ANGLE_ADJUST;
        break;

    // かお
    case eBD_SLOT_FcBase:
        w = 0.55f*s_nTexW * ANGLE_ADJUST;
        h = 0.75f*s_nTexH * ANGLE_ADJUST;
        break;

    // からだ
    case eBD_SLOT_BlBase:
        w = 0.7f*s_nTexW * ANGLE_ADJUST;
        h = 0.9*s_nTexH * ANGLE_ADJUST;
        break;
            
    // その他はデフォルト
    default:
        break;
    }

    // 登録
    return( s_oAnglePlane.registPlaneInfoForSlot( slot, slotIndex, w, h, baseX, baseY, isFlipH, isFlipV, rot, scale ) );
}
