/*+----------------------------------------------------------------+
  | Title:      LayerOrderList.cpp [共通環境]
  | Comment:    レイヤーオーダーリスト（※先に登録されたものほど手前の概念）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerOrderList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------
// コンストラクタ
//----------------
CLayerOrderList::CLayerOrderList( void ){
    clear();
}

//----------------
// コンストラクタ
//----------------
CLayerOrderList::~CLayerOrderList( void ){
    clear();    // リスト解放
}

//----------------
// クリア
//----------------
void CLayerOrderList::clear( void ){
    m_oList.deleteNodeAll();
    m_nCurPriority = 0;
}

//----------------
// セルの取得
//----------------
CLayerOrderCell* CLayerOrderList::getOrderCell( eBD_SLOT slot, int slotIndex ){
    // リストを検索して見つかったら返す
    CLayerOrderCell* pCell = getOrderCellHead();
    while( pCell != NULL ){
        if( pCell->isEqual( slot, slotIndex ) ){
            return( pCell );
        }
        
        pCell = pCell->getNext();
    }
    
    //---------------------------------------------
    // 見つからない場合は作成（※ワークなので[APP]から確保）
    //---------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    pCell = new CLayerOrderCell();
    CMemMgr::PopTargetField();
        
    pCell->setUid( m_nCurPriority++ );
    pCell->setOrder( slot, slotIndex );
    
    // リストに追加
    m_oList.add( pCell );
    return( pCell );
}

//------------------
// レイヤーデータの登録
//------------------
stLAYER_ORDER_CELL_NODE* CLayerOrderList::addLayerData( CLayerData* pData, eBD_LAYER_ORDER_TYPE type, eBD_SLOT slot, int slotIndex ){
    // 無効は無視
    if( pData == NULL ){
        LOGE( "@ CLayerOrderList::addLayerData: INVALID DATA: pData=%p, type=%d, target=%d, index=%d\n",
              pData, type, slot, slotIndex );
        return( NULL );
    }
    
    if( type < 0 || type >= eBD_LAYER_ORDER_TYPE_MAX ){
        LOGE( "@ CLayerOrderList::addLayerData: INVALID TYPE: type=%d, target=%d, index=%d\n",
              type, slot, slotIndex );
        return( NULL );
    }
    
    // ターゲットセルの取得（※見つからない場合は内部で作成されるため[NULL]は返らない）
    CLayerOrderCell* pTargetCell = getOrderCell( slot, slotIndex );

    // セルへ登録
    return( pTargetCell->addNode( pData, type ) );
}

//------------------------------
// 登録済みのセルを優先順位順に取り出す
//------------------------------
CLayerOrderCell* CLayerOrderList::getOrderCellByPriority( int priority ){
    if( priority >= 0 && priority < m_nCurPriority ){
        CListNode* pNode = m_oList.getByUid( priority, false );
        return( (CLayerOrderCell*) pNode );
    }

    // ここまできたら困る
    LOGE( "@ CLayerOrderList::getOrderCellByPriority: INVALID: priorirty=%d\n", priority );
    return( NULL );
}
