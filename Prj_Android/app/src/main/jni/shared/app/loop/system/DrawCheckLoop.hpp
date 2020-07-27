/*+----------------------------------------------------------------+
  |	Title:		DrawCheckLoop.hpp [共通環境]
  |	Comment:	描画確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DRAW_CHECK_LOOP_HPP__
#define __DRAW_CHECK_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "loop/GestureLoop.hpp"
#include "loop/node/Button.hpp"
#include "loop/node/StrMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 描画確認ループ
//----------------------------------------
class CDrawCheckLoop: public CGestureLoop{
protected:
	CButton*	m_pButtonExit;
	CButton*	m_pButtonBack;
	CStrMenu*	m_pMenuCategory;
	CStrMenu*	m_pMenuTarget;

    CButton*    m_pButtonFrameDec;
    CButton*    m_pButtonFrameInc;
    CButton*    m_pButtonPalW;
    CButton*    m_pButtonBlend;

    int  m_nCurBlend;
    bool m_bUsePalW;

    CImg* m_pImg;
    int  m_nMaxDot, m_nCurDot;
    int  m_nMaxFrame, m_nCurFrame;
    bool m_bAnimStop;

public:
	CDrawCheckLoop( void );
	virtual ~CDrawCheckLoop( void );

protected:
	virtual void init0( void );
	virtual void exit0( void );
	virtual void update0( void );
	virtual void onDraw0( void );
    
    void setCurImg( int target );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __DRAW_CHECK_LOOP_HPP__ */
