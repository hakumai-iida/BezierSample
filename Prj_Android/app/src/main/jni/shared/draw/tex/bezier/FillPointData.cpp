/*+----------------------------------------------------------------+
  |	Title:		FillPointData.cpp [共通環境]
  |	Comment:	塗りポイントデータ（※[CPaintObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FillPointData.hpp"
#include "draw/tex/stroke/Stroke.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_FILL_POINT_DATA   (16*1024)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )

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
//----------------------------------------------
// 確保メモリセル数（※管理内で[new]するセル数）
//----------------------------------------------
int CFillPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CFillPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CFillPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_FILL_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CFillPointData) * MAX_ALLOC_FILL_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CFillPointData::CFillPointData( void ): CListNode(){
	clear();
}

//---------------------------
// クリア（※データの破棄）
//---------------------------
void CFillPointData::clear( void ){
    m_nFlag = 0;
    m_oAdjXY.clear();
    
    m_eFillOption = eFILL_OPTION_INVALID;
    m_nDarkOfsForOption = 0;
    m_eTouchTargetIdForOption = eSTROKE_TOUCH_TARGET_INVALID;
    m_nStrokeSizeForOption = 0;
    m_nStrokeRangeForOption = 0;
    m_nOfsXForOption = 0;
    m_nOfsYForOption = 0;
}

//--------------------
// コピー
//--------------------
void CFillPointData::copy( CFillPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );

    m_eFillOption = pData->m_eFillOption;
    m_nDarkOfsForOption = pData->m_nDarkOfsForOption;
    m_eTouchTargetIdForOption = pData->m_eTouchTargetIdForOption;
    m_nStrokeSizeForOption = pData->m_nStrokeSizeForOption;
    m_nStrokeRangeForOption = pData->m_nStrokeRangeForOption;
    m_nOfsXForOption = pData->m_nOfsXForOption;
    m_nOfsYForOption = pData->m_nOfsYForOption;
}

//---------------------------
// 読み込み
//---------------------------
void CFillPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );

    m_eFillOption = (eFILL_OPTION)CEnum::ReadEnum( pIB, g_pArrLabelFillOption );
    m_nDarkOfsForOption = pIB->readInt16();
    m_eTouchTargetIdForOption = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_nStrokeSizeForOption = pIB->readInt16();
    m_nStrokeRangeForOption = pIB->readInt16();
    m_nOfsXForOption = pIB->readInt16();
    m_nOfsYForOption = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CFillPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );
    
    CEnum::WriteEnum( pOB, m_eFillOption, eFILL_OPTION_MAX, g_pArrLabelFillOption );
    pOB->writeInt16( (short)m_nDarkOfsForOption );
    CEnum::WriteEnum( pOB, m_eTouchTargetIdForOption, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    pOB->writeInt16( (short)m_nStrokeSizeForOption );
    pOB->writeInt16( (short)m_nStrokeRangeForOption );
    pOB->writeInt16( (short)m_nOfsXForOption );
    pOB->writeInt16( (short)m_nOfsYForOption );
}

//---------------------------
// タッチ対象IDの取得
//---------------------------
eSTROKE_TOUCH_TARGET CFillPointData::getTouchTargetIdForOption( int slotIndex ){
    return( CStroke::AdjustTouchTargetForSlotIndex( m_eTouchTargetIdForOption, slotIndex ) );
}

//------------
// スケール適用
//------------
void CFillPointData::applyRateScale( int rateScale ){
    m_oAdjXY.applyRateScale( rateScale );
    
    // [m_eFillOption]は無視
    // [m_nDarkOfsForOption]は無視
    // [m_eTouchTargetIdForOption]は無視
    m_nStrokeSizeForOption = m_nStrokeSizeForOption*rateScale/BEZIER_SCALE_RATE;
    m_nStrokeRangeForOption = m_nStrokeRangeForOption*rateScale/BEZIER_SCALE_RATE;
    m_nOfsXForOption = m_nOfsXForOption*rateScale/BEZIER_SCALE_RATE;
    m_nOfsYForOption = m_nOfsYForOption*rateScale/BEZIER_SCALE_RATE;
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CFillPointData::setEditValueMenu( CEditValueMenu* pMenu ){
	// 編集項目数設定
	pMenu->setItemNum( 7 + eFPD_FLAG_MAX );

	int id = 0;
    
    pMenu->setItemAtAsLabel( id++, "OPTION: TYPE", &m_eFillOption, eEDIT_VALUE_TYPE_INT32,
                             eFILL_OPTION_INVALID, eFILL_OPTION_MAX-1, g_pArrLabelFillOption );

    pMenu->setItemAt( id++, "OPTION: DARK_OFS", &m_nDarkOfsForOption, eEDIT_VALUE_TYPE_INT32,
                      -(BEZIER_PAL_COL_NUM_IN_GRP-1), (BEZIER_PAL_COL_NUM_IN_GRP-1) );

    pMenu->setItemAtAsLabel( id++, "OPTION: TOUCH_TARGET_ID", &m_eTouchTargetIdForOption, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAt( id++, "OPTION: STROKE_SIZE", &m_nStrokeSizeForOption, eEDIT_VALUE_TYPE_INT32, 0, 15 );
    pMenu->setItemAt( id++, "OPTION: STROKE_RANGE", &m_nStrokeRangeForOption, eEDIT_VALUE_TYPE_INT32, 0, 15 );

    pMenu->setItemAt( id++, "OPTION: OFS_X", &m_nOfsXForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );
    pMenu->setItemAt( id++, "OPTION: OFS_Y", &m_nOfsYForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: TRANSPARENCY", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_TRANSPARENCY );
    pMenu->setItemAtAsBit( id++, "FLAG: ON_OUT_COL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_FILL_ON_OUT_COL );

	// 確定
	pMenu->fixMenu();
}
