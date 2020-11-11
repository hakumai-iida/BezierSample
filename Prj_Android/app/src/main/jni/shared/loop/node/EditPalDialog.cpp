/*+----------------------------------------------------------------+
  |	Title:		EditPalDialog.hpp [共通環境]
  |	Comment:	パレット編集ダイアログ（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "EditPalDialog.hpp"
#include "StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ボタン数（※４色×３桁）
#define EP_INC_DEC_BUTTON_NUM       (4*3)

// 基本サイズ
#define EP_DIALOG_W				    545
#define EP_DIALOG_H				    100
#define EP_DIALOG_DEGREE_MAX	    8

// 色
#define EP_DIALOG_RGBA			    0xFFFFFF80
#define EP_DIALOG_RGBA_DARK         0x80808080

#define EP_DIALOG_BORDER_RGBA	    0x000080FF
#define EP_DIALOG_BORDER_RGBA_DARK  0x000040FF

#define EP_DIALOG_BASE_RGBA		    0x00000040

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
//------------------
// コンストラクタ
//------------------
CEditPalDialog::CEditPalDialog( void ){
	// 各種ボタン
    m_pButtonRandL = new CButton( eGRP_DEBUG_BUTTON_S, "RnL" );
    m_pButtonRandS = new CButton( eGRP_DEBUG_BUTTON_S, "RnS" );
	m_pButtonUndo = new CButton( eGRP_DEBUG_BUTTON, "UNDO" );
	m_pButtonRedo = new CButton( eGRP_DEBUG_BUTTON, "REDO" );
    
	m_pButtonBright = new CButton( eGRP_DEBUG_BUTTON_L, "BRT" );
	m_pButtonDark = new CButton( eGRP_DEBUG_BUTTON_L, "DRK" );
	m_pButtonDark->setFlipX( true );
	m_pButtonCopy = new CButton( eGRP_DEBUG_BUTTON, "COPY" );
	m_pButtonPaste = new CButton( eGRP_DEBUG_BUTTON, "PASTE" );

    // 増減ボタン
    m_oArrButtonInc = new CButton[EP_INC_DEC_BUTTON_NUM];
    m_oArrButtonDec = new CButton[EP_INC_DEC_BUTTON_NUM];
    for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
        m_oArrButtonDec[i].setGrp( eGRP_DEBUG_BUTTON_M );
        m_oArrButtonInc[i].setGrp( eGRP_DEBUG_BUTTON_P );
    }

	reset();
}

//------------------
// デストラクタ
//------------------
CEditPalDialog::~CEditPalDialog( void ){
    SAFE_DELETE( m_pButtonRandL );
    SAFE_DELETE( m_pButtonRandS );
    SAFE_DELETE( m_pButtonUndo );
	SAFE_DELETE( m_pButtonRedo );

    SAFE_DELETE( m_pButtonBright );
	SAFE_DELETE( m_pButtonDark );
	
    SAFE_DELETE( m_pButtonCopy );
	SAFE_DELETE( m_pButtonPaste );
	
    SAFE_DELETE_ARRAY( m_oArrButtonInc );
	SAFE_DELETE_ARRAY( m_oArrButtonDec );
}

//------------------
// リセット
//------------------
void CEditPalDialog::reset( void ){
	m_pValueR = NULL;
    m_pValueG = NULL;
    m_pValueB = NULL;
    m_pValueA = NULL;
    
	m_nValueUndoR = 0;
    m_nValueUndoG = 0;
    m_nValueUndoB = 0;
    m_nValueUndoA = 0;
    
	m_nValueRedoR = 0;
    m_nValueRedoG = 0;
    m_nValueRedoB = 0;
    m_nValueRedoA = 0;

	// ペースト値は持ち越す
	//m_nValuePasteR = 0;
    //m_nValuePasteG = 0;
    //m_nValuePasteB = 0;
    //m_nValuePasteA = 0;

	m_bChanged = false;
}

//------------------
// 設定
//------------------
void CEditPalDialog::set( BYTE* pValR, BYTE* pValG, BYTE* pValB, BYTE* pValA ){
	// 設定
	m_pValueR = pValR;
    m_pValueG = pValG;
    m_pValueB = pValB;
    m_pValueA = pValA;
    
    if( m_pValueR != NULL ){ m_nValueUndoR = *m_pValueR; }
    if( m_pValueG != NULL ){ m_nValueUndoG = *m_pValueG; }
    if( m_pValueB != NULL ){ m_nValueUndoB = *m_pValueB; }
    if( m_pValueA != NULL ){ m_nValueUndoA = *m_pValueA; }
    
    m_nValueRedoR = m_nValueUndoR;
    m_nValueRedoG = m_nValueUndoG;
    m_nValueRedoB = m_nValueUndoB;
    m_nValueRedoA = m_nValueUndoA;
    
    m_bChanged = false;
}

//--------------------
// ダイアログ確定
//--------------------
void CEditPalDialog::fixDialog( void ){
	put( m_fX, m_fY );
}

//--------------------
// ダーク設定
//--------------------
void CEditPalDialog::setDark( bool isDark ){
    CNode::setDark( isDark );
    
    m_pButtonRandL->setDark( isDark );
    m_pButtonRandS->setDark( isDark );
    m_pButtonUndo->setDark( isDark );
    m_pButtonRedo->setDark( isDark );

    m_pButtonBright->setDark( isDark );
    m_pButtonDark->setDark( isDark );
    m_pButtonCopy->setDark( isDark );
    m_pButtonPaste->setDark( isDark );
    
    for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
        m_oArrButtonInc[i].setDark( isDark );
        m_oArrButtonDec[i].setDark( isDark );
    }
}

//------------------
// 位置指定
//------------------
void CEditPalDialog::put( float x, float y ){
	CNode::put( x, y );
    
    float tempX, tempY;

    // 左寄せ
    tempX = x + 1 + 1;
    tempY = y + 1;
    m_pButtonRandL->put( tempX, tempY + 0*(32+1) );
    m_pButtonRandS->put( tempX + 1*(32+1)-1, tempY + 0*(32+1) );
	m_pButtonUndo->put( tempX, tempY + 1*(32+1) );
    m_pButtonRedo->put( tempX, tempY + 2*(32+1) );

    // 右寄せ
    tempX = x + EP_DIALOG_W - (64+4) + 2;
	m_pButtonBright->put( tempX, tempY );
	m_pButtonDark->put( tempX + 1*(32+1)-1, tempY );

    // 右寄せ
    tempX = x + EP_DIALOG_W - (64+4) + 2;
	m_pButtonCopy->put( tempX, tempY + 1*(32+1) );
	m_pButtonPaste->put( tempX, tempY + 2*(32+1) );

    // 左寄せ
    tempX = x + (64+4) + 1;
	for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
		m_oArrButtonDec[i].put( tempX, tempY );
		m_oArrButtonInc[i].put( tempX, tempY + 2*(32+1) );
        tempX += (32+1);
        if((i%3) == 2 ){
            tempX += 4;
        }
	}
}

//------------------
// 入場
//------------------
void CEditPalDialog::enter( void ){
	CNode::enter();

    m_pButtonRandL->enter();
    m_pButtonRandS->enter();
	m_pButtonUndo->enter();
	m_pButtonRedo->enter();

    m_pButtonBright->enter();
	m_pButtonDark->enter();
    m_pButtonCopy->enter();
	m_pButtonPaste->enter();

	for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
		m_oArrButtonInc[i].enter();
		m_oArrButtonDec[i].enter();
	}
    
    m_bChanged = false;
}

//------------------
// 退場
//------------------
void CEditPalDialog::exit( void ){
	CNode::exit();

    m_pButtonRandL->exit();
    m_pButtonRandS->exit();
	m_pButtonUndo->exit();
	m_pButtonRedo->exit();

    m_pButtonBright->exit();
	m_pButtonDark->exit();
    m_pButtonCopy->exit();
	m_pButtonPaste->exit();

	for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
		m_oArrButtonInc[i].exit();
		m_oArrButtonDec[i].exit();
	}
}

//------------------
// 活動指定
//------------------
void CEditPalDialog::setActive( bool flag ){
	CNode::setActive( flag );

    m_pButtonRandL->setActive( flag );
    m_pButtonRandS->setActive( flag );
	m_pButtonUndo->setActive( flag );
	m_pButtonRedo->setActive( flag );

    m_pButtonBright->setActive( flag );
	m_pButtonDark->setActive( flag );
    m_pButtonCopy->setActive( flag );
	m_pButtonPaste->setActive( flag );

	for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
		m_oArrButtonInc[i].setActive( flag );
		m_oArrButtonDec[i].setActive( flag );
	}
}

//------------------
// サイズ取得
//------------------
float CEditPalDialog::getW( void ){ return( EP_DIALOG_W ); }
float CEditPalDialog::getH( void ){ return( EP_DIALOG_H ); }

//------------------
// 更新
//------------------
void CEditPalDialog::onUpdate0( void ){
	m_bChanged = false;
    bool isUpdateRedoValue = false;

    // 要素の更新
    m_pButtonRandL->onUpdate();
    m_pButtonRandS->onUpdate();
    m_pButtonUndo->onUpdate();
    m_pButtonRedo->onUpdate();
    m_pButtonBright->onUpdate();
    m_pButtonDark->onUpdate();
    m_pButtonCopy->onUpdate();
    m_pButtonPaste->onUpdate();
    for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
        m_oArrButtonInc[i].onUpdate();
    }

    //------------------
    // RandL
    //------------------
    if( m_pButtonRandL->isTapped() ){
        // ランダム（※変動幅大）
        BYTE tempR;
        BYTE tempG;
        BYTE tempB;
        CConst::AdjustColorRand( &tempR, &tempG, &tempB, 0 );   // ０以下を指定することで完全ランダム

        // 差分があれば最小値へ
        if( *m_pValueR != tempR || *m_pValueG != tempG || *m_pValueB != tempB ){
            *m_pValueR = (BYTE)tempR;
            *m_pValueG = (BYTE)tempG;
            *m_pValueB = (BYTE)tempB;
                        
            m_bChanged = true;
            isUpdateRedoValue = true;
        }
    }
    //------------------
    // RandS
    //------------------
    else if( m_pButtonRandS->isTapped() ){
        // ランダム（※変動幅小）
        BYTE tempR = *m_pValueR;
        BYTE tempG = *m_pValueG;
        BYTE tempB = *m_pValueB;
        CConst::AdjustColorRand( &tempR, &tempG, &tempB, 32+1 );    // 渡した値に対して[-32〜+32]の範囲でランダム補正

        // 差分があれば最小値へ
        if( *m_pValueR != tempR || *m_pValueG != tempG || *m_pValueB != tempB ){
            *m_pValueR = tempR;
            *m_pValueG = tempG;
            *m_pValueB = tempB;
                        
            m_bChanged = true;
            isUpdateRedoValue = true;
        }
    }
    //------------------
    // Undo
    //------------------
    else if( m_pButtonUndo->isTapped() ){
        // 差分があれば設定
        if( *m_pValueR != m_nValueUndoR || *m_pValueG != m_nValueUndoG || *m_pValueB != m_nValueUndoB || *m_pValueA != m_nValueUndoA ){
            *m_pValueR = m_nValueUndoR;
            *m_pValueG = m_nValueUndoG;
            *m_pValueB = m_nValueUndoB;
            *m_pValueA = m_nValueUndoA;
            
            m_bChanged = true;
        }
    }
    //------------------
    // Redo
    //------------------
    else if( m_pButtonRedo->isTapped() ){
        // 差分があれば設定
        if( *m_pValueR != m_nValueRedoR || *m_pValueG != m_nValueRedoG || *m_pValueB != m_nValueRedoB || *m_pValueA != m_nValueRedoA ){
            *m_pValueR = m_nValueRedoR;
            *m_pValueG = m_nValueRedoG;
            *m_pValueB = m_nValueRedoB;
            *m_pValueA = m_nValueRedoA;
            
            m_bChanged = true;
        }
    }
    //------------------
	// Bright
    //------------------
	else if( m_pButtonBright->isTapped() ){
        // 各色１０％明るくする
        BYTE tempR = *m_pValueR;
        BYTE tempG = *m_pValueG;
        BYTE tempB = *m_pValueB;
        CConst::AdjustColor( &tempR, &tempG, &tempB, false, 0.1f, 4 );

        // 差分があれば最小値へ
        if( *m_pValueR != tempR || *m_pValueG != tempG || *m_pValueB != tempB ){
            *m_pValueR = tempR;
            *m_pValueG = tempG;
            *m_pValueB = tempB;
                        
            m_bChanged = true;
            isUpdateRedoValue = true;
        }
	}
    //------------------
	// Dark
    //------------------
	else if( m_pButtonDark->isTapped() ){
        // 各色１０％暗くする
        BYTE tempR = *m_pValueR;
        BYTE tempG = *m_pValueG;
        BYTE tempB = *m_pValueB;
        CConst::AdjustColor( &tempR, &tempG, &tempB, true, 0.1f, 4 );

        // 差分があれば最大値へ
        if( *m_pValueR != tempR || *m_pValueG != tempG || *m_pValueB != tempB ){
            *m_pValueR = tempR;
            *m_pValueG = tempG;
            *m_pValueB = tempB;
            
            m_bChanged = true;
            isUpdateRedoValue = true;
        }
	}
    //------------------
	// Copy
    //------------------
	else if( m_pButtonCopy->isTapped() ){
        m_nValuePasteR = *m_pValueR;
        m_nValuePasteG = *m_pValueG;
        m_nValuePasteB = *m_pValueB;
        m_nValuePasteA = *m_pValueA;
	}
    //------------------
	// Paste
    //------------------
	else if( m_pButtonPaste->isTapped() ){
        // 差分があればペースト
        if( *m_pValueR != m_nValuePasteR || *m_pValueG != m_nValuePasteG || *m_pValueB != m_nValuePasteB || *m_pValueA != m_nValuePasteA ){
            *m_pValueR = m_nValuePasteR;
            *m_pValueG = m_nValuePasteG;
            *m_pValueB = m_nValuePasteB;
            *m_pValueA = m_nValuePasteA;
            
            m_bChanged = true;
            isUpdateRedoValue = true;
        }
	}
    //------------------
    // INC/DEC
    //------------------
    else{
        // 増減判定
        int addVal[EP_INC_DEC_BUTTON_NUM];
        memset( addVal, 0, sizeof(int)*EP_INC_DEC_BUTTON_NUM );

        // [Inc]ボタン確認
        for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
            if( m_oArrButtonInc[i].isTapped() ){
                addVal[i] += 1;
            }
        }

        // [Dec]ボタン確認
        for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
            m_oArrButtonDec[i].onUpdate();
            if( m_oArrButtonDec[i].isTapped() ){
                addVal[i] -= 1;
            }
        }
        
        // 変更の検出
        for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
            if( addVal[i] != 0 ){
                // 対象データの検出
                BYTE* pTarget;
                int targetVal = i/3;
                if( targetVal == 0 ){
                    pTarget = m_pValueR;
                }else if( targetVal == 1 ){
                    pTarget = m_pValueG;
                }else if( targetVal == 2 ){
                    pTarget = m_pValueB;
                }else{
                    pTarget = m_pValueA;
                }

                // 増減値の検出
                int baseVal;
                int targetDec = i%3;
                if( targetDec == 0 ){
                    baseVal = 100;
                }else if( targetDec == 1 ){
                    baseVal = 10;
                }else{
                    baseVal = 1;
                }
                
                // 増減
                int temp = *pTarget + baseVal*addVal[i];
                if( temp > 0xFF ){ temp = 0xFF; }
                else if( temp < 0 ){ temp = 0; }

                // 差分があれば値の反映
                if( *pTarget != temp ){
                    *pTarget = temp;

                    m_bChanged = true;
                    isUpdateRedoValue = true;
                    break;
                }
            }
        }
    }
    
    // 指定があれば[Redo]値の更新
    if( isUpdateRedoValue ){
        m_nValueRedoR = *m_pValueR;
        m_nValueRedoG = *m_pValueG;
        m_nValueRedoB = *m_pValueB;
        m_nValueRedoA = *m_pValueA;
    }
}

//------------------
// 描画
//------------------
void CEditPalDialog::onDraw0( void ){
	CDrawCell* pDC = CDrawCell::GetFreeCell();

    DWORD rgba;
    
    if( isDark() ){ rgba = EP_DIALOG_RGBA_DARK; }
    else{ rgba = EP_DIALOG_RGBA; }

	// 下地の表示
    pDC->clear();
	pDC->setRect( EP_DIALOG_W, EP_DIALOG_H );
	pDC->setRGBA( rgba );
	pDC->setBlendAlpha();
	pDC->draw( m_fX, m_fY );

    if( isDark() ){ rgba = EP_DIALOG_BORDER_RGBA_DARK; }
    else{ rgba = EP_DIALOG_BORDER_RGBA; }

	// 枠線の表示
	pDC->clear();
	pDC->setRect( EP_DIALOG_W+4, 2 );
	pDC->setRGBA( rgba );
	pDC->draw( m_fX-2, m_fY-2 );
	pDC->draw( m_fX-2, m_fY+EP_DIALOG_H );

	pDC->clear();
	pDC->setRect( 2, EP_DIALOG_H );
	pDC->setRGBA( rgba );
	pDC->draw( m_fX-2, m_fY );
	pDC->draw( m_fX+EP_DIALOG_W, m_fY );
    
    // 無効は無視
    if( m_pValueA == NULL || m_pValueG == NULL || m_pValueB == NULL || m_pValueA == NULL ){
        return;
    }

    if( isDark() ){ rgba = SM_ITEM_RGBA_DARK; }
    else{ rgba = SM_ITEM_RGBA; }

	// ボタン表示
    m_pButtonRandL->onDraw();
    m_pButtonRandS->onDraw();
	m_pButtonUndo->onDraw();
	m_pButtonRedo->onDraw();
	m_pButtonBright->onDraw();
	m_pButtonDark->onDraw();
	m_pButtonCopy->onDraw();
	m_pButtonPaste->onDraw();

	for( int i=0; i<EP_INC_DEC_BUTTON_NUM; i++ ){
		m_oArrButtonInc[i].onDraw();
		m_oArrButtonDec[i].onDraw();
	}

    float x = m_fX + (64+4) + 1;
    float y = m_fY + (32+2);
    for( int i=0; i<4; i++ ){
        int val;
        DWORD numRGBA;
        if( i == 0 ){
            val = *m_pValueR;
            numRGBA = CConst::CalcMultiForRGBA( rgba, 0xFFC0C0FF);
        }else if( i == 1 ){
            val = *m_pValueG;
            numRGBA = CConst::CalcMultiForRGBA( rgba, 0xC0FFC0FF);
        }else if( i == 2 ){
            val = *m_pValueB;
            numRGBA = CConst::CalcMultiForRGBA( rgba, 0xC0C0FFFF);
        }else{
            val = *m_pValueA;
            numRGBA = CConst::CalcMultiForRGBA( rgba, 0xFFFFFFFF);
        }
        
        int baseVal = 100;
        
        // 項目の表示
        for( int j=0; j<3; j++ ){
            pDC->clear();
            pDC->setRect( 32, 32 );
            pDC->setRGBA( EP_DIALOG_BASE_RGBA );
            pDC->setBlendAlpha();
            pDC->draw( x, y );
            
            // 数字
            pDC->clear();
            pDC->setSpr( (eSPR)(eSPR_DEBUG_NUMBER_00 + (val/baseVal)%10 ) );
            pDC->setScaleXY( 3.0f, 3.0f );
            pDC->setAlignX( eDRAW_ALIGN_X_C );
            pDC->setRGBA( numRGBA );
            pDC->draw( x + 16, y - 1 );
            
            x += 32 + 1;
            baseVal /= 10;
        }
        
        x += 4;
    }
}
