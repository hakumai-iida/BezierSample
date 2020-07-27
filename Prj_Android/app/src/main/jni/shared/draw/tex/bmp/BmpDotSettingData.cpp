/*+----------------------------------------------------------------+
  | Title:      BmpDotSettingData.cpp [共通環境]
  | Comment:    BMP設定データ（※各スロットのBDPDを指定するデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotSettingData.hpp"

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
CBmpDotSettingData::CBmpDotSettingData( void ): CDataFile(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpDotSettingData::~CBmpDotSettingData( void ){
}

//-------------------------
// クリア
//-------------------------
void CBmpDotSettingData::clear( void ){
    m_nIdForFace = BD_UID_INVALID;
    m_nIdForBody = BD_UID_INVALID;
    m_nIdForSuit = BD_UID_INVALID;
    m_eEmo = eBD_EMO_INVALID;
    m_eForm = eBD_FORM_INVALID;
    
    m_nRateAdjustH = 0;
    m_nRateAdjustV = 0;
    m_nRateAngleLR = 0;
    m_nRateAngleUD = 0;
    
    m_nRateVX = 0;
    m_nRateVY = 0;
    m_nRateAX = 0;
    m_nRateAY = 0;
    m_nMoveCount = 0;

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_bArrOptionValid[i] = false;
    }

    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        m_oArrSlot[i].clear();
    }
}
 
//------------------------------
// コピー
//------------------------------
void CBmpDotSettingData::copy( CBmpDotSettingData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }

    m_nIdForFace = pData->m_nIdForFace;
    m_nIdForBody = pData->m_nIdForBody;
    m_nIdForSuit = pData->m_nIdForSuit;
    m_eEmo = pData->m_eEmo;
    m_eForm = pData->m_eForm;
    
    m_nRateAdjustH = pData->m_nRateAdjustH;
    m_nRateAdjustV = pData->m_nRateAdjustV;
    m_nRateAngleLR = pData->m_nRateAngleLR;
    m_nRateAngleUD = pData->m_nRateAngleUD;
    
    m_nRateVX = pData->m_nRateVX;
    m_nRateVY = pData->m_nRateVY;
    m_nRateAX = pData->m_nRateAX;
    m_nRateAY = pData->m_nRateAY;
    m_nMoveCount = pData->m_nMoveCount;

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_bArrOptionValid[i] = pData->m_bArrOptionValid[i];
    }

    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
         m_oArrSlot[i].copy( &pData->m_oArrSlot[i] );
    }
}


//------------------------------
// オプション設定
//------------------------------
void CBmpDotSettingData::setOptionValid( eBD_OPTION option, bool flag ){
    // 無効は無視
    if( ! IS_BD_OPTION_VALID( option ) ){
        LOGE( "@ CBmpDotSettingData::setOptionValid: INVALID OPTION: option=%d\n", option );
        return;
    }
    
    m_bArrOptionValid[option] = flag;
}

//------------------------------
// オプションターゲット取得
//------------------------------
bool CBmpDotSettingData::getOptionValid( eBD_OPTION option ){
    // 無効は無視
    if( ! IS_BD_OPTION_VALID( option ) ){
        LOGE( "@ CBmpDotSettingData::getOptionValid: INVALID OPTION: option=%d\n", option );
        return( false );
    }
    
    return( m_bArrOptionValid[option] );
}

//--------------------------
// スロットの取得
//--------------------------
CBmpDotSettingSlotData* CBmpDotSettingData::getSlotAt( int at ){
    if( at >= 0 && at < BMP_DOT_SETTING_SLOT_DATA_MAX ){
        return( &m_oArrSlot[at] );
    }

    return( NULL );
}

//--------------------------
// スロットの検索
//--------------------------
CBmpDotSettingSlotData* CBmpDotSettingData::searchSlot( eBD_SLOT slot, int slotIndex ){
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        if( m_oArrSlot[i].checkSlot( slot, slotIndex ) ){
            return( &m_oArrSlot[i] );
        }
    }
    
    return( NULL );
}

//---------------------------
// ロード実体（※読み込み）
//---------------------------
void CBmpDotSettingData::load0( CInputBuffer* pIB ){
    m_nIdForFace = pIB->readInt32();
    m_nIdForBody = pIB->readInt32();
    m_nIdForSuit = pIB->readInt32();
    m_eEmo = (eBD_EMO)CEnum::ReadEnum( pIB, g_pArrLabelBdEmo );
    m_eForm = (eBD_FORM)CEnum::ReadEnum( pIB, g_pArrLabelBdForm );
    
    m_nRateAdjustH = pIB->readInt16();
    m_nRateAdjustV = pIB->readInt16();
    m_nRateAngleLR = pIB->readInt16();
    m_nRateAngleUD = pIB->readInt16();
    
    m_nRateVX = pIB->readInt16();
    m_nRateVY = pIB->readInt16();
    m_nRateAX = pIB->readInt16();
    m_nRateAY = pIB->readInt16();
    m_nMoveCount = pIB->readInt16();
    
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_bArrOptionValid[i] = (pIB->readUint8() ? 1: 0 );
    }

    // スロットデータは保存数がかきこまれている（※枠の数が変わった際への用心）
    int num = pIB->readInt16();
    
    // スロットデータの読み込み
    if( num > BMP_DOT_SETTING_SLOT_DATA_MAX ){
        for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
            m_oArrSlot[i].read( pIB );
        }
        
        // 余剰分は読み捨て
        CBmpDotSettingSlotData tempSlot;
        for( int i=BMP_DOT_SETTING_SLOT_DATA_MAX; i<num; i++ ){
            tempSlot.read( pIB );
        }
    }else{
        for( int i=0; i<num; i++ ){
            m_oArrSlot[i].read( pIB );
        }

        // 余剰分はクリア
        for( int i=num; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
            m_oArrSlot[i].clear();
        }
    }
}

//---------------------------
// セーブ実体（※書き込み）
//---------------------------
void CBmpDotSettingData::save0( COutputBuffer* pOB ){
    pOB->writeInt32( m_nIdForFace );
    pOB->writeInt32( m_nIdForBody );
    pOB->writeInt32( m_nIdForSuit );
    CEnum::WriteEnum( pOB, m_eEmo, eBD_EMO_MAX, g_pArrLabelBdEmo );
    CEnum::WriteEnum( pOB, m_eForm, eBD_FORM_MAX, g_pArrLabelBdForm );

    pOB->writeInt16( (short)m_nRateAdjustH );
    pOB->writeInt16( (short)m_nRateAdjustV );
    pOB->writeInt16( (short)m_nRateAngleLR );
    pOB->writeInt16( (short)m_nRateAngleUD );

    pOB->writeInt16( (short)m_nRateVX );
    pOB->writeInt16( (short)m_nRateVY );
    pOB->writeInt16( (short)m_nRateAX );
    pOB->writeInt16( (short)m_nRateAY );
    pOB->writeInt16( (short)m_nMoveCount );

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        pOB->writeUint8( m_bArrOptionValid[i]? 1: 0 );
    }

    // スロットデータの要素数の出力（※読み込み時に利用する）
    pOB->writeInt16( BMP_DOT_SETTING_SLOT_DATA_MAX );
    
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        m_oArrSlot[i].write( pOB );
    }
}

//-------------------------
// スーツ（着ぐるみ）判定
//-------------------------
bool CBmpDotSettingData::isSuited( void ){
    // [BlBase](上半身と下半身を呼ぶ腹)がスーツカテゴリなら着ぐるみ
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        if( m_oArrSlot[i].getSlot() == eBD_SLOT_BlBase ){
            if( m_oArrSlot[i].getCategory() == eBD_CATEGORY_SUIT ){
                return( true );
            }
        }
    }
    
    return( false );
}

//-------------------------
// デフォルト設定
//-------------------------
void CBmpDotSettingData::setDefault( void ){
    // 一旦クリア
    clear();

    // オプションは全て有効（※とりあえず）
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_bArrOptionValid[i] = true;
    }
    
    // 各スロットをインデックス毎に登録
    int at = 0;
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        if( at >= BMP_DOT_SETTING_SLOT_DATA_MAX ){
            LOGE( "@ CBmpDotSettingData::setDefault: BUF SHORTAGE\n" );
            return;
        }

        int numSlot = CBDConst::GetNumSlotIndex( (eBD_SLOT)i );

        for( int j=0; j<numSlot; j++ ){
            CBmpDotSettingSlotData* pSlot = getSlotAt( at++ );
            pSlot->clear();
            pSlot->setSlot( (eBD_SLOT)i );
            pSlot->setSlotIndex( j );
        }
    }
}

//-------------------------
// ランダム設定：縦横
//-------------------------
void CBmpDotSettingData::setRandomAdjForHV( void ){
    m_nRateAdjustH = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
    m_nRateAdjustV = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
}

//-------------------------
// ランダム設定：テンション調整
//-------------------------
void CBmpDotSettingData::setRandomAdjForT( void ){
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );
        
        int temp = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
        pSlot->setRateAdjForT( temp );
    }
}

//-------------------------
// ランダム設定：サイズ調整
//-------------------------
void CBmpDotSettingData::setRandomAdjForS( void ){
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );
        
        int temp = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
        pSlot->setRateAdjForS( temp );
    }
}

//-------------------------
// ランダム設定：傾け
//-------------------------
void CBmpDotSettingData::setRandomAdjForAngle( void ){
    m_nRateAngleLR = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
    m_nRateAngleUD = CRand::GetRand( 2*BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE;
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpDotSettingData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 14 + eBD_OPTION_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAt( id++, "ID FOR FACE", &m_nIdForFace, eEDIT_VALUE_TYPE_INT32, -1, 9999 );
    pMenu->setItemAt( id++, "ID FOR BODY", &m_nIdForBody, eEDIT_VALUE_TYPE_INT32, -1, 9999 );
    pMenu->setItemAt( id++, "ID FOR SUIT", &m_nIdForSuit, eEDIT_VALUE_TYPE_INT32, -1, 999999 );
    
    pMenu->setItemAtAsLabel( id++, "EMO", &m_eEmo, eEDIT_VALUE_TYPE_INT32,
                            eBD_EMO_INVALID, eBD_EMO_MAX-1, g_pArrLabelBdEmo );
    
    pMenu->setItemAtAsLabel( id++, "FORM", &m_eForm, eEDIT_VALUE_TYPE_INT32,
                            eBD_FORM_INVALID, eBD_FORM_MAX-1, g_pArrLabelBdForm );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "RATE ADJ-H", &m_nRateAdjustH, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE ADJ-V", &m_nRateAdjustV, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE ANG-LR", &m_nRateAngleLR, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE ANG-UD", &m_nRateAngleUD, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    
    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "RATE VX", &m_nRateVX, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE VY", &m_nRateVY, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE AX", &m_nRateAX, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE AY", &m_nRateAY, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "MOVE COUNT", &m_nMoveCount, eEDIT_VALUE_TYPE_INT32, 0, 999 );

    pMenu->setSeparatorAt( id, true );

    // オプション
    pMenu->setItemAtAsBool( id++, "OPT: UpBaseApron", &m_bArrOptionValid[eBD_OPTION_UpBaseApron] );
    pMenu->setItemAtAsBool( id++, "OPT: LwBaseApron", &m_bArrOptionValid[eBD_OPTION_LwBaseApron] );
    pMenu->setItemAtAsBool( id++, "OPT: FcNoseBridge", &m_bArrOptionValid[eBD_OPTION_FcNoseBridge] );
    pMenu->setItemAtAsBool( id++, "OPT: FcGlasses", &m_bArrOptionValid[eBD_OPTION_FcGlasses] );
    pMenu->setItemAtAsBool( id++, "OPT: MoTooth", &m_bArrOptionValid[eBD_OPTION_MoTooth] );
    pMenu->setItemAtAsBool( id++, "OPT: MoLip", &m_bArrOptionValid[eBD_OPTION_MoLip] );
    pMenu->setItemAtAsBool( id++, "OPT: MoDrool", &m_bArrOptionValid[eBD_OPTION_MoDrool] );
    pMenu->setItemAtAsBool( id++, "OPT: EyLid", &m_bArrOptionValid[eBD_OPTION_EyLid] );
    pMenu->setItemAtAsBool( id++, "OPT: EyLashUp", &m_bArrOptionValid[eBD_OPTION_EyLashUp] );
    pMenu->setItemAtAsBool( id++, "OPT: EyLashLow", &m_bArrOptionValid[eBD_OPTION_EyLashLow] );
    pMenu->setItemAtAsBool( id++, "OPT: EyShadowUp", &m_bArrOptionValid[eBD_OPTION_EyShadowUp] );
    pMenu->setItemAtAsBool( id++, "OPT: EyShadowLow", &m_bArrOptionValid[eBD_OPTION_EyShadowLow] );
    pMenu->setItemAtAsBool( id++, "OPT: EyMole", &m_bArrOptionValid[eBD_OPTION_EyMole] );
    pMenu->setItemAtAsBool( id++, "OPT: EyTear", &m_bArrOptionValid[eBD_OPTION_EyTear] );
    pMenu->setItemAtAsBool( id++, "OPT: HdHairMesh", &m_bArrOptionValid[eBD_OPTION_HdHairMesh] );
    pMenu->setItemAtAsBool( id++, "OPT: HdHairBand", &m_bArrOptionValid[eBD_OPTION_HdHairBand] );

    // 確定
    pMenu->fixMenu();
}
