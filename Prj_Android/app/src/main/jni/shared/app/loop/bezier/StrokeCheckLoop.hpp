/*+----------------------------------------------------------------+
  |	Title:		StrokeCheckLoop.hpp [共通環境]
  |	Comment:	ストローク確認ループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __STROKE_CHECK_LOOP_HPP__
#define __STROKE_CHECK_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "loop/GestureLoop.hpp"
#include "loop/node/Button.hpp"
#include "loop/node/StrMenu.hpp"

#include "draw/bmp/BmpTex.hpp"

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
class CStrokeCheckLoop: public CGestureLoop{
protected:
    // 基本機能
	CButton*	m_pButtonExit;
	CButton*	m_pButtonBack;
    CButton*    m_pButtonUpdate;
    CButton*    m_pButtonDot;

    // 項目増減（※範囲）
    CButton*    m_pButtonRangeDec;
    CButton*    m_pButtonRangeInc;

    // メニュー
    CStrMenu*   m_pMenuStroke;
    CStrMenu*   m_pMenuSize;

    // ワーク
    CBmpTex* m_pBmpTex;
    CLayerData* m_pLayer;
    
    bool m_bDot;
    int m_nOfsRange1000;
    
    int m_nCalcNum;
    int m_nCalcTimeTotal;
    int m_nCalcTimeAvg;

public:
	CStrokeCheckLoop( void );
	virtual ~CStrokeCheckLoop( void );

protected:
	virtual void init0( void );
	virtual void exit0( void );
	virtual void update0( void );
	virtual void onDraw0( void );
    
    void allocForTest( void );
    void releaseForTest( void );
    
    void updateTex( eSTROKE_TYPE stroke, int sizeAt, int range1000, bool isDot, int num );

public:
    // 流用したいので外部に公開
    static bool CreateLayerDataForBrush( CLayerData** ppData, eBRUSH brush );
    static bool ReleaseLayerDataForBrush( CLayerData** ppData );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __STROKE_CHECK_LOOP_HPP__ */
