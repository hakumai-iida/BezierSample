/*+----------------------------------------------------------------+
  |	Title:		BezierForFrillLayerData.cpp [共通環境]
  |	Comment:	ベジェ曲線：フリルのレイヤーデータ
  |             ここで作成されるレイヤーは[CBezier::DrawFrill]で利用される
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// フリルデータ確保
#define ALLOC_FRILL_THEN_CHECK( _frillId )                          \
    s_pArrLayerDataForFrill[(_frillId)] = CLayerData::Alloc();      \
    if( s_pArrLayerDataForFrill[(_frillId)] == NULL ){              \
        return( false );                                            \
    }

// フリルデータ解放
#define RELEASE_FRILL( _frillId )                                   \
    if( s_pArrLayerDataForFrill[(_frillId)] != NULL ){              \
        CLayerData::Free( s_pArrLayerDataForFrill[(_frillId)] );    \
        s_pArrLayerDataForFrill[(_frillId)] = NULL;                 \
    }

// フリル確保処理ワーク
#define ALLOC_FRILL_WORK( _frillId )                                \
    CLayerData* pLD = s_pArrLayerDataForFrill[(_frillId)];          \
    CLineObjectData* pLOD;                                          \
    CAnchorPointData* pAP;                                          \
    CPaintObjectData* pPOD;                                         \
    CFillPointData* pFP;                                            \
    CAdjustablePoint* pAdj;                                         \
    eBRUSH brush = eBRUSH_C_BASE;                                   \
    eBUCKET bucket = eBUCKET_TEST_MONO;                             \
    ePAL_OFS bucketPalOfs = ePAL_OFS_C_FRILL_A

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
//-----------------------------------
// [FrillLayerData] フリルデータの取得
//-----------------------------------
CLayerData* CBezier::GetLayerDataForFrill( eSTROKE_FRILL frillId ){
    // 用心
    if( frillId < 0 || frillId >= eSTROKE_FRILL_MAX ){
        LOGE( "@ CBezier::GetLayerDataForFrill: INVALID: frillId=%d\n", frillId );
        return( NULL );
    }
    
    return( s_pArrLayerDataForFrill[frillId] );
}

//-----------------------------------
// [FrillLayerData] フリルデータ確保
//-----------------------------------
bool CBezier::AllocFrill( void ){
    if( ! AllocFrillForTestCircle() ){ return( false ); }
    if( ! AllocFrillForTestTriangle() ){ return( false ); }
    if( ! AllocFrillForTestSquare() ){ return( false ); }
    if( ! AllocFrillForFluffySquareMain() ){ return( false ); }
    if( ! AllocFrillForFluffySquareSub() ){ return( false ); }

    return( true );
}

//-----------------------------------
// [FrillLayerData] フリルデータ解放
//-----------------------------------
void CBezier::ReleaseFrill( void ){
    for( int i=0; i<eSTROKE_FRILL_MAX; i++ ){
        RELEASE_FRILL( i );
    }
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_CIRCLE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestCircle( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_CIRCLE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );

    //-----------------------------------
    // [TEST]: 外円
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );

    //-----------------------------------
    // [TEST]: 内円（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 5000, 5000 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );

    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_TRIANGLE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestTriangle( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_TRIANGLE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );

    //-----------------------------------
    // [TEST]: 三角外側
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1500, 0,-1500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3750, -500,0, 500,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1500, 0,1500 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );
    
    //-----------------------------------
    // [TEST]: 三角内側（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 5000, 5000 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1500, 0,-1500 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-3750-250, -500,0, 500,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    //pAdj = pAP->getAdjXY();
    //pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1500, 0,1500 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
    
    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_TEST_SQUARE
//--------------------------------------------------------
bool CBezier::AllocFrillForTestSquare( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_TEST_SQUARE;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );
    
    //-----------------------------------
    // [TEST]: 外四角
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-2500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-2500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,2000, 0,0, 0,0 );
    //pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,2000, 0,0, 0,0 );
    pLOD->addData( pAP );

    //-----------------------------------
    // [TEST]: 内四角（※塗りに影響させない）
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,-1500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    //pAdj = pAP->getAdjXY();
    //pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000,-1500, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    //pAdj = pAP->getAdjXY();
    //pAdj->setDelayType( eDELAY_LOG_FRILL );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000,1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,1000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );
        
    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
    
    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_FLUFFY_SQUARE_MAIN
//--------------------------------------------------------
bool CBezier::AllocFrillForFluffySquareMain( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_FLUFFY_SQUARE_MAIN;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );

    //-----------------------------------
    // [FLUFFY_SQUARE_MAIN]: 四角
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-3000, 0,0, 0,500 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_CONNECT_0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-3000, 0,500, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_CONNECT_1 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1500,1000, 0,0, 0,0 );
    //pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1500,1000, 0,0, 0,0 );
    pLOD->addData( pAP );

    //-----------------------------------
    // [FLUFFY_SQUARE_MAIN]: タッチ左
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -400,1000, 0,0, 200,-400 );
    pAP->setStroke( 10000, 0, 5000, 0 );
    pAP->setFlagOn( eAPD_FLAG_AS_TOUCH_STROKE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -800,-1000, 200,400, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    //-----------------------------------
    // [FLUFFY_SQUARE_MAIN]: タッチ右
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 400,1000, 0,0, -200,-400 );
    pAP->setStroke( 10000, 0, 5000, 0 );
    pAP->setFlagOn( eAPD_FLAG_AS_TOUCH_STROKE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 800,-1000, -200,400, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
#if 0
    // 影（重い）
    pFP->setOption( eFILL_OPTION_RESERVE, +1 ); // 暗く
    pFP->setOfsXYForOption( 2, 4 );
#endif
    pPOD->addData( pFP );

    return( true );
}

//--------------------------------------------------------
// [FrillLayerData]: eSTROKE_FRILL_FLUFFY_SQUARE_SUB
//--------------------------------------------------------
bool CBezier::AllocFrillForFluffySquareSub( void ){
    eSTROKE_FRILL targetId = eSTROKE_FRILL_FLUFFY_SQUARE_SUB;
    
    // 用心に解放＆確保
    RELEASE_FRILL( targetId );
    ALLOC_FRILL_THEN_CHECK( targetId );
    
    // ワーク確保
    ALLOC_FRILL_WORK( targetId );
       
    //-----------------------------------
    // [FLUFFY_SQUARE_MAIN]: 四角
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-4000, 0,0, 0,-500 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_TEMP_A );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-4000, 0,-500, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_TEMP_B );
    pLOD->addData( pAP );

    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1800,1000, 0,0, 0,0 );
    //pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1800,1000, 0,0, 0,0 );
    pLOD->addData( pAP );

    //-----------------------------------
    // [FLUFFY_SQUARE_SUB]: タッチ左
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -300,1000, 0,0, 0,0 );
    pAP->setStroke( 10000, 0, 5000, 0 );
    pAP->setFlagOn( eAPD_FLAG_AS_TOUCH_STROKE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -600,-2000, 200,400, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    //-----------------------------------
    // [FLUFFY_SQUARE_SUB]: タッチ右
    //-----------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );

    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 300,1000, 0,0, 0,0 );
    pAP->setStroke( 10000, 0, 5000, 0 );
    pAP->setFlagOn( eAPD_FLAG_AS_TOUCH_STROKE );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 600,-2000, -200,400, 0,0 );
    pLOD->addData( pAP );
    
    // ランダム遅延予約（※タイプが指定されるとフリル描画時に遅延設定される）
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_FRILL );

    //----------------------------------------
    // [FLUFFY_SQUARE_MAIN]: 左の線（MAINと接続）
    //----------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_TEMP_A );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1000,1000, 0,0, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_CONNECT_0 );
    pLOD->addData( pAP );

    //----------------------------------------
    // [FLUFFY_SQUARE_MAIN]: 右の線（MAINと接続）
    //----------------------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-4000, 0,0, 0,0 );
    pAP->setFlagOn( eAPD_FLAG_NO_FILL_GUIDE );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_TEMP_B );
    pLOD->addData( pAP );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1000,1000, 0,0, 0,0 );
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_FRILL_CONNECT_1 );
    pLOD->addData( pAP );
    
    //---------------------
    // 塗り
    //---------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pLD->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
#if 0
    // ここの影はフリルに影響はしない（※サブは先行する両サイドの後に描画されるので影を落とす後続がいない）
    // 影（重い）
    pFP->setOption( eFILL_OPTION_RESERVE, +1 ); // 暗く
    pFP->setOfsXYForOption( 2, 2 );
#endif
    pPOD->addData( pFP );
    
    return( true );
}
