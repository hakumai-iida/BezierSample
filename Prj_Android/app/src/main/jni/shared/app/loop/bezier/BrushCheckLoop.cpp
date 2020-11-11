/*+----------------------------------------------------------------+
  | Title:      BrushCheckLoop.cpp [共通環境]
  | Comment:    ブラシ確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "BrushCheckLoop.hpp"
#include "StrokeCheckLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum eSTEP{
	eSTEP_SELECT_BRUSH,
    eSTEP_EDIT_DATA,
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
#define BRUSH_WORK      eBRUSH_C_BASE

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
CBrushCheckLoop::CBrushCheckLoop( void ){
	setId( eLOOP_ID_BRUSH_CHECK );

    setDrawLayer( eDP_LAYER_LOOP );
    
    // BMP環境リセット
    CBmpGenerator::Reset();
    
    // 領域確保
    allocForTest();
}

//------------------------
// デストラクタ
//------------------------
CBrushCheckLoop::~CBrushCheckLoop( void ){
    // 領域開放
    releaseForTest();
}

//------------------------
// 初期化
//------------------------
void CBrushCheckLoop::init0( void ){
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
    // レイアウト：右上
    //--------------------------
    setLayoutRT( -64-4, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

    // 戻るボタン
    m_pButtonBack = new CButton( eGRP_DEBUG_BUTTON_L, "BACK" );
    putNodeOnLayout( m_pButtonBack, 0, 2 );

    //--------------------------
	// レイアウト：左上
    //--------------------------
	setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

	// メニュー：ブラシ
	m_pMenuBrush = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 256 );
	m_pMenuBrush->setTitle( "Brush Menu");
	m_pMenuBrush->setItemArrayThenFix( g_pArrLabelBrush );
    m_pMenuBrush->setSelect( BRUSH_WORK );
	putNodeOnLayout( m_pMenuBrush );
    
    //---------------------------------------------
    // レイアウト：左下
    //---------------------------------------------
    setLayoutLB( -2, +2, eLAYOUT_DIR_F, eLAYOUT_DIR_N );
    
    // メニュー：値編集
    m_pMenuEV = new CEditValueMenu( eGRP_DEBUG_WINDOW, 256, 110 );
    m_pMenuEV->setTitle( "EDIT VALUE" );
    putNodeOnLayout( m_pMenuEV );

    //---------------------------------------------
    // レイアウト：右下
    //---------------------------------------------
    setLayoutRB( 0, 0, eLAYOUT_DIR_F, eLAYOUT_DIR_N );

    // ダイアログ：値編集
    m_pDialogEV = new CEditValueDialog();
    putNodeOnLayout( m_pDialogEV );

    //--------------------------
	// Node登録
    //--------------------------
    addNode( m_pMenuEV );
    addNode( m_pDialogEV );

	addNode( m_pMenuBrush );
    
    addNode( m_pButtonDot );
    addNode( m_pButtonUpdate );
	addNode( m_pButtonBack );
	addNode( m_pButtonExit );
    
    // 初期値
    m_bDot = false;
    m_nCalcNum = 0;
    m_nCalcTimeTotal = 0;
    m_nCalcTimeAvg = 0;

	// ループ開始
	setStep( eSTEP_SELECT_BRUSH );

	// ジェスチャー設定
	setGestureBase( IMG_BASE_X, IMG_BASE_Y, IMG_DRAG_R );
	setGestureMoveXY( IMG_BASE_X, IMG_BASE_Y );
	setGestureScale( IMG_BASE_SCALE );
}

//------------------------
// 終了
//------------------------
void CBrushCheckLoop::exit0( void ){}

//------------------------
// 更新
//------------------------
void CBrushCheckLoop::update0( void ){
	// ループの終了
	if( m_pButtonExit->isTapped() ){
		CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_EXIT, 0, 0 );
		return;
	}
    
    CDefBinTable<CBrushBinData>* pTable = CDefTable::GetBrushTable();
    CBrushBinData* pBin;
    int updateTexNum = 0;
	int selectBrush = m_pMenuBrush->getSelect();
    
    // ボタン
    if( m_pButtonUpdate->isTapped() ){ updateTexNum = 8; }
    else if( m_pButtonDot->isTapped() ){
        m_bDot = !m_bDot;
        m_pButtonDot->setFocused( m_bDot );
        updateTexNum = 1;
    }
    
	if( isStepChanged() ){
		exitNodeAll();

        // 共通設定
        m_pButtonExit->enter();
        m_pButtonUpdate->enter();
        m_pButtonDot->enter();

        // とりあえず更新
        updateTexNum = 1;

        switch( m_nStep ){
		case eSTEP_SELECT_BRUSH:
            m_pMenuBrush->enter();
			break;
                
        case eSTEP_EDIT_DATA:
            m_pButtonBack->enter();

            // 編集メニュー設定
            pBin = pTable->getDataAt( selectBrush );
            if( pBin != NULL ){
                pBin->setEditValueMenu( m_pMenuEV );
                m_pMenuEV->setEditValueDialog( m_pDialogEV );

                m_pMenuEV->enter();
                m_pDialogEV->enter();
            }
            break;
		}

		fixStep();
	}else{
		switch( m_nStep ){
        case eSTEP_SELECT_BRUSH:
            if( m_pMenuBrush->isDecided() ){
                setStep( eSTEP_EDIT_DATA );
            }else if( m_pMenuBrush->isSelectChanged() ){
                updateTexNum = 1;
            }
			break;
                
        case eSTEP_EDIT_DATA:
            if( m_pButtonBack->isTapped() ){
                setStep( eSTEP_SELECT_BRUSH );
            }else if( m_pMenuEV->isChanged() || m_pDialogEV->isChanged() ){
                updateTexNum = 1;
            }else if( m_pMenuEV->isDecided() || m_pMenuEV->isSelectChanged() ){
                m_pMenuEV->setEditValueDialog( m_pDialogEV );
                m_pDialogEV->enter();
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
        updateTex( (eBRUSH)selectBrush, m_bDot, updateTexNum );
    }

	// ジェスチャ更新
    checkGestureScale();
	checkGestureMove();
}

//------------------------
// 描画
//------------------------
void CBrushCheckLoop::onDraw0( void ){
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
    pDC->setTex( m_pBmpTex->getTexForLine(), NULL );
    pDC->setWidth( w );
    pDC->setHeight( h );
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
    pDC->setBlendAlpha();
    pDC->setRGBA( 0x000000FF );
    pDC->draw( x, y );

    //-----------------------
    // 情報
    //-----------------------
    CDrawCell::SetLogAlignLT( LOG_SCALE, LOG_OFS_X, LOG_OFS_Y );
    
    sprintf( buf, "time:%d.%dms (%dus/%d)", m_nCalcTimeAvg/1000, (m_nCalcTimeAvg%1000)/100, m_nCalcTimeTotal, m_nCalcNum );
    CDrawCell::DrawLogAlign( buf );
}

//----------------------
// テストデータ確保
//----------------------
void CBrushCheckLoop::allocForTest( void ){
    // 用心に開放しておく
    releaseForTest();
    
    // テクスチャ確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    m_pBmpTex = new CBmpTex();
    CMemMgr::PopTargetField();

    // テストデータ作成
    CStrokeCheckLoop::CreateLayerDataForBrush( &m_pLayer, (eBRUSH)0 );
}

//----------------------
// テストデータ開放
//----------------------
void CBrushCheckLoop::releaseForTest( void ){
    SAFE_DELETE( m_pBmpTex );

    CStrokeCheckLoop::ReleaseLayerDataForBrush( &m_pLayer );
}

//----------------------
// テクスチャ更新
//----------------------
void CBrushCheckLoop::updateTex( eBRUSH brush, bool isDot, int num ){
    // ブラシ／パレットを上書き
    CLayerObject* pObj = m_pLayer->getDataHead();
    while( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData* pLine = pObj->getLineObject();
            pLine->setBrushId( brush );

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
        
        m_pBmpTex->createBmpWithLayerList( &createParam, &list );
        m_nCalcTimeTotal += m_pBmpTex->getBmpGenTime();
    }
    
    m_nCalcTimeAvg = m_nCalcTimeTotal/ m_nCalcNum;
}
