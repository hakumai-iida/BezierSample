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
#define CHECK_PAL_OFS_STRIPE_A  ePAL_OFS_C_STRIPE_A
#define CHECK_PAL_OFS_STRIPE_B  ePAL_OFS_C_STRIPE_B
#define CHECK_PAL_OFS_FRILL_A   ePAL_OFS_C_FRILL_A
#define CHECK_PAL_OFS_FRILL_B   ePAL_OFS_C_FRILL_B

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
    //#define FRILL_PAL
#else
    // ストライプテスト
    //#define STRIPE_TEST
    //#define STRIPE_SUB_TEST
#endif

// 色を塗らない（タッチストローク確認用）
#define NO_PAINT

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
#ifdef DELAY_TEST
    CAdjustablePoint* pAdj;
#endif

    //------------------------------
    // 塗りワーク
    //------------------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;

    // ブラシ＆パレット
    eBRUSH brush = CHECK_BRUSH;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;
    
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
#else
    //-------------------------
    // 中心のフック
    //-------------------------
    // CLOTH
    pLOD = CLineObjectData::Alloc();
    pLOD->setFlagOn( eLOD_FLAG_DOT );
    m_pLayerTouch->addData( pLOD );
    
    // 点０：
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    pLOD->addData( pAP );

    #ifdef DELAY_TEST
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 8 );
    pAdj->setDelayPowerRate( -2000, 2000 );
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
    pAP->setFlagOn( eAPD_FLAG_REVERSE_FRILL_HOOK );
    #endif
    #ifdef FRILL_PAL
    pAP->setStripeOrFrillMainPalOfsId( CHECK_PAL_OFS_FRILL_A );
    pAP->setStripeOrFrillSubPalOfsId( CHECK_PAL_OFS_FRILL_B );
    #endif

    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pAP->setFrillMainId( FRILL_MAIN );
    pAP->setFrillSubId( FRILL_SUB );
    
    #if 0
    pAP->setTouchBase( 10000, 2000, 2700 );
    pAP->setTouchFront( 10000, 0, -3600, 12, 1 );
    #endif

    #if 0
    pAP->setTouchBase( 0, 2000, 2700 );
    pAP->setTouchBack( 10000, 2000, 3600, 12, 1 );
    #endif
    
    #if 0
    // ここで指定するサイズは高さにのみ影響する
    pAP->setTouchBase( 5000, 2000, 900 );
    pAP->setTouchFront( 10000, 1000, -1800, 6, 0 );
    pAP->setTouchBack( 10000, -1000, 1800, 6, 1 );  // スキップは後側で指定する
    #endif
    
    #if 1
    // サイズのオフセットにより正負を跨ぐと反転する
    //（※フリルで正負を反転するような需要はないとは思うが注意すること）
    pAP->setTouchBase( 5000, 2000, 900 );
    pAP->setTouchFront( 10000, -5000, -1800, 6, 0 );
    pAP->setTouchBack( 10000, 3000, 1800, 6, 1 );  // スキップは後側で指定する
    #endif

    #if 0
    // 幅の調整をしたい場合は[setFrillWithRate]で指定する
    pAP->setFrillWidthRate( 15000, 5000 );
    pAP->setTouchBase( 5000, 2000, 900 );
    pAP->setTouchFront( 10000, 500, -1800, 6, 0 );
    pAP->setTouchBack( 10000, -500, 1800, 6, 1 );  // スキップは後側で指定する
    #endif

    pLOD->addData( pAP );
    
#else   // FRILL_TEST
    
    #ifdef STRIPE_TEST
    //-----------------------------
    // [ストライプ]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();

    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    #ifdef STRIPE_SUB_TEST
    pAP->setTouchTargetSubId( eSTROKE_TOUCH_TARGET_TEMP_B );
    #endif

    pAP->setStripeOrFrillMainPalOfsId( CHECK_PAL_OFS_STRIPE_A );
    pAP->setStripeOrFrillSubPalOfsId( CHECK_PAL_OFS_STRIPE_B );

    #ifdef STRIPE_SUB_TEST
    pAP->set( 0,0, 0,0, 0,-500 );
    
    // 中央から前後に半分の領域までタッチをつなげて塗りつぶす
    pAP->setTouchBase( 5000, -10000, 0 );
    pAP->setTouchFront( 5000, 10000, -900, 5, 0, 3000 );
    pAP->setTouchBack( 5000, -10000, 900, 5, 0, 3000 );
    #else
    pAP->set( 0,0, 0,0, 200,0 );
    
    // 外周から中心へ向かってタッチを伸ばして塗りつぶす
    //（※全領域を指定すると最後のストロークが重複する＆そのストロークをスキップすると塗りつぶされないので元の色が残る）
    pAP->setTouchBase( 10000, -10000, 900 );
    pAP->setTouchFront( 10000, 20000, -3600, 16, 1, 0 );
    #endif

    pLOD->addData( pAP );

    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    #ifdef STRIPE_SUB_TEST
    pAP->set( 0,-5000, 0,-500, 0,0 );
    #else
    pAP->set( 0,-1500, 200,0, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
    #endif
    
    pLOD->addData( pAP );
    
    #else   // STRIPE_TEST
    
    //-----------------------------
    // [タッチ]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 200,0 );
    pAP->setStroke( 20000,0, 0,0 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );

#if 0
    // 末尾（左のAP）ろから先頭へぐるりと１周（※最後の１タッチはスキップ）
    pAP->setTouchBase( 10000, 10000, 900 );
    pAP->setTouchFront( 10000, 0, -3600, 24, 1, 0 );
#endif

#if 0
    // 先頭（左のAP）から末尾へぐるりと１周（※二重線／最後の１タッチはスキップ）
    pAP->setTouchBase( 0, 10000, 900 );
    pAP->setTouchBack( 10000, 0, 3600, 24, 1, 3000 );
#endif
    
#if 1
    // 中央（右のAP）から前後（左のAP）へそれぞれ半周（サイズの増減／二重線／最後の１タッチはスキップ）
    pAP->setTouchBase( 5000, 10000, 2700 );
    pAP->setTouchFront( 10000, -3000, -1800, 12, 0, 2000 );
    pAP->setTouchBack( 10000, 3000, 1800, 12, 1, 2000 );
#endif

#if 0
    // サイズ確認：基本サイズが負＆オフセットで０をまたぐ（※負になるとタッチが反転する＝外側へ伸びるので注意）
    pAP->setTouchBase( 5000, -10000, 2700 );
    pAP->setTouchFront( 10000, 20000, -1800, 12, 0, 2000 );
    pAP->setTouchBack( 10000, -20000, 1800, 12, 1, 2000 );
#endif
       
    pLOD->addData( pAP );
    
    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-1500, 200,0, 0,0 );
    
    pLOD->addData( pAP );
    
    #endif  // STRIPE_TEST
    
#endif      // FRILL_TEST
}
