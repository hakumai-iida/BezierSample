/*+----------------------------------------------------------------+
  |	Title:		AnchorPointData.cpp [共通環境]
  |	Comment:	アンカーポイントデータ（※[CLineObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AnchorPointData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_ANCHOR_POINT_DATA   (32*1024)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )

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
int CAnchorPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CAnchorPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CAnchorPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_ANCHOR_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CAnchorPointData) * MAX_ALLOC_ANCHOR_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CAnchorPointData::CAnchorPointData( void ):  CListNode(){
	clear();
}

//----------------------------------------------------------------------------
// クリア（※データの破棄）
//----------------------------------------------------------------------------
void CAnchorPointData::clear( void ){
    m_nFlag = 0x0;
    m_oAdjXY.clear();
    m_oAdjInXY.clear();
    m_oAdjOutXY.clear();
        
    m_eJointPointId = eJOINT_POINT_INVALID;
    m_eJointPointIdForMC = eJOINT_POINT_INVALID;

    m_eHookTargetId = eSTROKE_HOOK_TARGET_INVALID;
    m_eHookTargetIdForMC = eSTROKE_HOOK_TARGET_INVALID;

    m_eReHookTargetId = eSTROKE_HOOK_TARGET_INVALID;
    m_eReHookTargetIdForMC = eSTROKE_HOOK_TARGET_INVALID;

    m_eGuideTargetId = eSTROKE_GUIDE_TARGET_INVALID;
    m_eGuideTargetIdForMC = eSTROKE_GUIDE_TARGET_INVALID;

    m_nStrokeStartSize = BEZIER_SCALE_RATE;
    m_nStrokeStartRange = 0;
    m_nStrokeEndSize = BEZIER_SCALE_RATE;
    m_nStrokeEndRange = 0;
    m_nStrokeEdgeFillSize = 0;
    
    m_eTouchTargetId = eSTROKE_TOUCH_TARGET_INVALID;
    m_eTouchTargetIdForMC = eSTROKE_TOUCH_TARGET_INVALID;

    m_eTouchTargetSubId = eSTROKE_TOUCH_TARGET_INVALID;
    m_eTouchTargetSubIdForMC = eSTROKE_TOUCH_TARGET_INVALID;

    m_eStripeOrFrillMainPalOfsId = ePAL_OFS_INVALID;
    m_eStripeOrFrillSubPalOfsId = ePAL_OFS_INVALID;
    m_eFrillMainId = eSTROKE_FRILL_INVALID;
    m_eFrillSubId = eSTROKE_FRILL_INVALID;

    // 初期値は０
    m_nTouchBasePos = 0;
    m_nTouchBaseSize = BEZIER_SCALE_RATE;
    m_nTouchBaseRot = 0;
    
    m_nTouchFrontArea = 0;
    m_nTouchFrontSizeOfs = 0;
    m_nTouchFrontRotOfs = 0;
    
    m_nTouchBackArea = 0;
    m_nTouchBackSizeOfs = 0;
    m_nTouchBackRotOfs = 0;
    
    m_nTouchFrontNum = 0;
    m_nTouchFrontSkip = 0;
    m_nTouchFrontBorderRate = 0;
    
    m_nTouchBackNum = 0;
    m_nTouchBackSkip = 0;
    m_nTouchBackBorderRate = 0;
    
    m_nTouchRandomOfsFor0 = 0;
    m_nTouchRandomOfsFor1 = 0;
    
    m_nTouchHeadSkipRate = 0;
    m_nTouchTailSkipRate = 0;
    
    m_nTouchRefOfsRateForMain = 0;
    m_nTouchRefOfsRateForSub = 0;

    m_nFrillWidthRateForMain = BEZIER_SCALE_RATE;
    m_nFrillWidthRateForSub = BEZIER_SCALE_RATE;
}

//---------------------------
// 設定
//---------------------------
void CAnchorPointData::set( int rX, int rY, int inRX, int inRY, int outRX, int outRY ){
    m_oAdjXY.setRateXY( rX, rY );
    m_oAdjInXY.setRateXY( inRX, inRY );
    m_oAdjOutXY.setRateXY( outRX, outRY );
}

//---------------------------
// 設定：ストローク
//---------------------------
void CAnchorPointData::setStroke( int startSize, int startRange, int endSize, int endRange ){
    m_nStrokeStartSize = startSize;
    m_nStrokeStartRange = startRange;
    m_nStrokeEndSize = endSize;
    m_nStrokeEndRange = endRange;
}

//---------------------------
// 設定：タッチ（基本）
//---------------------------
void CAnchorPointData::setTouchBase( int pos, int size, int rot ){
    m_nTouchBasePos = pos;
    m_nTouchBaseSize = size;
    m_nTouchBaseRot = rot;
}

//---------------------------
// 設定：タッチ（前方）
//---------------------------
void CAnchorPointData::setTouchFront( int area, int sizeOfs, int rotOfs, int num, int skip, int borderRate ){
    m_nTouchFrontArea = area;
    m_nTouchFrontSizeOfs = sizeOfs;
    m_nTouchFrontRotOfs = rotOfs;
    
    m_nTouchFrontNum = num;
    m_nTouchFrontSkip = skip;
    m_nTouchFrontBorderRate = borderRate;
}

//---------------------------
// 設定：タッチ（後方）
//---------------------------
void CAnchorPointData::setTouchBack( int area, int sizeOfs, int rotOfs, int num, int skip, int borderRate ){
    m_nTouchBackArea = area;
    m_nTouchBackSizeOfs = sizeOfs;
    m_nTouchBackRotOfs = rotOfs;

    m_nTouchBackNum = num;
    m_nTouchBackSkip = skip;
    m_nTouchBackBorderRate = borderRate;
}

//---------------------------
// 設定：タッチのランダム域
//---------------------------
void CAnchorPointData::setTouchRandomOfs( int rand0, int rand1 ){
    m_nTouchRandomOfsFor0 = rand0;
    m_nTouchRandomOfsFor1 = rand1;
}

//---------------------------
// 設定：タッチのスキップ率
//---------------------------
void CAnchorPointData::setTouchSkipRate( int rateHead, int rateTail ){
    m_nTouchHeadSkipRate = rateHead;
    m_nTouchTailSkipRate = rateTail;
}

//---------------------------
// 設定：タッチ参照のオフセット
//---------------------------
void CAnchorPointData::setTouchRefOfsRate( int rateMain, int rateSub ){
    m_nTouchRefOfsRateForMain = rateMain;
    m_nTouchRefOfsRateForSub = rateSub;
}

//---------------------------
// 設定：タッチのスキップ率
//---------------------------
void CAnchorPointData::setFrillWidthRate( int rateMain, int rateSub ){
    m_nFrillWidthRateForMain = rateMain;
    m_nFrillWidthRateForSub = rateSub;
}

//---------------------------
// 結合ポイントIDの取得
//---------------------------
int CAnchorPointData::fixJointPointId( int slotIndex, bool isMC ){
    eJOINT_POINT id = ((isMC)? m_eJointPointIdForMC: m_eJointPointId);
    return( CJointPoint::AdjustJointPoint( id, slotIndex ) );
}

//---------------------------
// フック対象IDの確定
//---------------------------
int CAnchorPointData::fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC ){
    eSTROKE_HOOK_TARGET id = ((isMC)? m_eHookTargetIdForMC: m_eHookTargetId);
    return( CStroke::AdjustHookTargetForSlotIndex( id, slot, slotIndex ) );
}

//---------------------------
// 再フック対象IDの確定
//---------------------------
int CAnchorPointData::fixReHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC ){
    eSTROKE_HOOK_TARGET id = ((isMC)? m_eReHookTargetIdForMC: m_eReHookTargetId);
    return( CStroke::AdjustHookTargetForSlotIndex( id, slot, slotIndex ) );
}

//---------------------------
// ガイド対象IDの確定
//---------------------------
int CAnchorPointData::fixGuideTargetId( int slotIndex, bool isMC ){
    eSTROKE_GUIDE_TARGET id = ((isMC)? m_eGuideTargetIdForMC: m_eGuideTargetId);
    return( CStroke::AdjustGuideTargetForSlotIndex( id, slotIndex ) );
}

//---------------------------
// タッチ対象IDの確定
//---------------------------
int CAnchorPointData::fixTouchTargetId( int slotIndex, bool isMC ){
    eSTROKE_TOUCH_TARGET id = ((isMC)? m_eTouchTargetIdForMC: m_eTouchTargetId);
    return( CStroke::AdjustTouchTargetForSlotIndex( id, slotIndex ) );
}

//---------------------------
// タッチサブIDの確定
//---------------------------
int CAnchorPointData::fixTouchTargetSubId( int slotIndex, bool isMC ){
    eSTROKE_TOUCH_TARGET id = ((isMC)? m_eTouchTargetSubIdForMC: m_eTouchTargetSubId);
    return( CStroke::AdjustTouchTargetForSlotIndex( id, slotIndex ) );
}

//--------------------
// コピー
//--------------------
void CAnchorPointData::copy( CAnchorPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );
    m_oAdjInXY.copy( &pData->m_oAdjInXY );
    m_oAdjOutXY.copy( &pData->m_oAdjOutXY );
        
    m_eJointPointId = pData->m_eJointPointId;
    m_eJointPointIdForMC = pData->m_eJointPointIdForMC;

    m_eHookTargetId = pData->m_eHookTargetId;
    m_eHookTargetIdForMC = pData->m_eHookTargetIdForMC;

    m_eReHookTargetId = pData->m_eReHookTargetId;
    m_eReHookTargetIdForMC = pData->m_eReHookTargetIdForMC;

    m_eGuideTargetId = pData->m_eGuideTargetId;
    m_eGuideTargetIdForMC = pData->m_eGuideTargetIdForMC;

    m_nStrokeStartSize = pData->m_nStrokeStartSize;
    m_nStrokeStartRange = pData->m_nStrokeStartRange;
    m_nStrokeEndSize = pData->m_nStrokeEndSize;
    m_nStrokeEndRange = pData->m_nStrokeEndRange;
    m_nStrokeEdgeFillSize = pData->m_nStrokeEdgeFillSize;

    m_eTouchTargetId = pData->m_eTouchTargetId;
    m_eTouchTargetIdForMC = pData->m_eTouchTargetIdForMC;

    m_eTouchTargetSubId = pData->m_eTouchTargetSubId;
    m_eTouchTargetSubIdForMC = pData->m_eTouchTargetSubIdForMC;

    m_eStripeOrFrillMainPalOfsId = pData->m_eStripeOrFrillMainPalOfsId;
    m_eStripeOrFrillSubPalOfsId = pData->m_eStripeOrFrillSubPalOfsId;
    m_eFrillMainId = pData->m_eFrillMainId;
    m_eFrillSubId = pData->m_eFrillSubId;

    m_nTouchBasePos = pData->m_nTouchBasePos;
    m_nTouchBaseSize = pData->m_nTouchBaseSize;
    m_nTouchBaseRot = pData->m_nTouchBaseRot;
    m_nTouchFrontArea = pData->m_nTouchFrontArea;
    m_nTouchFrontSizeOfs = pData->m_nTouchFrontSizeOfs;
    m_nTouchFrontRotOfs = pData->m_nTouchFrontRotOfs;
    m_nTouchBackArea = pData->m_nTouchBackArea;
    m_nTouchBackSizeOfs = pData->m_nTouchBackSizeOfs;
    m_nTouchBackRotOfs = pData->m_nTouchBackRotOfs;
    m_nTouchFrontNum = pData->m_nTouchFrontNum;
    m_nTouchFrontSkip = pData->m_nTouchFrontSkip;
    m_nTouchFrontBorderRate = pData->m_nTouchFrontBorderRate;
    m_nTouchBackNum = pData->m_nTouchBackNum;
    m_nTouchBackSkip = pData->m_nTouchBackSkip;
    m_nTouchBackBorderRate = pData->m_nTouchBackBorderRate;
    m_nTouchRandomOfsFor0 = pData->m_nTouchRandomOfsFor0;
    m_nTouchRandomOfsFor1 = pData->m_nTouchRandomOfsFor1;
    m_nTouchHeadSkipRate = pData->m_nTouchHeadSkipRate;
    m_nTouchTailSkipRate = pData->m_nTouchTailSkipRate;
    m_nTouchRefOfsRateForMain = pData->m_nTouchRefOfsRateForMain;
    m_nTouchRefOfsRateForSub = pData->m_nTouchRefOfsRateForSub;
    m_nFrillWidthRateForMain = pData->m_nFrillWidthRateForMain;
    m_nFrillWidthRateForSub = pData->m_nFrillWidthRateForSub;
}

//---------------------------
// 読み込み
//---------------------------
void CAnchorPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );
    m_oAdjInXY.read( pIB );
    m_oAdjOutXY.read( pIB );    
   
    m_eJointPointId = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );
    m_eJointPointIdForMC = (eJOINT_POINT)CEnum::ReadEnum( pIB, g_pArrLabelJointPoint );

    m_eHookTargetId = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );
    m_eHookTargetIdForMC = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );

    m_eReHookTargetId = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );
    m_eReHookTargetIdForMC = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );

    m_eGuideTargetId = (eSTROKE_GUIDE_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeGuideTarget );
    m_eGuideTargetIdForMC = (eSTROKE_GUIDE_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeGuideTarget );

    m_nStrokeStartSize = pIB->readInt16();
    m_nStrokeStartRange = pIB->readInt16();
    m_nStrokeEndSize = pIB->readInt16();
    m_nStrokeEndRange = pIB->readInt16();
    m_nStrokeEdgeFillSize = pIB->readInt16();

    m_eTouchTargetId = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_eTouchTargetIdForMC = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );

    m_eTouchTargetSubId = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );
    m_eTouchTargetSubIdForMC = (eSTROKE_TOUCH_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeTouchTarget );

    m_eStripeOrFrillMainPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    m_eStripeOrFrillSubPalOfsId = (ePAL_OFS)CEnum::ReadEnum( pIB, g_pArrLabelPalOfs );
    m_eFrillMainId = (eSTROKE_FRILL)CEnum::ReadEnum( pIB, g_pArrLabelStrokeFrill );
    m_eFrillSubId = (eSTROKE_FRILL)CEnum::ReadEnum( pIB, g_pArrLabelStrokeFrill );
    
    m_nTouchBasePos = pIB->readInt16();
    m_nTouchBaseSize = pIB->readInt16();
    m_nTouchBaseRot = pIB->readInt16();
    m_nTouchFrontArea = pIB->readInt16();
    m_nTouchFrontSizeOfs = pIB->readInt16();
    m_nTouchFrontRotOfs = pIB->readInt16();
    m_nTouchBackArea = pIB->readInt16();
    m_nTouchBackSizeOfs = pIB->readInt16();
    m_nTouchBackRotOfs = pIB->readInt16();
    m_nTouchFrontNum = pIB->readInt16();
    m_nTouchFrontSkip = pIB->readInt16();
    m_nTouchFrontBorderRate = pIB->readInt16();
    m_nTouchBackNum = pIB->readInt16();
    m_nTouchBackSkip = pIB->readInt16();
    m_nTouchBackBorderRate = pIB->readInt16();
    m_nTouchRandomOfsFor0 = pIB->readInt16();
    m_nTouchRandomOfsFor1 = pIB->readInt16();
    m_nTouchHeadSkipRate = pIB->readInt16();
    m_nTouchTailSkipRate = pIB->readInt16();
    m_nTouchRefOfsRateForMain = pIB->readInt16();
    m_nTouchRefOfsRateForSub = pIB->readInt16();
    m_nFrillWidthRateForMain = pIB->readInt16();
    m_nFrillWidthRateForSub = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CAnchorPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );
    m_oAdjInXY.write( pOB );
    m_oAdjOutXY.write( pOB );
    
    CEnum::WriteEnum( pOB, m_eJointPointId, eJOINT_POINT_MAX, g_pArrLabelJointPoint );
    CEnum::WriteEnum( pOB, m_eJointPointIdForMC, eJOINT_POINT_MAX, g_pArrLabelJointPoint );

    CEnum::WriteEnum( pOB, m_eHookTargetId, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    CEnum::WriteEnum( pOB, m_eHookTargetIdForMC, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );

    CEnum::WriteEnum( pOB, m_eReHookTargetId, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    CEnum::WriteEnum( pOB, m_eReHookTargetIdForMC, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );

    CEnum::WriteEnum( pOB, m_eGuideTargetId, eSTROKE_GUIDE_TARGET_MAX, g_pArrLabelStrokeGuideTarget );
    CEnum::WriteEnum( pOB, m_eGuideTargetIdForMC, eSTROKE_GUIDE_TARGET_MAX, g_pArrLabelStrokeGuideTarget );

    pOB->writeInt16( (int16)m_nStrokeStartSize );
    pOB->writeInt16( (int16)m_nStrokeStartRange );
    pOB->writeInt16( (int16)m_nStrokeEndSize );
    pOB->writeInt16( (int16)m_nStrokeEndRange );
    pOB->writeInt16( (int16)m_nStrokeEdgeFillSize );

    CEnum::WriteEnum( pOB, m_eTouchTargetId, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    CEnum::WriteEnum( pOB, m_eTouchTargetIdForMC, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );

    CEnum::WriteEnum( pOB, m_eTouchTargetSubId, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );
    CEnum::WriteEnum( pOB, m_eTouchTargetSubIdForMC, eSTROKE_TOUCH_TARGET_MAX, g_pArrLabelStrokeTouchTarget );

    CEnum::WriteEnum( pOB, m_eStripeOrFrillMainPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    CEnum::WriteEnum( pOB, m_eStripeOrFrillSubPalOfsId, ePAL_OFS_MAX, g_pArrLabelPalOfs );
    CEnum::WriteEnum( pOB, m_eFrillMainId, eSTROKE_FRILL_MAX, g_pArrLabelStrokeFrill );
    CEnum::WriteEnum( pOB, m_eFrillSubId, eSTROKE_FRILL_MAX, g_pArrLabelStrokeFrill );

    pOB->writeInt16( (int16)m_nTouchBasePos );
    pOB->writeInt16( (int16)m_nTouchBaseSize );
    pOB->writeInt16( (int16)m_nTouchBaseRot );
    pOB->writeInt16( (int16)m_nTouchFrontArea );
    pOB->writeInt16( (int16)m_nTouchFrontSizeOfs );
    pOB->writeInt16( (int16)m_nTouchFrontRotOfs );
    pOB->writeInt16( (int16)m_nTouchBackArea );
    pOB->writeInt16( (int16)m_nTouchBackSizeOfs );
    pOB->writeInt16( (int16)m_nTouchBackRotOfs );
    pOB->writeInt16( (int16)m_nTouchFrontNum );
    pOB->writeInt16( (int16)m_nTouchFrontSkip );
    pOB->writeInt16( (int16)m_nTouchFrontBorderRate );
    pOB->writeInt16( (int16)m_nTouchBackNum );
    pOB->writeInt16( (int16)m_nTouchBackSkip );
    pOB->writeInt16( (int16)m_nTouchBackBorderRate );
    pOB->writeInt16( (int16)m_nTouchRandomOfsFor0 );
    pOB->writeInt16( (int16)m_nTouchRandomOfsFor1 );
    pOB->writeInt16( (int16)m_nTouchHeadSkipRate );
    pOB->writeInt16( (int16)m_nTouchTailSkipRate );
    pOB->writeInt16( (int16)m_nTouchRefOfsRateForMain );
    pOB->writeInt16( (int16)m_nTouchRefOfsRateForSub );
    pOB->writeInt16( (int16)m_nFrillWidthRateForMain );
    pOB->writeInt16( (int16)m_nFrillWidthRateForSub );
}

//------------
// スケール適用
//------------
void CAnchorPointData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視

    m_oAdjXY.applyRateScale( rateScale );
    m_oAdjInXY.applyRateScale( rateScale );
    m_oAdjOutXY.applyRateScale( rateScale );
        
    // [m_eJointPointId]は無視
    // [m_eJointPointIdForMC]は無視

    // [m_eHookTargetId]は無視
    // [m_eHookTargetIdForMC]は無視

    // [m_eReHookTargetId]は無視
    // [m_eReHookTargetIdForMC]は無視

    // [m_eGuideTargetId]は無視
    // [m_eGuideTargetIdForMC]は無視

    // [m_nStrokeStartSize]は無視
    // [m_nStrokeStartRange]は無視
    // [m_nStrokeEndSize]は無視
    // [m_nStrokeEndRange]は無視
    // [m_nStrokeEdgeFillSize]は無視
    
    // [m_eTouchTargetId]は無視
    // [m_eTouchTargetIdForMC]は無視

    // [m_eTouchTargetSubId]は無視
    // [m_eTouchTargetSubIdForMC]は無視

    // [m_eStripeOrFrillMainPalOfsId]は無視
    // [m_eStripeOrFrillSubPalOfsId]は無視
    
    // [eSTROKE_FRILL m_eFrillMainId]は無視
    // [eSTROKE_FRILL m_eFrillSubId]は無視

    // [m_nTouchBasePos]は無視
    // [m_nTouchBaseSize]は無視
    // [m_nTouchBaseRot]は無視
    
    // [m_nTouchFrontArea]は無視
    // [m_nTouchFrontSizeOfs]は無視
    // [m_nTouchFrontRotOfs]は無視
    
    // [m_nTouchBackArea]は無視
    // [m_nTouchBackSizeOfs]は無視
    // [m_nTouchBackRotOfs]は無視
    
    // [m_nTouchFrontNum]は無視
    // [m_nTouchFrontSkip]は無視
    // [m_nTouchFront]は無視
    
    // [m_nTouchBackNum]は無視
    // [m_nTouchBackSkip]は無視
    // [m_nTouchBackBorderRate]は無視
    
    // [m_nTouchRandomOfsFor0]は無視
    // [m_nTouchRandomOfsFor1]は無視
    
    // [m_nTouchHeadSkipRate]は無視
    // [m_nTouchTailSkipRate]は無視
    
    // [m_nTouchRefOfsRateForMain]は無視
    // [m_nTouchRefOfsRateForSub]は無視

    // [m_nFrillWidthRateForMain]は無視
    // [m_nFrillWidthRateForSub]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CAnchorPointData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 13 + 8 + 23 + eAPD_FLAG_MAX );
    
    int id = 0;

    // 値
    pMenu->setItemAtAsLabel( id++, "JOINT", &m_eJointPointId, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );

    pMenu->setItemAtAsLabel( id++, "JOINT(MC)", &m_eJointPointIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eJOINT_POINT_INVALID, eJOINT_POINT_MAX-1, g_pArrLabelJointPoint );

    pMenu->setItemAtAsLabel( id++, "HOOK", &m_eHookTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "HOOK(MC)", &m_eHookTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "RE-HOOK", &m_eReHookTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "RE-HOOK(MC)", &m_eReHookTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "GUIDE", &m_eGuideTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_GUIDE_TARGET_INVALID, eSTROKE_GUIDE_TARGET_MAX-1, g_pArrLabelStrokeGuideTarget );

    pMenu->setItemAtAsLabel( id++, "GUIDE(MC)", &m_eGuideTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_GUIDE_TARGET_INVALID, eSTROKE_GUIDE_TARGET_MAX-1, g_pArrLabelStrokeGuideTarget );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "STROKE: START_SIZE", &m_nStrokeStartSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "STROKE: START_RANGE", &m_nStrokeStartRange, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "STROKE: END_SIZE", &m_nStrokeEndSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "STROKE: END_RANGE", &m_nStrokeEndRange, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "STROKE: EDGE_FILL_SIZE", &m_nStrokeEdgeFillSize, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAtAsLabel( id++, "TOUCH", &m_eTouchTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "TOUCH(MC)", &m_eTouchTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "TOUCH SUB", &m_eTouchTargetSubId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "TOUCH SUB(MC)", &m_eTouchTargetSubIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_TOUCH_TARGET_INVALID, eSTROKE_TOUCH_TARGET_MAX-1, g_pArrLabelStrokeTouchTarget );

    pMenu->setItemAtAsLabel( id++, "STRIPE/FRILL_MAIN_PAL", &m_eStripeOrFrillMainPalOfsId, eEDIT_VALUE_TYPE_INT32,
                            ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );
    pMenu->setItemAtAsLabel( id++, "STRIPE/FRILL_SUB_PAL", &m_eStripeOrFrillSubPalOfsId, eEDIT_VALUE_TYPE_INT32,
                            ePAL_OFS_INVALID, ePAL_OFS_MAX-1, g_pArrLabelPalOfs );

    pMenu->setItemAtAsLabel( id++, "FRILL: MAIN_ID", &m_eFrillMainId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_FRILL_INVALID, eSTROKE_FRILL_MAX-1, g_pArrLabelStrokeFrill );

    pMenu->setItemAtAsLabel( id++, "FRILL: SUB_ID", &m_eFrillSubId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_FRILL_INVALID, eSTROKE_FRILL_MAX-1, g_pArrLabelStrokeFrill );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "TOUCH: BASE_POS", &m_nTouchBasePos, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: BASE_SIZE", &m_nTouchBaseSize, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TOUCH: BASE_ROT", &m_nTouchBaseRot, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "TOUCH: FRONT_AREA", &m_nTouchFrontArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: FRONT_SIZE_OFS", &m_nTouchFrontSizeOfs, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TOUCH: FRONT_ROT_OFS", &m_nTouchFrontRotOfs, eEDIT_VALUE_TYPE_INT32,
                      8*BEZIER_ROTATION_RATE_MIN, 8*BEZIER_ROTATION_RATE_MAX ); // short
    
    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "TOUCH: BACK_AREA", &m_nTouchBackArea, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: BACK_SIZE_OFS", &m_nTouchBackSizeOfs, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TOUCH: BACK_ROT_OFS", &m_nTouchBackRotOfs, eEDIT_VALUE_TYPE_INT32,
                      8*BEZIER_ROTATION_RATE_MIN, 8*BEZIER_ROTATION_RATE_MAX ); // short
    
    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "TOUCH: FRONT_NUM", &m_nTouchFrontNum, eEDIT_VALUE_TYPE_INT32, 0, 99 );
    pMenu->setItemAt( id++, "TOUCH: FRONT_SKIP", &m_nTouchFrontSkip, eEDIT_VALUE_TYPE_INT32, 0, 99 );
    pMenu->setItemAt( id++, "TOUCH: FRONT_BORDER_RATE", &m_nTouchFrontBorderRate, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    
    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "TOUCH: BACK_NUM", &m_nTouchBackNum, eEDIT_VALUE_TYPE_INT32, 0, 99 );
    pMenu->setItemAt( id++, "TOUCH: BACK_SKIP", &m_nTouchBackSkip, eEDIT_VALUE_TYPE_INT32, 0, 99 );
    pMenu->setItemAt( id++, "TOUCH: BACK_BORDER_RATE", &m_nTouchBackBorderRate, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "TOUCH: RAND_OFS_0", &m_nTouchRandomOfsFor0, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: RAND_OFS_1", &m_nTouchRandomOfsFor1, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    
    pMenu->setItemAt( id++, "TOUCH: HEAD_SKIP_RATE", &m_nTouchHeadSkipRate, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: TAIL_SKIP_RATE", &m_nTouchTailSkipRate, eEDIT_VALUE_TYPE_INT32, 0, BEZIER_SCALE_RATE );

    pMenu->setItemAt( id++, "TOUCH: REF_OFS_FOR_MAIN", &m_nTouchRefOfsRateForMain, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "TOUCH: REF_OFS_FOR_SUB", &m_nTouchRefOfsRateForSub, eEDIT_VALUE_TYPE_INT32,
                      -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );

    pMenu->setItemAt( id++, "FRILL: W_RATE_FOR_MAIN", &m_nFrillWidthRateForMain, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TOUCH: W_RATE_FOR_SUB", &m_nFrillWidthRateForSub, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: HOOK_WITH_OFS", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_HOOK_WITH_OFS );
    pMenu->setItemAtAsBit( id++, "FLAG: TRANSPARENCY", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_TRANSPARENCY );
    pMenu->setItemAtAsBit( id++, "FLAG: NO_FILL_GUIDE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_NO_FILL_GUIDE );
    pMenu->setItemAtAsBit( id++, "FLAG: DOT_ON_OUT_COL", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DOT_ON_OUT_COL );
    pMenu->setItemAtAsBit( id++, "FLAG: DOT_ERASE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_DOT_ERASE );
    pMenu->setItemAtAsBit( id++, "FLAG: TEST_LINE_REPAIR", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_TEST_LINE_REPAIR );
    pMenu->setItemAtAsBit( id++, "FLAG: REF_JOINT_POINT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_REF_JOINT_POINT );
    pMenu->setItemAtAsBit( id++, "FLAG: REVERSE_JOINT_DIR", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_REVERSE_JOINT_DIR );
    pMenu->setItemAtAsBit( id++, "FLAG: REVERSE_FRILL_HOOK", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_REVERSE_FRILL_HOOK );
    pMenu->setItemAtAsBit( id++, "FLAG: AS_TOUCH_STROKE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_AS_TOUCH_STROKE );
    pMenu->setItemAtAsBit( id++, "FLAG: TOUCH_SKIP_F_IF_MC", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_TOUCH_SKIP_FRONT_IF_MC );
    pMenu->setItemAtAsBit( id++, "FLAG: TOUCH_SKIP_B_IF_MC", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eAPD_FLAG_TOUCH_SKIP_BACK_IF_MC );
    
    // 確定
    pMenu->fixMenu();
}

#if 0
//----------------------------------------------------------
// [pPrev]と[pNext]の中間に配置する
//（※現状、利用はしていないが後で使うかもしれないのでコードは残しておく）
//----------------------------------------------------------
void CAnchorPointData::setAtIntermediate( CAnchorPointData* pPrev, CAnchorPointData* pNext ){
    CAdjustablePoint* pPrevXY = pPrev->getAdjXY();
    CAdjustablePoint* pPrevInXY = pPrev->getAdjInXY();
    CAdjustablePoint* pPrevOutXY = pPrev->getAdjOutXY();
    CAdjustablePoint* pNextXY = pNext->getAdjXY();
    CAdjustablePoint* pNextInXY = pNext->getAdjInXY();
    CAdjustablePoint* pNextOutXY = pNext->getAdjOutXY();

    //-----------------------------------------------
    // 基本XY／進入XY／出立XYの算出
    //-----------------------------------------------
    int x0 = pPrevXY->getRateX();
    int y0 = pPrevXY->getRateY();
    int x1 = pNextXY->getRateX();
    int y1 = pNextXY->getRateY();
    int xOut = pPrevOutXY->getRateX();
    int yOut = pPrevOutXY->getRateY();
    int xIn = pNextInXY->getRateX();
    int yIn = pNextInXY->getRateY();

    // t, (1-t)
    float t1 = 0.5f;        // 中点
    float tR1 = 1.0f - t1;
    
    // t^2, (1-t)^2
    float t2 = t1 * t1;
    float tR2 = tR1 * tR1;
    
    // t^3, (1-t)^3
    float t3 = t2 * t1;
    float tR3 = tR2 * tR1;
    
    // (1-t)^2*t, (1-t)*t^2
    float tR2_t1 = tR2 * t1;
    float tR1_t2 = tR1 * t2;
    
    // 座標の算出
    float newX = x0*tR3 + 3*(x0+xOut)*tR2_t1 + 3*(x1+xIn)*tR1_t2 + x1*t3;
    float newY = y0*tR3 + 3*(y0+yOut)*tR2_t1 + 3*(y1+yIn)*tR1_t2 + y1*t3;
    m_oAdjXY.setRateXY( (int)newX, (int)newY );
    
    // 前後要素の方向線が有効であれば算出(IN/OUT)
    if( xOut != 0 || yOut != 0 || xIn != 0 || yIn != 0 ){
        float newInX = x0*tR2 + 2*(x0+xOut)*tR1*t1 + (x1+xIn)*t2;
        float newInY = y0*tR2 + 2*(y0+yOut)*tR1*t1 + (y1+yIn)*t2;
        m_oAdjInXY.setRateXY( (int)(newInX-newX), (int)(newInY-newY) );
        
        float newOutX = x1*t2 + 2*(x1+xIn)*t1*tR1 + (x0+xOut)*tR2;
        float newOutY = y1*t2 + 2*(y1+yIn)*t1*tR1 + (y0+yOut)*tR2;
        m_oAdjOutXY.setRateXY( (int)(newOutX-newX), (int)(newOutY-newY) );
        
        // 前後要素の方向線を調整
        pPrevOutXY->setRateXY( (int)(pPrevOutXY->getRateX()*tR1), (int)(pPrevOutXY->getRateY()*tR1) );
        pNextInXY->setRateXY( (int)(pNextInXY->getRateX()*t1), (int)(pNextInXY->getRateY()*t1) );
    }else{
        m_oAdjInXY.setRateXY( 0, 0 );
        m_oAdjOutXY.setRateXY( 0, 0 );
    }
    
    //------------------------------------------------------------------------------------------
    // 調整値の算出（※とりあえず各値は前後要素の平均にしておく）
    //（※遅延タイプの平均は全くもって意味不明だが、基本的には前後で同じ値になるはずなので実質的には問題ないはず）
    //------------------------------------------------------------------------------------------
    int avgType, avgDepth, avgX, avgY;

    // 基本XY
    avgX = (pPrevXY->getOfsMinRateXForH() + pNextXY->getOfsMinRateXForH())/2;
    avgY = (pPrevXY->getOfsMinRateYForH() + pNextXY->getOfsMinRateYForH())/2;
    m_oAdjXY.setOfsMinRateXYForH( avgX, avgY );

    avgX = (pPrevXY->getOfsMaxRateXForH() + pNextXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevXY->getOfsMaxRateYForH() + pNextXY->getOfsMaxRateYForH())/2;
    m_oAdjXY.setOfsMaxRateXYForH( avgX, avgY );

    avgX = (pPrevXY->getOfsMinRateXForV() + pNextXY->getOfsMinRateXForV())/2;
    avgY = (pPrevXY->getOfsMinRateYForV() + pNextXY->getOfsMinRateYForV())/2;
    m_oAdjXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevXY->getOfsMaxRateXForV() + pNextXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevXY->getOfsMaxRateYForV() + pNextXY->getOfsMaxRateYForV())/2;
    m_oAdjXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevXY->getDelayType() + pNextXY->getDelayType())/2;
    avgDepth = (pPrevXY->getDelayDepth() + pNextXY->getDelayDepth())/2;
    avgX = (pPrevXY->getDelayPowerRateX() + pNextXY->getDelayPowerRateX())/2;
    avgY = (pPrevXY->getDelayPowerRateY() + pNextXY->getDelayPowerRateY())/2;
    m_oAdjXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjXY.setDelayPowerRate( avgX, avgY );
    
    // 進入XY
    avgX = (pPrevInXY->getOfsMinRateXForH() + pNextInXY->getOfsMinRateXForH())/2;
    avgY = (pPrevInXY->getOfsMinRateYForH() + pNextInXY->getOfsMinRateYForH())/2;
    m_oAdjInXY.setOfsMinRateXYForH( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMaxRateXForH() + pNextInXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevInXY->getOfsMaxRateYForH() + pNextInXY->getOfsMaxRateYForH())/2;
    m_oAdjInXY.setOfsMaxRateXYForH( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMinRateXForV() + pNextInXY->getOfsMinRateXForV())/2;
    avgY = (pPrevInXY->getOfsMinRateYForV() + pNextInXY->getOfsMinRateYForV())/2;
    m_oAdjInXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevInXY->getOfsMaxRateXForV() + pNextInXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevInXY->getOfsMaxRateYForV() + pNextInXY->getOfsMaxRateYForV())/2;
    m_oAdjInXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevInXY->getDelayType() + pNextInXY->getDelayType())/2;
    avgDepth = (pPrevInXY->getDelayDepth() + pNextInXY->getDelayDepth())/2;
    avgX = (pPrevInXY->getDelayPowerRateX() + pNextInXY->getDelayPowerRateX())/2;
    avgY = (pPrevInXY->getDelayPowerRateY() + pNextInXY->getDelayPowerRateY())/2;
    m_oAdjInXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjInXY.setDelayPowerRate( avgX, avgY );

    // 出立XY
    avgX = (pPrevOutXY->getOfsMinRateXForH() + pNextOutXY->getOfsMinRateXForH())/2;
    avgY = (pPrevOutXY->getOfsMinRateYForH() + pNextOutXY->getOfsMinRateYForH())/2;
    m_oAdjOutXY.setOfsMinRateXYForH( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMaxRateXForH() + pNextOutXY->getOfsMaxRateXForH())/2;
    avgY = (pPrevOutXY->getOfsMaxRateYForH() + pNextOutXY->getOfsMaxRateYForH())/2;
    m_oAdjOutXY.setOfsMaxRateXYForH( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMinRateXForV() + pNextOutXY->getOfsMinRateXForV())/2;
    avgY = (pPrevOutXY->getOfsMinRateYForV() + pNextOutXY->getOfsMinRateYForV())/2;
    m_oAdjOutXY.setOfsMinRateXYForV( avgX, avgY );
    
    avgX = (pPrevOutXY->getOfsMaxRateXForV() + pNextOutXY->getOfsMaxRateXForV())/2;
    avgY = (pPrevOutXY->getOfsMaxRateYForV() + pNextOutXY->getOfsMaxRateYForV())/2;
    m_oAdjOutXY.setOfsMaxRateXYForV( avgX, avgY );

    avgType = (pPrevOutXY->getDelayType() + pNextOutXY->getDelayType())/2;
    avgDepth = (pPrevOutXY->getDelayDepth() + pNextOutXY->getDelayDepth())/2;
    avgX = (pPrevOutXY->getDelayPowerRateX() + pNextOutXY->getDelayPowerRateX())/2;
    avgY = (pPrevOutXY->getDelayPowerRateY() + pNextOutXY->getDelayPowerRateY())/2;
    m_oAdjOutXY.setDelayType( (eDELAY_LOG)avgType, avgDepth );
    m_oAdjOutXY.setDelayPowerRate( avgX, avgY );
};
#endif
