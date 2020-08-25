/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoopForDelay.cpp [共通環境]
  |	Comment:	ベジェテストループ：遅延確認
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
//------------------------------------------
// ブラシ＆塗り指定
//------------------------------------------
#define CHECK_BRUSH     eBRUSH_C_BASE
#define CHECK_PAL_OFS   ePAL_OFS_C_SKIN

//------------------------------------------
// 動作指定
//------------------------------------------
#define CLOTH_TEST
#define HAIR_TEST
#define SKIN_TEST
#define BUST_TEST

#define SEPARATE_TEST
#define ADJUST_IN_OUT_TEST

//#define HOOK_TEST

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
// [サブ] ベジェ確認：バッファ確保：遅延
// 処理対象：[m_pLayerDelay]
//---------------------------------
void CBezierTestLoop::allocForLayerForDelay( void ){
    // レイヤー確保
    m_pLayerDelay = CLayerData::Alloc();

    //----------------------
    // 線ワーク
    //----------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    CAdjustablePoint* pAdj;

    eBRUSH brush = CHECK_BRUSH;

    //----------------------
    // 塗りワーク
    //----------------------
    CPaintObjectData* pPOD;
    CFillPointData* pFP;
    eBUCKET bucket = eBUCKET_TEST_MONO;
    ePAL_OFS bucketPalOfs = CHECK_PAL_OFS;

#ifdef CLOTH_TEST
    //----------------------
    // 線オブジェクト確保＆設定
    //----------------------
    // CLOTH
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef SEPARATE_TEST
    pLOD->setTempAdjust( -2500,-2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pLOD->addData( pAP );
    
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 1 );
    pAdj->setDelayPowerRate( 2500/2, 2500/3 );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 2 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_B );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_CLOTH, 4 );
    pAdj->setDelayPowerRate( 2500, 2500 );

    //----------------------
    // 塗りオブジェクト確保＆設定
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
#ifdef SEPARATE_TEST
    pPOD->setTempAdjust( -2500,-2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
#endif

#ifdef HAIR_TEST
    //----------------------
    // 線オブジェクト確保＆設定
    //----------------------
    // HAIR
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef SEPARATE_TEST
    pLOD->setTempAdjust( 2500,-2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_A );
#endif
    pLOD->addData( pAP );
    
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_HAIR, 8 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_HAIR, 2 );
    pAdj->setDelayPowerRate( 2500/2, 2500/4 );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_HAIR, 8 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_C );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_HAIR, 16 );
    pAdj->setDelayPowerRate( 2500, 2500 );

    //----------------------
    // 塗りオブジェクト確保＆設定
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
#ifdef SEPARATE_TEST
    pPOD->setTempAdjust( 2500,-2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
#endif
    
#ifdef SKIN_TEST
    //----------------------
    // 線オブジェクト確保＆設定
    //----------------------
    // SKIN
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef SEPARATE_TEST
    pLOD->setTempAdjust( -2500,2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
    pLOD->addData( pAP );
    
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_SKIN, 2 );
    pAdj->setDelayPowerRate( 2500/2, 2500/3 );

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_B );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_SKIN, 1 );
    pAdj->setDelayPowerRate( 2500/4, 2500/4 );

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_D );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_SKIN, 2 );
    pAdj->setDelayPowerRate( 2500/2, 2500/3 );

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_SKIN, 4 );
    pAdj->setDelayPowerRate( 2500/2, 2500/2 );

    //----------------------
    // 塗りオブジェクト確保＆設定
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
#ifdef SEPARATE_TEST
    pPOD->setTempAdjust( -2500,2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
#endif

#ifdef BUST_TEST
    //----------------------
    // 線オブジェクト確保＆設定
    //----------------------
    // BUST
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
#ifdef SEPARATE_TEST
    pLOD->setTempAdjust( 2500,2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -2500,0, 0,1388, 0,-1388 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_D );
#endif
    pLOD->addData( pAP );
    
    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
#ifdef ADJUST_IN_OUT_TEST
    pAdj = pAP->getAdjInXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 0, 1388 );

    pAdj = pAP->getAdjOutXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 0, -1388 );
#endif

    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,-2500, -1388,0, 1388,0 );
#ifdef HOOK_TEST
    pAP->setHookTargetId( eSTROKE_HOOK_TARGET_TEMP_C );
#endif
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 1 );
    pAdj->setDelayPowerRate( 2500/8, 2500/8 );
#ifdef ADJUST_IN_OUT_TEST
    pAdj = pAP->getAdjInXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 1 );
    pAdj->setDelayPowerRate( -1388/4, 0 );

    pAdj = pAP->getAdjOutXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 1 );
    pAdj->setDelayPowerRate( 1388/4, 0 );
#endif

    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 2500,0, 0,-1388, 0,1388 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 2500, 2500/2 );
#ifdef ADJUST_IN_OUT_TEST
    pAdj = pAP->getAdjInXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 0, -1388 );

    pAdj = pAP->getAdjOutXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 4 );
    pAdj->setDelayPowerRate( 0, 1388 );
#endif

    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0,2500, 1388,0, -1388,0 );
    pLOD->addData( pAP );

    pAdj = pAP->getAdjXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 8 );
    pAdj->setDelayPowerRate( 2500, 2500 );
#ifdef ADJUST_IN_OUT_TEST
    pAdj = pAP->getAdjInXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 8 );
    pAdj->setDelayPowerRate( 1388*2, 0 );

    pAdj = pAP->getAdjOutXY();
    pAdj->setDelayType( eDELAY_LOG_BUST, 8 );
    pAdj->setDelayPowerRate( -1388*2, 0 );
#endif

    //----------------------
    // 塗りオブジェクト確保＆設定
    //----------------------
    pPOD = CPaintObjectData::Alloc();
    pPOD->setBucketId( bucket );
    pPOD->setPalOfsId( bucketPalOfs );
#ifdef SEPARATE_TEST
    pPOD->setTempAdjust( 2500,2500, 6666,6666 );
#endif
    m_pLayerDelay->addData( pPOD );
    
    // 塗り０
    pFP = CFillPointData::Alloc();
    pFP->set( 0,0 );
    pPOD->addData( pFP );
#endif
}
