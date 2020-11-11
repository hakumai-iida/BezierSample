/*+----------------------------------------------------------------+
  | Title:      BmpDotSettingData.cpp [共通環境]
  | Comment:    BMPドット設定データ（※各スロットのBDPDを指定するデータ）
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
// フォルダ名
#define BDS_FOLDER_NAME     "bds"

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// UID
#define RAND_PART_CAND_NUM      6

static int __rand_uid[] = {
    1004,       // type04
    1003,       // type03
    1002,       // type02
    1001,       // type01
    100,        // goth
    101,        // miku
};

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
    m_bFlipH = false;
    m_bFlipV = false;
    m_nRateScale = BEZIER_SCALE_RATE;
    m_nRateRot = 0;

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

    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        m_nArrOptionValid[i] = 0x0;
    }

    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        m_oArrSlot[i].clear();
    }
    
    // 確定値リセット
    resetFixed();
    
    // 差し替え値のリセット
    resetReplaced();
}

//----------------
// 調整値リセット
//----------------
void CBmpDotSettingData::resetFixed( void ){
    m_bUseDefaultForSuit = false;
    
    m_bFlipHFixed = false;
    m_bFlipVFixed = false;
    m_bScaleFixed = false;
    m_bRotFixed = false;

    m_bIdForFaceFixed = false;
    m_bIdForBodyFixed = false;
    m_bIdForSuitFixed = false;
    m_bEmoFixed = false;
    m_bFormFixed = false;

    m_bAdjustHFixed = false;
    m_bAdjustVFixed = false;
    m_bAdjustTFixed = false;
    m_bAdjustSFixed = false;
    m_bAngleLRFixed = false;
    m_bAngleUDFixed = false;
}

//-------------------
// 差し替え値のリセット
//-------------------
void CBmpDotSettingData::resetReplaced( void ){
    m_nReplaceIdForUp = BD_UID_INVALID;
    m_nReplaceIdForLow = BD_UID_INVALID;
}
 
//------------------------------
// コピー
//------------------------------
void CBmpDotSettingData::copy( CBmpDotSettingData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }

    m_bFlipH = pData->m_bFlipH;
    m_bFlipV = pData->m_bFlipV;
    m_nRateScale = pData->m_nRateScale;
    m_nRateRot = pData->m_nRateRot;

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

//------------------------------
// スロットオプション設定
//------------------------------
void CBmpDotSettingData::setOptionForSlotValidAt( eBD_SLOT slot, int slotIndex, int at, bool flag ){
    // データの取得（※無効は無視）
    CBmpDotSettingSlotData* pData = searchSlot( slot, slotIndex );
    if( pData == NULL ){
        // 無効値でのアクセスも多いのでログはオフ
        //LOGE( "@ CBmpDotSettingData::setOptionForSlotValid: INVALID: slot=%d, slotIndex=%d, at=%d\n", slot, slotIndex, at );
        return;
    }

    pData->setOptionForSlotValidAt( at, flag  );
}

//------------------------------
// スロットオプションが有効か？
//------------------------------
bool CBmpDotSettingData::isOptionForSlotValidAt( eBD_SLOT slot, int slotIndex, int at ){
    // データの取得（※無効は無視）
    CBmpDotSettingSlotData* pData = searchSlot( slot, slotIndex );
    if( pData == NULL ){
        // 無効値でのアクセスも多いのでログはオフ
        //LOGE( "@ CBmpDotSettingData::isOptionForSlotValidAt: INVALID: slot=%d, slotIndex=%d, at=%d\n", slot, slotIndex, at );
        return( false );
    }

    return( pData->isOptionForSlotValidAt( at ) );
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
    m_bFlipH = (pIB->readUint8() != 0);
    m_bFlipV = (pIB->readUint8() != 0);
    m_nRateScale = pIB->readInt16();
    m_nRateRot = pIB->readInt16();

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

    // オプション
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
    pOB->writeUint8( (m_bFlipH)? 1: 0 );
    pOB->writeUint8( (m_bFlipV)? 1: 0 );
    pOB->writeInt16( (short)m_nRateScale );
    pOB->writeInt16( (short)m_nRateRot );

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

    // オプション
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        pOB->writeUint8( m_nArrOptionValid[i] );
    }

    // スロットデータの要素数の出力（※読み込み時に利用する）
    pOB->writeInt16( BMP_DOT_SETTING_SLOT_DATA_MAX );
    
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        m_oArrSlot[i].write( pOB );
    }
}

//-------------------
// ワーク設定読み込み
//-------------------
bool CBmpDotSettingData::loadFile( const char* pName ){
    return( load( pName, BDS_FOLDER_NAME ) );
}

//-------------------
// ワーク設定書き込み
//-------------------
bool CBmpDotSettingData::saveFile( const char* pName, bool isTemporary ){
    return( save( pName, isTemporary ) );
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

    // 腹
    int blUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
    
    // 上半身（※腹に準ずる）
    int upUid = blUid;
    
    // うで（※上半身に準ずる）
    int amUidForL = upUid;
    int amUidForR = upUid;

    // ひじ先（※腕に準ずる）
    int elUidForL = amUidForL;
    int elUidForR = amUidForR;
    
    // 下半身
    int lwUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];

    // 脚
    int lgUidForL = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
    int lgUidForR = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
    
    // ひざ下（※脚に準ずる）
    int knUidForL = lgUidForL;
    int knUidForR = lgUidForR;

    // 胴体のオプション（※負で無効）
    int bdOptionId = CRand::GetRand( RAND_PART_CAND_NUM+1 ) - 1;
    if( bdOptionId >= 0 ){ bdOptionId = __rand_uid[bdOptionId]; }

    // もみあげ
    int headSideUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];

    // 目の基本／目玉／まゆげ
    int eyeBaseUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
    int eyeBallUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
    int browsUid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];

    // シャッフル
    for( int i=0; i<BMP_DOT_SETTING_SLOT_DATA_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );

        eBD_SLOT slot = pSlot->getSlot();
        int slotIndex = pSlot->getSlotIndex();
        if( IS_BD_SLOT_VALID( slot ) ){
            // 基本設定
            eBD_CATEGORY category = CBDConst::GetCategoryForSlot( slot );
            eBD_EMO emo = eBD_EMO_BASE;
            eBD_FORM form = eBD_FORM_FRONT;
            eBD_DIR dir = (slotIndex>0)? eBD_DIR_R: eBD_DIR_L;
            int subId = 0;
            int uid = 0;

            switch( slot ){
            // 腹
            case eBD_SLOT_BlBase:
                uid = blUid;
                break;
                    
            // 胴体のオプション
            case eBD_SLOT_BdOptionA:
            case eBD_SLOT_BdOptionB:
                if( bdOptionId < 0 ){
                    slot = eBD_SLOT_INVALID;
                }else{
                    uid = bdOptionId;
                }
                break;
                    
            // 腹オプション
            case eBD_SLOT_BlOptionA:
            case eBD_SLOT_BlOptionB:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;
                    
            // 上半身
            case eBD_SLOT_UpBase:
            case eBD_SLOT_NkBase:
            case eBD_SLOT_NkCover:
                uid = upUid;
                break;

            // 上半身のオプション
            case eBD_SLOT_UpOptionA:
            case eBD_SLOT_UpOptionB:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // うで
            case eBD_SLOT_AmBase:
            case eBD_SLOT_AmCover:
                if( slotIndex > 0 ){
                    uid = amUidForR;
                }else{
                    uid = amUidForL;
                }
                break;

            // ひじ先
            case eBD_SLOT_ElBase:
            case eBD_SLOT_HnBase:
                if( slotIndex > 0 ){
                    uid = elUidForR;
                }else{
                    uid = elUidForL;
                }
                break;
                    
            // うでオプション
            case eBD_SLOT_AmOptionA:
            case eBD_SLOT_AmOptionB:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // 下半身
            case eBD_SLOT_LwBase:
                uid = lwUid;
                break;

            // 下半身のオプション
            case eBD_SLOT_LwOptionA:
            case eBD_SLOT_LwOptionB:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // 脚
            case eBD_SLOT_LgBase:
            case eBD_SLOT_LgCover:
                if( slotIndex > 0 ){
                    uid = lgUidForR;
                }else{
                    uid = lgUidForL;
                }
                break;

            // 脚のオプション
            case eBD_SLOT_LgOptionA:
            case eBD_SLOT_LgOptionB:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;
                    
            // 膝下
            case eBD_SLOT_KnBase:
            case eBD_SLOT_FoBase:
            case eBD_SLOT_FoGround:
                if( slotIndex > 0 ){
                    uid = knUidForR;
                }else{
                    uid = knUidForL;
                }
                break;

            // 頭
            case eBD_SLOT_HdBase:
            case eBD_SLOT_HdTop:
            case eBD_SLOT_HdTopCover:
            case eBD_SLOT_HdCap:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // もみあげ
            case eBD_SLOT_HdSide:
                uid = headSideUid;
                break;
                    
            // 前髪（※ない場合もある）
            case eBD_SLOT_HdFront:
                if( CRand::IsRandHappen( 1, 4 ) ){
                    slot = eBD_SLOT_INVALID;
                }else{
                    uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                }
                break;

            // 頭の飾り
            case eBD_SLOT_HdBand:
            case eBD_SLOT_HdRibbon:
            case eBD_SLOT_HdAccentTop:
            case eBD_SLOT_HdAccentSide:
            case eBD_SLOT_HdHairBackKnot:
            case eBD_SLOT_HdHairBack:
            case eBD_SLOT_HdHairSideKnot:
            case eBD_SLOT_HdHairSide:
            case eBD_SLOT_HdHairDownKnot:
            case eBD_SLOT_HdHairDown:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // 顔
            case eBD_SLOT_FcBase:
            case eBD_SLOT_FcUnder:
            case eBD_SLOT_FcUnderCover:
            case eBD_SLOT_FcEar:
            case eBD_SLOT_FcCheek:
            case eBD_SLOT_FcNose:
            case eBD_SLOT_MoBase:
                uid = __rand_uid[CRand::GetRand( RAND_PART_CAND_NUM )];
                break;

            // まゆ
            case eBD_SLOT_FcBrows:
                uid = browsUid;
                break;

            // 目の基本
            case eBD_SLOT_EyBase:
                uid = eyeBaseUid;
                break;

            // 目玉
            case eBD_SLOT_EyBall:
                uid = eyeBallUid;
                break;

            default:
                break;
            }

            // 設定
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
    m_nRateAdjustH = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
    m_nRateAdjustV = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
}

//-------------------------
// ランダム設定：テンション調整
//-------------------------
void CBmpDotSettingData::setRandomAdjForT( void ){
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );
        
        int temp = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
        pSlot->setRateAdjForT( temp );
    }
}

//-------------------------
// ランダム設定：サイズ調整
//-------------------------
void CBmpDotSettingData::setRandomAdjForS( void ){
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        CBmpDotSettingSlotData* pSlot = getSlotAt( i );
        
        int temp = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
        pSlot->setRateAdjForS( temp );
    }
}

//-------------------------
// ランダム設定：傾け
//-------------------------
void CBmpDotSettingData::setRandomAdjForAngle( void ){
    m_nRateAngleLR = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
    m_nRateAngleUD = CRand::GetRand( BEZIER_SCALE_RATE+1 ) - BEZIER_SCALE_RATE/2;
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpDotSettingData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 15 + eBD_OPTION_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsBool(id++, "FLIP-H", &m_bFlipH );
    pMenu->setItemAtAsBool(id++, "FLIP-V", &m_bFlipV );
    pMenu->setItemAt( id++, "RATE SCALE", &m_nRateScale, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE/10, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "RATE ROT", &m_nRateRot, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    
    pMenu->setSeparatorAt( id, true );

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
    pMenu->setItemAt( id++, "OPT: SlotA(NoUse)", &m_nArrOptionValid[eBD_OPTION_SlotA], eEDIT_VALUE_TYPE_INT32, 0, maxVal );
    pMenu->setItemAt( id++, "OPT: SlotB(NoUse)", &m_nArrOptionValid[eBD_OPTION_SlotB], eEDIT_VALUE_TYPE_INT32, 0, maxVal );

    // 確定
    pMenu->fixMenu();
}
