/*+----------------------------------------------------------------+
  | Title:      FillForUndercoat.cpp [共通環境]
  | Comment:    塗り：下塗り
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
//----------------------
// [下塗り]: 準備
//----------------------
void CFill::readyForUndercoat( void ){
    // ヘッダ位置のクリア
    m_nUndercoatCellHeadAt = m_nUndercoatCellTailAt = 0;

    // バッファ確保
    uint32 reqSize = (uint32)(FILL_UNDERCOAT_CELL_MAX * sizeof( stFILL_UNDERCOAT_CELL ));
    uint32 bufSize = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_FILL_CELL_OR_SS );
    m_stArrUndercoatCell = (stFILL_UNDERCOAT_CELL*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_FILL_CELL_OR_SS );

    // 領域不足なら（※ここに来たらバッファの拡張を検討）
    if( reqSize > bufSize ){
        LOGE( "@ CFill::readyForWork: BUF SHORTAGE: reqSize(%u) > bufSize(%u)\n", reqSize, bufSize );
        m_stArrUndercoatCell = NULL;
    }
}

//----------------------
// [下塗り]: セルの追加
//----------------------
void CFill::addUndercoatCell( int hint, int xL, int xR, int y ){
    stFILL_UNDERCOAT_CELL* pCell = &m_stArrUndercoatCell[m_nUndercoatCellTailAt];
    pCell->hint = hint;
    pCell->xL = xL;
    pCell->xR = xR;
    pCell->y = y;

    // 次の要素へ（※バッファの終端へ来ていたら循環させる）
    m_nUndercoatCellTailAt++;
    if( m_nUndercoatCellTailAt >= FILL_UNDERCOAT_CELL_MAX ){
        m_nUndercoatCellTailAt = 0;
    }
}

//----------------------
// [下塗り]: セルの取得
//----------------------
stFILL_UNDERCOAT_CELL* CFill::getUndercoatCell( void ){
    stFILL_UNDERCOAT_CELL* pCell = &m_stArrUndercoatCell[m_nUndercoatCellHeadAt];

    // 次の要素へ（※バッファの終端へ来ていたら循環させる）
    m_nUndercoatCellHeadAt++;
    if( m_nUndercoatCellHeadAt >= FILL_UNDERCOAT_CELL_MAX ){
        m_nUndercoatCellHeadAt = 0;
    }

    return( pCell );
}

//-----------------------------------------------
// [下塗り]: ガイド領域を[x,y]位置から下塗り色で塗りつぶす
//-----------------------------------------------
void CFill::fillUndercoat( int x, int y ){
    BYTE* p;
    int xL, xR;

    // 指定座標を追加（※開始時はなんのヒントもないので値は０）
    addUndercoatCell( 0x00, x, x, y );
    
    // セルが有効であれば
    while( ! isUndercoatCellEmpty() ){
        // 処理対象の取得
        stFILL_UNDERCOAT_CELL* pCell = getUndercoatCell();

        // 都度[m_nDotW*y+x]でアクセスするのもあれなので指定行を配列で取り出しておく
        y = pCell->y;
        p = &m_pBufGuide[m_nDotW*y];

        //------------------------------------------------------------------------------------------------
        // 指定位置が有効色であれば無視
        //------------------------------------------------------------------------------------------------
        // [UNDERCOAT_COL]色の書き込みは[fillUndercoat]でのみ行われるが、この時、横方向に連なる領域が塗りつぶされる
        // また、ワークセルに登録されて処理される対象の区域は、登録時には[未出力＝抜き色:0x00]であったことが保証される
        // もし、[xL-xR]内のいずれかのドットが対象色で無くなっていた場合、先行するワークセルにより塗りつぶされたと判断できる
        //------------------------------------------------------------------------------------------------
        if( IS_NOT_BFG_CLEAR_COL( p[pCell->xL] ) ){
            continue;
        }
        
        // 左サイドの確認（※閉じていなければ指定値の先を走査）
        xL = pCell->xL;
        if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_CLOSE_LEFT) == 0 ){
            while( xL > 0 && IS_BFG_CLEAR_COL( p[xL-1] ) ){
                xL--;
            }
        }

        // 右サイドの確認（※閉じていなければ指定値の先を走査）
        xR = pCell->xR;
        if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_CLOSE_RIGHT) == 0 ){
            while( (xR+1) < m_nDotW && IS_BFG_CLEAR_COL( p[xR+1] ) ){
                xR++;
            }
        }

        // ここまできたら指定範囲を下塗り
        if( xR >= xL ){
            for( int i=xL; i<=xR; i++ ){
                SET_BFG_UNDERCOAT_COL( p[i] );
            }
        }

        // 処理範囲の更新
        if( m_nUndercoatL > xL ){
            m_nUndercoatL = xL;
        }
        if( m_nUndercoatR < xR){
            m_nUndercoatR = xR;
        }
        if( m_nUndercoatT > y ){
            m_nUndercoatT = y;
        }
        if( m_nUndercoatB < y ){
            m_nUndercoatB = y;
        }

        // 一つ上の走査
        if( y > 0 ){
            // 上からきていた場合は調整（※処理済みの領域に対して逆走しない）
            if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_MOVE_DOWN) != 0 ){
                // 左側の未走査をチェック
                if( xL < pCell->xL ){
                    scanLineForUndercoat( xL, pCell->xL-1, y-1, true );
                }
                // 右側の未走査をチェック
                if( xR > pCell->xR ){
                    scanLineForUndercoat( pCell->xR+1, xR, y-1, true );
                }
            }else{
                scanLineForUndercoat( xL, xR, y-1, true );
            }
        }
        
        // 一つ下の走査
        if( y < (m_nDotH-1) ){
            // 下からきていた場合は調整（※処理済みの領域に対して逆走しない）
            if( (pCell->hint&FILL_UNDERCOAT_SCAN_HINT_MOVE_UP) != 0 ){
                // 左側の未走査をチェック
                if( xL < pCell->xL ){
                    scanLineForUndercoat( xL, pCell->xL-1, y+1, false );
                }
                // 右側の未走査をチェック
                if( xR > pCell->xR ){
                    scanLineForUndercoat( pCell->xR+1, xR, y+1, false );
                }
            }else{
                scanLineForUndercoat( xL, xR, y+1, false );
            }
        }
    }
}

//------------------------------------------------
// [下塗り]: 下塗り領域を走査して塗り対象であればセルに登録
//------------------------------------------------
void CFill::scanLineForUndercoat( int xL, int xR, int y, bool isMoveUp ){
    BYTE* p = &m_pBufGuide[m_nDotW*y];
    int xStart = xL;
    while( xStart <= xR ){
        // 有効な画素は無視
        while( xStart <= xR && IS_NOT_BFG_CLEAR_COL( p[xStart] ) ){
            xStart++;
        }
        
        // 対象色が見つからなければ終了
        if( xStart > xR ){
            return;
        }
        
        // クリア色が途切れるまで走査
        int xEnd = xStart;
        while( (xEnd+1) <= xR && IS_BFG_CLEAR_COL( p[xEnd+1] ) ){
            xEnd++;
        }

        // ここまできたらヒントと座標の登録
        int hint = (isMoveUp)? FILL_UNDERCOAT_SCAN_HINT_MOVE_UP: FILL_UNDERCOAT_SCAN_HINT_MOVE_DOWN;
        
        // 左端が内側：左サイドが閉じている
        if( xStart > xL ){
            hint |= FILL_UNDERCOAT_SCAN_HINT_CLOSE_LEFT;
        }
        // 右端が内側：右サイドが閉じている
        if( xEnd < xR ){
            hint |= FILL_UNDERCOAT_SCAN_HINT_CLOSE_RIGHT;
        }

        // ここまできたら登録
        addUndercoatCell( hint, xStart, xEnd, y );
        xStart = xEnd + 1;
    }
}
