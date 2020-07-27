/*+----------------------------------------------------------------+
  |	Title:		BrushCheckLoop.hpp [共通環境]
  |	Comment:	ブラシ確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BRUSH_CHECK_LOOP_HPP__
#define __BRUSH_CHECK_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "loop/GestureLoop.hpp"
#include "loop/node/Button.hpp"
#include "loop/node/StrMenu.hpp"
#include "loop/node/EditValueMenu.hpp"
#include "loop/node/EditValueDialog.hpp"

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
// ストローク確認ループ
//----------------------------------------
class CBrushCheckLoop: public CGestureLoop{
protected:
    // 基本機能
	CButton*	m_pButtonExit;
	CButton*	m_pButtonBack;
    CButton*    m_pButtonUpdate;
    CButton*    m_pButtonDot;

    // メニュー
    CStrMenu*   m_pMenuBrush;

    // メニュー：編集
    CEditValueMenu* m_pMenuEV;
    CEditValueDialog* m_pDialogEV;

    // ワーク
    CTex* m_pTex;
    CLayerData* m_pLayer;

    bool m_bDot;
    int m_nCalcNum;
    int m_nCalcTimeTotal;
    int m_nCalcTimeAvg;

public:
	CBrushCheckLoop( void );
	virtual ~CBrushCheckLoop( void );

protected:
	virtual void init0( void );
	virtual void exit0( void );
	virtual void update0( void );
	virtual void onDraw0( void );
    
    void allocForTest( void );
    void releaseForTest( void );
    
    void updateTex( eBRUSH brushId, bool isDot, int num );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BRUSH_CHECK_LOOP_HPP__ */
