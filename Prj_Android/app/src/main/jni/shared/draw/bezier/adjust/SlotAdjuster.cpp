/*+----------------------------------------------------------------+
  |	Title:		SlotAdjuster.cpp [共通環境]
  | Comment:    スロット調整（※外部指定のデータによりスロットを調整）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SlotAdjuster.hpp"

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
//-------------------------
// コンストラクタ
//-------------------------
CSlotAdjuster::CSlotAdjuster( void ){
	clear();
}

//-------------------------
// クリア（※データの破棄）
//-------------------------
void CSlotAdjuster::clear( void ){
    // for T
    m_nMinRateForT = 0;
    m_nMaxRateForT = 0;

    // for S
    m_nMinRateForS = BEZIER_SCALE_RATE;
    m_nMaxRateForS = BEZIER_SCALE_RATE;

    // joint
    m_eJointPointIdForFrom = eJOINT_POINT_INVALID;
    m_eJointPointIdForTo = eJOINT_POINT_INVALID;
    
    m_nJointRateRotR = 0;
    m_nJointMinRateOfsRForH = 0;
    m_nJointMaxRateOfsRForH = 0;
    m_nJointRateRotOfs = 0;
    m_nJointRateOfsForRotCenter = 0;

    m_nJointOpenFromRateDirX_p = 0;
    m_nJointOpenFromRateDirY_p = 0;
    m_nJointCloseFromRateDirX_p = 0;
    m_nJointCloseFromRateDirY_p = 0;
    m_nJointOpenToRateDirX_p = 0;
    m_nJointOpenToRateDirY_p = 0;
    m_nJointCloseToRateDirX_p = 0;
    m_nJointCloseToRateDirY_p = 0;
    
    m_nJointOpenFromRateDirX_m = 0;
    m_nJointOpenFromRateDirY_m = 0;
    m_nJointCloseFromRateDirX_m = 0;
    m_nJointCloseFromRateDirY_m = 0;
    m_nJointOpenToRateDirX_m = 0;
    m_nJointOpenToRateDirY_m = 0;
    m_nJointCloseToRateDirX_m = 0;
    m_nJointCloseToRateDirY_m = 0;

    m_nCoverRateOfsX = 0;
    m_nCoverRateOfsY = 0;
    m_nCoverRateScaleX = BEZIER_SCALE_RATE;
    m_nCoverRateScaleY = BEZIER_SCALE_RATE;
    m_nCoverRateRot = 0;

}

//--------------------
// コピー
//--------------------
void CSlotAdjuster::copy( CSlotAdjuster* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }

    // for T
    m_nMinRateForT = pData->m_nMinRateForT;
    m_nMaxRateForT = pData->m_nMaxRateForT;

    // for S
    m_nMinRateForS = pData->m_nMinRateForS;
    m_nMaxRateForS = pData->m_nMaxRateForS;

    // joint
    m_eJointPointIdForFrom = pData->m_eJointPointIdForFrom;
    m_eJointPointIdForTo = pData->m_eJointPointIdForTo;
    
    m_nJointRateRotR = pData->m_nJointRateRotR;
    m_nJointMinRateOfsRForH = pData->m_nJointMinRateOfsRForH;
    m_nJointMaxRateOfsRForH = pData->m_nJointMaxRateOfsRForH;
    m_nJointRateRotOfs = pData->m_nJointRateRotOfs;
    m_nJointRateOfsForRotCenter = pData->m_nJointRateOfsForRotCenter;
    
    m_nJointOpenFromRateDirX_p = pData->m_nJointOpenFromRateDirX_p;
    m_nJointOpenFromRateDirY_p = pData->m_nJointOpenFromRateDirY_p;
    m_nJointCloseFromRateDirX_p = pData->m_nJointCloseFromRateDirX_p;
    m_nJointCloseFromRateDirY_p = pData->m_nJointCloseFromRateDirY_p;
    m_nJointOpenToRateDirX_p = pData->m_nJointOpenToRateDirX_p;
    m_nJointOpenToRateDirY_p = pData->m_nJointOpenToRateDirY_p;
    m_nJointCloseToRateDirX_p = pData->m_nJointCloseToRateDirX_p;
    m_nJointCloseToRateDirY_p = pData->m_nJointCloseToRateDirY_p;
    
    m_nJointOpenFromRateDirX_m = pData->m_nJointOpenFromRateDirX_m;
    m_nJointOpenFromRateDirY_m = pData->m_nJointOpenFromRateDirY_m;
    m_nJointCloseFromRateDirX_m = pData->m_nJointCloseFromRateDirX_m;
    m_nJointCloseFromRateDirY_m = pData->m_nJointCloseFromRateDirY_m;
    m_nJointOpenToRateDirX_m = pData->m_nJointOpenToRateDirX_m;
    m_nJointOpenToRateDirY_m = pData->m_nJointOpenToRateDirY_m;
    m_nJointCloseToRateDirX_m = pData->m_nJointCloseToRateDirX_m;
    m_nJointCloseToRateDirY_m = pData->m_nJointCloseToRateDirY_m;
    
    m_nCoverRateOfsX = pData->m_nCoverRateOfsX;
    m_nCoverRateOfsY = pData->m_nCoverRateOfsY;
    m_nCoverRateScaleX = pData->m_nCoverRateScaleX;
    m_nCoverRateScaleY = pData->m_nCoverRateScaleY;
    m_nCoverRateRot = pData->m_nCoverRateRot;
}

//---------------------------
// 読み込み
//---------------------------
void CSlotAdjuster::read( CInputBuffer* pIB ){
    // for T
    m_nMinRateForT = pIB->readInt16();
    m_nMaxRateForT = pIB->readInt16();

    // for S
    m_nMinRateForS = pIB->readInt16();
    m_nMaxRateForS = pIB->readInt16();

    // joint
    m_eJointPointIdForFrom = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );
    m_eJointPointIdForTo = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );

    m_nJointRateRotR = pIB->readInt16();
    m_nJointMinRateOfsRForH = pIB->readInt16();
    m_nJointMaxRateOfsRForH = pIB->readInt16();
    m_nJointRateRotOfs = pIB->readInt16();
    m_nJointRateOfsForRotCenter = pIB->readInt16();

    m_nJointOpenFromRateDirX_p = pIB->readInt16();
    m_nJointOpenFromRateDirY_p = pIB->readInt16();
    m_nJointCloseFromRateDirX_p = pIB->readInt16();
    m_nJointCloseFromRateDirY_p = pIB->readInt16();
    m_nJointOpenToRateDirX_p = pIB->readInt16();
    m_nJointOpenToRateDirY_p = pIB->readInt16();
    m_nJointCloseToRateDirX_p = pIB->readInt16();
    m_nJointCloseToRateDirY_p = pIB->readInt16();

    m_nJointOpenFromRateDirX_m = pIB->readInt16();
    m_nJointOpenFromRateDirY_m = pIB->readInt16();
    m_nJointCloseFromRateDirX_m = pIB->readInt16();
    m_nJointCloseFromRateDirY_m = pIB->readInt16();
    m_nJointOpenToRateDirX_m = pIB->readInt16();
    m_nJointOpenToRateDirY_m = pIB->readInt16();
    m_nJointCloseToRateDirX_m = pIB->readInt16();
    m_nJointCloseToRateDirY_m = pIB->readInt16();

    m_nCoverRateOfsX = pIB->readInt16();
    m_nCoverRateOfsY = pIB->readInt16();
    m_nCoverRateScaleX = pIB->readInt16();
    m_nCoverRateScaleY = pIB->readInt16();
    m_nCoverRateRot = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CSlotAdjuster::write( COutputBuffer* pOB ){
    // for T
    pOB->writeInt16( (int16)m_nMinRateForT );
    pOB->writeInt16( (int16)m_nMaxRateForT );

    // for S
    pOB->writeInt16( (int16)m_nMinRateForS );
    pOB->writeInt16( (int16)m_nMaxRateForS );

    // joint
    CEnum::WriteEnum( pOB, m_eJointPointIdForFrom, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    CEnum::WriteEnum( pOB, m_eJointPointIdForTo, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    
    pOB->writeInt16( (int16)m_nJointRateRotR );
    pOB->writeInt16( (int16)m_nJointMinRateOfsRForH );
    pOB->writeInt16( (int16)m_nJointMaxRateOfsRForH );
    pOB->writeInt16( (int16)m_nJointRateRotOfs );
    pOB->writeInt16( (int16)m_nJointRateOfsForRotCenter );
    
    pOB->writeInt16( (int16)m_nJointOpenFromRateDirX_p );
    pOB->writeInt16( (int16)m_nJointOpenFromRateDirY_p );
    pOB->writeInt16( (int16)m_nJointCloseFromRateDirX_p );
    pOB->writeInt16( (int16)m_nJointCloseFromRateDirY_p );
    pOB->writeInt16( (int16)m_nJointOpenToRateDirX_p );
    pOB->writeInt16( (int16)m_nJointOpenToRateDirY_p );
    pOB->writeInt16( (int16)m_nJointCloseToRateDirX_p );
    pOB->writeInt16( (int16)m_nJointCloseToRateDirY_p );

    pOB->writeInt16( (int16)m_nJointOpenFromRateDirX_m );
    pOB->writeInt16( (int16)m_nJointOpenFromRateDirY_m );
    pOB->writeInt16( (int16)m_nJointCloseFromRateDirX_m );
    pOB->writeInt16( (int16)m_nJointCloseFromRateDirY_m );
    pOB->writeInt16( (int16)m_nJointOpenToRateDirX_m );
    pOB->writeInt16( (int16)m_nJointOpenToRateDirY_m );
    pOB->writeInt16( (int16)m_nJointCloseToRateDirX_m );
    pOB->writeInt16( (int16)m_nJointCloseToRateDirY_m );
    
    pOB->writeInt16( (short)m_nCoverRateOfsX );
    pOB->writeInt16( (short)m_nCoverRateOfsY );
    pOB->writeInt16( (short)m_nCoverRateScaleX );
    pOB->writeInt16( (short)m_nCoverRateScaleY );
    pOB->writeInt16( (short)m_nCoverRateRot );
}

//-----------------------------
// 結合ポイントIDの調整（※接続元用）
//-----------------------------
int CSlotAdjuster::fixJointPointIdForFrom( int slotIndex ){
    return( CJointPoint::AdjustJointPoint( m_eJointPointIdForFrom, slotIndex ) );
}

//-----------------------------
// 結合ポイントIDの調整（※接続元用）
//-----------------------------
int CSlotAdjuster::fixJointPointIdForTo( int slotIndex ){
    return( CJointPoint::AdjustJointPoint( m_eJointPointIdForTo, slotIndex ) );
}

//------------
// スケール適用
//------------
void CSlotAdjuster::applyRateScale( int rateScale ){
    // [m_nMinRateForT]は無視（※この値は割合）
    // [m_nMaxRateForT]は無視（※この値は割合）
    
    // [m_nMinRateForS]は無視（※この値は割合）
    // [m_nMaxRateForS]は無視（※この値は割合）

    // [m_eJointPointIdForFrom]は無視（※この値はIDなのでスケールに意味はない）
    // [m_eJointPointIdForTo]は無視（※この値はIDなのでスケールに意味はない）
    
    m_nJointRateRotR = m_nJointRateRotR*rateScale/BEZIER_SCALE_RATE;
    m_nJointMinRateOfsRForH = m_nJointMinRateOfsRForH*rateScale/BEZIER_SCALE_RATE;
    m_nJointMaxRateOfsRForH = m_nJointMaxRateOfsRForH*rateScale/BEZIER_SCALE_RATE;
    // [m_nJointRateRotOfs]は無視（※この値は割合）
    // [m_nJointRateOfsForRotCenter]は無視（この値は半径に対して適用される割合）
    
    // [m_nJointOpenFromRateDirX_p/m]は無視（※この値は割合値）
    // [m_nJointOpenFromRateDirY_p/m]は無視（※この値は割合値）
    // [m_nJointCloseFromRateDirX_p/m]は無視（※この値は割合値）
    // [m_nJointCloseFromRateDirY_p/m]は無視（※この値は割合値）
    // [m_nJointOpenToRateDirX_p/m]は無視（※この値は割合値）
    // [m_nJointOpenToRateDirY_p/m]は無視（※この値は割合値）
    // [m_nJointCloseToRateDirX_p/m]は無視（※この値は割合値）
    // [m_nJointCloseToRateDirY_p/m]は無視（※この値は割合値）

    m_nCoverRateOfsX = m_nCoverRateOfsX*rateScale/BEZIER_SCALE_RATE;
    m_nCoverRateOfsY = m_nCoverRateOfsY*rateScale/BEZIER_SCALE_RATE;
    // [m_nCoverRateScaleX]は無視
    // [m_nCoverRateScaleY]は無視
    // [m_nCoverRateRot]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CSlotAdjuster::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 2 + 2 + 7 + 16 + 5 );
    
    int id = 0;
    
    // for T
    pMenu->setItemAt( id++, "T: MIN_R", &m_nMinRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    pMenu->setItemAt( id++, "T: MAX_R", &m_nMaxRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    // for S
    pMenu->setItemAt( id++, "S: MIN_R", &m_nMinRateForS, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "S: MAX_R", &m_nMaxRateForS, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    // joint
    pMenu->setItemAtAsLabel( id++, "J: ID_FROM", &m_eJointPointIdForFrom, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );
    pMenu->setItemAtAsLabel( id++, "J: ID_TO", &m_eJointPointIdForTo, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "J: ROT_R", &m_nJointRateRotR, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ADJ_MIN_R", &m_nJointMinRateOfsRForH, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ADJ_MAX_R", &m_nJointMaxRateOfsRForH, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "J: ROT_OFS", &m_nJointRateRotOfs, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    
    pMenu->setItemAt( id++, "J: RATE_FOR_ROT_CENTER_OFS", &m_nJointRateOfsForRotCenter, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "J: T+ OPEN FROM DIR X", &m_nJointOpenFromRateDirX_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ OPEN FROM DIR Y", &m_nJointOpenFromRateDirY_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ CLOSE FROM DIR X", &m_nJointCloseFromRateDirX_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ CLOSE FROM DIR Y", &m_nJointCloseFromRateDirY_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ OPEN TO DIR X", &m_nJointOpenToRateDirX_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ OPEN TO DIR Y", &m_nJointOpenToRateDirY_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ CLOSE TO DIR X", &m_nJointCloseToRateDirX_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T+ CLOSE TO DIR Y", &m_nJointCloseToRateDirY_p, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "J: T- OPEN FROM DIR X", &m_nJointOpenFromRateDirX_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- OPEN FROM DIR Y", &m_nJointOpenFromRateDirY_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- CLOSE FROM DIR X", &m_nJointCloseFromRateDirX_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- CLOSE FROM DIR Y", &m_nJointCloseFromRateDirY_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- OPEN TO DIR X", &m_nJointOpenToRateDirX_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- OPEN TO DIR Y", &m_nJointOpenToRateDirY_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- CLOSE TO DIR X", &m_nJointCloseToRateDirX_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "J: T- CLOSE TO DIR Y", &m_nJointCloseToRateDirY_m, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "COVER: OFS_X", &m_nCoverRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "COVER: OFS_Y", &m_nCoverRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "COVER: SCALE_X", &m_nCoverRateScaleX, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE/10, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "COVER: SCALE_Y", &m_nCoverRateScaleY, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE/10, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "COVER: ROT", &m_nCoverRateRot, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );

    // 確定
    pMenu->fixMenu();
}
