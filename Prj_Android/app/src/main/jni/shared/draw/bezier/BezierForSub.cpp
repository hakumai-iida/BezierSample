/*+----------------------------------------------------------------+
  |	Title:		BezierForSub.cpp [共通環境]
  |	Comment:	ベジェ曲線：サブ
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
//---------------------------------------
// [Sub] ループ間を行き来する際の環境リセット
//---------------------------------------
void CBezier::ResetEnv( void ){
    // 各種フラグオフ
    SetDispOffTouch( false );
    SetDispOffStripe( false );
    SetDispOffFrill( false );
    SetDispOffEdge( false );
    SetDispOffFillOption( false );

    SetDispDirVectorAlways( false );
    SetDispOffDirVectorAlways( false );
    SetDispAllSelected( false );
    SetDispOnlyAnchorPoint( false );
    
    SetDispIndividualPart( false );

    // 開発フラグオフ
    ResetAllFlagForDev();

    // BMP環境リセット
    CBmpGenerator::Reset();

    // 固定ブラシ解除
    CBezierStructConv::SetFixedBrush( eBRUSH_INVALID );
    
    // パレットをデフォルトに
    CBmpPalData* pPal = CBmpPalMgr::GetBmpPalDataAt( 0 );
    pPal->loadToImgPal( eIMG_BMP_PAL_00 );
}

//------------------------
// 全フラグリセット（※開発用）
//------------------------
void CBezier::ResetAllFlagForDev( void ){
    // BmpDotPartData
    CBmpDotPartData::SetFlagOffAll( eBDPD_FLAG_TEMP_EDIT_TARGET );
    CBmpDotPartData::SetFlagOffAll( eBDPD_FLAG_TEMP_SELECTED );
    CBmpDotPartData::SetFlagOffAll( eBDPD_FLAG_TEMP_ACTIVE );

    // LayerData
    CLayerData::SetFlagOffAll( eLAYER_FLAG_TEMP_SELECTED );
    CLayerData::SetFlagOffAll( eLAYER_FLAG_TEMP_ACTIVE );

    // LineObjectData
    CLineObjectData::SetFlagOffAll( eLOD_FLAG_TEMP_SELECTED );
    CLineObjectData::SetFlagOffAll( eLOD_FLAG_TEMP_ACTIVE );

    // PaintObjectData
    CPaintObjectData::SetFlagOffAll( ePOD_FLAG_TEMP_SELECTED );
    CPaintObjectData::SetFlagOffAll( ePOD_FLAG_TEMP_ACTIVE );

    // SlotObjectData
    CSlotObjectData::SetFlagOffAll( eSOD_FLAG_TEMP_SELECTED );
    CSlotObjectData::SetFlagOffAll( eSOD_FLAG_TEMP_ACTIVE );

    // AnchorPointData
    CAnchorPointData::SetFlagOffAll( eAPD_FLAG_TEMP_SELECTED );
    CAnchorPointData::SetFlagOffAll( eAPD_FLAG_TEMP_ACTIVE );

    // FillPointData
    CFillPointData::SetFlagOffAll( eFPD_FLAG_TEMP_SELECTED );
    CFillPointData::SetFlagOffAll( eFPD_FLAG_TEMP_ACTIVE );

    // SlotPointData
    CSlotPointData::SetFlagOffAll( eSPD_FLAG_TEMP_SELECTED );
    CSlotPointData::SetFlagOffAll( eSPD_FLAG_TEMP_ACTIVE );
}

//---------------------------------------
// [Sub] 描画実体：ストローク
//---------------------------------------
void CBezier::DrawStroke( stBEZIER_ANCHOR_POINT* pHead, float skipHeadRate, float skipTailRate ){
    stBEZIER_ANCHOR_POINT* pP0 = pHead;
    
    //---------------------------------------------------
    // フック調整
    //---------------------------------------------------
    pP0 = pHead;
    while( pP0 != NULL ){
        // フック解決（※傾き適用前）
        ResolveHookPoint( &pP0->x, &pP0->y,
                          pP0->systemHookTargetIdFixed, pP0->hookTargetIdFixed,
                          pP0->isOfsValid, pP0->ofsX, pP0->ofsY, pP0->reHookTargetIdFixed );

        // 次のポイントへ（※閉じていたら抜ける）
        pP0 = pP0->pNext;
        if( pP0 == pHead ){
            break;
        }
    }
    
    //---------------------------------------------------
    // ワークパス指定であればポイントと方向線の表示
    //---------------------------------------------------
    if( s_bWorkPath ){
        pP0 = pHead;
        while( pP0 != NULL ){
            //------------------------------------------------------
            // アングルの指定が有効であれば変換（※ワークパスのAPの座標を傾ける）
            //------------------------------------------------------
            float x = pP0->x;
            float y = pP0->y;
            if( s_pLayerParam->fixedAngleRateLR != 0.0f || s_pLayerParam->fixedAngleRateUD != 0.0f ){
                s_pBaseParam->pAnglePlane->calcXY( &x, &y, x, y,
                                                   pP0->angleSearchKey, s_pLayerParam->fixedAngleRateLR, s_pLayerParam->fixedAngleRateUD );
            }

            // アンカーの表示（※黒い四角）
            PutPointForWorkPath( x, y, pP0 );
            
            // 方向線の表示（※赤い矢印＝誘引ベクトル、青い矢印＝誘導ベクトル）（※方向線は場合によって煩わしいので色々と表示の分岐）
            if( !IsDispOffDirVectorAlways() && (pP0->workPathPalOfsS != BEZIER_WP_PAL_VAL_FOR_SLEEP || IsDispDirVectorAlways()) ){
                if( pP0->xIn != 0.0f || pP0->yIn != 0.0f ){
                    PutLinearForWorkPath( x, y, pP0->xIn, pP0->yIn, BEZIER_WP_DIR_IN_PAL + pP0->workPathPalOfsS );
                    PutTriangleForWorkPath( x, y, pP0->xIn, pP0->yIn, BEZIER_WP_DIR_IN_PAL + pP0->workPathPalOfsS, false );
                }
                
                if( pP0->xOut != 0.0f || pP0->yOut != 0.0f ){
                    PutLinearForWorkPath( x, y, pP0->xOut, pP0->yOut, BEZIER_WP_DIR_OUT_PAL + pP0->workPathPalOfsS );
                    PutTriangleForWorkPath( x, y, pP0->xOut, pP0->yOut, BEZIER_WP_DIR_OUT_PAL + pP0->workPathPalOfsS, true );
                }
            }
            
            // 次のポイントへ（※閉じていたら抜ける）
            pP0 = pP0->pNext;
            if( pP0 == pHead ){
                break;
            }
        }
    }
    
    //---------------------------------------------------
    // 分割点リストの構築（※[pNext==NULL]で抜けている点に注意）
    //---------------------------------------------------
    pP0 = pHead;
    stBEZIER_ANCHOR_POINT* pLastPrev = pP0;
    while( pP0->pNext != NULL ){
        pLastPrev = pP0;
        stBEZIER_ANCHOR_POINT* pP1 = pP0->pNext;
        
        // [pP0, pP1)間の分割点をリストへ登録
        RegistDivisionPointList( pP0, pP1 );
    
        // 次のポイントへ（※閉じていたら抜ける）
        pP0 = pP1;
        if( pP0 == pHead ){
            break;
        }
    }

    // 終点を追加（※[registDivisionPointList]は[pP1]の座標を登録しないため、この時点では末尾の点が未登録となっている）
    //（※ここで追加されるのは、オープンパスなら最後のアンカーポイント、クローズパスなら最初のアンカーポイントとなる）
    float strokeSize = pLastPrev->strokeSize * pLastPrev->strokeEndSize;
    AddDivisionPoint( pP0->x, pP0->y, strokeSize, true, pP0 );

    // 用心（※分割点が２未満だと線が引けない）
    if( s_nUseDivisionPoint < 2 ){
        LOGE( "@ CBezier::DrawStroke: DIVISION POINT(%d) LESS THAN 2\n", s_nUseDivisionPoint );
        return;
    }

    //---------------------------------------------------------
    // スキップ指定があればリミッタの設定
    //---------------------------------------------------------
    if( !s_bWorkPath ){
        if( skipHeadRate > 0.0f || skipTailRate > 0.0f ){
            // 各分割点間のポイント数の算出
            for( int i=1; i<s_nUseDivisionPoint; i++ ){
                int numPoint = CalcPointNum( s_stArrDivisionPoint[i-1].x, s_stArrDivisionPoint[i-1].y,
                                             s_stArrDivisionPoint[i].x, s_stArrDivisionPoint[i].y );
                s_nSkipStrokePointMax += numPoint;
            }
            
            // オープンパスであれば終点を考慮
            stBEZIER_DIVISION_POINT* pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint-1];
            if( pDP->x != s_stArrDivisionPoint[0].x || pDP->y != s_stArrDivisionPoint[0].y ){
                s_nSkipStrokePointMax += 1;
            }
            
            // 制限数の確定：先頭
            s_nSkipStrokePointLimitForHead = (int)(s_nSkipStrokePointMax*skipHeadRate);
            if( s_nSkipStrokePointLimitForHead < 0 ){
                s_nSkipStrokePointLimitForHead = 0;
            }
            
            // 制限数の確定：末尾
            s_nSkipStrokePointLimitForTail = (int)(s_nSkipStrokePointMax * (1.0f-skipTailRate));
            if( s_nSkipStrokePointLimitForTail > s_nSkipStrokePointMax ){
                s_nSkipStrokePointLimitForTail = s_nSkipStrokePointMax;
            }
        }
    }

    // ブラシのブレ設定
    ResetBrushShake();
    SetBrushShake();
    
    //--------------------------
    // 分割点による線の描画
    //--------------------------
    for( int i=1; i<s_nUseDivisionPoint; i++ ){
        // 始点[i-1]から、終点[i]に線を伸ばす（※スタイル／色は始点[i-1]のもの）
        PutDotLinear( &s_stArrDivisionPoint[i-1], &s_stArrDivisionPoint[i], s_stArrDivisionPoint[i-1].pAP );
    }
    
    // オープンパスであれば終点を描画（※[registDivisionPointList]同様に最後の点が表示されないので別途描画）
    //（※ここは[最後の一つ手前]の分割点から見た終点を描画するため[s_nUseDivisionPoint-2]の[AP]を渡している）
    stBEZIER_DIVISION_POINT* pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint-1];
    if( pDP->x != s_stArrDivisionPoint[0].x || pDP->y != s_stArrDivisionPoint[0].y  ){
        PutDotLinear( pDP, pDP, s_stArrDivisionPoint[s_nUseDivisionPoint-2].pAP );
    }
    
    //-----------------------------------
    // 後始末：通常ストロークであれば
    //-----------------------------------
    if( !s_bWorkPath && !s_bTouch ){
        if( ! IsDispOffEdge() ){
            // 指定があればストロークのエッジ塗りの確認
            // TODO:ストロークスキップの際に表示されてしまう（※ストロークスキップの際は何かしらの調整をすべきか？）
            for( int i=0; i<s_nUseDivisionPoint; i++ ){
                if( s_stArrDivisionPoint[i].isEdgeFillCheck ){
                    DrawStrokeEdge( i );
                }
            }
        }
    }
}

//---------------------------------------
// [Sub] 描画実体：ドット
//---------------------------------------
void CBezier::DrawDot( stBEZIER_ANCHOR_POINT* pHead ){
    stBEZIER_ANCHOR_POINT* pBAP = pHead;
    
    // 各点の表示
    while( pBAP != NULL ){
        // フック解決（※アングル適用前）
        ResolveHookPoint( &pBAP->x, &pBAP->y,
                          pBAP->systemHookTargetIdFixed, pBAP->hookTargetIdFixed,
                          pBAP->isOfsValid, pBAP->ofsX, pBAP->ofsY, pBAP->reHookTargetIdFixed );
                
        //-----------------------------------------------
        // アングルの指定が有効であれば変換（※ドットの座標を傾ける）
        //-----------------------------------------------
        float x = pBAP->x;
        float y = pBAP->y;
        if( s_pLayerParam->fixedAngleRateLR != 0.0f || s_pLayerParam->fixedAngleRateUD != 0.0f ){
            s_pBaseParam->pAnglePlane->calcXY( &x, &y, x, y,
                                               pBAP->angleSearchKey, s_pLayerParam->fixedAngleRateLR, s_pLayerParam->fixedAngleRateUD );
        }

        // ワークパス時はポイントの描画
        if( s_bWorkPath ){
            PutPointForWorkPath( x, y, pBAP );
        }
        // 通常時はブラシによる描画
        else{
            CStroke* pStroke = GetStrokeWithAnchorPoint( pBAP );
            if( pStroke != NULL ){
                // ドット時はアンカーポイント（開始点）のサイズのみを反映させる（※終点は無視）
                float strokeSize = pBAP->strokeSize * pBAP->strokeStartSize;
                if( strokeSize > 0.0f ){
                    PutDotAt( x, y, strokeSize, s_nTestPalGrp, pStroke, pBAP );
                }
            }
        }
        
        // 次のポイントへ
        pBAP = pBAP->pNext;
        
        // 用心（※クローズパスの場合は抜ける）
        if( pBAP == pHead ){
            break;
        }
    }
}

//---------------------------------------
// [Sub] ワーククリア
//---------------------------------------
void CBezier::ClearWork( void ){
    ClearWorkSetting();
    ClearWorkPoint();
    ClearWorkStripePoint();
}

//---------------------------------------
// [Sub] ワーク設定クリア
//---------------------------------------
void CBezier::ClearWorkSetting( void ){
    s_pBaseParam = NULL;
    s_pLayerParam = NULL;

    s_pBuf = NULL;
    s_pBufTest = NULL;
    s_pBufFillGuide = NULL;
    s_pBufFillGuard = NULL;
    s_pBufTemp = NULL;

    s_nBufW = s_nBufH = 0;

    s_nTestPalGrp = 0x00;
    s_nTestPalGrpForRepair = 0x00;
    s_bTouch = false;
    s_bStripe = false;
    s_bFrill = false;
    s_bWorkPath = false;
}

//---------------------------------------
// [Sub] ワークポイントクリア
//---------------------------------------
void CBezier::ClearWorkPoint( void ){
    s_nUseDivisionPoint = 0;
    s_nNumStripeGuidePoint = 0;
    s_nSkipStrokePointCur = 0;
    s_nSkipStrokePointMax = 0;
    s_nSkipStrokePointLimitForHead = 0;
    s_nSkipStrokePointLimitForTail = 0;
}

//---------------------------------------
// [Sub] ストライプ登録クリア
//---------------------------------------
void CBezier::ClearWorkStripePoint( void ){
    s_nStripePointNum = 0;
}

//---------------------------------------
// [Sub] ワーク設定：ストローク用
//---------------------------------------
bool CBezier::SetBufForStroke( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForStroke: INVALID DATA: pParam=%p, pLayerParam=%p\n", pParam, pLayerParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufLine == NULL || pParam->pBufFillGuide == NULL || pParam->pBufColor == NULL ||
        pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForStroke: INVALID PARAM: pBuf=%p, pBufFillGuide=%p, pBufColor=%p, w=%d, h=%d\n",
              pParam->pBufLine, pParam->pBufFillGuide, pParam->pBufColor, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    s_pLayerParam = pLayerParam;
    s_bWorkPath = isWorkPath;

    s_pBuf = s_pBaseParam->pBufLine;
    s_pBufTest = s_pBaseParam->pBufColor;
    s_pBufFillGuide = s_pBaseParam->pBufFillGuide;
    s_pBufFillGuard = s_pBaseParam->pBufFillGuard;
    s_pBufTemp = s_pBaseParam->pBufTemp;
    
    s_nBufW = pParam->texW;
    s_nBufH = pParam->texH;
    
    return( true );
}

//---------------------------------------
// [Sub] ワーク設定：塗り用
//---------------------------------------
bool CBezier::SetBufForFill( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForFill: INVALID DATA: pParam=%p, pLayerParam=%p\n", pParam, pLayerParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufColor == NULL ||  pParam->pBufFillGuide == NULL || pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForFill: INVALID PARAM: pBufColor=%p, pBufFillGuide=%p, w=%d, h=%d\n",
              pParam->pBufColor, pParam->pBufFillGuide, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    s_pLayerParam = pLayerParam;
    s_bWorkPath = isWorkPath;

    s_pBuf = s_pBaseParam->pBufColor;
    s_pBufFillGuide = s_pBaseParam->pBufFillGuide;
    s_pBufFillGuard = s_pBaseParam->pBufFillGuard;
    s_pBufTemp = s_pBaseParam->pBufTemp;
    s_nBufW = s_pBaseParam->texW;
    s_nBufH = s_pBaseParam->texH;
    
    // ワークパスの指定であれば出力先をメインバッファへ
    if( s_bWorkPath ){
        s_pBuf = s_pBaseParam->pBufLine;
    }
    return( true );
}

//---------------------------------------
// [Sub] ワーク設定：開発用
//---------------------------------------
bool CBezier::SetBufForDev( stBEZIER_BASE_PARAM* pParam ){
    // 無効確認：データ
    if( pParam == NULL  ){
        LOGE( "@ CBezier::SetBufForDev: INVALID DATA: pParam=%p\n", pParam );
        return( false );
    }
    
    // 無効確認：バッファ
    if( pParam->pBufLine == NULL || pParam->texW <= 0 || pParam->texH <= 0 ){
        LOGE( "@ CBezier::SetBufForDev: INVALID PARAM: pBuf=%p, w=%d, h=%d\n", pParam->pBufLine, pParam->texW, pParam->texH );
        return( false );
    }

    s_pBaseParam = pParam;
    
    s_pBuf = s_pBaseParam->pBufLine;
    s_nBufW = s_pBaseParam->texW;
    s_nBufH = s_pBaseParam->texH;
    
    return( true );
}

//---------------------------------------
// [Sub] ブラシのブレ：リセット
//---------------------------------------
void CBezier::ResetBrushShake( void ){
    s_oSpringForBrushPos.clearSpring();
    s_oSpringForBrushSize.clearSpring();
    m_nResetCountForBrushPos = 0;
    m_nResetCountForBrushSize = 0;
    m_nUpdateFrameForBrushPos = 0.0f;
    m_nUpdateFrameForBrushSize = 0.0f;
}

//---------------------------------------
// [Sub] ブラシのブレ：設定
//---------------------------------------
void CBezier::SetBrushShake( void ){
    // バネの動きは一定（※緩い動きで減衰しない）
    s_oSpringForBrushPos.set( 10, 0.01f, 1.0f );
    s_oSpringForBrushPos.setV( 10 );
    s_oSpringForBrushPos.start();
    
    // バネの動きは一定（※緩い動きで減衰しない）
    s_oSpringForBrushSize.set( 10, 0.01f, 1.0f );
    s_oSpringForBrushSize.setV( 10 );
    s_oSpringForBrushSize.start();
}

//---------------------------------------
// [Sub] ブラシのブレ：更新
//---------------------------------------
void CBezier::UpdateBrushShake( void ){
    if( m_nResetCountForBrushPos <= 0 ){
        m_nUpdateFrameForBrushPos = 0.3f + CRand::GetRandF();
        m_nResetCountForBrushPos = 32 + CRand::GetRand( 64 );
    }else{
        m_nResetCountForBrushPos--;
    }
    s_oSpringForBrushPos.updateWithFrame( m_nUpdateFrameForBrushPos );
    
    if( m_nResetCountForBrushSize <= 0 ){
        m_nUpdateFrameForBrushSize = 0.3f + CRand::GetRandF();
        m_nResetCountForBrushSize = 32 + CRand::GetRand( 64 );
    }else{
        m_nResetCountForBrushSize--;
    }
    s_oSpringForBrushSize.updateWithFrame( m_nUpdateFrameForBrushSize );
}

//---------------------------------------
// [Sub] ブラシのブレ：Ｘ位置
//---------------------------------------
float CBezier::GetBrushShakeForPosX( void ){
    return( s_oSpringForBrushPos.getY()/100.0f );
}

//---------------------------------------
// [Sub] ブラシのブレ：Ｙ位置
//---------------------------------------
float CBezier::GetBrushShakeForPosY( void ){
    return( s_oSpringForBrushPos.getY()/100.0f );
}

//---------------------------------------
// [Sub] ブラシのブレ：サイズ
//---------------------------------------
float CBezier::GetBrushShakeForSize( void ){
    return( s_oSpringForBrushSize.getY()/300.0f );
}

//---------------------------------------
// [Sub] ガイド復旧
//---------------------------------------
void CBezier::RecoverGuide( int idFixed ){
    // ワーク時は無視
    if( s_bWorkPath ){
        return;
    }
    
    // 無効は無視
    if( ! IS_STROKE_GUIDE_TARGET_FIXED_VALID( idFixed ) ){
        LOGE( "@ CBezier::RecoverGuide: INVALID idFixed=%d\n", idFixed );
        return;
    }

    // 要素がないのは何か変
    int num = s_nArrGuidePointNum[idFixed];
    if( num <= 0 ){
        LOGE( "@ CBezier::RecoverGuide: NO GUIDE POINT idFixed=%d, num=%d\n", idFixed, num );
        return;
    }

    float* arrX = s_fArrArrGuidePointX[idFixed];
    float* arrY = s_fArrArrGuidePointY[idFixed];
    for( int i=0; i<num; i++ ){
        int at = ((int)arrY[i])*s_nBufW + ((int)arrX[i]);
        SET_BFG_UNDERCOAT_COL( s_pBufFillGuide[at] );
    }
}

//---------------------------------------
// [Sub] ガイド復旧（タッチ経由）
//---------------------------------------
void CBezier::RecoverGuideWithTouch( int idFixed ){
    // ワーク時は無視
    if( s_bWorkPath ){
        return;
    }

    // 無効は無視
    if( ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( idFixed ) ){
        LOGE( "@ CBezier::RecoverGuideWithTouch: INVALID idFixed=%d\n", idFixed );
        return;
    }

    // 要素がないのは何か変
    int num = s_nArrTouchPointNum[idFixed];
    if( num <= 0 ){
        LOGE( "@ CBezier::RecoverGuideWithTouch: NO TOUCH POINT idFixed=%d, num=%d\n", idFixed, num );
        return;
    }

    float* arrX = s_fArrArrTouchPointX[idFixed];
    float* arrY = s_fArrArrTouchPointY[idFixed];
    for( int i=0; i<num; i++ ){
        int at = ((int)arrY[i])*s_nBufW + ((int)arrX[i]);
        SET_BFG_UNDERCOAT_COL( s_pBufFillGuide[at] );
    }
}

//--------------------------
// APからストロークの取得
//--------------------------
CStroke* CBezier::GetStrokeWithAnchorPoint( stBEZIER_ANCHOR_POINT* pAP ){
    // 無効は無視
    if( pAP == NULL ){ return( NULL ); }
    if( ! IS_STROKE_TYPE_VALID( pAP->type ) ){ return( NULL ); }
    if( pAP->type == eSTROKE_TYPE_OFF ){ return( NULL ); }
    
    // タッチ扱いのストロークであればタッチオフ時には無効
    if( pAP->bAsTouchStroke && IsDispOffTouch() ){ return( NULL ); }

    // ここまできたらマネージャへ問い合わせる
    return( CStrokeMgr::GetStroke( pAP->type, pAP->size, pAP->ofsRange ) );
}

//--------------------------
// FPから塗りの取得
//--------------------------
CFill* CBezier::GetFillWithFillPoint( stBEZIER_FILL_POINT* pFP ){
    // 無効は無視
    if( pFP == NULL ){ return( NULL ); }
    
    // オプションが無効の場合は塗り潰しタイプの確認
    if( ! IS_FILL_OPTION_VALID( pFP->option ) ){
        if( ! IS_FILL_TYPE_VALID( pFP->type ) ){ return( NULL ); }
        if( pFP->type == eFILL_TYPE_OFF ){ return( NULL ); }
    }
    
    // ここまできたらマネージャへ問い合わせる
    return( CFillMgr::GetFill( pFP->type ) );
}

//--------------------------
// フック登録のクリア
//--------------------------
void CBezier::ClearHookPoint( int idFixed ){
    // 枠が無効なら無視
    if( ! IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
        return;
    }

    s_bArrHookPoint[idFixed] = false;
}

//--------------------------
// フック情報の取得
//--------------------------
// 登録済みか？
bool CBezier::IsHookPointRegistered( int idFixed ){
    // 枠が無効なら無視
    if( ! IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
        return( false );
    }

    return( s_bArrHookPoint[idFixed] );
}

// X
float CBezier::GetHookPointX( int idFixed ){
    // 枠が無効なら無視
    if( ! IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
        return( 0.0f );
    }

    // 未登録は無視
    if( ! s_bArrHookPoint[idFixed] ){
        return( 0.0f );
    }
    
    // ここまできたら座標を返す
    return( s_fArrHookPointX[idFixed] );
}

// Y
float CBezier::GetHookPointY( int idFixed ){
    // 枠が無効なら無視
    if( ! IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
        return( 0.0f );
    }

    // 未登録は無視
    if( ! s_bArrHookPoint[idFixed] ){
        return( 0.0f );
    }
    
    // ここまできたら座標を返す
    return( s_fArrHookPointY[idFixed] );
}


//-----------------
// タッチ合成
//-----------------
bool CBezier::MergeTouchPoint( int destIdFixed, bool isDestMinus, int optIdFixed, bool isOptMinus ){
    // 対象が無効なら無視
    if( ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( destIdFixed ) ){
        LOGE( "@ CBezier::MergeTouchPoint: INVALID DEST: destIdFixed=%d, optIdFixed=%d", destIdFixed, optIdFixed );
        return( false );
    }
    
    // オプションが無効なら無視（※正常な無効値は許容する）
    if( optIdFixed != eSTROKE_TOUCH_TARGET_INVALID && ! IS_STROKE_TOUCH_TARGET_FIXED_VALID( optIdFixed ) ){
        LOGE( "@ CBezier::MergeTouchPoint: INVALID OPT: destIdFixed=%d, optIdFixed=%d", destIdFixed, optIdFixed );
        return( false );
    }

    // 登録数の確認
    int num = s_nArrTouchPointNum[destIdFixed];

    int numOpt = 0;
    if( optIdFixed != eSTROKE_TOUCH_TARGET_INVALID ){
        numOpt = s_nArrTouchPointNum[optIdFixed];
    }

    // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
    if( (num+numOpt) > BEZIER_TOUCH_POINT_MAX ){
        LOGE( "@ CBezier::MergeTouchPoint: TOUCH POINT BUF SHORTAGE: touch[%d]=(%d+%d)/%d\n",
              destIdFixed, num, numOpt, BEZIER_TOUCH_POINT_MAX );
        return( false );
    }

    // 対象の事前処理（※マイナス指定があれば反転）
    if( isDestMinus ){
        float *pDestX = &s_fArrArrTouchPointX[destIdFixed][0];
        float *pDestY = &s_fArrArrTouchPointY[destIdFixed][0];
        float *pDestX0 = &s_fArrArrTouchPointX0[destIdFixed][0];
        float *pDestY0 = &s_fArrArrTouchPointY0[destIdFixed][0];

        for( int i=num/2-1; i>=0; i-- ){
            int at = num - (i+1);
            
            float temp = pDestX[i];
            pDestX[i] = pDestX[at];
            pDestX[at] = temp;

            temp = pDestY[i];
            pDestY[i] = pDestY[at];
            pDestY[at] = temp;

            temp = pDestX0[i];
            pDestX0[i] = pDestX0[at];
            pDestX0[at] = temp;

            temp = pDestY0[i];
            pDestY0[i] = pDestY0[at];
            pDestY0[at] = temp;
        }
    }
    
    // オプションの追加
    if( numOpt > 0 ){
        float *pDestX = &s_fArrArrTouchPointX[destIdFixed][num];
        float *pDestY = &s_fArrArrTouchPointY[destIdFixed][num];
        float *pDestX0 = &s_fArrArrTouchPointX0[destIdFixed][num];
        float *pDestY0 = &s_fArrArrTouchPointY0[destIdFixed][num];

        float *pOptX = &s_fArrArrTouchPointX[optIdFixed][0];
        float *pOptY = &s_fArrArrTouchPointY[optIdFixed][0];
        float *pOptX0 = &s_fArrArrTouchPointX0[optIdFixed][0];
        float *pOptY0 = &s_fArrArrTouchPointY0[optIdFixed][0];

        if( isOptMinus ){
            for( int i=0; i<numOpt; i++ ){
                int at = numOpt - (i+1);

                pDestX[i] = pOptX[at];
                pDestY[i] = pOptY[at];
                pDestX0[i] = pOptX0[at];
                pDestY0[i] = pOptY0[at];
            }
        }else{
            memcpy( pDestX, pOptX, numOpt*sizeof(float) );
            memcpy( pDestY, pOptY, numOpt*sizeof(float) );
            memcpy( pDestX0, pOptX0, numOpt*sizeof(float) );
            memcpy( pDestY0, pOptY0, numOpt*sizeof(float) );
        }
        
        num += numOpt;
    }

    // 登録数の更新
    s_nArrTouchPointNum[destIdFixed] = num;
    return( true );
}
