/*+----------------------------------------------------------------+
  |	Title:		BezierForTouch.cpp [共通環境]
  |	Comment:	ベジェ曲線：タッチ処理
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
// ストライプ登録（塗りつぶし位置は新たに描画したタッチの基準点から、前回の基準点へ少し移動したところ）
#define CHECK_STRIPE_THEN_ADD_POINT()                                                                   \
    if( s_bStripe ){                                                                                    \
        if( ! isReadyForStripe ){                                                                       \
            baseCenterX = curCenterX;                                                                   \
            baseCenterY = curCenterY;                                                                   \
            isReadyForStripe = true;                                                                    \
        }else{                                                                                          \
            float x = 0.25f*lastCenterX + 0.75f*curCenterX;                                             \
            float y = 0.25f*lastCenterY + 0.75f*curCenterY;                                             \
            BYTE palOfsGrp = (isSubNext)? pBAP->stripeOrFrillSubPalGrp: pBAP->stripeOrFrillMainPalGrp;  \
            AddStripePoint( palOfsGrp, x, y );                                                          \
            isSubNext = !isSubNext;                                                                     \
        }                                                                                               \
        lastCenterX = curCenterX;                                                                       \
        lastCenterY = curCenterY;                                                                       \
    }

// ストライプの塗り位置の確認用（※ストライプの塗り漏れがでたらこれで確かめる）
//#define CHECK_POINT_FOR_STRIPE_FILL

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
//----------------------------------------------------------
// [Touch] 描画実体：タッチ
//----------------------------------------------------------
// [pHead]は２点で１組のデータ構成とする
//（※複雑なストローク利用したい場合は複数のタッチに分割して対応する想定）
//----------------------------------------------------------
void CBezier::DrawTouch( stBEZIER_ANCHOR_POINT* pHead ){
#ifndef CHECK_POINT_FOR_STRIPE_FILL
    // ワークパス表示時は無視（※ゴチャゴチャになるので対応しない）
    if( s_bWorkPath ){
        return;
    }
#endif

    // 乱数設定（※処理中のデータに対して一律になってくれさえすれば良いのでアドレスをシードに突っ込んでみる）
    uintptr_t seed = (uintptr_t) pHead;
    CRand::SetSeedAt( eRAND_ID_BEZIER_TOUCH, (uint32)seed, 0 );
    
    stBEZIER_ANCHOR_POINT* pHeadTmp = pHead;

    //-------------------------
    // データ数を調べる
    //-------------------------
    int numData = 0;
    while( pHeadTmp != NULL ){
        numData++;
        
        // 次の要素へ
        pHeadTmp = pHeadTmp->pNext;
        
        // 用心(※クローズパスであれば抜けておく／タッチストローク上では無視）
        if( pHeadTmp == pHead ){
            break;
        }
    }
    
    // 要素数が２でなければ無視（※ストロークの構成が不正）
    if( numData != 2 ){
        LOGE( "@ CBezier::DrawTouch: INVALID DATA NUM: numData=%d\n", numData );
        return;
    }

    // 情報の参照先はペアの先頭
    stBEZIER_ANCHOR_POINT* pBAP = &pHead[0];

    // 対象の確認（※無効であれば無視）
    int idFixed = pBAP->touchTargetIdFixed;
    if( ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( idFixed ) ){
        LOGE( "@ CBezier::DrawTouch: INVALID TARGET: idFixed=%d\n", idFixed );
        return;
    }

    // 登録情報の取得
    int numPoint = s_nArrTouchPointNum[idFixed];
    if( numPoint <= 0 ){
        LOGE( "@ CBezier::DrawTouch: NOT TOUCH POINT: idFixed=%d\n", idFixed );
        return;
    }
    
    //-----------------------------
    // work
    //-----------------------------
    // タッチ登録点の距離の算出
    float totalLen = CalcTouchWorkForLen0( idFixed );

    // 対象座標（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
    float* arrX0 = s_fArrArrTouchPointX0[idFixed];
    float* arrY0 = s_fArrArrTouchPointY0[idFixed];

    // サブ
    int subIdFixed = pBAP->touchTargetSubIdFixed;
    bool isTargetSubValid = false;
    float subTotalLen = 0.0f;
    float* arrSubX0 = NULL;
    float* arrSubY0 = NULL;
    if( IS_STROKE_TOUCH_TARGET_FIXED_VALID( subIdFixed ) && s_nArrTouchPointNum[subIdFixed] > 0 ){
        isTargetSubValid = true;
        subTotalLen = CalcTouchWorkForLen0( subIdFixed, true );
        arrSubX0 = s_fArrArrTouchPointX0[subIdFixed];
        arrSubY0 = s_fArrArrTouchPointY0[subIdFixed];
    }
    
    // 起点（前方／後方領域の分割点）となる座標の特定
    float baseLen = totalLen * pBAP->touchBasePos;
    if( baseLen < 0.0f ){
        baseLen = 0.0f;
    }else if( baseLen > totalLen ){
        baseLen = totalLen;
    }
    
    int pivotAt = SearchTouchIndexForLen0( idFixed, baseLen, 0, true, pBAP->touchRefOfsRateForMain );
    if( pivotAt < 0 ){
        LOGE( "@ CBezier::DrawTouch: SEARCH MISS: baseLen=%f, idFixed=%d\n", baseLen, idFixed );
        return;
    }

    bool isFlipH = (s_pLayerParam->pixelScaleX < 0);
    bool isFlipV = (s_pLayerParam->pixelScaleY < 0);
    
    bool isPivotDone = false;

    int numFront = pBAP->touchFrontNum;
    int numBack = pBAP->touchBackNum;
    bool isFrontHeadSkip = false;
    bool isFrontTailSkip = false;
    bool isBackHeadSkip = false;
    bool isBackTailSkip = false;

    // 反転指定があれば調整（※ミラーコピーによる反転を想定するで横方向のみ）
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

    //--------------------------
    // work（※ストライプ時専用）
    //--------------------------
    bool isReadyForStripe = false;
    bool isSubNext = false;
    float curCenterX = 0.0f;
    float curCenterY = 0.0f;
    float lastCenterX = 0.0f;
    float lastCenterY = 0.0f;
    float baseCenterX = 0.0f;
    float baseCenterY = 0.0f;

    // 指定があればストライプワークのクリア
    if( s_bStripe ){
        ClearWorkStripePoint();
    }
    
    //---------------------------
    // 前方の確認
    //---------------------------
    if( numFront > 0 ){
        // 末端となる座標の特定
        float frontEndLen = baseLen * (1.0f-pBAP->touchFrontArea);
        if( frontEndLen < 0.0f ){
            frontEndLen = 0.0f;
        }else if( frontEndLen > baseLen ){
            frontEndLen = baseLen;
        }
        
        int frontEndAt = SearchTouchIndexForLen0( idFixed, frontEndLen, pivotAt, false, pBAP->touchRefOfsRateForMain );
        if( frontEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: frontEndLen=%f, idFixed=%d\n", frontEndLen, idFixed );
            return;
        }
        if( frontEndAt > pivotAt ){
            frontEndAt = pivotAt;
        }

        isSubNext = false;   // ストライプはメインから開始する
        lastCenterX = baseCenterX;
        lastCenterY = baseCenterY;

        //--------------------------------
        // 前方の領域が有効であれば描画
        //--------------------------------
        float frontRange = baseLen - frontEndLen;
        if( frontRange > 0.0f ){
            // ステップ幅：前方
            float step = frontRange / numFront;
            float stepBorder = step * pBAP->touchFrontBorderRate;
            step -= stepBorder;
            
            // ボーダーが有効であれば調整（※ボーダーが差し込めるように若干狭くする）
            if( stepBorder > 0 ){
                float adj = stepBorder / (frontRange+stepBorder);
                step *= 1.0 - adj;
                stepBorder *= 1.0 - adj;
            }

            int at = pivotAt;
            float curLen = baseLen;
            
            // 指定数タッチを描画する
            int numTouch = 2*(numFront+1);
            int numSkip = 2*pBAP->touchFrontSkip;
            while( numTouch > numSkip ){
                // 末尾スキップ指定があれば抜ける（※回数指定が有効なら無視）
                if( isFrontTailSkip && numSkip <= 0 ){
                    if( stepBorder > 0 ){
                        if( (numTouch-1) <= 0 ){
                            break;
                        }
                    }else{
                        if( (numTouch-2) <= 0 ){
                            break;
                        }
                    }
                }
                
                // 対象ポイント取得
                at = SearchTouchIndexForLen0( idFixed, curLen, at, false, pBAP->touchRefOfsRateForMain );
                
                // 用心（※いきすぎたら戻す）
                if( at < 0 ){
                    at = frontEndAt;
                    curLen = frontEndLen;
                }
                
                // 先端スキップ指定があれば飛ばす
                if( isFrontHeadSkip ){
                    isFrontHeadSkip = false;
                }else{
                    // タッチ描画
                    float rate = (curLen-frontEndLen)/frontRange;
                    float size = pBAP->touchBaseSize + (1.0f-rate)*pBAP->touchFrontSizeOfs;
                    float rot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchFrontRotOfs, rate, isFlipH, isFlipV );
                    if( isTargetSubValid ){
                        // 対応するサブの座標を検索
                        float subLen = (curLen/totalLen) * subTotalLen;
                        int subAt = SearchTouchIndexForSub( subIdFixed, subLen, false, pBAP->touchRefOfsRateForSub );
                        float subX = arrSubX0[subAt];
                        float subY = arrSubY0[subAt];
                        
                        DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY, true, subX, subY );
                    }else{
                        DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                    }

                    // ストライプ登録
                    CHECK_STRIPE_THEN_ADD_POINT();
                }

                // ステップ更新（※[stepBorder]が無効ならスキップ）
                if( (numTouch%2) == 0 ){
                    if( stepBorder <= 0 ){
                        numTouch--;
                    }
                }
                numTouch--;

                // ステップ消費
                if( (numTouch%2) == 0 ){
                    curLen -= step;
                }else{
                    curLen -= stepBorder;
                }

                isPivotDone = true;
            }
        }else{
            LOGE( "@ CBezier::DrawTouch: INVALID RANGE FOR FRONT: idFixed=%d, num=%d\n", idFixed, numFront );
        }
    }

    //---------------------------
    // 後方の確認
    //---------------------------
    if( numBack > 0 ){
        // 末端となる座標の特定
        float backEndLen = baseLen + (totalLen-baseLen)*pBAP->touchBackArea;
        if( backEndLen < baseLen ){
            backEndLen = baseLen;
        }else if( backEndLen > totalLen ){
            backEndLen = totalLen;
        }
        
        int backEndAt = SearchTouchIndexForLen0( idFixed, backEndLen, pivotAt, true, pBAP->touchRefOfsRateForMain );
        if( backEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: backEndLen=%f, idFixed=%d\n", backEndLen, idFixed );
            return;
        }
        if( backEndAt < pivotAt ){
            backEndAt = pivotAt;
        }
        
        isSubNext = true;   // ストライプはサブから開始する
        lastCenterX = baseCenterX;
        lastCenterY = baseCenterY;

        //--------------------------------
        // 後方の領域が有効であれば描画
        //--------------------------------
        float backRange = backEndLen - baseLen;
        if( backRange > 0.0f ){
            // ステップ幅：後方
            float step = backRange / numBack;
            float stepBorder = step * pBAP->touchBackBorderRate;
            step -= stepBorder;

            // ボーダーが有効であれば調整（※ボーダーが差し込めるように若干狭くする）
            if( stepBorder > 0 ){
                float adj = stepBorder / (backRange+stepBorder);
                step *= 1.0 - adj;
                stepBorder *= 1.0 - adj;
            }

            int at = pivotAt;
            float curLen = baseLen;
            
            // 指定数タッチを描画する（※起点が表示済みであればスキップ）
            int numTouch = 2*(numBack+1);
            int numSkip = 2*pBAP->touchBackSkip;
            
            // 起点が描画済みなら調整（※線が重ならないように）
            if( isPivotDone ){
                curLen += step + stepBorder;
                numTouch -= 2;
            }
            while( numTouch > numSkip ){
                // 末尾スキップ指定があれば抜ける（※回数指定が有効なら無視）
                if( isBackTailSkip && numSkip <= 0 ){
                    if( stepBorder > 0 ){
                        if( (numTouch-1) <= numSkip ){
                            break;
                        }
                    }else{
                        if( (numTouch-2) <= numSkip ){
                            break;
                        }
                    }
                }

                // 対象ポイント取得
                at = SearchTouchIndexForLen0( idFixed, curLen, at, true, pBAP->touchRefOfsRateForMain );
                
                // 用心（※いきすぎたら戻す）
                if( at < 0 ){
                    at = backEndAt;
                    curLen = backEndLen;
                }
                
                // 先端スキップ指定があれば飛ばす
                if( isBackHeadSkip ){
                    isBackHeadSkip = false;
                }else{
                    // タッチ描画
                    float rate = (backEndLen-curLen)/backRange;
                    float size = pBAP->touchBaseSize + (1.0f-rate)*pBAP->touchBackSizeOfs;
                    float rot = CalcRotForTouch( pBAP->touchBaseRot, pBAP->touchBackRotOfs, rate, isFlipH, isFlipV );
                    if( isTargetSubValid ){
                        // 対応するサブの座標を検索
                        float subLen = (curLen/totalLen) * subTotalLen;
                        int subAt = SearchTouchIndexForSub( subIdFixed, subLen, true, pBAP->touchRefOfsRateForSub );
                        float subX = arrSubX0[subAt];
                        float subY = arrSubY0[subAt];
                        
                        DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY, true, subX, subY );
                    }else{
                        DrawTouchAt( pHead, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                    }

                    // ストライプ登録
                    CHECK_STRIPE_THEN_ADD_POINT();
                }

                // ステップ更新（※[stepBorder]が無効ならスキップ）
                if( (numTouch%2) == 0  ){
                    if( stepBorder <= 0 ){
                        numTouch--;
                    }
                }
                numTouch--;

                // ステップ消費
                if( (numTouch%2) == 0 ){
                    curLen += step;
                }else{
                    curLen += stepBorder;
                }

                isPivotDone = true;
            }
        }else{
            LOGE( "@ CBezier::DrawTouch: INVALID RANGE FOR BACK: idFixed=%d, num=%d\n", idFixed, numBack );
        }
    }

    //---------------------------------------------
    // ストライプであればタッチストローク間の領域を塗りつぶす
    //---------------------------------------------
    if( s_bStripe && s_nStripePointNum > 0){
        // ガイドの復旧
        if( IS_STROKE_TOUCH_TARGET_FIXED_VALID( pBAP->touchTargetIdFixed ) ){
            RecoverGuideWithTouch( pBAP->touchTargetIdFixed );
        }

        if( IS_STROKE_TOUCH_TARGET_FIXED_VALID( pBAP->touchTargetSubIdFixed ) ){
            RecoverGuideWithTouch( pBAP->touchTargetSubIdFixed );
        }

        //　ストローク間の塗りつぶし
        for( int i=0; i<s_nStripePointNum; i++ ){
            BYTE palOfsGrp = s_nArrStripePalGrp[i];
            if( palOfsGrp > 0 ){
                float x = s_fArrStripePointX[i];
                float y = s_fArrStripePointY[i];
                
                // 塗り形式は「単色」（※ストロークの出力色指定を参照する）
                CFill* pFill = CFillMgr::GetFill( eFILL_TYPE_MONO );
                pFill->fill( s_pBaseParam->pBufColor, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, (int)x, (int)y,
                             s_pLayerParam->bitMask, palOfsGrp, BEZIER_PAL_VAL_BASE, s_nTestPalGrp, pBAP->isDotPutOnOutCol );
            
#ifdef CHECK_POINT_FOR_STRIPE_FILL
                // 塗りポイント確認
                PutCrossForWorkPath( x, y, BEZIER_WP_PAINT_PAL+BEZIER_WP_PAL_VAL_FOR_SELECTED, false, false );
#endif
            }
        }
        
        // ガイドのリセットはしない（※必要であれば[eLOD_FLAG_RESET_GUIDE_AFTER_STROKE]を立てておくこと）
        //（※細かい話だが、レイヤーデータの最後の要素でストライプを読んだ場合に２重リセットされるのが嫌なので）
    }
}

//---------------------------------
// [Touch] タッチストロークの描画：単体
//---------------------------------
void CBezier::DrawTouchAt( stBEZIER_ANCHOR_POINT* pHead, float x, float y, float size, float rot, float* pCenterX, float* pCenterY,
                           bool isSubValid, float subX, float subY ){
    //-----------------------------------------------
    // ストロークデータの退避（※処理の流れで値が破壊されるので）
    //-----------------------------------------------
    float x0 = pHead[0].x;
    float y0 = pHead[0].y;
    float x1 = pHead[1].x;
    float y1 = pHead[1].y;
    
    float xIn0 = pHead[0].xIn;
    float yIn0 = pHead[0].yIn;
    float xIn1 = pHead[1].xIn;
    float yIn1 = pHead[1].yIn;
    
    float xOut0 = pHead[0].xOut;
    float yOut0 = pHead[0].yOut;
    float xOut1 = pHead[1].xOut;
    float yOut1 = pHead[1].yOut;
    
    //---------------------
    // ストロークデータの調整
    //---------------------
    // オフセット算出
    float ofsX, ofsY;
    if( rot != 0.0f ){
        ofsX = CMath::RotX( x1-x0, y1-y0, rot );
        ofsY = CMath::RotY( x1-x0, y1-y0, rot );

        pHead[0].xIn = CMath::RotX( xIn0, yIn0, rot );
        pHead[0].yIn = CMath::RotY( xIn0, yIn0, rot );
        pHead[1].xIn = CMath::RotX( xIn1, yIn1, rot );
        pHead[1].yIn = CMath::RotY( xIn1, yIn1, rot );
        
        pHead[0].xOut = CMath::RotX( xOut0, yOut0, rot );
        pHead[0].yOut = CMath::RotY( xOut0, yOut0, rot );
        pHead[1].xOut = CMath::RotX( xOut1, yOut1, rot );
        pHead[1].yOut = CMath::RotY( xOut1, yOut1, rot );
    }else{
        ofsX = x1 - x0;
        ofsY = y1 - y0;
    }
    
    // タッチ位置から線が引かれるように調整（※指定座標へ原点を合わせる＆サイズの適用）
    pHead[0].x = x;
    pHead[0].y = y;
    
    if( isSubValid ){
        pHead[1].x = subX;
        pHead[1].y = subY;

#if 1
        // サイズ調整（※実際のデータからの線の伸び具合を方向線へも加味させてみる）
        float len0 = CMath::Len( x1-x0, y1-y0 );
        if( len0 > 0.0f ){
            float len = CMath::Len( pHead[1].x-pHead[0].x, pHead[1].y-pHead[0].y );
            size *= len/len0;
        }
#endif
    }else{
        pHead[1].x = pHead[0].x + ofsX * size;
        pHead[1].y = pHead[0].y + ofsY * size;
    }
    
    pHead[0].xIn *= size;
    pHead[0].yIn *= size;
    pHead[1].xIn *= size;
    pHead[1].yIn *= size;
    
    pHead[0].xOut *= size;
    pHead[0].yOut *= size;
    pHead[1].xOut *= size;
    pHead[1].yOut *= size;

    //------------------------
    // ストロークのスキップ率を設定
    //------------------------
    // 先頭
    float skipHeadRate = pHead[0].touchHeadSkipRate;
    if( skipHeadRate > 0.0f ){
        skipHeadRate *= CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
    }

    // 末尾
    float skipTailRate = pHead[0].touchTailSkipRate;
    if( skipTailRate > 0.0f ){
        skipTailRate *= CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
    }

#if 1
    //-----------------------
    // ランダムオフセット
    //-----------------------
    // 始点
    if( pHead[0].touchRandomOfsFor0 > 0.0f ){
        float temp = pHead[0].touchRandomOfsFor0 * CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
        if( CRand::GetRandAt( eRAND_ID_BEZIER_TOUCH, 2 ) == 0 ){ temp *= -1; }
        pHead[0].x += temp * s_pBaseParam->texW0;

        temp = pHead[0].touchRandomOfsFor0 * CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
        if( CRand::GetRandAt( eRAND_ID_BEZIER_TOUCH, 2 ) == 0 ){ temp *= -1; }
        pHead[0].y += temp * s_pBaseParam->texH0;
    }
    
    // 終点（※データの指定は始点[0]となる）
    if( pHead[0].touchRandomOfsFor1 > 0.0f ){
        float temp = pHead[0].touchRandomOfsFor1 * CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
        if( CRand::GetRandAt( eRAND_ID_BEZIER_TOUCH, 2 ) == 0 ){ temp *= -1; }
        pHead[1].x += temp * s_pBaseParam->texW0;

        temp = pHead[0].touchRandomOfsFor1 * CRand::GetRandAtF( eRAND_ID_BEZIER_TOUCH );
        if( CRand::GetRandAt( eRAND_ID_BEZIER_TOUCH, 2 ) == 0 ){ temp *= -1; }
        pHead[1].y += temp * s_pBaseParam->texH0;
    }
#endif

    //----------------------------------
    // ストロークの描画
    //----------------------------------
    ClearWorkPoint();
    DrawStroke( pHead, skipHeadRate, skipTailRate );
    
    //------------------------------------------------
    // 実際に出力したガイド座標の値を基準点とし呼び出し元へ返す
    //（※ストライプの塗りつぶし開始点検出用の座標を受け渡す）
    //------------------------------------------------
    if( s_nNumStripeGuidePoint > 0 ){
        int pivot;
        if( isSubValid ){
            // サブが有効なら中央
            pivot = (int)(0.5f*s_nNumStripeGuidePoint);
        }else{
            // サブが無効なら少しいったところ（※サブがない場合 pHead[1] が尻切れになる可能性があるので）
            pivot = (int)(0.2f*s_nNumStripeGuidePoint);
        }
        
        if( pCenterX != NULL ){ *pCenterX = (int)s_fArrStripeGuidePointX[pivot]; }
        if( pCenterY != NULL ){ *pCenterY = (int)s_fArrStripeGuidePointY[pivot]; }
    }

    //----------------------
    // ストロークデータの復旧
    //----------------------
    pHead[0].x = x0;
    pHead[0].y = y0;
    pHead[1].x = x1;
    pHead[1].y = y1;

    pHead[0].xIn = xIn0;
    pHead[0].yIn = yIn0;
    pHead[1].xIn = xIn1;
    pHead[1].yIn = yIn1;
    
    pHead[0].xOut = xOut0;
    pHead[0].yOut = yOut0;
    pHead[1].xOut = xOut1;
    pHead[1].yOut = yOut1;
}

//---------------------------------------------------------------------------
// [Touch] タッチの長さをワークへ設定
//---------------------------------------------------------------------------
// １座標毎の距離はほぼ１なので、「s_fArrTouchWorkForLen0[i] = i;」としたいのだが、
// 微妙に距離がばらつくので、ちゃんと各点間の距離を計算して累積している
//----------------------------------------------------------------------------
float CBezier::CalcTouchWorkForLen0( int idFixed, bool isSub ){
    if( ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( idFixed ) ){
        LOGE( "@ CBezier::CalcTouchWorkForLen0: INVALID TARGET: idFixed=%d, isSub=%d\n", idFixed, isSub );
        return( 0.0f );
    }
    
    // バッファ選択
    float* arrLen = (isSub)? s_fArrSubTouchWorkForLen0: s_fArrTouchWorkForLen0;

    // 開始点の距離は０
    arrLen[0] = 0.0f;

    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[idFixed];
    if( numPoint < 2 ){ return( 0.0f ); }

    // work（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
    float* arrX0 = s_fArrArrTouchPointX0[idFixed];
    float* arrY0 = s_fArrArrTouchPointY0[idFixed];
    
    // 各点までの移動距離を累積で設定
    for( int i=1; i<numPoint; i++ ){
        float divX = arrX0[i] - arrX0[i-1];
        float divY = arrY0[i] - arrY0[i-1];
        arrLen[i] = arrLen[i-1] + CMath::Len( divX, divY );
    }

    // タッチポイント全ての距離を返しておく
    return( arrLen[numPoint-1] );
}

//-----------------------------------------------------------------------------
// [Touch] タッチ点インデックスの検索
//-----------------------------------------------------------------------------
// [CalcTouchWorkForLen0]で計算した値から位置を特定する（※見つからない場合は無効値を返す）
//-----------------------------------------------------------------------------
int CBezier::SearchTouchIndexForLen0( int idFixed, float len, int from, bool isSearchBackward, float ofsRate ){
    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[idFixed];
    if( numPoint < 2 ){ return( -1 ); }
    
    // オフセットによる調整
    len += s_fArrTouchWorkForLen0[numPoint-1] * ofsRate;

    // 後方へ
    if( isSearchBackward ){
        for( int i=from; i<numPoint; i++ ){
            if( s_fArrTouchWorkForLen0[i] >= len ){
                return( i );
            }
        }
    }
    // 前方へ
    else{
        for( int i=from; i>=0; i-- ){
            if( s_fArrTouchWorkForLen0[i] <= len ){
                return( i );
            }
        }
    }

    // ここまできたら検索失敗（※候補がいない）
    return( -1 );
}

//--------------------------------------------------------------------------------------------
// [Touch] サブのタッチ点インデックスの検索
//--------------------------------------------------------------------------------------------
// [SearchTouchIndexForLen0]によってメインの参照先が決まった後に、対応する相対距離から座標を検索する
//（※もともとは[SearchTouchIndexForLen0]の返値[参照インデックス]を登録座標数で補正した座標を検索していたが、
//　　メインで指定した時とサブで指定した時で、タッチの座標がずれてしまったため、まじめに距離で計算するように変更）
//--------------------------------------------------------------------------------------------
int CBezier::SearchTouchIndexForSub( int subIdFixed, float len, bool isSearchBackward, float ofsRate ){
    // 無効は無視
    int numPointSub = s_nArrTouchPointNum[subIdFixed];
    if( numPointSub <= 0 ){ return( -1 ); }

    // オフセットによる調整
    len += s_fArrSubTouchWorkForLen0[numPointSub-1] * ofsRate;
    
    // 後方へ
    if( isSearchBackward ){
        for( int i=0; i<numPointSub; i++ ){
            if( s_fArrSubTouchWorkForLen0[i] >= len ){
                return( i );
            }
        }
        
        // ここまできたら末尾要素を返す（※メインとは違い無効値は返さない）
        return( s_fArrSubTouchWorkForLen0[numPointSub-1] );
    }

    // 前方へ
    for( int i=numPointSub-1; i>=0; i-- ){
        if( s_fArrSubTouchWorkForLen0[i] <= len ){
            return( i );
        }
    }
    
    // ここまで来たら先頭要素を返す（※メインとは違い無効値は返さない）
    return( 0 );
}

//-----------------------------------------------------------------------
// [Touch] タッチ向けの角度の算出（※タッチストロークを描画する際に適用する角度の算出）
//-----------------------------------------------------------------------
float CBezier::CalcRotForTouch( float rotBase, float rotOfs, float rate, bool isFlipH, bool isFlipV ){
    float rot = rotBase;
    rot += rotOfs * (1.0f-rate);
    if( isFlipH != isFlipV ){ rot *= -1; }
    return( rot );
}
