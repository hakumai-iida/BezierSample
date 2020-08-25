/*+----------------------------------------------------------------+
  |	Title:		BezierForEdge.cpp [共通環境]
  |	Comment:	ベジェ曲線：エッジ塗り
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
// 外側を塗る処理（※閉領域の内側を１点で指定するのが無理なので）
#define STROKE_EDGE_FILL_OUTER_AREA

// ストロークの先端塗りのログ指定
//#define STROKE_EDGE_FILL_LOG

// ストロークの先端ぬりの下限／上限／高さ制限
#define STROKE_EDGE_FILL_BORDER_MIN         (2*GM_P_RATE)
#define STROKE_EDGE_FILL_BORDER_MAX         (s_nBufSizeForEdgeFillGuide/12)
#define STROKE_EDGE_FILL_BORDER_HEIGHT      (1*GM_P_RATE)

// 塗りガイド（※[CFill]とは別の管理）
#define EDGE_FILL_GUIDE_CLEAR_COL           0
#define EDGE_FILL_GUIDE_DONE_COL            1
#define EDGE_FILL_GUIDE_IGNORE_COL          2
#define EDGE_FILL_GUIDE_BORDER_COL          3

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
//------------------------------------------
// [Edge] 指定の分割点を中心にストロークのエッジ埋め
//------------------------------------------
void CBezier::DrawStrokeEdge( int targetDivisionPoint ){
    // 無効は無視
    if( targetDivisionPoint < 0 || targetDivisionPoint >= s_nUseDivisionPoint ){
        return;
    }

    stBEZIER_DIVISION_POINT* pTarget = &s_stArrDivisionPoint[targetDivisionPoint];
    stBEZIER_ANCHOR_POINT* pAP = pTarget->pAP;

    // ストロークが無効であれば無視（※目に見える線でなければエッジを埋める必要はない）
    CStroke* pStroke = GetStrokeWithAnchorPoint( pAP );
    if( pStroke == NULL ){
        return;
    }

    //---------------------------------------------------------------------------------------------------------
    // エッジ塗りのガイドをリセット（※エッジ塗りは自前の塗りガイドを利用する）
    //---------------------------------------------------------------------------------------------------------
    // エッジ塗りは、[EDGE_FILL_GUIDE_CLEAR_COL＝塗られていない]、[EDGE_FILL_GUIDE_DONE_COL＝塗られている]の２値で処理する
    //---------------------------------------------------------------------------------------------------------
    memset( s_pBufForEdgeFillGuide, EDGE_FILL_GUIDE_CLEAR_COL, sizeof(BYTE)*s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide );

    //---------------------------------------------
    // 起点の設定（※対象のAPが塗りガイドの中央に来るように）
    //---------------------------------------------
    float baseX = pTarget->x;
    float baseY = pTarget->y;
    int ofsX = s_nBufSizeForEdgeFillGuide/2 - (int)baseX;
    int ofsY = s_nBufSizeForEdgeFillGuide/2 - (int)baseY;

    // 確認する領域
    int borderL = baseX;
    int borderR = baseX;
    int borderT = baseY;
    int borderB = baseY;

    // 検出するポイント数（※この値が大きいほどエッジの塗りつぶしが大きくなる＝ブラシのサイズにストロークとエッジ補正を適用する）
    int numCheck = (int)(4*pStroke->getSize()*pAP->strokeSize*pAP->strokeEdgeFillSize);
    
#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ numCheck = %d\n", numCheck );
    LOGD( "@ pStroke->GetSize() = %f\n", pStroke->getSize() );
    LOGD( "@ pAP->size = %f\n", pAP->size );
    LOGD( "@ pAP->strokeSize = %f\n", pAP->strokeSize );
    LOGD( "@ pAP->strokeEdgeFillSize = %f\n", pAP->strokeEdgeFillSize );
    LOGD( "@ STROKE_EDGE_FILL_BORDER_MIN = %d\n", STROKE_EDGE_FILL_BORDER_MIN );
    LOGD( "@ STROKE_EDGE_FILL_BORDER_MAX = %d\n", STROKE_EDGE_FILL_BORDER_MAX );
#endif

    // 一定を下回ったら無視（※小すぎる値は処理しない）
    if( numCheck <= STROKE_EDGE_FILL_BORDER_MIN ){
#ifdef STROKE_EDGE_FILL_LOG
        LOGD(  "@@@@ [NG] numCheck = %d/%d\n", numCheck, STROKE_EDGE_FILL_BORDER_MIN );
#endif
        return;
    }

    // 一定を上回ったら調整（※大きぎる値は制限＝開発時の拡大表示等）
    if( numCheck > STROKE_EDGE_FILL_BORDER_MAX ){
#ifdef STROKE_EDGE_FILL_LOG
        LOGD(  "@@@@ [LIMIT] numCheck = %d/(%d)\n", numCheck, STROKE_EDGE_FILL_BORDER_MAX );
#endif
        numCheck = STROKE_EDGE_FILL_BORDER_MAX;
    }

    // work
    stBEZIER_DIVISION_POINT* pTemp0;
    stBEZIER_DIVISION_POINT* pTemp1;
    int num, point, at;
    int numRest, pointAt;

    //--------------------------------------
    // 手前の線（侵入してくるストローク）を確認
    //--------------------------------------
    float frontX = baseX;
    float frontY = baseY;
    float frontNearX = baseX;
    float frontNearY = baseY;

    pTemp0 = NULL;
    pTemp1 = pTarget;
    numRest = numCheck;
    pointAt = targetDivisionPoint - 1;
    while( numRest > 0 ){
        // 対象が始点を超えた場合
        if( pointAt < 0 ){
            pointAt = s_nUseDivisionPoint - 1;  // 終点へ

            // クローズパスでなければ抜ける（※以降のポイントが存在しない）
            if( s_stArrDivisionPoint[pointAt].pAP != s_stArrDivisionPoint[0].pAP ){
                break;
            }
            
            // クローズパスであればもう一つ先へ（※終点は重複登録されているので）
            pointAt--;
        }

        // 対象の分割点の取り出し
        pTemp0 = &s_stArrDivisionPoint[pointAt];
        
        // [pTemp0]〜[pTemp1]間の座標を登録
        point = CalcThenSetPointForTempBuf( pTemp0, pTemp1, pTemp0->pAP );
        num = (numRest>point)? point: numRest;
        
        for( int i=0; i<num; i++ ){
            frontX = s_fArrTempPointX[point-(i+1)];
            frontY = s_fArrTempPointY[point-(i+1)];

            at = ((int)frontY+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)frontX+ofsX);
            s_pBufForEdgeFillGuide[at] = EDGE_FILL_GUIDE_DONE_COL;

            // 領域の確認
            if( frontX < borderL ){ borderL = frontX; }
            if( frontX > borderR ){ borderR = frontX; }
            if( frontY < borderT ){ borderT = frontY; }
            if( frontY > borderB ){ borderB = frontY; }
        }
        
        // 塗りつぶし位置算出用（※基準点から基本ドット前の点）
        if( numRest >= (numCheck-STROKE_EDGE_FILL_BORDER_HEIGHT) ){
            frontNearX = frontX;
            frontNearY = frontY;
        }
        
        // 処理した分だけ残り回数を減らす
        numRest -= num;

        // 一つ前へ
        pTemp1 = pTemp0;
        pointAt--;
    }
    
    //--------------------------------------
    // 起点からの線（出立するストローク）を確認
    //--------------------------------------
    float backX = baseX;
    float backY = baseY;
    float backNearX = baseX;
    float backNearY = baseY;

    pTemp0 = pTarget;
    pTemp1 = NULL;
    numRest = numCheck;
    pointAt = targetDivisionPoint + 1;
    while( numRest > 0 ){
        // 対象が終点に達した場合（※終点は重複登録されるため）
        if( pointAt >= (s_nUseDivisionPoint-1) ){
            pointAt = 0;  // 始点へ
            
            // クローズパスでなければ抜ける（※以降のポイントが存在しない）
            if( s_stArrDivisionPoint[pointAt].pAP != s_stArrDivisionPoint[s_nUseDivisionPoint-1].pAP ){
                break;
            }
        }
        
        // 対象の分割点の取り出し
        pTemp1 = &s_stArrDivisionPoint[pointAt];
        
        // [pTemp0]〜[pTemp1]間の座標を登録
        point = CalcThenSetPointForTempBuf( pTemp0, pTemp1, pTemp0->pAP );
        num = (numRest>point)? point: numRest;
        
        for( int i=0; i<num; i++ ){
            backX = s_fArrTempPointX[i];
            backY = s_fArrTempPointY[i];
            
            at = ((int)backY+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)backX+ofsX);
            s_pBufForEdgeFillGuide[at] = EDGE_FILL_GUIDE_DONE_COL;
            
            // 領域の確認
            if( frontX < borderL ){ borderL = frontX; }
            if( frontX > borderR ){ borderR = frontX; }
            if( frontY < borderT ){ borderT = frontY; }
            if( frontY > borderB ){ borderB = frontY; }
        }
        
        // 塗りつぶし位置算出用（※基準点から基本ドット後ろの点）
        if( numRest > (numCheck-STROKE_EDGE_FILL_BORDER_HEIGHT) ){
            backNearX = backX;
            backNearY = backY;
        }

        // 処理した分だけ残り回数を減らす
        numRest -= num;
        
        // 一つ後ろへ
        pTemp0 = pTemp1;
        pointAt++;
    }
    
    //------------------------------------
    // ３点による三角形の確認
    //------------------------------------
    float centerX = (baseX + frontX + backX) / 3.0f;
    float centerY = (baseY + frontY + backY) / 3.0f;
    float height = CMath::Len( baseX-centerX, baseY-centerY );

#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ STROKE EDGE FILL LOG\n" );
    LOGD( "@ ofsX=%d, ofsY=%d\n", ofsX, ofsY );
    LOGD( "@ baseX=%f, baseY=%f\n", baseX, baseY );
    LOGD( "@ frontX=%f(%f), frontY=%f(%f)\n", frontX, frontNearX, frontY, frontNearY );
    LOGD( "@ backX=%f(%f), backY=%f(%f)\n", backX, backNearX, backY, backNearY );
    LOGD( "@ centerX = %f, centerY = %f\n", centerX, centerY );
    LOGD( "@ height = %f(%d)\n", height, STROKE_EDGE_FILL_BORDER_HEIGHT );
#endif
    
    // 三角形の高さが一定以上なければ無視（※塗りつぶしの面積が無い）
    if( height < STROKE_EDGE_FILL_BORDER_HEIGHT ){
#ifdef STROKE_EDGE_FILL_LOG
        LOGD( "@ SKIP: HEIGHT NOT ENOUGH\n" );
#endif
        return;
    }
    
    //------------------------------------
    // 前後のストローク間を補完
    //------------------------------------
    // 出立＆侵入方向の算出
    float outX = 2.0f*(baseX-frontX)/3.0f;
    float outY = 2.0f*(baseY-frontY)/3.0f;
    float inX = 2.0f*(baseX-backX)/3.0f;
    float inY = 2.0f*(baseY-backY)/3.0f;

    int x0 = (int)frontX;
    int y0 = (int)frontY;
    int x1, y1;
    num = CalcPointNum( frontX, frontY, backX, backY );
    // ループ範囲は始点／終点を含めておく（※始点／終点前後でピクセル飛びが起こらないように）
    for( int i=0; i<=num; i++ ){
        // t, (1-t)
        float t1 = ((float)i)/num;
        float tR1 = 1.0f - t1;
        
        // t^2, (1-t)^2
        float t2 = t1 * t1;
        float tR2 = tR1 * tR1;
        
        // t^3, (1-t)^3
        float t3 = t2 * t1;
        float tR3 = tR2 * tR1;
        
        // (1-t)^2*t, (1-t)*t^2
        float tR2_t1 = tR2 * t1;
        float tR1_t2 = tR1 * t2;
        
        // 分割点座標の算出（※ベジェ曲線）
        float x = frontX*tR3 + 3*(frontX+outX)*tR2_t1 + 3*(backX+inX)*tR1_t2 + backX*t3;
        float y = frontY*tR3 + 3*(frontY+outY)*tR2_t1 + 3*(backY+inY)*tR1_t2 + backY*t3;
        
        // ストロークの描画（※ここは１ドットのみ出力＝塗り判定用のガイドは完全に閉じる必要があるが、見た目の部分は多少距離が離れても違和感はないはず）
        PutDotAt( x, y, 1.0f, s_nTestPalGrp, pStroke, pAP );

        //------------------------------------------------------------------------------------
        // 塗りガイドが２ピクセル以上離れてしまっても領域が閉じるように前回描画座標からの距離でピクセルを処理する
        //------------------------------------------------------------------------------------
        x1 = (int)x;
        y1 = (int)y;

        int divX = x1-x0;
        int divY = y1-y0;
        if( divX < 0 ){ divX *= -1; }
        if( divY < 0 ){ divY *= -1; }
        int len = ((divX>divY)? divX: divY);

#if 1
        // ３以上であれば偶数調整（※分母が３になると「割り算→掛け算」の計算誤差で色々と困るので小細工）
        if( len >= 3 ){
            len = 2*((len+1)/2);
        }
#endif
        
        if( len > 0 ){
            for( int j=0; j<len; j++ ){
                t1 = ((float)j)/len;
                tR1 = 1.0f - t1;
                x = x1*tR1 + x0*t1;
                y = y1*tR1 + y0*t1;
                
                int at = ((int)y+ofsY)*s_nBufSizeForEdgeFillGuide + ((int)x+ofsX);
                s_pBufForEdgeFillGuide[at] = EDGE_FILL_GUIDE_DONE_COL;
                
                // 領域の確認
                if( x < borderL ){ borderL = x; }
                if( x > borderR ){ borderR = x; }
                if( y < borderT ){ borderT = y; }
                if( y > borderB ){ borderB = y; }
            }
            
            x0 = x1;
            y0 = y1;
        }
    }

    //-----------------
    // 領域検証
    //-----------------
    borderL += ofsX;
    borderR += ofsX;
    borderT += ofsY;
    borderB += ofsY;
    
    // 空き枠＋保護塗り分拡張（※開始位置判定で空き枠を想定するので保護マスの隣に空きますを確保する）
    borderL -= 2;
    borderR += 2;
    borderT -= 2;
    borderB += 2;

    bool isInvalid = false;
    if( borderL < 0 || borderL >= s_nBufSizeForEdgeFillGuide ){ isInvalid = true; }
    else if( borderR < 0 || borderR >= s_nBufSizeForEdgeFillGuide ){ isInvalid = true; }
    else if( borderT < 0 || borderT >= s_nBufSizeForEdgeFillGuide ){ isInvalid = true; }
    else if( borderB < 0 || borderB >= s_nBufSizeForEdgeFillGuide ){ isInvalid = true; }
    if( isInvalid ){
        LOGE(  "@ CBezier::FillStrokeEdge: INVALID BORDER: l=%d, r=%d, t=%d, b=%d\n", borderL, borderR, borderT, borderB );
        return;
    }

#ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ BORDER: l=%d, r=%d, t=%d, b=%d\n", borderL, borderR, borderT, borderB );
#endif

    //-----------------------------------------------------------------------
    // 保護領域（※万一塗り漏れした場合に無限ループにならないように特殊な値で領域を閉じておく）
    //-----------------------------------------------------------------------
    memset( &s_pBufForEdgeFillGuide[s_nBufSizeForEdgeFillGuide*borderT+borderL], EDGE_FILL_GUIDE_BORDER_COL, borderR-borderL+1 );
    memset( &s_pBufForEdgeFillGuide[s_nBufSizeForEdgeFillGuide*borderB+borderL], EDGE_FILL_GUIDE_BORDER_COL, borderR-borderL+1 );

    for( int i=borderT; i<=borderB; i++ ){
        s_pBufForEdgeFillGuide[s_nBufSizeForEdgeFillGuide*i+borderL] = EDGE_FILL_GUIDE_BORDER_COL;
        s_pBufForEdgeFillGuide[s_nBufSizeForEdgeFillGuide*i+borderR] = EDGE_FILL_GUIDE_BORDER_COL;
    }

#ifdef STROKE_EDGE_FILL_OUTER_AREA
    //=================================================================
    // 閉領域の外側を塗りつぶした結果、塗られていないマスが領域の内側とする処理
    //（※内側を指定する処理よりも若干遅いが、塗り漏れがないのでこちらを採用しておく）
    //=================================================================
    s_pApForEdgeFillGuide = pAP;
    s_nOfsXForEdgeFillGuide = ofsX;
    s_nOfsYForEdgeFillGuide = ofsY;

    // ボーダーの左上(1,1)を起点に外側を塗りつぶす
    FillStrokeEdgeAt( 0, borderL+1, borderT+1 );

    // 塗られていないマスが内側となる
    for( int i=borderT; i<=borderB; i++ ){
        for( int j=borderL; j<=borderR; j++ ){
            int at = i*s_nBufSizeForEdgeFillGuide + j;

            #ifdef STROKE_EDGE_FILL_LOG
            if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_CLEAR_COL ){
                LOGD( "." );
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_BORDER_COL ){
                LOGD( "#" );
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_IGNORE_COL ){
                LOGD( "-" );
            }else{
                LOGD( "@" );
            }
            #endif

            if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_CLEAR_COL ){
                // ストロークの塗りつぶし（※出力の判定は[PutDotAt]と同じにしておくこと）
                int bufAt = s_nBufW*(i-ofsY) + (j-ofsX);
                
                //-------------------------------------------
                // 出力が可能か？（※ここは本来の塗りガイドを参照する）
                //-------------------------------------------
                bool checkResult = true;
                if( s_pBufFillGuard[bufAt] == 0 ){
                    if( !pAP->isDotPutForce && pAP->isDotPutOnOutCol ){
                        checkResult = IS_BFG_OUT_COL( s_pBufFillGuide[bufAt] ) ;
                    }
                }else{
                    checkResult = false;
                }

                // ここは画素を出力するのみ（※周辺のデリケートなストロークは出力済みのはずなので最大値で描き込んでもなじむはず）
                if( checkResult && BP_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt]) == s_nTestPalGrp ){
                    // 線バッファへ出力（※[CBmpGenerator::UpdateLineBufInfo]は不要＝エッジ塗りは描画済みの領域の内側に収まる想定）
                    s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
                }
            }
        }
        #ifdef STROKE_EDGE_FILL_LOG
        LOGD( "\n" );
        #endif
    }
#else
    //=======================================================================
    // 閉領域の内側を特定して塗りつぶすやり方はどうしてもうまくいかないのでボツ
    //=======================================================================
    //-----------------------------------------------------------------------
    // ストロークの塗りつぶし開始（※基本座標から前後のストローク方向へ少しずらした位置）
    //-----------------------------------------------------------------------
    int startX = (int)((baseX+2*frontNearX+2*backNearX)/5 + ofsX);
    int startY = (int)((baseY+2*frontNearY+2*backNearY)/5 + ofsY);

    #ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ FILL_POINT: startX0=%d, startY0=%d\n", startX, startY );

    LOGD( "@ STROKE EDGE GUIDE: BEFORE\n" );
    for( int i=borderT; i<=borderB; i++ ){
        for( int j=borderL; j<=borderR; j++ ){
            int at = i*s_nBufSizeForEdgeFillGuide + j;
            if( i==startY && j==startX ){
                if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_CLEAR_COL ){
                    LOGD( "!" );
                }else{
                    LOGD( "?" );
                }
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_CLEAR_COL ){
                LOGD( "." );
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_BORDER_COL ){
                LOGD( "#" );
            }else{
                LOGD( "@" );
            }
        }
        LOGD( "\n" );
    }
    #endif
    
    // 塗り漏れ防止確認
    centerX = (borderL+borderR)/2;
    centerY = (borderT+borderB)/2;
    if( ! CheckFillStrokeEdgeStartAt( &startX, &startY, borderL, borderR, borderT, borderB, (startX>centerX), (startY>centerY) ) ){
        LOGE(  "@ CBezier::FillStrokeEdge: INVALID FILL POINT: startX=%d, startY=%d\n", startX, startY );
        return;
    }
    
    #ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ FIXED FILL POINT: startX=%d, startY=%d\n", startX, startY );
    #endif

    // ここまできたらエッジ塗り
    s_pApForEdgeFillGuide = pAP;
    s_nOfsXForEdgeFillGuide = ofsX;
    s_nOfsYForEdgeFillGuide = ofsY;

    // TODO:開始点／終了点付近で微妙な閉領域が残ってしまうことがあるがどうにかならないか？（※小さい領域なので濡れていなくても目視できないかも？）
    FillStrokeEdgeAt( 0, startX, startY );

    #ifdef STROKE_EDGE_FILL_LOG
    LOGD( "@ STROKE EDGE GUIDE: AFTER\n" );
    for( int i=borderT; i<=borderB; i++ ){
        for( int j=borderL; j<=borderR; j++ ){
            int at = i*s_nBufSizeForEdgeFillGuide + j;
            if( i==startY && j==startX ){
                LOGD( "!" );
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_CLEAR_COL ){
                LOGD( "." );
            }else if( s_pBufForEdgeFillGuide[at] == EDGE_FILL_GUIDE_BORDER_COL ){
                LOGD( "#" );
            }else{
                LOGD( "@" );
            }
        }
        LOGD( "\n" );
    }
    #endif
#endif
}

//------------------------------------------
// [Edge] エッジ塗り開始座標確認（※塗り漏れ防止）
//------------------------------------------
bool CBezier::CheckFillStrokeEdgeStartAt( int* pStartX, int* pStartY, int bL, int bR, int bT, int bB, bool isCheckLeft, bool isCheckUp ){
    // 用心
    if( pStartX == NULL || pStartY == NULL ){
        return( false );
    }

    // 用心
    if( *pStartX <= bL || *pStartX >= bR || *pStartY <= bT || *pStartY >= bB ){
        return( false );
    }

    // work
    int startX = *pStartX;
    int startY = *pStartY;
    int w = s_nBufSizeForEdgeFillGuide;

    // 基本位置が有効なら返す
    if( CheckFillStrokeEdgeAt( &startX, &startY, bL, bR, bT, bB ) ){
        if( s_pBufForEdgeFillGuide[startY*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            *pStartX = startX;
            *pStartY = startY;
            return( true );
        }
    }
    
    // １マス横にずらして判定
    startX = *pStartX;
    startY = *pStartY;
    if( isCheckLeft ){ startX--; }
    else{ startX++; }
    if( CheckFillStrokeEdgeAt( &startX, &startY, bL, bR, bT, bB ) ){
        if( s_pBufForEdgeFillGuide[startY*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            *pStartX = startX;
            *pStartY = startY;
            return( true );
        }
    }

    // １マス縦にずらして判定
    startX = *pStartX;
    startY = *pStartY;
    if( isCheckUp ){ startY--; }
    else{ startY++; }
    if( CheckFillStrokeEdgeAt( &startX, &startY, bL, bR, bT, bB ) ){
        if( s_pBufForEdgeFillGuide[startY*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            *pStartX = startX;
            *pStartY = startY;
            return( true );
        }
    }

    // １マスななめにずらして判定
    startX = *pStartX;
    startY = *pStartY;
    if( isCheckLeft ){ startX--; }
    else{ startX++; }
    if( isCheckUp ){ startY--; }
    else{ startY++; }
    if( CheckFillStrokeEdgeAt( &startX, &startY, bL, bR, bT, bB ) ){
        if( s_pBufForEdgeFillGuide[startY*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            *pStartX = startX;
            *pStartY = startY;
            return( true );
        }
    }

    // ここまできたら諦める
    return( false );
}

//------------------------------------------
// [Edge] エッジ塗り開始座標確認（※チェック実体）
//------------------------------------------
bool CBezier::CheckFillStrokeEdgeAt( int* pStartX, int* pStartY, int bL, int bR, int bT, int bB ){
    // work
    int startX = *pStartX;
    int startY = *pStartY;

    int leftAt = -1;
    int rightAt = -1;
    int topAt = -1;
    int bottomAt = -1;

    int at;
    int w = s_nBufSizeForEdgeFillGuide;

    //----------------------
    // 左側の領域を検索
    //----------------------
    at = startX - 1;
    while( at > bL ){
        // 壁にぶつかったら抜ける
        if( s_pBufForEdgeFillGuide[startY*w+at] != EDGE_FILL_GUIDE_CLEAR_COL ){
            break;
        }
        at--;
    }
    
    // １つ先の空きマスを検索
    at--;
    while( at > bL ){
        if( s_pBufForEdgeFillGuide[startY*w+at] == EDGE_FILL_GUIDE_CLEAR_COL ){
            leftAt = at;
            break;
        }
        at--;
    }

    //----------------------
    // 右側の領域を検索
    //----------------------
    at = startX + 1;
    while( at < bR ){
        // 壁にぶつかったら抜ける
        if( s_pBufForEdgeFillGuide[startY*w+at] != EDGE_FILL_GUIDE_CLEAR_COL ){
            break;
        }
        at++;
    }
    
    // １つ先の空きマスを検索
    at++;
    while( at < bR ){
        if( s_pBufForEdgeFillGuide[startY*w+at] == EDGE_FILL_GUIDE_CLEAR_COL ){
            rightAt = at;
            break;
        }
        at++;
    }

    // 左右にマスがなければ困る（閉じていない）
    if( leftAt < 0 && rightAt < 0 ){
        return( false );
    }
    
    //----------------------------------------
    // 左側にマスがなければ（※閉じているのは右側だけ）
    //----------------------------------------
    if( leftAt < 0 ){
        // 開始点を右側へ移動
        startX = rightAt;
        
        // 右側の領域を検索
        at = startX + 1;
        while( at < bR ){
            // 壁にぶつかったらぬける
            if( s_pBufForEdgeFillGuide[startY*w+at] != EDGE_FILL_GUIDE_CLEAR_COL ){
                break;
            }
            at++;
        }

        // 見つからなければ困る（閉じていない）
        if( at >= bR ){
            return( false );
        }
    }
    //----------------------------------------
    // 右側にマスがなければ（※閉じているのは左側だけ）
    //----------------------------------------
    else if( rightAt < 0 ){
        // 開始点を左側へ移動
        startX = leftAt;
        
        // 左側の領域を検索
        at = startX - 1;
        while( at > bL ){
            // 壁にぶつかったら
            if( s_pBufForEdgeFillGuide[startY*w+at] != EDGE_FILL_GUIDE_CLEAR_COL ){
                break;
            }
            at--;
        }

        // 見つからなければ困る（閉じていない）
        if( at <= bL ){
            return( false );
        }
    }
    
    //--------------------------
    // 上側の領域を検索
    //--------------------------
    at = startY - 1;
    while( at > bT ){
        // 壁にぶつかったら抜ける
        if( s_pBufForEdgeFillGuide[at*w+startX] != EDGE_FILL_GUIDE_CLEAR_COL ){
            break;
        }
        at--;
    }
    
    // １つ先の空きマスを検索
    at--;
    while( at > bT ){
        if( s_pBufForEdgeFillGuide[at*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            topAt = at;
            break;
        }
        at--;
    }
    
    //--------------------------
    // 下側の領域を検索
    //--------------------------
    at = startY + 1;
    while( at < bB ){
        // 壁にぶつかったら
        if( s_pBufForEdgeFillGuide[at*w+startX] != EDGE_FILL_GUIDE_CLEAR_COL ){
            break;
        }
        at++;
    }
    
    // １つ先の空きマスを検索
    at++;
    while( at < bB ){
        if( s_pBufForEdgeFillGuide[at*w+startX] == EDGE_FILL_GUIDE_CLEAR_COL ){
            bottomAt = at;
            break;
        }
        at++;
    }

    // 上下にマスがなければ困る（閉じていない）
    if( topAt < 0 && bottomAt < 0 ){
        return( false );
    }
    
    //----------------------------------------
    // 上側にマスがなければ（※閉じているのは下側だけ）
    //----------------------------------------
    if( topAt < 0 ){
        // 開始点を下側へ移動
        startY = bottomAt;

        // 下側の領域を検索
        at = startY + 1;
        while( at < bB ){
            // 壁にぶつかったら
            if( s_pBufForEdgeFillGuide[at*w+startX] != EDGE_FILL_GUIDE_CLEAR_COL ){
                break;
            }
            at++;
        }
        
        // 見つからなければ困る（閉じていない）
        if( at >= bB ){
            return( false );
        }
    }
    //----------------------------------------
    // 下側にマスがなければ（※閉じているのは上側だけ）
    //----------------------------------------
    else if( bottomAt < 0 ){
        // 開始点を上側へ移動
        startY = topAt;
        
        // 上側の領域を検索
        at = startY - 1;
        while( at > bT ){
            // 壁にぶつかったら
            if( s_pBufForEdgeFillGuide[at*w+startX] != EDGE_FILL_GUIDE_CLEAR_COL ){
                break;
            }
            at--;
        }
        
        // 見つからなければ困る（閉じていない）
        if( at <= bT ){
            return( false );
        }
    }
    
    // ここまできたら開始座標は閉領域に存在する
    *pStartX = startX;
    *pStartY = startY;
    return( true );
}

//-----------------------------------------------------------------
// [Edge] 指定位置のストロークを塗りガイドにより埋める
//-----------------------------------------------------------------
// 固定値を引数で渡すのを避けるため呼び出し前には下記の値を設定する必要がある
// ・s_nOfsXForEdgeFillGuide：描画オフセット：X
// ・s_nOfsYForEdgeFillGuide：描画オフセット：Y
// ・s_pApForEdgeFillGuide：ドットを出力するAP（※描画情報用）
//------------------------------------------------------------------
#ifdef STROKE_EDGE_FILL_LOG
static int __fill_stroke_depth = 0;
#endif
void CBezier::FillStrokeEdgeAt( int from, int x, int y ){
    // 塗られていたら無視
    int at = y*s_nBufSizeForEdgeFillGuide + x;
    if( s_pBufForEdgeFillGuide[at] != EDGE_FILL_GUIDE_CLEAR_COL ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth--;
#endif
        return;
    }
    
#ifndef STROKE_EDGE_FILL_OUTER_AREA
    // ストロークの塗りつぶし（※出力の判定は[PutDotAt]と同じにしておくこと）
    stBEZIER_ANCHOR_POINT* pAP = s_pApForEdgeFillGuide;
    int bufAt = s_nBufW*(y-s_nOfsYForEdgeFillGuide) + (x-s_nOfsXForEdgeFillGuide);
    
    //-------------------------------------------
    // 出力が可能か？（※ここは本来の塗りガイドを参照する）
    //-------------------------------------------
    bool checkResult = true;
    if( s_pBufFillGuard[bufAt] == 0 ){
        if( !pAP->isDotPutForce && pAP->isDotPutOnOutCol ){
            checkResult = IS_BFG_OUT_COL( s_pBufFillGuide[bufAt] ) ;
        }
    }else{
        checkResult = false;
    }

    // ここは画素を出力するのみ（※周辺のデリケートなストロークは出力済みのはずなので最大値で描き込んでもなじむはず）
    if( checkResult && BP_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt]) == s_nTestPalGrp ){
        // 線バッファへ出力（※[CBmpGenerator::UpdateLineBufInfo]は不要＝エッジ塗りは描画済みの領域の内側に収まる想定）
        s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
    }
    
    // 完了色
    s_pBufForEdgeFillGuide[at] = EDGE_FILL_GUIDE_DONE_COL;
#else
    // 無視色
    s_pBufForEdgeFillGuide[at] = EDGE_FILL_GUIDE_IGNORE_COL;
#endif
    
    // 左右上下へ（※[from]の値により移動元には行かないようにしておく）
    if( from != 1 && x > 0 ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        //LOGD( "@@@ [LEFT]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeEdgeAt( 2, x-1, y );
    }
    if( from != 2 && x < (s_nBufSizeForEdgeFillGuide-1) ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        //LOGD( "@@@ [RIGHT]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeEdgeAt( 1, x+1, y );
    }
    if( from != 3 && y > 0 ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        //LOGD( "@@@ [UP]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeEdgeAt( 4, x, y-1 );
    }
    if( from != 4 && y < (s_nBufSizeForEdgeFillGuide-1) ){
#ifdef STROKE_EDGE_FILL_LOG
        __fill_stroke_depth++;
        //LOGD( "@@@ [DOWN]: __fill_stroke_depth=%d\n", __fill_stroke_depth );
#endif
        FillStrokeEdgeAt( 3, x, y+1 );
    }
    
#ifdef STROKE_EDGE_FILL_LOG
    __fill_stroke_depth--;
#endif
}
