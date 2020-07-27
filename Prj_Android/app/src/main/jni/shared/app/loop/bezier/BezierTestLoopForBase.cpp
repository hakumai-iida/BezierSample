/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoopForBase.cpp [共通環境]
  |	Comment:	ベジェテストループ：基本確認
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
#define CHECK_BRUSH_DOT         eBRUSH_C_BASE
#define CHECK_PAL_OFS           ePAL_OFS_C_SKIN
#define CHECK_PAL_OFS_OPTION    ePAL_OFS_C_HAIR
#define CHECK_PAL_OFS_STRIPE_A  ePAL_OFS_C_DRESS
#define CHECK_PAL_OFS_STRIPE_B  ePAL_OFS_C_SYMBOL_SUB

//-------------------------------------------------
// 動作指定
//-------------------------------------------------
// ドット確認
//#define BASE_FOR_DOT_TEST

// ストローク消去確認
#define ERASE_TEST

// 塗りつぶしオプション確認
#define FILL_OPTION_TEST

// 塗り出力領域へのストローク確認
#define BASE_STROKE_ON_OUT_TEST

// エッジ調整（※アンカーポイントの折り返しによる角度が尖っている場合の先端調整）
#define BASE_FOR_EDGE_FILL_TEST

// タッチ確認
#define BASE_FOR_TOUCH_TEST
//#define BASE_FOR_TOUCH_TEST_WITH_STRIPE
//#define BASE_FOR_TOUCH_TEST_WITH_FRILL

// マスク確認
#define BASE_FOR_MASK_TEST

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
// [サブ] ベジェ確認：バッファ確保：基本
// 処理対象：[m_pLayerBase]
//---------------------------------
void CBezierTestLoop::allocForLayerForBase( void ){
    // レイヤー確保
    m_pLayerBase = CLayerData::Alloc();

    //------------------------------
    // 線ワーク
    //------------------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    CAdjustablePoint* pAdj;

#ifdef BASE_FOR_DOT_TEST
    eBRUSH brush = CHECK_BRUSH_DOT;
#else
    eBRUSH brush = CHECK_BRUSH;
#endif

    //------------------------------
    // 塗りワーク
    //------------------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS_OPTION;
       
#ifdef BASE_FOR_MASK_TEST
    //----------------------
    // マスクの線
    //----------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 1250, -1250, 8000,8000 );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pLOD->addData( pAP );
    
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 1000, 1000/2 );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 1 );
    pAdj->setDelayPowerRate( 1000/2, 1000/3 );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 1000, 1000/2 );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 4 );
    pAdj->setDelayPowerRate( 1000, 1000 );

    //----------------------
    // 塗りオブジェクト確保＆設定
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 1250,-1250, bucketPalOfs );
    pFP->setOption( eFILL_OPTION_MASK );
    pPOD->addData( pFP );
#endif

#ifdef FILL_OPTION_TEST
    //--------------------------
    // 影を落とす要素：ライン
    //--------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( -2000,-500, 5000,5000 );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,0, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,0, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //--------------------------
    // 影を落とす要素：ペイント
    //--------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -2000,-1500, bucketPalOfs );
    //pFP->setOption( eFILL_OPTION_RESERVE_BRIGHT );
    pFP->setOption( eFILL_OPTION_RESERVE_DARK );
    pFP->setOptionOfsXY( +10, +10 );
    pPOD->addData( pFP );
#endif
    
#ifdef BASE_FOR_MASK_TEST
/*
    //--------------------------
    // マスクのクリア（ストローク）
    //--------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 2000, 2000, 10000,10000 );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_RESET_FULL );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
*/
#endif

    //--------------------------
    // パレット戻す
    //--------------------------
    bucketPalOfs = CHECK_PAL_OFS;

    //---------------------
    // オブジェクト確保＆設定
    //---------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
#ifdef BASE_FOR_DOT_TEST
    pLOD->setFlagOn( eLOD_FLAG_DOT );
#else
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#endif

    m_pLayerBase->addData( pLOD );
    
    //-----------------------------
    // 点０(A)
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -3600, 900, 0, 2000, 0, -2000 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない
#else
    pAP->setStrokeSize( 10000, 30000,6666, 0,0 );
#endif
#ifdef BASE_FOR_TOUCH_TEST
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_A0 );
#endif
    pLOD->addData( pAP );
    
    // 調整値テスト
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST );
    pAdj->setDelayPowerRate( 2000, 2000 );
    pAdj->setOfsMaxRateXYForH( -1000, 0 );
    pAdj->setOfsMinRateXYForH(  +500, 0 );
    pAdj->setOfsMaxRateXYForV( 0, 0 );
    pAdj->setOfsMinRateXYForV( 0, 0 );

    //-----------------------------
    // 点１(B)
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1400, -1800, -1000, 0, 1000, 0 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない
#else
    pAP->setStrokeSize( 10000, 0,0, 2500,3333 );
#endif
#ifdef BASE_FOR_TOUCH_TEST
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_B0 );
#endif
    pLOD->addData( pAP );
    
    // 調整値テスト
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST );
    pAdj->setDelayPowerRate( 1000, 1000 );
    pAdj->setOfsMaxRateXYForH( -500, 0 );
    pAdj->setOfsMinRateXYForH( +250, 0 );
    pAdj->setOfsMaxRateXYForV( 0, -800 );
    pAdj->setOfsMinRateXYForV( 0, +400 );

    //-----------------------------
    // 点２(C)
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000, -800, 1600, -800, -1600, 800 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない
#else
    pAP->setStrokeSize( 20000, 2500,3333, 2500,3333 );
#endif
#ifdef BASE_FOR_TOUCH_TEST
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_C0 );
#endif
    pLOD->addData( pAP );

    // 調整値テスト
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST );
    pAdj->setDelayPowerRate( 500, 500 );
    pAdj->setOfsMaxRateXYForH( +500, 0 );
    pAdj->setOfsMinRateXYForH( -250, 0 );
    pAdj->setOfsMaxRateXYForV( 0, -500 );
    pAdj->setOfsMinRateXYForV( 0, +250 );

    //-----------------------------
    // 点３(D)
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 3200, 1600, -2000, 0, 0, 400 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画される
#else
    pAP->setStrokeSize( 30000, 2500,3333, 0,0 );
#endif
#ifdef BASE_FOR_TOUCH_TEST
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_D0 );
#endif
    pLOD->addData( pAP );

    // 調整値テスト
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST );
    pAdj->setDelayPowerRate( 2000, 2000 );
    pAdj->setOfsMaxRateXYForH( +1000, 0 );
    pAdj->setOfsMinRateXYForH(  -500, 0 );
    pAdj->setOfsMaxRateXYForV( 0, +500 );
    pAdj->setOfsMinRateXYForV( 0, -250 );

    //-----------------------------
    // 点４(E)
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -300, 3000, 800, 0, -800, 0 );
#ifdef BASE_FOR_EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない
#else
    pAP->setStrokeSize( 30000, 0,0, 0,0 );
#endif
#ifdef BASE_FOR_TOUCH_TEST
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_E0 );
#endif
    pLOD->addData( pAP );

    // 調整値テスト
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST );
    pAdj->setDelayPowerRate( 3000, 3000 );
    pAdj->setOfsMaxRateXYForH( -200, 0 );
    pAdj->setOfsMinRateXYForH( +100, 0 );
    pAdj->setOfsMaxRateXYForV( 0, +1000 );
    pAdj->setOfsMinRateXYForV( 0, -500 );

#ifdef ERASE_TEST
    //------------------------------
    // 消去ストローク確保＆設定
    //------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点Ａ０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-4000, 0,0, -1000,0 );
    pAP->setFlagOn( eAPD_FLAG_DOT_ERASE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,4000, 1000,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_DOT_ERASE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点Ａ２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,0, 0,-1000, -000,0 );
    pAP->setFlagOn( eAPD_FLAG_DOT_ERASE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
#endif

#ifndef BASE_FOR_DOT_TEST
    //---------------------
    // オブジェクト確保＆設定
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    //-----------------------------
    // 塗り０
    //-----------------------------
    pFP = CFillPointData::Alloc();
    pFP->set( -2000, 1000, bucketPalOfs );
    pPOD->addData( pFP );

    //-------------------------------------
    // タッチテスト
    //-------------------------------------
    #ifdef BASE_FOR_TOUCH_TEST
    
    //------------------------------
    // タッチ／フリルオブジェクト確保＆設定
    //------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_TOUCH );
    #ifdef BASE_FOR_TOUCH_TEST_WITH_FRILL
    pLOD->setFlagOn( eLOD_FLAG_TOUCH_IS_FRILL );
    #else
    pLOD->setTestPalOfsId( bucketPalOfs );
    #ifdef BASE_FOR_TOUCH_TEST_WITH_STRIPE
    pLOD->setFlagOn( eLOD_FLAG_TOUCH_IS_STRIPE );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_DRAW_BEFORE_STROKE );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE );  // ガイドは都度消す
    #endif
    #endif
    m_pLayerBase->addData( pLOD );

    #ifdef BASE_FOR_TOUCH_TEST_WITH_FRILL
    {
        //-----------------------------
        // [フリル]:点Ａ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,0 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_A0 );
        pAP->setFrillId( eSTROKE_FRILL_TEST_TRIANGLE );
        pAP->setFrillBase(  5000, 1500, 3050 );
        pAP->setFrillFront( 10000, 1000, 2650 );
        pAP->setFrillBack( 10000, 1000, 3750 );
        pLOD->addData( pAP );

        //-----------------------------
        // [フリル]:点Ｂ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,0 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_B0 );
        pAP->setFrillId( eSTROKE_FRILL_TEST_CIRCLE );
        pAP->setFrillBase(  5000, 1500, 0 );
        pAP->setFrillFront( 10000, 1000, 0 );
        pAP->setFrillBack( 10000, 1000, 0 );
        pLOD->addData( pAP );

        //-----------------------------
        // [フリル]:点Ｃ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,0 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_C0 );
        pAP->setFrillId( eSTROKE_FRILL_TEST_SQUARE );
        pAP->setFrillBase(  2500, 1500, 900 );
        pAP->setFrillFront( 10000, 1000, 1800 );
        pAP->setFrillBack( 10000, 1000, -150 );
        pLOD->addData( pAP );
    }
    #else
    {
        #ifdef BASE_FOR_TOUCH_TEST_WITH_STRIPE
        //-----------------------------
        // [ストライプ]:点Ａ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,500 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_A0 );
        
        pAP->setTouchBase( 5000, 10000, 0 );                // 中央を基本に１００％で描画（※回転０度）
        pAP->setTouchFront( 8500, 10000, 0, 400, 0, 0 );    // 前方８５％まで１００％で描画＆幅４％間隔（※回転０度）
        pAP->setTouchBack( 8500, 10000, 0, 400, 0, 0 );     // 広報８５％まで１００％で描画＆幅４％間隔（※回転０度）

#if 0
        pAP->setFlagOn( eAPD_FLAG_STRIPE_FILL_FRONT_EDGE );
        pAP->setFlagOn( eAPD_FLAG_STRIPE_FILL_BACK_EDGE );
#endif
        pAP->setStripeMainPalOfsId( CHECK_PAL_OFS_STRIPE_A );
        pAP->setStripeSubPalOfsId( CHECK_PAL_OFS_STRIPE_B );
        pLOD->addData( pAP );

        // 点Ａ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( 6000,200, -200,0, 0,0 );
        pLOD->addData( pAP );
        
        #else
        
        //-----------------------------
        // [タッチ]:点Ａ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,-500 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_A0 );
        pAP->setTouchBase( 0, 10000, 0 );                   // ０％開始（※後方のみ）
        pAP->setTouchBack( 10000, 5000, 0, 100, 200, 0 );   // 後方１００％まで１＋２％のステップで５０％の長さまで
        pLOD->addData( pAP );
        
        // 点Ａ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( 2000,200, -200,0, 0,0 );
        pLOD->addData( pAP );

        //-----------------------------
        // [タッチ]:点Ｂ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 200,0 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_B0 );
        pAP->setTouchBase( 7000, 5000, 0 );                    // ７０％開始（※前方のみ）
        pAP->setTouchFront( 10000, 10000, 0, 100, 100, 0 );    // 前方１００％まで１＋１％のステップで１００％の長さまで
        pLOD->addData( pAP );

        // 点Ｂ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,500, 100,0, 0,0 );
        pLOD->addData( pAP );

        //-----------------------------
        // [タッチ]:点Ｃ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,-100 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_C0 );
        pAP->setTouchBase( 5000, 10000, 0 );                    // ５０％開始（※中央から）
        pAP->setTouchFront( 8000, 1000, 0, 100, 0, 0 );         // 前方８０％まで１％のステップで１０％の長さまで
        pAP->setTouchBack( 8000, 5000, 0, 100, 0, 0 );          // 後方８０％まで１％のステップで５０％の長さまで
        pLOD->addData( pAP );

        // 点Ｃ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( -1000,0, 0,0, 100,0 );
        pLOD->addData( pAP );

        //-----------------------------
        // [タッチ]:点Ｄ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,0 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_D0 );
        pAP->setTouchBase( 10000, 10000, 0 );                   // ５０％開始（中央から）
        pAP->setTouchFront( 10000, 10000, -500, 200, 0, 0 );    // 前方１００％まで角度を加えながら２％のステップで
        pAP->setTouchBack( 10000, 10000, 500, 200, 0, 0 );      // 後方１００％まで角度を加えながら２％のステップで
        pLOD->addData( pAP );

        // 点Ｄ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( -500,-500, 0,0, 0,0 );
        pLOD->addData( pAP );

        //-----------------------------
        // [タッチ]:点Ｅ０
        //-----------------------------
        pAP = CAnchorPointData::Alloc();
        pAP->set( 0,0, 0,0, 0,500 );
        pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_E0 );
        pAP->setTouchBase( 5000, 10000, -300 );
        pAP->setTouchFront( 10000, 10000, 0, 200, 0, 0 );
        pAP->setTouchBack( 10000, 5000, -600, 200, 0, 0 );
        pLOD->addData( pAP );

        // 点Ｅ１
        pAP = CAnchorPointData::Alloc();
        pAP->set( 2000,200, -200,0, 0,0 );
        pLOD->addData( pAP );
        #endif
    }
    #endif
    // タッチテスト終了
    #endif
#endif

#ifdef FILL_OPTION_TEST
    //---------------------------------
    // ラインオブジェクト確保＆設定：明色塗り
    //（※範囲目一杯に描いているので拡大厳禁）
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 点０：明色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：明色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-4000, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pLOD->addData( pAP );
    
    // 点２：明色塗り）
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,+4000, -3000,-3000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：明色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // ペイントオブジェクト確保＆設定：明色塗り
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：明色塗り
    pFP = CFillPointData::Alloc();
    pFP->set( -2500,0, bucketPalOfs );
    pFP->setOption( eFILL_OPTION_BRIGHT, eSTROKE_TOUCH_TARGET_TEMP_A );
    pPOD->addData( pFP );
    
    //---------------------------------
    // ラインオブジェクト確保＆設定：暗色塗り
    //（※範囲目一杯に描いているので拡大厳禁）
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 点０：暗色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：暗色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( +2000,-4000, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );
    pLOD->addData( pAP );
    
    // 点２：暗色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( +2000,+4000, -3000,-3000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：暗色塗り
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // ペイントオブジェクト確保＆設定：暗色塗り
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：暗色塗り
    pFP = CFillPointData::Alloc();
    pFP->set( +2500,0, bucketPalOfs );
    pFP->setOption( eFILL_OPTION_DARK, eSTROKE_TOUCH_TARGET_TEMP_B );
    pPOD->addData( pFP );

    //--------------------------------
    // ラインオブジェクト確保＆設定：明暗
    //--------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
 
    pLOD->setTempAdjust( -1500,-300, 2000, 2000 );
    
    // 点０：明暗
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：明暗
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２：明暗
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：明暗
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // ペイントオブジェクト確保＆設定
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：明暗
    pFP = CFillPointData::Alloc();
    pFP->set( -1500,0 );
    pFP->setOption( eFILL_OPTION_AREA_BRIGHT );
    //pFP->setOption( eFILL_OPTION_AREA_DARK );
    pPOD->addData( pFP );
    
    // 塗り１：ポイント（ハイライト）
    pFP = CFillPointData::Alloc();
    pFP->set( -1500+200,-200-300 );
    pFP->setOption( eFILL_OPTION_POINT_BRIGHT );
    pFP->setOptionStroke( 10, 0 );
    pPOD->addData( pFP );
    
    // 塗り２：ポイント（瞳孔）
    pFP = CFillPointData::Alloc();
    pFP->set( -1500,-300 );
    pFP->setOption( eFILL_OPTION_POINT_DARK );
    pFP->setOptionStroke( 15, 0 );
    pPOD->addData( pFP );

    //------------------------------------
    // ラインオブジェクト確保＆設定：広域（明るく）
    //（※範囲目一杯に描いているので拡大厳禁）
    //------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 点０：広域（明るく）
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：広域（明るく）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２：広域（明るく）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+2500, 0,0, -3000,-4000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：広域（明るく）
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+2500, +3000,-4000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );

    //--------------------------------------
    // ペイントオブジェクト確保＆設定：広域（明るく）
    //--------------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：広域（明るく）
    pFP = CFillPointData::Alloc();
    pFP->set( 0,-2000, bucketPalOfs );
    pFP->setOption( eFILL_OPTION_AREA_BRIGHT );
    pPOD->addData( pFP );
    
    //-----------------------------------
    // ラインオブジェクト確保＆設定：広域（暗く）
    //（※範囲目一杯に描いているので拡大厳禁）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 点０：広域（暗く）
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：広域（暗く）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２：広域（暗く）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-1500, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：広域（暗く）
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-1500, +3000,+3000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //------------------------------------
    // ペイントオブジェクト確保＆設定：広域（暗く）
    //------------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：広域（暗く）
    pFP = CFillPointData::Alloc();
    pFP->set( 0,+2500, bucketPalOfs );
    pFP->setOption( eFILL_OPTION_AREA_DARK );
    pPOD->addData( pFP );

    //----------------------------------
    // ペイントオブジェクト確保＆設定：全色
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：全色（明るく）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_COLOR_BRIGHT );
    pFP->setOptionOfsXY( -10, -10 );
    pPOD->addData( pFP );

    // 塗り１：全色（暗く）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_COLOR_DARK );
    pFP->setOptionOfsXY( +15, +15 );
    pPOD->addData( pFP );
#endif
    
#ifdef BASE_STROKE_ON_OUT_TEST
    //--------------------------
    // パレット差し替え
    //--------------------------
    bucketPalOfs = CHECK_PAL_OFS_OPTION;

    //---------------------------------
    // ラインオブジェクト確保＆設定：矩形（内）
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０：矩形（内）
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：矩形（内）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3000,-3000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );

    // 点２：矩形（内）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3000,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：矩形（内）
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // ペイントオブジェクト確保＆設定：矩形（内）
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：矩形（内）
    pFP = CFillPointData::Alloc();
    pFP->set( +2000,-2000, bucketPalOfs );
#ifdef BASE_FOR_MASK_TEST
    // マスクのクリア（塗り）
    pPOD->setFlagOn( ePOD_FLAG_GUIDE_RESET_FULL );
#endif
    pPOD->addData( pFP );

    //---------------------------------
    // ラインオブジェクト確保＆設定：矩形（外）
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０：矩形（外）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１：矩形（外）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２：矩形（外）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３：矩形（外）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1000,-1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // ペイントオブジェクト確保＆設定：矩形（外）
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setFlagOn( ePOD_FLAG_STAY_GUIDE_AFTER_FILL );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０：矩形（外）
    pFP = CFillPointData::Alloc();
    pFP->set( +2000,-2000, bucketPalOfs );
    pPOD->addData( pFP );
    
    //------------------------------
    // タッチ確保＆設定
    //------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_TOUCH );
    pLOD->setFlagOn( eLOD_FLAG_GUIDE_RESET_AFTER_STROKE );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    //-----------------------------
    // [タッチ]:点Ａ０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,-500 );
    pAP->setFlagOn( eAPD_FLAG_DOT_PUT_ON_FILL_OUT );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );
    pAP->setTouchBase( 0, 10000, 0 );                   // ０％開始（※後方のみ）
    pAP->setTouchBack( 10000, 5000, 0, 100, 200, 0 );   // 後方１００％まで１＋２％のステップで５０％の長さまで
    pLOD->addData( pAP );
    
    // 点Ａ１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,200, 200,0, 0,0 );
    pLOD->addData( pAP );
#endif
}
