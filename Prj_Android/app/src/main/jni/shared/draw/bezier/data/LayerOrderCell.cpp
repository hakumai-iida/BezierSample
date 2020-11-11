/*+----------------------------------------------------------------+
  | Title:      LayerOrderCell.cpp [共通環境]
  | Comment:    レイヤーオーダーセル（※オーダー登録単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerOrderCell.hpp"

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
CLayerOrderCell::CLayerOrderCell( void ){
    clear();
}

//----------------
// コンストラクタ
//----------------
CLayerOrderCell::~CLayerOrderCell( void ){
    clear();    // リストの解放
}

//----------------
// クリア
//----------------
void CLayerOrderCell::clear( void ){
    m_nOrderSlot = eBD_SLOT_DEFAULT;
    m_nOrderSlotIndex = 0;

    // リストのクリア
    for( int i=0; i<eBD_LAYER_ORDER_TYPE_MAX; i++ ){
        // ノードの解放
        stLAYER_ORDER_CELL_NODE* pHead = m_pArrNodeHead[i];
        while( pHead != NULL ){
            stLAYER_ORDER_CELL_NODE* pNext = pHead->pNext;
            SAFE_FREE( pHead );
            pHead = pNext;
        }
        
        m_pArrNodeHead[i] = NULL;
    }
}

//---------------
// 設定
//---------------
void CLayerOrderCell::setOrder( eBD_SLOT slot, int slotIndex ){
    m_nOrderSlot = slot;
    m_nOrderSlotIndex = CBDConst::FixIndexForSlot( slot, slotIndex );
}

//---------------
// 評価
//---------------
bool CLayerOrderCell::isEqual( eBD_SLOT slot, int slotIndex ){
    slotIndex = CBDConst::FixIndexForSlot( slot, slotIndex );
    return( m_nOrderSlot==slot && m_nOrderSlotIndex==slotIndex );
}

//----------------
// ヘッダ取得
//----------------
stLAYER_ORDER_CELL_NODE* CLayerOrderCell::getNodeHead( eBD_LAYER_ORDER_TYPE type ){
    if( type < 0 || type >= eBD_LAYER_ORDER_TYPE_MAX ){
        LOGE( "@ CLayerOrderCell::getNodeHead: INVALID TYPE: type=%d\n", type );
        return( NULL );
    }
    
    return( m_pArrNodeHead[type] );
}

//----------------------------------------------
// リスト関連（※管理情報領域を確保する）
//（※先に登録されるほど先に描画される＝手前に表示される）
//----------------------------------------------
stLAYER_ORDER_CELL_NODE* CLayerOrderCell::addNode( CLayerData* pData, eBD_LAYER_ORDER_TYPE type ){
    // 無効は無視
    if( pData == NULL ){
        LOGE( "@ CLayerOrderCell::addNode: INVALID DATA: pData=%p\n", pData );
        return( NULL );
    }
    
    if( type < 0 || type >= eBD_LAYER_ORDER_TYPE_MAX ){
        LOGE( "@ CLayerOrderCell::addNode: INVALID TYPE: type=%d\n", type );
        return( NULL );
    }

    //--------------------------------------------------------------------------
    // ノードの確保（※ワークなので[APP]から確保）
    //（※専用マネージャ等を作っての管理はしない＝BMP生成の流れで確保されすぐに解放される想定）
    //--------------------------------------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    stLAYER_ORDER_CELL_NODE* pNode = (stLAYER_ORDER_CELL_NODE*) MALLOC( sizeof(stLAYER_ORDER_CELL_NODE) );
    CMemMgr::PopTargetField();

    // 設定
    pNode->pLayerData = pData;
    CLEAR_BEZIER_LAYER_PARAM( &pNode->layerParam ); // レイヤーパラメータのクリア（※設定は呼び出し元で行う）
    pNode->group = eBD_LAYER_ORDER_GROUP_INVALID;
    pNode->isDone = false;

    //-----------------------------
    // リストへ登録
    //-----------------------------
    stLAYER_ORDER_CELL_NODE* pHead = m_pArrNodeHead[type];
    
    // リストが空ならヘッダに設定
    if( pHead == NULL ){
        m_pArrNodeHead[type] = pNode;
    }
    // 空でなければ末尾へ追加
    else{
        while( pHead != NULL ){
            // 末尾であれば抜ける
            if( pHead->pNext == NULL ){
                break;
            }

            pHead = pHead->pNext;
        }
        
        pHead->pNext = pNode;
    }

    return( pNode );
}
