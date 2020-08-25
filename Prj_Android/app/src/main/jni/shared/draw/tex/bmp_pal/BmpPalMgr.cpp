/*+----------------------------------------------------------------+
  |	Title:		BmpPalMgr.cpp [共通環境]
  |	Comment:	BMPパレット管理（※ファイル単位で管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpPalMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// データ格納フォルダ
#define BPD_FOLDER_NAME        "bpd"
#define BMP_PAL_FOLDER_NAME    "bmp_pal"

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
// ファイル名リスト
//-----------------------------------------------------------------
const char* CBmpPalMgr::s_pArrBmpPalDataName[] = {
    // miku
    "A0000_00.bpd",
    "A0000_01.bpd",

    // goth
    "A0001_00.bpd",
    "A0001_01.bpd",

    // negi
    "S000000_00.bpd",
    "S000000_01.bpd",

    NULL
};

int CBmpPalMgr::s_nNumData;
CBmpPalData* CBmpPalMgr::s_oArrBmpPalData;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CBmpPalMgr::GetAllocCellNum( void ){
    int num = 0;
    
    //------------------------------------------------------
    // s_oArrBmpPalData = new CBmpPalData[GetBmpPalDataNum()];
    //------------------------------------------------------
    num += 1;
    
    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CBmpPalMgr::GetAllocSizeK( void ){
    int size = 0;
    
    //--------------------------------------------------------
    // s_oArrBmpPalData = new CBmpPalData[GetBmpPalDataNum()];
    //--------------------------------------------------------
    size += sizeof(CBmpPalData) * GetBmpPalDataNum();
    
    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//------------------------
// 初期化
//------------------------
bool CBmpPalMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBmpPalMgr::OnCreate()\n" );
#endif
    
    // データ構築
    if( ! CreateAll() ){
        LOGE( "@ CreateAll(): FAILED!!\n" );
        env_push_error_log( "[CBmpPalMgr::CreateAll()]に失敗しました。\n" );
        return( false );
    }

	return( true );
}

//------------------------
// 終了
//------------------------
void CBmpPalMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CBmpPalMgr::OnDestroy()\n" );
#endif

    // 解放
    SAFE_DELETE_ARRAY( s_oArrBmpPalData );
}

//----------------------
// データ数取得
//----------------------
int CBmpPalMgr::GetBmpPalDataNum( void ){
    // 未初期化ならファイル数の検出
    if( s_nNumData <= 0 ){
        s_nNumData = 0;
        while( s_pArrBmpPalDataName[s_nNumData] != NULL ){
            s_nNumData++;
        }
    }
    
    return( s_nNumData );
}

//---------------------------
// ファイル名取得
//---------------------------
const char* CBmpPalMgr::GetBmpPalDataNameAt( int at ){
    int numData = GetBmpPalDataNum();
    
    // 無効は無視
    if( at < 0 || at >= numData ){
        LOGE( "@ CBmpPalMgr::GetBmpPalDataNameAt: INVALID at=%d/%d\n", at, numData );
        return( NULL );
    }
    
    return( s_pArrBmpPalDataName[at] );
}

//------------------------------------
// データ取得
//------------------------------------
CBmpPalData* CBmpPalMgr::GetBmpPalDataAt( int at ){
    int numData = GetBmpPalDataNum();

    // 無効は無視
    if( at < 0 || at >= numData ){
        LOGE( "@ CBmpPalMgr::GetBmpPalDataAt: INVALID at=%d/%d\n", at, numData );
        return( NULL );
    }

    return( &s_oArrBmpPalData[at] );
}

//------------------------------------
// データ検索
//------------------------------------
CBmpPalData* CBmpPalMgr::SearchBmpPalData( eBP_CATEGORY category, int uid, int subId, bool useDefaultIfNotFound ){
    int numData = GetBmpPalDataNum();

    // 検索
    for( int i=0; i<numData; i++ ){
        if( s_oArrBmpPalData[i].isEqual( category, uid, subId ) ){
            return( &s_oArrBmpPalData[i] );
        }
    }
    
    // 指定があれば[subId==0]を探す
    if( useDefaultIfNotFound && subId != 0 ){
        return( SearchBmpPalData( category, uid, 0, false ) );
    }

    // ここまできたら未発見
    return( NULL );
}

//-------------------
// 読み込み
//-------------------
bool CBmpPalMgr::LoadBpdAt( int at, const char* pName ){
    CBmpPalData* pData = GetBmpPalDataAt( at );
    if( pData != NULL ){
        pData->setExecAsBmp( false );   // 通常処理
        return( pData->load( pName, BPD_FOLDER_NAME ) );
    }
    
    return( false );
}

//-------------------
// 保存
//-------------------
bool CBmpPalMgr::SaveBpdAt( int at, const char* pName, bool isTemporary ){
    CBmpPalData* pData = GetBmpPalDataAt( at );
    
    if( pData != NULL ){
        pData->setExecAsBmp( false );   // 通常処理
        return( pData->save( pName, isTemporary ) );
    }
    
    return( false );
}

//-------------------
// 全保存
//-------------------
bool CBmpPalMgr::SaveBpdAll( void ){
    int num = GetBmpPalDataNum();
    
    for( int i=0; i<num; i++ ){
        CBmpPalData* pData = GetBmpPalDataAt( i );
    
        if( pData != NULL ){
            pData->setExecAsBmp( false );   // 通常処理
            if( ! pData->save( s_pArrBmpPalDataName[i], false ) ){
                return( false );
            }
        }
    }
    
    return( true );
}

//------------------------------------------------------------------------------
// BMP読み込み（※リソースからのみ読み込む＝パレットの元データをBMPから参照する程度の利用を想定）
//------------------------------------------------------------------------------
bool CBmpPalMgr::LoadBpdAtAsBmp( int at, const char* pName ){
    CBmpPalData* pData = GetBmpPalDataAt( at );
    if( pData != NULL ){
        pData->setExecAsBmp( true );    // BMP処理
        return( pData->load( pName, BMP_PAL_FOLDER_NAME, false, false ) );
    }
    
    return( false );
}

//------------------------------------------------------------------------------------------------
// BMP書き込み（※書き込んだデータはアプリから参照されない＝外部ツール[iExplore]等で抽出して確認する程度の利用を想定）
//------------------------------------------------------------------------------------------------
bool CBmpPalMgr::SaveBpdAtAsBmp( int at, const char* pName ){
    CBmpPalData* pData = GetBmpPalDataAt( at );

    if( pData != NULL ){
        pData->setExecAsBmp( true );    // BMP処理
        return( pData->save( pName, false ) );
    }
    
    return( false );
}

//------------------
// 全データ作成
//------------------
bool CBmpPalMgr::CreateAll( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBmpPalMgr::CreateAll()\n" );
#endif
    
    int numData = GetBmpPalDataNum();
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_BMP_PAL );
    s_oArrBmpPalData = new CBmpPalData[numData];
    CMemMgr::PopTargetField();
    
    // パレットデータ読み込み
    for( int i=0; i<numData; i++ ){
        LoadBpdAt( i, s_pArrBmpPalDataName[i] );
    }
   
    return( true );
}
