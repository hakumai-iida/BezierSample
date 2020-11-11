/*+----------------------------------------------------------------+
  | Title:      BmpDotData.cpp [共通環境]
  | Comment:    BMPの画素素材データ：フォームに対応したデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_BMP_DOT_DATA   256

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotData, eMEM_FIELD_D_BMP_DOT, MAX_ALLOC_BMP_DOT_DATA )

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
int CBmpDotData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotData, eMEM_FIELD_D_BMP, MAX_ALLOC_BMP_DOT_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CBmpDotData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotData, eMEM_FIELD_D_BMP, MAX_ALLOC_BMP_DOT_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CBmpDotData) * MAX_ALLOC_BMP_DOT_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CBmpDotData::CBmpDotData( void ): CDataList(true), CListNode(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpDotData::~CBmpDotData( void ){
    clear();    // リストの解放
}

//-------------------------
// クリア
//-------------------------
void CBmpDotData::clear( void ){
    m_nFlag = 0;

    // リスト解放
    CBmpDotPartData* pData = getDataHead();
    while( pData != NULL ){
        CBmpDotPartData* pOld = pData;
        pData = pData->getNext();
        CBmpDotPartData::Free( pOld );
    }
    m_oDataList.disconnectAll();
    
    m_eForm = eBD_FORM_INVALID;
}

//-------------------------
// コピー
//-------------------------
void CBmpDotData::copy( CBmpDotData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();
    
    m_nFlag = pData->m_nFlag;
    
    // リストのコピー
    CBmpDotPartData* pBDPD = pData->getDataHead();
    while( pBDPD != NULL ){
        CBmpDotPartData* pNew = CBmpDotPartData::Alloc();
        pNew->copy( pBDPD );
        addData( pNew );
        
        pBDPD = pBDPD->getNext();
    }
    
    m_eForm = pData->m_eForm;
}

//-------------------------
// データ取得
//-------------------------
bool CBmpDotData::isEqual( eBD_FORM form ){
    // 無効であれば検索にかからない
    if( checkFlag( eBDD_FLAG_DISABLE ) ){
        return( false );
    }

    return( m_eForm == form );
}

//-------------------------
// データ取得
//-------------------------
CBmpDotPartData* CBmpDotData::searchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParam ){
    // 指定が一致したら要素の確認
    if( isEqual( pParam->form ) ){
        CBmpDotPartData* pData = getDataHead();
        while( pData != NULL ){
            if( pData->isEqual( pParam->slot, pParam->subId, pParam->dir ) ){
                return( pData );
            }
            pData = pData->getNext();
        }
    }
    
    // ここまできたらミス（※[NULL]を返しておく）
    return( NULL );
}

//---------------------------
// 読み込み
//---------------------------
void CBmpDotData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    m_nFlag = pIB->readInt16();
    
    // 要素読み込み
    int num = pIB->readInt16();
    for( int i=0; i<num; i++ ){
        CBmpDotPartData* pNew = CBmpDotPartData::Alloc();
        pNew->read( pIB );
        addData( pNew );
    }

    m_eForm = (eBD_FORM)CEnum::ReadEnum( pIB, g_pArrLabelBdForm );
}

//---------------------------
// 書き込み
//---------------------------
void CBmpDotData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (short)m_nFlag );

    // 要素出力
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    CBmpDotPartData* pData = getDataHead();
    while( pData != NULL ){
        pData->write( pOB );
        pData = pData->getNext();
    }

    CEnum::WriteEnum( pOB, m_eForm, eBD_FORM_MAX, g_pArrLabelBdForm );
}

//--------------------------------------
// スケールの反映
//--------------------------------------
void CBmpDotData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視
    
    // [m_oDataList]
    CBmpDotPartData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_eForm]は無視
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpDotData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 1 + eBDD_FLAG_MAX  );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "FORM", &m_eForm, eEDIT_VALUE_TYPE_INT32, eBD_FORM_INVALID, eBD_FORM_MAX-1, g_pArrLabelBdForm );
    
    pMenu->setSeparatorAt( id, true );
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: LOCKED", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDD_FLAG_LOCKED );

    // 確定
    pMenu->fixMenu();
}
