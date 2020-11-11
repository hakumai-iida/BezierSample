/*+----------------------------------------------------------------+
  |	Title:		BezierForFrill.cpp [共通環境]
  |	Comment:	ベジェ曲線：フリル
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

#include "draw/bezier/struct/BezierStructConv.hpp"

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
//--------------------------------------------------------------
// [Frill] 描画実体：フリル（※[pHead]は１点のみの指定を想定）
//--------------------------------------------------------------
void CBezier::DrawFrill( stBEZIER_ANCHOR_POINT* pHead ){
    // ワークパス表示時は無視（※ゴチャゴチャになるので対応しない）
    if( s_bWorkPath ){
        return;
    }
    
    // 乱数設定（※データに対して一律になってくれさえすれば良いのでアドレスをシードに突っ込んでみる）
    uintptr_t seed = (uintptr_t) pHead;
    CRand::SetSeedAt( eRAND_ID_BEZIER_FRILL, (uint32)seed, 0 );

    stBEZIER_ANCHOR_POINT* pHeadTmp = pHead;
    
    //-------------------------
    // データ数を調べる
    //-------------------------
    int numData = 0;
    while( pHeadTmp != NULL ){
        numData++;
        
        // 次の要素へ
        pHeadTmp = pHeadTmp->pNext;
        
        // 用心(※クローズパスであれば抜けておく）
        if( pHeadTmp == pHead ){
            break;
        }
    }

    // 要素数が１でなければ無視（※フリルの構成が不正）
    if( numData != 1 ){
        LOGE( "@ CBezier::DrawFrill: INVALID DATA NUM: numData=%d\n", numData );
        return;
    }
        
    //--------------------------------------------------------------
    // 処理対象データの確認
    //--------------------------------------------------------------
    stBEZIER_ANCHOR_POINT* pBAP = &pHead[0];

    // フリルデータの取得（※メインが無効の場合は無視）
    CLayerData* pMainLD = GetLayerDataForFrill( pBAP->frillMainId );
    if( pMainLD == NULL ){
        LOGE( "@ CBezier::Frill: INVALID MAIN FRILL DATA: frillId=%d\n", pBAP->frillMainId );
        return;
    }

    // サブの取得（※こちらは無効でも問題ない）
    CLayerData* pSubLD = NULL;
    if( IS_STROKE_FRILL_VALID( pBAP->frillSubId ) ){
       pSubLD = GetLayerDataForFrill( pBAP->frillSubId );
    }

    // タッチ対象の確認（※無効は無視）
    int idFixed = pBAP->touchTargetIdFixed;
    if( ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( idFixed ) ){
        LOGE( "@ CBezier::DrawFrill: INVALID TOUCH TARGET: idFixed=%d\n", idFixed );
        return;
    }
    
    // タッチ登録点の確認（※無効は無視）
    int numPoint = s_nArrTouchPointNum[idFixed];
    if( numPoint <= 0 ){
        LOGE( "@ CBezier::DrawFrill: NO TOUCH POINT: idFixed=%d, numPoint=%d\n", idFixed, numPoint );
        return;
    }
    
    //--------------------------------------------------------------
    // work
    //--------------------------------------------------------------
    // フリルの描画で傾き処理が行われるためここで扱う座標は傾き前の値
    float* arrX0 = s_fArrArrTouchPointX0[idFixed];
    float* arrY0 = s_fArrArrTouchPointY0[idFixed];
    float totalLen0 = CalcTouchWorkForLen0( idFixed );
    float baseLen0 = totalLen0 * pBAP->touchBasePos;

    // 起点となる座標の特定
    int pivotAt = SearchTouchIndexForLen0( idFixed, baseLen0, 0, true, pBAP->touchRefOfsRateForMain );
    if( pivotAt < 0 ){
        LOGE( "@ CBezier::DrawFrill: PIVOT SEARCH MISS: idFixed=%d, baseLen0=%f\n", idFixed, baseLen0 );
        return;
    }

    bool isFlipH = (s_pLayerParam->pixelScaleX < 0);
    bool isFlipV = (s_pLayerParam->pixelScaleY < 0);

    // サイズ取得（※[X/Y]は反転用にわかれているがスケールの絶対値的には同じになるはず＝縦横の辺のサイズは等しい想定）
    float layerSizeAbs = s_pLayerParam->pixelScaleX;
    if( layerSizeAbs < 0 ){ layerSizeAbs *= -1; }

    float baseW;
    int numFront = pBAP->touchFrontNum;
    int numBack = pBAP->touchBackNum;
    bool isFrontHeadSkip = false;
    bool isFrontTailSkip = false;
    bool isBackHeadSkip = false;
    bool isBackTailSkip = false;
    
    // 反転指定があれば調整（※ミラーコピーによる反転なので横方向のみ）
    if( pBAP->isTouchReverse ){
        isFlipH = !isFlipH;

        // 前方スキップ指定があれば
        if( pBAP->isTouchSkipFront ){
            // 前方のタッチが有効なら末端（タッチの並びでいうと一番若い）をスキップする
            if( numFront > 0 ){
                isFrontTailSkip = true;
            }
            // 後方のタッチが有効なら先端（タッチの並びで言うと一番若い）をスキップする
            else if( numBack > 0 ){
                isBackHeadSkip = true;
            }
        }

        // 後方スキップ指定があれば
        if( pBAP->isTouchSkipBack ){
            // 後方のタッチが有効なら末端（タッチの並びでいうと一番古い）をスキップする
            if( numBack > 0 ){
                isBackTailSkip = true;
            }
            // 前方のタッチが有効なら先端（タッチの並びでいうと一番古い）をスキップする
            else if( numFront > 0 ){
                isFrontHeadSkip = true;
            }
        }
    }

    bool isPivotDone = false;
    bool isPivotFixed = false;

    int at0 = -1;
    float rate0 = 0.0f;
    float len0 = 0.0f;
    float pivotLen0 = 0.0f;

    // メインとサブの接続用（※[hookX/hookY]の後ろから登録して真ん中を利用する）
    //（※起点メインの左右が[0][1]に登録され、後続メインの左右が[2][3]に登録し、サブが[1][2]を消費し、内容が[0][3]に前詰めされるイメージ）
    //（※上記の流れはタッチが右回りの時を想定＝タッチが左回りの場合は、[isHookReverse]にて参照先の反転を指定する）
    float hookX[4];
    float hookY[4];
    float pivotHookX0 = 0.0f;
    float pivotHookY0 = 0.0f;
    float pivotHookX1 = 0.0f;
    float pivotHookY1 = 0.0f;
    
    bool isHookReverse;
    int hook0At, hook1At;

    //---------------------------------
    // 前方の確認（※指定の要素数分表示する）
    //---------------------------------
    if( numFront > 0 ){
        // 末端となる座標の特定
        float frontEndLen = baseLen0 * (1.0f-pBAP->touchFrontArea);
        if( frontEndLen < 0.0f ){
            frontEndLen = 0.0f;
        }else if( frontEndLen > baseLen0 ){
            frontEndLen = baseLen0;
        }
        
        // 領域が有効であれば
        float frontRange = baseLen0 - frontEndLen;
        if( frontRange > 0 ){
            int at = pivotAt;
            float rate, size, len, rot;
            float lastLen = baseLen0;

            // 前方のトータルサイズを算出（※[baseLen0]から[frontEndLen]までの各フリルのサイズ合計を求める）
            baseW = frontRange/numFront;
            float totalW = baseW;               // 起点
            for( int i=1; i<numFront; i++ ){    // 終端が[frontEndLen]に表示されるように[i=0]はスキップ
                rate = i/((float)numFront);
                size = 1.0f + (1.0f-rate)*pBAP->touchFrontSizeOfs;
                if( size < 0.0f ){ totalW -= baseW * size; }
                else{ totalW += baseW * size; }
            }
            
            // 反転フラグ設定
            isHookReverse = pBAP->isFrillReverseHook;
            
            // 起点から開始
            float curW = totalW;
            for( int i=numFront; i>=pBAP->touchFrontSkip; i-- ){
                // 参照先の算出（※サイズの利用状況で判定）
                int curLen;
                if( i == numFront ){
                    curLen = baseLen0;
                }else{
                    curLen = frontEndLen + curW*frontRange/totalW;
                }
                at = SearchTouchIndexForLen0( idFixed, curLen, at, false, pBAP->touchRefOfsRateForMain );

                // サイズの算出＆消費
                rate = i/((float)numFront);
                size = 1.0f + (1.0f-rate)*pBAP->touchFrontSizeOfs;
                if( size < 0.0f ){ curW += baseW * size; }
                else{ curW -= baseW * size; }

                // 角度の算出
                rot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchFrontRotOfs, rate, isFlipH, isFlipV );

                // 幅の算出
                if( i == numFront ){
                    // １つ先のサイズを参照する
                    len = curLen - ( frontEndLen + curW*frontRange/totalW);
                    float tempRate = (i-1)/((float)numFront);
                    len /= 1.0f + (1.0f-tempRate)*pBAP->touchFrontSizeOfs;  // サイズオフセットの調整
                }else{
                    len = lastLen - curLen;
                }
                if( len < 0 ){
                    len *= -1;
                }

                // スキップ判定
                bool isSkip = false;
                if( isFrontHeadSkip ){
                    isSkip = true;
                    isFrontHeadSkip = false;
                }else if( isFrontTailSkip && i == 0 ){
                    isSkip = true;
                    isFrontTailSkip = false;
                }
                
                if( !isSkip ){
                    hook0At = (isHookReverse)? 2: 3;
                    hook1At = (isHookReverse)? 3: 2;
                    float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-rate)*pBAP->touchFrontSizeOfs);
                    DrawFrillAt( pBAP, pMainLD, arrX0[at], arrY0[at], tempScale, len, rot,
                                 false, &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                }
                lastLen = curLen;

                // 起点が描かれていればサブの確認
                if( isPivotDone ){
                    if( pSubLD != NULL ){
                        int subAt = (at + at0)/2;
                        float subRate = (rate+rate0)/2.0f;
                        float subLen = (len+len0)/2.0f;
                        float subRot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchFrontRotOfs, subRate, isFlipH, isFlipV );
                        hook0At = (isHookReverse)? 1: 2;
                        hook1At = (isHookReverse)? 2: 1;
                        float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-subRate)*pBAP->touchFrontSizeOfs);
                        DrawFrillAt( pBAP, pSubLD, arrX0[subAt], arrY0[subAt], tempScale, subLen, subRot,
                                     true, &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                        
                        // 前詰め
                        hookX[1] = hookX[3];
                        hookY[1] = hookY[3];
                    }
                }
                // 起点が処理されていなければ情報の保持
                else{
                    isPivotDone = true;
                    pivotLen0 = len;
                    
                    // 前詰め
                    hookX[0] = hookX[2];
                    hookY[0] = hookY[2];
                    hookX[1] = hookX[3];
                    hookY[1] = hookY[3];
                }

                // 履歴の保持
                at0 = at;
                rate0 = rate;
                len0 = len;
            }

            // スキップの指定があれば（※つながったストローク上のフリルの最後の要素を無視している想定）
            if( pSubLD != NULL && pBAP->touchFrontSkip > 0 ){
                int subAt = SearchTouchIndexForLen0( idFixed, frontEndLen, at0, false, pBAP->touchRefOfsRateForMain );
                if( subAt >= 0 && subAt < at0 ){
                    subAt = (at0 + subAt)/2;
                    float subRate = (0.0f+rate0)/2.0f;      // 末端なので[rate]は[0.0f]
                    float subLen = (pivotLen0+len0)/2.0f;   // 末端なので[len]は[pivotLen0]（※初出のフリル）
                    float subRot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchFrontRotOfs, subRate, isFlipH, isFlipV );
                    hook0At = (isHookReverse)? 1: 0;
                    hook1At = (isHookReverse)? 0: 1;
                    float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-subRate)*pBAP->touchFrontSizeOfs);
                    DrawFrillAt( pBAP, pSubLD, arrX0[subAt], arrY0[subAt], tempScale, subLen, subRot,
                                 true, &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                }
            }
            
            // 後方の処理用にピボットの位置を更新（※後方の最後の要素用）
            isPivotDone = false;
            isPivotFixed = true;
            pivotLen0 = len0;
            pivotHookX0 = hookX[0];
            pivotHookY0 = hookY[0];
            pivotHookX1 = hookX[1];
            pivotHookY1 = hookY[1];
        }else{
            LOGE( "@ CBezier::DrawFrill: INVALID RANGE FOR FRONT: idFixed=%d, num=%d\n", idFixed, numFront );
        }
    }
    
    //---------------------------------
    // 後方の確認（※指定の要素数分表示する）
    //---------------------------------
    if( numBack > 0 ){
        float backEndLen = baseLen0 + (totalLen0-baseLen0)*pBAP->touchBackArea;
        if( backEndLen < baseLen0 ){
            backEndLen = baseLen0;
        }else if( backEndLen > totalLen0 ){
            backEndLen = totalLen0;
        }

        // 領域が有効であれば
        float backRange = backEndLen - baseLen0;
        if( backRange > 0 ){
            int at = pivotAt;
            float rate, size, len, rot;
            float lastLen = baseLen0;

            // 後方のトータルサイズを算出（※[baseLen0]から[backEndLen]までの各フリルのサイズ合計を求める）
            baseW = backRange/numBack;
            float totalW = baseW;               // 起点
            for( int i=1; i<numBack; i++ ){    // 終端が[backEndLen]に表示されるように[i=0]はスキップ
                rate = i/((float)numBack);
                size = 1.0f + (1.0f-rate)*pBAP->touchBackSizeOfs;
                if( size < 0.0f ){ totalW -= size * baseW; }
                else{ totalW += size * baseW; }
            }

            // 反転フラグ設定
            isHookReverse = pBAP->isFrillReverseHook;

            // 起点から開始
            float curW = totalW;
            for( int i=numBack; i>=pBAP->touchBackSkip; i-- ){
                // 参照先の算出（※サイズの利用状況で判定）
                int curLen;
                if( i == numBack ){
                    curLen = baseLen0;
                }else{
                    curLen = backEndLen - curW*backRange/totalW;
                }
                at = SearchTouchIndexForLen0( idFixed, curLen, at, true, pBAP->touchRefOfsRateForMain );

                // サイズの算出＆消費
                rate = i/((float)numBack);
                size = 1.0f + (1.0f-rate)*pBAP->touchBackSizeOfs;
                if( size < 0.0f ){ curW += baseW * size; }
                else{ curW -= baseW * size; }

                // 角度の算出
                rot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchBackRotOfs, rate, isFlipH, isFlipV );

                // 幅の算出
                if( i == numBack ){
                    // １つ先のサイズを参照する
                    len = curLen - ( backEndLen - curW*backRange/totalW);
                    float tempRate = (i-1)/((float)numBack);
                    len /= 1.0f + (1.0f-tempRate)*pBAP->touchBackSizeOfs;  // サイズオフセットの調整
                }else{
                    len = lastLen - curLen;
                }
                if( len < 0 ){
                    len *= -1;
                }

                // メインの表示（※ピボット調整フラグが立っていた場合はオフ＝すでに前方処理で表示されている）
                if( isPivotDone || !isPivotFixed ){
                    // スキップ判定
                    bool isSkip = false;
                    if( isBackHeadSkip ){
                        isSkip = true;
                        isBackHeadSkip = false;
                    }else if( isBackTailSkip && i == 0 ){
                        isSkip = true;
                        isBackTailSkip = false;
                    }

                    if( !isSkip ){
                        hook0At = (isHookReverse)? 3: 2;
                        hook1At = (isHookReverse)? 2: 3;
                        float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-rate)*pBAP->touchBackSizeOfs);
                        DrawFrillAt( pBAP, pMainLD, arrX0[at], arrY0[at], tempScale, len, rot,
                                     false,  &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                    }
                }
                lastLen = curLen;
                
                // 起点が描かれていればサブの確認
                if( isPivotDone ){
                    if( pSubLD != NULL ){
                        int subAt = (at + at0)/2;
                        float subRate = (rate+rate0)/2.0f;
                        float subLen = (len+len0)/2.0f;
                        float subRot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchBackRotOfs, subRate, isFlipH, isFlipV );
                        hook0At = (isHookReverse)? 2: 1;
                        hook1At = (isHookReverse)? 1: 2;
                        float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-subRate)*pBAP->touchBackSizeOfs);
                        DrawFrillAt( pBAP, pSubLD, arrX0[subAt], arrY0[subAt], tempScale, subLen, subRot,
                                     true, &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                        
                        // 前詰め
                        hookX[1] = hookX[3];
                        hookY[1] = hookY[3];
                    }
                }
                // 起点が処理されていなければ情報の保持
                else{
                    isPivotDone = true;
                    if( !isPivotFixed ){
                        pivotLen0 = len;
                        
                        // 前詰め
                        hookX[0] = hookX[2];
                        hookY[0] = hookY[2];
                        hookX[1] = hookX[3];
                        hookY[1] = hookY[3];
                    }else{
                        // 調整
                        hookX[1] = pivotHookX0;
                        hookY[1] = pivotHookY0;
                        hookX[0] = pivotHookX1;
                        hookY[0] = pivotHookY1;
                    }
                }

                // 履歴の保持
                at0 = at;
                rate0 = rate;
                len0 = len;
            }
            
            // スキップの指定があれば（※つながったストローク上のフリルの最後の要素を無視している想定）
            if( pSubLD != NULL && pBAP->touchBackSkip > 0 ){
                int subAt = SearchTouchIndexForLen0( idFixed, backEndLen, at0, true, pBAP->touchRefOfsRateForMain );
                if( subAt >= 0 && subAt > at0 ){
                    subAt = (at0 + subAt)/2;
                    float subRate = (0.0f+rate0)/2.0f;      // 末端なので[rate]は[0.0f]
                    float subLen = (pivotLen0+len0)/2.0f;    // 末端なので[len]は[pivotLen0]（※初出のフリル）
                    float subRot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchBackRotOfs, subRate, isFlipH, isFlipV );
                    hook0At = (isHookReverse)? 0: 1;
                    hook1At = (isHookReverse)? 1: 0;
                    float tempScale = layerSizeAbs*(pBAP->touchBaseSize+(1.0f-subRate)*pBAP->touchBackSizeOfs);
                    DrawFrillAt( pBAP, pSubLD, arrX0[subAt], arrY0[subAt], tempScale, subLen, subRot,
                                 true, &hookX[hook0At], &hookY[hook0At], &hookX[hook1At], &hookY[hook1At] );
                }
            }
        }else{
            LOGE( "@ CBezier::DrawFrill: INVALID RANGE FOR BACK: idFixed=%d, num=%d\n", idFixed, numBack );
        }
    }
}

//------------------------------------------------------------------
// [Frill] １フリルの表示
//------------------------------------------------------------------
// フリルのデータはラインを引いて塗りつぶすだけのシンプルなデータを想定
// [pLD]は素材中央を原点とし、[50%]サイズを基本に作成されたデータとする
// [size/rot]は画面サイズに対して調整された値とする
// [pX0,pY0]と[pX1,pY1]は呼び出しがわでフック座標の管理をする際に利用される
//------------------------------------------------------------------
void CBezier::DrawFrillAt( stBEZIER_ANCHOR_POINT* pBAP, CLayerData* pLD, float x, float y, float size, float baseLen, float rot,
                           bool isSub, float* pX0, float* pY0, float* pX1, float* pY1 ){
    // 横幅調整（※基本サイズ時にフリル同士が重ならず／離れずに並ぶように）
    float baseW = size * s_pBaseParam->texW0 / 2.0f;
    float adjW = baseLen/baseW;

    // データによる調整
    if( isSub ){
        adjW *= pBAP->frillWidthRateForSub;

#if 1
        // ランダムオフセット[SUB]
        if( pBAP->touchRandomOfsFor1 > 0.0f ){
            float temp = pBAP->touchRandomOfsFor1 * CRand::GetRandAtF( eRAND_ID_BEZIER_FRILL );
            if( CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 2 ) == 0 ){ temp *= -1; }
            x += temp * s_pBaseParam->texW0;

            temp = pBAP->touchRandomOfsFor1 * CRand::GetRandAtF( eRAND_ID_BEZIER_FRILL );
            if( CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 2 ) == 0 ){ temp *= -1; }
            y += temp * s_pBaseParam->texH0;
        }
#endif
    }else{
        adjW *= pBAP->frillWidthRateForMain;
        
#if 1
        // ランダムオフセット[MAIN]
        if( pBAP->touchRandomOfsFor0 > 0.0f ){
            float temp = pBAP->touchRandomOfsFor0 * CRand::GetRandAtF( eRAND_ID_BEZIER_FRILL );
            if( CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 2 ) == 0 ){ temp *= -1; }
            x += temp * s_pBaseParam->texW0;

            temp = pBAP->touchRandomOfsFor0 * CRand::GetRandAtF( eRAND_ID_BEZIER_FRILL );
            if( CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 2 ) == 0 ){ temp *= -1; }
            y += temp * s_pBaseParam->texH0;
        }
#endif
    }
    
    //-----------------------------------------------------------
    // フリル表示用レイヤーパラメータの準備（※呼び出し下の値をコピーして調整）
    //-----------------------------------------------------------
    stBEZIER_LAYER_PARAM layerParam = *s_pLayerParam;
    layerParam.pixelOfsX = x;
    layerParam.pixelOfsY = y;

    layerParam.pixelScaleX = size * adjW;
    layerParam.pixelScaleY = size;
    if( s_pLayerParam->pixelScaleX < 0 ){ layerParam.pixelScaleX *= -1; }
    if( s_pLayerParam->pixelScaleY < 0 ){ layerParam.pixelScaleY *= -1; }

    layerParam.pixelRot += rot;
    
    // サブであればフックの予約
    if( isSub ){
        s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] = true;
        s_fArrHookPointX[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] = *pX0;
        s_fArrHookPointY[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] = *pY0;

        s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] = true;
        s_fArrHookPointX[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] = *pX1;
        s_fArrHookPointY[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] = *pY1;
    }
    // サブでなければ無効化
    else{
        s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] = false;
        s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] = false;
    }

    // テンポラリクリア
    s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_TEMP_A] = false;
    s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_TEMP_B] = false;

    //-------------------------------------------
    // 描画（※[LayerData::draw]の処理をそのまま展開）
    //-------------------------------------------
    CLayerObject* pObj = pLD->getDataHead();
    while( pObj != NULL ){
        //---------------------------
        // ラインであれば
        //---------------------------
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = pObj->getLineObject();
            
#if 1
            // 遅延指定があればランダム設定（※乱数のシードはフリル処理の頭で初期化されているので同じ構成のデータであれば同じ乱数となる）
            CAnchorPointData* pAPD = pLOD->getDataHead();
            while( pAPD != NULL ){   // とりあえず上の２点を無理やり動かしてみる
                CAdjustablePoint* pAP = pAPD->getAdjXY();
                if( IS_DELAY_LOG_VALID( pAP->getDelayType() ) ){
                    int depth = CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 10 );
                    pAP->setDelayDepth( depth );
                    
                    int tempX = CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 1500 );
                    int tempY = CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 1500 );
                    if( (tempX%2) != 0 ){ tempX *= -1; }
                    if( (tempY%2) != 0 ){ tempY *= -1; }
                    pAP->setDelayPowerRate( tempX, tempY );
                }

                // 次の要素へ
                pAPD = pAPD->getNext();
            }
#endif
            
            // アンカーポイント変換
            stBEZIER_ANCHOR_POINT* pHead0 = CBezierStructConv::CreateBezierAnchorPoint( pLOD, s_pBaseParam, &layerParam );
            stBEZIER_ANCHOR_POINT* pHead = pHead0;
            
            // アンカーポイントの調整
            while( pHead != NULL ){
                // 元のストローク設定が有効であればコピー
                if( pHead->type != eSTROKE_TYPE_OFF ){
                    pHead->type = pBAP->type;
                    pHead->size = pBAP->size;
                    pHead->ofsRange = pBAP->ofsRange;
                    pHead->dotPutMax = pBAP->dotPutMax;
                    pHead->dotPutRateBase = pBAP->dotPutRateBase;
                    pHead->dotPutRateStrong = pBAP->dotPutRateStrong;
                    pHead->dotPutRateWeak = pBAP->dotPutRateWeak;
                    pHead->isDotPutForce = pBAP->isDotPutForce;
                    pHead->isDotPutWhetherMax = pBAP->isDotPutWhetherMax;
                    pHead->isDotPutOnOutCol = pBAP->isDotPutOnOutCol;
                    pHead->isDotErase = pBAP->isDotErase;

                    pHead->shakeScaleForPos = pBAP->shakeScaleForSize;
                    pHead->shakeScaleForSize = pBAP->shakeScaleForSize;

                    pHead->strokeSize = pBAP->strokeSize;
                    if( pHead->strokeStartSize == 10000 && pHead->strokeEndSize == 10000 ){
                        pHead->strokeStartRange = pBAP->strokeStartRange;
                        pHead->strokeStartSize = pBAP->strokeStartSize;
                        pHead->strokeEndRange = pBAP->strokeEndRange;
                        pHead->strokeEndSize = pBAP->strokeEndSize;
                        pHead->strokeEdgeFillSize = pBAP->strokeEdgeFillSize;
                    }

                    // タッチ（閉じていない線）であれば少しずらしてみる
                    if( pHead->pNext == NULL ){
                        int tempX = CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 200 );
                        int tempY = CRand::GetRandAt( eRAND_ID_BEZIER_FRILL, 400 );
                        if( (tempX%2) != 0 ){ tempX *= -1; }
                        if( (tempY%2) != 0 ){ tempY *= -1; }
                        
                        pHead->x += (pHead->x-x)*tempX/1000.0f;
                        pHead->y += (pHead->y-y)*tempY/1000.0f;
                    }
                }

                // 傾き設定のコピー
                pHead->angleSearchKey = pBAP->angleSearchKey;
                
                // 用心（※閉じていたら抜ける）
                if( pHead->pNext == pHead0 ){
                    break;
                }
                
                pHead = pHead->pNext;
            }

            // ワーククリア
            ClearWorkPoint();
            
            // 出力先を切り替えて描画
            SetBufForStroke( s_pBaseParam, s_pLayerParam, s_bWorkPath );
            DrawStroke( pHead0 );
            
            // アンカーポイント解放
            CBezierStructConv::ReleaseBezierAnchorPoint( pHead0 );
        }
        //--------------------
        // 塗りであれば
        //--------------------
        else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = pObj->getPaintObject();
            
            // オブジェクトが有効であれば描画
            if( ! pPOD->checkFlag( ePOD_FLAG_DISABLE ) ){
                stBEZIER_FILL_POINT* pHead = CBezierStructConv::CreateBezierFillPoint( pPOD, s_pBaseParam, &layerParam );

                // 傾き設定のコピー
                pHead->angleSearchKey = pBAP->angleSearchKey;
                
                // 指定が有効ならパレット差し替え
                if( isSub ){
                    if( pBAP->stripeOrFrillSubPalGrp > 0 ){
                        pHead->palGrp = pBAP->stripeOrFrillSubPalGrp;
                    }
                }else{
                    if( pBAP->stripeOrFrillMainPalGrp > 0 ){
                        pHead->palGrp = pBAP->stripeOrFrillMainPalGrp;
                    }
                }

                // 出力先を切り替えて描画
                SetBufForFill( s_pBaseParam, s_pLayerParam, s_bWorkPath );
                DrawFill( pHead );
                
                // 塗りポイント解放
                CBezierStructConv::ReleaseBezierFillPoint( pHead );

                // 後始末（※出力色を残す＝あとで明暗処理がよべるように）
                CFill::AdjustFillGuideForOutCol( s_pBufFillGuide, s_nBufW, s_nBufH );
            }
        }
        
        pObj = pObj->getNext();
    }
    
    // サブでなければフックの取り出し
    if( ! isSub ){
        if( s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] ){
            *pX0 = s_fArrHookPointX[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0];
            *pY0 = s_fArrHookPointY[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0];
            s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_0] = false;
        }

        if( s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] ){
            *pX1 = s_fArrHookPointX[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1];
            *pY1 = s_fArrHookPointY[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1];
            s_bArrHookPoint[eSTROKE_HOOK_TARGET_FRILL_CONNECT_1] = false;
        }
    }
}
