/*+----------------------------------------------------------------+
  |	Title:		DrawCheckLoop.hpp [共通環境]
  |	Comment:	描画確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "DrawCheckLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum eSTEP{
	eSTEP_SELECT_CATEGORY,
	eSTEP_SELECT_TARGET,
};

// カテゴリ
enum eMENU_CATEGORY{
    eMENU_CATEGORY_IMG,
    eMENU_CATEGORY_SPR,
    eMENU_CATEGORY_GRP,
    eMENU_CATEGORY_NUM,
};

// 画像位置
#define IMG_BASE_X		SA_XC
#define IMG_BASE_Y		SA_YM
#define IMG_DRAG_R		128
#define IMG_BASE_SCALE	CMainRenderer::GetGamePixelRate()

// ログ位置（左上）
#define LOG_OFS_X		128
#define LOG_OFS_Y		32
#define LOG_SCALE		1.0f

// 情報位置（右下）
#define INFO_OFS_X     (-4)
#define INFO_OFS_Y     (-32)
#define INFO_SCALE      1.0f

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
static const char* _pArrMenu[] = {
	"Img",
	"Spr",
	"Grp",
    "Num",

	NULL
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CDrawCheckLoop::CDrawCheckLoop( void ){
	setId( eLOOP_ID_DRAW_CHECK );

    setDrawLayer( eDP_LAYER_LOOP );
}

//------------------------
// デストラクタ
//------------------------
CDrawCheckLoop::~CDrawCheckLoop( void ){
}

//------------------------
// 初期化
//------------------------
void CDrawCheckLoop::init0( void ){
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
	m_pMenuCategory = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 96 );
	m_pMenuCategory->setTitle( "Category Menu");
	m_pMenuCategory->setItemArrayThenFix( _pArrMenu );
	putNodeOnLayout( m_pMenuCategory );

	// メニュー：ターゲット
	m_pMenuTarget = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 168 );
	putNodeOnLayout( m_pMenuTarget );
    
    // レイアウト：右下
    setLayoutRB( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );
   
    // 白パレットボタン
    m_pButtonPalW = new CButton( eGRP_DEBUG_BUTTON, "PAL WHT" );
    putNodeOnLayout( m_pButtonPalW );
    
    // ブレンドボタン
    m_pButtonBlend = new CButton( eGRP_DEBUG_BUTTON, "BLEND" );
    putNodeOnLayout( m_pButtonBlend );

    // フレーム：増加
    m_pButtonFrameInc = new CButton( eGRP_DEBUG_BUTTON_L, "FRM" );
    putNodeOnLayout( m_pButtonFrameInc );
    m_pButtonFrameInc->setFlipX( true );
    
    // フレーム：減少
    m_pButtonFrameDec = new CButton( eGRP_DEBUG_BUTTON_L, "FRM" );
    putNodeOnLayout( m_pButtonFrameDec );
    
	// Node登録
    addNode( m_pButtonFrameDec );
    addNode( m_pButtonFrameInc );
    addNode( m_pButtonPalW );
    addNode( m_pButtonBlend );

	addNode( m_pMenuTarget );
	addNode( m_pMenuCategory );
	addNode( m_pButtonBack );
	addNode( m_pButtonExit );

	// ループ開始
	setStep( eSTEP_SELECT_CATEGORY );

	// ジェスチャー設定
	setGestureBase( IMG_BASE_X, IMG_BASE_Y, IMG_DRAG_R );
	setGestureMoveXY( IMG_BASE_X, IMG_BASE_Y );
	setGestureScale( IMG_BASE_SCALE );
    
    m_nCurBlend = eDRAW_BLEND_ALPHA;
    m_bUsePalW  = false;
    
    m_pImg = NULL;
    m_nMaxDot = m_nCurDot = 0;
    m_nMaxFrame = m_nCurFrame = 0;
    m_bAnimStop = false;
}

//------------------------
// 終了
//------------------------
void CDrawCheckLoop::exit0( void ){
}

//------------------------
// 更新
//------------------------
void CDrawCheckLoop::update0( void ){
	// ループの終了
	if( m_pButtonExit->isTapped() ){
		CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_EXIT, 0, 0 );
		return;
	}
    
	int category = m_pMenuCategory->getSelect();

	if( isStepChanged() ){
		exitNodeAll();

		// 常時表示
		m_pButtonExit->enter();
		m_pMenuCategory->enter();

		switch( m_nStep ){
		case eSTEP_SELECT_CATEGORY:
			m_pButtonExit->setCheckBackButton( true );
                
            setCurImg( eIMG_INVALID );
			break;

		case eSTEP_SELECT_TARGET:
			m_pButtonBack->enter();
			m_pButtonBack->setCheckBackButton( true );

			// カテゴリメニュー（※非動作で表示）
			m_pMenuCategory->setDark( true );

			// イメージメニュー
			if( category == eMENU_CATEGORY_IMG ){
				m_pMenuTarget->setTitle( "Img Menu" );
				m_pMenuTarget->setItemArrayThenFix( g_pArrLabelImg );
                
                m_pButtonFrameInc->enter();
                m_pButtonFrameDec->enter();
                m_pButtonPalW->enter();
                m_pButtonBlend->enter();
                
                setCurImg( m_pMenuTarget->getSelect() );
			}
			// スプライトメニュー
			else if( category == eMENU_CATEGORY_SPR ){
				m_pMenuTarget->setTitle( "Spr Menu" );
				m_pMenuTarget->setItemArrayThenFix( g_pArrLabelSpr );
			}
            // グループメニュー
            else if( category == eMENU_CATEGORY_GRP ){
                m_pMenuTarget->setTitle( "Grp Menu" );
                m_pMenuTarget->setItemArrayThenFix( g_pArrLabelGrp );
            }
			// 数字メニュー
            else/* if( category == eMENU_CATEGORY_NUM )*/{
				m_pMenuTarget->setTitle( "Num Menu" );
				m_pMenuTarget->setItemArrayThenFix( g_pArrLabelNum );
			}

			m_pMenuTarget->enter();
			break;
		}

		fixStep();
	}else{
        int addDot = 0;

		switch( m_nStep ){
		case eSTEP_SELECT_CATEGORY:
			if( m_pMenuCategory->isDecided() ){
				setStep( eSTEP_SELECT_TARGET );
			}
			break;

		case eSTEP_SELECT_TARGET:
            if( m_pButtonBack->isTapped() ){
                setStep( eSTEP_SELECT_CATEGORY );
            }else if( m_pMenuTarget->isDecided() ){
                if( category == 0 ){
                    m_bAnimStop = !m_bAnimStop;
                    if( !m_bAnimStop ){
                        m_nCurDot = 0;
                        m_nCurFrame = 0;
                    }
                }
            }else if( m_pMenuTarget->isSelectChanged() ){
                if( category == eMENU_CATEGORY_IMG ){
                    setCurImg( m_pMenuTarget->getSelect() );
                    m_bAnimStop = false;
                }
            }else if( m_pButtonPalW->isTapped() ){
                m_bUsePalW = !m_bUsePalW;
            }else if( m_pButtonBlend->isTapped() ){
                m_nCurBlend++;
                if( m_nCurBlend >= eDRAW_BLEND_MAX ){
                    m_nCurBlend = eDRAW_BLEND_OFF;
                }
            }else if( m_pButtonFrameInc->isTapped() ){
                addDot = +1;
            }else if( m_pButtonFrameDec->isTapped() ){
                addDot = -1;
            }
			break;
		}

		if( isStepChanged() ){
			setActiveNodeAll( false );
		}
        
        // 画像が有効なら
        if( m_pImg != NULL ){
            // アニメ再生中
            if( !m_bAnimStop ){
                m_nCurFrame++;
                
                if( m_nCurFrame >= m_nMaxFrame ){
                    m_nCurDot++;
                    if( m_nCurDot >= m_nMaxDot ){
                        m_nCurDot = 0;
                    }
                    m_nMaxFrame = m_pImg->getDotDelayAsFrame( m_nCurDot );
                    m_nCurFrame = 0;
                }
            }
            // アニメ停止中
            else{
                if( addDot != 0 ){
                    m_nCurDot += addDot;
                    
                    if( m_nCurDot < 0 ){ m_nCurDot = m_nMaxDot - 1; }
                    else if( m_nCurDot >= m_nMaxDot ){ m_nCurDot = 0; }
                    
                    m_nMaxFrame = m_pImg->getDotDelayAsFrame( m_nCurDot );
                    m_nCurFrame = 0;
                }
            }
        }
	}

	// ジェスチャ更新
    checkGestureScale();
	checkGestureMove();
}

//----------------------------
// 対象の画像を選択
//----------------------------
void CDrawCheckLoop::setCurImg( int target ){
    m_pImg = CImgMgr::GetImg( (eIMG)target );
    
    if( m_pImg != NULL ){
        m_nMaxDot = m_pImg->getNumDot();
        m_nMaxFrame = m_pImg->getDotDelay( m_nCurDot );
    }
    m_nCurDot = 0;
    m_nCurFrame = 0;
}

//------------------------
// 描画
//------------------------
void CDrawCheckLoop::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	char* buf = CMemMgr::GetBufFromTempStr();
	float drawX = m_fGestureMoveX;
	float drawY = m_fGestureMoveY;
	float scale = m_fGestureScale / IMG_BASE_SCALE;

	// 対象選択モードであれば
	if( m_nStep0 == eSTEP_SELECT_TARGET ){
		int category = m_pMenuCategory->getSelect();
		int target = m_pMenuTarget->getSelect();
        float x, y, w, h;

		// ジェスチャ領域表示
		drawGestureBase( 0xFFFFFF40 );

        // 画像表示
		if( category == eMENU_CATEGORY_IMG ){
			CImg* pImg = CImgMgr::GetImg( (eIMG)target );
            CTex* pTex = pImg->getDot( (m_nCurDot<pImg->getNumDot())? m_nCurDot: 0 );
            CTex* pTexP = pImg->getPal();
            bool isAnimGif = (pImg->getNumDot() > 1);

            if( pTexP != NULL && m_bUsePalW ){
                pTexP = CImgMgr::GetImgPal( eIMG_PAL_WHITE );
            }

			// フレームバッファでなく有効なテクスチャ／パレットであれば
			pDC->clear();
			pDC->setTex( pTex, pTexP );
			pDC->setScaleXY( scale, scale );
			w = pDC->getWidth();
			h = pDC->getHeight();
			x = drawX - w/2;
			y = drawY - h/2;
            pDC->setBlend( (eDRAW_BLEND)m_nCurBlend );
			pDC->draw( x, y );
            
            // 外枠の表示
            pDC->clear();
            pDC->setRect( w+4, 2 );
            pDC->setRGBA( (isAnimGif&&!m_bAnimStop)? 0xFF0000FF: 0x0000FFFF );
            pDC->draw( x-2, y-2 );
            pDC->draw( x-2, y+h );
            
            pDC->clear();
            pDC->setRect( 2, h );
            pDC->setRGBA( (isAnimGif&&!m_bAnimStop)? 0xFF0000FF: 0x0000FFFF );
            pDC->draw( x-2, y );
            pDC->draw( x+w, y );
            
            // 内枠の表示
            if( pTex->getOfsL() != 0 || pTex->getOfsT() != 0 || pTex->getOfsR() != 0 || pTex->getOfsB() != 0 ){
                x += scale * pTex->getOfsL();
                y += scale * pTex->getOfsT();
                w = scale * pTex->getWidth();
                h = scale * pTex->getHeight();
            
                pDC->clear();
                pDC->setRect( w+2, 1 );
                pDC->setRGBA( (isAnimGif)? 0x00FF00FF: 0x008000FF );
                pDC->draw( x-1, y-1 );
                pDC->draw( x-1, y+h );
                
                pDC->clear();
                pDC->setRect( 1, h );
                pDC->setRGBA( (isAnimGif)? 0x00FF00FF: 0x008000FF );
                pDC->draw( x-1, y );
                pDC->draw( x+w, y );
            }

            // 左上
            CDrawCell::SetLogAlignLT( LOG_SCALE, LOG_OFS_X, LOG_OFS_Y );
            sprintf( buf, "X=%.1f Y=%.1f SCALE=%.2f", drawX, drawY, scale );
            CDrawCell::DrawLogAlign( buf );
            
            sprintf( buf, "IMG(%d): W=%d H=%d", target, (int)pTex->getWidth(), (int)pTex->getHeight() );
            CDrawCell::DrawLogAlign( buf );

			if( pTexP != NULL ){
				sprintf( buf, "PAL(%d): W=%d H=%d", target, (int)pTexP->getWidth(), (int)pTexP->getHeight() );
			}else{
				sprintf( buf, "PAL(%d): NULL", target );
			}
            CDrawCell::DrawLogAlign( buf, (pTexP!=NULL)? 0xFFFFFFFF: 0x808080FF );

            // 右下（描画順は下から上）
            CDrawCell::SetLogAlignRB( INFO_SCALE, INFO_OFS_X, INFO_OFS_Y );

            sprintf( buf, "CNT:%02d/%02d",m_nCurFrame, m_nMaxFrame );
            CDrawCell::DrawLogAlign( buf, (isAnimGif)? 0xFFFFFFFF: 0x808080FF );

            sprintf( buf, "FRM:%02d/%02d", m_nCurDot, m_nMaxDot );
            CDrawCell::DrawLogAlign( buf, (isAnimGif)? 0xFFFFFFFF: 0x808080FF );

            if( isAnimGif ){
                if( m_bAnimStop ){
                    sprintf( buf, "ANIM:STOP" );
                }else{
                    sprintf( buf, "ANIM:PLAY" );
                }
            }else{
                sprintf( buf, "ANIM:---" );
            }
            CDrawCell::DrawLogAlign( buf, (isAnimGif)? 0xFFFFFFFF: 0x808080FF );

            const char* pBlendMode = "OFF";
            switch( m_nCurBlend ){
            case eDRAW_BLEND_ALPHA:      pBlendMode = "ALPHA";    break;
            case eDRAW_BLEND_ALPHA_ONE:  pBlendMode = "ALP ONE";  break;
            case eDRAW_BLEND_ADD:        pBlendMode = "ADD";      break;
            case eDRAW_BLEND_MULTI:      pBlendMode = "MULTI";    break;
            }
            
            sprintf( buf, "BLEND: %s", pBlendMode );
            CDrawCell::DrawLogAlign( buf );
		}
		// スプライト表示
		else if( category == eMENU_CATEGORY_SPR ){
			CSpr* pSpr = CSprMgr::GetSpr( (eSPR)target );

			pDC->clear();
			pDC->setSpr( pSpr );
			pDC->setScaleXY( scale, scale );
			w = pDC->getWidth();
			h = pDC->getHeight();
			x = drawX-w/2;
			y = drawY-h/2;
			pDC->setBlendAlpha();
			pDC->draw( x, y );

			// 枠の表示
			pDC->clear();
			pDC->setRect( w+4, 2 );
			pDC->setRGBA( 0xFF0000FF );
			pDC->draw( x-2, y-2 );
			pDC->draw( x-2, y+h );

			pDC->clear();
			pDC->setRect( 2, h );
			pDC->setRGBA( 0xFF0000FF );
			pDC->draw( x-2, y );
			pDC->draw( x+w, y );

            // 左上
            CDrawCell::SetLogAlignLT( LOG_SCALE, LOG_OFS_X, LOG_OFS_Y );
            sprintf( buf, "X=%.1f Y=%.1f SCALE=%.2f", drawX, drawY, scale );
            CDrawCell::DrawLogAlign( buf );

            sprintf( buf, "SPR(%d): W=%d H=%d", target, (int)pSpr->getWidth(), (int)pSpr->getHeight() );
            CDrawCell::DrawLogAlign( buf );
        }
		// グループ設定
		else if( category == eMENU_CATEGORY_GRP ){
            CGrp *pGrp = CGrpMgr::GetGrp( (eGRP)target );
            int num = pGrp->getSprNum();

            float totalW = 8 * (num - 1);
            float totalH = 0;
            for( int i=0; i<num; i++ ){
                CSpr* pSpr = pGrp->getSprAt( i );
                totalW += pSpr->getWidth() * scale;
                if( totalH < (pSpr->getHeight() * scale) ){
                    totalH = pSpr->getHeight() * scale;
                }
            }

            x = drawX - totalW / 2;
            y = drawY - totalH / 2;

            for (int i = 0; i < num; i++) {
                pDC->clear();
                pDC->setSpr( pGrp->getSprAt( i ) );
                pDC->setScaleXY(scale, scale);
                pDC->setBlendAlpha();
                w = pDC->getWidth();
                h = pDC->getHeight();
                pDC->draw(x, y);

                // 枠の表示
                pDC->clear();
                pDC->setRect(w + 4, 2);
                pDC->setRGBA(0x00FF00FF);
                pDC->draw(x - 2, y - 2);
                pDC->draw(x - 2, y + h);

                pDC->clear();
                pDC->setRect(2, h);
                pDC->setRGBA(0x00FF00FF);
                pDC->draw(x - 2, y);
                pDC->draw(x + w, y);

                x += w + 8;
            }

            // 左上
            CDrawCell::SetLogAlignLT( LOG_SCALE, LOG_OFS_X, LOG_OFS_Y );
            sprintf( buf, "X=%.1f Y=%.1f SCALE=%.2f", drawX, drawY, scale );
            CDrawCell::DrawLogAlign( buf );

            sprintf( buf, "GRP(%d): NUM=%d", target, num );
			CDrawCell::DrawLogAlign( buf );
		}
        // 数字設定
        else/* if( category == eMENU_CATEGORY_NUM )*/{
            pDC->clear();
            pDC->setNum( (eNUM)target, 123456789 );
            pDC->setNumWithPlus( true );
            pDC->setScaleXY(scale, scale);
            pDC->setBlendAlpha();
            x = drawX;
            y = drawY;
            pDC->draw(x, y);
            
            pDC->clear();
            pDC->setNum( (eNUM)target, -987654321 );
            pDC->setNumWithPlus( true );
            pDC->setScaleXY(scale, scale);
            pDC->setBlendAlpha();
            x = drawX;
            y = drawY + 32;
            pDC->draw(x, y);
        }
	}
}
