/*+----------------------------------------------------------------+
  |	Title:		BmpGenerator.cpp [共通環境]
  |	Comment:	Bmp生成
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenerator.hpp"
#include "draw/tex/adjust/AdjustCalc.hpp"
#include "draw/tex/bezier/BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define BMP_GEN_DEFAULT_MARGIN           96
#define BMP_GEN_DEFAULT_WIDTH           256
#define BMP_GEN_DEFAULT_HEIGHT          256
#define BMP_GEN_DEFAULT_STROKE_SCALE    1.0f

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
int CBmpGenerator::s_nTexResolution;
int CBmpGenerator::s_nBaseTexMargin;
int CBmpGenerator::s_nBaseTexSizeW;
int CBmpGenerator::s_nBaseTexSizeH;
float CBmpGenerator::s_fBaseStrokeScale;

BYTE* CBmpGenerator::s_pBufLine;
BYTE* CBmpGenerator::s_pBufColor;
BYTE* CBmpGenerator::s_pBufFillGuide;
BYTE* CBmpGenerator::s_pBufFillGuard;

int CBmpGenerator::s_nTexMargin;
int CBmpGenerator::s_nTexW0;
int CBmpGenerator::s_nTexH0;
int CBmpGenerator::s_nTexW;
int CBmpGenerator::s_nTexH;
int CBmpGenerator::s_nSizeBuf;

stBEZIER_BASE_PARAM CBmpGenerator::s_stBaseParam;

CJointPoint CBmpGenerator::s_oJointPoint;
CAnglePlane CBmpGenerator::s_oAnglePlane;

float CBmpGenerator::s_fTexBaseX;
float CBmpGenerator::s_fTexBaseY;
float CBmpGenerator::s_fTexFocusX;
float CBmpGenerator::s_fTexFocusY;

float CBmpGenerator::s_fTexGroundX0_L;
float CBmpGenerator::s_fTexGroundY0_L;
float CBmpGenerator::s_fTexGroundX0_R;
float CBmpGenerator::s_fTexGroundY0_R;
float CBmpGenerator::s_fTexFocusX0;
float CBmpGenerator::s_fTexFocusY0;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// 初期化
//------------------------
bool CBmpGenerator::OnCreate( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpGenerator::OnCreate()\n" );
#endif

    Reset();
    
    return( true );
}

//------------------------
// 終了
//------------------------
void CBmpGenerator::OnDestroy( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ BmpGenerator::OnDestroy()\n" );
#endif
}

//------------------------
// リセット
//------------------------
void CBmpGenerator::Reset( void ){
    s_nTexResolution = GM_P_RATE;
    s_nBaseTexMargin = BMP_GEN_DEFAULT_MARGIN;
    s_nBaseTexSizeW = BMP_GEN_DEFAULT_WIDTH;
    s_nBaseTexSizeH = BMP_GEN_DEFAULT_HEIGHT;
    s_fBaseStrokeScale = BMP_GEN_DEFAULT_STROKE_SCALE;
}

//------------------------
// ガイドのリセット
//------------------------
// ガイドのリセット
void CBmpGenerator::ResetGuide( bool isFullReset ){
    memset( s_pBufFillGuide, 0, s_nSizeBuf );
    if( isFullReset ){
        memset( s_pBufFillGuard, 0, s_nSizeBuf );
    }
}

//------------------------
// BMP生成：生成パラメータ指定
//------------------------
int CBmpGenerator::CreateTexWithGenParam( CTex* pTexLine, CTex* pTexColor,
                                          CBmpGenParam* pGenParam, eBD_SLOT slot, int subId, int slotIndex,
                                          stBMP_GEN_CREATE_PARAM* pCreateParam,
                                          bool isWorkPath, bool isWorkDisp ){
    // 無効は無視
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: INVALID PARAM: pGenParam=%p\n", pGenParam );
        return( -1 );
    }

    // 処理準備
    if( ! Ready( pCreateParam ) ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: [Ready()] FAILED\n" );
        return( -1 );
    }

    // スーツオプションの上書き
    s_stBaseParam.arrOption[eBD_OPTION_SUITED] = pGenParam->isSuited();
    
    // 対象の生成パラメータの取得
    stBMP_GEN_PARAM_CELL *pCell = pGenParam->searchCell( slot, slotIndex );
    if( pCell == NULL || pCell->pData == NULL ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: DATA NOT FOUND: slot=%d, subId=%d, slotIndex=%d\n", slot, subId, slotIndex );
        return( -1 );
    }

    // 表示対象が含まれていなければ無視しておく
    //（※エディタ画面で対象データ以外をいじってしまうことがないように）
    if( pCell->pData->getSubId() != subId ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: SUB ID UNMATCH: subId=%d\n", subId );
        return( -1 );
    }

    // 計測開始
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-[START] CBmpGenerator::CreateTexWithGenParam\n" );
    }
#endif
    time_log_start( eTIME_LOG_ID_TEX_GEN );

    // 編集調整時は[TestVal]の確認（※スロットの単体描画の際は色バッファにテストパレット値を設定しておく）
    if( pCell->pData->checkFlag( eBDPD_FLAG_TEMP_EDIT_TARGET ) ){
        BYTE testPalGrp = 0x00;

        // 先頭要素のテスト値を抽出（※とりあえず）
        CLayerData* pLayer = pCell->pData->getData( 0 );
        if( pLayer != NULL ){
            CLayerObject* pObj = pLayer->getData( 0 );
            if( pObj != NULL ){
                if( pObj->isLineObject() ){
                    testPalGrp = (pObj->getLineObject())->getTestPalGrp();
                }else if( pObj->isPaintObject() ){
                    testPalGrp = (pObj->getPaintObject())->getTestPalGrp();
                }
            }
        }
        
        if( testPalGrp != 0x00 ){
            memset( s_pBufColor, BEZIER_CONV_PAL_INFO_TO_DOT(testPalGrp, BEZIER_PAL_VAL_BASE), s_nSizeBuf );
        }
    }
    
    // ワーククリア
    pGenParam->resetForWork();
    
    //------------------------------------------
    // オーダーリスト確保（※ワークなので[APP]から確保）
    //------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    CLayerOrderList* pOrderList = new CLayerOrderList();
    CMemMgr::PopTargetField();

#ifdef BMP_GEN_LOG
    LOGD( "@ REGIST ORDER\n" );
#endif

    // オーダーの事前登録（※フォーム毎にパーツの描画順が指定される）
    RegistOrder( pOrderList, pGenParam->getForm(), pGenParam->isSuited() );

    // オーダー登録パラメータ
    stREGIST_ORDER_PARAM rop;
    CLEAR_REGIST_ORDER_PARAM( &rop );
    
    rop.isFlipH = s_stBaseParam.isFlipH;
    rop.isFlipV = s_stBaseParam.isFlipV;
    rop.rot = s_stBaseParam.rot;
    rop.scale = s_stBaseParam.scale;
    rop.baseX = s_nTexW/2.0f;
    rop.baseY = s_nTexH/2.0f;
    
    // 指定があれば作業フラグを立てておく
    if( isWorkDisp ){
        rop.isWorkScale = true;
        rop.isSlotIgnore = true;
    }
    
    // オーダー登録
#ifdef BMP_GEN_LOG
    LOGD( "@ START: SET ORDER LIST\n" );
#endif
    SetOrderList( pOrderList, pGenParam, slot, slotIndex, false, &s_stBaseParam, rop, isWorkPath );

#ifdef BMP_GEN_LOG
    LOGD( "@ FINISH: SET ORDER LIST\n" );
#endif

    //-----------------------------------------------------------------------------------------------
    // オーダーリストに登録されたレイヤーの描画
    // [i==0]で[LyFront]、[i==priority]で[LyBack]が処理される
    //-----------------------------------------------------------------------------------------------
#ifdef BMP_GEN_LOG
    LOGD( "@ DRAW LAYER BY PRIORITY\n" );
#endif
    int priority = pOrderList->getOrderCellPriority();
    for( int i=0; i<=priority; i++ ){
        // 優先順位順にセルの取り出し（※無効は無視）
        CLayerOrderCell* pOrderCell = pOrderList->getOrderCellByPriority( i );
        if( pOrderCell == NULL ){
            continue;
        }

        // 登録内容を[FRONT/NORMAL/BACK]の順に処理
        for( int i=0; i<eLAYER_ORDER_TYPE_MAX; i++ ){
            stLAYER_ORDER_CELL_NODE* pNode = pOrderCell->getNodeHead( (eLAYER_ORDER_TYPE)i );
            while( pNode != NULL ){
                CLayerData* pLayerData = pNode->pLayerData;
                
#ifdef BMP_GEN_LOG
                const char* pLabelForData = "UNKNOWN";
                eBD_SLOT slotForData = pLayerData->getOrderSlot();
                if( slotForData >= 0 && slotForData < eBD_SLOT_MAX ){
                    pLabelForData = g_pArrLabelBdSlot[slotForData];
                }else if( slotForData == eBD_SLOT_INVALID ){
                    pLabelForData = "INVALID";
                }

                const char* pLabelForOrder = "UNKNOWN";
                eBD_SLOT slotForOrder = pOrderCell->getOrderSlot();
                if( slotForOrder >= 0 && slotForOrder < eBD_SLOT_MAX ){
                    pLabelForOrder = g_pArrLabelBdSlot[slotForOrder];
                }else if( slotForOrder == eBD_SLOT_INVALID ){
                    pLabelForOrder = "INVALID";
                }

                LOGD( "@ [DRAW:%s]: order=%s, slot=%s(%d)/%s(%d), index=%d\n",
                      pLayerData->getName(), g_pArrLabelLayerOrderType[i],
                      pLabelForData, slotForData, pLabelForOrder, slotForOrder, pOrderCell->getOrderSlotIndex());
#endif
                // レイヤーの描画
                pLayerData->draw( &s_stBaseParam, &pNode->layerParam, isWorkPath );
                
                pNode = (stLAYER_ORDER_CELL_NODE*) pNode->pNext;
            }
        }
    }
    
    //-----------------------------------------
    // 検出座標の設定（※中間座標に傾きの適用）
    //-----------------------------------------
    CAnglePlane* pAngle = s_stBaseParam.pAnglePlane;
    if( pAngle != NULL ){
        // 基本座標（接地座標）
        eBD_SLOT slot = CBDConst::GetParentSlotForAnglePlane( eBD_SLOT_FoGround );
        int searchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( slot, 0 );
        pAngle->calcXY( &s_fTexBaseX, &s_fTexBaseY,
                        (s_fTexGroundX0_L+s_fTexGroundX0_R)/2.0f,
                        (s_fTexGroundY0_L+s_fTexGroundY0_R)/2.0f,
                        searchKey, s_stBaseParam.adjustRateLR, s_stBaseParam.adjustRateUD );

        // フォーカス座標
        slot = CBDConst::GetParentSlotForAnglePlane( eBD_SLOT_FcNose );
        searchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( slot, 0 );
        pAngle->calcXY( &s_fTexFocusX, &s_fTexFocusY,
                        s_fTexFocusX0, s_fTexFocusY0, searchKey, s_stBaseParam.adjustRateLR, s_stBaseParam.adjustRateUD );
        
    }else{
        // 基本座標（接地座標）
        s_fTexBaseX = (s_fTexGroundX0_L+s_fTexGroundX0_R)/2;
        s_fTexBaseY = (s_fTexGroundY0_L+s_fTexGroundY0_R)/2;
        
        // フォーカス座標
        s_fTexFocusX = s_fTexFocusX0;
        s_fTexFocusY = s_fTexFocusY0;
    }

    //-----------------------------------------------------------
    // 解放
    //-----------------------------------------------------------
    SAFE_DELETE( pOrderList );

    // 終了（※テクスチャ生成）
    if( isWorkPath ){
        FinishForWorkPath( pTexLine );
    }else{
        Finish( pTexLine, pTexColor );
    }

    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_TEX_GEN );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-[FINISH] CBmpGenerator::CreateTexWithGenParam: %dus\n", time );
    }
#endif
    
    return( time );
}

//------------------------
// BMP生成：レイヤーデータ指定
//------------------------
int CBmpGenerator::CreateTexWithLayerList( CTex* pTexLine, CTex* pTexColor,
                                           CList* pLayerDataList,
                                           stBMP_GEN_CREATE_PARAM* pCreateParam,
                                           bool isWorkPath ){
    // 無効は無視
    if( pLayerDataList == NULL ){
        LOGE( "@ CBmpGenerator::CreateTexWithLayerList: INVALID DATA: pLayerDataList=%p\n", pLayerDataList );
        return( false );
    }
    
    // 処理準備
    if( ! Ready( pCreateParam ) ){
        LOGE( "@ CBmpGenerator::CreateTexWithLayerList: [Ready()] FAILED\n" );
        return( false );
    }
    
    // 計測開始
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-[START] CBmpGenerator::CreateTexWithLayerList\n" );
    }
#endif
    time_log_start( eTIME_LOG_ID_TEX_GEN );

    // レイヤーパラメータ準備
    stBEZIER_LAYER_PARAM stLayerParam;
    CLEAR_BEZIER_LAYER_PARAM( &stLayerParam );
    
    stLayerParam.pixelOfsX = s_nTexW/2.0f;
    stLayerParam.pixelOfsY = s_nTexH/2.0f;
    stLayerParam.pixelScaleX = ((s_stBaseParam.isFlipH)? -s_stBaseParam.scale: s_stBaseParam.scale);
    stLayerParam.pixelScaleY = ((s_stBaseParam.isFlipV)? -s_stBaseParam.scale: s_stBaseParam.scale);
    stLayerParam.pixelRot = s_stBaseParam.rot;

    stLayerParam.fixedAdjustRateH = s_stBaseParam.adjustRateH;
    FIX_ADJUST_RATE( stLayerParam.fixedAdjustRateH );

    stLayerParam.fixedAdjustRateV = s_stBaseParam.adjustRateV;
    FIX_ADJUST_RATE( stLayerParam.fixedAdjustRateV );

    stLayerParam.fixedAngleRateLR = s_stBaseParam.adjustRateLR;
    FIX_ADJUST_RATE( stLayerParam.fixedAngleRateLR );

    stLayerParam.fixedAngleRateUD = s_stBaseParam.adjustRateUD;
    FIX_ADJUST_RATE( stLayerParam.fixedAngleRateUD );
    
#if 1
    // 傾き設定
    stLayerParam.slot = eBD_SLOT_FcBase;
    stLayerParam.slotIndex = 0;
    stLayerParam.parentSlot = eBD_SLOT_FcBase;
    stLayerParam.parentSlotIndex = 0;
    SetAnglePlaneInfo( eBD_SLOT_FcBase, 0, s_nTexW/2, s_nTexH/2, false, false, 0, 1.0f );
#endif
    
    // 要素の描画
    CLayerData* pLayerData = (CLayerData*) pLayerDataList->getHead();
    while( pLayerData != NULL ){
        float rot0 = stLayerParam.pixelRot;
        
        // 遅延によるレイヤーの回転
        int rateRot;
        CAdjustCalc::CalcRateRotForDelayLog( &rateRot, false, false, pLayerData, s_stBaseParam.pDelayLog );
        stLayerParam.pixelRot += CConst::ConvBezierRotationRate( rateRot );

        pLayerData->draw( &s_stBaseParam, &stLayerParam, isWorkPath );
        pLayerData = (CLayerData*) pLayerData->getNext();
        stLayerParam.pixelRot = rot0;
    }
    
    // 終了（※テクスチャ生成）
    if( isWorkPath ){
        FinishForWorkPath( pTexLine );
    }else{
        Finish( pTexLine, pTexColor );
    }

    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_TEX_GEN );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-[FINISH] CBmpGenerator::CreateTexWithLayerList: %dus\n", time );
    }
#endif
    
    return( time );
}

//-------------------------------------------------------
// 処理準備（※[pCreateParam]から[s_stBaseParam]への受け渡し）
//-------------------------------------------------------
bool CBmpGenerator::Ready( stBMP_GEN_CREATE_PARAM* pCreateParam ){
    // 無効は無視
    if( pCreateParam == NULL ){
        LOGE( "@ CBmpGenerator::Ready: INVALID PARAM: pCreateParam=%p\n", pCreateParam );
        return( false );
    }
    
    // ワークバッファ確保
    if( ! AllocWorkBuf() ){
        return( false );
    }

    // ワークバッファリセット
    ResetWorkBuf();

    // 共通パラメータ準備
    s_stBaseParam.pBufLine = s_pBufLine;
    s_stBaseParam.pBufColor = s_pBufColor;
    s_stBaseParam.pBufFillGuide = s_pBufFillGuide;
    s_stBaseParam.pBufFillGuard = s_pBufFillGuard;

    s_stBaseParam.pixelRate = s_nTexResolution;
    s_stBaseParam.texW0 = s_nTexW0;
    s_stBaseParam.texH0 = s_nTexH0;
    s_stBaseParam.texW = s_nTexW;
    s_stBaseParam.texH = s_nTexH;
    s_stBaseParam.strokeScale = s_fBaseStrokeScale;
    
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        s_stBaseParam.arrOption[i] = pCreateParam->arrOption[i];
    }

    s_stBaseParam.isFlipH = pCreateParam->isFlipH;
    s_stBaseParam.isFlipV = pCreateParam->isFlipV;
    s_stBaseParam.rot = pCreateParam->rot;
    s_stBaseParam.scale = pCreateParam->scale;
    s_stBaseParam.adjustRateH = pCreateParam->adjustRateH;
    s_stBaseParam.adjustRateV = pCreateParam->adjustRateV;
    s_stBaseParam.adjustRateLR = pCreateParam->adjustRateLR;
    s_stBaseParam.adjustRateUD = pCreateParam->adjustRateUD;
    s_stBaseParam.pDelayLog = pCreateParam->pDelayLog;
    s_stBaseParam.pJointPoint = &s_oJointPoint;
    s_stBaseParam.pAnglePlane = &s_oAnglePlane;
    
#if 1
    // 傾き面のデフォルト値の設定（※とりあえず）
    s_oAnglePlane.registPlaneDefault( s_nTexW, s_nTexH, s_nTexW/2.0f, s_nTexH/2.0f, 
                                      pCreateParam->isFlipH, pCreateParam->isFlipV, pCreateParam->rot, pCreateParam->scale );
#endif

    // デフォルト出力値
    s_fTexBaseX = s_nTexW/2;
    s_fTexBaseY = s_nTexH/2;
    s_fTexFocusX = s_nTexW/2;
    s_fTexFocusY = s_nTexH/2;
    
    s_fTexGroundX0_L = s_fTexGroundX0_R = s_nTexW/2;
    s_fTexGroundY0_L = s_fTexGroundY0_R = s_nTexW/2;
    s_fTexFocusX0 = s_nTexW/2;
    s_fTexFocusY0 = s_nTexH/2;
    
    // ベジェリ関連セット
    CBezier::ResetBufInfo( s_nTexW, s_nTexH );
    CBezier::ResetHookPoint();
    CBezier::ResetTouchPoint();

    return( true );
}

//----------------------------------------------------------
// ワークバッファ確保（※共有メモリを利用するため実際には確保していない）
//----------------------------------------------------------
bool CBmpGenerator::AllocWorkBuf( void ){
    //------------------------
    // 作業バッファサイズ
    //------------------------
    s_nTexMargin = s_nBaseTexMargin * s_nTexResolution;
    s_nTexW0 = s_nBaseTexSizeW * s_nTexResolution;
    s_nTexH0 = s_nBaseTexSizeH * s_nTexResolution;
    s_nTexW = s_nTexW0 + 2*s_nTexMargin;
    s_nTexH = s_nTexH0 + 2*s_nTexMargin;

    //------------------------------------------------
    // 作業バッファ確保
    //------------------------------------------------
    BYTE* pBuf = (BYTE*)CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_BMP_GEN_OR_SS );
    int sizeBuf = (int)CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_BMP_GEN_OR_SS );
    
    // バッファサイズ算出（※[P8D]なので１画素１バイト）
    s_nSizeBuf = 1 * s_nTexW * s_nTexH;
    
    // サイズ確認（※４枚分確保したい）
    if( 4*s_nSizeBuf > sizeBuf ){
        LOGE( "@ CBmpGenerator::AllocWorkBuf: BUF SHORTAGE: required=4*%d > allocted=%d\n", s_nSizeBuf, sizeBuf );
        return( false );
    }
    
    // バッファ確保
    s_pBufLine = &pBuf[0*s_nSizeBuf];           // 線画像バッファ
    s_pBufFillGuide = &pBuf[1*s_nSizeBuf];      // ガイドバッファ
    s_pBufFillGuard = &pBuf[2*s_nSizeBuf];      // ガードバッファ
    s_pBufColor = &pBuf[3*s_nSizeBuf];          // 塗り画像バッファ

    return( true );
}

//----------------------------------------------------------
// ワークバッファリセット
//----------------------------------------------------------
void CBmpGenerator::ResetWorkBuf( void ){
    //------------------------
    // 各種描画バッファクリア
    //------------------------
    memset( s_pBufLine, 0, s_nSizeBuf );

    // 塗りバッファの初期値は基本パレット値（※０ではないので注意）
    memset( s_pBufColor, BEZIER_PAL_VAL_BASE, s_nSizeBuf );

    memset( s_pBufFillGuide, 0, s_nSizeBuf );

    memset( s_pBufFillGuard, 0, s_nSizeBuf );

    //------------------------
    // ワーククリア
    //------------------------
    CLEAR_BEZIER_BASE_PARAM( &s_stBaseParam );

    s_oJointPoint.clear();
    s_oAnglePlane.clear();
}

//----------------------------------------------
// 処理の終了（※通常描画時）
//----------------------------------------------
int CBmpGenerator::Finish( CTex* pTexLine, CTex* pTexColor ){
    int timeTotal = 0;
    
    // work（※お役御免になった塗りガイドをワークに流用）
    int texW, texH;
    int ofsL, ofsR, ofsT, ofsB;
    BYTE* pBuf = s_pBufFillGuide;

    //-------------------------------------------
    // [GRAY] テクスチャ作成：線
    //-------------------------------------------
    if( pTexLine != NULL ){
        // 計測開始
        time_log_start( eTIME_LOG_ID_TEX_TRANS );

        // 用心に解放
        pTexLine->release();

        // 余白をカットした画素をバッファに抽出する（※ベジェ側の利用情報によるカット）
        CopyTexWithBezierBufInfo( pBuf, s_pBufLine, &ofsL, &ofsR, &ofsT, &ofsB, false );

        // テクスチャ作成（転送）
        texW = s_nTexW - (ofsL+ofsR);
        texH = s_nTexH - (ofsT+ofsB);
        pTexLine->create( pBuf, eTEX_DATA_TYPE_GRAY, texW, texH );
        pTexLine->setOfsLRTB( ofsL, ofsR, ofsT, ofsB );
        pTexLine->setBaseXY( s_fTexBaseX, s_fTexBaseY );
        pTexLine->setFocusXY( s_fTexFocusX, s_fTexFocusY );

        // 計測終了
        int time = (int)time_log_stop( eTIME_LOG_ID_TEX_TRANS );
#ifdef BEZIER_TIME_LOG
        LOGD( "@BTL-[DONE] CBmpGenerator::Finish: pTexLine->create %dus\n", time );
#endif
        timeTotal += time;
    }
    
    //-------------------------------------------
    // [P8D] テクスチャ作成：色（※パス指定の際は無視）
    //-------------------------------------------
    if( pTexColor != NULL ){
        // 計測開始
        time_log_start( eTIME_LOG_ID_TEX_TRANS );

        // 用心に解放
        pTexColor->release();

        // 余白をカットした画素をバッファに抽出する（※ベジェ側の利用情報によるカット）
        CopyTexWithBezierBufInfo( pBuf, s_pBufColor, &ofsL, &ofsR, &ofsT, &ofsB, true );

        // テクスチャ作成（転送）
        texW = s_nTexW - (ofsL+ofsR);
        texH = s_nTexH - (ofsT+ofsB);
        pTexColor->create( pBuf, eTEX_DATA_TYPE_P8D, texW, texH );
        pTexColor->setOfsLRTB( ofsL, ofsR, ofsT, ofsB );
        pTexColor->setBaseXY( s_fTexBaseX, s_fTexBaseY );
        pTexColor->setFocusXY( s_fTexFocusX, s_fTexFocusY );

        // 計測終了
        int time = (int)time_log_stop( eTIME_LOG_ID_TEX_TRANS );
#ifdef BEZIER_TIME_LOG
        LOGD( "@BTL-[DONE] CBmpGenerator::Finish: pTexColor->create %dus\n", time );
#endif
        timeTotal += time;
    }
    
    return( timeTotal );
}

//----------------------------------------------
// 処理の終了（※ワークパス用）
//----------------------------------------------
int CBmpGenerator::FinishForWorkPath( CTex* pTexLine ){
    int timeTotal = 0;

    // work（※お役御免になった塗りガイドをワークに流用）
    int texW, texH;
    int ofsL, ofsR, ofsT, ofsB;
    BYTE* pBuf = s_pBufFillGuide;

    //-------------------------------------------
    // [P8D] テクスチャ作成：ワークパス（線）
    //-------------------------------------------
    if( pTexLine != NULL ){
        // 計測開始
        time_log_start( eTIME_LOG_ID_TEX_TRANS );

        // 用心に解放
        pTexLine->release();

        // 余白をカットした画素をバッファに抽出する（※ワークパスはバッファの利用情報がないので余白を検出して設定）
        CopyTexWithOfsCheck( pBuf, s_pBufLine, &ofsL, &ofsR, &ofsT, &ofsB );
        
        // テクスチャ作成（転送）
        texW = s_nTexW - (ofsL+ofsR);
        texH = s_nTexH - (ofsT+ofsB);
        pTexLine->create( pBuf, eTEX_DATA_TYPE_P8D, texW, texH );
        pTexLine->setOfsLRTB( ofsL, ofsR, ofsT, ofsB );
        
        // 計測終了
        int time = (int)time_log_stop( eTIME_LOG_ID_TEX_TRANS );
        
        timeTotal += time;
    }
    
    return( timeTotal );
}

//--------------------------------------------------------------
// 有効な領域を検出してバッファにコピーする（※[CBezier]のバッファ情報を利用）
//--------------------------------------------------------------
void CBmpGenerator::CopyTexWithBezierBufInfo( BYTE* pBuf, BYTE* pBufSrc, int* pOfsL, int* pOfsR, int* pOfsT, int* pOfsB, bool isColor ){
    int ofsL = 0;
    int ofsR = 0;
    int ofsT = 0;
    int ofsB = 0;
    
    if( isColor ){
        ofsL = CBezier::GetLeftForColorBuf();
        ofsR = s_nTexW - (CBezier::GetRightForColorBuf()+1);
        ofsT = CBezier::GetTopForColorBuf();
        ofsB = s_nTexH - (CBezier::GetBottomForColorBuf()+1);
    }else{
        ofsL = CBezier::GetLeftForStrokeBuf();
        ofsR = s_nTexW - (CBezier::GetRightForStrokeBuf()+1);
        ofsT = CBezier::GetTopForStrokeBuf();
        ofsB = s_nTexH - (CBezier::GetBottomForStrokeBuf()+1);
    }
    
    // オフセットは４の倍数にしておく（※画像コンバータと挙動を同じにしておく）
    ofsL = 4*(ofsL/4);
    ofsR = 4*(ofsR/4);
    ofsT = 4*(ofsT/4);
    ofsB = 4*(ofsB/4);
    
    // 画像のコピー
    int y = 0;
    int w = s_nTexW - (ofsL+ofsR);
    int h = s_nTexH - (ofsT+ofsB);

    // 有効な領域がない場合は最小領域を指定しておく
    if( w <= 0 || h <= 0 ){
        ofsL = 0;
        ofsR = s_nTexW - 4;
        w = 4;
        
        ofsT = 0;
        ofsB = s_nTexH - 4;
        h = 4;
    }

    for( int i=0; i<h; i++ ){
        memcpy( &pBuf[y*w], &pBufSrc[s_nTexW*(y+ofsT)+ofsL], w );
        y++;
    }
    
    // オフセットの出力
    if( pOfsL != NULL ){ *pOfsL = ofsL; }
    if( pOfsR != NULL ){ *pOfsR = ofsR; }
    if( pOfsT != NULL ){ *pOfsT = ofsT; }
    if( pOfsB != NULL ){ *pOfsB = ofsB; }
}

//-------------------------------------
// 有効な領域を検出してバッファにコピーする
//-------------------------------------
void CBmpGenerator::CopyTexWithOfsCheck( BYTE* pBuf, BYTE* pBufSrc, int* pOfsL, int* pOfsR, int* pOfsT, int* pOfsB ){
    int i, max;
    int ofsL = 0;
    int ofsR = 0;
    int ofsT = 0;
    int ofsB = 0;

    //----------------------
    // 有効な領域を検出：上
    //----------------------
    max = s_nTexH - ofsB;
    while( ofsT < max ){
        int tmp = s_nTexW * ofsT;
        for( i=0; i<s_nTexW; i++ ){
            if( pBufSrc[tmp+i] != 0x00 ){
                break;
            }
        }
        
        if( i < s_nTexW ){
            break;
        }
        
        ofsT++;
    }
    
    //----------------------
    // 有効な領域を検出：下
    //----------------------
    max = s_nTexH - ofsT;
    while( ofsB < max ){
        int tmp = s_nTexW * (s_nTexH-(ofsB+1));
        for( i=0; i<s_nTexW; i++ ){
            if( pBufSrc[tmp+i] != 0x00 ){
                break;
            }
        }
        
        if( i < s_nTexW ){
            break;
        }
        
        ofsB++;
    }

    //----------------------
    // 有効な領域を検出：左
    //----------------------
    max = s_nTexW - ofsR;
    while( ofsL < max ){
        int tmp = s_nTexH - ofsB;
        for( i=ofsT; i<tmp; i++ ){
            if( pBufSrc[s_nTexW*i+ofsL] != 0x00 ){
                break;
            }
        }
        
        if( i < tmp ){
            break;
        }
        
        ofsL++;
    }

    //----------------------
    // 有効な領域を検出：右
    //----------------------
    max = s_nTexW - ofsL;
    while( ofsR < max ){
        int tmp = s_nTexH - ofsB;
        for( i=ofsT; i<tmp; i++ ){
            if( pBufSrc[s_nTexW*i+(s_nTexW-(ofsR+1))] != 0x00 ){
                break;
            }
        }
        
        if( i < tmp ){
            break;
        }
        
        ofsR++;
    }

    // オフセットは４の倍数にしておく（※画像コンバータと挙動を同じにしておく）
    ofsL = 4*(ofsL/4);
    ofsR = 4*(ofsR/4);
    ofsT = 4*(ofsT/4);
    ofsB = 4*(ofsB/4);
    
    // 画像のコピー
    int y = 0;
    int w = s_nTexW - (ofsL+ofsR);
    int h = s_nTexH - (ofsT+ofsB);
    
    // 有効な領域がない場合は最小領域を指定しておく
    if( w <= 0 || h <= 0 ){
        ofsL = 0;
        ofsR = s_nTexW - 4;
        w = 4;
        
        ofsT = 0;
        ofsB = s_nTexH - 4;
        h = 4;
    }
    
    for( i=0; i<h; i++ ){
        memcpy( &pBuf[y*w], &pBufSrc[s_nTexW*(y+ofsT)+ofsL], w );
        y++;
    }

    // オフセットの出力
    if( pOfsL != NULL ){ *pOfsL = ofsL; }
    if( pOfsR != NULL ){ *pOfsR = ofsR; }
    if( pOfsT != NULL ){ *pOfsT = ofsT; }
    if( pOfsB != NULL ){ *pOfsB = ofsB; }
}
