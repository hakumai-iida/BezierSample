/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoopForTouch.cpp [共通環境]
  |	Comment:	ベジェテストループ：タッチ確認
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
#define CHECK_BRUSH             eBRUSH_C_BASE
#define CHECK_BRUSH_FOR_TOUCH   eBRUSH_C_BASE
#define CHECK_PAL_OFS           ePAL_OFS_C_SKIN
#if 0
#define CHECK_PAL_OFS_STRIPE_A  ePAL_OFS_C_STRIPE_A
#define CHECK_PAL_OFS_STRIPE_B  ePAL_OFS_C_STRIPE_B
#define CHECK_PAL_OFS_FRILL_A   ePAL_OFS_C_FRILL_A
#define CHECK_PAL_OFS_FRILL_B   ePAL_OFS_C_FRILL_B
#else
#define CHECK_PAL_OFS_STRIPE_A  ((ePAL_OFS)0)
#define CHECK_PAL_OFS_STRIPE_B  ((ePAL_OFS)0)
#define CHECK_PAL_OFS_FRILL_A   ((ePAL_OFS)0)
#define CHECK_PAL_OFS_FRILL_B   ((ePAL_OFS)0)
#endif

//-------------------------------------------------
// 動作指定
//-------------------------------------------------
// フリルテスト
#define FRILL_TEST

#ifdef FRILL_TEST
    // フリル素材
    //#define FRILL_MAIN  eSTROKE_FRILL_TEST_CIRCLE
    //#define FRILL_SUB   eSTROKE_FRILL_TEST_TRIANGLE
    #define FRILL_MAIN  eSTROKE_FRILL_FLUFFY_SQUARE_MAIN
    #define FRILL_SUB   eSTROKE_FRILL_FLUFFY_SQUARE_SUB

    // 向きのスワップ
    //#define FRILL_DIR_SWAP

    // 色の指定
    #define FRILL_PAL
#else
    // ストライプテスト
    #define STRIPE_TEST
    //#define STRIPE_SUB_TEST
#endif

// フックテスト（タッチとストライプのストロークを円の中心へフックする）
//#define HOOK_TEST

// 色を塗らない（タッチストローク確認用）
//#define NO_PAINT

// 遅延テスト（移動値による形状変化確認用）
//#define DELAY_TEST

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
//---------------------------------
// [サブ] タッチ確認：バッファ確保：基本
// 処理対象：[m_pLayerTouch]
//---------------------------------
void CBezierTestLoop::allocForLayerForTouch( void ){
    // レイヤー確保
    m_pLayerTouch = CLayerData::Alloc();

    //------------------------------
    // 線ワーク
    //------------------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    CAdjustablePoint* pAdj;

    //------------------------------
    // 塗りワーク
    //------------------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;

    // ブラシ＆パレット
    eBRUSH brush = CHECK_BRUSH;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;
    
#ifdef HOOK_TEST
    //----------------------
    // フックドット確保
    //----------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setFlagOn( eLOD_FLAG_DOT );
    pLOD->setBrushId( brush );
    m_pLayerTouch->addData( pLOD );
    
    // 点０：フック
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    pLOD->addData( pAP );
#endif
       
    //----------------------
    // 線オブジェクト確保＆設定
    //----------------------
    // CLOTH
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef NO_PAINT
    pLOD->setFlagOn( eLOD_FLAG_RESET_GUIDE_AFTER_STROKE );
#endif
    m_pLayerTouch->addData( pLOD );
    
    // 点０：タッチＡ
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pLOD->addData( pAP );

#ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
#endif

    // 点１：タッチＡ
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pLOD->addData( pAP );

#ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 1 );
    pAdj->setDelayPowerRate( 2500/2, 2500/3 );
#endif

    // 点２：タッチＡ
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pLOD->addData( pAP );

#ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
#endif

    // 点３：タッチＡ
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pLOD->addData( pAP );

#ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 4 );
    pAdj->setDelayPowerRate( 2500, 2500 );
#endif

#ifdef STRIPE_SUB_TEST
    //-------------------------
    // サブ線オブジェクト確保＆設定
    //-------------------------
    // CLOTH
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0, 0, 5000, 5000 );
    m_pLayerTouch->addData( pLOD );
    
    // 点０：タッチＢ
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );
    pLOD->addData( pAP );

    #ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
    #endif

    // 点１：タッチＢ
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );
    pLOD->addData( pAP );

    #ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 1 );
    pAdj->setDelayPowerRate( 2500/2, 2500/3 );
    #endif

    // 点２：タッチＢ
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );
    pLOD->addData( pAP );

    #ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
    #endif

    // 点３：タッチB
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );
    pLOD->addData( pAP );

    #ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 4 );
    pAdj->setDelayPowerRate( 2500, 2500 );
    #endif
#endif
    
#ifndef NO_PAINT
    //---------------------
    // ペイントの確保
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerTouch->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0, 0 );
    pPOD->addData( pFP );
#endif

    //------------------------------
    // タッチ／フリルオブジェクト確保
    //------------------------------
    brush = CHECK_BRUSH_FOR_TOUCH;
    
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_TOUCH );
#ifdef FRILL_TEST
    pLOD->setFlagOn( eLOD_FLAG_TOUCH_IS_FRILL );
#else
    #ifndef NO_PAINT
    pLOD->setTestPalOfsId( bucketPalOfs );
    #endif
    #ifdef STRIPE_TEST
    pLOD->setFlagOn( eLOD_FLAG_TOUCH_IS_STRIPE );
    pLOD->setFlagOn( eLOD_FLAG_RESET_GUIDE_AFTER_STROKE );  // ガイドの削除（※ゴミが悪さをしないように）
    #endif
#endif
    m_pLayerTouch->addData( pLOD );

#ifdef FRILL_TEST
    //-----------------------------
    // [フリル]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,0 );
    #ifdef FRILL_DIR_SWAP
    pAP->setFlagOn( eAPD_FLAG_JOINT_OR_FRILL_DIR_SWAP );
    #endif
    #ifdef FRILL_PAL
    pAP->setStripeOrFrillMainPalOfsId( CHECK_PAL_OFS_FRILL_A );
    pAP->setStripeOrFrillSubPalOfsId( CHECK_PAL_OFS_FRILL_B );
    #endif

    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pAP->setFrillMainId( FRILL_MAIN );
    pAP->setFrillSubId( FRILL_SUB );
    //pAP->setTouchRandomOfs( 100, 100 );
    //pAP->setFrillWidthRate( 8000, 12000 );
    
    #if 1
    pAP->setTouchBase( 00000, 2000, 0 );
    pAP->setTouchBack( 10000, 0, 10000, 12, 1 );
    #else
    pAP->setTouchBase( 5000, 2000, 0 );
    pAP->setTouchFront( 10000, -3000, 10000, 6, 0 );
    pAP->setTouchBack( 10000, -3000, 10000, 6, 1 );
    #endif
    pLOD->addData( pAP );
    
#else   // FRILL_TEST
    
    #ifdef STRIPE_TEST
    //-----------------------------
    // [ストライプ]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    #ifdef STRIPE_SUB_TEST
    pAP->set( 0,0, 0,0, -1000,0 );
    #else
    pAP->set( 0,0, 0,0, 0,-500 );
    #endif

    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
#ifdef STRIPE_SUB_TEST
    pAP->setTouchTargetSubId( eSTROKE_TOUCH_TARGET_TEMP_B );
#endif

    pAP->setStripeOrFrillMainPalOfsId( CHECK_PAL_OFS_STRIPE_A );
    //pAP->setStripeOrFrillSubPalOfsId( CHECK_PAL_OFS_STRIPE_B );

    //pAP->setTouchRandomOfs( 300, 300 );
    //pAP->setTouchSkipRate( 5000, 5000 );

#if 1
    pAP->setTouchBase( 10000, 10000, 0 );
    pAP->setTouchFront( 10000, 0, 10000, 16, 1, 2000 );
#else
    pAP->setTouchBase( 6000, 10000, 0 );
    pAP->setTouchFront( 3000, 2000, 2000, 8, 0, 3000 );
    pAP->setTouchBack( 3000, -3000, 2000, 8, 0, 3000 );
#endif

    pLOD->addData( pAP );

    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    #ifdef STRIPE_SUB_TEST
    pAP->set( 0,-5000, -1000,0, 0,0 );
    #else
    pAP->set( -6000,-1000, 0,-500, 0,0 );
    #endif
    
    #ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    #endif
    pLOD->addData( pAP );
    
    #else   // STRIPE_TEST
    
    //-----------------------------
    // [タッチ]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 200,0 );
    pAP->setStroke( 0,20000, 0,0 );

    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    //pAP->setTouchRandomOfs( 200, 0/*500*/ );
    //pAP->setTouchSkipRate( 0/*3000*/, 3000 );

#if 0
    pAP->setTouchBase( 10000, 10000, 0 );
    pAP->setTouchFront( 10000, 0, 10000, 24, 1, 2000 );
#else
    pAP->setTouchBase( 5000, 10000, 0 );
    pAP->setTouchFront( 10000, -5000, 10000, 12, 0, 2000 );
    pAP->setTouchBack( 10000, 5000, -10000, 12, 1, 2000 );
#endif
       
    pLOD->addData( pAP );
    
    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-1500, 200,0, 0,0 );
    #ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    #endif
    
    pLOD->addData( pAP );
    
    #endif  // STRIPE_TEST
    
#endif      // FRILL_TEST
}
