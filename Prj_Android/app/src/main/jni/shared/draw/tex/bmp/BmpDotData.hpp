/*+----------------------------------------------------------------+
  |	Title:		BmpDotData.hpp [共通環境]
  |	Comment:	BMPの画素素材データ：フォームに対応したデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_DATA_HPP__
#define __BMP_DOT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "BmpDotPartData.hpp"
#include "BmpDotOrderData.hpp"

#include "sys/data/DataList.hpp"
#include "sys/data/DataCell.hpp"
#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//---------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//---------------------------------------
enum eBDD_FLAG{
    eBDD_FLAG_INVALID = -1,             // 無効値
    
    eBDD_FLAG_DISABLE,                  // 00:[*] 無効
    eBDD_FLAG_LOCKED,                   // 01:[!] ロック（※編集不可）

    eBDD_FLAG_MAX,                      // 最大（※フラグ変数は[short]なので[00〜15]まで）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMP画素データパック
//----------------------------------------
class CBmpDotData: public CDataList, public CListNode{
    // 各種実装
    IMPLEMENT_NODE( CBmpDotData )                                   // 実装：リストノード
    IMPLEMENT_DATA( CBmpDotData )                                   // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eBDD_FLAG )                                // 実装：フラグ操作
    IMPLEMENT_DATA_EDIT()                                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CBmpDotData )                  // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CBmpDotPartData )   // 実装：インスタンス確保／解放込みのリスト操作

public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    //int  m_nFlag;                 // [2]:フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;            // [X]:データリスト：CBmpDotPartData
    
    eBD_FORM m_eForm;               // [enum]:区分：eBD_FORM_XXX

public:
    // コンストラクタ／デストラクタ
    CBmpDotData( void );
    virtual ~CBmpDotData( void );

    // 設定処理は無し（※値の操作はエディタで行う）

    // 取得
    inline eBD_FORM getForm( void ){ return( m_eForm ); }

    // 判定
    bool isEqual( eBD_FORM form );
    
    //----------------------------------------------------------------------
    // データ検索（※なければ[NULL]が返る）
    //----------------------------------------------------------------------
    CBmpDotPartData* searchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParamr );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_DOT_DATA_HPP__
