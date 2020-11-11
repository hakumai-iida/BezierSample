/*+----------------------------------------------------------------+
  |	Title:		BmpDotOrderData.hpp [共通環境]
  |	Comment:	BMPのドットパーツ並びデータ（※並びはフォーム毎に指定する）
  |             実装の変遷により編集機能をもつが現状はデフォルト指定のみを利用
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_ORDER_DATA_HPP__
#define __BMP_DOT_ORDER_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 保持するデータ枠
#define BMP_DOT_ORDER_DATA_CELL_MAX     128

// 編集機能を有効にするか？
#define BMP_DOT_ORDER_EDIT_ENABLE

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
// 登録データ
struct stBMP_DOT_ORDER_DATA_CELL{
    eBD_SLOT    slot;           // [enum]:対象スロット
    int         slotIndex;      // [2]:スロットインデックス
};

// CLEAR
#define CLEAR_BMP_DOT_ORDER_CELL( _pCell )                      \
    memset( (_pCell), 0, sizeof(stBMP_DOT_ORDER_DATA_CELL) );   \
    (_pCell)->slot = eBD_SLOT_INVALID;                          \
    (_pCell)->slotIndex = BD_SLOT_INDEX_INVALID

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMPパーツ並びデータ
//----------------------------------------
class CBmpDotOrderData{
    // 各種実装
    IMPLEMENT_DATA( CBmpDotOrderData )      // 実装：データのクリア／コピー／読み書き

protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    stBMP_DOT_ORDER_DATA_CELL   m_stArrCell[BMP_DOT_ORDER_DATA_CELL_MAX];

#ifdef BMP_DOT_ORDER_EDIT_ENABLE
    //-----------------------------------------
    // メンバー [Work]
    //-----------------------------------------
    int m_nSelect;                              // 現在のデータ選択位置
    stBMP_DOT_ORDER_DATA_CELL m_stCellCoppied;  // コピペ用
#endif
    
public:
    // コンストラクタ／デストラクタ
    CBmpDotOrderData( void );
    virtual ~CBmpDotOrderData( void );

    // 取得
    stBMP_DOT_ORDER_DATA_CELL* getCellAt( int at );

#ifdef BMP_DOT_ORDER_EDIT_ENABLE
    stBMP_DOT_ORDER_DATA_CELL* getSelectedCell( void );

    // 選択関連
    int setSelect( int select );
    int getSelect( void );
    int addSelect( int add );

    //---------------------------------------------------------------
    // 操作（※対象は[setSelect]で指定しておく＆返値は操作後の選択インデックス）
    //---------------------------------------------------------------
    int clearCell( void );              // 選択セルのクリア
    int addCellSlot( int add );         // 選択セルのスロットの増減
    int addCellSlotIndex( int add );    // 選択セルのスロットインデックスの増減
    int copyCell( void );               // セルのコピー（※ペーストへ備える）
    int pasteCell( void );              // セルのペースト
    int moveCellUp( void );             // 選択セルを一つ前へ移動
    int moveCellDown( void );           // 選択セルを一つ後ろへ移動
    int slideCellBlockUp( void );       // 選択セルから手前までのブロックを全て１つ前へ移動（※０番要素が押し出される点に注意）
    int slideCellBlockDown( void );     // 選択セルから後ろまでのブロックを全て１つ後ろへ移動（※最終要素が押し出される点に注意）
#endif

    // 設定
    void setCellAt( int at, eBD_SLOT slot, int slotIndex );
    
    // デフォルト設定
    void setDefault( eBD_FORM form, bool isSuited=false );
    
protected:
    void setDefaultForFront( void );
    void setDefaultForFrontSuited( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_DOT_ORDER_DATA_HPP__
