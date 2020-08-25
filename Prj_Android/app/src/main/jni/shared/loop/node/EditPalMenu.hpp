/*+----------------------------------------------------------------+
  |	Title:		EditPalMenu.hpp [共通環境]
  |	Comment:	パレット編集メニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __EDIT_PAL_MENU_HPP__
#define __EDIT_PAL_MENU_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ScrollBarMenu.hpp"
#include "EditPalDialog.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//--------------------------------------------
// メニュー項目（※表示用＆ダイアログ呼び出しパラメータ）
//--------------------------------------------
struct stEDIT_PAL_MENU_ITEM{
    // 表示名
	char    cArrName[COMMON_NAME_LEN];
    
    // 編集対象
	BYTE*	pValR;          // R
    BYTE*   pValG;          // G
    BYTE*   pValB;          // B
    BYTE*   pValA;          // A
    
    // 表示指定
    bool    bSeparator;     // 要素の下にセパレータ（横線）を表示するか？
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// パレット編集メニュー
//----------------------------------------
class CEditPalMenu: public CScrollBarMenu{
protected:
	// メンバー
	stEDIT_PAL_MENU_ITEM* m_stArrItem;
	int m_nValueOfsX;

public:
	CEditPalMenu( eGRP grpId, int cW, int cH );
	virtual ~CEditPalMenu( void );

	void releaseItem( void );

	virtual void setItemNum( int num, bool isBlockRequired=false );

    //--------------------------------------
    // アイテム設定
    //--------------------------------------
	bool setItemAt( int id, stEDIT_PAL_MENU_ITEM* pItem, bool isBlock=false );
	bool setItemAt( int id, const char* pName, BYTE* pValR, BYTE* pValG, BYTE* pValB, BYTE* pValA, bool isBlock=false );

    bool setSeparatorAt( int id, bool flag );

	// 取得
	stEDIT_PAL_MENU_ITEM* getItemAt( int at );
	stEDIT_PAL_MENU_ITEM* getSelectedItem( void );

    // ダイアログ設定
	void setEditPalDialog( CEditPalDialog* pDialog );

protected:
	virtual float calcInternalW( void );
	virtual float calcInternalH( void );
	virtual float getItemTapH( void );

	virtual void onDraw0( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __EDIT_PAL_MENU_HPP__ */
