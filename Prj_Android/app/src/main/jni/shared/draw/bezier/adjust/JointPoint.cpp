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
    // [UP_BODY]
    "UP_BODY_0_FROM",   "UP_BODY_0_TO",
    "UP_BODY_1_FROM",   "UP_BODY_1_TO",
    
    // [LOW_BODY]
    "LOW_BODY_0_FROM",  "LOW_BODY_0_TO",
    "LOW_BODY_1_FROM",  "LOW_BODY_1_TO",
    
    // [NECK]
    "NECK_0_FROM",      "NECK_0_TO",
    "NECK_1_FROM",      "NECK_1_TO",
    
    // [HEAD_TOP]
    "HEAD_TOP_0_FROM",  "HEAD_TOP_0_TO",
    "HEAD_TOP_1_FROM",  "HEAD_TOP_1_TO",

    // [TEMPLE]
    "TEMPLE_0_FROM",    "TEMPLE_0_TO",
    "TEMPLE_1_FROM",    "TEMPLE_1_TO",

    // [SHOLDER]
    "SHOLDER_L0_FROM",  "SHOLDER_L0_TO",
    "SHOLDER_L1_FROM",  "SHOLDER_L1_TO",
    "SHOLDER_R0_FROM",  "SHOLDER_R0_TO",
    "SHOLDER_R1_FROM",  "SHOLDER_R1_TO",

    // [ELBOW]
    "ELBOW_L0_FROM",    "ELBOW_L0_TO",
    "ELBOW_L1_FROM",    "ELBOW_L1_TO",
    "ELBOW_R0_FROM",    "ELBOW_R0_TO",
    "ELBOW_R1_FROM",    "ELBOW_R1_TO",
    
    // [WRIST]
    "WRIST_L0_FROM",    "WRIST_L0_TO",
    "WRIST_L1_FROM",    "WRIST_L1_TO",
    "WRIST_R0_FROM",    "WRIST_R0_TO",
    "WRIST_R1_FROM",    "WRIST_R1_TO",
    
    // [LEG]
    "LEG_L0_FROM",       "LEG_L0_TO",
    "LEG_L1_FROM",       "LEG_L1_TO",
    "LEG_R0_FROM",       "LEG_R0_TO",
    "LEG_R1_FROM",       "LEG_R1_TO",
   
    // [KNEE]
    "KNEE_L0_FROM",      "KNEE_L0_TO",
    "KNEE_L1_FROM",      "KNEE_L1_TO",
    "KNEE_R0_FROM",      "KNEE_R0_TO",
    "KNEE_R1_FROM",      "KNEE_R1_TO",

    // [ANKLE]
    "ANKLE_L0_FROM",    "ANKLE_L0_TO",
    "ANKLE_L1_FROM",    "ANKLE_L1_TO",
    "ANKLE_R0_FROM",    "ANKLE_R0_TO",
    "ANKLE_R1_FROM",    "ANKLE_R1_TO",
    
    // [BD_OPTION A]
    "BD_OPTION_A_L0_FROM",   "BD_OPTION_A_L0_TO",
    "BD_OPTION_A_L1_FROM",   "BD_OPTION_A_L1_TO",

    "BD_OPTION_A_R0_FROM",   "BD_OPTION_A_R0_TO",
    "BD_OPTION_A_R1_FROM",   "BD_OPTION_A_R1_TO",

    // [BD_OPTION B]
    "BD_OPTION_B_L0_FROM",   "BD_OPTION_B_L0_TO",
    "BD_OPTION_B_L1_FROM",   "BD_OPTION_B_L1_TO",

    "BD_OPTION_B_R0_FROM",   "BD_OPTION_B_R0_TO",
    "BD_OPTION_B_R1_FROM",   "BD_OPTION_B_R1_TO",

    NULL,
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------
// ID調整
//----------------
int CJointPoint::AdjustJointPoint( eJOINT_POINT target, int slotIndex ){
    int idFixed = target;
    
    // スロットインデックスにより参照先を補正
    if( IS_JOINT_POINT_VALID( target ) && IS_BD_SLOT_INDEX_VALID( slotIndex ) ){
        if( slotIndex > 0 ){
            idFixed = target + JOINT_POINT_OFS_FOR_SLOT_INDEX;
        }
    }
    
    // 無効は無視
    if( ! IS_JOINT_POINT_FIXED_VALID( idFixed ) ){
        return( -1 );
    }

    return( idFixed );
}

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
    for( int i=0; i<eJOINT_POINT_TOTAL_MAX; i++ ){
        CLEAR_JOINT_POINT_CELL( &m_stArrCell[i] );
    }
}

//-----------------------------------
// 登録情報の取得（※無効時は[NULL]を返す)
//-----------------------------------
stJOINT_POINT_CELL* CJointPoint::getJointPoint( int idFixed ){
    if( ! IS_JOINT_POINT_FIXED_VALID( idFixed ) ){
        //LOGE( "@ CJointPoint::getJointPoint: INVALID: idFixed=%d\n", idFixed );
        return( NULL );
    }
    
    return( &m_stArrCell[idFixed] );
}

//-------------------------
// 登録
//-------------------------
void CJointPoint::registJointPoint( int idFixed, int rateX, int rateY, int ofsRX, int ofsRY,
                                    float scaleAbs, float rot0, bool isFlipH, bool isFlipV, bool isTo ){
    stJOINT_POINT_CELL* pCell = getJointPoint( idFixed );

    // 無効は無視
    if( pCell == NULL ){
        LOGE( "@ CJointPoint::registJointPoint0: INVALID: "
              "idFixed=%d, x=%d, y=%d, ox=%d, oy=%d, sAbs=%f, rot=%f, isH=%d, isV=%d, isTo=%d\n",
              idFixed, rateX, rateY, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, isTo );
        return;
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
    pCell->nRateOfsX = ofsRX;
    pCell->nRateOfsY = ofsRY;
    
    pCell->fScaleAbs = scaleAbs;
    pCell->fRot0 = rot0;
}

//-------------------------
// 方向線調整値の設定
//-------------------------
void CJointPoint::setDirRateXY( int idFixed, float openDirX, float openDirY, float closeDirX, float closeDirY ){
    stJOINT_POINT_CELL* pCell = getJointPoint( idFixed );
    
    // 無効は無視
    if( pCell == NULL ){
        LOGE( "@ CJointPoint::setDirRateXY: INVALID: id=%d, openDirX=%f, openDirY=%f, closeDirX=%f, closeDirY=%f\n",
              idFixed, openDirX, openDirY, closeDirX, closeDirY );
    }
    
    pCell->openDirX = openDirX;
    pCell->openDirY = openDirY;
    pCell->closeDirX = closeDirX;
    pCell->closeDirY = closeDirY;
}
