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

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-------------------------------------------------
// レイヤーの並びタイプ（※実際のデータ出力時の重ね合わせ判定用）
//-------------------------------------------------
enum eLAYER_ORDER_TYPE{
    eLAYER_ORDER_TYPE_INVALID = -1,
    
    eLAYER_ORDER_TYPE_FRONT,        // 手前に配置する
    eLAYER_ORDER_TYPE_NORMAL,       // 通常の配置先
    eLAYER_ORDER_TYPE_BACK,         // 後方に配置する
    
    eLAYER_ORDER_TYPE_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
// [LayerData.hpp]で[LayerOrderCell.hpp]をインクルードしているのでここではクラスの宣言だけしておく
class CLayerData;

//-------------------------------------------------
// セル内リストのノード（※パラメータを表示データへ紐づける器）
//-------------------------------------------------
typedef struct{
    // 対象レイヤーデータ（※表示データ）
    CLayerData* pLayerData;
    
    // レイヤーデータに対するパラメータ
    stBEZIER_LAYER_PARAM layerParam;
    
    // 次の要素（※リスト形式）
    void* pNext;
} stLAYER_ORDER_CELL_NODE;

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// レイヤーオーダーセル
//----------------------------------------
class CLayerOrderCell: public CListNode{
    // 実装
    IMPLEMENT_NODE( CLayerOrderCell )       // 実装：リストノード
    
protected:
    // オーダー値（※このセルがどのスロットに対するものかを表す）
    eBD_SLOT m_nOrderSlot;          // スロット
    int m_nOrderSlotIndex;          // スロットインデックス
    
    // 各タイプのノードヘッダ
    stLAYER_ORDER_CELL_NODE* m_pArrNodeHead[eLAYER_ORDER_TYPE_MAX];

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
    stLAYER_ORDER_CELL_NODE* getNodeHead( eLAYER_ORDER_TYPE type );
    
    // データ追加（※先に登録されるほど先に描画される＝手前に表示される）
    stLAYER_ORDER_CELL_NODE* addNode( CLayerData* pData, eLAYER_ORDER_TYPE type );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelLayerOrderType[];
extern const char* g_pArrLabelLayerOrderTypeS[];

#endif	// __LAYER_ORDER_CELL_HPP__
