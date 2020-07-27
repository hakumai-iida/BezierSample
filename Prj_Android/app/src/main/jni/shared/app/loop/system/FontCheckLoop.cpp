/*+----------------------------------------------------------------+
  |	Title:		FontCheckLoop.hpp [共通環境]
  |	Comment:	フォント確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "FontCheckLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum eSTEP{
	eSTEP_SELECT_CATEGORY,
	eSTEP_SELECT_TARGET,
};

// テキスト位置
#define CHAR_BASE_X		    SA_XC
#define CHAR_BASE_Y		    SA_YM
#define CHAR_BASE_SCALE     CMainRenderer::GetGamePixelRate()

// 画像位置
#define GESTURE_BASE_X      SA_XC
#define GESTURE_BASE_Y      SA_YM
#define GESTURE_DRAG_R      128

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
CFontCheckLoop::CFontCheckLoop( void ){
	setId( eLOOP_ID_FONT_CHECK );

    setDrawLayer( eDP_LAYER_LOOP );
}

//------------------------
// デストラクタ
//------------------------
CFontCheckLoop::~CFontCheckLoop( void ){
}

//------------------------
// 初期化
//------------------------
void CFontCheckLoop::init0( void ){
	// レイアウト：右上
	setLayoutRT( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );

	// 終了ボタン
	m_pButtonExit = new CButton( eGRP_DEBUG_BUTTON, "EXIT" );
	putNodeOnLayout( m_pButtonExit);

	// 戻るボタン
	m_pButtonBack = new CButton( eGRP_DEBUG_BUTTON_L, "BACK" );
	putNodeOnLayout( m_pButtonBack);

	// レイアウト：左上
	setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

	// メニュー：カテゴリー
	m_pMenuCategory = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 74 );
	m_pMenuCategory->setTitle( "Category Menu");
	m_pMenuCategory->setItemArrayThenFix( g_pArrLabelFont );
	putNodeOnLayout( m_pMenuCategory );

	// メニュー：ターゲット
	m_pMenuTarget = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 256 );
	m_pMenuTarget->setTitle( "Char Menu" );
	putNodeOnLayout( m_pMenuTarget );

	// Node登録
	addNode( m_pMenuTarget );
	addNode( m_pMenuCategory );
	addNode( m_pButtonBack );
	addNode( m_pButtonExit );

	// ループ開始
	setStep( eSTEP_SELECT_CATEGORY );
    
    // ジェスチャー設定
    setGestureBase( GESTURE_BASE_X, GESTURE_BASE_Y, GESTURE_DRAG_R );
    setGestureMoveXY( CHAR_BASE_X, CHAR_BASE_Y );
    setGestureScale( CHAR_BASE_SCALE );
}

//------------------------
// 終了
//------------------------
void CFontCheckLoop::exit0( void ){
}

//------------------------
// 更新
//------------------------
void CFontCheckLoop::update0( void ){
	// ループの終了
	if( m_pButtonExit->isTapped() ){
		CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_EXIT, 0, 0 );
		return;
	}

	int category = m_pMenuCategory->getSelect();
	CFont* pFont = CFontMgr::GetFont( (eFONT)category );

	if( isStepChanged() ){
		exitNodeAll();

		switch( m_nStep ){
		case eSTEP_SELECT_CATEGORY:
			m_pButtonExit->enter();
			m_pButtonExit->setCheckBackButton( true );

			m_pMenuCategory->enter();
			break;

		case eSTEP_SELECT_TARGET:
			m_pButtonExit->enter();
			m_pButtonBack->enter();
			m_pButtonBack->setCheckBackButton( true );

			// カテゴリメニュー（※非動作で表示）
			m_pMenuCategory->enter();
			m_pMenuCategory->setDark( true );

			// 文字メニュー
			m_pMenuTarget->setItemArrayThenFix( pFont->getCharLabelArray() );
			m_pMenuTarget->enter();
			break;
		}

		fixStep();
	}else{
		switch( m_nStep ){
		case eSTEP_SELECT_CATEGORY:
			if( m_pMenuCategory->isDecided() ){
				setStep( eSTEP_SELECT_TARGET );
			}
			break;

		case eSTEP_SELECT_TARGET:
			if( m_pMenuTarget->isDecided() ){
			}else if( m_pButtonBack->isTapped() ){
				setStep( eSTEP_SELECT_CATEGORY );
			}
			break;
		}

		if( isStepChanged() ){
			setActiveNodeAll( false );
		}
	}
    
    // ジェスチャ更新
    checkGestureScale();
    checkGestureMove();
}

//------------------------
// 描画
//------------------------
void CFontCheckLoop::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	eFONT font = (eFONT)m_pMenuCategory->getSelect();
	int target = m_pMenuTarget->getSelect();
    
    float baseX = m_fGestureMoveX;
    float baseY = m_fGestureMoveY;
    float scale = m_fGestureScale/CHAR_BASE_SCALE;

    // ジェスチャ領域表示
    drawGestureBase( 0xFFFFFF40 );
    
    // 情報
    char* buf = CMemMgr::GetBufFromTempStr();
    
    CDrawCell::SetLogAlignRB();
    sprintf( buf, "X=%.1f Y=%.1f SCALE=%.2f", baseX, baseY, scale );
    CDrawCell::DrawLogAlign( buf );

	// カテゴリ（テキスト）
	if( m_nStep0 == eSTEP_SELECT_CATEGORY ){
		const char* pCheckStr = "";
		buf = CMemMgr::GetBufFromTempStr();

		switch( font ){
		case eFONT_DEBUG:
			pCheckStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789!?@+-=*[]{}<>().,:;/_% $\nDebug, font. 1+2*3/4=5%";
			sprintf( buf, "%s\n%c100%% %c66%% %c33%% %c0%% %c12:34", pCheckStr, (char)17, (char)18, (char)19, (char)20, (char)22 );
			pCheckStr = buf;
			break;

		default:
			break;
		}

		pDC->clear();
		pDC->setStr( font, pCheckStr );
		pDC->setScaleXY( scale, scale );
        pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
		pDC->setRGBA( 0xFFFFFFFF );
		pDC->draw( baseX, baseY );
	}

	// キャラクタ単体
	if( m_nStep0 == eSTEP_SELECT_TARGET ){
		CFont* pFont = CFontMgr::GetFont( font );
		CSpr* pSpr = pFont->getSprCharAt( target );
		float lineH = pFont->getLineHeight();
		float lineY = pFont->getLineBaseY();

		// 文字情報
		int code = pSpr->getCharCode();
		float ofsX = pSpr->getCharOfsX();
		float ofsY = pSpr->getCharOfsY();
		float advX = pSpr->getCharAdvX();
		float sprW = pSpr->getWidth();

		// サイズの取得
		pDC->clear();
		pDC->setSpr( pSpr );
		pDC->setScaleXY( scale, scale );
		float x = baseX + ofsX*scale;
		float y = baseY + ofsY*scale;
		float w = pDC->getWidth();
		float h = pDC->getHeight();

		// ラインエリアの表示
		pDC->clear();
		pDC->setRect( SA_W, lineH*scale );
		pDC->setRGBA( 0X99CC9980 );
		pDC->setBlendAlpha();
		pDC->draw( 0, baseY );

		// 文字の表示
		pDC->clear();
		pDC->setSpr( pSpr );
		pDC->setDrawSetType( pFont->getDrawSetType() );
		pDC->setScaleXY( scale, scale );
		pDC->draw( x, y );

		// UV枠の表示
		pDC->clear();
		pDC->setRect( w+2*scale, scale );
		pDC->setBlendAlpha();
		pDC->setRGBA( 0xFF000080 );
		pDC->draw( x-scale, y-scale );
		pDC->draw( x-scale, y+h );

		pDC->clear();
		pDC->setRect( scale, h );
		pDC->setBlendAlpha();
		pDC->setRGBA( 0xFF000080 );
		pDC->draw( x-scale, y );
		pDC->draw( x+w, y );

		// 次の文字の開始位置
		pDC->clear();
		pDC->setRect( scale, SA_H );
		pDC->setBlendAlpha();
		pDC->setRGBA( 0X00008040 );
		pDC->draw( baseX+advX*scale, 0 );

		// ラインベースの表示
		pDC->clear();
		pDC->setRect( SA_W, scale );
		pDC->setBlendAlpha();
		pDC->setRGBA( 0X00800040 );
		pDC->draw( 0, baseY+lineY*scale );
        
        // 情報
        if( target == 0 || code != 0 ){
            sprintf( buf, "oX=%.0f oY=%.0f aX=%.0f, w=%.0f(%.0f)", ofsX, ofsY, advX, sprW, (ofsX+sprW)-advX );
            CDrawCell::DrawLogAlign( buf );

            sprintf( buf, "CODE:%d(0x%X)", code, code );
            CDrawCell::DrawLogAlign( buf );
        }else{
            sprintf( buf, "CODE:INVALID" );
            CDrawCell::DrawLogAlign( buf );
        }
    }
}
