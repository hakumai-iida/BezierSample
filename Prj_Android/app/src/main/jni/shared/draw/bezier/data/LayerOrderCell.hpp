/*+----------------------------------------------------------------+
  |	Title:		LayerOrderCell.hpp [共通環境]
  |	Comment:	レイヤーオーダーセル（※オーダー登録単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LAYER_ORDER_CELL_HPP__
#define __LAYER_ORDER_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "sys/data/ListNode.hpp"
#include "LayerData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// セル内リストのノード（※パラメータを表示データへ紐づける器）
//-------------------------------------------------
struct stLAYER_ORDER_CELL_NODE{
    CLayerData* pLayerData;             // 対象レイヤーデータ（※表示データ）
    stBEZIER_LAYER_PARAM layerParam;    // レイヤーデータに対するパラメータ
    eBD_LAYER_ORDER_GROUP group;        // オーダーグループ（※スーツ時の描画順調整値）
    
    // 処理済みか？（※循環参照回避用）
    bool isDone;
    
    // 1-way list
    stLAYER_ORDER_CELL_NODE* pNext;     // 次の要素
};

// クリアマクロは無し
// この構造体は[CLayerOrderCell::addNode]でのみ生成され、その際にはメンバーが適切に処理される想定

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// レイヤーオーダーセル
//----------------------------------------
class CLayerOrderCell: public CListNode{
    // 実装
    IMPLEMENT_NODE( CLayerOrderCell )           // 実装：リストノード
    
protected:
    // オーダー値（※このセルがどのスロットに対するものかを表す）
    eBD_SLOT m_nOrderSlot;                      // スロット
    int m_nOrderSlotIndex;                      // スロットインデックス
    
    // 各タイプのノードヘッダ
    stLAYER_ORDER_CELL_NODE* m_pArrNodeHead[eBD_LAYER_ORDER_TYPE_MAX];

public:
    // コンストラクタ／デストラクタ
    CLayerOrderCell( void );
    virtual ~CLayerOrderCell( void );
    
    // クリア
    void clear( void );

    // 設定
    void setOrder( eBD_SLOT slot, int slotIndex );

    // 取得
    inline eBD_SLOT getOrderSlot( void ){ return( m_nOrderSlot ); }
    inline int getOrderSlotIndex( void ){ return( m_nOrderSlotIndex ); }
    
    // 判定
    bool isEqual( eBD_SLOT slot, int slotIndex );

    // ヘッダ取得
    stLAYER_ORDER_CELL_NODE* getNodeHead( eBD_LAYER_ORDER_TYPE type );
    
    // データ追加（※先に登録されるほど先に描画される＝手前に表示される）
    stLAYER_ORDER_CELL_NODE* addNode( CLayerData* pData, eBD_LAYER_ORDER_TYPE type );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LAYER_ORDER_CELL_HPP__
