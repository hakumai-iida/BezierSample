/*+----------------------------------------------------------------+
  | Title:      BmpPalData.cpp [共通環境]
  | Comment:    BMPパレットデータ（※BezierConstに対応した形式の色管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpPalData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// デフォルトデータBMPのバイナリサイズ
#define DEFULAT_BMP_DATA_SIZE   5174

// BMPデータ内のパレットへのオフセット
#define PAL_OFS_IN_BMP_FILE     54

// 色番号算出
#define PAL_MIN_COL_BYTE_AT( _id )      (4*(BEZIER_PAL_GRP_HEAD + (_id)*BEZIER_PAL_COL_NUM_IN_GRP + BEZIER_PAL_VAL_MIN))
#define PAL_BASE_COL_BYTE_AT( _id )     (4*(BEZIER_PAL_GRP_HEAD + (_id)*BEZIER_PAL_COL_NUM_IN_GRP + BEZIER_PAL_VAL_BASE))
#define PAL_MAX_COL_BYTE_AT( _id )      (4*(BEZIER_PAL_GRP_HEAD + (_id)*BEZIER_PAL_COL_NUM_IN_GRP + BEZIER_PAL_VAL_MAX))

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 基本[BPM]データ（※ベタで持つ）
static BYTE _default_bmp_data[DEFULAT_BMP_DATA_SIZE] = {
#include "inc/_FixedDefaultBmpPalData.inc"
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------------
// コンストラクタ
//-------------------------
CBmpPalData::CBmpPalData( void ): CDataFile(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpPalData::~CBmpPalData( void ){}

//-------------------------
// クリア
//-------------------------
void CBmpPalData::clear( void ){
    m_eCategory = eBP_CATEGORY_INVALID;
    m_nUid = BP_UID_INVALID;
    m_nSubId = BP_SUB_ID_INVALID;

    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++){
        m_nArrArrBrightRGBA[i][0] = 0xFF;   // R
        m_nArrArrBrightRGBA[i][1] = 0x00;   // G
        m_nArrArrBrightRGBA[i][2] = 0xFF;   // B
        m_nArrArrBrightRGBA[i][3] = 0xFF;   // A

        m_nArrArrBaseRGBA[i][0] = 0xFF;     // R
        m_nArrArrBaseRGBA[i][1] = 0x00;     // G
        m_nArrArrBaseRGBA[i][2] = 0xFF;     // B
        m_nArrArrBaseRGBA[i][3] = 0xFF;     // A

        m_nArrArrDarkRGBA[i][0] = 0xFF;     // R
        m_nArrArrDarkRGBA[i][1] = 0x00;     // G
        m_nArrArrDarkRGBA[i][2] = 0xFF;     // B
        m_nArrArrDarkRGBA[i][3] = 0xFF;     // A
    }
    
    m_bExecAsBmp = false;
}
 
//------------------------------
// コピー
//------------------------------
void CBmpPalData::copy( CBmpPalData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    m_eCategory = pData->m_eCategory;
    m_nUid = pData->m_nUid;
    m_nSubId = pData->m_nSubId;

    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++){
        for( int j=0; j<4; j++){ m_nArrArrBrightRGBA[i][j] = m_nArrArrBrightRGBA[i][j]; }
        for( int j=0; j<4; j++){ m_nArrArrBaseRGBA[i][j] = m_nArrArrBaseRGBA[i][j]; }
        for( int j=0; j<4; j++){ m_nArrArrDarkRGBA[i][j] = m_nArrArrDarkRGBA[i][j]; }
    }
}

//---------------------------
// 判定
//---------------------------
bool CBmpPalData::isEqual( eBP_CATEGORY category, int uid, int subId ){
    if( m_eCategory != category  ){ return( false ); }
    if( m_nUid != uid  ){ return( false ); }
    if( m_nSubId != subId  ){ return( false ); }

    return( true );
}

//---------------------------
// ロード実体（※読み込み）
//---------------------------
void CBmpPalData::load0( CInputBuffer* pIB ){
    // 指定があれば[BMP]読み込みへ
    if( m_bExecAsBmp ){
        loadAsBmp( pIB );
        return;
    }
    
    // ここまできたら通常の読み込み
    m_eCategory = (eBP_CATEGORY)CEnum::ReadEnum( pIB, g_pArrLabelBpCategory );
    m_nUid = pIB->readInt32();
    m_nSubId = pIB->readInt16();
    
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++){
        for( int j=0; j<4; j++){ m_nArrArrBrightRGBA[i][j] = pIB->readUint8(); }
        for( int j=0; j<4; j++){ m_nArrArrBaseRGBA[i][j] = pIB->readUint8(); }
        for( int j=0; j<4; j++){ m_nArrArrDarkRGBA[i][j] = pIB->readUint8(); }
    }
}

//---------------------------
// セーブ実体（※書き込み）
//---------------------------
void CBmpPalData::save0( COutputBuffer* pOB ){
    // 指定があれば[BMP]書き込みへ
    if( m_bExecAsBmp ){
        saveAsBmp( pOB );
        return;
    }

    // ここまできたら通常の書き込み
    CEnum::WriteEnum( pOB, m_eCategory, eBP_CATEGORY_MAX, g_pArrLabelBpCategory );
    pOB->writeInt32( m_nUid );
    pOB->writeInt16( m_nSubId );

    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++){
        for( int j=0; j<4; j++){ pOB->writeUint8( m_nArrArrBrightRGBA[i][j] ); }
        for( int j=0; j<4; j++){ pOB->writeUint8( m_nArrArrBaseRGBA[i][j] ); }
        for( int j=0; j<4; j++){ pOB->writeUint8( m_nArrArrDarkRGBA[i][j] ); }
    }
}

//-------------------
// BMPとして読み込み
//-------------------
void CBmpPalData::loadAsBmp( CInputBuffer* pIB ){
    // ヘッダ部分の読み飛ばし
    pIB->skip( PAL_OFS_IN_BMP_FILE );
    
    // パレットデータの読み込み
    BYTE pPal[256*4];
    pIB->readBlock( pPal, 256*4 );

    // 欲しい色だけ抽出
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        int minAt = PAL_MIN_COL_BYTE_AT( i );
        int baseAt = PAL_BASE_COL_BYTE_AT( i );
        int maxAt = PAL_MAX_COL_BYTE_AT( i );

        // RGB
        for( int j=0; j<3; j++ ){
            m_nArrArrBrightRGBA[i][2-j] = pPal[minAt+j];
            m_nArrArrBaseRGBA[i][2-j] = pPal[baseAt+j];
            m_nArrArrDarkRGBA[i][2-j] = pPal[maxAt+j];
        }

        // A（※BMPではアルファが０で保存されている）
        m_nArrArrBrightRGBA[i][3] = 0xFF;
        m_nArrArrBaseRGBA[i][3] = 0xFF;
        m_nArrArrDarkRGBA[i][3] = 0xFF;
    }
}

//-------------------
// BMPとして書き込み
//-------------------
void CBmpPalData::saveAsBmp( COutputBuffer* pOB ){
    // デフォルトデータバッファのパレット部分をワークに設定
    BYTE* pPal = &_default_bmp_data[PAL_OFS_IN_BMP_FILE];

    // 管理色を書き込む
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        int minAt = PAL_MIN_COL_BYTE_AT( i );
        int baseAt = PAL_BASE_COL_BYTE_AT( i );
        int maxAt = PAL_MAX_COL_BYTE_AT( i );

        // RGB
        for( int j=0; j<3; j++ ){
            pPal[minAt+j] = m_nArrArrBrightRGBA[i][2-j];
            pPal[baseAt+j] = m_nArrArrBaseRGBA[i][2-j];
            pPal[maxAt+j] = m_nArrArrDarkRGBA[i][2-j];
        }
        
        // A（※アルファは０で出力＝BMPはアルファ枠をもつが値は０で管理されている）
        pPal[minAt+3] = 0x00;
        pPal[baseAt+3] = 0x00;
        pPal[maxAt+3] = 0x00;
    }
    
    // 出力
    pOB->writeBlock( _default_bmp_data, DEFULAT_BMP_DATA_SIZE );
}

//---------------------------------------------------
// パレット画像の読み直し（※管理データを実際の画像へ反映させる）
//---------------------------------------------------
bool CBmpPalData::reloadPalImg( eIMG target ){
    // 対象データの確認
    if( ! IS_IMG_VALID( target ) ){
        LOGE( "@ CBmpPalData::reloadPalImg: INVALID ID: target=%d\n", target );
        return( false );
    }
    
    // テクスチャ取得
    CTex* pTex = CImgMgr::GetImgPal( target );
    if( pTex == NULL ){
        LOGE( "@ CBmpPalData::reloadPalImg: INVALID TEX: target=%d\n", target );
        return( false );
    }
    
    // パレットデータの構築
    BYTE pPal[256*4];
    memset( pPal, 0, sizeof(BYTE)*256*4 );

    // 管理色を設定
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        int minAt = PAL_MIN_COL_BYTE_AT( i );
        int baseAt = PAL_BASE_COL_BYTE_AT( i );
        int maxAt = PAL_MAX_COL_BYTE_AT( i );
        
        // RGBA（※ここはアルファも指定する＝反映される）
        for( int j=0; j<4; j++ ){
            pPal[minAt+j] = m_nArrArrBrightRGBA[i][j];
            pPal[baseAt+j] = m_nArrArrBaseRGBA[i][j];
            pPal[maxAt+j] = m_nArrArrDarkRGBA[i][j];
        }
    }
    
    // テクスチャの開放＆読み直し
    pTex->release();
    return( pTex->create( pPal, eTEX_DATA_TYPE_P8P, 256, 1, TEX_CREATE_OPT_PAL_ADJ_FOR_ARITH_COL ) );
}

//-------------------------------
// 基本色をMin色／Max色へコピー
//-------------------------------
void CBmpPalData::copyBaseToMinMax( void ){
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        for( int j=0; j<4; j++ ){
            m_nArrArrBrightRGBA[i][j] = m_nArrArrBaseRGBA[i][j];
            m_nArrArrDarkRGBA[i][j] = m_nArrArrBaseRGBA[i][j];
        }
    }
}

//-------------------------------
// 基本色を明るく／暗く
//-------------------------------
void CBmpPalData::adjustBase( bool isDark, float rate, int minStep ){
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        CConst::AdjustColor( &m_nArrArrBaseRGBA[i][0], &m_nArrArrBaseRGBA[i][1], &m_nArrArrBaseRGBA[i][2], isDark, rate, minStep );
    }
}

//-------------------------------
// 基本色に対してランダム
//-------------------------------
void CBmpPalData::adjustRandForBase( int randRange ){
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        CConst::AdjustColorRand( &m_nArrArrBaseRGBA[i][0], &m_nArrArrBaseRGBA[i][1], &m_nArrArrBaseRGBA[i][2], randRange );
    }

    //---------------------------------------------------
    // 基本的な色の調整（※特定の色は印象が残るように）
    //---------------------------------------------------
    int at;
    
    // 白
    at = ePAL_OFS_C_WHITE - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0]+4*255)/5);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1]+4*255)/5);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2]+4*255)/5);

    // 黒
    at = ePAL_OFS_C_BLACK - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0]+4*0)/5);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1]+4*0)/5);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2]+4*0)/5);

    // 肌
    at = ePAL_OFS_C_SKIN - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0]+4*253)/5);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1]+4*219)/5);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2]+4*204)/5);

    // 顔（肌）
    int at0 = at;
    at = ePAL_OFS_C_FACE - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = m_nArrArrBaseRGBA[at0][0];
    m_nArrArrBaseRGBA[at][1] = m_nArrArrBaseRGBA[at0][1];
    m_nArrArrBaseRGBA[at][2] = m_nArrArrBaseRGBA[at0][2];

    // ほっぺた
    at = ePAL_OFS_C_FACE_CHEEK - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0]+4*251)/5);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1]+4*200)/5);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2]+4*214)/5);
}

//-------------------------------
// Min色／Max色を明るく／暗く
//-------------------------------
void CBmpPalData::adjustOneStepForMinMax( float rate, int minStep ){
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        CConst::AdjustColor( &m_nArrArrBrightRGBA[i][0], &m_nArrArrBrightRGBA[i][1], &m_nArrArrBrightRGBA[i][2], false, rate, minStep );
        CConst::AdjustColor( &m_nArrArrDarkRGBA[i][0], &m_nArrArrDarkRGBA[i][1], &m_nArrArrDarkRGBA[i][2], true, rate, minStep );
    }
}

//-------------------------------------------------
// 編集メニューに関連付け
//-------------------------------------------------
void CBmpPalData::setEditPalMenu( CEditPalMenu* pMenu ){
    // 編集項目数設定：パレットグループ１つにつき「基本色／明色／暗色」の３枠が必要
    pMenu->setItemNum( 3*BEZIER_PAL_GRP_NUM );
    
    char* buf = CMemMgr::GetBufFromTempStr();
    int id = 0;

    // 全色の登録
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        const char* pLabel = g_pArrLabelPalOfs[i+ePAL_OFS_C_HEAD];
        
        sprintf( buf, "%s[MIN]", pLabel );
        pMenu->setItemAt( id, buf, &m_nArrArrBrightRGBA[i][0], &m_nArrArrBrightRGBA[i][1], &m_nArrArrBrightRGBA[i][2], &m_nArrArrBrightRGBA[i][3] );
        id++;

        sprintf( buf, "%s[BAS]", pLabel );
        pMenu->setItemAt( id, buf, &m_nArrArrBaseRGBA[i][0], &m_nArrArrBaseRGBA[i][1], &m_nArrArrBaseRGBA[i][2], &m_nArrArrBaseRGBA[i][3] );
        id++;

        sprintf( buf, "%s[MAX]", pLabel );
        pMenu->setItemAt( id, buf, &m_nArrArrDarkRGBA[i][0], &m_nArrArrDarkRGBA[i][1], &m_nArrArrDarkRGBA[i][2], &m_nArrArrDarkRGBA[i][3] );
        id++;

        pMenu->setSeparatorAt( id, true );
    }

    // 確定
    pMenu->fixMenu();
}
