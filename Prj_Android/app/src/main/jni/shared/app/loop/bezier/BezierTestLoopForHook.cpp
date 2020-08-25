/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoopForHook.cpp [共通環境]
  |	Comment:	ベジェテストループ：フック確認
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "BezierTestLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// ブラシ＆塗り指定
//-------------------------------------------------
#define CHECK_BRUSH     eBRUSH_C_BASE
#define CHECK_PAL_OFS   ePAL_OFS_C_SKIN

//-------------------------------------------------
// 動作指定
//-------------------------------------------------
// エッジ調整（※フックによる挙動確認用）
//#define BASE_FOR_EDGE_FILL_TEST

// フック[Hook]設定有効（オン／オフの効果確認用）
#define ENABLE_HOOK_SETTING

// 線修復[LineRepair]設定有効（オン／オフの効果確認用）
#define ENABLE_LINE_REPAIR_SETTING

// 遅延（※レイヤー直接描画はジョイントが使えないので角度のつなぎ調整は行われない）
#define DELAY_TEST

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------------------------
// [サブ] ベジェ確認：バッファ確保：フックＡ(腕)
// 処理対象：[m_pLayerHookA]
//-----------------------------------------
void CBezierTestLoop::allocForLayerForHookA( void ){
    // レイヤー確保
    m_pLayerHookA = CLayerData::Alloc();
    m_pLayerHookA->setFlagOn( eLAYER_FLAG_STAY_HOOK_TEMP );

#ifdef DELAY_TEST
    m_pLayerHookA->setDelayType( eDELAY_LOG_HAIR, 0 );
    m_pLayerHookA->setDelayPowerRateForXY( -1000, -1000 );
    m_pLayerHookA->setDelayPowerRateForRot( -500, -500 );
    m_pLayerHookA->setDelayPowerRateForScale( 3000, 3000 );
#endif
    
    //-----------------------
    // 線オブジェクト確保＆設定
    //-----------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    
    eBRUSH brush = CHECK_BRUSH;

    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_HOOK_SETTING
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( CHECK_PAL_OFS );
    #endif
#endif
    pLOD->setTempAdjust( 0,-2000, 5000,5000 );
    m_pLayerHookA->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-4000, 0,0, 0,0 );
#ifdef ENABLE_HOOK_SETTING
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
    #endif
#endif
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 2500 );
#endif
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,0, 0,0, 0,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
#endif
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 5000 );
#endif
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,0, 0,0, 0,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_C );
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
    #endif
#endif
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );
#endif
    pLOD->addData( pAP );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-4000, 0,0, 0,0 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 15000 );
#endif
    pLOD->addData( pAP );

    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;

    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerHookA->addData( pPOD );

    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,-3000 );
    pPOD->addData( pFP );
}

//----------------------------------------
// [サブ] ベジェ確認：バッファ確保：フックＢ（肘下）
// 処理対象：[m_pLayerHookB]
//----------------------------------------
void CBezierTestLoop::allocForLayerForHookB( void ){
    // レイヤー確保
    m_pLayerHookB = CLayerData::Alloc();
    m_pLayerHookB->setFlagOn( eLAYER_FLAG_STAY_HOOK_TEMP );

#ifdef DELAY_TEST
    m_pLayerHookB->setDelayType( eDELAY_LOG_BUST, 0 );
    m_pLayerHookB->setDelayPowerRateForXY( 1000, 1000 );
    m_pLayerHookB->setDelayPowerRateForRot( 200, 200 );
    m_pLayerHookB->setDelayPowerRateForScale( -5000, -5000 );
#endif
   
    //-----------------------
    // 線オブジェクト確保＆設定
    //-----------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    
    eBRUSH brush = CHECK_BRUSH;
    
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 5000,5000 );
    m_pLayerHookB->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,0, 0,1111, 0,-1111 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
#endif
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2000, -1111,0, 1111,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_B );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
#endif
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,0, 0,-1111, 0,1111 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_C );
#endif
    pLOD->addData( pAP );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2000, 1111,0, -1111,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_D );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
#endif
    pLOD->addData( pAP );

    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;
    
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerHookB->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
}

//-----------------------------------------
// [サブ] ベジェ確認：バッファ確保：フックC（下腕）
// 処理対象：[m_pLayerHookC]
//-----------------------------------------
void CBezierTestLoop::allocForLayerForHookC( void ){
    // レイヤー確保
    m_pLayerHookC = CLayerData::Alloc();
    
#ifdef DELAY_TEST
    m_pLayerHookC->setDelayType( eDELAY_LOG_SKIN, 0 );
    m_pLayerHookC->setDelayPowerRateForXY( 1000, -1000 );
    m_pLayerHookC->setDelayPowerRateForRot( 800, -800 );
    m_pLayerHookC->setDelayPowerRateForScale( 3000, -3000 );
#endif

    //-----------------------
    // 線オブジェクト確保＆設定
    //-----------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    
    eBRUSH brush = CHECK_BRUSH;
    
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_HOOK_SETTING
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( CHECK_PAL_OFS );
    #endif
#endif
    pLOD->setTempAdjust( -2000,0, 5000,5000 );
    m_pLayerHookC->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-2000, 0,0, 0,0 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 30000 );
#endif
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2000, 0,0, 0,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_B );
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    #endif
#endif
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 20000 );
#endif
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2000, 0,0, 0,0 );
#ifdef ENABLE_HOOK_SETTING
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_D );
    #ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
    #endif
#endif
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );
#endif
    pLOD->addData( pAP );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,2000, 0,0, 0,0 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 5000 );
#endif
    pLOD->addData( pAP );

    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;
    
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerHookC->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -3000,0 );
    pPOD->addData( pFP );
}
