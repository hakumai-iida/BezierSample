/*+----------------------------------------------------------------+
  |	Title:		BezierStructConv.cpp [共通環境]
  |	Comment:	ベジェ関連構造体変換
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BezierStructConv.hpp"
#include "draw/bezier/adjust/AdjustCalc.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 調整値準備
#define READY_FOR_TEMP_ADJUST                                                                                   \
    int tempRateOfsX = pData->getTempAdjustRateOfsX();                                                          \
    int tempRateOfsY = pData->getTempAdjustRateOfsY();                                                          \
    int tempRateScaleX = pData->getTempAdjustRateScaleX();                                                      \
    int tempRateScaleY = pData->getTempAdjustRateScaleY();                                                      \
    float tempOfsX = pLayerParam->pixelScaleX*CConst::ConvBezierPositionRate( w, tempRateOfsX );                \
    float tempOfsY = pLayerParam->pixelScaleY*CConst::ConvBezierPositionRate( h, tempRateOfsY );                \
    float tempScaleX = pLayerParam->pixelScaleX*CConst::ConvBezierScaleRate( tempRateScaleX );                  \
    float tempScaleY = pLayerParam->pixelScaleY*CConst::ConvBezierScaleRate( tempRateScaleY );                  \
    float tempRot = pLayerParam->pixelRot + ((isMC)? -pLayerParam->pixelRotWithT: pLayerParam->pixelRotWithT);  \
    tempOfsX += pLayerParam->pixelScaleX*CConst::ConvBezierPositionRate( w, pLayerParam->tempRateOfsX );        \
    if( isMC ){ tempOfsX *= -1; }                                                                               \
    tempOfsY += pLayerParam->pixelScaleY*CConst::ConvBezierPositionRate( h, pLayerParam->tempRateOfsY );        \
    tempScaleX *= CConst::ConvBezierScaleRate( pLayerParam->tempRateScaleX );                                   \
    tempScaleY *= CConst::ConvBezierScaleRate( pLayerParam->tempRateScaleY )

// 調整座標設定：スケール適用
#define ADJUST_XY_WITH_SCALE( _x, _y )                              \
    (_x) = (_x)*tempScaleX;                                         \
    (_y) = (_y)*tempScaleY;                                         \
    if( tempRot != 0.0f ){                                          \
        float __x = CMath::RotX( (_x), (_y), tempRot );             \
        float __y = CMath::RotY( (_x), (_y), tempRot );             \
        (_x) = __x;                                                 \
        (_y) = __y;                                                 \
    }

// 調整座標設定：スケール適用＆オフセット
#define ADJUST_XY_WITH_SCALE_AND_OFFSET( _x, _y, _adjX, _adjY  )    \
    (_x) = (_x)*tempScaleX + tempOfsX;                              \
    (_y) = (_y)*tempScaleY + tempOfsY;                              \
    if( tempRot != 0.0f ){                                          \
        float __x = CMath::RotX( (_x), (_y), tempRot );             \
        float __y = CMath::RotY( (_x), (_y), tempRot );             \
        (_x) = __x;                                                 \
        (_y) = __y;                                                 \
    }                                                               \
    (_x) += pLayerParam->pixelOfsX + (_adjX);                       \
    (_y) += pLayerParam->pixelOfsY + (_adjY)


/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
#ifdef ENABLE_FIXED_BRUSH
eBRUSH CBezierStructConv::s_eFixedBrush = eBRUSH_INVALID;
#endif

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------------------------
// アンカーポイントの作成（※内部でメモリを確保する）
//------------------------------------------
stBEZIER_ANCHOR_POINT* CBezierStructConv::CreateBezierAnchorPoint( CLineObjectData* pData,
                                                                   stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam,
                                                                   bool isMC ){
    // 用心
    if( pData == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierAnchorPoint: INVALID DATA pData=%p\n", pData );
        return( NULL );
    }
    
    // データ取り出し
    CAnchorPointData* pHead = pData->getDataHead();
    
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierAnchorPoint: INVALID HEAD: pHead=%p\n", pHead );
        return( NULL );
    }
    
    // work
    CAnchorPointData* pAPD = pHead;
    stBEZIER_ANCHOR_POINT* pBAP;
    int w = pBaseParam->texW0;          // 座標変換は名目サイズで行う
    int h = pBaseParam->texH0;          // 座標変換は名目サイズで行う
    
    //------------------------------
    // 要素数を調べる
    //------------------------------
    int numPoint = 0;
    while( pAPD != NULL ){
        // 有効であれば
        if( ! pAPD->checkFlag( eAPD_FLAG_DISABLE ) ){
            numPoint++;
        }
        
        pAPD = pAPD->getNext();
    }
    
    // 用心
    if( numPoint <= 0 ){
        LOGE( "@ CBezierStructConv::CreateBezierAnchorPoint: NO POINT pHead=%p\n", pHead );
        return( NULL );
    }

    //---------------------------------------------------
    // 領域確保（※ワークなので[APP]から確保＆簡便のため配列で確保）
    //---------------------------------------------------
    stBEZIER_ANCHOR_POINT* pBAPArr = NULL;
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    pBAPArr = new stBEZIER_ANCHOR_POINT[numPoint];
    CMemMgr::PopTargetField();
    
    //------------------------------
    // ベジェ描画データへ変換
    //------------------------------
    int at = 0;
    pAPD = pHead;
    while( pAPD != NULL ){
        // 有効であれば
        if( ! pAPD->checkFlag( eAPD_FLAG_DISABLE ) ){
            pBAP = &pBAPArr[at++];
            
            // ポイント、方向線（進入）、方向線（出立）の取得
            CAdjustablePoint* pXY = pAPD->getAdjXY();
            CAdjustablePoint* pInXY = pAPD->getAdjInXY();
            CAdjustablePoint* pOutXY = pAPD->getAdjOutXY();
            
            // 座標、方向線（進入）、方向線（出立）の算出
            int rX = pXY->getRateX();
            int rY = pXY->getRateY();
            CAdjustCalc::CalcRateXYForHV( &rX, &rY, rX, rY, pXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
            if( isMC ){ rX *= -1; }    // HV影響の後に反転（※遅延よりも前）

            int inRX = pInXY->getRateX();
            int inRY = pInXY->getRateY();
            CAdjustCalc::CalcRateXYForHV( &inRX, &inRY, inRX, inRY, pInXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
            if( isMC ){ inRX *= -1; }    // HV影響の後に反転（※遅延よりも前）

            int outRX = pOutXY->getRateX();
            int outRY = pOutXY->getRateY();
            CAdjustCalc::CalcRateXYForHV( &outRX, &outRY, outRX, outRY, pOutXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
            if( isMC ){ outRX *= -1; }    // HV影響の後に反転（※遅延よりも前）
            
            //-----------------------------------------------------
            // 結合ポイントの指定が有効なら検出
            //-----------------------------------------------------
            int systemHookTargetId = -1;
            int jointPointIdFixed = pAPD->fixJointPointId( pLayerParam->slotIndex, isMC );
            if( IS_JOINT_POINT_FIXED_VALID( jointPointIdFixed ) && pBaseParam->pJointPoint != NULL ){
                // 元の座標の退避
                int tempX = rX;
                int tempY = rY;

                // 登録が有効なら
                CJointPoint* pJP = pBaseParam->pJointPoint;
                stJOINT_POINT_CELL* pCell = pJP->getJointPoint( jointPointIdFixed );
                if( pCell != NULL && pCell->bValid ){
                    // 指定があれば参照のみ
                    if( pAPD->checkFlag( eAPD_FLAG_REF_JOINT_POINT ) ){
                        tempX = pCell->nRateX;
                        tempY = pCell->nRateY;
                    }
                    // ここまで来たら結合処理（※二点が重なる前提で方向線の調整）
                    else{
                        // 方向線の適用方向
                        int valForFlipH = 1;
                        int valForFlipV = 1;

                        // 接続先[TO]の調整
                        //（※回転処理が重複されないように回転前の値を利用＝若干妙な感じはするが管理上いたしかたなし）
                        if( pCell->bTo ){
                            // 接続元情報の取得（※接続先から[JOINT_POINT_OFS_FOR_FROM_TO]を引いた値で取得できる）
                            stJOINT_POINT_CELL* pCellFrom = pJP->getJointPoint( jointPointIdFixed - JOINT_POINT_OFS_FOR_FROM_TO );
                            
                            // 接続元が有効であれば
                            if( pCellFrom != NULL && pCellFrom->bValid ){
                                // 重なりの検出
                                bool isOverwrapped = false;
                                bool isOdd = false;
                                float rotFrom = pCellFrom->fRot0;
                                
                                // [奇数]側であれば
                                if( ((jointPointIdFixed/JOINT_POINT_OFS_FOR_OTHER_SIDE)%2) != 0 ){
                                    isOdd = true;

                                    // 接続元の角度が大きい場合は重なっている
                                    isOverwrapped = (pCell->fRot0 < rotFrom);
                                }
                                // [偶数]側であれば
                                else{
                                    // 接続元の角度が小さい場合は重なっている
                                    isOverwrapped = (pCell->fRot0 > rotFrom);
                                }

                                // 上下／左右の反転による判定の調整
                                if( (pLayerParam->pixelScaleX<0) != (pLayerParam->pixelScaleY<0) ){
                                    isOverwrapped = !isOverwrapped;
                                }

                                // ここまできたら調整値を設定
                                tempX = pCell->nRateX;
                                tempY = pCell->nRateY;
                                
                                // 方向線の反転（※想定の逆順でAPが配置されていた場合は反転させる）
                                if( pAPD->checkFlag( eAPD_FLAG_REVERSE_JOINT_DIR ) ){
                                    isOdd = !isOdd;
                                    valForFlipH *= -1;
                                    valForFlipV *= -1;
                                }
                                
                                // 方向線の調整（※線のつなぎ目を自然にする）
                                if( isOdd ){
                                    if( isOverwrapped ){
                                        inRX -= pBaseParam->texW0 * pCell->closeDirX * valForFlipH;
                                        inRY -= pBaseParam->texH0 * pCell->closeDirY * valForFlipV;
                                    }else{
                                        inRX += pBaseParam->texW0 * pCell->openDirX * valForFlipH;
                                        inRY += pBaseParam->texH0 * pCell->openDirY * valForFlipV;
                                    }
                                }
                                // [偶数]側であれば
                                else{
                                    if( isOverwrapped ){
                                        outRX += pBaseParam->texW0 * pCell->closeDirX * valForFlipH;
                                        outRY += pBaseParam->texH0 * pCell->closeDirY * valForFlipV;
                                    }else{
                                        outRX -= pBaseParam->texW0 * pCell->openDirX * valForFlipH;
                                        outRY -= pBaseParam->texH0 * pCell->openDirY * valForFlipV;
                                    }
                                }
                            }
                        }
                        // 接続元[FROM]の調整
                        else{
                            // 呼び出し先の角度を取得（※接続元から[JOINT_POINT_OFS_FOR_FROM_TO]を足した値で取得できる）
                            stJOINT_POINT_CELL* pCellTo = pJP->getJointPoint( jointPointIdFixed + JOINT_POINT_OFS_FOR_FROM_TO );

                            // 接続先が有効であれば
                            if( pCellTo != NULL && pCellTo->bValid ){
                                bool isOdd = false;
                                bool isOverwrapped = false;
                                float rotTo = pCellTo->fRot0;
                                
                                // [奇数]側であれば
                                if( ((jointPointIdFixed/JOINT_POINT_OFS_FOR_OTHER_SIDE)%2) != 0 ){
                                    isOdd = true;
                                    
                                    // 接続先の角度が小さい場合は重なっている
                                    isOverwrapped = (pCell->fRot0 > rotTo);
                                }
                                // [偶数]側であれば
                                else{
                                    // 接続先の角度が大きい場合は重なっている
                                    isOverwrapped = (pCell->fRot0 < rotTo);
                                }
                                
                                // 上下／左右の反転による判定の調整
                                if( (pLayerParam->pixelScaleX<0) != (pLayerParam->pixelScaleY<0) ){
                                    isOverwrapped = !isOverwrapped;
                                }

                                // ここまできたら接続元の値を設定
                                tempX = pCell->nRateX;
                                tempY = pCell->nRateY;

                                // 方向線の反転（※想定の逆順でAPが配置されていた場合は反転させる）
                                if( pAPD->checkFlag( eAPD_FLAG_REVERSE_JOINT_DIR ) ){
                                    isOdd = !isOdd;
                                    valForFlipH *= -1;
                                    valForFlipV *= -1;
                                }
                                
                                // 方向線の調整（※線のつなぎ目を自然にする）
                                if( isOdd ){
                                    if( isOverwrapped ){
                                        outRX -= pBaseParam->texW0 * pCell->closeDirX * valForFlipH;
                                        outRY -= pBaseParam->texH0 * pCell->closeDirY * valForFlipV;
                                    }else{
                                        outRX += pBaseParam->texW0 * pCell->openDirX * valForFlipH;
                                        outRY += pBaseParam->texH0 * pCell->openDirY * valForFlipV;
                                    }
                                }
                                // [偶数]側であれば
                                else{
                                    if( isOverwrapped ){
                                        inRX += pBaseParam->texW0 * pCell->closeDirX * valForFlipH;
                                        inRY += pBaseParam->texH0 * pCell->closeDirY * valForFlipV;
                                    }else{
                                        inRX -= pBaseParam->texW0 * pCell->openDirX * valForFlipH;
                                        inRY -= pBaseParam->texH0 * pCell->openDirY * valForFlipV;
                                    }
                                }
                            }
                        }
                    }
                        
                    // サイズ調整（※ジョイントのサイズで計算された登録座標を、接続元／先のサイズへ合わせる）
                    float scale = pCell->fScaleAbs / pLayerParam->scaleForBdpd;
                    tempX = (int)(scale * tempX);
                    tempY = (int)(scale * tempY);
                    
                    // 符号が一致しなければ反転
                    bool isFlipH = pLayerParam->pixelScaleX < 0.0f;
                    if( pCell->bFlipH != isFlipH ){
                        tempX *= -1;
                    }
                    
                    bool isFlipV = pLayerParam->pixelScaleY < 0.0f;
                    if( pCell->bFlipV != isFlipV ){
                        tempY *= -1;
                    }

                    // 原点調整：[TO：呼び出された側]
                    if( pCell->bTo ){
                        // ジョイントを介さずに直接対象パーツを呼ぶことにしたのでオフセットは処理済み
                    }
                    // 原点調整：[FROM：呼び出した側]
                    else{
                        // 呼び出し側から見たオフセットを足しこむ（※呼びだし元がジョイントスロットに足し込んだオフセット）
                        tempX += pCell->nRateOfsX;
                        tempY += pCell->nRateOfsY;
                    }
                }
                             
                // 座標の出力
                rX = tempX;
                rY = tempY;
                
                // 参照指定であれば特殊なフックを指定（※オフセット処理のみ反映させたい）
                if( pAPD->checkFlag( eAPD_FLAG_REF_JOINT_POINT ) ){
                    systemHookTargetId = BEZIER_HOOK_POINT_ALREADY_HOOKED;
                }else{
                    // システムフックの予約（※[FROM]枠でフックする）
                    systemHookTargetId = eSTROKE_HOOK_TARGET_TOTAL_MAX; // システムフックは通常フックの末尾に確保されている
                    systemHookTargetId += (jointPointIdFixed/JOINT_POINT_OFS_FOR_OTHER_SIDE)*JOINT_POINT_OFS_FOR_OTHER_SIDE;
                }
            }

            //-----------------------------------------------------
            // 遅延ログによる調整
            //-----------------------------------------------------
            bool isReverseH = pLayerParam->pixelScaleX < 0;
            bool isReverseV = pLayerParam->pixelScaleY < 0;
            if( pBaseParam->pDelayLog != NULL ){
                CAdjustCalc::CalcRateXYForDelayLog( &rX, &rY, rX, rY, isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pXY, pBaseParam->pDelayLog );
                CAdjustCalc::CalcRateXYForDelayLog( &inRX, &inRY, inRX, inRY, isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pInXY, pBaseParam->pDelayLog );
                CAdjustCalc::CalcRateXYForDelayLog( &outRX, &outRY, outRX, outRY, isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pOutXY, pBaseParam->pDelayLog );
            }
            
            
            //-----------------------------------------------------
            // 描画用アンカーポイントへデータの設定
            //-----------------------------------------------------
            // 座標設定
            pBAP->x = CConst::ConvBezierPositionRate( w, rX );
            pBAP->y = CConst::ConvBezierPositionRate( h, rY );
            pBAP->xIn = CConst::ConvBezierPositionRate( w, inRX );
            pBAP->yIn = CConst::ConvBezierPositionRate( h, inRY );
            pBAP->xOut = CConst::ConvBezierPositionRate( w, outRX );
            pBAP->yOut = CConst::ConvBezierPositionRate( h, outRY );
            
            // 指定があればオフセット設定
            if( pAPD->checkFlag( eAPD_FLAG_HOOK_WITH_OFS ) ){
                // 体型調整
                rX = pXY->getRateOfsX();
                rY = pXY->getRateOfsY();
                CAdjustCalc::CalcRateXYForHV( &rX, &rY, rX, rY, pXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
                if( isMC ){ rX *= -1; }
                
                // 遅延調整
                CAdjustCalc::CalcRateXYForDelayLog( &rX, &rY, rX, rY, isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pXY, pBaseParam->pDelayLog );

                // 割り当て
                pBAP->isOfsValid = true;
                pBAP->ofsX = CConst::ConvBezierPositionRate( w, rX );
                pBAP->ofsY = CConst::ConvBezierPositionRate( h, rY );
            }
            
            // ストロークスケール（※ストロークの太さは基本情報とレイヤー情報の２つから補正される＆ピクセル率も反映）
            float strokeScale = pBaseParam->strokeScale * pLayerParam->strokeScale * pBaseParam->pixelRate;

            // ブラシ設定（※解像度の影響はここで加味する＆透明であれば無効）
            CBrushBinData* pBin = NULL;
            if( ! pAPD->checkFlag(eAPD_FLAG_TRANSPARENCY) ){
                eBRUSH brushId = pData->getBrushId();
                if( IS_BRUSH_VALID( brushId ) ){
#ifdef ENABLE_FIXED_BRUSH
                    // 基本ブラシの場合、指定があれば強制ブラシで差し替え
                    if( brushId == eBRUSH_C_BASE && IS_BRUSH_VALID( s_eFixedBrush ) ){
                        brushId = (eBRUSH)(eBRUSH_C_BASE + (s_eFixedBrush-eBRUSH_C_BASE));
                    }
                    
                    // タッチブラシの場合、指定があれば強制ブラシで差し替え
                    if( brushId == eBRUSH_C_TOUCH && IS_BRUSH_VALID( s_eFixedBrush ) ){
                        brushId = (eBRUSH)(eBRUSH_C_TOUCH + (s_eFixedBrush-eBRUSH_C_BASE));
                    }
                    
                    // フリルブラシの場合、指定があれば強制ブラシで差し替え
                    if( brushId == eBRUSH_C_FRILL && IS_BRUSH_VALID( s_eFixedBrush ) ){
                        brushId = (eBRUSH)(eBRUSH_C_FRILL + (s_eFixedBrush-eBRUSH_C_BASE));
                    }
#endif
                    CDefBinTable<CBrushBinData>* pTable = CDefTable::GetBrushTable();
                    pBin = pTable->getDataAt( brushId );
                }else if( brushId != eBRUSH_INVALID ){
                    LOGE( "@ BezierDatConv: UNKNOWN brushId=%d\n", brushId );
                }
            }
            
            // ブラシが有効であれば設定
            if( pBin != NULL ){
                pBAP->type = pBin->getStrokeType();
                pBAP->size = pBin->getStrokeSize() * strokeScale;
                pBAP->ofsRange = pBin->getStrokeRange() * strokeScale;
                               
                pBAP->dotPutMax = pBin->getDotPutMax();
                pBAP->dotPutRateBase = pBin->getDotPutRateBase();
                pBAP->dotPutRateStrong = pBin->getDotPutRateStrong();
                pBAP->dotPutRateWeak = pBin->getDotPutRateWeak();
                pBAP->isDotPutForce = pBin->isDotPutForce();
                pBAP->isDotPutWhetherMax = pBin->isDotPutWhetherMax();
                pBAP->isDotPutOnOutCol = pAPD->checkFlag( eAPD_FLAG_DOT_ON_OUT_COL );
                pBAP->isDotErase = pAPD->checkFlag( eAPD_FLAG_DOT_ERASE );
                
                // 線消去時は必ず出力されるようにフラグの調整
                if( pBAP->isDotErase ){
                    pBAP->isDotPutWhetherMax = true;
                }
                
                pBAP->shakeScaleForPos = pBin->getShakePosScale();
                pBAP->shakeScaleForSize = pBin->getShakeSizeScale();

            }
            // 無効であればオフ（非表示）
            else{
                pBAP->type = eSTROKE_TYPE_OFF;
            }
            
            // フラグ
            pBAP->bNoFillGuide = pAPD->checkFlag( eAPD_FLAG_NO_FILL_GUIDE );
            pBAP->bLineRepairTest = pAPD->checkFlag( eAPD_FLAG_TEST_LINE_REPAIR );
            pBAP->bAsTouchStroke = pAPD->checkFlag( eAPD_FLAG_AS_TOUCH_STROKE );
                        
            // ガイド設定
            pBAP->guideTargetIdFixed = pAPD->fixGuideTargetId( pLayerParam->slotIndex, isMC );

            // フック設定
            pBAP->hookTargetIdFixed = pAPD->fixHookTargetId( pLayerParam->slot, pLayerParam->slotIndex, isMC );
            pBAP->reHookTargetIdFixed = pAPD->fixReHookTargetId( pLayerParam->slot, pLayerParam->slotIndex, isMC );
            pBAP->systemHookTargetIdFixed = systemHookTargetId;
            pBAP->isRefJoint = pAPD->checkFlag( eAPD_FLAG_REF_JOINT_POINT );
            pBAP->isJointed = (pAPD->getJointPointId() != eJOINT_POINT_INVALID);
            
            // ストロークサイズ設定
            pBAP->strokeSize = CConst::ConvBezierScaleRate( pData->getStrokeSize() );
            pBAP->strokeStartRange = CConst::ConvBezierScaleRate( pAPD->getStrokeStartRange() );
            pBAP->strokeStartSize = CConst::ConvBezierScaleRate( pAPD->getStrokeStartSize() );
            pBAP->strokeEndRange = CConst::ConvBezierScaleRate( pAPD->getStrokeEndRange() );
            pBAP->strokeEndSize = CConst::ConvBezierScaleRate( pAPD->getStrokeEndSize() );
            pBAP->strokeEdgeFillSize = CConst::ConvBezierScaleRate( pAPD->getStrokeEdgeFillSize() );

            // タッチ設定（※ステップの値へのストローク影響はここで済ませておく）
            pBAP->touchTargetIdFixed = pAPD->fixTouchTargetId( pLayerParam->slotIndex, isMC );
            pBAP->touchTargetSubIdFixed = pAPD->fixTouchTargetSubId( pLayerParam->slotIndex, isMC );
            pBAP->touchBasePos = CConst::ConvBezierScaleRate( pAPD->getTouchBasePos() );
            pBAP->touchBaseSize = CConst::ConvBezierScaleRate( pAPD->getTouchBaseSize() );
            pBAP->touchBaseRot = CConst::ConvBezierRotationRate( pAPD->getTouchBaseRot() );
            pBAP->touchFrontArea = CConst::ConvBezierScaleRate( pAPD->getTouchFrontArea() );
            pBAP->touchFrontSizeOfs = CConst::ConvBezierScaleRate( pAPD->getTouchFrontSizeOfs() );
            pBAP->touchFrontRotOfs = CConst::ConvBezierRotationRate( pAPD->getTouchFrontRotOfs() );
            pBAP->touchBackArea = CConst::ConvBezierScaleRate( pAPD->getTouchBackArea() );
            pBAP->touchBackSizeOfs = CConst::ConvBezierScaleRate( pAPD->getTouchBackSizeOfs() );
            pBAP->touchBackRotOfs = CConst::ConvBezierRotationRate( pAPD->getTouchBackRotOfs() );
            pBAP->touchFrontNum = pAPD->getTouchFrontNum();
            pBAP->touchFrontSkip = pAPD->getTouchFrontSkip();
            pBAP->touchFrontBorderRate = CConst::ConvBezierScaleRate( pAPD->getTouchFrontBorderRate() );
            pBAP->touchBackNum =  pAPD->getTouchBackNum();
            pBAP->touchBackSkip = pAPD->getTouchBackSkip();
            pBAP->touchBackBorderRate = CConst::ConvBezierScaleRate( pAPD->getTouchBackBorderRate() );
            pBAP->touchRandomOfsFor0 = CConst::ConvBezierScaleRate( pAPD->getTouchRandomOfsFor0() );
            pBAP->touchRandomOfsFor1 = CConst::ConvBezierScaleRate( pAPD->getTouchRandomOfsFor1() );
            pBAP->touchHeadSkipRate = CConst::ConvBezierScaleRate( pAPD->getTouchHeadSkipRate() );
            pBAP->touchTailSkipRate = CConst::ConvBezierScaleRate( pAPD->getTouchTailSkipRate() );
            pBAP->touchRefOfsRateForMain = CConst::ConvBezierScaleRate( pAPD->getTouchRefOfsRateForMain() );
            pBAP->touchRefOfsRateForSub = CConst::ConvBezierScaleRate( pAPD->getTouchRefOfsRateForSub() );
            pBAP->frillWidthRateForMain = CConst::ConvBezierScaleRate( pAPD->getFrillWidthRateForMain() );
            pBAP->frillWidthRateForSub = CConst::ConvBezierScaleRate( pAPD->getFrillWidthRateForSub() );
            
            pBAP->isTouchReverse = isMC;
            pBAP->isFrillReverseHook = pAPD->checkFlag( eAPD_FLAG_REVERSE_FRILL_HOOK );
            pBAP->isTouchSkipFront = isMC && pAPD->checkFlag( eAPD_FLAG_TOUCH_SKIP_FRONT_IF_MC );
            pBAP->isTouchSkipBack = isMC && pAPD->checkFlag( eAPD_FLAG_TOUCH_SKIP_BACK_IF_MC );

            // ストライプパレットグループ
            pBAP->stripeOrFrillMainPalGrp = pAPD->getStripeOrFrillMainPalGrp();
            pBAP->stripeOrFrillSubPalGrp = pAPD->getStripeOrFrillSubPalGrp();

            // フリル設定
            pBAP->frillMainId = pAPD->getFrillMainId();
            pBAP->frillSubId = pAPD->getFrillSubId();

            // 傾け：全体
            if( pLayerParam->parentSlot != eBD_SLOT_INVALID && pLayerParam->parentSlotIndex >= 0 ){
                pBAP->angleSearchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( pLayerParam->parentSlot, pLayerParam->parentSlotIndex );
            }else{
                pBAP->angleSearchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( pLayerParam->slot, pLayerParam->slotIndex );
            }
            
            // ワークパス用の色設定
            if( CBezier::IsDispAllSelected() ){
                pBAP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pAPD->checkFlag( eAPD_FLAG_TEMP_SELECTED ) && pData->checkFlag( eLOD_FLAG_TEMP_SELECTED ) ){
                pBAP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pAPD->checkFlag( eAPD_FLAG_TEMP_ACTIVE ) && pData->checkFlag( eLOD_FLAG_TEMP_ACTIVE ) ){
                pBAP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_ACTIVE;
            }else{
                pBAP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SLEEP;
            }
        }

        // 次のポイントへ
        pAPD = pAPD->getNext();
    }
    
    //------------------------------
    // ミラーコピーの先頭／末尾フック調整
    //------------------------------
    // ヘッド
    if( pData->checkFlag( eLOD_FLAG_MIRROR_COPY_HOOK_HEAD ) ){
        pBAP = &pBAPArr[0];
        // 無効であれば上書き（※元々のフックが有効ならそちらの挙動に準拠する）
        if( pBAP->hookTargetIdFixed == -1 ){
            pBAP->hookTargetIdFixed = eSTROKE_HOOK_TARGET_MC_HEAD;
        }
    }

    // テール
    if( pData->checkFlag( eLOD_FLAG_MIRROR_COPY_HOOK_TAIL ) ){
        pBAP = &pBAPArr[numPoint-1];
        // 無効であれば上書き（※元々のフックが有効ならそちらの挙動に準拠する）
        if( pBAP->hookTargetIdFixed == -1 ){
            pBAP->hookTargetIdFixed = eSTROKE_HOOK_TARGET_MC_TAIL;
        }
    }
    
    //------------------------------
    // リスト構築
    //------------------------------
    for( int i=1; i<numPoint; i++ ){
        pBAPArr[i].pPrev = &pBAPArr[i-1];
        pBAPArr[i-1].pNext = &pBAPArr[i];
    }
    
    // クローズパスであれば閉じる
    if( numPoint > 1 && pData->checkFlag( eLOD_FLAG_CLOSE_PATH ) ){
        pBAPArr[0].pPrev = &pBAPArr[numPoint-1];
        pBAPArr[numPoint-1].pNext = &pBAPArr[0];
    }
        
    //----------------------------------------------------
    // 座標調整（※原点調整／オフセット＆拡縮適用）
    //----------------------------------------------------
    READY_FOR_TEMP_ADJUST;
    
    // この時点で半ドット分ずらしておく（※ドットの描画が座標の中心にくるように）
    for( int i=0; i<numPoint; i++ ){
        pBAP = &pBAPArr[i];

        ADJUST_XY_WITH_SCALE_AND_OFFSET( pBAP->x, pBAP->y, -0.5f, -0.5f );
        ADJUST_XY_WITH_SCALE( pBAP->ofsX, pBAP->ofsY );
        ADJUST_XY_WITH_SCALE( pBAP->xIn, pBAP->yIn );
        ADJUST_XY_WITH_SCALE( pBAP->xOut, pBAP->yOut );
    }
    
    // ヘッダを返す
    return( &pBAPArr[0] );
}

//----------------------------------------------------------------------
// アンカーポイントの解放（※引数は[CreateBezierAnchorPoint]で確保した領域である想定）
//----------------------------------------------------------------------
void CBezierStructConv::ReleaseBezierAnchorPoint( stBEZIER_ANCHOR_POINT* pHead ){
    SAFE_DELETE_ARRAY( pHead );
}

//------------------------------------------------------------------------------------------
// 塗りポイントの作成（※内部でメモリを確保する）
// [CFillPointData:割合座標＆中心原点]から[stBEZIER_FILL_POINT:実座標＆左上原点]に変換したデータを返す
//------------------------------------------------------------------------------------------
stBEZIER_FILL_POINT* CBezierStructConv::CreateBezierFillPoint( CPaintObjectData* pData,
                                                               stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam,
                                                               bool isMC ){
    // 用心
    if( pData == NULL || pBaseParam == NULL || pLayerParam == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierFillPoint: INVALID PARAM pData=%p, pBaseParam=%p, pLayerParam=%p\n",
              pData, pBaseParam, pLayerParam );
        return( NULL );
    }
    
    // データ取り出し
    CFillPointData* pHead = pData->getDataHead();
    
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierFillPoint: INVALID HEAD pHead=%p\n", pHead );
        return( NULL );
    }
    
    // work
    CFillPointData* pFPD = pHead;
    stBEZIER_FILL_POINT* pBFP;
    int w = pBaseParam->texW0;          // 座標変換は名目サイズで行う
    int h = pBaseParam->texH0;          // 座標変換は名目サイズで行う
    
    //------------------------------
    // 要素数を調べる
    //------------------------------
    int numPoint = 0;
    while( pFPD != NULL ){
        if( ! pFPD->checkFlag( eFPD_FLAG_DISABLE ) ){
            numPoint++;
        }
        
        pFPD = pFPD->getNext();
    }
    
    // 用心
    if( numPoint <= 0 ){
        LOGE( "@ CBezierStructConv::CreateBezierFillPoint: NO POINT pHead=%p\n", pHead );
        return( NULL );
    }
    
    //---------------------------------------------------
    // 領域確保（※ワークなので[APP]から確保＆簡便のため配列で確保）
    //---------------------------------------------------
    stBEZIER_FILL_POINT* pBFPArr = NULL;
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    pBFPArr = new stBEZIER_FILL_POINT[numPoint];
    CMemMgr::PopTargetField();
    
    //------------------------------
    // ベジェ描画データへ変換
    //------------------------------
    int at = 0;
    pFPD = pHead;
    while( pFPD != NULL ){
        if( ! pFPD->checkFlag( eFPD_FLAG_DISABLE ) ){
            pBFP = &pBFPArr[at++];
            
            CAdjustablePoint* pXY = pFPD->getAdjXY();
            
            // 座標の算出
            int rX = pXY->getRateX();
            int rY = pXY->getRateY();
            CAdjustCalc::CalcRateXYForHV( &rX, &rY, rX, rY, pXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
            if( isMC ){ rX *= -1; }    // HV影響の後に反転（※遅延よりも前）

            // 遅延ログによる調整
            bool isReverseH = pLayerParam->pixelScaleX < 0;
            bool isReverseV = pLayerParam->pixelScaleY < 0;
            if( pBaseParam->pDelayLog != NULL ){
                CAdjustCalc::CalcRateXYForDelayLog( &rX, &rY, rX, rY,
                                                    isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pXY, pBaseParam->pDelayLog );
            }

            // 座標設定
            pBFP->x = CConst::ConvBezierPositionRate( w, rX );
            pBFP->y = CConst::ConvBezierPositionRate( h, rY );

            // 指定があればオフセット設定
            if( pFPD->checkFlag( eFPD_FLAG_HOOK_WITH_OFS ) ){
                // 体型調整
                rX = pXY->getRateOfsX();
                rY = pXY->getRateOfsY();
                CAdjustCalc::CalcRateXYForHV( &rX, &rY, rX, rY, pXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
                if( isMC ){ rX *= -1; }
                
                // 遅延調整
                CAdjustCalc::CalcRateXYForDelayLog( &rX, &rY, rX, rY, isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pXY, pBaseParam->pDelayLog );

                // 割り当て
                pBFP->isOfsValid = true;
                pBFP->ofsX = CConst::ConvBezierPositionRate( w, rX );
                pBFP->ofsY = CConst::ConvBezierPositionRate( h, rY );
            }
            
            // フック設定
            pBFP->hookTargetIdFixed = pFPD->fixHookTargetId( pLayerParam->slot, pLayerParam->slotIndex, isMC );
            
            // 塗り情報
            CBucketBinData* pBin = NULL;
            if( ! pFPD->checkFlag(eFPD_FLAG_TRANSPARENCY) ){
                eBUCKET bucketId = pData->getBucketId();
                if( IS_BUCKET_VALID( bucketId ) ){
                    CDefBinTable<CBucketBinData>* pTable = CDefTable::GetBucketTable();
                    pBin = pTable->getDataAt( bucketId );
                }else if( bucketId != eBUCKET_INVALID ){
                    LOGE( "@ BezierDatConv: UNKNOWN bucketId=%d\n", bucketId );
                }
            }

            if( pBin != NULL ){
                pBFP->type = pBin->getFillType();
            }else{
                pBFP->type = eFILL_TYPE_OFF;
            }

            pBFP->palGrp = pData->getPalGrp();
            pBFP->darkOfs = pData->getDarkOfs();
            pBFP->isFillOnOutCol = pFPD->checkFlag( eFPD_FLAG_FILL_ON_OUT_COL );
            
            // ストロークスケール（※ストロークの太さは基本情報とレイヤー情報の２つから補正される＆ピクセル率も反映）
            float strokeScale = pBaseParam->strokeScale * pLayerParam->strokeScale * pBaseParam->pixelRate;

            // 塗りオプション
            pBFP->option = pFPD->getFillOption();
            pBFP->optionDarkOfs = pFPD->getDarkOfsForOption();
            pBFP->optionTouchIdFixed = pFPD->fixTouchTargetIdForOption( pLayerParam->slotIndex, isMC );
            pBFP->optionBitMask = pFPD->getBitMaskForOption( pLayerParam->slot0, pLayerParam->slotIndex );  // ここは本来のスロットで判定する
            pBFP->optionStrokeSize = pFPD->getStrokeSizeForOption() * strokeScale;
            pBFP->optionStrokeRange = pFPD->getStrokeRangeForOption() * strokeScale;
            pBFP->optionOfsX = pFPD->getOfsXForOption() * strokeScale;
            if( isMC != (pLayerParam->pixelScaleX<0.0f) ){ pBFP->optionOfsX *= -1; }
            pBFP->optionOfsY = pFPD->getOfsYForOption() * strokeScale;
            
            // 傾け：全体
            if( pLayerParam->parentSlot != eBD_SLOT_INVALID && pLayerParam->parentSlotIndex >= 0 ){
                pBFP->angleSearchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( pLayerParam->parentSlot, pLayerParam->parentSlotIndex );
            }else{
                pBFP->angleSearchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( pLayerParam->slot, pLayerParam->slotIndex );
            }
            
            // ワークパス用の色設定
            if( CBezier::IsDispAllSelected() ){
                pBFP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pFPD->checkFlag( eFPD_FLAG_TEMP_SELECTED ) ){
                pBFP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pFPD->checkFlag( eFPD_FLAG_TEMP_ACTIVE ) ){
                pBFP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_ACTIVE;
            }else{
                pBFP->workPathPalOfsS = BEZIER_WP_PAL_VAL_FOR_SLEEP;
            }
        }
        
        pFPD = pFPD->getNext();
    }
    
    //------------------------------
    // リスト構築
    //------------------------------
    for( int i=1; i<numPoint; i++ ){
        pBFPArr[i].pPrev = &pBFPArr[i-1];
        pBFPArr[i-1].pNext = &pBFPArr[i];
    }
    
    //----------------------------------------------------
    // 座標調整（※原点調整／オフセット＆拡縮適用）
    //----------------------------------------------------
    READY_FOR_TEMP_ADJUST;
    
    // この時点で半ドット分ずらしておく（※[CAnchorPointData]の処理に合わせる）
    for( int i=0; i<numPoint; i++ ){
        pBFP = &pBFPArr[i];
        
        ADJUST_XY_WITH_SCALE_AND_OFFSET( pBFP->x, pBFP->y, -0.5f, -0.5f );
        ADJUST_XY_WITH_SCALE( pBFP->ofsX, pBFP->ofsY );
    }
    
    // ヘッダを返す
    return( &pBFPArr[0] );
}

//-------------------------------------------------------------------
// 塗りポイントの解放（※引数は[CreateBezierFillPoint]で確保した領域を想定）
//-------------------------------------------------------------------
void CBezierStructConv::ReleaseBezierFillPoint( stBEZIER_FILL_POINT* pHead ){
    SAFE_DELETE_ARRAY( pHead );
}

//------------------------------------------------------------------------------------------
// スロットポイントの作成（※内部でメモリを確保する）
// [CSlotPointData:割合座標＆中心原点]から[stBEZIER_SLOT_POINT:実座標＆左上原点]に変換したデータを返す
//------------------------------------------------------------------------------------------
stBEZIER_SLOT_POINT* CBezierStructConv::CreateBezierSlotPoint( CSlotObjectData* pData,
                                                               stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam,
                                                               bool isMC ){
    // 用心
    if( pData == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierSlotPoint: INVALID CALL pData=%p\n", pData );
        return( NULL );
    }
    
    // データ取り出し
    CSlotPointData* pHead = pData->getDataHead();
    
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezierStructConv::CreateBezierSlotPoint: INVALID DATA pHead=%p\n", pHead );
        return( NULL );
    }
    
    // work
    CSlotPointData* pSPD = pHead;
    stBEZIER_SLOT_POINT* pBSP;
    int w = pBaseParam->texW0;          // 座標変換は名目サイズで行う
    int h = pBaseParam->texH0;          // 座標変換は名目サイズで行う
    
    //------------------------------
    // 要素数を調べる
    //------------------------------
    int numPoint = 0;
    while( pSPD != NULL ){
        // 有効であれば
        if( !pSPD->checkFlag( eSPD_FLAG_DISABLE ) ){
            // ミラーコピー時であればスロット０のみを受け付ける
            if( !isMC || pSPD->getSlotIndex() == 0 ){
                numPoint++;
            }
        }
        
        pSPD = pSPD->getNext();
    }

    // 用心
    if( numPoint <= 0 ){
        LOGE( "@ CBezierStructConv::CreateBezierSlotPoint: NO POINT pHead=%p\n", pHead );
        return( NULL );
    }
    
    //---------------------------------------------------
    // 領域確保（※ワークなので[APP]から確保＆簡便のため配列で確保）
    //---------------------------------------------------
    stBEZIER_SLOT_POINT* pBSPArr = NULL;
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    pBSPArr = new stBEZIER_SLOT_POINT[numPoint];
    CMemMgr::PopTargetField();
    
    //------------------------------
    // ベジェスロットデータへ変換
    //------------------------------
    int at = 0;
    pSPD = pHead;
    while( pSPD != NULL ){
        if( !pSPD->checkFlag( eSPD_FLAG_DISABLE ) && ( !isMC || pSPD->getSlotIndex()==0 ) ){
            pBSP = &pBSPArr[at++];

            // スロット情報（※ミラーコピー時であれば[1]に固定）
            pBSP->slot = pSPD->getSlot();
            pBSP->slotIndex = (isMC)? 1: pSPD->getSlotIndex();

            // 座標の算出
            CAdjustablePoint* pXY = pSPD->getAdjXY();
            int rX = pXY->getRateX();
            int rY = pXY->getRateY();
            CAdjustCalc::CalcRateXYForHV( &rX, &rY, rX, rY, pXY, pLayerParam->fixedAdjustRateH, pLayerParam->fixedAdjustRateV );
            if( isMC ){ rX *= -1; }    // HV影響の後に反転（※遅延よりも前）

            // 遅延ログによる調整（※ここはミラーコピー時もそのまま）
            if( pBaseParam->pDelayLog != NULL ){
                bool isReverseH = pLayerParam->pixelScaleX < 0;
                bool isReverseV = pLayerParam->pixelScaleY < 0;
                CAdjustCalc::CalcRateXYForDelayLog( &rX, &rY, rX, rY,
                                                    isReverseH, isReverseV, pLayerParam->pixelRot,
                                                    pXY, pBaseParam->pDelayLog );
            }

            // 座標設定
            pBSP->rateOfsX = rX;
            pBSP->rateOfsY = rY;
            pBSP->x = CConst::ConvBezierPositionRate( w, rX );
            pBSP->y = CConst::ConvBezierPositionRate( h, rY );
            pBSP->isFlipH = (isMC)? !pSPD->checkFlag( eSPD_FLAG_FLIP_H ): pSPD->checkFlag( eSPD_FLAG_FLIP_H );
            pBSP->isFlipV = pSPD->checkFlag( eSPD_FLAG_FLIP_V );

            // ワークパス用の色設定
            if( CBezier::IsDispAllSelected() ){
                pBSP->workPathPalOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pSPD->checkFlag( eSPD_FLAG_TEMP_SELECTED ) ){
                pBSP->workPathPalOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;
            }else if( pSPD->checkFlag( eSPD_FLAG_TEMP_ACTIVE ) ){
                pBSP->workPathPalOfs = BEZIER_WP_PAL_VAL_FOR_ACTIVE;
            }else{
                pBSP->workPathPalOfs = BEZIER_WP_PAL_VAL_FOR_SLEEP;
            }
        }
        
        pSPD = pSPD->getNext();
    }
    
    //------------------------------
    // リスト構築
    //------------------------------
    for( int i=1; i<numPoint; i++ ){
        pBSPArr[i].pPrev = &pBSPArr[i-1];
        pBSPArr[i-1].pNext = &pBSPArr[i];
    }
    
    //----------------------------------------------------
    // 座標調整（※原点調整／オフセット＆拡縮適用）
    //----------------------------------------------------
    READY_FOR_TEMP_ADJUST;
    
    // ここでは半ドット分ずらさない（※描画用の座標ではないので）
    for( int i=0; i<numPoint; i++ ){
        pBSP = &pBSPArr[i];
        ADJUST_XY_WITH_SCALE_AND_OFFSET( pBSP->x, pBSP->y, 0, 0 );
    }
    
    // ヘッダを返す
    return( &pBSPArr[0] );
}

//-------------------------------------------------------------------
// スロットポイントの解放（※引数は[CreateBezierSlotPoint]で確保した領域を想定）
//-------------------------------------------------------------------
void CBezierStructConv::ReleaseBezierSlotPoint( stBEZIER_SLOT_POINT* pHead ){
    SAFE_DELETE_ARRAY( pHead );
}
