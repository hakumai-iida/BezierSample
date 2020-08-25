/*+----------------------------------------------------------------+
  |	Title:		BezierForFill.cpp [共通環境]
  |	Comment:	ベジェ曲線：塗り
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
//-------------------------------
// [Fill] 描画実体：塗り
//-------------------------------
void CBezier::DrawFill( stBEZIER_FILL_POINT* pHead ){
    stBEZIER_FILL_POINT* pFP = pHead;
    
    while( pFP != NULL ){
        //----------------------------------------------
        // アングルの指定が有効であれば変換（※塗りの座標を傾ける）
        //----------------------------------------------
        float x = pFP->x;
        float y = pFP->y;
        if( pFP->angleRateLR != 0.0f || pFP->angleRateUD != 0.0f ){
            s_pBaseParam->pAnglePlane->calcXY( &x, &y, x, y, pFP->angleSearchKey, pFP->angleRateLR, pFP->angleRateUD );
        }

        // ワークパスなら塗りつぶし位置の出力
        if( s_bWorkPath ){
            if( ! IsDispOnlyAnchorPoint() ){
                bool isMarkDisp = true;
                
                // 塗りオプションの場合線別
                if( IS_FILL_OPTION_VALID( pFP->option ) ){
                    // 座標に意味のあるものは退避
                    if( pFP->option == eFILL_OPTION_POINT_AFTER_FILL ){}
                    else if( pFP->option == eFILL_OPTION_AREA ){}
                    // ここまできたら座標に意味はない
                    else{ isMarkDisp = false; }
                }

                if( isMarkDisp ){
                    PutCrossForWorkPath( x, y, BEZIER_WP_PAINT_PAL + pFP->workPathPalOfsS );
                }

            }
        }else{
            // 通常時ならフィルを取得して塗りつぶし
            CFill* pFill = GetFillWithFillPoint( pFP );

            if( pFill != NULL ){
                eSTROKE_TOUCH_TARGET touchId;
                bool isOnOutCol = pFP->isFillOnOutCol;

                // 塗りの明暗オフセット（※ここは明色と暗色の値の制限を見る）
                int darkOfs = pFP->darkOfs;
                if( darkOfs < -BEZIER_PAL_MAX_LEVEL_FOR_BRIGHT ){ darkOfs = -BEZIER_PAL_MAX_LEVEL_FOR_BRIGHT; }
                else if( darkOfs > BEZIER_PAL_MAX_LEVEL_FOR_DARK ){ darkOfs = BEZIER_PAL_MAX_LEVEL_FOR_DARK; }

                // オプションの明暗オフセット（※ここは最小〜最大までの変動を許可する）
                int optionDarkOfs = pFP->optionDarkOfs;
                if( optionDarkOfs < -(BEZIER_PAL_COL_NUM_IN_GRP-1) ){ optionDarkOfs = -(BEZIER_PAL_COL_NUM_IN_GRP-1); }
                else if( optionDarkOfs > (BEZIER_PAL_COL_NUM_IN_GRP-1) ){ optionDarkOfs = (BEZIER_PAL_COL_NUM_IN_GRP-1); }
                
                // 塗りオプション制限
                if( IsDispOffFillOption() ){
                    if( IS_FILL_OPTION_VALID( pFP->option ) ){
                        if( pFP->option != eFILL_OPTION_MASK ){
                            return;
                        }
                    }
                    darkOfs = 0;
                }

                switch( pFP->option ){
                // ポイント
                case eFILL_OPTION_POINT_AFTER_FILL:
                    if( optionDarkOfs < 0 ){
                        DrawFillPointBright( x, y, pFP->optionStrokeSize, pFP->optionStrokeRange, -optionDarkOfs );
                    }else if( optionDarkOfs > 0 ){
                        DrawFillPointDark( x, y, pFP->optionStrokeSize, pFP->optionStrokeRange, optionDarkOfs );
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_POINT_AFTER_FILL]\n" );
                    }
                    break;

                // アンチストローク（※座標情報は不要）
                case eFILL_OPTION_ANTI_STROKE_AFTER_FILL:
                    touchId = pFP->optionTouchId;
                    if( IS_STROKE_TOUCH_TARGET_VALID( touchId ) ){
                        if( optionDarkOfs < 0 ){
                            DrawFillAntiStrokeBright( touchId, pFP->optionStrokeSize, pFP->optionStrokeRange, -optionDarkOfs );
                        }else if( optionDarkOfs > 0 ){
                            DrawFillAntiStrokeDark( touchId, pFP->optionStrokeSize, pFP->optionStrokeRange, optionDarkOfs );
                        }else{
                            LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_ANTI_STROKE_AFTER_FILL]\n" );
                        }
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [touchId] for [eFILL_OPTION_ANTI_STROKE_AFTER_FILL] touchId=%d\n", touchId );
                    }
                    break;
                    
                // 明暗：領域指定
                case eFILL_OPTION_AREA:
                    if( optionDarkOfs != 0 ){
                        pFill->fillAdd( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                                        (int)x, (int)y, s_nTestPalGrp, optionDarkOfs, isOnOutCol );
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_AREA]\n" );
                    }
                    break;
                        
                // 明暗（※座標情報は不要）
                case eFILL_OPTION_RESERVE:
                    if( optionDarkOfs != 0 ){
                        pFill->ofsAddForReserve( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                                                 pFP->optionOfsX, pFP->optionOfsY, optionDarkOfs );
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_RESERVE]\n" );
                    }
                    break;

                // 明暗（※座標情報は不要）
                case eFILL_OPTION_INTO:
                    if( optionDarkOfs != 0 ){
                        pFill->ofsAddForInto( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                                              pFP->optionOfsX, pFP->optionOfsY, optionDarkOfs );
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_INTO]\n" );
                    }
                    break;

                // 明暗（※座標情報は不要）
                case eFILL_OPTION_COLOR:
                    if( optionDarkOfs != 0 ){
                        pFill->ofsAddForColor( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH,
                                               pFP->optionOfsX, pFP->optionOfsY, optionDarkOfs );
                    }else{
                        LOGE( "@ CBezier::DrawFill: INVALID [optionDarkOfs] for [eFILL_OPTION_COLOR]\n" );
                    }
                    break;

                // マスク
                case eFILL_OPTION_MASK:
                    // マスクはガードバッファ[FillGuard]へ描き込む
                    pFill->mask( s_pBufFillGuard, s_pBufFillGuide, s_nBufW, s_nBufH, (int)x, (int)y );
                    break;
                    
                // ここまできたら通常の塗りつぶし
                default:
                    pFill->fill( s_pBuf, s_pBufFillGuide, s_pBufFillGuard, s_nBufW, s_nBufH, (int)x, (int)y,
                                 pFP->palGrp, BEZIER_PAL_VAL_BASE+darkOfs, s_nTestPalGrp, isOnOutCol );
                    break;
                }
            }
        }
        // 次のポイントへ
        pFP = pFP->pNext;
    }
}

//-----------------------------------
// [Fill] ポイント：明るく
//-----------------------------------
void CBezier::DrawFillPointBright( float x, float y, int strokeSize, int strokeRange, int ofs ){
    // 一時バッファにポイントの出力
    if( ! DrawPointForTemp( x, y, strokeSize, strokeRange ) ){
        LOGE( "@ CBezier::DrawFillPointBright: FAILED x=%f, y=%f, strokeSize=%d, strokeRange=%d, ofs=%d\n",
              x, y, strokeSize, strokeRange, ofs );
        return;
    }

    // work
    int l = CBmpGenerator::GetLeftForTempBuf();
    int r = CBmpGenerator::GetRightForTempBuf();
    int t = CBmpGenerator::GetTopForTempBuf();
    int b = CBmpGenerator::GetBottomForTempBuf();

    BYTE* pBuf;
    BYTE* pTemp;
    BYTE* pGuide;
    BYTE* pGuard;

    // 一時バッファの内容をカラーバッファの明暗へ反映
    for( int i=t; i<=b; i++ ){
        pBuf = &s_pBuf[s_nBufW*i];
        pTemp = &s_pBufTemp[s_nBufW*i];
        pGuide = &s_pBufFillGuide[s_nBufW*i];
        pGuard = &s_pBufFillGuard[s_nBufW*i];

        for( int j=l; j<=r; j++ ){
            //----------------------------------------------------------------
            // 直近色であれば（※先行する処理で塗り潰された画素が対象）
            //----------------------------------------------------------------
            if( IS_BFG_LAST_COL( pGuide[j] ) && pGuard[j] == 0 ){
                int subVal = (ofs*pTemp[j]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( subVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( pBuf[j] );
                    if( palGrp >= BEZIER_PAL_GRP_HEAD && (s_nTestPalGrp==0x00 || palGrp==s_nTestPalGrp) ){
                        // ストロークの濃さにより対象画素を明るく
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( pBuf[j] ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        pBuf[j] = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
        }
        
        // ついでに値のクリア（※テンポラリバッファのクリアを呼ばなくても済むように）
        memset( &pTemp[l], 0, r-l+1 );
    }

    // テンポラリ情報のクリア
    CBmpGenerator::ResetTempBufInfo();
}

//-----------------------------------
// [Fill] ポイント：暗く
//-----------------------------------
void CBezier::DrawFillPointDark( float x, float y, int strokeSize, int strokeRange, int ofs ){
    // 一時バッファにポイントの出力
    if( ! DrawPointForTemp( x, y, strokeSize, strokeRange ) ){
        LOGE( "@ CBezier::DrawFillPointDark: FAILED x=%f, y=%f, strokeSize=%d, strokeRange=%d, ofs=%d\n",
              x, y, strokeSize, strokeRange, ofs );
        return;
    }

    // work
    int l = CBmpGenerator::GetLeftForTempBuf();
    int r = CBmpGenerator::GetRightForTempBuf();
    int t = CBmpGenerator::GetTopForTempBuf();
    int b = CBmpGenerator::GetBottomForTempBuf();

    BYTE* pBuf;
    BYTE* pTemp;
    BYTE* pGuide;
    BYTE* pGuard;

    // 一時バッファの内容をカラーバッファの明暗へ反映
    for( int i=t; i<=b; i++ ){
        pBuf = &s_pBuf[s_nBufW*i];
        pTemp = &s_pBufTemp[s_nBufW*i];
        pGuide = &s_pBufFillGuide[s_nBufW*i];
        pGuard = &s_pBufFillGuard[s_nBufW*i];

        for( int j=l; j<=r; j++ ){
            //----------------------------------------------------------------
            // 直近色であれば（※先行する処理で塗り潰された画素が対象）
            //----------------------------------------------------------------
            if( IS_BFG_LAST_COL( pGuide[j] ) && pGuard[j] == 0 ){
                int addVal = (ofs*pTemp[j]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( addVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( pBuf[j] );
                    if( palGrp >= BEZIER_PAL_GRP_HEAD && (s_nTestPalGrp==0x00 || palGrp==s_nTestPalGrp) ){
                        // ストロークの濃さにより対象画素を暗く
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( pBuf[j] ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        pBuf[j] = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
        }
        
        // ついでに値のクリア（※テンポラリバッファのクリアを呼ばなくても済むように）
        memset( &pTemp[l], 0, r-l+1 );
    }

    // テンポラリ情報のクリア
    CBmpGenerator::ResetTempBufInfo();
}

//-----------------------------------
// [Fill] アンチストローク：明るく
//-----------------------------------
void CBezier::DrawFillAntiStrokeBright( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange, int ofs ){
    // 一時バッファにストロークの出力
    if( ! DrawStrokeWithTouchForTemp( touchId, strokeSize, strokeRange ) ){
        LOGE( "@ CBezier::DrawFillAntiStrokeBright: FAILED touchId=%d, strokeSize=%d, strokeRange=%d, ofs=%d\n",
              touchId, strokeSize, strokeRange, ofs );
        return;
    }

    // work
    int l = CBmpGenerator::GetLeftForTempBuf();
    int r = CBmpGenerator::GetRightForTempBuf();
    int t = CBmpGenerator::GetTopForTempBuf();
    int b = CBmpGenerator::GetBottomForTempBuf();

    BYTE* pBuf;
    BYTE* pTemp;
    BYTE* pGuide;
    BYTE* pGuard;

    // 一時バッファの内容をカラーバッファの明暗へ反映
    for( int i=t; i<=b; i++ ){
        pBuf = &s_pBuf[s_nBufW*i];
        pTemp = &s_pBufTemp[s_nBufW*i];
        pGuide = &s_pBufFillGuide[s_nBufW*i];
        pGuard = &s_pBufFillGuard[s_nBufW*i];

        for( int j=l; j<=r; j++ ){
            //----------------------------------------------------------------
            // 直近色でなければ（※ストローク境界上の塗り潰されなかった画素がアンチ対象）
            //----------------------------------------------------------------
            if( IS_NOT_BFG_LAST_COL( pGuide[j] ) && pGuard[j] == 0 ){
                int subVal = (ofs*pTemp[j]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( subVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( pBuf[j] );
                    if( palGrp >= BEZIER_PAL_GRP_HEAD && (s_nTestPalGrp==0x00 || palGrp==s_nTestPalGrp) ){
                        // ストロークの濃さにより対象画素を明るく
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( pBuf[j] ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        pBuf[j] = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
        }
        
        // ついでに値のクリア（※テンポラリバッファのクリアを呼ばなくても済むように）
        memset( &pTemp[l], 0, r-l+1 );
    }

    // テンポラリ情報のクリア
    CBmpGenerator::ResetTempBufInfo();
}

//-----------------------------------
// [Fill] アンチストローク：暗く
//-----------------------------------
void CBezier::DrawFillAntiStrokeDark( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange, int ofs ){
    // 一時バッファにストロークの出力
    if( ! DrawStrokeWithTouchForTemp( touchId, strokeSize, strokeRange ) ){
        LOGE( "@ CBezier::DrawFillAntiStrokeDark: FAILED touchId=%d, strokeSize=%d, strokeRange=%d, ofs=%d\n",
              touchId, strokeSize, strokeRange, ofs );
        return;
    }

    // work
    int l = CBmpGenerator::GetLeftForTempBuf();
    int r = CBmpGenerator::GetRightForTempBuf();
    int t = CBmpGenerator::GetTopForTempBuf();
    int b = CBmpGenerator::GetBottomForTempBuf();

    BYTE* pBuf;
    BYTE* pTemp;
    BYTE* pGuide;
    BYTE* pGuard;

    // 一時バッファの内容をカラーバッファの明暗へ反映
    for( int i=t; i<=b; i++ ){
        pBuf = &s_pBuf[s_nBufW*i];
        pTemp = &s_pBufTemp[s_nBufW*i];
        pGuide = &s_pBufFillGuide[s_nBufW*i];
        pGuard = &s_pBufFillGuard[s_nBufW*i];

        for( int j=l; j<=r; j++ ){
            //----------------------------------------------------------------
            // 直近色でなければ（※ストローク境界上の塗り潰されなかった画素がアンチ対象）
            //----------------------------------------------------------------
            if( IS_NOT_BFG_LAST_COL( pGuide[j] ) && pGuard[j] == 0 ){
                int addVal = (ofs*pTemp[j]) / STROKE_DOT_OUT_PRECISION_MAX;
                if( addVal > 0 ){
                    //-----------------------------------
                    // 対象パレットであれば
                    //-----------------------------------
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( pBuf[j] );
                    if( palGrp >= BEZIER_PAL_GRP_HEAD && (s_nTestPalGrp==0x00 || palGrp==s_nTestPalGrp) ){
                        // ストロークの濃さにより対象画素を暗く
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( pBuf[j] ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        pBuf[j] = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
        }
        
        // ついでに値のクリア（※テンポラリバッファのクリアを呼ばなくても済むように）
        memset( &pTemp[l], 0, r-l+1 );
    }

    // テンポラリ情報はクリア
    CBmpGenerator::ResetTempBufInfo();
}

//------------------------------------------------------
// [Fill] 描画実体：指定位置にポイントを描画する
//-------------------------------------------------------
bool CBezier::DrawPointForTemp( float x, float y, int strokeSize, int strokeRange ){
    CStroke* pStroke = CStrokeMgr::GetStroke( eSTROKE_TYPE_CIRCLE, strokeSize, strokeRange );
    if( pStroke == NULL ){
        LOGE( "@ CBezier::DrawPointForTemp: INVALID STROKE strokeSize=%d, strokeRange=%d\n", strokeSize, strokeRange );
        return( false );
    }

    // 指定の１ドットの出力
    PutDotAtForTemp( x, y, pStroke );
    return( true );
}

//------------------------------------------------------
// [Fill] 描画実体：一時バッファにタッチに沿ったストロークを描画する
//-------------------------------------------------------
bool CBezier::DrawStrokeWithTouchForTemp( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange ){
    if( ! IS_STROKE_TOUCH_TARGET_VALID( touchId ) ){
        LOGE( "@ CBezier::DrawStrokeWithTouchForTemp: INVALID touchId=%d\n", touchId );
        return( false );
    }
    
    CStroke* pStroke = CStrokeMgr::GetStroke( eSTROKE_TYPE_CIRCLE, strokeSize, strokeRange );
    if( pStroke == NULL ){
        LOGE( "@ CBezier::DrawStrokeWithTouchForTemp: INVALID STROKE strokeSize=%d, strokeRange=%d\n", strokeSize, strokeRange );
        return( false );
    }
    
    // タッチ登録されている座標の描画（※傾き後の座標で処理する）
    int num = s_nArrTouchPointNum[touchId];
    float* arrX = s_fArrArrTouchPointX[touchId];
    float* arrY = s_fArrArrTouchPointY[touchId];
    for( int i=0; i<num; i++ ){
        PutDotAtForTemp( arrX[i], arrY[i], pStroke );
    }
    
    return( true );
}

//------------------------------
// [Fill] 一時バッファにドットを置く
//------------------------------
void CBezier::PutDotAtForTemp( float x0, float y0, CStroke* pStroke ){
    // 座標調整（※ドットの中央に描画されるように）
    int size = (int)pStroke->getSize();
    float x = x0 - size/2.0f;
    float y = y0 - size/2.0f;

    // 座標のはみ出し具合の算出
    int iX0 = (int)x;
    int iY0 = (int)y;
    float rateOutX = x - iX0;   // 小数点以下がＸ方向のはみ出し
    float rateOutY = y - iY0;   // 小数点以下がＹ方向のはみ出し
    
    // ストローク画像取得
    stSTROKE_OUTPUT_CELL* pCell = pStroke->getStrokeCell( rateOutX, rateOutY );
    int w = pCell->sizeW;
    int h = pCell->sizeH;
    int iX = iX0 + pCell->ofsX; // １ピクセルを超えるオフセットの適用
    int iY = iY0 + pCell->ofsY; // １ピクセルを超えるオフセットの適用

    // 出力
    for( int i=0; i<h; i++ ){
        BYTE* pOut = &s_pBufTemp[s_nBufW*(iY+i)+iX];
        BYTE* pDot = &pCell->pBuf[w*i];
        
        for( int j=0; j<w; j++ ){
            if( pOut[j] < STROKE_DOT_OUT_PRECISION_MAX ){
                pOut[j] += pDot[j];
                if( pOut[j] > STROKE_DOT_OUT_PRECISION_MAX ){
                    pOut[j] = STROKE_DOT_OUT_PRECISION_MAX;
                }
            }
        }
    }
    
    // 一時バッファの領域を更新
    CBmpGenerator::UpdateTempBufInfo( iX, iY );        // 左上
    CBmpGenerator::UpdateTempBufInfo( iX+w, iY+h );    // 右下
}
