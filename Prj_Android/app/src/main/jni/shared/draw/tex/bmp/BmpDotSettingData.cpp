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
    m_nRateAdjustT = 0;
    m_nRateAdjustS = 0;
    m_nRateAngleLR = 0;
    m_nRateAngleUD = 0;
    
    m_nRateVX = 0;
    m_nRateVY = 0;
    m_nRateAX = 0;
    m_nRateAY = 0;
    m_nMoveCount = 0;

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_nArrOptionValid[i] = 0x0;
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
    m_nRateAdjustT = pData->m_nRateAdjustT;
    m_nRateAdjustS = pData->m_nRateAdjustS;
    m_nRateAngleLR = pData->m_nRateAngleLR;
    m_nRateAngleUD = pData->m_nRateAngleUD;
    
    m_nRateVX = pData->m_nRateVX;
    m_nRateVY = pData->m_nRateVY;
    m_nRateAX = pData->m_nRateAX;
    m_nRateAY = pData->m_nRateAY;
    m_nMoveCount = pData->m_nMoveCount;

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_nArrOptionValid[i] = pData->m_nArrOptionValid[i];
    }

    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
         m_oArrSlot[i].copy( &pData->m_oArrSlot[i] );
    }
}


//------------------------------
// オプション設定
//------------------------------
void CBmpDotSettingData::setOptionValid( eBD_OPTION option, int slotIndex, bool flag ){
    // 無効は無視
    if( !IS_BD_OPTION_VALID( option ) || !IS_BD_SLOT_INDEX_VALID( slotIndex ) ){
        LOGE( "@ CBmpDotSettingData::setOptionValid: INVALID: option=%d, slotIndex=%d\n", option, slotIndex );
        return;
    }
    
    int val = m_nArrOptionValid[option];
    if( flag ){
        val |= (1<<slotIndex);
    }else{
        val &= ~(1<<slotIndex);
    }
    m_nArrOptionValid[option] = val;
}

//------------------------------
// オプションが有効か？
//------------------------------
bool CBmpDotSettingData::isOptionValid( eBD_OPTION option, int slotIndex ){
    // 無効は無視
    if( !IS_BD_OPTION_VALID( option ) || !IS_BD_SLOT_INDEX_VALID( slotIndex ) ){
        LOGE( "@ CBmpDotSettingData::isOptionValid: INVALID: option=%d, slotIndex=%d\n", option, slotIndex );
        return( false );
    }

    int val = m_nArrOptionValid[option];
    return( (val & (1<<slotIndex)) != 0x0 );
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
    m_nRateAdjustT = pIB->readInt16();
    m_nRateAdjustS = pIB->readInt16();
    m_nRateAngleLR = pIB->readInt16();
    m_nRateAngleUD = pIB->readInt16();
    
    m_nRateVX = pIB->readInt16();
    m_nRateVY = pIB->readInt16();
    m_nRateAX = pIB->readInt16();
    m_nRateAY = pIB->readInt16();
    m_nMoveCount = pIB->readInt16();

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_nArrOptionValid[i] = pIB->readUint8();
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
    pOB->writeInt16( (short)m_nRateAdjustT );
    pOB->writeInt16( (short)m_nRateAdjustS );
    pOB->writeInt16( (short)m_nRateAngleLR );
    pOB->writeInt16( (short)m_nRateAngleUD );

    pOB->writeInt16( (short)m_nRateVX );
    pOB->writeInt16( (short)m_nRateVY );
    pOB->writeInt16( (short)m_nRateAX );
    pOB->writeInt16( (short)m_nRateAY );
    pOB->writeInt16( (short)m_nMoveCount );

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        pOB->writeUint8( m_nArrOptionValid[i] );
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
        // スロットインデックス分１を立てる
        int val = 0;
        
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            val |= (1<<j);
        }
        m_nArrOptionValid[i] = val;
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
void CBmpDotSettingData::setRandomPart( void ){
    // 用心にデフォルト
    setDefault();

    int eyeBaseUid = CRand::GetRand( 2 );
    int eyeBallUid = CRand::GetRand( 2 );
    int browseUid = CRand::GetRand( 2 );
    int headSideUid = CRand::GetRand( 2 );
    int upUid = CRand::GetRand( 2 );
    int lowUid = CRand::GetRand( 2 );
    int armUid = CRand::GetRand( 2 );
    int legUid = CRand::GetRand( 2 );
    int optionId = CRand::GetRand( 3 ) - 1;

    // シャッフル
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );

        eBD_SLOT slot = pSlot->getSlot();
        int slotIndex = pSlot->getSlotIndex();
        if( IS_BD_SLOT_VALID( slot ) ){
            eBD_CATEGORY category = CBDConst::GetCategoryForSlot( slot );
            eBD_EMO emo = eBD_EMO_BASE;
            eBD_FORM form = eBD_FORM_FRONT;
            eBD_DIR dir = (slotIndex>0)? eBD_DIR_R: eBD_DIR_L;
            int subId = 0;
            int uid = 0;

            switch( slot ){
            case eBD_SLOT_BlBase:
            case eBD_SLOT_BlJointUpBody:
            case eBD_SLOT_BlJointLowBody:
            case eBD_SLOT_BlOptionA:
            case eBD_SLOT_BlOptionB:
            case eBD_SLOT_UpBase:
            case eBD_SLOT_UpJointNeck:
            case eBD_SLOT_UpJointSholder:
                uid = upUid;
                break;

            case eBD_SLOT_BdOptionA:
            case eBD_SLOT_BdOptionB:
            case eBD_SLOT_BdOptionJointA:
            case eBD_SLOT_BdOptionJointB:
                if( optionId < 0 ){
                    slot = eBD_SLOT_INVALID;
                }else{
                    uid = optionId;
                }
                break;

            case eBD_SLOT_UpOptionA:
            case eBD_SLOT_UpOptionB:
            case eBD_SLOT_NkBase:
                uid = CRand::GetRand( 2 );
                break;

            case eBD_SLOT_AmBase:
            case eBD_SLOT_AmCover:
            case eBD_SLOT_AmJointElbow:
            case eBD_SLOT_ElBase:
            case eBD_SLOT_ElJointWrist:
            case eBD_SLOT_HnBase:
                uid = armUid;
                break;
                    
            case eBD_SLOT_AmOptionA:
            case eBD_SLOT_AmOptionB:
                uid = CRand::GetRand( 2 );
                break;
                    
            case eBD_SLOT_LwBase:
                    
            case eBD_SLOT_LwJointLeg:
            case eBD_SLOT_LwOptionA:
            case eBD_SLOT_LwOptionB:
                uid = lowUid;

            case eBD_SLOT_LgBase:
            case eBD_SLOT_LgCover:
            case eBD_SLOT_LgJointKnee:
            case eBD_SLOT_LgOptionA:
            case eBD_SLOT_LgOptionB:
            case eBD_SLOT_KnBase:
            case eBD_SLOT_KnJointAnkle:
            case eBD_SLOT_FoBase:
            case eBD_SLOT_FoGround:
                uid = legUid;
                break;
                    
            case eBD_SLOT_FcBase:
            case eBD_SLOT_FcHairLine:
            case eBD_SLOT_FcEar:
            case eBD_SLOT_FcCheek:
            case eBD_SLOT_FcNose:
            case eBD_SLOT_FcGlasses:
            case eBD_SLOT_FcDeco:
            case eBD_SLOT_MoBase:
                uid = CRand::GetRand( 2 );
                break;
                    
            case eBD_SLOT_FcBrows:
                uid = browseUid;
                break;

            case eBD_SLOT_EyBase:
                uid = eyeBaseUid;
                break;

            case eBD_SLOT_EyBall:
                uid = eyeBallUid;
                break;

            case eBD_SLOT_HdBase:
                uid = CRand::GetRand( 2 );
                break;

            case eBD_SLOT_HdCover:
            case eBD_SLOT_HdTop:
                uid = CRand::GetRand( 2 );
                break;

            case eBD_SLOT_HdSide:
                uid = headSideUid;
                break;
                    
            case eBD_SLOT_HdFront:
                if( CRand::IsRandHappen( 1, 4 ) ){
                    slot = eBD_SLOT_INVALID;
                }else{
                    uid = CRand::GetRand( 2 );
                }
                break;
                    
            case eBD_SLOT_HdCap:
            case eBD_SLOT_HdRibbon:
            case eBD_SLOT_HdAccentTop:
            case eBD_SLOT_HdAccentSide:
            case eBD_SLOT_HdHairSideKnot:
            case eBD_SLOT_HdHairSide:
            case eBD_SLOT_HdHairBackKnot:
            case eBD_SLOT_HdHairBack:
                uid = CRand::GetRand( 2 );
                break;

            default:
                break;
            }

            pSlot->setSlot( slot );
            pSlot->setSlotIndex( slotIndex );
            pSlot->setCategory( category );
            pSlot->setEmo( emo );
            pSlot->setForm( form );
            pSlot->setSubId( subId );
            pSlot->setDir( dir );
            pSlot->setUid( uid );
        }
    }
    
    // オプション
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        int val = 0;
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            if( CRand::IsRandHappen( 1, 2 ) ){
                val |= (1<<j);
            }
        }
        m_nArrOptionValid[i] = val;
    }
}

//-------------------------
// ランダム設定：スーツ
//-------------------------
void CBmpDotSettingData::setRandomPartForSuit( void ){
    //
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
    pMenu->setItemNum( 16 + eBD_OPTION_MAX );
    
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
    pMenu->setItemAt( id++, "RATE ADJ-T", &m_nRateAdjustT, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
    pMenu->setItemAt( id++, "RATE ADJ-S", &m_nRateAdjustS, eEDIT_VALUE_TYPE_INT32, -BEZIER_SCALE_RATE, BEZIER_SCALE_RATE );
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
    int maxVal = (1<<BD_SLOT_INDEX_MAX) - 1;
    pMenu->setItemAt( id++, "OPT: EyLid", &m_nArrOptionValid[eBD_OPTION_EyLid], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyLashUp", &m_nArrOptionValid[eBD_OPTION_EyLashUp], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyLashLow", &m_nArrOptionValid[eBD_OPTION_EyLashLow], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyShadowUp", &m_nArrOptionValid[eBD_OPTION_EyShadowUp], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyShadowLow", &m_nArrOptionValid[eBD_OPTION_EyShadowLow], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyMole", &m_nArrOptionValid[eBD_OPTION_EyMole], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: EyTear", &m_nArrOptionValid[eBD_OPTION_EyTear], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: MoTooth", &m_nArrOptionValid[eBD_OPTION_MoTooth], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: MoLip", &m_nArrOptionValid[eBD_OPTION_MoLip], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: MoDrool", &m_nArrOptionValid[eBD_OPTION_MoDrool], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: FcRough", &m_nArrOptionValid[eBD_OPTION_FcRough], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: FcAngry", &m_nArrOptionValid[eBD_OPTION_FcAngry], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: FcSad", &m_nArrOptionValid[eBD_OPTION_FcSad], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: FcShy", &m_nArrOptionValid[eBD_OPTION_FcShy], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: FcBlue", &m_nArrOptionValid[eBD_OPTION_FcBlue], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: HdHairMesh", &m_nArrOptionValid[eBD_OPTION_HdHairMesh], eEDIT_VALUE_TYPE_INT32, 0, maxVal );

#if 1
    pMenu->setItemAt( id++, "OPT: FcEarMike", &m_nArrOptionValid[eBD_OPTION_FcEarMike], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
#endif

    // 確定
    pMenu->fixMenu();
}
