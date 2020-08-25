/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoopForLineRepair.cpp [共通環境]
  |	Comment:	ベジェテストループ：線修復
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
// 線修復[LineRepair]設定有効（オン／オフの効果確認用）
#define ENABLE_LINE_REPAIR_SETTING

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
// [サブ] ベジェ確認：バッファ確保：線修復
// 処理対象：[m_pLayerLineRepair]
//---------------------------------
void CBezierTestLoop::allocForLayerForLineRepair( void ){
    // レイヤー確保
    m_pLayerLineRepair = CLayerData::Alloc();
    
    // 線ワーク
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    eBRUSH brush = CHECK_BRUSH;

    // 塗りワーク
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;

    //----------------------------
    // [十字]: オブジェクト確保＆設定
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pLOD );
    
    // 土台：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1500,-500, -500,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,-500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,-1500, 0,0, 0,-500 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 土台：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,-1500, 0,-500, 0,0 );
    pLOD->addData( pAP );

    // 土台：点４
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,-500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点５
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1500,-500, 0,0, 500,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 土台：点６
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1500,500, 500,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点７
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,500, 0,0, 0,0 );
    pLOD->addData( pAP );

    // 土台：点８
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,1500, 0,0, 0,500 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );
    
    // 土台：点９
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,1500, 0,500, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点１０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点１１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1500,500, 0,0, -500,0 );
    pAP->setFlagOn( eAPD_FLAG_TRANSPARENCY );
    pLOD->addData( pAP );

    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pPOD );
    
    // 土台：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );

    //-------------------------
    // [上]:オブジェクト確保＆設定
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( bucketPalOfs );
#endif
    pLOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pLOD );
    
    // 土台：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,-1500+200, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,-2000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,-2000, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,-1500+200, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pPOD );
    
    // 土台：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,-1750 );
    pPOD->addData( pFP );

    //-------------------------
    // [右]:オブジェクト確保＆設定
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( bucketPalOfs );
#endif
    pLOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pLOD );
    
    // 土台：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1500-200,-500, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,-500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2000,500, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 1500-200,500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pPOD );
    
    // 土台：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 1750,0 );
    pPOD->addData( pFP );

    //-------------------------
    // [下]:オブジェクト確保＆設定
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( bucketPalOfs );
#endif
    pLOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pLOD );
    
    // 土台：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,1500-200, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 500,2000, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,2000, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -500,1500-200, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pPOD );
    
    // 土台：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,1750 );
    pPOD->addData( pFP );
    
    //-------------------------
    // [左]:オブジェクト確保＆設定
    //-------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pLOD->setTestPalOfsIdForRepair( bucketPalOfs );
#endif
    pLOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pLOD );
    
    // 土台：点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1500+200,500, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    // 土台：点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2000,-500, 0,0, 0,0 );
#ifdef ENABLE_LINE_REPAIR_SETTING
    pAP->setFlagOn( eAPD_FLAG_LINE_REPAIR_TEST );
#endif
    pLOD->addData( pAP );
    
    // 土台：点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -1500+200,-500, 0,0, 0,0 );
    pLOD->addData( pAP );
    
    //-----------------------
    // 塗りオブジェクト確保＆設定
    //-----------------------
    // オブジェクト確保＆設定
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
    pPOD->setTempAdjust( 0,0, 20000,20000 );
    m_pLayerLineRepair->addData( pPOD );
    
    // 土台：塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( -1750,0 );
    pPOD->addData( pFP );
}
