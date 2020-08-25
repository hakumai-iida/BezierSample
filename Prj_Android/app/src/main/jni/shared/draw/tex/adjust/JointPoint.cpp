/*+----------------------------------------------------------------+
  |	Title:		JointPoint.cpp [共通環境]
  | Comment:    結合点（※ベジェ描画時のワーク）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "JointPoint.hpp"

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
// 結合ラベル
const char* g_pArrLabelJointPoint[] = {
    // [UP_BODY]：腹から上半身へのつなぎ
    "UP_BODY_0_FROM",   "UP_BODY_0_TO",
    "UP_BODY_1_FROM",   "UP_BODY_1_TO",
    
    // [LOW_BODY]：腹から下半身へのつなぎ
    "LOW_BODY_0_FROM",  "LOW_BODY_0_TO",
    "LOW_BODY_1_FROM",  "LOW_BODY_1_TO",
    
    // [NECK]：上半身から首へのつなぎ
    "NECK_0_FROM",      "NECK_0_TO",
    "NECK_1_FROM",      "NECK_1_TO",
    
    // [SHOLDER]：上半身から腕へのつなぎ
    "SHOLDER_L0_FROM",  "SHOLDER_L0_TO",
    "SHOLDER_L1_FROM",  "SHOLDER_L1_TO",
    "SHOLDER_R0_FROM",  "SHOLDER_R0_TO",
    "SHOLDER_R1_FROM",  "SHOLDER_R1_TO",

    // [ELBOW]：腕から肘下へのつなぎ
    "ELBOW_L0_FROM",    "ELBOW_L0_TO",
    "ELBOW_L1_FROM",    "ELBOW_L1_TO",
    "ELBOW_R0_FROM",    "ELBOW_R0_TO",
    "ELBOW_R1_FROM",    "ELBOW_R1_TO",
    
    // [WRIST]：肘下から手へのつなぎ
    "WRIST_L0_FROM",    "WRIST_L0_TO",
    "WRIST_L1_FROM",    "WRIST_L1_TO",
    "WRIST_R0_FROM",    "WRIST_R0_TO",
    "WRIST_R1_FROM",    "WRIST_R1_TO",
    
    // [LEG]：下半身から脚へのつなぎ
    "LEG_L0_FROM",       "LEG_L0_TO",
    "LEG_L1_FROM",       "LEG_L1_TO",
    "LEG_R0_FROM",       "LEG_R0_TO",
    "LEG_R1_FROM",       "LEG_R1_TO",
   
    // [KNEE]: 脚から膝下へのつなぎ
    "KNEE_L0_FROM",      "KNEE_L0_TO",
    "KNEE_L1_FROM",      "KNEE_L1_TO",
    "KNEE_R0_FROM",      "KNEE_R0_TO",
    "KNEE_R1_FROM",      "KNEE_R1_TO",

    // [ANKLE]：脚から足先へのつなぎ
    "ANKLE_L0_FROM",    "ANKLE_L0_TO",
    "ANKLE_L1_FROM",    "ANKLE_L1_TO",
    "ANKLE_R0_FROM",    "ANKLE_R0_TO",
    "ANKLE_R1_FROM",    "ANKLE_R1_TO",
    
    // [BD_OPTION A]：汎用枠：Ａ
    "BD_OPTION_A_L0_FROM",   "BD_OPTION_A_L0_TO",
    "BD_OPTION_A_L1_FROM",   "BD_OPTION_A_L1_TO",

    "BD_OPTION_A_R0_FROM",   "BD_OPTION_A_R0_TO",
    "BD_OPTION_A_R1_FROM",   "BD_OPTION_A_R1_TO",

    // [BD_OPTION B]：汎用枠：Ｂ
    "BD_OPTION_B_L0_FROM",   "BD_OPTION_B_L0_TO",
    "BD_OPTION_B_L1_FROM",   "BD_OPTION_B_L1_TO",

    "BD_OPTION_B_R0_FROM",   "BD_OPTION_B_R0_TO",
    "BD_OPTION_B_R1_FROM",   "BD_OPTION_B_R1_TO",

    NULL,
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------------
// コンストラクタ
//-------------------------
CJointPoint::CJointPoint( void ){
    clear();
}

//-------------------------
// クリア
//-------------------------
void CJointPoint::clear( void ){
    for( int i=0; i<eJOINT_POINT_MAX; i++ ){
        CLEAR_JOINT_POINT_CELL( &m_stArrCell[i] );
    }
}

//-----------------------------------
// 登録情報の取得（※無効時は[NULL]を返す)
//-----------------------------------
stJOINT_POINT_CELL* CJointPoint::getJointPoint( eJOINT_POINT id ){
    if( ! IS_JOINT_POINT_VALID( id ) ){
        //LOGE( "@ CJointPoint::getJointPoint: INVALID: id=%d\n", id );
        return( NULL );
    }
    
    return( &m_stArrCell[id] );
}

//-------------------------
// 登録
//-------------------------
void CJointPoint::registJointPoint( eJOINT_POINT id, int rateX, int rateY, int rateX0, int rateY0, int ofsRX, int ofsRY,
                                    float scaleAbs, float rot0, bool isFlipH, bool isFlipV, bool isTo ){
    stJOINT_POINT_CELL* pCell = getJointPoint( id );

    // 無効は無視
    if( pCell == NULL ){
        LOGE( "@ CJointPoint::registJointPoint0: INVALID: id=%d, x=%d, y=%d, x0=%d, y0=%d, ox=%d, oy=%d, "
              "sAbs=%f, rot=%f, isH=%d, isV=%d, isTo=%d\n",
              id, rateX, rateY, rateX0, rateY0, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, isTo );
    }

    // クリア（一応）
    CLEAR_JOINT_POINT_CELL( pCell );
    
    // 値の設定
    pCell->bValid = true;
    pCell->bTo = isTo;
    pCell->bFlipH = isFlipH;
    pCell->bFlipV = isFlipV;
    pCell->nRateX = rateX;
    pCell->nRateY = rateY;
    pCell->nRateX0 = rateX0;
    pCell->nRateY0 = rateY0;
    pCell->nRateOfsX = ofsRX;
    pCell->nRateOfsY = ofsRY;
    pCell->fScaleAbs = scaleAbs;
    pCell->fRot0 = rot0;
}

//-------------------------
// 方向線調整値の設定
//-------------------------
void CJointPoint::setDirRateXY( eJOINT_POINT id, float openDirX, float openDirY, float closeDirX, float closeDirY  ){
    stJOINT_POINT_CELL* pCell = getJointPoint( id );
    
    // 無効は無視
    if( pCell == NULL ){
        LOGE( "@ CJointPoint::setDirRateXY: INVALID: id=%d, openDirX=%f, openDirY=%f, closeDirX=%f, closeDirY=%f\n",
              id, openDirX, openDirY, closeDirX, closeDirY );
    }
    
    pCell->openDirX = openDirX;
    pCell->openDirY = openDirY;
    pCell->closeDirX = closeDirX;
    pCell->closeDirY = closeDirY;
}
