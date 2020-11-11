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
// ID調整
//---------------------------
int CBmpPalData::addId( int val ){
    m_nUid += val;
    
    if( m_nUid < BP_UID_MIN ){
        m_nUid = BP_UID_MIN;
    }else if( m_nUid >= BP_UID_MAX ){
        m_nUid = BP_UID_MAX - 1;
    }
    
    return( m_nUid );
}

//---------------------------
// ID調整
//---------------------------
int CBmpPalData::addSubId( int val ){
    m_nSubId += val;
    
    if( m_nSubId < BP_SUB_ID_MIN ){
        m_nSubId = BP_SUB_ID_MIN;
    }else if( m_nSubId >= BP_SUB_ID_MAX ){
        m_nSubId = BP_SUB_ID_MAX - 1;
    }

    return( m_nSubId );
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
    // 全色をランダム設定
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        CConst::AdjustColorRand( &m_nArrArrBaseRGBA[i][0], &m_nArrArrBaseRGBA[i][1], &m_nArrArrBaseRGBA[i][2], randRange );
    }

    //---------------------------------------------------
    // 特定の色であれば調整
    //---------------------------------------------------
    int at, at0;
    
    // 白：固定
    at = ePAL_OFS_C_WHITE - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = 255;
    }

    // 黒：固定
    at = ePAL_OFS_C_BLACK - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = 0;
    }

    // 顔：基本色から外れない範囲に収める
    at = ePAL_OFS_C_FACE - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0] + 7*253)/8);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1] + 7*219)/8);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2] + 7*204)/8);

    // ほっぺた＆口：基本色から外れない範囲に収める
    at = ePAL_OFS_C_FACE_CHEEK - ePAL_OFS_C_HEAD;
    m_nArrArrBaseRGBA[at][0] = (BYTE)((m_nArrArrBaseRGBA[at][0] + 7*251)/8);
    m_nArrArrBaseRGBA[at][1] = (BYTE)((m_nArrArrBaseRGBA[at][1] + 7*200)/8);
    m_nArrArrBaseRGBA[at][2] = (BYTE)((m_nArrArrBaseRGBA[at][2] + 7*214)/8);
    
    // 目玉（サブ）：目玉の値を反転
    at0 = ePAL_OFS_C_EYE - ePAL_OFS_C_HEAD;
    at = ePAL_OFS_C_EYE_SUB - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = 0xFF - m_nArrArrBaseRGBA[at0][i];
    }
    
    // 髪（サブ）：髪の色を反転
    at0 = ePAL_OFS_C_HAIR - ePAL_OFS_C_HEAD;
    at = ePAL_OFS_C_HAIR_SUB - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = 0xFF - m_nArrArrBaseRGBA[at0][i];
    }

    // 確定（※一部の色が調整される）
    fix();
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

    // 確定（※一部の色が調整される）
    pMenu->fixMenu();
}

//---------------------------------------------------
// テクスチャへ読み込み
//---------------------------------------------------
bool CBmpPalData::loadToTex( CTex* pTex ){
    // 対象データの確認
    if( pTex == NULL ){
        LOGE( "@ CBmpPalData::loadToTex: INVALID TEX\n" );
        return( false );
    }
    
    // パレットデータの構築
    BYTE pPal[256*4];
    memset( pPal, 0, sizeof(BYTE)*256*4 );

    // キーとなる色を設定
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

//---------------------------------------------------
// パレット画像へ読み込み
//---------------------------------------------------
bool CBmpPalData::loadToImgPal( eIMG target ){
    // 対象データの確認
    if( ! IS_IMG_VALID( target ) ){
        LOGE( "@ CBmpPalData::loadToImgPal: INVALID ID: target=%d\n", target );
        return( false );
    }

    return( loadToTex( CImgMgr::GetImgPal( target ) ) );
}

//-------------------
// パレットの合成：平均
//-------------------
void CBmpPalData::mixAverage( CBmpPalData** pArrPal, int* arrWeight, int num ){
    // 無効は無視
    if( pArrPal == NULL || arrWeight == NULL || num <= 0 ){
        LOGE( "@ CBmpPalData::mixAverage: INVALID: pArrPal=%p, arrWeight=%p, num=%d\n", pArrPal, arrWeight, num );
        return;
    }

    // 比重の累計
    int totalWeight = 0;
    for( int i=0; i<num; i++ ){
        if( pArrPal[i] != NULL ){
            totalWeight += arrWeight[i];
        }
    }
    
    // 用心
    if( totalWeight <= 0 ){
        LOGE( "@ CBmpPalData::mixAverage: INVALID: totalWeight=%d\n", totalWeight );
        return;
    }
    
    int rgbaBright[4];
    int rgbaBase[4];
    int rgbaDark[4];
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        // 初期化
        for( int k=0; k<4; k++ ){
            rgbaBright[k] = 0;
            rgbaBase[k] = 0;
            rgbaDark[k] = 0;
        }

        // 累積
        for( int j=0; j<num; j++ ){
            if( pArrPal[j] != NULL ){
                for( int k=0; k<4; k++ ){
                    rgbaBright[k] += pArrPal[j]->m_nArrArrBrightRGBA[i][k] * arrWeight[j];
                    rgbaBase[k] += pArrPal[j]->m_nArrArrBaseRGBA[i][k] * arrWeight[j];
                    rgbaDark[k] += pArrPal[j]->m_nArrArrDarkRGBA[i][k] * arrWeight[j];
                }
            }
        }

        // 平均値を設定
        for( int k=0; k<4; k++ ){
            m_nArrArrBrightRGBA[i][k] = rgbaBright[k]/totalWeight;
            m_nArrArrBaseRGBA[i][k] = rgbaBase[k]/totalWeight;
            m_nArrArrDarkRGBA[i][k] = rgbaDark[k]/totalWeight;
        }
    }

    // 確定（※一部の色が調整される）
    fix();
}

//-------------------
// パレットの合成：抽選
//-------------------
void CBmpPalData::mixSelect( CBmpPalData** pArrPal, int* arrWeight, int num, eRAND_ID randId ){
    // 無効は無視
    if( pArrPal == NULL || arrWeight == NULL || num <= 0 ){
        LOGE( "@ CBmpPalData::mixSelect: INVALID: pArrPal=%p, arrWeight=%p, num=%d\n", pArrPal, arrWeight, num );
        return;
    }

    // 比重の累計
    int totalWeight = 0;
    for( int i=0; i<num; i++ ){
        if( pArrPal[i] != NULL ){
            totalWeight += arrWeight[i];
        }
    }
    
    // 用心
    if( totalWeight <= 0 ){
        LOGE( "@ CBmpPalData::mixSelect: INVALID: totalWeight=%d\n", totalWeight );
        return;
    }

    int target;
    int weight;
    for( int i=0; i<BEZIER_PAL_GRP_NUM; i++ ){
        // 比重で抽選
        target = 0;
        weight = CRand::GetRandAt( randId, totalWeight );
        while( target < num ){
            if( pArrPal[target] != NULL ){
                if( weight < arrWeight[target] ){
                    break;
                }
                weight -= arrWeight[target];
            }
            target++;
        }

        // 当選した色を設定
        for( int j=0; j<4; j++ ){
            m_nArrArrBrightRGBA[i][j] = pArrPal[target]->m_nArrArrBrightRGBA[i][j];
            m_nArrArrBaseRGBA[i][j] = pArrPal[target]->m_nArrArrBaseRGBA[i][j];
            m_nArrArrDarkRGBA[i][j] = pArrPal[target]->m_nArrArrDarkRGBA[i][j];
        }
    }
    
    // 確定
    fix();
}

//------------------
// パレット調整
//------------------
void CBmpPalData::fix( void ){
    int at0, at;
    
    // 眉毛：髪の色をコピー（※色テストに影響されないための枠）
    at0 = ePAL_OFS_C_HAIR - ePAL_OFS_C_HEAD;
    at = ePAL_OFS_C_BROWS - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = m_nArrArrBaseRGBA[at0][i];
    }

    // 肌：顔の色をコピー（※色テストに影響されないための枠）
    at0 = ePAL_OFS_C_FACE - ePAL_OFS_C_HEAD;
    at = ePAL_OFS_C_SKIN - ePAL_OFS_C_HEAD;
    for( int i=0; i<3; i++ ){
        m_nArrArrBaseRGBA[at][i] = m_nArrArrBaseRGBA[at0][i];
    }
}
