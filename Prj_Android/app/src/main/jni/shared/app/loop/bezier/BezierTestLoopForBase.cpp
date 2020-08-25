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
#define CHECK_BRUSH_FOR_DOT     eBRUSH_SQUARE_XL
#define CHECK_BRUSH_FOR_ERASE   eBRUSH_CIRCLE_XL
#define CHECK_PAL_OFS           ePAL_OFS_C_SKIN
#define CHECK_PAL_OFS_OPTION    ePAL_OFS_C_HAIR

//-------------------------------------------------
// 動作指定
//-------------------------------------------------
// ドット確認
//#define DOT_TEST

// エッジ調整（※アンカーポイントの折り返しによる角度が尖っている場合の先端調整）
#define EDGE_FILL_TEST

// ストローク消去確認
#define ERASE_TEST

// マスク確認
#define MASK_TEST

// 出力領域への描画確認
#define ON_FILL_OUT_TEST

// 塗りつぶしオプション確認
#define FILL_OPTION_TEST_A

#define FILL_OPTION_TEST_B
#define FILL_OPTION_TEST_B_WITH_ANTI

#define FILL_OPTION_TEST_C
#define FILL_OPTION_TEST_C_WITH_ANTI

// ガイド確認
#define GUIDE_TEST
#define GUIDE_TEST_AT_PAINT

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

    //------------------------------
    // 塗りワーク
    //------------------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;

    // ブラシ＆パレット
    eBRUSH brush = CHECK_BRUSH;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;
    
#ifdef GUIDE_TEST
    //==================================
    // ガイド登録：左下領域の矩形
    //==================================
    // ブラシ
    brush = CHECK_BRUSH;

    //--------------------------
    // 補正矩形：ラインの確保
    //--------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+4000, 0,0, 0,0 );
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,+2000, 0,0, 0,0 );
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+2000, 0,0, 0,0 );
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    pLOD->addData( pAP );
#endif
       
#ifdef MASK_TEST
    //==================================
    // マスク領域：右上領域に円形でマスクする
    //==================================
    // ブラシ＆パレット指定
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS_OPTION;

    //----------------------
    // マスク：ラインの確保
    //----------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 1250, -1250, 6666,6666 );
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
    // マスク：ペイントの確保
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 1250,-1250 );
    pFP->setOption( eFILL_OPTION_MASK );
    pPOD->addData( pFP );
#endif

#ifdef FILL_OPTION_TEST_A
    //==========================================
    // 領域左上の矩形：歯を補正する矩形（※塗り予約確認）
    //==========================================
    // ブラシ＆パレット指定
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS_OPTION;

    //--------------------------
    // 補正矩形：ラインの確保
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
    // 補正矩形：ペイントの確保
    //--------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );

    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -2000,-1500 );
    pPOD->addData( pFP );

    // 明暗予約
    pFP = CFillPointData::Alloc();
    pFP->set( -2000,-1500 );
    //pFP->setOption( eFILL_OPTION_RESERVE, -1 );     // 明るく
    pFP->setOption( eFILL_OPTION_RESERVE, +1 );     // 暗く
    pFP->setOptionOfsXY( +8, +8 );
    pPOD->addData( pFP );
#endif
    
#ifdef MASK_TEST
/*
    // ブラシ指定（※塗りは無し）
    brush = CHECK_BRUSH;
    
    //==========================================
    // マスクのクリア：領域右側の矩形ストローク後にクリア
    //==========================================
    //--------------------------
    // マスクのクリア：ラインの確保
    //--------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 2000, 2000, 10000,10000 );
    pLOD->setFlagOn( eLOD_FLAG_RESET_MASK_AFTER_STROKE );
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

    //========================================
    // 基本図形（※歯のような形状）
    //========================================
    // ブラシ指定
#ifdef DOT_TEST
    brush = CHECK_BRUSH_FOR_DOT;
#else
    brush = CHECK_BRUSH;
#endif
    
    //---------------------
    // 歯：ラインの確保
    //---------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 20000 );
#ifdef DOT_TEST
    pLOD->setFlagOn( eLOD_FLAG_DOT );           // ドット指定
#else
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );    // パスを閉じる
#endif

    m_pLayerBase->addData( pLOD );
    
    //-----------------------------
    // 歯：点０
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -3600, 900, 0, 2000, 0, -2000 );
    pAP->setStroke( 0,20000, 0,5000 );
#ifdef EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない（角度が浅いので）
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
    // 歯：点１
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1400, -1800, -1000, 0, 1000, 0 );
    pAP->setStroke( 0,5000, 0,2500 );
#ifdef EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない（角度が浅いので）
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
    // 歯：点２
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000, -800, 1600, -800, -1600, 800 );
    pAP->setStroke( 3333,2500, 3333,5000 );
#ifdef EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない（角度が浅いので）
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
    // 歯：点３
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( 3200, 1600, -2000, 0, 0, 800 );
    pAP->setStroke( 0,5000, 0,10000 );
#ifdef EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画される（角度が深いので）
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
    // 歯：点４
    //-----------------------------
    pAP = CAnchorPointData::Alloc();
    pAP->set( -300, 3000, 1600, 0, -800, 0 );
    pAP->setStroke( 0,10000, 0,20000 );
#ifdef EDGE_FILL_TEST
    pAP->setStrokeEdgeFillSize( 10000 );    // ここは描画されない（角度が浅いので）
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
    //===============================================================
    // 消去ストローク：領域全体に曲線を引く
    //（※消去ストロークも色テストをするので塗られていない領域のみクリアする）
    //（※[FILL_OPTION]で左上に矩形が描いている場合、そこの線は欠けた感じになる）
    //===============================================================
    // ブラシ差し替え
    brush = CHECK_BRUSH_FOR_ERASE;
    
    //------------------------------
    // 消去ストローク：ラインの確保
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

#ifndef DOT_TEST
    //====================================
    // 歯を塗りつぶす（※ドット出力指定でなければ）
    //====================================
    // パレット指定
    bucketPalOfs = CHECK_PAL_OFS;

    //---------------------
    // 歯：ペイントの確保
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -2000, 1000 );
    pPOD->addData( pFP );

    #ifdef FILL_OPTION_TEST_A
    // 明暗：食い込み：１
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_INTO, -1 );     // 明るく
    pFP->setOptionOfsXY( -10, -10 );
    pPOD->addData( pFP );

    // 明暗：食い込み：２
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_INTO, +1 );     // 暗く
    pFP->setOptionOfsXY( +10, +10 );
    pPOD->addData( pFP );
    #endif
#endif

#ifdef ON_FILL_OUT_TEST
    //=====================================================
    // 出力済み領域への書き込みテスト：領域右上の重なり合う矩形
    // 奥の矩形にのみストロークが描画される（手前の領域には影響しない）
    //=====================================================
    // ブラシ＆パレット指定
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS_OPTION;

    //---------------------------------
    // 手前の矩形：ラインの確保
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3000,-3000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3000,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 手前の矩形：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( +2000,-2000 );
    #ifdef MASK_TEST
    // マスクのクリア（塗り）：これで以降の描画でマスクが無効（描き込みが可能）になる
    pPOD->setFlagOn( ePOD_FLAG_RESET_MASK_AFTER_FILL );
    #endif
    pPOD->addData( pFP );

    //------------------------------------
    // 奥の矩形（出力テストの領域）：ラインの確保
    //------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１（タッチ指定＝矩形の右辺に沿ってタッチが描画される）
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_H );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1000,-1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 奥の矩形：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setFlagOn( ePOD_FLAG_FOCUS_OUT_COL_AFTER_FILL );  // 出力色を残す（※ここの塗り領域が以降の処理の[ON_OUT_COL]の対象となる）
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( +2000,-2000 );
    pPOD->addData( pFP );
    
    //------------------------------
    // 奥の矩形：ラインの確保（タッチ）
    //------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTestPalOfsId( bucketPalOfs );
    pLOD->setFlagOn( eLOD_FLAG_TOUCH );
    m_pLayerBase->addData( pLOD );
    
    // [タッチ]: 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,0, 0,0, 0,-500 );
    pAP->setStroke( 0,20000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_DOT_ON_OUT_COL );             // 出力色上に描画する
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_H );
    pAP->setTouchBase( 0, 10000, 0 );
    pAP->setTouchBack( 10000, 0, 0, 8, 0, 0 );
    pLOD->addData( pAP );
    
    // [タッチ]: 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -5000,500, 0,1000, 0,0 );
    pLOD->addData( pAP );
    
    //------------------------------------
    // 中央の矩形（出力塗り）：ラインの確保
    //------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1500,-3500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3500,-3500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +3500,-1500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( +1500,-1500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 中央の矩形：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( ePAL_OFS_C_SKIN );
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( +2000,-2000 );
    pFP->setFlagOn( eFPD_FLAG_FILL_ON_OUT_COL );        // 出力色領域上に描画する
    pPOD->addData( pFP );
#endif

#ifdef FILL_OPTION_TEST_B
    //====================================
    // 歯に対する塗りオプション（※基本確認）
    //====================================
    // ブラシ＆パレット指定
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS;   // テストパレット

    //--------------------------------------
    // 明色塗り：ラインの確保
    //（※画面左側に大きめに描いているので拡大厳禁）
    //--------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 明色塗り：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 明色塗り：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-4000, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_A );   // 歯の左側を横断する線
    pLOD->addData( pAP );
    
    // 明色塗り：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,+4000, -3000,-3000, 0,0 );
    pLOD->addData( pAP );
    
    // 明色塗り：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 明色塗り：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs, -2 );  // 明るく
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 明色塗り：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -2500,0 );
    pPOD->addData( pFP );

    #ifdef FILL_OPTION_TEST_B_WITH_ANTI
    // 明色塗り：アンチ（※座標に意味はない）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_ANTI_STROKE_AFTER_FILL, -2 );
    pFP->setOptionTouch( eSTROKE_TOUCH_TARGET_TEMP_A );
    pFP->setOptionStroke( 2, 6 );
    pPOD->addData( pFP );
    #endif

    //-------------------------------------
    // 暗色塗り：ラインの確保
    //（※画面右側に大きめに描いているので拡大厳禁）
    //-------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 暗色塗り：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 暗色塗り：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +2000,-4000, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_B );   // 歯の右側を横断する線
    pLOD->addData( pAP );
    
    // 暗色塗り：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +2000,+4000, -3000,-3000, 0,0 );
    pLOD->addData( pAP );
    
    // 暗色塗り：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+4000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 暗色塗り：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs, +3 );  // 暗く
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 暗色塗り：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( +2500,0 );
    pPOD->addData( pFP );

    #ifdef FILL_OPTION_TEST_B_WITH_ANTI
    // 暗色塗り：アンチ（※座標に意味はない）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_ANTI_STROKE_AFTER_FILL, +3 );
    pFP->setOptionTouch( eSTROKE_TOUCH_TARGET_TEMP_B );
    pFP->setOptionStroke( 2, 0 );
    pPOD->addData( pFP );
    #endif

    //--------------------------------
    // 明暗：ラインの確保（※目玉）
    //--------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
 
    pLOD->setTempAdjust( -1500,1300, 2000,2000 );
    
    // 明暗：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 明暗：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 明暗：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 明暗：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 明暗：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 明暗：塗り０（目玉：白目）
    pFP = CFillPointData::Alloc();
    pFP->set( -1500,1300 );
    //pFP->setOption( eFILL_OPTION_AREA, -1 );    // 明るく
    pFP->setOption( eFILL_OPTION_AREA, +1 );    // 暗く
    pPOD->addData( pFP );
    
    // ポイント（目玉：瞳孔）：塗り１
    pFP = CFillPointData::Alloc();
    pFP->set( -1500,1300 );
    pFP->setOption( eFILL_OPTION_POINT_AFTER_FILL, +3 );   // 暗く
    pFP->setOptionStroke( 15, 0 );      // 最大15：[基本ストローク最大＝５]×[倍率最大＝３００％]
    pPOD->addData( pFP );

    // ポイント（目玉：ハイライト）：塗り２
    pFP = CFillPointData::Alloc();
    pFP->set( -1500+200,1300-200 );
    pFP->setOption( eFILL_OPTION_POINT_AFTER_FILL, -2 );   // 明るく
    pFP->setOptionStroke( 10, 0 );
    pPOD->addData( pFP );
    
    //------------------------------------
    // 広域（明るく）：ラインの確保（※左上側）
    //（※範囲目一杯に描いているので拡大厳禁）
    //------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 広域（明るく）：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（明るく）：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（明るく）：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+2500, 0,0, -3000,-4000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（明るく）：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+2500, +3000,-4000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //--------------------------------------
    // 広域（明るく）：ペイントの確保
    //--------------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 広域（明るく）：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,-2000 );
    pFP->setOption( eFILL_OPTION_AREA, -1 );    // 明るく
    pPOD->addData( pFP );

    //-----------------------------------
    // 広域（暗く）：ラインの確保（※右下側）
    //（※範囲目一杯に描いているので拡大厳禁）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pLOD );
    
    // 広域（暗く）：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（暗く）：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,+4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（暗く）：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +4000,-1500, 0,0, -3000,+3000 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 広域（暗く）：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000,-1500, +3000,+3000, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    //------------------------------------
    // 広域（暗く）：ペイントの確保
    //------------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 広域（暗く）：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,+2500 );
    pFP->setOption( eFILL_OPTION_AREA, +1 );    // 暗く
    pPOD->addData( pFP );

    //----------------------------------
    // 全体補正（明暗）：ペイントの確保定
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 全体補正（明）：塗り０
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_COLOR, -1 );   // 明るく
    pFP->setOptionOfsXY( -5, -5 );            // カラーバッファの左上
    pPOD->addData( pFP );

    // 全体補正（暗）：塗り１
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_COLOR, +1 );   // 暗く
    pFP->setOptionOfsXY( +5, +5 );            // カラーバッファの右下
    pPOD->addData( pFP );
#endif
    
#ifdef FILL_OPTION_TEST_C
    //====================================
    // 通常塗りとオプション塗りの確認
    //====================================
    // ブラシ＆パレット指定
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS;   // テストパレット

    //---------------------------------
    // 暗色塗り挙動確認：ラインの確保（領域上）
    //（※テスト指定での塗りの確認）
    //---------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 暗色塗り挙動確認：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -5000+100,-5000+100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 暗色塗り挙動確認：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +0-2000,-5000+100, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_C );   // 歯の中央左を横断する線
    pLOD->addData( pAP );
    
    // 暗色塗り挙動確認：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +0-1000,+5000-100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 暗色塗り挙動確認：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -5000+100,+5000-100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //----------------------------------
    // 暗色塗り挙動確認：ペイントの確保
    //----------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs, +3 );  // 暗く
    pPOD->setTestPalOfsId( bucketPalOfs );
    m_pLayerBase->addData( pPOD );
    
    // 暗色塗り挙動確認：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -2500,0 );
    pPOD->addData( pFP );
    
    #ifdef FILL_OPTION_TEST_C_WITH_ANTI
    // 暗色塗り挙動確認：アンチ（※座標に意味はない）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_ANTI_STROKE_AFTER_FILL, +3 );
    pFP->setOptionTouch( eSTROKE_TOUCH_TARGET_TEMP_C );
    pFP->setOptionStroke( 1, 0 );
    pPOD->addData( pFP );
    #endif

    //------------------------------------
    // 広域塗り挙動確認：ラインの確保（領域下）
    //（※エリア指定での塗りの確認）
    //------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    m_pLayerBase->addData( pLOD );
    
    // 広域塗り挙動確認：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( +5000-100,-5000+100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 広域塗り挙動確認：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( +0+2000,-5000+100, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setTouchTargetId( eSTROKE_TOUCH_TARGET_TEMP_D );   // 歯の中央右を横断する線
    pLOD->addData( pAP );
    
    // 広域塗り挙動確認：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( +0+1000,+5000-100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 広域塗り挙動確認：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( +5000-100,+5000-100, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //--------------------------------------
    // 広域塗り挙動確認：ペイントの確保
    //--------------------------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    m_pLayerBase->addData( pPOD );
    
    // 広域塗り挙動確認：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( +2500,0 );
    pFP->setOption( eFILL_OPTION_AREA, -2 );    // 明るく
    pPOD->addData( pFP );

    #ifdef FILL_OPTION_TEST_C_WITH_ANTI
    // 広域塗り挙動確認：アンチ（※座標に意味はない）
    pFP = CFillPointData::Alloc();
    pFP->setOption( eFILL_OPTION_ANTI_STROKE_AFTER_FILL, -2 );
    pFP->setOptionTouch( eSTROKE_TOUCH_TARGET_TEMP_D );
    pFP->setOptionStroke( 4, 4 );
    pPOD->addData( pFP );
    #endif
#endif
    
#ifdef GUIDE_TEST
    //==================================
    // ガイド登録：左下領域の矩形
    //==================================
    // ブラシ＆パレット
    brush = CHECK_BRUSH;
    bucketPalOfs = CHECK_PAL_OFS;

    //--------------------------
    // 補正矩形：ラインの確保
    //--------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    #ifndef GUIDE_TEST_AT_PAINT
    pLOD->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    #endif
    m_pLayerBase->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,+4000, 0,0, 0,0 );
    #ifdef GUIDE_TEST_AT_PAINT
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    #endif
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,+4000, 0,0, 0,0 );
    #ifdef GUIDE_TEST_AT_PAINT
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    #endif
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,+2000, 0,0, 0,0 );
    #ifdef GUIDE_TEST_AT_PAINT
    pAP->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    #endif
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,+2000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    //----------------------
    // マスク：ペイントの確保
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    #ifdef GUIDE_TEST_AT_PAINT
    pPOD->setGuideTargetId( eSTROKE_GUIDE_TARGET_TEMP_A );
    #endif
    m_pLayerBase->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -3000,+3000 );
    pPOD->addData( pFP );
#endif

}
