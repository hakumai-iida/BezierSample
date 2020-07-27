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
    
    // 最前面／最後面の要素クリア＆設定
    m_oCellLyFront.clear();
    m_oCellLyFront.setOrder( eBD_SLOT_LyFront, 0 );
    
    m_oCellLyBack.clear();
    m_oCellLyFront.setOrder( eBD_SLOT_LyBack, 0 );
    
    // 優先順位は１始め（※[0＝LyFront]、[m_nCurPriority=LyBack]となる）
    m_nCurPriority = 1;
}

//----------------
// セルの取得
//----------------
CLayerOrderCell* CLayerOrderList::getOrderCell( eBD_SLOT slot, int slotIndex, bool isCreateIfNotFound ){
    // リストを検索して見つかったら返す
    CLayerOrderCell* pCell = getOrderCellHead();
    while( pCell != NULL ){
        if( pCell->isEqual( slot, slotIndex ) ){
            return( pCell );
        }
        
        pCell = pCell->getNext();
    }
    
    // 見つからない場合、指定があれば作成
    if( isCreateIfNotFound ){
        //-------------------------------------------------------------------------
        // セルの確保（※ワークなので[APP]から確保）
        //（※専用マネージャ等を作っての管理はしない＝BMP生成の流れで確保されすぐに解放される想定）
        //-------------------------------------------------------------------------
        CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
        CLayerOrderCell* pCell = new CLayerOrderCell();
        CMemMgr::PopTargetField();
            
        pCell->setUid( m_nCurPriority++ );
        pCell->setOrder( slot, slotIndex );
        
        // リストに追加
        m_oList.add( pCell );
        return( pCell );
    }

    return( NULL );
}

//------------------
// レイヤーデータの登録
//------------------
stLAYER_ORDER_CELL_NODE* CLayerOrderList::addLayerData( CLayerData* pData, eLAYER_ORDER_TYPE type, eBD_SLOT slot, int slotIndex ){
    // 無効は無視
    if( pData == NULL ){
        LOGE( "@ CLayerOrderList::addLayerData: INVALID DATA: pData=%p, type=%d, target=%d, index=%d\n",
              pData, type, slot, slotIndex );
        return( NULL );
    }
    
    if( type < 0 || type >= eLAYER_ORDER_TYPE_MAX ){
        LOGE( "@ CLayerOrderList::addLayerData: INVALID TYPE: type=%d, target=%d, index=%d\n",
              type, slot, slotIndex );
        return( NULL );
    }
    
    // ターゲットセルの取得（※見つからない場合は内部で作成される）
    CLayerOrderCell* pTargetCell = getOrderCell( slot, slotIndex, true );

    // セルへ登録
    return( pTargetCell->addNode( pData, type ) );
}

//------------------------------
// 登録済みのセルを優先順位順に取り出す
//------------------------------
CLayerOrderCell* CLayerOrderList::getOrderCellByPriority( int priority ){
    // 先頭要素は[LyFront]
    if( priority == 0 ){
        return( &m_oCellLyFront );
    }
    
    // 末尾要素は[LyBack]
    if( priority == m_nCurPriority ){
        return( &m_oCellLyBack );
    }

    // 先頭と末尾の間の指定であれば
    if( priority > 0 && priority < m_nCurPriority ){
        CListNode* pNode = m_oList.getByUid( priority, false );
        return( (CLayerOrderCell*) pNode );
    }

    // ここまできたら困る
    LOGE( "@ CLayerOrderList::getOrderCellByPriority: INVALID: priorirty=%d\n", priority );
    return( NULL );
}
