/*+----------------------------------------------------------------+
  |	Title:		DrawConst.cpp [共通環境]
  |	Comment:	描画定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DrawConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//------------------------
// ラベル
//------------------------
const char* g_pArrLabelDrawType[] = {
    "RECT",
    "TEX",
    "SPR",
    "STR",
    "NUM",
    "POLY",
    
    NULL,
};

const char* g_pArrLabelDrawShader[] = {
    "NORMAL",
    "RGBA",
    "ALPHA",
    
    NULL,
};

const char* g_pArrLabelDrawBlend[] = {
    "OFF",
    "ALPHA",
    "ALPHA_ONE",
    "ADD",
    "MULTI",
    
    NULL,
};

const char* g_pArrLabelDrawAlignX[] = {
    "L",
    "C",
    "R",
    
    NULL,
};

const char* g_pArrLabelDrawAlignY[] = {
    "T",
    "M",
    "B",
    
    NULL,
};

const char* g_pArrLabelDrawSetType[] = {
    "P8",
    "P8_ALPHA",
    "ALPHA",
    "RGBA",
    
    NULL,
};

const char* g_pArrLabelLayoutDir[] = {
    "N",
    "F",
    "B",
    
    NULL,
};

/*+----------------------------------------------------------------+
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
