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
#include "draw/tex/adjust/DelayLog.hpp"
#include "draw/tex/bezier/LayerOrderList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// データ格納フォルダ
#define BDP_FOLDER_NAME        "bdp"

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
    "AF0000_BASE.bdp",
    "AB0000_BASE.bdp",
    
    "AF0001_BASE.bdp",
    "AB0001_BASE.bdp",
    
    "AS000000_BASE.bdp",

    NULL
};

int CBmpDotMgr::s_nNumPack;
CBmpDotPack* CBmpDotMgr::s_oArrBmpDotPack;

const char* CBmpDotMgr::s_pBmpDotSettingFileNameForWork = "bdSetting.bds";
CBmpDotSettingData* CBmpDotMgr::s_pBmpDotSettingDataForWork;

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
    SAFE_DELETE( s_pBmpDotSettingDataForWork );
    
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
// デフォルト方向[DIR]の検索
#define SEARCH_DEFAULT_DIR_THEN_RETURN_IF_FOUND                     \
    if( pParam->dir != eBD_DIR_DEFAULT ){                           \
        pParamTmp->dir = eBD_DIR_DEFAULT;                           \
        pData = SearchBmpDotPartData( pParamTmp );                  \
        if( pData != NULL ){                                        \
        DEF_HIT_LOG( "DIR" );                                       \
            return( pData );                                        \
        }                                                           \
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
    // 指定条件での検索（※ヒットしたらそのまま返す）
    CBmpDotPartData* pData = SearchBmpDotPartData( pParam );
    if( pData != NULL ){
        // 出力指定が有効であれば検索パラメータをコピー
        if( pParamOut != NULL ){
            *pParamOut = *pParam;
        }

        return( pData );
    }

    // ここまできたらパラメータを差し替えつつ検索する
    stBD_PART_SEARCH_PARAM paramTmp;
    stBD_PART_SEARCH_PARAM* pParamTmp;
    if( pParamOut != NULL ){
        pParamTmp = pParamOut;
    }else{
        pParamTmp = &paramTmp;
    }

    // 検索パラメータをワークにコピー
    *pParamTmp = *pParam;
    
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
        LOGD( "@ CBmpDotMgr::SearchBmpDotPartDataWithDefault: NOT FOUND ctg=%d, uid=%08d, emo=%d, f=%d, slt=%d, sid=%d, dir=%d\n",
              pParam->category, pParam->uid, pParam->emo, pParam->form, pParam->slot, pParam->subId, pParam->dir );
    }

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

//-------------------
// ワーク設定読み込み
//-------------------
bool CBmpDotMgr::LoadBmpDotSettingForWork( const char* pName ){
    if( s_pBmpDotSettingDataForWork->load( pName, BDP_FOLDER_NAME ) ){
        return( true );
    }
    
    return( false );
}

//-------------------
// ワーク設定書き込み
//-------------------
bool CBmpDotMgr::SaveBmpDotSettingForWork( const char* pName, bool isTemporary ){
    if( s_pBmpDotSettingDataForWork->save( pName, isTemporary ) ){
        return( true );
    }
    
    return( false );
}

//------------------
// 全データ読み込み
//------------------
bool CBmpDotMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpDotMgr::CreateAll()\n" );
#endif
    
    int numPack = GetBmpDotPackNum();
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_BMP );
    s_oArrBmpDotPack = new CBmpDotPack[numPack];
    s_pBmpDotSettingDataForWork = new CBmpDotSettingData();
    CMemMgr::PopTargetField();
    
    // パック読み込み
    for( int i=0; i<numPack; i++ ){
        LoadBdpAt( i, s_pArrBmpDotPackName[i] );
    }

    // 設定
    LoadBmpDotSettingForWork( s_pBmpDotSettingFileNameForWork );
    
    return( true );
}
