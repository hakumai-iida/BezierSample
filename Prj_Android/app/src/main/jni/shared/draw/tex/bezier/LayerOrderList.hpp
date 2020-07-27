/*+----------------------------------------------------------------+
  |	Title:		LayerOrderList.hpp [共通環境]
  |	Comment:	レイヤーオーダーリスト（※先に登録されたものほど手前の概念）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LAYER_ORDER_LIST_HPP__
#define __LAYER_ORDER_LIST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerOrderCell.hpp"

#include "sys/data/List.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//--------------------------
// レイヤーオーダーリスト
//--------------------------
class CLayerOrderList{
protected:
    // リスト
    CList m_oList;

    // 現在の優先順位
    int m_nCurPriority;
    
    // 特殊セル
    CLayerOrderCell m_oCellLyFront;     // 最前面に来るレイヤー
    CLayerOrderCell m_oCellLyBack;      // 最後面に来るレイヤー    

public:
    // コンストラクタ／デストラクタ
    CLayerOrderList( void );
    virtual ~CLayerOrderList( void );
    
    // クリア
    void clear( void );
    
    // 取得
    inline CList* getList( void ){ return( &m_oList ); }
    inline CLayerOrderCell* getOrderCellHead( void ){ return( (CLayerOrderCell*) m_oList.getHead() ); }
    inline int getOrderCellPriority( void ){ return( m_nCurPriority ); }

    // セルの取得／登録
    CLayerOrderCell* getOrderCell( eBD_SLOT slot, int slotIndex, bool isCreateIfNotFound=false );
    stLAYER_ORDER_CELL_NODE* addLayerData( CLayerData* pData, eLAYER_ORDER_TYPE type, eBD_SLOT slot, int slotIndex );

    //---------------------------------------------------------------------
    // 登録済みのセルを優先順位順（描画順）に取り出す
    //（※プライオリティの値は[０＝LyFront]、[getOrderCellPriority＝LyBack]となる）
    //---------------------------------------------------------------------
    CLayerOrderCell* getOrderCellByPriority( int priority );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LAYER_ORDER_LIST_HPP__
