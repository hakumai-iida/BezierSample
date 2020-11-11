/*+----------------------------------------------------------------+
  |	Title:		BmpDotMgr.cpp [共通環境]
  |	Comment:	BMP画素管理（※ファイル単位で管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotMgr.hpp"

#include "draw/bezier/adjust/DelayLog.hpp"
#include "draw/bezier/data/LayerOrderList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// データ格納フォルダ
#define BDP_FOLDER_NAME        "bdp"

// スケールの一律適用
//#define APPLY_SCALE

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ファイル名リスト（※あるカテゴリに対するデータを複数ファイルでもつことも可能）
//-----------------------------------------------------------------
const char* CBmpDotMgr::s_pArrBmpDotPackName[] = {
    //--------------------------
    // 本番想定データ：[1000-9999]
    //--------------------------
    // type04
    "AF1004_BASE.bdp",
    "AB1004_BASE.bdp",

    // type03
    "AF1003_BASE.bdp",
    "AB1003_BASE.bdp",

    // type02
    "AF1002_BASE.bdp",
    "AB1002_BASE.bdp",

    // type01
    "AF1001_BASE.bdp",
    "AB1001_BASE.bdp",

    //--------------------------
    // テストデータ：[0100-0999]
    //--------------------------
#if 1
    // goth
    "AF0100_BASE.bdp",
    "AB0100_BASE.bdp",

    // miku
    "AF0101_BASE.bdp",
    "AB0101_BASE.bdp",

    // suit
    "AS000102_BASE.bdp",    // sheep
    "AS000101_BASE.bdp",    // negi
    "AS000100_BASE.bdp",    // box
#endif
    
    NULL
};

#ifdef APPLY_SCALE
// サイズ調整用（※パーツ内の要素を一律で補正する場合）
static int __apply_scale_rate[] = {
    10000,
    10000,
    
     9600,
     9000,

     9800,
     9100,
    
     9200,
     9200,
     9200,
};
#endif

int CBmpDotMgr::s_nNumPack;
CBmpDotPack* CBmpDotMgr::s_oArrBmpDotPack;

CBmpDotPartData* CBmpDotMgr::s_pDefaultBdpd_HdBand;
CBmpDotPartData* CBmpDotMgr::s_pDefaultBdpd_HdHairBackKnot;
CBmpDotPartData* CBmpDotMgr::s_pDefaultBdpd_HdHairSideKnot;
CBmpDotPartData* CBmpDotMgr::s_pDefaultBdpd_HdHairDownKnot;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CBmpDotMgr::GetAllocCellNum( void ){
    int num = 0;
    
    //---------------------------------
    // CBmpDotPartData::Init()
    // CBmpDotData::Init()
    //---------------------------------
    num += CBmpDotPartData::GetAllocCellNum();
    num += CBmpDotData::GetAllocCellNum();

    //------------------------------------------------------
    // s_oArrBmpDotPack = new CBmpDotPack[GetBmpDotPackNum()];
    //------------------------------------------------------
    num += 1;
    
    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CBmpDotMgr::GetAllocSizeK( void ){
    int size = 0;
    
    //---------------------------------
    // CBmpDotPartData::Init()
    // CBmpDotData::Init()
    //---------------------------------
    size += CBmpDotPartData::GetAllocSize();
    size += CBmpDotData::GetAllocSize();

    //------------------------------------------------------
    // s_oArrBmpDotPack = new CBmpDotPack[GetBmpDotPackNum()];
    //------------------------------------------------------
    size += sizeof(CBmpDotPack) * GetBmpDotPackNum();
    
    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//------------------------
// 初期化
//------------------------
bool CBmpDotMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBmpDotMgr::OnCreate()\n" );
#endif
    
    // Bmp画素パーツデータ管理初期化
    if( ! CBmpDotPartData::Init() ){
        LOGE( "@ CBmpDotPartData::Init(): FAILED!!\n" );
        env_push_error_log( "[CBmpDotPartData::Init()]に失敗しました。\n" );
        return( false );
    }

    // Bmp画素データ管理初期化
    if( ! CBmpDotData::Init() ){
        LOGE( "@ CBmpDotData::Init(): FAILED!!\n" );
        env_push_error_log( "[CBmpDotData::Init()]に失敗しました。\n" );
        return( false );
    }

    // データ構築
    if( ! CreateAll() ){
        LOGE( "@ CreateAll(): FAILED!!\n" );
        env_push_error_log( "[CBmpDotMgr::CreateAll()]に失敗しました。\n" );
        return( false );
    }
    
    // デフォルトデータ構築
    if( ! CreateDefault() ){
        LOGE( "@ CreateDefault(): FAILED!!\n" );
        env_push_error_log( "[CBmpDotMgr::CreateDefault()]に失敗しました。\n" );
        return( false );
    }

	return( true );
}

//------------------------
// 終了
//------------------------
void CBmpDotMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBmpDotMgr::OnDestroy()\n" );
#endif

    // 解放
    SAFE_DELETE_ARRAY( s_oArrBmpDotPack );
    
    // 開放
    CBmpDotPartData::Free( s_pDefaultBdpd_HdBand );
    s_pDefaultBdpd_HdBand = NULL;

    CBmpDotPartData::Free( s_pDefaultBdpd_HdHairBackKnot );
    s_pDefaultBdpd_HdHairBackKnot = NULL;
    
    CBmpDotPartData::Free( s_pDefaultBdpd_HdHairSideKnot );
    s_pDefaultBdpd_HdHairSideKnot = NULL;
    
    CBmpDotPartData::Free( s_pDefaultBdpd_HdHairDownKnot );
    s_pDefaultBdpd_HdHairDownKnot = NULL;

    // 終了
    CBmpDotData::Exit();
    CBmpDotPartData::Exit();
}

//----------------------
// データ数取得
//----------------------
int CBmpDotMgr::GetBmpDotPackNum( void ){
    // 未初期化ならファイル数の検出
    if( s_nNumPack <= 0 ){
        s_nNumPack = 0;
        while( s_pArrBmpDotPackName[s_nNumPack] != NULL ){
            s_nNumPack++;
        }
    }
    
    return( s_nNumPack );
}

//---------------------------
// ファイル名取得
//---------------------------
const char* CBmpDotMgr::GetBmpDotPackNameAt( int at ){
    int numPack = GetBmpDotPackNum();
    
    // 無効は無視
    if( at < 0 || at >= numPack ){
        LOGE( "@ CBmpDotMgr::GetBmpDotPackNameAt: INVALID at=%d/%d\n", at, numPack );
        return( NULL );
    }
    
    return( s_pArrBmpDotPackName[at] );
}

//------------------------------------
// データ取得
//------------------------------------
CBmpDotPack* CBmpDotMgr::GetBmpDotPackAt( int at ){
    int numPack = GetBmpDotPackNum();

    // 無効は無視
    if( at < 0 || at >= numPack ){
        LOGE( "@ CBmpDotMgr::GetBmpDotPackAt: INVALID at=%d/%d\n", at, numPack );
        return( NULL );
    }

    return( &s_oArrBmpDotPack[at] );
}

//------------------------------------
// データ検索
//------------------------------------
CBmpDotData* CBmpDotMgr::SearchBmpDotData( stBD_PART_SEARCH_PARAM* pParam ){
    int numPack = GetBmpDotPackNum();
    
    for( int i=0; i<numPack; i++ ){
        CBmpDotData* pData = s_oArrBmpDotPack[i].searchBmpDotData( pParam );
        if( pData != NULL ){
            return( pData );
        }
    }

    // ここまできたら未発見（※ない場合もありうる）
    return( NULL );
}


//------------------------------------
// パーツ検索
//------------------------------------
CBmpDotPartData* CBmpDotMgr::SearchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParam ){
    int numPack = GetBmpDotPackNum();
    
    for( int i=0; i<numPack; i++ ){
        CBmpDotPartData* pData = s_oArrBmpDotPack[i].searchBmpDotPartData( pParam );
        if( pData != NULL ){
            return( pData );
        }
    }
    
    return( NULL );
}

//------------------------------------------------------------------
// パーツ検索（※デフォルト参照付き）
// 指定のパーツが見つからない場合、検索パラメータをデフォルトに置き換えつつ検索する
//------------------------------------------------------------------
// デフォルトサブIDの検索
#define SEARCH_DEFAULT_SUB_ID_THEN_RETURN_IF_FOUND                  \
    if( pParam->subId != BD_SUB_ID_DEFAULT ){                       \
        pParamTmp->subId = BD_SUB_ID_DEFAULT;                       \
        pData = SearchBmpDotPartData( pParamTmp );                  \
        if( pData != NULL ){                                        \
        DEF_HIT_LOG( "SUB_ID" );                                    \
            return( pData );                                        \
        }                                                           \
        pParamTmp->subId = pParam->subId;                           \
    }

// デフォルト方向[DIR]の検索
#define SEARCH_DEFAULT_DIR_THEN_RETURN_IF_FOUND                     \
    if( pParam->dir != eBD_DIR_DEFAULT ){                           \
        pParamTmp->dir = eBD_DIR_DEFAULT;                           \
        pData = SearchBmpDotPartData( pParamTmp );                  \
        if( pData != NULL ){                                        \
        DEF_HIT_LOG( "DIR" );                                       \
            return( pData );                                        \
        }                                                           \
        SEARCH_DEFAULT_SUB_ID_THEN_RETURN_IF_FOUND;                 \
        pParamTmp->dir = pParam->dir;                               \
    }

// デフォルト感情[EMO]の検索
#define SEARCH_DEFAULT_EMO_THEN_RETURN_IF_FOUND                     \
    if( pParam->emo != eBD_EMO_DEFAULT ){                           \
        pParamTmp->emo = eBD_EMO_DEFAULT;                           \
        pData = SearchBmpDotPartData( pParamTmp );                  \
        if( pData != NULL ){                                        \
            DEF_HIT_LOG( "EMO" );                                   \
            return( pData );                                        \
        }                                                           \
        SEARCH_DEFAULT_DIR_THEN_RETURN_IF_FOUND;                    \
        pParamTmp->emo = pParam->emo;                               \
    }

// デフォルトの検索
#define SEARCH_DEFAULT_BDPD_THEN_RETURN_IF_FOUND                    \
    pData = CBmpDotMgr::GetDefaultBmpDotPartData( pParam->slot );   \
    if( pData != NULL ){                                            \
        DEF_HIT_LOG( "DEFAULT" );                                   \
        return( pData );                                            \
    }

// ヒット時のログ
#ifdef BMP_GEN_LOG
#define DEF_HIT_LOG( _pLabel )                                                                                      \
    LOGD( "@     +DEF HIT(%s): ctg=%s/%d, uid=%08d/%08d, emo=%s/%d, f=%s/%d, slt=%s/%d, sid=%d/%d, dir=%s/%d\n",    \
          (_pLabel),                                                                                                \
          g_pArrLabelBdCategory[pParamTmp->category], pParam->category,                                             \
          pParamTmp->uid, pParam->uid,                                                                              \
          g_pArrLabelBdEmo[pParamTmp->emo], pParam->emo,                                                            \
          g_pArrLabelBdForm[pParamTmp->form], pParam->form,                                                         \
          g_pArrLabelBdSlot[pParamTmp->slot], pParam->slot,                                                         \
          pParamTmp->subId, pParam->subId,                                                                          \
          g_pArrLabelBdDir[pParamTmp->dir], pParam->dir )
#else
#define DEF_HIT_LOG( _pLabel )                                                                                      
#endif

CBmpDotPartData* CBmpDotMgr::SearchBmpDotPartDataWithDefault( stBD_PART_SEARCH_PARAM* pParam, stBD_PART_SEARCH_PARAM* pParamOut ){
    CBmpDotPartData* pData;
    stBD_PART_SEARCH_PARAM paramTmp;

#if 1
    // スーツの指定があれば優先的に検索する
    if( IS_BD_UID_VALID( pParam->suitUid ) ){
        // パラメータのコピー
        paramTmp = *pParam;
        
        // 検索条件をスーツに変更
        paramTmp.category = eBD_CATEGORY_SUIT;
        paramTmp.uid = pParam->suitUid;
        paramTmp.subId = 0;

        // 検索
        pData = SearchBmpDotPartData( &paramTmp );
        
        // 対象が右要素であれば左要素も候補となる
        if( pData == NULL && paramTmp.dir == eBD_DIR_R ){
            paramTmp.dir = eBD_DIR_L;
            pData = SearchBmpDotPartData( &paramTmp );
        }

        // 見つかったら返す
        if( pData != NULL ){
            // パラメータの指定があれば出力
            if( pParamOut != NULL ){
                *pParamOut = paramTmp;
            }
            
            return( pData );
        }
    }
#endif
    
    //----------------------------------------------
    // 指定条件での検索（※見つかったら返す）
    //----------------------------------------------
    pData = SearchBmpDotPartData( pParam );
    if( pData != NULL ){
        // パラメータの指定があれば出力
        if( pParamOut != NULL ){
            *pParamOut = *pParam;
        }

        return( pData );
    }

    //----------------------------------------------
    // ここまできたらパラメータを差し替えつつ検索する
    //----------------------------------------------
    stBD_PART_SEARCH_PARAM* pParamTmp;
    if( pParamOut != NULL ){
        pParamTmp = pParamOut;
    }else{
        pParamTmp = &paramTmp;
    }

    // 検索パラメータをワークにコピー
    *pParamTmp = *pParam;
    
    //-------------------------------------------
    // サブIDのデフォルト確認
    //-------------------------------------------
    SEARCH_DEFAULT_SUB_ID_THEN_RETURN_IF_FOUND;

    //-------------------------------------------
    // 向き[eBD_DIR]のデフォルト確認
    //-------------------------------------------
    SEARCH_DEFAULT_DIR_THEN_RETURN_IF_FOUND;

    //-------------------------------------------------------------------------------
    // フォーム[eBD_FORM]のデフォルトは見ない
    //（※フォームの素材は流用性がない想定 ＝ FRONTがQUATERを代替すると見た目に変になる可能性がある）
    //-------------------------------------------------------------------------------

    //-------------------------------------------
    // 感情[eBD_EMO]のデフォルト確認
    //-------------------------------------------
    SEARCH_DEFAULT_EMO_THEN_RETURN_IF_FOUND;

    //------------------------------------------------------------------------
    // サブIDは対象のパーツがある前提で指定されるので、この時点で検索がヒットしないのは何か変
    //------------------------------------------------------------------------
    if( pParam->subId != 0 ){
        // とりあえずログを吐いておく
        const char* pLabel = "UNKNOWN";
        if( IS_BD_SLOT_VALID( pParam->slot ) ){ pLabel = g_pArrLabelBdSlot[pParam->slot]; }
        LOGE( "@ CBmpDotMgr::SearchBmpDotPartDataWithDefault: NOT FOUND ctg=%d, uid=%08d, emo=%d, f=%d, slt=%s(%d), sid=%d, dir=%d\n",
              pParam->category, pParam->uid, pParam->emo, pParam->form, pLabel, pParam->slot, pParam->subId, pParam->dir );
    }
    
    //-----------------------------
    // システムデフォルトの参照
    //-----------------------------
    SEARCH_DEFAULT_BDPD_THEN_RETURN_IF_FOUND;

    // ここまできたら未発見
    return( NULL );
}

//-------------------
// 読み込み
//-------------------
bool CBmpDotMgr::LoadBdpAt( int at, const char* pName ){
    CBmpDotPack* pPack = GetBmpDotPackAt( at );
    if( pPack != NULL && pPack->load( pName, BDP_FOLDER_NAME ) ){
        return( true );
    }
    
    return( false );
}

//-------------------
// 保存
//-------------------
bool CBmpDotMgr::SaveBdpAt( int at, const char* pName, bool isTemporary ){
    CBmpDotPack* pPack = GetBmpDotPackAt( at );
    
    if( pPack != NULL && pPack->save( pName, isTemporary ) ){
        return( true );
    }
    
    return( false );
}

//-------------------
// 全保存
//-------------------
bool CBmpDotMgr::SaveBdpAll( void ){
    int num = GetBmpDotPackNum();
    
    for( int i=0; i<num; i++ ){
        CBmpDotPack* pPack = GetBmpDotPackAt( i );
    
        if( pPack != NULL ){
            if( ! pPack->save( s_pArrBmpDotPackName[i], false ) ){
                return( false );
            }
        }
    }
    
    return( true );
}

//--------------------------------------------------------------------------
// デフォルトBDPD関連
// 一部のスロットは画像要素をもたず機械的な構成で事足りるため、デフォルトデータが成り立つ
//--------------------------------------------------------------------------
CBmpDotPartData* CBmpDotMgr::GetDefaultBmpDotPartData( eBD_SLOT slot ){
    switch( slot ){
    // 髪：ヘアバンド
    case eBD_SLOT_HdBand:
        return( s_pDefaultBdpd_HdBand );

    // 髪：ポニーテールの結び
    case eBD_SLOT_HdHairBackKnot:
        return( s_pDefaultBdpd_HdHairBackKnot );
    
    // 髪：ツインテールの結び
    case eBD_SLOT_HdHairSideKnot:
        return( s_pDefaultBdpd_HdHairSideKnot );
            
    // 髪：おさげの結び
    case eBD_SLOT_HdHairDownKnot:
        return( s_pDefaultBdpd_HdHairDownKnot );
            
    default:
        break;
    }

    return( NULL );
}

//------------------
// 全データ作成
//------------------
bool CBmpDotMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpDotMgr::CreateAll()\n" );
#endif
    
    int numPack = GetBmpDotPackNum();
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_BMP_DOT );
    s_oArrBmpDotPack = new CBmpDotPack[numPack];
    CMemMgr::PopTargetField();
    
    // パック読み込み
    for( int i=0; i<numPack; i++ ){
        LoadBdpAt( i, s_pArrBmpDotPackName[i] );

#ifdef APPLY_SCALE
        int rateScale = __apply_scale_rate[i];
        if( rateScale != 10000 ){
            CBmpDotPack* pPack = GetBmpDotPackAt( i );
            pPack->applyRateScale( rateScale );
        }
#endif
    }

    return( true );
}

//------------------
// デフォルトデータ作成
//------------------
bool CBmpDotMgr::CreateDefault( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpDotMgr::CreateDefault()\n" );
#endif
    
    CLayerData* pLD;
    CSlotObjectData* pSOD;
    CSlotPointData* pSPD;

    //---------------------------
    // [HdBand]
    //---------------------------
    pSPD = CSlotPointData::Alloc();
    pSPD->setSlot( eBD_SLOT_HdRibbon );     // リボンを呼ぶ
    
    pSOD = CSlotObjectData::Alloc();
    pSOD->addData( pSPD );
    
    pLD = CLayerData::Alloc();
    pLD->addData( pSOD );
    
    s_pDefaultBdpd_HdBand = CBmpDotPartData::Alloc();
    s_pDefaultBdpd_HdBand->setSlot( eBD_SLOT_HdBand );
    s_pDefaultBdpd_HdBand->addData( pLD );

    //---------------------------
    // [HdHairBackKnot]
    //---------------------------
    pSPD = CSlotPointData::Alloc();
    pSPD->setSlot( eBD_SLOT_HdHairBack );   // ポニーテールを呼ぶ
    
    pSOD = CSlotObjectData::Alloc();
    pSOD->addData( pSPD );
    
    pLD = CLayerData::Alloc();
    pLD->addData( pSOD );
    
    s_pDefaultBdpd_HdHairBackKnot = CBmpDotPartData::Alloc();
    s_pDefaultBdpd_HdHairBackKnot->setSlot( eBD_SLOT_HdHairBackKnot );
    s_pDefaultBdpd_HdHairBackKnot->addData( pLD );

    //---------------------------
    // [HdHairSideKnot]
    //---------------------------
    pSPD = CSlotPointData::Alloc();
    pSPD->setSlot( eBD_SLOT_HdHairSide );   // サイドテールを呼ぶ
    
    pSOD = CSlotObjectData::Alloc();
    pSOD->addData( pSPD );
    
    pLD = CLayerData::Alloc();
    pLD->addData( pSOD );

    s_pDefaultBdpd_HdHairSideKnot = CBmpDotPartData::Alloc();
    s_pDefaultBdpd_HdHairSideKnot->setSlot( eBD_SLOT_HdHairSideKnot );
    s_pDefaultBdpd_HdHairSideKnot->addData( pLD );

    //---------------------------
    // [HdHairDownKnot]
    //---------------------------
    pSPD = CSlotPointData::Alloc();
    pSPD->setSlot( eBD_SLOT_HdHairDown );   // おさげを呼ぶ
    
    pSOD = CSlotObjectData::Alloc();
    pSOD->addData( pSPD );
    
    pLD = CLayerData::Alloc();
    pLD->addData( pSOD );

    s_pDefaultBdpd_HdHairDownKnot = CBmpDotPartData::Alloc();
    s_pDefaultBdpd_HdHairDownKnot->setSlot( eBD_SLOT_HdHairDownKnot );
    s_pDefaultBdpd_HdHairDownKnot->addData( pLD );
    
    return( true );
}
