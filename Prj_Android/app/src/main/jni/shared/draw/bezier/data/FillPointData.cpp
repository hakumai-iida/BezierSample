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
    
    m_eHookTargetId = eSTROKE_HOOK_TARGET_INVALID;
    m_eHookTargetIdForMC = eSTROKE_HOOK_TARGET_INVALID;

    m_eFillOption = eFILL_OPTION_INVALID;
    m_nDarkOfsForOption = 0;

    m_eTouchTargetIdForOption = eSTROKE_TOUCH_TARGET_INVALID;
    m_eTouchTargetIdForOptionForMC = eSTROKE_TOUCH_TARGET_INVALID;
    
    m_eMaskTargetForOption = eBD_MASK_TARGET_INVALID;

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
    
    m_eHookTargetId = pData->m_eHookTargetId;
    m_eHookTargetIdForMC = pData->m_eHookTargetIdForMC;

    m_eFillOption = pData->m_eFillOption;
    m_nDarkOfsForOption = pData->m_nDarkOfsForOption;

    m_eTouchTargetIdForOption = pData->m_eTouchTargetIdForOption;
    m_eTouchTargetIdForOptionForMC = pData->m_eTouchTargetIdForOptionForMC;
    
    m_eMaskTargetForOption = pData->m_eMaskTargetForOption;

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
    
    m_eHookTargetId = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );
    m_eHookTargetIdForMC = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );

    m_eFillOption = (eFILL_OPTION)CEnum::ReadEnum( pIB, g_pArrLabelFillOption );
    m_nDarkOfsForOption = pIB->readInt16();
    m_eTouchTargetIdForOption = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_eTouchTargetIdForOptionForMC = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_eMaskTargetForOption = (eBD_MASK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelBdMaskTarget );
    
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
    
    CEnum::WriteEnum( pOB, m_eHookTargetId, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    CEnum::WriteEnum( pOB, m_eHookTargetIdForMC, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    
    CEnum::WriteEnum( pOB, m_eFillOption, eFILL_OPTION_MAX, g_pArrLabelFillOption );
    pOB->writeInt16( (short)m_nDarkOfsForOption );
    CEnum::WriteEnum( pOB, m_eTouchTargetIdForOption, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    CEnum::WriteEnum( pOB, m_eTouchTargetIdForOptionForMC, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    CEnum::WriteEnum( pOB, m_eMaskTargetForOption, eBD_MASK_TARGET_MAX, g_pArrLabelBdMaskTarget );
    
    pOB->writeInt16( (short)m_nStrokeSizeForOption );
    pOB->writeInt16( (short)m_nStrokeRangeForOption );
    pOB->writeInt16( (short)m_nOfsXForOption );
    pOB->writeInt16( (short)m_nOfsYForOption );
}

//------------------------------
// フック対象IDの確定
//------------------------------
int CFillPointData::fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC ){
    eSTROKE_HOOK_TARGET id = ((isMC)? m_eHookTargetIdForMC: m_eHookTargetId);
    return( CStroke::AdjustHookTargetForSlotIndex( id, slot, slotIndex ) );
}

//------------------------------
// オプション塗りのタッチ対象IDの確定
//------------------------------
int CFillPointData::fixTouchTargetIdForOption( int slotIndex, bool isMC ){
    eSTROKE_TOUCH_TARGET id = ((isMC)? m_eTouchTargetIdForOptionForMC: m_eTouchTargetIdForOption);
    return( CStroke::AdjustTouchTargetForSlotIndex( id, slotIndex ) );
}

//------------------------------
// オプション塗りのビットマスクの取得
//------------------------------
BYTE CFillPointData::getBitMaskForOption( eBD_SLOT slot, int slotIndex ){
    return( CBDConst::GetBitMaskForSlotIndex( m_eMaskTargetForOption, slot, slotIndex ) );
}

//------------
// スケール適用
//------------
void CFillPointData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視
    
    m_oAdjXY.applyRateScale( rateScale );
    
    // [m_eHookTargetId]は無視
    // [m_eHookTargetIdForMC]は無視

    // [m_eFillOption]は無視
    // [m_nDarkOfsForOption]は無視
    // [m_eTouchTargetIdForOption]は無視
    // [m_eTouchTargetIdForOptionForMC]は無視
    // [m_eMaskTargetForOption]は無視
    
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
	pMenu->setItemNum( 2 + 9 + eFPD_FLAG_MAX );

	int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "HOOK", &m_eHookTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "HOOK(MC)", &m_eHookTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "OPTION: TYPE", &m_eFillOption, eEDIT_VALUE_TYPE_INT32,
                             eFILL_OPTION_INVALID, eFILL_OPTION_MAX-1, g_pArrLabelFillOption );

    pMenu->setItemAt( id++, "OPTION: DARK_OFS", &m_nDarkOfsForOption, eEDIT_VALUE_TYPE_INT32,
                      -(BEZIER_PAL_COL_NUM_IN_GRP-1), (BEZIER_PAL_COL_NUM_IN_GRP-1) );  // 「最も明るい〜もっとも暗い」の変動ができるように

    pMenu->setItemAtAsLabel( id++, "OPTION: TOUCH_TARGET_ID", &m_eTouchTargetIdForOption, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "OPTION: TOUCH_TARGET_ID(MC)", &m_eTouchTargetIdForOptionForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "OPTION: MASK_TARGET", &m_eMaskTargetForOption, eEDIT_VALUE_TYPE_INT32,
                             eBD_MASK_TARGET_INVALID, eBD_MASK_TARGET_MAX-1, g_pArrLabelBdMaskTarget );
    

    pMenu->setItemAt( id++, "OPTION: STROKE_SIZE", &m_nStrokeSizeForOption, eEDIT_VALUE_TYPE_INT32, 0, 15 );
    pMenu->setItemAt( id++, "OPTION: STROKE_RANGE", &m_nStrokeRangeForOption, eEDIT_VALUE_TYPE_INT32, 0, 15 );

    pMenu->setItemAt( id++, "OPTION: OFS_X", &m_nOfsXForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );
    pMenu->setItemAt( id++, "OPTION: OFS_Y", &m_nOfsYForOption, eEDIT_VALUE_TYPE_INT32, -99, 99 );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: HOOK_WITH_OFS", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_HOOK_WITH_OFS );
    pMenu->setItemAtAsBit( id++, "FLAG: TRANSPARENCY", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_TRANSPARENCY );
    pMenu->setItemAtAsBit( id++, "FLAG: ON_OUT_COL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eFPD_FLAG_FILL_ON_OUT_COL );

	// 確定
	pMenu->fixMenu();
}
