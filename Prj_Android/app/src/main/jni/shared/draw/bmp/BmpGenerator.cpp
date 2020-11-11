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

#include "draw/bezier/adjust/AdjustCalc.hpp"

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

int CBmpGenerator::s_nTexMargin;
int CBmpGenerator::s_nTexW0;
int CBmpGenerator::s_nTexH0;
int CBmpGenerator::s_nTexW;
int CBmpGenerator::s_nTexH;
int CBmpGenerator::s_nSizeBuf;

BYTE* CBmpGenerator::s_pBufLine;
BYTE* CBmpGenerator::s_pBufColor;
BYTE* CBmpGenerator::s_pBufFillGuide;
BYTE* CBmpGenerator::s_pBufFillGuard;
BYTE* CBmpGenerator::s_pBufTemp;

int CBmpGenerator::s_nLeftForLineBuf;
int CBmpGenerator::s_nRightForLineBuf;
int CBmpGenerator::s_nTopForLineBuf;
int CBmpGenerator::s_nBottomForLineBuf;

int CBmpGenerator::s_nLeftForColorBuf;
int CBmpGenerator::s_nRightForColorBuf;
int CBmpGenerator::s_nTopForColorBuf;
int CBmpGenerator::s_nBottomForColorBuf;

int CBmpGenerator::s_nLeftForFillGuideBuf;
int CBmpGenerator::s_nRightForFillGuideBuf;
int CBmpGenerator::s_nTopForFillGuideBuf;
int CBmpGenerator::s_nBottomForFillGuideBuf;

int CBmpGenerator::s_nLeftForTempBuf;
int CBmpGenerator::s_nRightForTempBuf;
int CBmpGenerator::s_nTopForTempBuf;
int CBmpGenerator::s_nBottomForTempBuf;

stBEZIER_BASE_PARAM CBmpGenerator::s_stBaseParam;

CJointPoint CBmpGenerator::s_oJointPoint;
CAnglePlane CBmpGenerator::s_oAnglePlane;

float CBmpGenerator::s_fArrSlotBaseX[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX];
float CBmpGenerator::s_fArrSlotBaseY[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX];

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
//--------------------------
// 初期化
//--------------------------
bool CBmpGenerator::OnCreate( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpGenerator::OnCreate()\n" );
#endif

    Reset();
    
    return( true );
}

//--------------------------
// 終了
//--------------------------
void CBmpGenerator::OnDestroy( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ BmpGenerator::OnDestroy()\n" );
#endif
}

//--------------------------
// リセット
//--------------------------
void CBmpGenerator::Reset( void ){
    s_nTexResolution = GM_P_RATE;
    s_nBaseTexMargin = BMP_GEN_DEFAULT_MARGIN;
    s_nBaseTexSizeW = BMP_GEN_DEFAULT_WIDTH;
    s_nBaseTexSizeH = BMP_GEN_DEFAULT_HEIGHT;
    s_fBaseStrokeScale = BMP_GEN_DEFAULT_STROKE_SCALE;
}

//--------------------------
// リセット：各種バッファ
//--------------------------
// 線のリセット
void CBmpGenerator::ResetLineBuf( void ){
    // ０初期化
    memset( s_pBufLine, 0, s_nSizeBuf );
    ResetLineBufInfo();
}

// 色のリセット
void CBmpGenerator::ResetColorBuf( void ){
    // 基本明度による初期化
    memset( s_pBufColor, BEZIER_PAL_VAL_BASE, s_nSizeBuf );
    ResetColorBufInfo();
}

// 塗りガイドのリセット
void CBmpGenerator::ResetFillGuideBuf( void ){
    // クリア色による初期化
    memset( s_pBufFillGuide, BEZIER_FILL_GUIDE_CLEAR_COL, s_nSizeBuf );
    ResetFillGuideBufInfo();
}

// 塗りガード（マスク）のリセット
void CBmpGenerator::ResetFillGuardBuf( void ){
    // ０初期化（※マスクはビット管理）
    memset( s_pBufFillGuard, 0, s_nSizeBuf );
}

// テンポラリのリセット
void CBmpGenerator::ResetTempBuf( void ){
    // 線と同じ取り扱い（※０初期化）
    memset( s_pBufTemp, 0, s_nSizeBuf );
    ResetTempBufInfo();
}

//---------------------------------------
// バッファ利用状況リセット
//---------------------------------------
// 線
void CBmpGenerator::ResetLineBufInfo( void ){
    s_nLeftForLineBuf = 99999;
    s_nRightForLineBuf = -1;
    s_nTopForLineBuf = 99999;
    s_nBottomForLineBuf = -1;
}

// 色
void CBmpGenerator::ResetColorBufInfo( void ){
    s_nLeftForColorBuf = 99999;
    s_nRightForColorBuf = -1;
    s_nTopForColorBuf = 99999;
    s_nBottomForColorBuf = -1;
}

// 塗りガイド
void CBmpGenerator::ResetFillGuideBufInfo( void ){
    s_nLeftForFillGuideBuf = 99999;
    s_nRightForFillGuideBuf = -1;
    s_nTopForFillGuideBuf = 99999;
    s_nBottomForFillGuideBuf = -1;
}

// テンポラリ
void CBmpGenerator::ResetTempBufInfo( void ){
    s_nLeftForTempBuf = 99999;
    s_nRightForTempBuf = -1;
    s_nTopForTempBuf = 99999;
    s_nBottomForTempBuf = -1;
}

//---------------------------------------
// バッファ利用状況更新
//---------------------------------------
// 線
void CBmpGenerator::UpdateLineBufInfo( int x, int y ){
    if( s_nLeftForLineBuf > x ){ s_nLeftForLineBuf = x; }
    if( s_nRightForLineBuf < x ){ s_nRightForLineBuf = x; }
    if( s_nTopForLineBuf > y ){ s_nTopForLineBuf = y; }
    if( s_nBottomForLineBuf < y ){ s_nBottomForLineBuf = y; }
}

// 色
void CBmpGenerator::UpdateColorBufInfo( int x, int y ){
    if( s_nLeftForColorBuf > x ){ s_nLeftForColorBuf = x; }
    if( s_nRightForColorBuf < x ){ s_nRightForColorBuf = x; }
    if( s_nTopForColorBuf > y ){ s_nTopForColorBuf = y; }
    if( s_nBottomForColorBuf < y ){ s_nBottomForColorBuf = y; }
}

// 塗り
void CBmpGenerator::UpdateFillGuideBufInfo( int x, int y ){
    if( s_nLeftForFillGuideBuf > x ){ s_nLeftForFillGuideBuf = x; }
    if( s_nRightForFillGuideBuf < x ){ s_nRightForFillGuideBuf = x; }
    if( s_nTopForFillGuideBuf > y ){ s_nTopForFillGuideBuf = y; }
    if( s_nBottomForFillGuideBuf < y ){ s_nBottomForFillGuideBuf = y; }
}

// テンポラリ
void CBmpGenerator::UpdateTempBufInfo( int x, int y ){
    if( s_nLeftForTempBuf > x ){ s_nLeftForTempBuf = x; }
    if( s_nRightForTempBuf < x ){ s_nRightForTempBuf = x; }
    if( s_nTopForTempBuf > y ){ s_nTopForTempBuf = y; }
    if( s_nBottomForTempBuf < y ){ s_nBottomForTempBuf = y; }
}

//--------------------------
// BMP生成：生成パラメータ指定
//--------------------------
int CBmpGenerator::CreateTexWithGenParam( CTex* pTexLine, CTex* pTexColor,
                                          CBmpGenParam* pGenParam, eBD_SLOT slot, int subId, int slotIndex,
                                          stBMP_GEN_CREATE_PARAM* pCreateParam,
                                          bool isWorkPath, float workScale, bool isSlotIgnore ){
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

    // 描画起点となるパラメータセルの取得
    stBMP_GEN_PARAM_CELL *pCell = pGenParam->searchCell( slot, slotIndex );
    if( pCell == NULL || pCell->pData == NULL ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: DATA NOT FOUND: slot=%d, subId=%d, slotIndex=%d\n", slot, subId, slotIndex );
        return( -1 );
    }

    // 表示対象が含まれていなければ無視しておく
    //（※エディタ画面で対象データ以外をいじってしまうことがないように）
    if( pCell->pData->getSubId() != subId ){
        LOGE( "@ CBmpGenerator::CreateTexWithGenParam: SUB ID UNMATCH: subId=%d(!=%d)\n", pCell->pData->getSubId(), subId );
        return( -1 );
    }

    //--------------------------------------------------------------------------------------------------
    // 編集調整時は[TestVal]の確認（※スロットの単体描画の際は色バッファにテストパレット値を設定しておく）
    //（※顔の上に配置するパーツ等は特定色の上にのみ描画するため、単体描画の際は対象の色でバッファを塗りつぶしておく必要がある）
    //--------------------------------------------------------------------------------------------------
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
            memset( s_pBufColor, BP_CONV_PAL_INFO_TO_DOT(testPalGrp, BEZIER_PAL_VAL_BASE), s_nSizeBuf );
        }
    }

    // 計測開始
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-[START] CBmpGenerator::CreateTexWithGenParam\n" );
    }
#endif
    time_log_start( eTIME_LOG_ID_TEX_GEN );

    // ワーククリア
    pGenParam->resetForWork();

    //------------------------------------------
    // オーダーリスト確保（※ワークなので[APP]から確保）
    //------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    CLayerOrderList* pOrderList = new CLayerOrderList();
    CMemMgr::PopTargetField();

#ifdef BMP_GEN_LOG
    LOGD( "@ INIT ORDER\n" );
#endif

    //------------------------------------------
    // オーダーリストの初期化
    //------------------------------------------
    InitOrderList( pOrderList, pGenParam->getForm(), pGenParam->isSuited() );

    //------------------------------------------
    // オーダー登録パラメータの設定
    //------------------------------------------
    stREGIST_ORDER_PARAM rop;
    CLEAR_REGIST_ORDER_PARAM( &rop );
    
    rop.isFlipH = s_stBaseParam.isFlipH;
    rop.isFlipV = s_stBaseParam.isFlipV;
    rop.scale = s_stBaseParam.scale * workScale;
    rop.rot = s_stBaseParam.rot;
    rop.baseX = s_nTexW/2.0f;
    rop.baseY = s_nTexH/2.0f;
    // 作業スケールはストロークには反映させない（※線が太くなってこまかな形状が見え辛いので）
    //rop.strokeScale = workScale;
    rop.isSlotIgnore = isSlotIgnore;
    
    //---------------------------------------------------------
    // オーダー登録：指定のパーツを起点に内部のスロットを順次登録していく
    //---------------------------------------------------------
#ifdef BMP_GEN_LOG
    LOGD( "@ START: SET ORDER LIST\n" );
#endif
    SetOrderList( pOrderList, pGenParam, slot, slotIndex, false, &s_stBaseParam, rop, isWorkPath );

#ifdef BMP_GEN_LOG
    LOGD( "@ FINISH: SET ORDER LIST\n" );
#endif

    //-----------------------------------------
    // オーダーリストの確定（※レイヤーフックの解決）
    //-----------------------------------------
#ifdef BMP_GEN_LOG
    LOGD( "@ FIX ORDER LIST\n" );
#endif

    FixOrderList( pOrderList );

    //-----------------------------------------
    // オーダーリストの描画
    //-----------------------------------------
#ifdef BMP_GEN_LOG
    LOGD( "@ START: DRAW ORDER LIST\n" );
#endif

    DrawOrderList( pOrderList, pGenParam->isSuited(), isWorkPath );

#ifdef BMP_GEN_LOG
    LOGD( "@ FINISH: DRAW ORDER LIST\n" );
#endif

    //----------------------------------------------------------------------------
    // 検出座標
    //----------------------------------------------------------------------------
    // 基本座標（接地座標）：足元を判定するための座標（※両脚先に指定しておき、その中点を抽出する）
    // フォーカス座標：顔にフォーカスするための座標（※とりあえず鼻の座標を抽出）
    //----------------------------------------------------------------------------
    CAnglePlane* pAngle = s_stBaseParam.pAnglePlane;
    
    // アングルが有効なら傾きの適用
    if( pAngle != NULL ){
        // 基本座標
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
    }
    // アングルが無効ならそのままの値を抽出
    else{
        // 基本座標
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
// BMP生成：レイヤーリスト指定
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

    // 表示用のレイヤーパラメータ基本設定（※このパラメータを各レイヤーで使い回す）
    stBEZIER_LAYER_PARAM stLayerParam;
    CLEAR_BEZIER_LAYER_PARAM( &stLayerParam );

    // スロット設定（※とりあえず顔）
    stLayerParam.slot = eBD_SLOT_FcBase;
    stLayerParam.parentSlot = eBD_SLOT_FcBase;

    stLayerParam.fixedAdjustRateH = s_stBaseParam.adjustRateH;
    FIX_ADJUST_RATE( stLayerParam.fixedAdjustRateH );

    stLayerParam.fixedAdjustRateV = s_stBaseParam.adjustRateV;
    FIX_ADJUST_RATE( stLayerParam.fixedAdjustRateV );

    stLayerParam.fixedAngleRateLR = s_stBaseParam.adjustRateLR;
    FIX_ADJUST_RATE( stLayerParam.fixedAngleRateLR );

    stLayerParam.fixedAngleRateUD = s_stBaseParam.adjustRateUD;
    FIX_ADJUST_RATE( stLayerParam.fixedAngleRateUD );

    stLayerParam.pixelOfsX = s_nTexW/2.0f;
    stLayerParam.pixelOfsY = s_nTexH/2.0f;
    stLayerParam.pixelScaleX = ((s_stBaseParam.isFlipH)? -s_stBaseParam.scale: s_stBaseParam.scale);
    stLayerParam.pixelScaleY = ((s_stBaseParam.isFlipV)? -s_stBaseParam.scale: s_stBaseParam.scale);
    stLayerParam.pixelRot = s_stBaseParam.rot;

    // 傾きの適用（とりあえずテクスチャ中央）
    SetAnglePlaneInfo( eBD_SLOT_FcBase, 0, s_nTexW/2, s_nTexH/2, false, false, 0, 1.0f );

    // 要素の描画（※リスト内の全てのレイヤーの表示）
    CLayerData* pLayerData = (CLayerData*) pLayerDataList->getHead();
    while( pLayerData != NULL ){
        // パラメータの退避
        float x0 = stLayerParam.pixelOfsX;
        float y0 = stLayerParam.pixelOfsY;
        float rot0 = stLayerParam.pixelRot;
        float scaleX0 = stLayerParam.pixelScaleX;
        float scaleY0 = stLayerParam.pixelScaleY;

        // 遅延によるレイヤーの調整
        int rate, rateX, rateY;

        // 遅延：座標（※ここは左右の影響を受けない＝移動値の方向そのままを受け取る）
        CAdjustCalc::CalcRateOfsXYForDelayLog( &rateX, &rateY, false, false, pLayerData, s_stBaseParam.pDelayLog );
        stLayerParam.pixelOfsX += CConst::ConvBezierPositionRate( s_nTexW0, rateX );
        stLayerParam.pixelOfsY += CConst::ConvBezierPositionRate( s_nTexH0, rateY );

        // 遅延：回転
        CAdjustCalc::CalcRateRotForDelayLog( &rate, false, false, pLayerData, s_stBaseParam.pDelayLog );
        stLayerParam.pixelRot += CConst::ConvBezierRotationRate( rate );

        // 遅延：スケール
        CAdjustCalc::CalcRateScaleForDelayLog( &rate, pLayerData, s_stBaseParam.pDelayLog );
        stLayerParam.pixelScaleX *= CConst::ConvBezierScaleRate( rate );
        stLayerParam.pixelScaleY *= CConst::ConvBezierScaleRate( rate );

        // 描画
        pLayerData->draw( &s_stBaseParam, &stLayerParam, isWorkPath );
        pLayerData = (CLayerData*) pLayerData->getNext();

        // パラメータの復旧
        stLayerParam.pixelOfsX = x0;
        stLayerParam.pixelOfsY = y0;
        stLayerParam.pixelRot = rot0;
        stLayerParam.pixelScaleX = scaleX0;
        stLayerParam.pixelScaleY = scaleY0;
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
    s_stBaseParam.pBufTemp = s_pBufTemp;

    s_stBaseParam.pixelRate = s_nTexResolution;
    s_stBaseParam.texW0 = s_nTexW0;
    s_stBaseParam.texH0 = s_nTexH0;
    s_stBaseParam.texW = s_nTexW;
    s_stBaseParam.texH = s_nTexH;
    
    s_stBaseParam.isFlipH = pCreateParam->isFlipH;
    s_stBaseParam.isFlipV = pCreateParam->isFlipV;
    s_stBaseParam.rot = pCreateParam->rot;
    s_stBaseParam.scale = pCreateParam->scale;

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            s_stBaseParam.arrOption[i][j] = pCreateParam->arrOption[i][j];
        }
    }

    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            for( int k=0; k<BD_OPTION_FOR_SLOT_MAX; k++ ){
                s_stBaseParam.arrOptionForSlot[i][j][k] = pCreateParam->arrOptionForSlot[i][j][k];
            }
        }
    }
    
    s_stBaseParam.adjustRateH = pCreateParam->adjustRateH;
    s_stBaseParam.adjustRateV = pCreateParam->adjustRateV;
    s_stBaseParam.adjustRateLR = pCreateParam->adjustRateLR;
    s_stBaseParam.adjustRateUD = pCreateParam->adjustRateUD;
    
    s_stBaseParam.pDelayLog = pCreateParam->pDelayLog;
    s_stBaseParam.pJointPoint = &s_oJointPoint;
    s_stBaseParam.pAnglePlane = &s_oAnglePlane;
    
    s_stBaseParam.strokeScale = s_fBaseStrokeScale;

#if 1
    // 傾き面のデフォルト値の設定（※とりあえず）
    s_oAnglePlane.registPlaneDefault( s_nTexW, s_nTexH, s_nTexW/2.0f, s_nTexH/2.0f, 
                                      pCreateParam->isFlipH, pCreateParam->isFlipV, pCreateParam->rot, pCreateParam->scale );
#endif

    // スロット位置の初期化（※用心に原点＝中心を指定しておく）
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            s_fArrSlotBaseX[i][j] = s_nTexW/2.0f;
            s_fArrSlotBaseY[i][j] = s_nTexH/2.0f;
        }
    }
    
    // デフォルト出力値の設定
    s_fTexBaseX = s_nTexW/2;
    s_fTexBaseY = s_nTexH/2;
    s_fTexFocusX = s_nTexW/2;
    s_fTexFocusY = s_nTexH/2;
    
    s_fTexGroundX0_L = s_fTexGroundX0_R = s_nTexW/2;
    s_fTexGroundY0_L = s_fTexGroundY0_R = s_nTexW/2;
    s_fTexFocusX0 = s_nTexW/2;
    s_fTexFocusY0 = s_nTexH/2;
    
    // ベジェリ関連セット
    CBezier::ResetHookPoint();
    CBezier::ResetTouchPoint();
    CBezier::ResetGuidePoint();
    
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
    
    // サイズ確認（※５枚分確保したい）
    int required = 5*s_nSizeBuf;
    //LOGD( "@ BMP GEN ALLOCATE %d/%d(%.1f%%)\n", required, sizeBuf, CConst::CalcUseRate(required,sizeBuf) );
    if( required > sizeBuf ){
        LOGE( "@ CBmpGenerator::AllocWorkBuf: BUF SHORTAGE: required=%d > allocted=%d\n", 5*s_nSizeBuf, sizeBuf );
        return( false );
    }
    
    // バッファ確保
    s_pBufLine = &pBuf[0*s_nSizeBuf];           // 線画像バッファ
    s_pBufColor = &pBuf[1*s_nSizeBuf];          // 色画像バッファ
    s_pBufFillGuide = &pBuf[2*s_nSizeBuf];      // 塗りガイドバッファ
    s_pBufFillGuard = &pBuf[3*s_nSizeBuf];      // 塗りガードバッファ
    s_pBufTemp = &pBuf[4*s_nSizeBuf];           // テンポラリバッファ

    return( true );
}

//----------------------------------------------------------
// ワークバッファリセット
//----------------------------------------------------------
void CBmpGenerator::ResetWorkBuf( void ){
    // バッファクリア
    ResetLineBuf();
    ResetColorBuf();
    ResetFillGuideBuf();
    ResetFillGuardBuf();
    ResetTempBuf();

    // ワーククリア
    CLEAR_BEZIER_BASE_PARAM( &s_stBaseParam );
    s_oJointPoint.clear();
    s_oAnglePlane.clear();

    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            s_fArrSlotBaseX[i][j] = 0.0f;
            s_fArrSlotBaseY[i][j] = 0.0f;
        }
    }
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
        ofsL = GetLeftForColorBuf();
        ofsR = s_nTexW - (GetRightForColorBuf()+1);
        ofsT = GetTopForColorBuf();
        ofsB = s_nTexH - (GetBottomForColorBuf()+1);
    }else{
        ofsL = GetLeftForLineBuf();
        ofsR = s_nTexW - (GetRightForLineBuf()+1);
        ofsT = GetTopForLineBuf();
        ofsB = s_nTexH - (GetBottomForLineBuf()+1);
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
