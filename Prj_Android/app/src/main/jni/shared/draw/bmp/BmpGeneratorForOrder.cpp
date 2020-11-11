/*+----------------------------------------------------------------+
  |	Title:		BmpGeneratorForOrder.cpp [共通環境]
  |	Comment:	Bmp生成：オーダー関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenerator.hpp"

#include "draw/bezier/adjust/AdjustCalc.hpp"
#include "draw/bezier/struct/BezierStructConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//---------------------------------------------------------------
// [ORDER] オーダーの初期化（※外部データからスロットの前後関係の決定）
//---------------------------------------------------------------
bool CBmpGenerator::InitOrderList( CLayerOrderList* pOrderList, eBD_FORM form, bool isSuited ){
    // 無効は無視
    if( pOrderList == NULL ){
        return( false );
    }
    
    // 用心
    pOrderList->clear();

    // オーダーをデフォルトの並びに設定
    CBmpDotOrderData orderData;
    orderData.setDefault( form, isSuited );

    // オーダーの登録順でセルを作成（※この並びがレイヤーのデフォルト処理順となる）
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        stBMP_DOT_ORDER_DATA_CELL* pDataCell = orderData.getCellAt( i );
        if( IS_BD_SLOT_VALID( pDataCell->slot ) && IS_BD_SLOT_INDEX_VALID( pDataCell->slotIndex ) ){
            // 取得処理の内部で枠が確保される
            pOrderList->getOrderCell( pDataCell->slot, pDataCell->slotIndex );
        }
    }
    
    return( true );
}

//----------------------------------------------------------------------------------------
// [ORDER] オーダーリスト設定
//----------------------------------------------------------------------------------------
// 各レイヤーの描画は[CBmpGenerator::InitOrderList]により構成された並びで行われる
// パーツ内の[CLayerData]は[getOrderSlot/getOrderpType]により自身の表示位置を設定できる
// 同じ表示枠に登録されたレイヤーは登録順で表示される（※登録が若いほど先＝手前に表示される）
// スロットによるパーツの呼び出しは呼び出し元のレイヤーに先立って行われる
//（※パーツ(X)のレイヤー(A)がスロット呼び出しでパーツ(Y)を呼び出した場合、
//   パーツ(Y)の全レイヤーの登録が行われた後に、 呼び出し元であるレイヤー(A)の登録が行われる）
//（※仮に、レイヤー(A)のスロットオブジェクトに先行するライン／ペイントオブジェクトが存在していても、
//   スロット呼び出しがレイヤー(X)の登録に先行されるため、パーツ(Y)の各オブジェクト表示が優先されることになる）
//（※オブジェクトの微妙な表示順が悪さをしないようにスロット呼び出し専用のレイヤーを作るのが無難）
//----------------------------------------------------------------------------------------
void CBmpGenerator::SetOrderList( CLayerOrderList* pOrderList, CBmpGenParam* pGenParam,
                                  eBD_SLOT slot, int slotIndex, bool isRef,
                                  stBEZIER_BASE_PARAM* pBaseParam,
                                  stREGIST_ORDER_PARAM rop, // 呼び出し先で編集されるので値で渡す
                                  bool isWorkPath ){
    // 呼び出しパラメータの保持
    stREGIST_ORDER_PARAM rop0 = rop;    // 参照呼び出し用に調整前の値を保持しておく

    //--------------------------------------------------------
    // 特定のスロットであれば[baseX/baseY]を控えておく
    //（※ここで保持した値が描画後、基本座標とフォーカス座標に設定される）
    //--------------------------------------------------------
    // 基本座標（※左右の足元）
    if( slot == eBD_SLOT_FoGround ){
        if( slotIndex == 0 ){
            s_fTexGroundX0_L = rop.baseX;
            s_fTexGroundY0_L = rop.baseY;
        }else{
            s_fTexGroundX0_R = rop.baseX;
            s_fTexGroundY0_R = rop.baseY;
        }
    }
    // フォーカス座標（※鼻＝顔の中心）
    else if( slot == eBD_SLOT_FcNose ){
        s_fTexFocusX0 = rop.baseX;
        s_fTexFocusY0 = rop.baseY;
    }

    //-----------------------------------------------------
    // 対象スロットの生成パラメータ検索
    //-----------------------------------------------------
    stBMP_GEN_PARAM_CELL* pBGPCell = pGenParam->searchCell( slot, slotIndex );

    // 無効は無視（※登録が無い場合はそのまま返す）
    if( pBGPCell == NULL || pBGPCell->isIgnore ){
        return;
    }
    
    // 参照呼び出しでなければ
    if( ! isRef ){
        // 処理済みであれば無視（※[SlotA]→[SlotB]→[SLotA]→[SlotB]...のような循環をしてしまわないよう用心）
        if( pBGPCell->isDone ){
#ifdef BMP_GEN_LOG
            LOGD( "@ CBmpGenerator::SetOrderList: ALREADY DONE SLOT: slot=%d, slotIndex=%d\n", slot, slotIndex );
#endif
            return;
        }
        
        // 処理済みフラグを立てておく
        pBGPCell->isDone = true;
    }
    
    //---------------------------------------------------
    // 処理対象となるBDPDの設定（※参照指定があれば参照データを利用）
    //---------------------------------------------------
    CBmpDotPartData* pBDPD;
    if( isRef ){
        pBDPD = pBGPCell->pDataRef;
    }else{
        pBDPD = pBGPCell->pData;
    }
    
    // データがなければ無視（※スロットとして呼び出されてもデータが指定されていない場合がありうる）
    if( pBDPD == NULL ){
        return;
    }
    
    // アジャスタ取得
    CSlotAdjuster* pSA = pBDPD->getSlotAdjuster();
    
    //--------------------------------------------
    // 回転（※以降のスロットにも影響）
    //--------------------------------------------
    float applyRot = 0;
    
    // テンション調整による補正
    int rateRot;
    CAdjustCalc::CalcRateRotForT( &rateRot, pSA->getMinRateForT(), pSA->getMaxRateForT(), pBGPCell->adjForT );
    applyRot += CConst::ConvBezierRotationRate( rateRot );

    // 参照による補正
    if( isRef ){
        applyRot += pBGPCell->refRot;
    }
    
    // ここはオフセットの足しこみのみ（※フリップによる回転方向の反転のみを加味する）
    if( rop.isFlipH != rop.isFlipV ){ applyRot *= -1; }
    rop.rot += applyRot;

    //--------------------------------------------
    // 拡縮（※以降のスロットにも影響）
    //--------------------------------------------
    float applyScale = 1.0f;
    
    // 参照による補正
    if( isRef ){
        applyScale *= pBGPCell->refScale;
    }

    rop.scale *= applyScale;
    
    //----------------------------------------------
    // 拡縮（※このパーツにのみ影響＝パーツ毎の個性付け)
    //----------------------------------------------
    float bdpdScale = 1.0f;

    // サイズ調整による補正
    int rateScale;
    CAdjustCalc::CalcRateScaleForS( &rateScale, pSA->getMinRateForS(), pSA->getMaxRateForS(), pBGPCell->adjForS );
    bdpdScale *= CConst::ConvBezierScaleRate( rateScale );

    float bdpdScaleX = rop.scale * bdpdScale;
    float bdpdScaleY = rop.scale * bdpdScale;
    
    // フラグ適用（※タテ／ヨコ反転はスケール値の符号で行う）
    if( rop.isFlipH ){
        bdpdScaleX *= -1.0f;
    }
    
    if( rop.isFlipV ){
        bdpdScaleY *= -1.0f;
    }
    
    // 参照指定でなければアングル平面の設定（※[baseX/Y]にオフセットが適用される直前の値で設定）
    if( ! isRef ){
        SetAnglePlaneInfo( slot, slotIndex, rop.baseX, rop.baseY, rop.isFlipH, rop.isFlipV, rop.rot, rop.scale );
    }

    // ワークパス指定があれば基準点の出力
    if( isWorkPath ){
        bool isSelected = pBDPD->checkFlag( eBDPD_FLAG_TEMP_SELECTED );
        bool isInvalid = pBDPD->checkFlag( eBDPD_FLAG_DISABLE );
        
        // 多少強引だが登録処理の最中に描画してしまう
        CBezier::BasePointForDev( rop.baseX, rop.baseY, pBaseParam, isSelected, isInvalid );
    }

    //---------------------------------------------------------------------------
    // 回転／拡縮が確定したので結合点の登録（※呼び出し元と呼び出し先で座標を共有するための仕組み）
    //---------------------------------------------------------------------------
    RegistJointPoint( pBaseParam, pBGPCell, pSA, slotIndex,
                      rop.isFlipH, rop.isFlipV, applyRot, applyScale, bdpdScale, rop.rateOfsX, rop.rateOfsY );
    
    //-----------------------------------------------
    // カバー対象と表示情報の検出
    //-----------------------------------------------
    bool isCovered = false;
    int coverRateOfsX, coverRateOfsY;
    float coverScaleX, coverScaleY;
    float coverRot;
    CBmpDotPartData* pCoverBDPD = CheckThenGetCoverBDPDAndInfo( pGenParam, pBDPD, slotIndex,
                                                                &coverRateOfsX, &coverRateOfsY, &coverScaleX, &coverScaleY, &coverRot );

    // 対応するカバーパーツが存在していたら[pBDPD]はカバーされる
    if( pCoverBDPD != NULL ){
        isCovered = true;
    }
    
    //-----------------------------------------------
    // [CBmpDotPartData]内のレイヤーを走査
    //-----------------------------------------------
    CLayerData* pLD = pBDPD->getDataHead();
    while( pLD != NULL ){
        //-----------------------------------------
        // 無視判定
        //-----------------------------------------
        bool isIgnore = false;
        
        // 無効は無視
        if( pLD->checkFlag( eLAYER_FLAG_DISABLE ) ){
            isIgnore = true;
        }
        // カバー時有効フラグが立っていてスーツでない場合は無視
        else if( pLD->checkFlag( eLAYER_FLAG_VALID_IF_COVERED ) && !pBGPCell->isCovered ){
            isIgnore = true;
        }
        // カバー時無効化フラグが立っていてスーツの場合は無視
        else if( pLD->checkFlag( eLAYER_FLAG_INVALID_IF_COVERED ) && pBGPCell->isCovered ){
            isIgnore = true;
        }else{
            // オプション指定のレイヤーでフラグが寝ている場合は無視（※フォーカス時は表示する）
            eBD_OPTION option = pLD->getOption();
            if( IS_BD_OPTION_VALID( option ) && !rop.isSlotIgnore ){
                // スロットオプションであれば専用枠を参照
                if( IS_BD_OPTION_FOR_SLOT_VALID( option ) ){
                    isIgnore = ! pBaseParam->arrOptionForSlot[slot][slotIndex][option-eBD_OPTION_FOR_SLOT_START];
                }
                // その他のオプションであれば広域枠を参照
                else{
                    isIgnore = ! pBaseParam->arrOption[option][slotIndex];
                }
            }
            
            // 非表示フラグが立っていたらはみ出る髪は無視
            if( pGenParam->isHideOverHair() ){
                isIgnore = pLD->checkFlag( eLAYER_FLAG_OVER_HAIR );
            }
        }

        // この時点でフラグが立っていたら無視
        if( isIgnore ){
            // 次のレイヤーへ
            pLD = pLD->getNext();
            continue;
        }
        
        //-------------------------------------------
        // レイヤー調整値（※レイヤー単位で表示を補正するもの）
        //-------------------------------------------
        int rate, rateX, rateY;
        bool isReverseH = false;
        bool isReverseV = (slotIndex != 0); // 対の要素（おさげの片方等）であれば縦反転

        // 回転割合（※髪の房等、常に下に向かって垂れて欲しい要素には回転割合が低めに設定される）
        float rotPowForLayer = pLD->getRotPow();

        // T：回転（※ここは左右の影響を受ける）
        CAdjustCalc::CalcRateRotForT( &rate, pLD->getMinRateForT(), pLD->getMaxRateForT(), pBGPCell->adjForT );
        float rotForLayerWithT = CConst::ConvBezierRotationRate( rate );
        if( slotIndex != 0 ){ rotForLayerWithT *= -1.0f; }

        // 遅延：座標（※ここは左右の影響を受けない＝移動値の方向そのままを受け取る）
        CAdjustCalc::CalcRateOfsXYForDelayLog( &rateX, &rateY, false, false, pLD, pBaseParam->pDelayLog );
        float ofsXForLayer = CConst::ConvBezierPositionRate( pBaseParam->texW0, rateX );
        float ofsYForLayer = CConst::ConvBezierPositionRate( pBaseParam->texH0, rateY );
        
        // 遅延：回転
        CAdjustCalc::CalcRateRotForDelayLog( &rate, isReverseH, isReverseV, pLD, pBaseParam->pDelayLog );
        float rotForLayer = CConst::ConvBezierRotationRate( rate );

        // 遅延：スケール
        CAdjustCalc::CalcRateScaleForDelayLog( &rate, pLD, pBaseParam->pDelayLog );
        float scaleForLayer = CConst::ConvBezierScaleRate( rate );
                
        //----------------------------------
        // レイヤー内部のオブジェクトを走査
        //----------------------------------
        CLayerObject* pLO = pLD->getDataHead();
        while( pLO != NULL ){
            //-------------------------------------------------------------------------------
            // スロット[別パーツの呼び出し]を優先（※編集調整中は無視／参照時は指定がなければ無視）
            //-------------------------------------------------------------------------------
            if( pLO->isSlotObject() && !rop.isSlotIgnore && (!isRef || pBGPCell->isRefSlotValid) ){
                CSlotObjectData* pSOD = pLO->getSlotObject();
                
                // 有効であればスロット対象の登録
                if( ! pSOD->checkFlag( eSOD_FLAG_DISABLE ) ){
                    int loop = 0;
                    do{
                        bool isMC = (loop==1);
                        
                        // レイヤーパラメータの確保＆設定（※ここは呼び出し先の座標検出をしたいだけなのでスロット等の設定は不要）
                        stBEZIER_LAYER_PARAM stLayerParam;
                        CLEAR_BEZIER_LAYER_PARAM( &stLayerParam );
                        
                        stLayerParam.fixedAdjustRateH = pBaseParam->adjustRateH;
                        stLayerParam.fixedAdjustRateV = pBaseParam->adjustRateV;
                        stLayerParam.fixedAngleRateLR = pBaseParam->adjustRateLR;
                        stLayerParam.fixedAngleRateUD = pBaseParam->adjustRateUD;

                        stLayerParam.pixelOfsX = rop.baseX + ofsXForLayer;
                        stLayerParam.pixelOfsY = rop.baseY + ofsYForLayer;
                        stLayerParam.pixelScaleX = bdpdScaleX * scaleForLayer;
                        stLayerParam.pixelScaleY = bdpdScaleY * scaleForLayer;
                        stLayerParam.pixelRot = s_stBaseParam.rot + rotPowForLayer*(rop.rot-s_stBaseParam.rot) + rotForLayer;
                        stLayerParam.pixelRotWithT = rotForLayerWithT;
                        
                        stLayerParam.scaleForBdpd = bdpdScale;
                                                
                        stLayerParam.tempRateOfsX = pLD->getTempAdjustRateOfsX();
                        stLayerParam.tempRateOfsY = pLD->getTempAdjustRateOfsY();
                        stLayerParam.tempRateScaleX = pLD->getTempAdjustRateScaleX();
                        stLayerParam.tempRateScaleY = pLD->getTempAdjustRateScaleY();

                        // スロットポイントの変換（※有効なポイントがリスト形式で返される）
                        //（※[stLayerParam]で渡したパラメータに対し、各スロットの調整座標が加味されたデータがリストで返される）
                        stBEZIER_SLOT_POINT* pBSPNodeHead;
                        pBSPNodeHead = CBezierStructConv::CreateBezierSlotPoint( pSOD, pBaseParam, &stLayerParam, isMC );
                        
                        // 各スロットポイントをオーダーリストへ登録
                        stBEZIER_SLOT_POINT* pBSPNode = pBSPNodeHead;
                        while( pBSPNode != NULL ){
                            // スロットインデックスが無効であれば呼び出し元に依存
                            int tempSlotIndex = pBSPNode->slotIndex;
                            if( ! IS_BD_SLOT_INDEX_VALID( tempSlotIndex ) ){ tempSlotIndex = slotIndex; }

                            // オーダー登録パラメータの設定（※ここは登録毎に値を設定する）
                            stREGIST_ORDER_PARAM tmpRop = rop;  // 親の値を丸ごとコピー

                            tmpRop.isFlipH ^= pBSPNode->isFlipH;
                            tmpRop.isFlipV ^= pBSPNode->isFlipV;
                            tmpRop.rot = s_stBaseParam.rot + rotPowForLayer*(rop.rot-s_stBaseParam.rot) + rotForLayer;
                            tmpRop.rot += ((isMC)? -rotForLayerWithT: rotForLayerWithT);
                            tmpRop.baseX = pBSPNode->x + ofsXForLayer;
                            tmpRop.baseY = pBSPNode->y + ofsYForLayer;
                            tmpRop.rateOfsX = pBSPNode->rateOfsX;
                            tmpRop.rateOfsY = pBSPNode->rateOfsY;
                            
                            // スロットの原点を保持しておく（※描画直前のスロットフック用）
                            s_fArrSlotBaseX[pBSPNode->slot][tempSlotIndex] = tmpRop.baseX;
                            s_fArrSlotBaseY[pBSPNode->slot][tempSlotIndex] = tmpRop.baseY;

                            // オーダー登録の再起呼び出し（※呼び出し元のレイヤー登録は、このスロット呼び出しが終わった後に行われる）
                            SetOrderList( pOrderList, pGenParam, pBSPNode->slot, tempSlotIndex, false, pBaseParam, tmpRop, isWorkPath );
                            
                            // 次へ
                            pBSPNode = pBSPNode->pNext;
                        }
                        
                        // 解放
                        CBezierStructConv::ReleaseBezierSlotPoint( pBSPNodeHead );
                        
                        loop++;
                    }while( pSOD->checkFlag( eSOD_FLAG_MIRROR_COPY ) && loop==1 );
                }
            }

            // 次のオブジェクトへ
            pLO = pLO->getNext();
        }
        
        //-----------------------------------------------------
        // レイヤーの登録（※スロット呼び出し終了後に描画要素の登録を行う）
        //-----------------------------------------------------
        if( !isRef || pBGPCell->isRefDrawValid ){
            // カバーが有効ならフラグの検出
            bool isMaskCoverRequired = false;
            bool isFrontCoverRequired = false;
            bool isBackCoverRequired = false;
            if( pCoverBDPD != NULL ){
                isMaskCoverRequired = pLD->checkFlag( eLAYER_FLAG_MASK_COVER );
                isFrontCoverRequired = pLD->checkFlag( eLAYER_FLAG_FRONT_COVER );
                isBackCoverRequired = pLD->checkFlag( eLAYER_FLAG_BACK_COVER );
            }
                        
            // work
            bool isTargetDone = false;
            int maskCoverLayerAt = 0;
            int frontCoverLayerAt = 0;
            int backCoverLayerAt = 0;

            //---------------------------------------------------------------------------------------------
            // レイヤーの登録（※[pLD]を中心にして前後にカバーレイヤーを登録する）
            // カバーの挙動には明確なルール（制限）を持たせる（※これにより呼び出し元とカバー側のレイヤーの描画の整合性をとる）
            // [mC=NORMAL]でカバーのマスクと下地が処理され、[fC=FRONT]と[bB=BACK]で呼び出し元を挟む要素を描画する想定
            //---------------------------------------------------------------------------------------------
            while( true ){
                CLayerData* pCurLayer = NULL;
                bool isCoverPart = false;

                //-------------------------------------------------------------------------------------------------
                // カバーパーツの[NORMAL]オーダーレイヤーは「マスク」と「下地」を想定する（※[mC]で呼ばれるレイヤー）
                //-------------------------------------------------------------------------------------------------
                // マスクは[PRE_DRAW]フラグで最初に処理されるようにする（※関連する要素全ての描画を制限する）
                // 下地は[ORDER_GROUP==COVER]、[MASK_TARGET==INVALID]を設定し、ハリボテの上にマスク無視で描画されるようにする
                //-------------------------------------------------------------------------------------------------
                if( pCurLayer == NULL && isMaskCoverRequired ){
                    if( (pCurLayer=pCoverBDPD->getLayerDataWithOrderTypeAt( eBD_LAYER_ORDER_TYPE_NORMAL, maskCoverLayerAt )) != NULL ){
                        maskCoverLayerAt++;
                        isCoverPart = true;
                    }
                }

                //-------------------------------------------------------------------------------------------------
                // カバーパーツの[FRONT]オーダーレイヤーは呼び出し対象直前の表示を想定する（※[fC]で呼ばれるレイヤー）
                //-------------------------------------------------------------------------------------------------
                // [ORDER_SLOT==DEFAULT]であれば対象のスロットの直前に配置される
                // [ORDER_SLOT!=DEFAULT]であれば指定のスロット枠へ登録される（※登録タイプは[FRONT]となる）
                //-------------------------------------------------------------------------------------------------
                if( pCurLayer == NULL && isFrontCoverRequired ){
                    if( (pCurLayer=pCoverBDPD->getLayerDataWithOrderTypeAt( eBD_LAYER_ORDER_TYPE_FRONT, frontCoverLayerAt )) != NULL ){
                        frontCoverLayerAt++;
                        isCoverPart = true;
                    }
                }

                //--------------------------------------
                // この時点で[pCurLayer]無効なら[pLD]を設定
                //--------------------------------------
                if( pCurLayer == NULL && !isTargetDone ){
                    pCurLayer = pLD;
                    isTargetDone = true;
                }

                //-------------------------------------------------------------------------------------------------
                // カバーパーツの[BACK]オーダーレイヤーは呼び出し対象直後の表示を想定する（※[bC]で呼ばれるレイヤー）
                //-------------------------------------------------------------------------------------------------
                // [ORDER_SLOT==DEFAULT]であれば対象のスロットの直後に配置される
                // [ORDER_SLOT!=DEFAULT]であれば指定のスロット枠へ登録される（※登録タイプは[BACK]となる）
                //-------------------------------------------------------------------------------------------------
                if( pCurLayer == NULL && isBackCoverRequired ){
                    if( (pCurLayer=pCoverBDPD->getLayerDataWithOrderTypeAt( eBD_LAYER_ORDER_TYPE_BACK, backCoverLayerAt )) != NULL ){
                        backCoverLayerAt++;
                        isCoverPart = true;
                    }
                }

                // この時点で処理対象が無効なら終了
                if( pCurLayer == NULL ){
                    break;
                }

                // オーダータイプの抽出
                eBD_LAYER_ORDER_TYPE orderType = pCurLayer->getOrderType();

                // オーダースロットの解決（※カバー時の指定が有効であれば差し替える）
                eBD_SLOT orderSlot = pCurLayer->getOrderSlot();
                if( !isCoverPart && pBGPCell->isCovered ){
                    eBD_SLOT orderSlotForCovered = pCurLayer->getOrderSlotForCovered();
                    if( orderSlotForCovered != eBD_SLOT_DEFAULT ){
                        orderSlot = orderSlotForCovered;
                    }
                }
                
                // デフォルト指定であれば調整
                if( orderSlot == eBD_SLOT_DEFAULT ){
                    orderSlot = slot;
                    
                    // オーダーがデフォルト指定の場合はオータータイプも呼び出し元へ合わせる（※カバーパーツが対象のパーツの前後に配置されるように）
                    if( isCoverPart ){
                        orderType = pLD->getOrderType();
                    }
                }
                
                // オーダースロットインデックスの解決
                int orderSlotIndex = CBDConst::FixIndexForSlot( orderSlot, slotIndex );
                
#ifdef BMP_GEN_LOG
                LOGD( "@ [ADD:%s]: slot=%s[%d](%s[%d]), slotIndex=%d(%d), fH=%d, fV=%d\n",
                      pCurLayer->getName(),
                      g_pArrLabelBdSlot[orderSlot], orderSlot,
                      g_pArrLabelBdSlot[slot], slot,
                      orderSlotIndex, slotIndex,
                      isFlipH, isFlipV );
#endif

                // レイヤーの登録（※これで[pCurLayer]が表示されることになる）
                stLAYER_ORDER_CELL_NODE* pNode = pOrderList->addLayerData( pCurLayer, orderType, orderSlot, orderSlotIndex );
                
                // 用心（※登録枠不足の際はサイズを拡張すること）
                if( pNode == NULL ){
                    LOGE( "@ CBmpGenerator::SetOrderList: FAILED [ADD:%s]: orderSlot=%d, orderSlotIndex=%d\n",
                          pCurLayer->getName(), orderSlot, orderSlotIndex );
                    return;
                }
                
                // カバーパーツであれば調整値の設定
                eBD_SLOT partSlot0 = slot;
                int adjRateOfsX = 0;
                int adjRateOfsY = 0;
                float adjScaleX = 1.0f;
                float adjScaleY = 1.0f;
                float adjRot = 0.0f;

                if( isCoverPart ){
                    partSlot0 = pCoverBDPD->getSlot();

                    adjRateOfsX = coverRateOfsX;
                    adjRateOfsY = coverRateOfsY;
                    adjScaleX = coverScaleX;
                    adjScaleY = coverScaleY;
                    adjRot = coverRot;
                    
                    // 反転調整
                    if( rop.isFlipH != rop.isFlipV ){
                        adjRateOfsX *= -1;
                        adjRot *= -1;
                    }
                }

                //------------------------------------------------------------------------
                // レイヤーパラメータの設定（※この値はクリア済みの状態なので登録側で設定する必要がある）
                //------------------------------------------------------------------------
                stBEZIER_LAYER_PARAM* pLP = &pNode->layerParam;
                
                // スロット（※カバーのレイヤーであっても呼び出し元のスロットとして登録）
                // 注意：スロット枠が混在することになるので、カバー側で流動的なフック等を利用してはならない
                pLP->slot = slot;
                pLP->slot0 = partSlot0;
                pLP->slotIndex = slotIndex;
                
                // フック（※この時点ではロット座標が全て確定していないので処理の予約をしておくのみ）
                pLP->hookSlot = pBDPD->getHookSlot();
                pLP->hookSlotIndex = pBDPD->getHookSlotIndex();
                if( ! IS_BD_SLOT_INDEX_VALID( pLP->hookSlotIndex ) ){ pLP->hookSlotIndex = slotIndex; }

                // 親スロット
                pLP->parentSlot = CBDConst::GetParentSlotForAnglePlane( slot );
                pLP->parentSlotIndex = CBDConst::FixIndexForSlot( pLP->parentSlot, slotIndex );

                // 調整：H
                pLP->fixedAdjustRateH = pBaseParam->adjustRateH;
                FIX_ADJUST_RATE( pLP->fixedAdjustRateH );

                // 調整：V
                pLP->fixedAdjustRateV = pBaseParam->adjustRateV;
                FIX_ADJUST_RATE( pLP->fixedAdjustRateV );

                // 傾き：左右
                pLP->fixedAngleRateLR = pBaseParam->adjustRateLR;
                FIX_ADJUST_RATE( pLP->fixedAngleRateLR );

                // 傾き：上下
                pLP->fixedAngleRateUD = pBaseParam->adjustRateUD;
                FIX_ADJUST_RATE( pLP->fixedAngleRateUD );

                // 位置／拡縮／回転
                pLP->pixelOfsX = rop.baseX + ofsXForLayer + CConst::ConvBezierPositionRate( pBaseParam->texW0, adjRateOfsX );
                pLP->pixelOfsY = rop.baseY + ofsYForLayer + CConst::ConvBezierPositionRate( pBaseParam->texH0, adjRateOfsY );
                pLP->pixelScaleX = bdpdScaleX * scaleForLayer * adjScaleX;
                pLP->pixelScaleY = bdpdScaleY * scaleForLayer * adjScaleY;
                pLP->pixelRot = s_stBaseParam.rot + rotPowForLayer*(rop.rot-s_stBaseParam.rot) + rotForLayer + adjRot;
                pLP->pixelRotWithT = rotForLayerWithT;

                // ビットマスク（※レイヤー単位でマスクの参照先を設定する）
                pLP->bitMask = pCurLayer->getBitMask( slot, slotIndex );

                // 結合ポイント算出用にBDPのスケールの保持（結合ポイントの座標は基本スケールで計算されているので）
                pLP->scaleForBdpd = bdpdScale;
                
                // ストロークサイズ
                pLP->strokeScale = rop.strokeScale;

                // 一時調整値
                pLP->tempRateOfsX = pCurLayer->getTempAdjustRateOfsX();
                pLP->tempRateOfsY = pCurLayer->getTempAdjustRateOfsY();
                pLP->tempRateScaleX = pCurLayer->getTempAdjustRateScaleX();
                pLP->tempRateScaleY = pCurLayer->getTempAdjustRateScaleY();
                
                // グループ指定（※[DEFAULT]指定であれば呼び出し元のパーツから判断）
                pNode->group = pCurLayer->getOrderGroup();
                if( pNode->group == eBD_LAYER_ORDER_GROUP_DEFAULT ){
                    // カバー時であっても[pCoverBDPD]の値は見ない（※カバーレイヤーのグループを指定したい場合はデータ上で明示すること）
                    pNode->group = CBDConst::GetOrderGroupForSlot( pBDPD->getSlot() );
                }
            }
        }
        
        // 次のレイヤーへ
        pLD = pLD->getNext();
    }
    
    // 参照でなければ
    if( ! isRef ){
        // 参照データが有効であれば対象を処理（※引数をそのまま受け渡して[pDataRef]を処理させる）
        if( pBGPCell->pDataRef != NULL ){
            SetOrderList( pOrderList, pGenParam, slot, slotIndex, true, pBaseParam, rop0, isWorkPath );
        }
    }
}

//--------------------------
// [ORDER] オーダーリストの確定
//--------------------------
void CBmpGenerator::FixOrderList( CLayerOrderList* pOrderList ){
    //----------------------------------------------------------
    // スロットのフック（※指定がある場合、別スロットの位置にフックする）
    //----------------------------------------------------------
    CLayerOrderCell* pOrderCell = pOrderList->getOrderCellHead();
    
    while( pOrderCell != NULL ){
        for( int i=0; i<eBD_LAYER_ORDER_TYPE_MAX; i++ ){
            stLAYER_ORDER_CELL_NODE* pNode = pOrderCell->getNodeHead( (eBD_LAYER_ORDER_TYPE)i );

            while( pNode != NULL ){
                stBEZIER_LAYER_PARAM* pLayerParam = &pNode->layerParam;

                // 指定が有効ならオフセットをフック先の座標で上書き
                if( IS_BD_SLOT_VALID( pLayerParam->hookSlot ) && IS_BD_SLOT_INDEX_VALID( pLayerParam->hookSlotIndex ) ){
                    pLayerParam->pixelOfsX = s_fArrSlotBaseX[pLayerParam->hookSlot][pLayerParam->hookSlotIndex];
                    pLayerParam->pixelOfsY = s_fArrSlotBaseY[pLayerParam->hookSlot][pLayerParam->hookSlotIndex];
                }
                
                pNode = pNode->pNext;
            }
        }

        pOrderCell = pOrderCell->getNext();
    }
}

//--------------------------------
// [ORDER] オーダーリストの描画（窓口）
//--------------------------------
void CBmpGenerator::DrawOrderList( CLayerOrderList* pOrderList, bool isSuited, bool isWorkPath ){
    // 事前描画（※レイヤーフラグ[PRE_DRAW]の処理＝フック座標やマスク描画等、事前に確定させておきたい処理を先行させる）
    //（※ここは[isSuited]の場合でも[DEFAULT]で処理する＝処理順による影響がない想定）
    DrawOrderList0( pOrderList, isWorkPath, true, eBD_LAYER_ORDER_GROUP_DEFAULT );

    // 本描画
    if( isSuited ){
        // スーツ時は各グループを順番に表示
        for( int i=eBD_LAYER_ORDER_GROUP_START_FOR_SUIT; i<=eBD_LAYER_ORDER_GROUP_END_FOR_SUIT; i++ ){
            DrawOrderList0( pOrderList, isWorkPath, false, (eBD_LAYER_ORDER_GROUP)i );
        }
    }else{
        // 通常時はグループ分けせずに表示（※[DEFAULT]指定でグループ判定を無視する）
        DrawOrderList0( pOrderList, isWorkPath, false, eBD_LAYER_ORDER_GROUP_DEFAULT );
    }
}

//--------------------------------
// [ORDER] オーダーリストの描画（実体）
//--------------------------------
void CBmpGenerator::DrawOrderList0( CLayerOrderList* pOrderList, bool isWorkPath, bool isPreDraw, eBD_LAYER_ORDER_GROUP targetGroup ){
#ifdef BMP_GEN_LOG
    LOGD( "@ DRAW LAYER: isWorkPath=%d, isPreDraw=%d, targetGroup=%s\n", isWorkPath, isPreDraw, g_pArrLabelBdLayerOrderGroup[targetGroup] );
#endif

    //-------------------------------------------
    // オーダーリストに登録されたレイヤーの描画
    //-------------------------------------------
    CLayerOrderCell* pOrderCell;
    int priority = pOrderList->getOrderCellPriority();
    for( int i=0; i<priority; i++ ){
        // 優先順位に応じてセルの取り出し（※無効は無視）
        pOrderCell = pOrderList->getOrderCellByPriority( i );
        if( pOrderCell == NULL ){
            continue;
        }

        // 登録内容を[FRONT/NORMAL/BACK]の順に処理
        for( int j=0; j<eBD_LAYER_ORDER_TYPE_MAX; j++ ){
            stLAYER_ORDER_CELL_NODE* pNode = pOrderCell->getNodeHead( (eBD_LAYER_ORDER_TYPE)j );
            while( pNode != NULL ){
                // 有効なレイヤーが未処理であれば
                CLayerData* pLayerData = pNode->pLayerData;
                if( pLayerData != NULL && !pNode->isDone ){
                    bool isValid = false;

                    // グループ指定が有効か？
                    if( pNode->group == targetGroup || targetGroup == eBD_LAYER_ORDER_GROUP_DEFAULT ){
                        // 事前描画指定があればレイヤーのフラグを確認
                        if( isPreDraw ){
                            isValid = pLayerData->checkFlag( eLAYER_FLAG_PRE_DRAW );
                        }
                        // 通常時であればそのまま抽出
                        else{
                            isValid = true;
                        }
                    }
                    
                    // 有効であれば
                    if( isValid ){
#ifdef BMP_GEN_LOG
                        // 内容の確認
                        const char* pLabelForData = "UNKNOWN";
                        eBD_SLOT slotForData = pLayerData->getOrderSlot();
                        if( IS_BD_SLOT_VALID( slotForData ) ){
                            pLabelForData = g_pArrLabelBdSlot[slotForData];
                        }else if( slotForData == eBD_SLOT_INVALID ){
                            pLabelForData = "INVALID";
                        }

                        const char* pLabelForOrder = "UNKNOWN";
                        eBD_SLOT slotForOrder = pOrderCell->getOrderSlot();
                        if( IS_BD_SLOT_VALID( slotForOrder ) ){
                            pLabelForOrder = g_pArrLabelBdSlot[slotForOrder];
                        }else if( slotForOrder == eBD_SLOT_INVALID ){
                            pLabelForOrder = "INVALID";
                        }

                        LOGD( "@ [%s][DRAW:%s]: order=%s, slot=%s(%d)/%s(%d), index=%d\n",
                              g_pArrLabelBdLayerOrderGroupS[targetGroup],
                              pLayerData->getName(), g_pArrLabelBdLayerOrderType[j],
                              pLabelForData, slotForData, pLabelForOrder, slotForOrder, pOrderCell->getOrderSlotIndex());
#endif
                        // レイヤーの描画
                        pLayerData->draw( &s_stBaseParam, &pNode->layerParam, isWorkPath );

                        // 処理フラグを立てておく（※重複呼び出しの回避用）
                        pNode->isDone = true;
                    }
                }
                
                pNode = pNode->pNext;
            }
        }
    }
}
