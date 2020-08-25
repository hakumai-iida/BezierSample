/*+----------------------------------------------------------------+
  | Title:      EditPalMenu.hpp [共通環境]
  | Comment:    パレット編集メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "EditPalMenu.hpp"
#include "StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 座標／サイズ
#define BASE_VALUE_AREA_W	144

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
//-------------------
// コンストラクタ
//-------------------
CEditPalMenu::CEditPalMenu( eGRP grpId, int cW, int cH ): CScrollBarMenu( grpId, cW, cH ){
}

//-------------------
// デストラクタ
//-------------------
CEditPalMenu::~CEditPalMenu( void ){
	releaseItem();
}

//-------------------
// 解放
//-------------------
void CEditPalMenu::releaseItem( void ){
	SAFE_DELETE_ARRAY( m_stArrItem );
}

//-------------------
// アイテム数設定
//-------------------
void CEditPalMenu::setItemNum( int num, bool isBlockRequired ){
	// 領域の解放
	releaseItem();
    releaseItemBlock();

	// メニュー項目数設定
	CScrollBarMenu::setItemNum( num );
    
    if( isBlockRequired ){
        setItemBlock();
    }

	// 枠の確保
	m_stArrItem = new stEDIT_PAL_MENU_ITEM[m_nItemNum];
}

//-------------------
// アイテム設定
//-------------------
bool CEditPalMenu::setItemAt( int id, stEDIT_PAL_MENU_ITEM* pItem, bool isBlock ){
	// 無効は無視
	if( id < 0 || id >= m_nItemNum ){ return( false ); }
	if( pItem == NULL  ){ return( false ); }

	// 丸毎コピー
	m_stArrItem[id] = *pItem;

    setItemBlockAt( id, isBlock );
    
	return( true );
}

//-------------------
// アイテム設定
//-------------------
bool CEditPalMenu::setItemAt( int id, const char* pName, BYTE* pValR, BYTE* pValG, BYTE* pValB, BYTE* pValA, bool isBlock ){
	// 無効は無視
	if( id < 0 || id >= m_nItemNum ){ return( false ); }

	// 対象のクリア（セパレーターフラグは前もって指定される想定なので退避＆復元させている）
	stEDIT_PAL_MENU_ITEM* pItem = &m_stArrItem[id];
    bool bSeparator = pItem->bSeparator;
	memset( pItem, 0, sizeof(stEDIT_PAL_MENU_ITEM) );
    pItem-> bSeparator = bSeparator;

	// 設定
    util_str_cpy( pItem->cArrName, pName, COMMON_NAME_LEN );
	pItem->pValR = pValR;
    pItem->pValG = pValG;
    pItem->pValB = pValB;
    pItem->pValA = pValA;

    setItemBlockAt( id, isBlock );

    return( true );
}

//-------------------
// セパレータの設定
//-------------------
bool CEditPalMenu::setSeparatorAt( int id, bool flag ){
    // 無効は無視
    if( id < 0 || id >= m_nItemNum ){ return( false ); }
    
    m_stArrItem[id].bSeparator = flag;
    return( true );
}


//-------------------
// 横幅の取得
//-------------------
stEDIT_PAL_MENU_ITEM* CEditPalMenu::getItemAt( int at ){
	if( at < 0 || at >= m_nItemNum ){
		return( NULL );
	}

	return( &m_stArrItem[at] );
}

//-------------------
// 選択対象の取得
//-------------------
stEDIT_PAL_MENU_ITEM* CEditPalMenu::getSelectedItem( void ){
	return( getItemAt( m_nSelect ) );
}

//------------------------
// パレット編集ダイアログの設定
//------------------------
void CEditPalMenu::setEditPalDialog( CEditPalDialog* pDialog ){
	stEDIT_PAL_MENU_ITEM* pItem = getSelectedItem();

    pDialog->set( pItem->pValR, pItem->pValG, pItem->pValB, pItem->pValA );
    
	pDialog->fixDialog();
}

//-------------------
// 内部横幅の取得
//-------------------
float CEditPalMenu::calcInternalW( void ){
    // ここではタイトル幅は計算にいれないでおく（※事前に値が確定しないため）
	m_nValueOfsX = m_nMaxCenterW - BASE_VALUE_AREA_W;
	return( m_nMaxCenterW );
}

//-------------------
// 縦幅の取得
//-------------------
float CEditPalMenu::calcInternalH( void ){
	return( m_nItemNum * SM_BASE_ITEM_STEP_Y - 1.0f );
}

//-------------------
// 項目の縦幅取得
//-------------------
float CEditPalMenu::getItemTapH( void ){
	return( SM_BASE_ITEM_STEP_Y );
}

//-------------------
// 描画
//-------------------
void CEditPalMenu::onDraw0( void ){
	CScrollBarMenu::onDraw0();

	// シザー（※無効は無視）
	if( ! CMainRenderer::SetScissor( m_fX+m_fCenterX, m_fY+m_fCenterY-1, m_fCenterW, m_fCenterH+2 ) ){
		return;
	}

	DWORD rgba;
    DWORD valRGBA;
    BYTE valR, valG, valB, valA;
	stEDIT_PAL_MENU_ITEM* pItem;
	char* buf = CMemMgr::GetBufFromTempStr();
	CDrawCell* pDC = CDrawCell::GetFreeCell();
	float x = m_fX + m_fCenterX - m_fInternalOfsX;
	float y = m_fY + m_fCenterY - m_fInternalOfsY;
    
	for( int i=0; i<m_nItemNum; i++ ){
		pItem = &m_stArrItem[i];

        // 表示色
        if( m_nSelect == i ){
            if( isDark() ){ rgba = SM_ITEM_SELECTED_RGBA_DARK; }
            else{ rgba = SM_ITEM_SELECTED_RGBA; }
        }else{
            if( isDark() ){ rgba = SM_ITEM_RGBA_DARK; }
            else{ rgba = SM_ITEM_RGBA; }
        }
        
        if( checkItemBlockedAt(i) ){
            rgba = CConst::CalcMultiForRGBA( rgba, SM_ITEM_BLOCKED_RGBA_RATE );
        }
        
        //----------------------------------
        // セパレータ
        //----------------------------------
        if( pItem->bSeparator ){
            pDC->clear();
            pDC->setRect( m_fCenterW, 2 );
            pDC->setRGBA( 0x40404080 );
            pDC->draw( x, y-2 );
        }

        //----------------------------------
		// 項目名表示：左寄せ
        //----------------------------------
		pDC->clear();
		pDC->setStr( eFONT_DEBUG, pItem->cArrName );
        pDC->setScaleXY( 1.0f, 2.0f );
        pDC->setRGBA( rgba );

		// 横幅が大きすぎる場合は横幅に収める
		if( pDC->getOriginalWidth() > (m_fCenterW-64) ){
			pDC->setWidth( m_fCenterW-64 );
		}

		pDC->draw( x, y-2 );
        
        //----------------------------------
		// 値のラベル設定
        //----------------------------------
        valR = *pItem->pValR;
        valG = *pItem->pValG;
        valB = *pItem->pValB;
        valA = *pItem->pValA;

        //----------------------------------
        // 下地
        //----------------------------------
        valRGBA = (valR << 24) | (valG << 16) | (valB << 8) | (valA << 0);

        pDC->clear();
        pDC->setRect( m_nValueOfsX-3, 18 );
        pDC->setRGBA( valRGBA );
        pDC->setAlignX( eDRAW_ALIGN_X_R );
        pDC->draw( x+m_nValueOfsX+BASE_VALUE_AREA_W-1, y+1 );

        //----------------------------------
        // 値表示：右寄せ
        //----------------------------------
        sprintf( buf, "%08X(%d,%d,%d,%d)", valRGBA, valR, valG, valB, valA );

        pDC->clear();
        pDC->setStr( eFONT_DEBUG, buf );
        pDC->setScaleXY( 1.0f, 2.0f );
        pDC->setRGBA( rgba );
        pDC->setAlignX( eDRAW_ALIGN_X_R );

        // 横幅が大きすぎる場合は横幅に収める
		if( pDC->getWidth() > m_nValueOfsX ){
			pDC->setWidth( m_nValueOfsX );
		}
        
		pDC->draw( x+m_nValueOfsX+BASE_VALUE_AREA_W-4, y-2 );

		y += SM_BASE_ITEM_STEP_Y;
	}

	CMainRenderer::SetScissorOff();
}
