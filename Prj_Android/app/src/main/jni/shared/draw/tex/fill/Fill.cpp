/*+----------------------------------------------------------------+
  | Title:      Fill.cpp [共通環境]
  | Comment:    塗り（※対象はパレット画像）
  | Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Fill.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// ガイド調整
//------------------------
void CFill::AdjustFillGuideForOutCol( BYTE* pBufGuide, int w, int h ){
    // 塗りガイド全体を参照
    int l = CBmpGenerator::GetLeftForFillGuideBuf();
    int r = CBmpGenerator::GetRightForFillGuideBuf();
    int t = CBmpGenerator::GetTopForFillGuideBuf();
    int b = CBmpGenerator::GetBottomForFillGuideBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pGuide = &pBufGuide[w*y];
        for( int x=l; x<=r; x++ ){
            // 出力色だけを持ち越す
            STAY_BFG_OUT_COL( pGuide[x] );
        }
    }
}

//------------------------
// コンストラクタ
//------------------------
CFill::CFill( void ){
    clear();
}

//------------------------
// デストラクタ
//------------------------
CFill::~CFill( void ){}

//-------------------
// クリア
//-------------------
void CFill::clear( void ){
    m_eType = eFILL_TYPE_INVALID;
    
    m_pBuf = NULL;
    m_pBufGuide = NULL;
    m_pBufGuard = NULL;
    m_nDotW = m_nDotH = 0;

    m_nUndercoatL = m_nUndercoatR = 0;
    m_nUndercoatT = m_nUndercoatB = 0;
    
    m_stArrUndercoatCell = NULL;
    m_nUndercoatCellHeadAt = m_nUndercoatCellTailAt = 0;
}

//------------------------------------------------------------------
// 塗り設定
// [type=eFILL_TYPE_TEX]の場合は[pSrc,srcW,srcH]の指定が必須
//------------------------------------------------------------------
bool CFill::set( eFILL_TYPE type ){
    // 一旦クリア
    clear();
    
    // 無効は無視
    if( type < 0 || type >= eFILL_TYPE_MAX ){
        LOGE( "@ CFill::set: FAILED: INVALID TYPE=%d\n", type );
        return( false );
    }

    // 設定
    m_eType = type;
    
    return( true );
}

//------------------------
// 指定座標から塗りつぶす
//------------------------
bool CFill::fill0( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y,
                   BYTE palGrp, BYTE palVal, BYTE testPalGrp, bool isForAdd, bool isOnOutCol, int addVal ){
    // 用心：バッファが無効
    if( pBuf == NULL || pBufGuide == NULL || pBufGuard == NULL ){
        LOGE( "CFill::fill0: INVALID BUF: pBuf=%p, pBufGuide=%p, pBufGuard=%p\n", pBuf, pBufGuide, pBufGuard );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::fill0: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：座標が無効
    if( x < 0 || x >= w || y < 0 || y >= h ){
        LOGE( "CFill::fill0: INVALID POSITION: x=%d/%d, y=%d/%d\n", x, w, y, h );
        return( false );
    }

    if( isForAdd ){
        // 用心：加算値が０であれば無視
        if( addVal == 0 ){
            LOGE( "CFill::fill0: INVALID VAL FOR ADD: addVal=%d\n", addVal );
            return( false );
        }
    }

    // 用心：指定座標が有効色であれば無視
    if( IS_NOT_BFG_CLEAR_COL( pBufGuide[w*y+x] ) ){
        LOGE( "CFill::fill0: INVALID FILL POSITION: x=%d, y=%d, flag=%X\n", x, y, pBufGuide[w*y+x] );
        return( false );
    }
    
    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;
    m_pBufGuide = pBufGuide;
    m_pBufGuard = pBufGuard;
    m_nDotW = w;
    m_nDotH = h;

    m_nUndercoatL = w;
    m_nUndercoatR = -1;
    m_nUndercoatT = h;
    m_nUndercoatB = -1;

    readyForUndercoat();

    //---------------------------------------------------
    // 下塗り（※塗りガイドの[x,y]の位置から閉じた範囲を塗りつぶす）
    //---------------------------------------------------
    fillUndercoat( x, y );
    
    // 下塗り範囲が有効であれば
    if( m_nUndercoatL <= m_nUndercoatR && m_nUndercoatT <= m_nUndercoatB ){
        // 領域の調整（処理範囲から１ピクセル拡大させる＝塗りガイドの末端が含まれるように）
        if( m_nUndercoatL > 0 ){ m_nUndercoatL--; }
        if( m_nUndercoatR < (m_nDotW-1) ){ m_nUndercoatR++; }
        if( m_nUndercoatT > 0 ){ m_nUndercoatT--; }
        if( m_nUndercoatB < (m_nDotH-1) ){ m_nUndercoatB++; }

        // 管理に対してバッファの領域を通知
        CBmpGenerator::UpdateFillGuideBufInfo( m_nUndercoatL, m_nUndercoatT );    // 左上
        CBmpGenerator::UpdateFillGuideBufInfo( m_nUndercoatR, m_nUndercoatB );    // 右下

        //-------------------------------------------------------
        // 本塗り（※塗りガイドで下塗りされている画素を色バッファに反映させる）
        //-------------------------------------------------------
        // 広域加算／減算
        if( isForAdd ){
            if( addVal < 0 ){
                if( isOnOutCol ){
                    exeFillSubOnOutCol( -addVal );
                }else{
                    exeFillSub( testPalGrp, -addVal );
                }
            }else if( addVal > 0 ){
                if( isOnOutCol ){
                    exeFillAddOnOutCol( addVal );
                }else{
                    exeFillAdd( testPalGrp, addVal );
                }
            }
        }
        // 単色塗り
        else if( m_eType == eFILL_TYPE_MONO ){
            if( palVal == BEZIER_PAL_VAL_BASE ){
                // 基本色が指定された場合は画素の明暗を引き継ぐ塗りつぶし
                if( isOnOutCol ){
                    exeFillMonoOnOutCol( palGrp );
                }else{
                    exeFillMono( palGrp, testPalGrp );
                }
            }else{
                // 基本色以外であれば差分を指定して塗りつぶし
                int addVal = (int)palVal - BEZIER_PAL_VAL_BASE;
                if( addVal > 0 ){
                    if( isOnOutCol ){
                        exeFillMonoAddOnOutCol( palGrp, addVal );
                    }else{
                        exeFillMonoAdd( palGrp, testPalGrp, addVal );
                    }
                }else{
                    if( isOnOutCol ){
                        exeFillMonoSubOnOutCol( palGrp, -addVal );
                    }else{
                        exeFillMonoSub( palGrp, testPalGrp, -addVal );
                    }
                }
            }
        }
        // オフ塗り
        else if( m_eType == eFILL_TYPE_OFF ){
            exeFillOff();
        }
        // ここに来たら困る
        else{
            LOGE( "@ CFill::fill0: INVALID type=%d\n", m_eType );
            return( false );
        }
    }
    
    return( true );
}

//------------------------------
// オフセット（別画素参照）による加算
//------------------------------
bool CFill::ofsAdd( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int ofsX, int ofsY, int addVal, eFILL_OPTION type ){
    // 用心：対象バッファが無効
    if( pBuf == NULL || pBufGuide == NULL || pBufGuard == NULL ){
        LOGE( "CFill::ofsAdd: INVALID BUF: pBuf=%p, pBufGuide=%p, pBufGuard=%p\n", pBuf, pBufGuide, pBufGuard );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::ofsAdd: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：オフセットが無効
    if( ofsX == 0 && ofsY == 0 ){
        LOGE( "CFill::ofsAdd: INVALID OFFSET: ofsX=%d, ofsY=%d\n", ofsX, ofsY );
        return( false );
    }
    
    // 用心：広域／全色時：加算値が０であれば無視
    if( addVal == 0 ){
        LOGE( "CFill::ofsAdd: INVALID VALN: addVal=%d\n", addVal );
        return( false );
    }
    
    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;
    m_pBufGuide = pBufGuide;
    m_pBufGuard = pBufGuard;
    m_nDotW = w;
    m_nDotH = h;
    
    // ここは下塗りせずに、色／ガイドの状況だけで処理される
    if( type == eFILL_OPTION_RESERVE ){
        if( addVal < 0 ){
            exeOfsSubForReserve( ofsX, ofsY, -addVal );
        }else if( addVal > 0 ){
            exeOfsAddForReserve( ofsX, ofsY, addVal );
        }
    }else if( type == eFILL_OPTION_INTO ){
        if( addVal < 0 ){
            exeOfsSubForInto( ofsX, ofsY, -addVal );
        }else if( addVal > 0 ){
            exeOfsAddForInto( ofsX, ofsY, addVal );
        }
    }else if( type == eFILL_OPTION_COLOR ){
        if( addVal < 0 ){
            exeOfsSubForColor( ofsX, ofsY, -addVal );
        }else if( addVal > 0 ){
            exeOfsAddForColor( ofsX, ofsY, addVal );
        }
    }else{
        // ここにきたら困る
        LOGE( "@ CFill::ofsAdd: INVALID type=%d\n", type );
        return( false );
    }
    
    return( true );
}

//--------------------------------------------------------------------------------------------------
// マスク
//--------------------------------------------------------------------------------------------------
bool CFill::mask( BYTE* pBuf, BYTE* pBufGuide, int w, int h, int x, int y ){
    // 用心：バッファが無効
    if( pBuf == NULL || pBufGuide == NULL ){
        LOGE( "CFill::mask: INVALID BUF: pBuf(Guard)=%p, pBufGuide=%p\n", pBuf, pBufGuide );
        return( false );
    }
    
    // 用心：サイズが無効
    if( w <= 0 || h <= 0 ){
        LOGE( "CFill::mask: INVALID SIZE: w=%d, h=%d\n", w, h );
        return( false );
    }

    // 用心：座標が無効
    if( x < 0 || x >= w || y < 0 || y >= h ){
        LOGE( "CFill::mask: INVALID POSITION: x=%d/%d, y=%d/%d\n", x, w, y, h );
        return( false );
    }

    //-------------------------------
    // 準備
    //-------------------------------
    m_pBuf = pBuf;      // ここに指定されるのがガードバッファとなる想定
    m_pBufGuide = pBufGuide;
    m_pBufGuard = NULL; // ここでは利用しない
    m_nDotW = w;
    m_nDotH = h;

    m_nUndercoatL = w;
    m_nUndercoatR = -1;
    m_nUndercoatT = h;
    m_nUndercoatB = -1;

    readyForUndercoat();

    //---------------------------------------------------
    // 下塗り（※塗りガイドの[x,y]の位置から閉じた範囲を塗りつぶす）
    //---------------------------------------------------
    fillUndercoat( x, y );
    
    // 下塗り範囲が有効であれば
    if( m_nUndercoatL <= m_nUndercoatR && m_nUndercoatT <= m_nUndercoatB ){
        // 領域の調整（処理範囲から１ピクセル拡大させる＝塗りガイドの末端が含まれるように）
        if( m_nUndercoatL > 0 ){ m_nUndercoatL--; }
        if( m_nUndercoatR < (m_nDotW-1) ){ m_nUndercoatR++; }
        if( m_nUndercoatT > 0 ){ m_nUndercoatT--; }
        if( m_nUndercoatB < (m_nDotH-1) ){ m_nUndercoatB++; }

        // マスクの領域は管理しない（※マスクの領域はピンポイントでのみ参照されるため、走査される機会がない）
        
        //-------------------------------------------------------
        // 本塗り（※塗りガイドで下塗りされている画素を色バッファに反映させる）
        //-------------------------------------------------------
        exeMask();
    }
    
    return( true );
}
