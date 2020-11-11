/*+----------------------------------------------------------------+
  |	Title:		BmpDotPartData.cpp [共通環境]
  |	Comment:	BMPの画素パーツデータ：パーツに対応したデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotPartData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_BMP_DOT_PART_DATA   4096

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotPartData, eMEM_FIELD_D_BMP_DOT, MAX_ALLOC_BMP_DOT_PART_DATA )

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
int CBmpDotPartData::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotPartData, eMEM_FIELD_D_BMP, MAX_ALLOC_BMP_DOT_PART_DATA )
    //------------------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CBmpDotPartData::GetAllocSize( void ){
    int allocSize = 0;
    
    //------------------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CBmpDotPartData, eMEM_FIELD_D_BMP, MAX_ALLOC_BMP_DOT_PART_DATA )
    //------------------------------------------------------------------------------------------------------
    allocSize += sizeof(CBmpDotPartData) * MAX_ALLOC_BMP_DOT_PART_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CBmpDotPartData::CBmpDotPartData( void ): CDataList(true), CListNode(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpDotPartData::~CBmpDotPartData( void ){
    clear();    // リストの解放
}

//-------------------------
// クリア
//-------------------------
void CBmpDotPartData::clear( void ){
    m_nFlag = 0;
    m_oSlotAdj.clear();

    // リスト解放
    CLayerData* pLayer =  getDataHead();
    while( pLayer != NULL ){
        CLayerData* pLD = pLayer;
        pLayer = pLayer->getNext();
        CLayerData::Free( pLD );
    }
    m_oDataList.disconnectAll();

    // 識別用
    m_eSlot = (eBD_SLOT)0;
	m_nSubId = BD_SUB_ID_MIN;
    m_eDir = (eBD_DIR)0;

    // HOOK用
    m_eHookSlot = eBD_SLOT_INVALID;
    m_nHookSlotIndex = BD_SLOT_INDEX_INVALID;

    // 参照用（※すべて無効値を指定する＝無効値は検索時に参照元と同じ値になる）
    m_eRefCategory = eBD_CATEGORY_INVALID;
    m_nRefUid = UNIQUE_ID_INVALID;
    m_eRefEmo = eBD_EMO_INVALID;
    m_eRefForm = eBD_FORM_INVALID;
    m_eRefSlot = eBD_SLOT_INVALID;
    m_nRefSubId = BD_SUB_ID_INVALID;
    m_eRefDir = eBD_DIR_INVALID;
    
    m_nRefRateScale = BEZIER_SCALE_RATE;
    m_nRefRateRot = 0;

    // 下絵
    m_eRoughImgId = eIMG_INVALID;
    m_nRoughRateScale = BEZIER_SCALE_RATE;
    m_nRoughRateOfsX = m_nRoughRateOfsY = 0;
    m_nRoughAlpha = 128;
    m_nRoughBgCol = 160;
    m_nRoughGridCol = 200;
    
    for( int i=0; i<BDPD_HIDE_SLOT_NUM; i++ ){
        m_eArrHideSlot[i] = eBD_SLOT_INVALID;
        m_eArrHideSlotOposite[i] = eBD_SLOT_INVALID;
    }
}

//-------------------------
// コピー
//-------------------------
void CBmpDotPartData::copy( CBmpDotPartData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();
    
    m_nFlag = pData->m_nFlag;
    m_oSlotAdj.copy( &pData->m_oSlotAdj );

    // リストのコピー
    CLayerData* pLayer = pData->getDataHead();
    while( pLayer != NULL ){
        CLayerData* pNew = CLayerData::Alloc();
        pNew->copy( pLayer);
        addData( pNew );
        
        pLayer = pLayer->getNext();
    }

    // 識別用
    m_eSlot = pData->m_eSlot;
	m_nSubId = pData->m_nSubId;
    m_eDir = pData->m_eDir;

    // HOOK用
    m_eHookSlot = pData->m_eHookSlot;
    m_nHookSlotIndex = pData->m_nHookSlotIndex;

    // 参照用
    m_eRefCategory = pData->m_eRefCategory;
    m_nRefUid = pData->m_nRefUid;
    m_eRefEmo = pData->m_eRefEmo;
    m_eRefForm = pData->m_eRefForm;
    m_eRefSlot = pData->m_eRefSlot;
    m_nRefSubId = pData->m_nRefSubId;
    m_eRefDir = pData->m_eRefDir;
    
    m_nRefRateScale = pData->m_nRefRateScale;
    m_nRefRateRot = pData->m_nRefRateRot;

    m_eRoughImgId = pData->m_eRoughImgId;
    m_nRoughRateScale = pData->m_nRoughRateScale;
    m_nRoughRateOfsX = pData->m_nRoughRateOfsX;
    m_nRoughRateOfsY = pData->m_nRoughRateOfsY;
    m_nRoughAlpha = pData->m_nRoughAlpha;
    m_nRoughBgCol = pData->m_nRoughBgCol;
    m_nRoughGridCol = pData->m_nRoughGridCol;
    
    for( int i=0; i<BDPD_HIDE_SLOT_NUM; i++ ){
        m_eArrHideSlot[i] = pData->m_eArrHideSlot[i];
        m_eArrHideSlotOposite[i] = pData->m_eArrHideSlotOposite[i];
    }
}

//---------------------------
// 読み込み
//---------------------------
void CBmpDotPartData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();
    
    m_nFlag = pIB->readInt16();
    m_oSlotAdj.read( pIB );

    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        CLayerData* pLD = CLayerData::Alloc();
        pLD->read( pIB );
        addData( pLD );
    }

    // 識別用
    m_eSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
	m_nSubId = pIB->readInt16();
    m_eDir = (eBD_DIR)CEnum::ReadEnum( pIB, g_pArrLabelBdDir );

    // HOOK用
    m_eHookSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_nHookSlotIndex = pIB->readInt16();

    // 参照用
    m_eRefCategory = (eBD_CATEGORY)CEnum::ReadEnum( pIB, g_pArrLabelBdCategory );
    m_nRefUid = pIB->readInt32();    
    m_eRefEmo = (eBD_EMO)CEnum::ReadEnum( pIB, g_pArrLabelBdEmo );
    m_eRefForm = (eBD_FORM)CEnum::ReadEnum( pIB, g_pArrLabelBdForm );
    m_eRefSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_nRefSubId = pIB->readInt16();
    m_eRefDir = (eBD_DIR)CEnum::ReadEnum( pIB, g_pArrLabelBdDir );
        
    m_nRefRateScale = pIB->readInt16();
    m_nRefRateRot = pIB->readInt16();

    m_eRoughImgId = (eIMG)CEnum::ReadEnum( pIB, g_pArrLabelImg );
    m_nRoughRateScale = pIB->readInt32();     // [int]で読み込む
    m_nRoughRateOfsX = pIB->readInt16();
    m_nRoughRateOfsY = pIB->readInt16();
    m_nRoughAlpha = pIB->readUint8();
    m_nRoughBgCol = pIB->readUint8();
    m_nRoughGridCol = pIB->readUint8();
    
    for( int i=0; i<BDPD_HIDE_SLOT_NUM; i++ ){
        m_eArrHideSlot[i] = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
        m_eArrHideSlotOposite[i] = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    }
}

//---------------------------
// 書き込み
//---------------------------
void CBmpDotPartData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (short)m_nFlag );
    m_oSlotAdj.write( pOB );

    // 要素出力
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    CLayerData* pLayer = getDataHead();
    while( pLayer != NULL ){
        pLayer->write( pOB );
        pLayer = pLayer->getNext();
    }

    // 識別用
    CEnum::WriteEnum( pOB, m_eSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
	pOB->writeInt16( (short)m_nSubId );
    CEnum::WriteEnum( pOB, m_eDir, eBD_DIR_MAX, g_pArrLabelBdDir );

    // HOOK用
    CEnum::WriteEnum( pOB, m_eHookSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    pOB->writeInt16( (short)m_nHookSlotIndex );

    // 参照用
    CEnum::WriteEnum( pOB, m_eRefCategory, eBD_CATEGORY_MAX, g_pArrLabelBdCategory );
    pOB->writeInt32( m_nRefUid );
    CEnum::WriteEnum( pOB, m_eRefEmo, eBD_EMO_MAX, g_pArrLabelBdEmo );
    CEnum::WriteEnum( pOB, m_eRefForm, eBD_FORM_MAX, g_pArrLabelBdForm );
    CEnum::WriteEnum( pOB, m_eRefSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    pOB->writeInt16( (short)m_nRefSubId );
    CEnum::WriteEnum( pOB, m_eRefDir, eBD_DIR_MAX, g_pArrLabelBdDir );

    pOB->writeInt16( (short)m_nRefRateScale );
    pOB->writeInt16( (short)m_nRefRateRot );

    CEnum::WriteEnum( pOB, m_eRoughImgId, eIMG_MAX, g_pArrLabelImg );
    pOB->writeInt32( m_nRoughRateScale );     // [int]で保存
    pOB->writeInt16( (short)m_nRoughRateOfsX );
    pOB->writeInt16( (short)m_nRoughRateOfsY );
    pOB->writeUint8( m_nRoughAlpha );
    pOB->writeUint8( m_nRoughBgCol );
    pOB->writeUint8( m_nRoughGridCol );
    
    for( int i=0; i<BDPD_HIDE_SLOT_NUM; i++ ){
        CEnum::WriteEnum( pOB, m_eArrHideSlot[i], eBD_SLOT_MAX, g_pArrLabelBdSlot );
        CEnum::WriteEnum( pOB, m_eArrHideSlotOposite[i], eBD_SLOT_MAX, g_pArrLabelBdSlot );
    }
}

//--------------------
// 識別名取得
//--------------------
const char* CBmpDotPartData::getNameWithTempBuf( void ){
    char* pBuf = CMemMgr::GetBufFromTempStr();
    CBDConst::SetBmpDotPartDataName( pBuf, m_eSlot, m_nSubId, m_eDir );
    return( pBuf );
}

//-------------------------------
// 一致するか？
//-------------------------------
bool CBmpDotPartData::isEqual( eBD_SLOT slot, int subId, eBD_DIR dir ){
    // 無効であれば検索にかからない
    if( checkFlag( eBDPD_FLAG_DISABLE ) ){
        return( false );
    }
    
    if( m_eSlot != slot ){ return( false ); }
	if( m_nSubId != subId ){ return( false ); }
    if( m_eDir != dir ){ return( false ); }

    // ここまできたら一致
	return( true );
}

//-----------------------------
// レイヤーの取得
//-----------------------------
CLayerData* CBmpDotPartData::getLayerDataWithOrderTypeAt( eBD_LAYER_ORDER_TYPE orderType, int at ){
    CLayerData* pLD = getDataHead();
    while( pLD != NULL ){
        if( ! pLD->checkFlag( eLAYER_FLAG_DISABLE) ){
            // オーダータイプにより取得先の判定
            if( pLD->getOrderType() == orderType ){
                if( at <= 0 ){
                    break;
                }
                at--;
            }
        }
        
        // 次のレイヤー
        pLD = pLD->getNext();
    }

    return( pLD );
}

//-----------------------------
// オブジェクトの一時調整値が有効か？
//-----------------------------
bool CBmpDotPartData::checkTempAdjustForNodes( void ){
    CLayerData* pCell = getDataHead();
    while( pCell != NULL ){
        if( pCell->isTempAdjustValid() || pCell->checkTempAdjustForNodes() ){
            return( true );
        }
        pCell = pCell->getNext();
    }

    return( false );
}

//--------------------------------------
// スケールの反映
//--------------------------------------
void CBmpDotPartData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視
    
    // [m_oSlotAdj]
    m_oSlotAdj.applyRateScale( rateScale );
    
    // [m_oDataList]
    CLayerData* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // 識別データは無視
    // [m_eSlot]
    // [m_nSubId]
    // [m_eDir]

    // HOOKデータは無視
    // [m_eHookSlot]
    // [m_nHookSlotIndex]

    // 参照データは無視
    // [m_eRefCategory]
    // [m_nRefUid]
    // [m_eRefEmo]
    // [m_eRefForm]
    // [m_eRefSlot]
    // [m_nRefSubId]
    // [m_eRefDir]

    // [m_nRefRateScale]
    // [m_nRefRateRot]
    
    // 作業用データは無視
    // [m_eRoughImgId]
    // [m_nRoughRateScale]
    // [m_nRoughRateOfsX]
    // [m_nRoughRateOfsY]
    // [m_nRoughAlpha]
    // [m_nRoughBgCol]
    // [m_nGridCol]
    
    // [m_eArrHideSlot]は無視
    // [m_eArrHideSlotOposite]は無視
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpDotPartData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 5 + 9 + 7 + 2*BDPD_HIDE_SLOT_NUM + eBDPD_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "SLOT", &m_eSlot, eEDIT_VALUE_TYPE_INT32, eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );
    
    // [subId]は無効値を設定できない
    pMenu->setItemAt( id++, "SUB_ID", &m_nSubId, eEDIT_VALUE_TYPE_INT32, BD_SUB_ID_MIN, BD_SUB_ID_MAX-1 );

    pMenu->setItemAtAsLabel( id++, "DIR", &m_eDir, eEDIT_VALUE_TYPE_INT32, eBD_DIR_INVALID, eBD_DIR_MAX-1, g_pArrLabelBdDir );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "HOOK: SLOT", &m_eHookSlot, eEDIT_VALUE_TYPE_INT32, eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );
    pMenu->setItemAt( id++, "HOOK: SLOT INDEX", &m_nHookSlotIndex, eEDIT_VALUE_TYPE_INT32, BD_SLOT_INDEX_INVALID, BD_SLOT_INDEX_MAX-1 );

    pMenu->setSeparatorAt( id, true );
    
    // 参照用
    pMenu->setItemAtAsLabel( id++, "REF: CATEGORY", &m_eRefCategory, eEDIT_VALUE_TYPE_INT32,
                             eBD_CATEGORY_INVALID, eBD_CATEGORY_MAX-1, g_pArrLabelBdCategory );

    pMenu->setItemAt( id++, "REF: UID", &m_nRefUid, eEDIT_VALUE_TYPE_INT32, BD_UID_INVALID, BD_UID_MAX-1 );

    pMenu->setItemAtAsLabel( id++, "REF: EMO", &m_eRefEmo, eEDIT_VALUE_TYPE_INT32,
                            eBD_EMO_INVALID, eBD_EMO_MAX-1, g_pArrLabelBdEmo );
    
    pMenu->setItemAtAsLabel( id++, "REF: FORM", &m_eRefForm, eEDIT_VALUE_TYPE_INT32,
                            eBD_FORM_INVALID, eBD_FORM_MAX-1, g_pArrLabelBdForm );
    pMenu->setItemAtAsLabel( id++, "REF: SLOT", &m_eRefSlot, eEDIT_VALUE_TYPE_INT32, eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );
    
    // [subId]は無効値を設定可能
    pMenu->setItemAt( id++, "REF: SUB_ID", &m_nRefSubId, eEDIT_VALUE_TYPE_INT32, BD_SUB_ID_INVALID, BD_SUB_ID_MAX-1 );
    pMenu->setItemAtAsLabel( id++, "REF: DIR", &m_eRefDir, eEDIT_VALUE_TYPE_INT32, eBD_DIR_INVALID, eBD_DIR_MAX-1, g_pArrLabelBdDir );

    pMenu->setItemAt( id++, "REF: SCALE", &m_nRefRateScale, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE/10, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "REF: ROT", &m_nRefRateRot, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "ROUGH: IMG", &m_eRoughImgId, eEDIT_VALUE_TYPE_INT32, eIMG_INVALID, eIMG_MAX-1, g_pArrLabelImg );
    pMenu->setItemAt( id++, "ROUGH: SCALE", &m_nRoughRateScale, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE/10, BEZIER_SCALE_RATE*10 );
    pMenu->setItemAt( id++, "ROUGH: X", &m_nRoughRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "ROUGH: Y", &m_nRoughRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "ROUGH: ALPHA", &m_nRoughAlpha, eEDIT_VALUE_TYPE_UINT8, 0, 255 );
    pMenu->setItemAt( id++, "ROUGH: BG COL", &m_nRoughBgCol, eEDIT_VALUE_TYPE_UINT8, 0, 255 );
    pMenu->setItemAt( id++, "ROUGH: GRID COL", &m_nRoughGridCol, eEDIT_VALUE_TYPE_UINT8, 0, 255 );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[0]", &m_eArrHideSlot[0], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[0]", &m_eArrHideSlotOposite[0], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[1]", &m_eArrHideSlot[1], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[1]", &m_eArrHideSlotOposite[1], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[2]", &m_eArrHideSlot[2], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[2]", &m_eArrHideSlotOposite[2], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[3]", &m_eArrHideSlot[3], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[3]", &m_eArrHideSlotOposite[3], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[4]", &m_eArrHideSlot[4], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[4]", &m_eArrHideSlotOposite[4], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[5]", &m_eArrHideSlot[5], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[5]", &m_eArrHideSlotOposite[5], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[6]", &m_eArrHideSlot[6], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[6]", &m_eArrHideSlotOposite[6], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT[7]", &m_eArrHideSlot[7], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "HIDE: SLOT_OP[7]", &m_eArrHideSlotOposite[7], eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDPD_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: ROUGH_FLIP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDPD_FLAG_ROUGH_FLIP );
    pMenu->setItemAtAsBit( id++, "FLAG: REF_DRAW", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDPD_FLAG_REF_DRAW );
    pMenu->setItemAtAsBit( id++, "FLAG: REF_SLOT", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDPD_FLAG_REF_SLOT );
    pMenu->setItemAtAsBit( id++, "FLAG: HIDE_OVER_HAIR", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eBDPD_FLAG_HIDE_OVER_HAIR );

    // 確定
    pMenu->fixMenu();
}
