/*+----------------------------------------------------------------+
  |	Title:		BezierForWork.cpp [共通環境]
  |	Comment:	ベジェ曲線：ワーク
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

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
//----------------------------------------------------------------------------
// [Work] ワークパス用のテクスチャドットの作成
// この時点でのドット値はフラグとしての値となる[1]（※利用側がドットを置くかの判断につかうのみ）
//----------------------------------------------------------------------------
void CBezier::ReadyDotForWorkPath( void ){
    int size;
    
    // 各点の準備（※矩形はバッファ不要）
    s_nSizeForWorkPathLine = BEZIER_WP_LINE_SIZE * GM_P_RATE;
    s_nSizeForWorkPathPoint = BEZIER_WP_POINT_SIZE * GM_P_RATE;
    s_nSizeForWorkPathSelected = BEZIER_WP_SELECTED_SIZE * GM_P_RATE;

    // 方向線の先端部分（※三角形：ここでは基本形として上向き）
    s_nSizeForWorkPathTriangle = BEZIER_WP_TRIANGLE_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathTriangle;
    int m = size/5;
    int cW = (size-2*m)/2;
    for( int i=0; i<(size-2*m); i++ ){
        for( int j=0; j<(size-2*m); j++ ){
            int border = ((size-2*m)-i)/2;
            int check = -1;
            
            // 右半分
            if( cW <= j ){
                check = (size-2*m) - (j+1);
            }
            // 左半分
            else{
                check = j;
            }
            
            if( check >= border ){
                s_pDotForWorkPathTriangle[size*(i+m)+(j+m)] = 1;
            }
        }
    }
    
    // 塗りポイント（※バツ）
    s_nSizeForWorkPathCross = BEZIER_WP_CROSS_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathCross;
    for( int i=0; i<size; i++ ){
        for( int j=1; j<(size-1); j++ ){
            if( i==j || (size-1-i) == j ){
                s_pDotForWorkPathCross[size*(i-1)+j-1] = 1;
                s_pDotForWorkPathCross[size*(i-1)+j] = 1;
                s_pDotForWorkPathCross[size*(i-1)+j+1] = 1;

                s_pDotForWorkPathCross[size*i+j-1] = 1;
                s_pDotForWorkPathCross[size*i+j] = 1;
                s_pDotForWorkPathCross[size*i+j+1] = 1;
                
                s_pDotForWorkPathCross[size*(i+1)+j-1] = 1;
                s_pDotForWorkPathCross[size*(i+1)+j] = 1;
                s_pDotForWorkPathCross[size*(i+1)+j+1] = 1;
            }
        }
    }
    
    // スロットポイント（※プラス）
    s_nSizeForWorkPathPlus = BEZIER_WP_PLUS_SIZE * GM_P_RATE;
    size = s_nSizeForWorkPathPlus;
    m = size/2;
    
    // ヨコ
    for( int i=0; i<size; i++ ){
        s_pDotForWorkPathPlus[size*i+m-1] = 1;
        s_pDotForWorkPathPlus[size*i+m] = 1;
        s_pDotForWorkPathPlus[size*i+m+1] = 1;
    }
    // タテ
    for( int j=0; j<size; j++ ){
        s_pDotForWorkPathPlus[size*(m-1)+j] = 1;
        s_pDotForWorkPathPlus[size*(m)+j] = 1;
        s_pDotForWorkPathPlus[size*(m+1)+j] = 1;
    }
}

//----------------------------------
// [Work] ワークパス用：テクスチャの出力
//----------------------------------
void CBezier::PutTexForWorkPath( float x0, float y0, int size, BYTE* pTex, BYTE pal, int cut ){
    int w = size;
    int h = size;
    int iX = (int)(x0-(w-1)/2.0f);
    int iY = (int)(y0-(h-1)/2.0f);
    
    // 範囲確認（※領域外にはみ出していたら調整）
    if( iX < 0 ){
        w += iX;
        iX = 0;
    }else if( (iX+w) >= s_nBufW ){
        w += (s_nBufW-1) - (iX+w);
    }
    
    if( iY < 0 ){
        h += iY;
        iY = 0;
    }else if( (iY+h) >= s_nBufH ){
        h += (s_nBufH-1) - (iY+h);
    }
    
    // サイズが無効になったら無視
    if( w <= 0 || h <= 0 ){
        return;
    }
    
    // ここまできたらドットを出力
    for( int i=cut; i<(h-cut); i++ ){
        for( int j=cut; j<(w-cut); j++ ){
            int at = s_nBufW*(iY+i) + (iX+j);
            int texAt = size*i + j;
            
            // 有効なドット＆既存の値よりも大きければ出力
            if( pTex == NULL || pTex[texAt] != 0 ){
                BYTE target = s_pBuf[at];

                // 未出力／オフセットが大きい／値が大きい場合は出力（※上書き判定時、線は弱くしておく）
                if( target == 0 ){
                    s_pBuf[at] = pal;
                }else if( (pal&0xF) > (target&0xF) && (pal&0xF0) != BEZIER_WP_LINE_PAL ){
                    s_pBuf[at] = pal;
                }else if( (pal&0xF) == (target&0xF) && pal > target ){
                    s_pBuf[at] = pal;
                }
            }
        }
    }
}

//----------------------------------
// [Work] ワークパス用：線ドットの出力
//----------------------------------
void CBezier::PutLineDotForWorkPath( float x0, float y0, BYTE pal ){
    if( s_bTouch ){
        PutTexForWorkPath( x0, y0, s_nSizeForWorkPathLine/2, NULL, pal );
    }else{
        PutTexForWorkPath( x0, y0, s_nSizeForWorkPathLine, NULL, pal );
    }
}

//----------------------------------
// [Work] ワークパス用：点の出力
//----------------------------------
void CBezier::PutPointForWorkPath( float x0, float y0, BYTE pal ){
    // 選択中であればサイズを大きく（※常時アクティブ時は無視）
    if( (pal&0x0F) == BEZIER_WP_PAL_VAL_FOR_SELECTED && !IsDispPathActiveAlways() ){
        if( s_bTouch ){
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathSelected/2, NULL, pal );
        }else{
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathSelected, NULL, pal );
        }
    }else{
        if( s_bTouch ){
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPoint/2, NULL, pal );
        }else{
            PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPoint, NULL, pal );
        }
    }
}

//----------------------------------
// [Work] ワークパス用：三角の出力
//----------------------------------
static BYTE __bufForWorkPath[4*1024];
void CBezier::PutTriangleForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal, bool isFlip ){
    BYTE *pBuf = __bufForWorkPath;
    BYTE *pBuf0 = s_pDotForWorkPathTriangle;
    int size = s_nSizeForWorkPathTriangle;

    // バッファのクリア
    memset( pBuf, 0, size*size );

    // 方向の取得（※回転の都合で軸をずらして計算）
    float deg = CMath::GetDeg( ofsY, ofsX ) - 90;
    if( isFlip ){
        deg += 180;
    }
    
    float fSin = CMath::GetSin( deg );
    float fCos = CMath::GetCos( deg );

    // ソースを回転してバッファにコピー
    float hS = size / 2.0f;
    for( int i=0; i<size; i++ ){
        for( int j=0; j<size; j++ ){
            int x = (int)(fCos*(j-hS) - fSin*(i-hS) + hS);
            int y = (int)(fSin*(j-hS) + fCos*(i-hS) + hS);

            if( x >= 0 && x < size && y >= 0 && y < size ){
                if( pBuf0[size*y+x] != 0 ){
                    pBuf[size * i + j] = 1;
                }
            }
        }
    }

    // 作成したバッファで描画
    PutTexForWorkPath( x0+ofsX, y0+ofsY, size, pBuf, pal );
}

//----------------------------------
// [Work] ワークパス用：バツの出力
//----------------------------------
void CBezier::PutCrossForWorkPath( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathCross, s_pDotForWorkPathCross, pal );
}

//----------------------------------
// [Work] ワークパス用：プラスの出力
//----------------------------------
void CBezier::PutPlusForWorkPath( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPlus, s_pDotForWorkPathPlus, pal );
}

//----------------------------------
// [Work] ワークパス用：プラスの出力(小)
//----------------------------------
void CBezier::PutPlusForWorkPathS( float x0, float y0, BYTE pal ){
    PutTexForWorkPath( x0, y0, s_nSizeForWorkPathPlus, s_pDotForWorkPathPlus, pal, 2 );
}

//--------------------------------------------------
// [Work] ワークパス用：２点間を線形補間で描画（※方向線用）
//--------------------------------------------------
void CBezier::PutLinearForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal ){
    // 二点間のドット数の算出（※整数で判定）
    int distX = (int)ofsX;
    int distY = (int)ofsY;
    if( distX < 0 ){ distX = -distX; }
    if( distY < 0 ){ distY = -distY; }
    int div = ((distX>distY)? distX: distY);
    
    // 用心（※始点と終点にドット差分がない場合は終点が描画されるように）
    int start = 0;
    if( div <= 0 ){
        start = div = 1;
    }
    
    //----------------------------------------------------------------------------------------
    // 各ドットの描画（※[i<=div]なので終点[x1,y1]まで描画される）
    //----------------------------------------------------------------------------------------
    for( int i=start; i<=div; i++ ){
        float t = ((float)i)/div;
        float x = x0 + ofsX*t;
        float y = y0 + ofsY*t;
        
        int iX = (int)x;
        int iY = (int)y;
        
        PutLineDotForWorkPath( iX, iY, pal );
    }
}
