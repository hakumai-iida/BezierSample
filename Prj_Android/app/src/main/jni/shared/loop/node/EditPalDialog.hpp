/*+----------------------------------------------------------------+
  |	Title:		EditPalDialog.hpp [共通環境]
  |	Comment:	パレット編集ダイアログ（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __EDIT_PAL_DIALOG_HPP__
#define __EDIT_PAL_DIALOG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Node.hpp"
#include "Button.hpp"

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
// パレット編集ダイアログ
//----------------------------------------
class CEditPalDialog: public CNode{
protected:
    CButton* m_pButtonRandL;
    CButton* m_pButtonRandS;
	CButton* m_pButtonUndo;
	CButton* m_pButtonRedo;
    
	CButton* m_pButtonBright;
	CButton* m_pButtonDark;
	CButton* m_pButtonCopy;
	CButton* m_pButtonPaste;
    
	CButton* m_oArrButtonInc;
	CButton* m_oArrButtonDec;

	// 対象
	BYTE* m_pValueR;
    BYTE* m_pValueG;
    BYTE* m_pValueB;
    BYTE* m_pValueA;

	// 各種値
    BYTE m_nValueUndoR;
    BYTE m_nValueUndoG;
    BYTE m_nValueUndoB;
    BYTE m_nValueUndoA;

	BYTE m_nValueRedoR;
    BYTE m_nValueRedoG;
    BYTE m_nValueRedoB;
    BYTE m_nValueRedoA;

    BYTE m_nValuePasteR;
    BYTE m_nValuePasteG;
    BYTE m_nValuePasteB;
    BYTE m_nValuePasteA;

	// 管理フラグ
	bool m_bChanged;

public:
	CEditPalDialog( void );
	virtual ~CEditPalDialog( void );

	void reset( void );
	void set( BYTE* pValR, BYTE* pValG, BYTE* pValB, BYTE* pValA );
    
	void fixDialog( void );
    
    virtual void setDark( bool isDark );

	virtual void put( float x, float y );
	virtual void enter( void );
	virtual void exit( void );
	virtual void setActive( bool flag );

	inline bool isChanged( void ){ return( m_bChanged ); }

	virtual float getW( void );
	virtual float getH( void );

protected:
	virtual void onUpdate0( void );
	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __EDIT_PAL_DIALOG_HPP__ */
