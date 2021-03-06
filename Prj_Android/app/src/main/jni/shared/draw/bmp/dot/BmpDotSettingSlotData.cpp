/*+----------------------------------------------------------------+
  | Title:      BmpDotSettingSlotData.hpp [共通環境]
  | Comment:    BMPドットのスロット設定データ（※[CBmpDotSettingData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotSettingSlotData.hpp"

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
//---------------------------
// コンストラクタ
//---------------------------
CBmpDotSettingSlotData::CBmpDotSettingSlotData( void ){
	clear();
}

//--------------------------
// クリア（※データの破棄）
//--------------------------
void CBmpDotSettingSlotData::clear( void ){
    // スロット情報のクリア
    m_eSlot = eBD_SLOT_INVALID;
    m_nSlotIndex = BD_SLOT_INDEX_INVALID;
    
    reset();
}

//--------------------------
// リセット（※設定の一部を消す）
//--------------------------
void CBmpDotSettingSlotData::reset( void ){
    // データ参照先のリセット
    m_eCategory = eBD_CATEGORY_INVALID;
    m_nUid = BD_SUB_ID_INVALID;
    m_eEmo = eBD_EMO_INVALID;
    m_eForm = eBD_FORM_INVALID;
    m_nSubId = BD_SUB_ID_INVALID;
    m_eDir = eBD_DIR_INVALID;
    
    m_nRateAdjForT = 0;
    m_nRateAdjForS = 0;
    
    m_bInvalid = false;
    
    for( int i=0; i<BD_OPTION_FOR_SLOT_MAX; i++ ){
        m_bArrOptionForSlotValid[i] = true; // 初期値は有効
    }
}

//--------------------
// コピー
//--------------------
void CBmpDotSettingSlotData::copy( CBmpDotSettingSlotData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_eSlot = pData->m_eSlot;
    m_nSlotIndex = pData->m_nSlotIndex;

    m_eCategory = pData->m_eCategory;
    m_nUid = pData->m_nUid;
    m_eEmo = pData->m_eEmo;
    m_eForm = pData->m_eForm;
    m_nSubId = pData->m_nSubId;
    m_eDir = pData->m_eDir;
    
    m_nRateAdjForT = pData->m_nRateAdjForT;
    m_nRateAdjForS = pData->m_nRateAdjForS;
    
    m_bInvalid = pData->m_bInvalid;
    
    for( int i=0; i<BD_OPTION_FOR_SLOT_MAX; i++ ){
        m_bArrOptionForSlotValid[i] = pData->m_bArrOptionForSlotValid[i];
    }
}

//---------------------------
// 読み込み
//---------------------------
void CBmpDotSettingSlotData::read( CInputBuffer* pIB ){
    m_eSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_nSlotIndex = pIB->readInt16();

    m_eCategory = (eBD_CATEGORY)CEnum::ReadEnum( pIB, g_pArrLabelBdCategory );
    m_nUid = pIB->readInt32();
    m_eEmo = (eBD_EMO)CEnum::ReadEnum( pIB, g_pArrLabelBdEmo );
    m_eForm = (eBD_FORM)CEnum::ReadEnum( pIB, g_pArrLabelBdForm );
    m_nSubId = pIB->readInt16();
    m_eDir = (eBD_DIR)CEnum::ReadEnum( pIB, g_pArrLabelBdDir );

    m_nRateAdjForT = pIB->readInt16();
    m_nRateAdjForS = pIB->readInt16();
    
    m_bInvalid = (pIB->readUint8() != 0);
    
    for( int i=0; i<BD_OPTION_FOR_SLOT_MAX; i++ ){
        m_bArrOptionForSlotValid[i] = (pIB->readUint8() != 0);
    }
}

//---------------------------
// 書き込み
//---------------------------
void CBmpDotSettingSlotData::write( COutputBuffer* pOB ){
    CEnum::WriteEnum( pOB, m_eSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    pOB->writeInt16( (short)m_nSlotIndex );
    
    CEnum::WriteEnum( pOB, m_eCategory, eBD_CATEGORY_MAX, g_pArrLabelBdCategory );
    pOB->writeInt32( m_nUid );
    CEnum::WriteEnum( pOB, m_eEmo, eBD_EMO_MAX, g_pArrLabelBdEmo );
    CEnum::WriteEnum( pOB, m_eForm, eBD_FORM_MAX, g_pArrLabelBdForm );
    pOB->writeInt16( (short)m_nSubId );
    CEnum::WriteEnum( pOB, m_eDir, eBD_DIR_MAX, g_pArrLabelBdDir );

    pOB->writeInt16( (short)m_nRateAdjForT );
    pOB->writeInt16( (short)m_nRateAdjForS );
    
    pOB->writeUint8( (m_bInvalid)? 1: 0 );
    
    for( int i=0; i<BD_OPTION_FOR_SLOT_MAX; i++ ){
        pOB->writeUint8( (m_bArrOptionForSlotValid[i])? 1: 0 );
    }
}

//-------------------------------
// 判定（※ここでは無効フラグは見ない）
//-------------------------------
bool CBmpDotSettingSlotData::checkSlot( eBD_SLOT slot, int slotIndex ){
    if( ! IS_BD_SLOT_VALID( m_eSlot ) ){
        return( false );
    }
    
    if( ! IS_BD_SLOT_INDEX_VALID( m_nSlotIndex ) ){
        return( false );
    }
    
    return( m_eSlot == slot && m_nSlotIndex == slotIndex );
}

//-------------------------------
// オプションの設定
//-------------------------------
void CBmpDotSettingSlotData::setOptionForSlotValidAt( int at, bool flag ){
    if( at < 0 || at >= BD_OPTION_FOR_SLOT_MAX ){
        LOGE( "@ CBmpDotSettingSlotData::setOptionForSlotValidAt: INVALID at=%d, flag=%d\n", at, flag );
        return;
    }
    
    m_bArrOptionForSlotValid[at] = flag;
}

//-------------------------------
// オプションの取得
//-------------------------------
bool CBmpDotSettingSlotData::isOptionForSlotValidAt( int at ){
    if( at < 0 || at >= BD_OPTION_FOR_SLOT_MAX ){
        LOGE( "@ CBmpDotSettingSlotData::isOptionForSlotValidAt: INVALID at=%d\n", at );
        return( false );
    }

    return( m_bArrOptionForSlotValid[at] );
}
