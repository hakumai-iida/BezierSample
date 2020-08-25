/*+----------------------------------------------------------------+
  | Title:      StrokeCheckLoop.cpp [共通環境]
  | Comment:    ストローク確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "StrokeCheckLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum eSTEP{
	eSTEP_SELECT,
};

// 画像位置
#define IMG_BASE_X		SA_XC
#define IMG_BASE_Y		SA_YM
#define IMG_DRAG_R		128
#define IMG_BASE_SCALE	CMainRenderer::GetGamePixelRate()

// ログ位置（左上）
#define LOG_OFS_X		200
#define LOG_OFS_Y		0
#define LOG_SCALE		1.0f

// 作業ブラシ
#define STROKE_WORK     eSTROKE_TYPE_CIRCLE
#define SIZE_WORK       (4-1)

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
//------------------------
// コンストラクタ
//------------------------
CStrokeCheckLoop::CStrokeCheckLoop( void ){
    setId( eLOOP_ID_STROKE_CHECK );
    
	setDrawLayer( eDP_LAYER_LOOP );
    
    // BMP環境リセット
    CBmpGenerator::Reset();

    // 領域確保
    allocForTest();
}

//------------------------
// デストラクタ
//------------------------
CStrokeCheckLoop::~CStrokeCheckLoop( void ){
    // 領域開放
    releaseForTest();
}

//------------------------
// 初期化
//------------------------
void CStrokeCheckLoop::init0( void ){
    //--------------------------
	// レイアウト：右上
    //--------------------------
	setLayoutRT( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

	// 終了ボタン
	m_pButtonExit = new CButton( eGRP_DEBUG_BUTTON, "EXIT" );
	putNodeOnLayout( m_pButtonExit );

    // 更新ボタン
    m_pButtonUpdate = new CButton( eGRP_DEBUG_BUTTON, "UPDATE" );
    putNodeOnLayout( m_pButtonUpdate );

    // ドットボタン
    m_pButtonDot = new CButton( eGRP_DEBUG_BUTTON, "DOT" );
    putNodeOnLayout( m_pButtonDot );

    //--------------------------
	// レイアウト：左下
    //--------------------------
	setLayoutLB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_N );

	// 戻るボタン
	m_pButtonBack = new CButton( eGRP_DEBUG_BUTTON_L, "BACK" );
	putNodeOnLayout( m_pButtonBack );

    //--------------------------
    // レイアウト：右下
    //--------------------------
    setLayoutRB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );

    // レンジ：増加
    m_pButtonRangeInc = new CButton( eGRP_DEBUG_BUTTON_L, "" );
    m_pButtonRangeInc->setFlipX( true );
    putNodeOnLayout( m_pButtonRangeInc );

    //--------------------------
    // レイアウト：右下
    //--------------------------
    setLayoutRB( -128, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );

    // レンジ：増加
    m_pButtonRangeDec = new CButton( eGRP_DEBUG_BUTTON_L, "" );
    putNodeOnLayout( m_pButtonRangeDec );

    //--------------------------
	// レイアウト：左上
    //--------------------------
	setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

	// メニュー：ストローク
	m_pMenuStroke = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 80 );
	m_pMenuStroke->setTitle( "Stroke Menu");
	m_pMenuStroke->setItemArrayThenFix( g_pArrLabelStrokeType );
    m_pMenuStroke->setSelect( STROKE_WORK );
	putNodeOnLayout( m_pMenuStroke );

	// メニュー：サイズ
	m_pMenuSize = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 128 );
    m_pMenuSize->setTitle( "Size Menu");

    // 項目登録
    char* pBuf = CMemMgr::GetBufFromTempStr();
    int num = STROKE_DOT_BASE_SIZE_MAX*STROKE_DOT_SIZE_PRECISION;
    m_pMenuSize->setItemNum( num );
    for( int i=0; i<num; i++ ){
        float size = (i+1.0f)/STROKE_DOT_SIZE_PRECISION;
        sprintf( pBuf, "size:%.2f", size );
        m_pMenuSize->setItemAt( i, pBuf );
    }
    m_pMenuSize->fixMenu();
    m_pMenuSize->setSelect( SIZE_WORK );
    
	putNodeOnLayout( m_pMenuSize );
    
    //--------------------------
	// Node登録
    //--------------------------
    addNode( m_pMenuSize );
	addNode( m_pMenuStroke );
    
    addNode( m_pButtonRangeInc );
    addNode( m_pButtonRangeDec );
    addNode( m_pButtonDot );
    addNode( m_pButtonUpdate );
	addNode( m_pButtonBack );
	addNode( m_pButtonExit );
    
  
    // 初期値
    m_bDot = false;
    m_nOfsRange1000 = 0;
    
    m_nCalcNum = 0;
    m_nCalcTimeTotal = 0;
    m_nCalcTimeAvg = 0;

	// ループ開始
	setStep( eSTEP_SELECT );

	// ジェスチャー設定
	setGestureBase( IMG_BASE_X, IMG_BASE_Y, IMG_DRAG_R );
	setGestureMoveXY( IMG_BASE_X, IMG_BASE_Y );
	setGestureScale( IMG_BASE_SCALE );
}

//------------------------
// 終了
//------------------------
void CStrokeCheckLoop::exit0( void ){}

//------------------------
// 更新
//------------------------
void CStrokeCheckLoop::update0( void ){
	// ループの終了
	if( m_pButtonExit->isTapped() ){
		CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_EXIT, 0, 0 );
		return;
	}
    
    int updateTexNum = 0;
	int selectStroke = m_pMenuStroke->getSelect();
    int selectSize = m_pMenuSize->getSelect();
    int ofsRange0 = m_nOfsRange1000;
    
    // ボタン
    if( m_pButtonUpdate->isTapped() ){ updateTexNum = 8; }
    else if( m_pButtonDot->isTapped() ){
        m_bDot = !m_bDot;
        m_pButtonDot->setFocused( m_bDot );
        updateTexNum = 1;
    }
    else if( m_pButtonRangeInc->isTapped() ){ m_nOfsRange1000 += 250; }
    else if( m_pButtonRangeDec->isTapped() ){ m_nOfsRange1000 -= 250; }
    
    if( m_nOfsRange1000 < 0 ){ m_nOfsRange1000 = 0; }
    if( m_nOfsRange1000 != ofsRange0 ){ updateTexNum = 1; }

	if( isStepChanged() ){
		exitNodeAll();

		switch( m_nStep ){
		case eSTEP_SELECT:
            m_pButtonExit->enter();
            m_pButtonUpdate->enter();
            m_pButtonDot->enter();
            m_pButtonRangeDec->enter();
            m_pButtonRangeInc->enter();

            m_pMenuStroke->enter();
            m_pMenuSize->enter();

            updateTexNum = 1;
			break;
		}

		fixStep();
	}else{
		switch( m_nStep ){
		case eSTEP_SELECT:
			if( m_pMenuStroke->isSelectChanged() ){
                updateTexNum = 1;
            }else if( m_pMenuSize->isSelectChanged() ){
                updateTexNum = 1;
            }
			break;
		}

		if( isStepChanged() ){
			setActiveNodeAll( false );
		}
	}
    
    // 指定があれば更新
    if( updateTexNum > 0 ){
        updateTex( (eSTROKE_TYPE)selectStroke, selectSize, m_nOfsRange1000, m_bDot, updateTexNum );
    }

	// ジェスチャ更新
    checkGestureScale();
	checkGestureMove();
}

//------------------------
// 描画
//------------------------
void CStrokeCheckLoop::onDraw0( void ){
    CDrawCell* pDC = CDrawCell::GetFreeCell();
    char* buf = CMemMgr::GetBufFromTempStr();
    
    float s = m_fGestureScale / IMG_BASE_SCALE;
    float x = m_fGestureMoveX;
    float y = m_fGestureMoveY;
    float w0 = s * CBmpGenerator::GetBaseTexSizeW();
    float h0 = s * CBmpGenerator::GetBaseTexSizeH();
    float w = s * CBmpGenerator::GetTotalTexSizeW();
    float h = s * CBmpGenerator::GetTotalTexSizeH();

    // ジェスチャ領域表示
    drawGestureBase( 0xFFFFFF40 );
    
    //-------------------------------
    // 下地
    //-------------------------------
    pDC->clear();
    pDC->setRect( w, h );
    pDC->setRGBA( 0xFFFFFF40 );
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
    pDC->setBlendAlpha();
    pDC->draw( x, y );

    pDC->clear();
    pDC->setRect( w0, h0 );
    pDC->setRGBA( 0xFFFFFF80 );
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
    pDC->setBlendAlpha();
    pDC->draw( x, y );

    //-------------------------------
    // [GRAY] テクスチャ表示：線
    //-------------------------------
    pDC->clear();
    pDC->setTex( m_pTex, NULL );
    pDC->setWidth( w );
    pDC->setHeight( h );
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
    pDC->setBlendAlpha();
    pDC->setRGBA( 0x000000FF );
    pDC->draw( x, y );

    //-------------------------------
    // 情報
    //-------------------------------
    x = SA_XR - 84;
    y = SA_YB - 20;
    
    sprintf( buf, "RNG:%d", m_nOfsRange1000 );
    pDC->clear();
    pDC->setStr( eFONT_DEBUG, buf );
    pDC->setScaleY( 2.0f );
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
    pDC->draw( x, y );
    y -= 32 + 4;
    
    //-----------------------
    // 情報
    //-----------------------
    CDrawCell::SetLogAlignLT( LOG_SCALE, LOG_OFS_X, LOG_OFS_Y );
    
    sprintf( buf, "time:%d.%dms (%dus/%d)", m_nCalcTimeAvg/1000, (m_nCalcTimeAvg%1000)/100, m_nCalcTimeTotal, m_nCalcNum );
    CDrawCell::DrawLogAlign( buf );
}

//--------------------------------
// ベジェ確認：バッファ確保：ラインテスト
//--------------------------------
void CStrokeCheckLoop::allocForTest( void ){
    // 用心に開放しておく
    releaseForTest();
    
    // テクスチャ確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    m_pTex = new CTex();
    CMemMgr::PopTargetField();

    // テストデータ作成
    CreateLayerDataForBrush( &m_pLayer, eBRUSH_WORK );
}

//----------------------
// テストデータ開放
//----------------------
void CStrokeCheckLoop::releaseForTest( void ){
    SAFE_DELETE( m_pTex );
    ReleaseLayerDataForBrush( &m_pLayer );
}

//----------------------
// テクスチャ更新
//----------------------
void CStrokeCheckLoop::updateTex( eSTROKE_TYPE stroke, int sizeAt, int range1000, bool isDot, int num ){
    // ブラシ上書き
    CDefBinTable<CBrushBinData>* pTable = CDefTable::GetBrushTable();
    CBrushBinData* pBin = pTable->getDataAt( eBRUSH_WORK );
    pBin->setStrokeType( stroke );
    pBin->setStrokeSize1000( 1000*(sizeAt+1)/STROKE_DOT_SIZE_PRECISION );
    pBin->setStrokeRange1000( range1000 );
    
    // パレットを上書き
    CLayerObject* pObj = m_pLayer->getDataHead();
    while( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData* pLine = pObj->getLineObject();
            
            if( isDot ){
                pLine->setFlagOn( eLOD_FLAG_DOT );
            }else{
                pLine->setFlagOff( eLOD_FLAG_DOT );
            }
        }
        pObj = pObj->getNext();
    }

    //----------------------------------
    // 描画
    //----------------------------------
    // 作成パラメータ
    stBMP_GEN_CREATE_PARAM createParam;
    CLEAR_BMP_GEN_CREATE_PARAM( &createParam );
    
    // 計測用に指定回数繰り返す
    m_nCalcTimeTotal = 0;
    for( m_nCalcNum=0; m_nCalcNum<num; m_nCalcNum++ ){
        CList list;
        list.add( m_pLayer );
        m_nCalcTimeTotal += CBmpGenerator::CreateTexWithLayerList( m_pTex, NULL, &list, &createParam, false );
    }
    
    m_nCalcTimeAvg = m_nCalcTimeTotal/ m_nCalcNum;
}

//--------------------------------------------
// ストローク確認用データ作成
//（※この処理は[CBrushCheckLoop]からも参照される）
//--------------------------------------------
bool CStrokeCheckLoop::CreateLayerDataForBrush( CLayerData** ppData, eBRUSH brush ){
    // 用心に解放
    ReleaseLayerDataForBrush( ppData );
    
    // データ確保
    CLayerData* pLD = CLayerData::Alloc();
    *ppData = pLD;

    //----------------------------------------
    // work（※オブジェクト＆ポイントをそれぞれ）
    //----------------------------------------
    CLineObjectData* pLOD;
    CAnchorPointData* pAP;
    
    //----------------------------
    // [左上] 線をひく：直線（ヨコ）
    //----------------------------
    // １本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 5000 );
    pLOD->setTempAdjust( -3333, -3500-666, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 0, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 0, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // ２本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( -3333, -3500-0, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 0, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 0, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // ３本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 30000 );
    pLOD->setTempAdjust( -3333, -3500+666, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 0, 0, 0, 0, 0 );
    pAP->setStroke(  0,10000, 10000,0 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 0, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [中央上] 線をひく：直線（タテ）
    //----------------------------
    // １本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( 0-666, -3333, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, -4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,10000, 10000,5000 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, 4000, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // ２本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 20000 );
    pLOD->setTempAdjust( 0-0, -3333, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, -4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,10000, 10000,2500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, 4000, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // ３本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 30000 );
    pLOD->setTempAdjust( 0+666, -3333, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, -4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,10000, 10000,2500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, 4000, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [右上] 線をひく：直線（ナナメ）
    //----------------------------
    // １本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 30000 );
    pLOD->setTempAdjust( +3333, -2500, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 4000, 0, 0, 0, 0 );
    pAP->setStroke( 10000,2500, 0,10000 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, -4000, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    // ２本目
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 30000 );
    pLOD->setTempAdjust( +3333, -2500, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,10000, 10000,2500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, -4000, 0, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [左中央] 線をひく：曲線
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( -3333, -1333, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 0, 0, 0, 0, 4000 );
    pAP->setStroke( 3333,30000, 3333,5000 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, 0, 0, 4000, 0, -4000 );
    pAP->setStroke( 3333,5000, 3333,30000 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 0, 0, -4000, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [中央] 線をひく：曲線
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setStrokeSize( 20000 );
    pLOD->setTempAdjust( 0, -1333, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 0, 0, 0, 0, 4000 );
    pAP->setStroke( 3333,1000, 3333,1000 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 0, 0, 4000, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [右中央] 線をひく：曲線
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( 3333, -2500, 3333, 3333 );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, -6000, 0, 0, -4000, 0 );
    pAP->setStroke( 6666,30000, 3333,2000 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, 6000, 4000, 0, 0, 0 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [左下] 線をひく：矩形
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( -2500, 2500, 5000, 5000 );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, -4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,0, 0,7500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, -4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,7500, 0,15000 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( 4000, 4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,15000, 0,22500 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( -4000, 4000, 0, 0, 0, 0 );
    pAP->setStroke( 0,22500, 0,30000 );
    pLOD->addData( pAP );
    
    //----------------------------
    // [右下] 線をひく：円
    //----------------------------
    pLOD = CLineObjectData::Alloc();
    pLOD->setBrushId( brush );
    pLOD->setTempAdjust( 2500, 2500, 5000, 5000 );
    pLOD->setFlagOn( eLOD_FLAG_CLOSE_PATH );
    pLD->addData( pLOD );
    
    int r = 4000;
    int dirPow = (r*55555)/100000;
    
    // 点０
    pAP = CAnchorPointData::Alloc();
    pAP->set( -r, 0, 0, dirPow, 0, -dirPow );
    pAP->setStroke( 0,30000, 0,22500 );
    pLOD->addData( pAP );
    
    // 点１
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, -r, -dirPow, 0, dirPow, 0 );
    pAP->setStroke( 0,22500, 0,15000 );
    pLOD->addData( pAP );
    
    // 点２
    pAP = CAnchorPointData::Alloc();
    pAP->set( r, 0, 0, -dirPow, 0, dirPow );
    pAP->setStroke( 0,15000, 0,7500 );
    pLOD->addData( pAP );
    
    // 点３
    pAP = CAnchorPointData::Alloc();
    pAP->set( 0, r, dirPow, 0, -dirPow, 0 );
    pAP->setStroke( 0,7500, 0,0 );
    pLOD->addData( pAP );
    
    return( true );
}

//--------------------------
// ストローク確認用データ解放
//--------------------------
bool CStrokeCheckLoop::ReleaseLayerDataForBrush( CLayerData** ppData ){
    CLayerData::Free( *ppData );
    *ppData = NULL;

    return( true );
}
