/*+----------------------------------------------------------------+
  | Title:      StrokeConst.hpp [共通環境]
  | Comment:    ストローク定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "StrokeConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//------------------
// ストロークラベル
//------------------
const char* g_pArrLabelStrokeType[] = {
    "OFF",
    "SQUARE",
    "CIRCLE",
    
    NULL,
};

//------------------
// フリルラベル
//------------------
const char* g_pArrLabelStrokeFrill[] = {
    "TEST_CIRCLE",
    "TEST_TRIANGLE",
    "TEST_SQUARE",
    
    "FLUFFY_SQUARE_MAIN",
    "FLUFFY_SQUARE_SUB",

    NULL,
};

//------------------
// フック対象ラベル
//------------------
const char* g_pArrLabelStrokeHookTarget[] = {
    // テンポラリ
    "TEMP_A",    "TEMP_B",    "TEMP_C",    "TEMP_D",
    "TEMP_E",    "TEMP_F",    "TEMP_G",    "TEMP_H",
    "TEMP_I",    "TEMP_J",    "TEMP_K",    "TEMP_L",
    "TEMP_M",    "TEMP_N",    "TEMP_O",    "TEMP_P",
    "TEMP_Q",    "TEMP_R",    "TEMP_S",    "TEMP_T",
    "TEMP_U",    "TEMP_V",    "TEMP_W",    "TEMP_X",
    "TEMP_Y",    "TEMP_Z",
    
    // パーツ
    "PART_A",   "PART_B",   "PART_C",   "PART_D",
    "PART_E",   "PART_F",   "PART_G",   "PART_H",
    "PART_I",   "PART_J",   "PART_K",   "PART_L",
    "PART_M",   "PART_N",   "PART_O",   "PART_P",
    "PART_Q",   "PART_R",   "PART_S",   "PART_T",
    "PART_U",   "PART_V",   "PART_W",   "PART_X",
    "PART_Y",   "PART_Z",

    // 特定
    "HEAD_TOP_L",             "HEAD_TOP_R",
    "HEAD_SIDE_L",            "HEAD_SIDE_R",
    "HAIR_LINE_L",            "HAIR_LINE_R",
    "NECK_TO_FACE_L",         "NECK_TO_FACE_R",
    "ARM_TO_BACK_L",          "ARM_TO_BACK_R",
    
    // 上半身
    "UP_A",    "UP_B",    "UP_C",    "UP_D",
    "UP_E",    "UP_F",    "UP_G",    "UP_H",
    "UP_I",    "UP_J",    "UP_K",    "UP_L",
    "UP_M",    "UP_N",    "UP_O",    "UP_P",
    "UP_Q",    "UP_R",    "UP_S",    "UP_T",
    "UP_U",    "UP_V",    "UP_W",    "UP_X",
    "UP_Y",    "UP_Z",

    // 下半身
    "LOW_A",   "LOW_B",   "LOW_C",   "LOW_D",
    "LOW_E",   "LOW_F",   "LOW_G",   "LOW_H",
    "LOW_I",   "LOW_J",   "LOW_K",   "LOW_L",
    "LOW_M",   "LOW_N",   "LOW_O",   "LOW_P",
    "LOW_Q",   "LOW_R",   "LOW_S",   "LOW_T",
    "LOW_U",   "LOW_V",   "LOW_W",   "LOW_X",
    "LOW_Y",   "LOW_Z",

    // スーツ
    "SUIT_A",   "SUIT_B",   "SUIT_C",   "SUIT_D",
    "SUIT_E",   "SUIT_F",   "SUIT_G",   "SUIT_H",
    "SUIT_I",   "SUIT_J",   "SUIT_K",   "SUIT_L",
    "SUIT_M",   "SUIT_N",   "SUIT_O",   "SUIT_P",
    "SUIT_Q",   "SUIT_R",   "SUIT_S",   "SUIT_T",
    "SUIT_U",   "SUIT_V",   "SUIT_W",   "SUIT_X",
    "SUIT_Y",   "SUIT_Z",

    // 腕
    "ARM_A_0",  "ARM_A_1",  "ARM_B_0",  "ARM_B_1",
    "ARM_C_0",  "ARM_C_1",  "ARM_D_0",  "ARM_D_1",
    "ARM_E_0",  "ARM_E_1",  "ARM_F_0",  "ARM_F_1",
    "ARM_G_0",  "ARM_G_1",  "ARM_H_0",  "ARM_H_1",
    "ARM_I_0",  "ARM_I_1",  "ARM_J_0",  "ARM_J_1",
    "ARM_K_0",  "ARM_K_1",  "ARM_L_0",  "ARM_L_1",
    "ARM_M_0",  "ARM_M_1",  "ARM_N_0",  "ARM_N_1",
    "ARM_O_0",  "ARM_O_1",  "ARM_P_0",  "ARM_P_1",
    "ARM_Q_0",  "ARM_Q_1",  "ARM_R_0",  "ARM_R_1",
    "ARM_S_0",  "ARM_S_1",  "ARM_T_0",  "ARM_T_1",
    "ARM_U_0",  "ARM_U_1",  "ARM_V_0",  "ARM_V_1",
    "ARM_W_0",  "ARM_W_1",  "ARM_X_0",  "ARM_X_1",
    "ARM_Y_0",  "ARM_Y_1",  "ARM_Z_0",  "ARM_Z_1",

    // 脚
    "LEG_A_0",  "LEG_A_1",  "LEG_B_0",  "LEG_B_1",
    "LEG_C_0",  "LEG_C_1",  "LEG_D_0",  "LEG_D_1",
    "LEG_E_0",  "LEG_E_1",  "LEG_F_0",  "LEG_F_1",
    "LEG_G_0",  "LEG_G_1",  "LEG_H_0",  "LEG_H_1",
    "LEG_I_0",  "LEG_I_1",  "LEG_J_0",  "LEG_J_1",
    "LEG_K_0",  "LEG_K_1",  "LEG_L_0",  "LEG_L_1",
    "LEG_M_0",  "LEG_M_1",  "LEG_N_0",  "LEG_N_1",
    "LEG_O_0",  "LEG_O_1",  "LEG_P_0",  "LEG_P_1",
    "LEG_Q_0",  "LEG_Q_1",  "LEG_R_0",  "LEG_R_1",
    "LEG_S_0",  "LEG_S_1",  "LEG_T_0",  "LEG_T_1",
    "LEG_U_0",  "LEG_U_1",  "LEG_V_0",  "LEG_V_1",
    "LEG_W_0",  "LEG_W_1",  "LEG_X_0",  "LEG_X_1",
    "LEG_Y_0",  "LEG_Y_1",  "LEG_Z_0",  "LEG_Z_1",

    // フリル
    "FRILL_TEMP_A",     "FRILL_TEMP_B",
    "FRILL_CONNECT_0",  "FRILL_CONNECT_1",

    NULL,
};

//------------------
// タッチ対象ラベル
//------------------
const char* g_pArrLabelStrokeTouchTarget[] = {
    // 一時枠
    "TEMP_A",    "TEMP_B",    "TEMP_C",    "TEMP_D",
    "TEMP_E",    "TEMP_F",    "TEMP_G",    "TEMP_H",

    // 顔／頭
    "FACE_0",    "FACE_1",
    "HEAD_0",    "HEAD_1",

    // 上半身
    "UP_A",    "UP_B",    "UP_C",    "UP_D",
    "UP_E",    "UP_F",    "UP_G",    "UP_H",

    // 下半身
    "LOW_A",    "LOW_B",    "LOW_C",    "LOW_D",
    "LOW_E",    "LOW_F",    "LOW_G",    "LOW_H",

    // スーツ
    "SUIT_A",    "SUIT_B",    "SUIT_C",    "SUIT_D",
    "SUIT_E",    "SUIT_F",    "SUIT_G",    "SUIT_H",

    // 腕
    "ARM_A_0",  "ARM_A_1",    "ARM_B_0",  "ARM_B_1",
    "ARM_C_0",  "ARM_C_1",    "ARM_D_0",  "ARM_D_1",
    "ARM_E_0",  "ARM_E_1",    "ARM_F_0",  "ARM_F_1",
    "ARM_G_0",  "ARM_G_1",    "ARM_H_0",  "ARM_H_1",

    // 脚
    "LEG_A_0",  "LEG_A_1",    "LEG_B_0",  "LEG_B_1",
    "LEG_C_0",  "LEG_C_1",    "LEG_D_0",  "LEG_D_1",
    "LEG_E_0",  "LEG_E_1",    "LEG_F_0",  "LEG_F_1",
    "LEG_G_0",  "LEG_G_1",    "LEG_H_0",  "LEG_H_1",
    
    NULL,
};

//------------------
// ガイド対象ラベル
//------------------
const char* g_pArrLabelStrokeGuideTarget[] = {
    "TEMP_A",    "TEMP_B",    "TEMP_C",    "TEMP_D",

    "FACE_A",    "FACE_B",    "FACE_C",    "FACE_D",
    "HEAD_A",    "HEAD_B",    "HEAD_C",    "HEAD_D",
    "BODY_A",    "BODY_B",    "BODY_C",    "BODY_D",
    "SUIT_A",    "SUIT_B",    "SUIT_C",    "SUIT_D",

    "ARM_A_0",   "ARM_A_1",   "ARM_B_0",   "ARM_B_1",
    "ARM_C_0",   "ARM_C_1",   "ARM_D_0",   "ARM_D_1",

    "LEG_A_0",   "LEG_A_1",   "LEG_B_0",   "LEG_B_1",
    "LEG_C_0",   "LEG_C_1",   "LEG_D_0",   "LEG_D_1",

    NULL,
};
    
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
