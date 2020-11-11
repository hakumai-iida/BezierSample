/*+----------------------------------------------------------------+
  | Title:      BmpDotPack.cpp [共通環境]
  | Comment:    BMP画素データパック
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotPack.hpp"

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
//-------------------------
// コンストラクタ
//-------------------------
CBmpDotPack::CBmpDotPack( void ): CDataList(true), CDataFile(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpDotPack::~CBmpDotPack( void ){
    clear();    // リストの解放
}

//-------------------------
// クリア
//-------------------------
void CBmpDotPack::clear( void ){
    // リスト解放
    CBmpDotData* pData = getDataHead();
    while( pData != NULL ){
        CBmpDotData* pOld = pData;
        pData = pData->getNext();
        CBmpDotData::Free( pOld );
    }
    m_oDataList.disconnectAll();

    m_eCategory = eBD_CATEGORY_INVALID;
    m_nUid = BD_UID_INVALID;
    m_eEmo = eBD_EMO_INVALID;
}

//-------------------------
// 判定
//-------------------------
bool CBmpDotPack::isEqual( eBD_CATEGORY category, int uid, eBD_EMO emo ){
    return( m_eCategory == category && m_nUid == uid && m_eEmo == emo );
}

//-------------------------
// データ検索
//-------------------------
CBmpDotData* CBmpDotPack::searchBmpDotData( stBD_PART_SEARCH_PARAM* pParam ){
    if( isEqual( pParam->category, pParam->uid, pParam->emo ) ){
        CBmpDotData* pData = getDataHead();
        while( pData != NULL ){
            if( pData->isEqual( pParam->form ) ){
                return( pData );
            }
            
            pData = pData->getNext();
        }
    }
    
    // ここまできたらミス（※[NULL]を返しておく）
    return( NULL );
}

//-------------------------
// パーツ検索
//-------------------------
CBmpDotPartData* CBmpDotPack::searchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParam ){
    if( isEqual( pParam->category, pParam->uid, pParam->emo ) ){
        CBmpDotData* pData = getDataHead();
        while( pData != NULL ){
            CBmpDotPartData* pBDPD = pData->searchBmpDotPartData( pParam );
            if( pBDPD != NULL ){
                return( pBDPD );
            }
            
            pData = pData->getNext();
        }
    }

    // ここまできたらミス（※[NULL]を返しておく）
    return( NULL );
}

//---------------------------
// ロード実体（※読み込み）
//---------------------------
void CBmpDotPack::load0( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    // 要素読み込み
    int num = pIB->readInt16();
    for( int i=0; i<num; i++ ){
        CBmpDotData* pNew = CBmpDotData::Alloc();
        pNew->read( pIB );
        addData( pNew );
    }

    m_eCategory = (eBD_CATEGORY)CEnum::ReadEnum( pIB, g_pArrLabelBdCategory );
    m_nUid = pIB->readInt32();
    m_eEmo = (eBD_EMO)CEnum::ReadEnum( pIB, g_pArrLabelBdEmo );
}

//---------------------------
// セーブ実体（※書き込み）
//---------------------------
void CBmpDotPack::save0( COutputBuffer* pOB ){
    // 要素出力
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    CBmpDotData* pData = getDataHead();
    while( pData != NULL ){
        pData->write( pOB );
        pData = pData->getNext();
    }

    CEnum::WriteEnum( pOB, m_eCategory, eBD_CATEGORY_MAX, g_pArrLabelBdCategory );
    pOB->writeInt32( m_nUid );
    CEnum::WriteEnum( pOB, m_eEmo, eBD_EMO_MAX, g_pArrLabelBdEmo );
}

//--------------------------------------
// スケールの反映
//--------------------------------------
void CBmpDotPack::applyRateScale( int rateScale ){
    // [m_oDataList]
    CBmpDotData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_eCategory]は無視
    // [m_nUid]は無視
    // [m_eEmo]は無視
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpDotPack::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 3 );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "CATEGORY", &m_eCategory, eEDIT_VALUE_TYPE_INT32,
                             eBD_CATEGORY_INVALID, eBD_CATEGORY_MAX-1, g_pArrLabelBdCategory );

    pMenu->setItemAt( id++, "UID", &m_nUid, eEDIT_VALUE_TYPE_INT32, BD_UID_INVALID, BD_UID_MAX-1 );

    pMenu->setItemAtAsLabel( id++, "EMO", &m_eEmo, eEDIT_VALUE_TYPE_INT32,
                             eBD_EMO_INVALID, eBD_EMO_MAX-1, g_pArrLabelBdEmo );

    // 確定
    pMenu->fixMenu();
}
