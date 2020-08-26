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
//------------------------------------------------------
// [Touch] 描画実体：タッチ
//------------------------------------------------------
// [pHead]は２点で１組のデータ構成とする
//（※複雑な形状をタッチで利用したい場合は「フリル」を利用すること）
//------------------------------------------------------
void CBezier::DrawTouch( stBEZIER_ANCHOR_POINT* pHead ){
#ifndef CHECK_POINT_FOR_STRIPE_FILL
    // ワークパス表示時は無視（※ゴチャゴチャになるので対応しない）
    if( s_bWorkPath ){
        return;
    }
#endif

    // 乱数設定（※データに対して一律になってくれさえすれば良いのでアドレスをシードに突っ込んでみる）
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
    eSTROKE_TOUCH_TARGET target = pBAP->touchTarget;
    if( ! IS_STROKE_TOUCH_TARGET_VALID( target ) ){
        LOGE( "@ CBezier::DrawTouch: INVALID TARGET: target=%d\n", target );
        return;
    }

    // 登録情報の取得
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint <= 0 ){
        LOGE( "@ CBezier::DrawTouch: NOT TOUCH POINT: target=%d\n", target );
        return;
    }
    
    //-----------------------------
    // work
    //-----------------------------
    // タッチ登録点の距離の算出
    float totalLen = CalcTouchWorkForLen0( target );

    // 対象座標（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
    float* arrX0 = s_fArrArrTouchPointX0[target];
    float* arrY0 = s_fArrArrTouchPointY0[target];

    eSTROKE_TOUCH_TARGET targetSub = pBAP->touchTargetSub;
    bool isTargetSubValid = false;
    float* arrSubX0 = NULL;
    float* arrSubY0 = NULL;
    if( IS_STROKE_TOUCH_TARGET_VALID( targetSub ) && s_nArrTouchPointNum[targetSub] > 0 ){
        isTargetSubValid = true;
        arrSubX0 = s_fArrArrTouchPointX0[targetSub];
        arrSubY0 = s_fArrArrTouchPointY0[targetSub];
    }
    
    // 起点（前方／後方領域の分割点）となる座標の特定
    float baseLen = totalLen * pBAP->touchBasePos;
    if( baseLen < 0.0f ){
        baseLen = 0.0f;
    }else if( baseLen > totalLen ){
        baseLen = totalLen;
    }
    
    int pivotAt = SearchTouchIndexForLen0( target, baseLen, 0, true );
    if( pivotAt < 0 ){
        LOGE( "@ CBezier::DrawTouch: SEARCH MISS: baseLen=%f, target=%d\n", baseLen, target );
        return;
    }

    bool isFlipH = (s_pLayerParam->pixelScaleX < 0);
    bool isFlipV = (s_pLayerParam->pixelScaleY < 0);

    bool isPivotDone = false;

    int numFront = pBAP->touchFrontNum;
    int numBack = pBAP->touchBackNum;

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
        
        int frontEndAt = SearchTouchIndexForLen0( target, frontEndLen, pivotAt, false );
        if( frontEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: frontEndLen=%f, target=%d\n", frontEndLen, target );
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
                // 対象ポイント取得
                at = SearchTouchIndexForLen0( target, curLen, at, false );
                
                // 用心（※いきすぎたら戻す）
                if( at < 0 ){
                    at = frontEndAt;
                    curLen = frontEndLen;
                }

                // タッチ描画
                float rate = (curLen-frontEndLen)/frontRange;
                float size = pBAP->touchBaseSize * (1.0f + (1.0f-rate)*pBAP->touchFrontSizeOfs);
                float rot = CalcRotForTouch( target, at, isFlipH, isFlipV, pBAP->touchFrontRotRate, pBAP->touchBaseRot );
                if( isTargetSubValid ){
                    int subAt = SearchTouchIndexForSub( target, at, targetSub, pBAP->isTouchSubReverse );
                    float subX = arrSubX0[subAt];
                    float subY = arrSubY0[subAt];
                    DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY, true, subX, subY );
                }else{
                    DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                }

                // ストライプ登録
                CHECK_STRIPE_THEN_ADD_POINT();

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
            LOGE( "@ CBezier::DrawTouch: INVALID RANGE FOR FRONT: target=%d, num=%d\n", target, numFront );
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
        
        int backEndAt = SearchTouchIndexForLen0( target, backEndLen, pivotAt, true );
        if( backEndAt < 0 ){
            LOGE( "@ CBezier::DrawTouch: SEARCH MISS: backEndLen=%f, target=%d\n", backEndLen, target );
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
                // 対象ポイント取得
                at = SearchTouchIndexForLen0( target, curLen, at, true );
                
                // 用心（※いきすぎたら戻す）
                if( at < 0 ){
                    at = backEndAt;
                    curLen = backEndLen;
                }
                
                // タッチ描画
                float rate = (backEndLen-curLen)/backRange;
                float size = pBAP->touchBaseSize * (1.0f + (1.0f-rate)*pBAP->touchBackSizeOfs);
                float rot = CalcRotForTouch( target, at, isFlipH, isFlipV, pBAP->touchBackRotRate, pBAP->touchBaseRot );
                if( isTargetSubValid ){
                    int subAt = SearchTouchIndexForSub( target, at, targetSub, pBAP->isTouchSubReverse );
                    float subX = arrSubX0[subAt];
                    float subY = arrSubY0[subAt];
                    DrawTouchAt( pBAP, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY, true, subX, subY );
                }else{
                    DrawTouchAt( pHead, arrX0[at], arrY0[at], size, rot, &curCenterX, &curCenterY );
                }

                // ストライプ登録
                CHECK_STRIPE_THEN_ADD_POINT();

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
            LOGE( "@ CBezier::DrawTouch: INVALID RANGE FOR BACK: target=%d, num=%d\n", target, numBack );
        }
    }

    //---------------------------------------------
    // ストライプであればタッチストローク間の領域を塗りつぶす
    //---------------------------------------------
    if( s_bStripe && s_nStripePointNum > 0){
        // ガイドの復旧
        if( IS_STROKE_TOUCH_TARGET_VALID( pBAP->touchTarget ) ){
            RecoverGuideWithTouch( pBAP->touchTarget );
        }

        if( IS_STROKE_TOUCH_TARGET_VALID( pBAP->touchTargetSub ) ){
            RecoverGuideWithTouch( pBAP->touchTargetSub );
        }

        //　ストローク間の塗りつぶし
        for( int i=0; i<s_nStripePointNum; i++ ){
            BYTE palOfsGrp = s_nArrStripePalGrp[i];
            if( palOfsGrp > 0 ){
                float x = s_fArrStripePointX[i];
                float y = s_fArrStripePointY[i];
                
                // 塗り形式は「単色」（※ストロークの出力色指定を参照する）
                CFill* pFill = CFillMgr::GetFill( eFILL_TYPE_MONO );
                pFill->fill( s_pBaseParam->pBufColor, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                             (int)x, (int)y, palOfsGrp, BEZIER_PAL_VAL_BASE, s_nTestPalGrp, pBAP->isDotPutOnOutCol );
            
#ifdef CHECK_POINT_FOR_STRIPE_FILL
                // 塗りポイント確認
                PutCrossForWorkPath( x, y, BEZIER_WP_PAINT_PAL+BEZIER_WP_PAL_VAL_FOR_SELECTED );
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
float CBezier::CalcTouchWorkForLen0( eSTROKE_TOUCH_TARGET target ){
    if( target < 0 || target >= eSTROKE_TOUCH_TARGET_MAX ){
        LOGE( "@ CBezier::CalcTouchWorkForLen0: INVALID TARGET: target=%d\n", target );
        return( 0.0f );
    }

    // 開始点の距離は０
    s_fArrTouchWorkForLen0[0] = 0.0f;

    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint < 2 ){ return( 0.0f ); }

    // work（※ストローク処理で傾き処理が行われるためここで扱う座標は傾き前の値）
    float* arrX0 = s_fArrArrTouchPointX0[target];
    float* arrY0 = s_fArrArrTouchPointY0[target];
    
    // 各点までの移動距離を累積で設定
    for( int i=1; i<numPoint; i++ ){
        float divX = arrX0[i] - arrX0[i-1];
        float divY = arrY0[i] - arrY0[i-1];
        s_fArrTouchWorkForLen0[i] = s_fArrTouchWorkForLen0[i-1] + CMath::Len( divX, divY );
    }

    // タッチポイント全ての距離を返しておく
    return( s_fArrTouchWorkForLen0[numPoint-1] );
}

//-------------------------------------------------
// [Touch] タッチ点インデックスの検索
//-------------------------------------------------
// [CalcTouchWorkForLen0]で計算した値から位置を特定する
//-------------------------------------------------
int CBezier::SearchTouchIndexForLen0( eSTROKE_TOUCH_TARGET target, float len, int from, bool isBack ){
    // ２点未満であれば返す
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint < 2 ){ return( -1 ); }

    // 後方へ
    if( isBack ){
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

    // ここまできたら終端
    return( -1 );
}

//-------------------------------------------------
// [Touch] サブのタッチ点インデックスの検索
//-------------------------------------------------
// 渡された対象のタッチ点の対になるサブタッチ点の検索
//-------------------------------------------------
int CBezier::SearchTouchIndexForSub( eSTROKE_TOUCH_TARGET target, int at, eSTROKE_TOUCH_TARGET targetSub, bool isReverse ){
    // 無効は無視
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint <= 0 ){ return( -1 ); }
    
    int numPointSub = s_nArrTouchPointNum[targetSub];
    if( numPointSub <= 0 ){ return( -1 ); }

    // 割合の算出
    float rate = ((float)at) / numPoint;
    if( isReverse ){ rate = 1.0f - rate; }
    
    int subAt = (int)(rate*numPointSub);
    if( subAt < 0 ) { subAt = 0; }
    else if( subAt >= numPointSub ) { subAt = numPointSub - 1; }
    return( subAt );
}

//-----------------------------------------------------------------------
// [Touch] タッチ向けの角度の算出（※タッチストロークを描画する際に適用する角度の算出）
//-----------------------------------------------------------------------
float CBezier::CalcRotForTouch( eSTROKE_TOUCH_TARGET target, int at, bool isFlipH, bool isFlipV, float rate, float baseRot ){
    float rot;
    float vecX = 0.0f;
    float vecY = 0.0f;
    
    // ストロークによる回転（※すでに出力済みのタッチ座標に対する角度の適用）
    if( rate != 0.0f ){
        // タッチ座標はすでに諸々の影響を受けている（※レイヤーの回転や反転等）
        rot = CalcTouchDirForLen0( target, at, isFlipH, isFlipV );
        if( isFlipH != isFlipV ){ rot *= -1; }
        
        // レイヤー角度を相殺しておく（※描画座標に対して角度がついて欲しいので以降の回転を無視したい）
        rot -= s_pLayerParam->pixelRot;

        // 回転ベクトルの算出
        float tempX = CMath::RotX( rot );
        float tempY = CMath::RotY( rot );

        // 補正値が負なら反転
        if( rate < 0.0f ){
            rate *= -1;
            tempX *= -1;
            tempY *= -1;
        }
    
        // 割合で反映
        vecX += rate * tempX;
        vecY += rate * tempY;
    }

    // 基本角度を反映（※アンカーポイントに指定されたタッチストロークの固定角度の適用）
    if( rate < 1.0f ){
        rot = baseRot;
        if( isFlipH != isFlipV ){ rot *= -1; }
        
        // 基本ベクトルの算出
        float tempX = CMath::RotX( rot );
        float tempY = CMath::RotY( rot );

        // 割合で反映
        vecX += (1-rate) * tempX;
        vecY += (1-rate) * tempY;
    }
    
    // 方向の算出
    rot = CMath::GetDeg( -vecX, -vecY );
    return( rot );
}

//--------------------------------------------------------------------------
// [Touch] タッチ点に対する角度の算出（※指定のタッチ登録点の前後の座標に対する法線の算出）
//--------------------------------------------------------------------------
float CBezier::CalcTouchDirForLen0( eSTROKE_TOUCH_TARGET target, int at, bool isFlipH, bool isFlipV ){
    // 無効は無視（※２点未満であれば算出できない）
    int numPoint = s_nArrTouchPointNum[target];
    if( numPoint < 2 ){ return( 0.0 ); }
    
    // 無効は無視
    if( at < 0 || at >= numPoint ){
        return( 0.0f );
    }
    
    // 基本点の前後の座標の取得
    int prevAt = at - 8;
    int nextAt = at + 8;
    
    if( prevAt < 0 ){
        prevAt = 0;
    }
    
    if( nextAt >= numPoint ){
        nextAt = numPoint -1 ;
    }
    
    // タッチ方向の算出
    float rot = CMath::GetDeg( s_fArrArrTouchPointX0[target][nextAt], s_fArrArrTouchPointY0[target][nextAt],
                               s_fArrArrTouchPointX0[target][prevAt], s_fArrArrTouchPointY0[target][prevAt] );

    // 描画時の反転影響を相殺（※実描画座標なので角度反転）
    // ここで受け取る[isFlipH/V]はタッチストロークに対する指定となり、タッチ点を登録したレイヤーのものとはことなる点に注意
    // 理屈上では、タッチ点登録レイヤーの反転指定とタッチストロークの指定がずれることはありえるが、運用上は同じになるという想定
    //（※例えば、腕の表示にタッチをつける場合、左右の腕から呼ばれるタッチストロークは、それぞれ左右の腕に属しているはずなので問題は発生しない）
    if( isFlipH ){ rot = CMath::FlipRotH( rot ); }
    if( isFlipV ){ rot = CMath::FlipRotV( rot ); }
    
    // 座標間の向きに対して直角になるように調整
    rot -= 90;
    return( rot );
}
