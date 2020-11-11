/*+----------------------------------------------------------------+
  |	Title:		SlotPointData.cpp [共通環境]
  | Comment:    スロットポイントデータ（※[CSlotObjectData]の構成要素）
  |             スロットは描画処理とは別管理のため[HOOK]等での座標同期ができない
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "SlotPointData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_SLOT_POINT_DATA   (8*1024)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )

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
//----------------------------------------------
// 確保メモリセル数（※管理内で[new]するセル数）
//----------------------------------------------
int CSlotPointData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CSlotPointData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CSlotPointData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_SLOT_POINT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CSlotPointData) * MAX_ALLOC_SLOT_POINT_DATA;
    
    return( allocSize );
}

//---------------------------
// コンストラクタ
//---------------------------
CSlotPointData::CSlotPointData( void ): CListNode(){
	clear();
}

//----------------------------------------------------------------------------
// クリア（※データの破棄）
//----------------------------------------------------------------------------
void CSlotPointData::clear( void ){
    m_nFlag = 0;
    m_oAdjXY.clear();
    
    m_eSlot = eBD_SLOT_INVALID;
    m_nSlotIndex = BD_SLOT_INDEX_INVALID;
}

//--------------------
// 設定
//--------------------
void CSlotPointData::set( int rX, int rY, eBD_SLOT slot, int slotIndex ){
    m_oAdjXY.setRateXY( rX, rY );
    
    m_eSlot = slot;
    m_nSlotIndex = slotIndex;
}

//--------------------
// コピー
//--------------------
void CSlotPointData::copy( CSlotPointData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_nFlag = pData->m_nFlag;
    m_oAdjXY.copy( &pData->m_oAdjXY );
    
    m_eSlot = pData->m_eSlot;
    m_nSlotIndex = pData->m_nSlotIndex;
}

//---------------------------
// 読み込み
//---------------------------
void CSlotPointData::read( CInputBuffer* pIB ){
    m_nFlag = pIB->readInt16();
    m_oAdjXY.read( pIB );

    m_eSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_nSlotIndex = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CSlotPointData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );
    m_oAdjXY.write( pOB );

    CEnum::WriteEnum( pOB, m_eSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    pOB->writeInt16( (short)m_nSlotIndex );
}

//------------
// スケール適用
//------------
void CSlotPointData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視
    
    m_oAdjXY.applyRateScale( rateScale );
    
    // [m_eSlot]は無視
    // [m_nSlotIndex]は無視
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CSlotPointData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 2 + eSPD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "SLOT", &m_eSlot, eEDIT_VALUE_TYPE_INT32, eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );
    
    // [slotIndex]は無効値を設定可能
    pMenu->setItemAt( id++, "SLOT_INDEX", &m_nSlotIndex, eEDIT_VALUE_TYPE_INT32, BD_SLOT_INDEX_INVALID, BD_SLOT_INDEX_MAX-1 );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: FLIP_H", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_FLIP_H );
    pMenu->setItemAtAsBit( id++, "FLAG: FLIP_V", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eSPD_FLAG_FLIP_V );

    // 確定
    pMenu->fixMenu();
}
