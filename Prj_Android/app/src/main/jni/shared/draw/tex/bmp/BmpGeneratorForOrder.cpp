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
#include "draw/tex/adjust/AdjustCalc.hpp"
#include "draw/tex/bezier/BezierDataConv.hpp"

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
// [ORDER] オーダーの事前構築（※外部データからスロットの前後関係の決定）
//---------------------------------------------------------------
// ・[スロット, スロットインデックス]を１組として表示順を列挙する
// ・インデックスが有効な（左右の区別がある）場合は[1,0]の順で登録
// （※親が参照する際に[XxXxx,0]でアクセスするため[0]を最後に確定しておく）
//---------------------------------------------------------------
bool CBmpGenerator::RegistOrder( CLayerOrderList* pOrderList, eBD_FORM form, bool isSuited ){
    // 無効は無視
    if( pOrderList == NULL ){
        return( false );
    }
    
    CBmpDotOrderData orderData;
    orderData.setDefault( form, isSuited );
    
    // 最前面
    pOrderList->getOrderCell( eBD_SLOT_LyFront, 0, true );

    // 登録
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        stBMP_DOT_ORDER_DATA_CELL* pCell = orderData.getCellAt( i );
        if( pCell->slot >= 0 && pCell->slot < eBD_SLOT_MAX ){
            pOrderList->getOrderCell( pCell->slot, pCell->slotIndex, true );
        }
    }
    
    // 最後面
    pOrderList->getOrderCell( eBD_SLOT_LyBack, 0, true );
    
    return( true );
}

//-----------------------------------------------------------------
// [ORDER] ジョイントポイント登録（※[XxJointYyy]のスロットから呼ばれる想定）
//-----------------------------------------------------------------
// [FROM=結合元]と[TO=結合先]の間で座標を同期させる必要があるので、
// [XxJointYyy]のBDPDに対する調整途中経過の値を保持しておく必要がある
//（※ここで保持した値は[CBezierDataConv]での座標算出に利用される）
//-----------------------------------------------------------------
bool CBmpGenerator::RegistJointPoint( stBEZIER_BASE_PARAM* pBaseParam, stBMP_GEN_PARAM_CELL* pBGPCell, CSlotAdjuster* pSlotAdj,
                                      int slotIndex, bool isFlipH, bool isFlipV,
                                      float applyRot, float applyScale, float bdpdScale, int rateOfsX, int rateOfsY ){
    // 無効は無視
    if( pBaseParam == NULL || pBaseParam->pJointPoint == NULL || pSlotAdj == NULL ){
        return( false );
    }

    //----------------------------------------------------------------------------
    // [pSlotAdj]で指定される結合ポイントの登録
    //（※ここでは素材レベルの座標を設定する＝左右／上下反転は描画時に加味されるので反転調整は不要）
    //----------------------------------------------------------------------------
    // IDの検出：FROM（※指定がなければ無視）
    eJOINT_POINT jointIdFrom0 = pSlotAdj->getJointPointIdForFrom( slotIndex );
    eJOINT_POINT jointIdFrom1 = (eJOINT_POINT)(jointIdFrom0+JOINT_POINT_OFS_FOR_OTHER_SIDE);
    if( jointIdFrom0 < 0 || jointIdFrom0 >= eJOINT_POINT_MAX || jointIdFrom1 < 0 || jointIdFrom1 >= eJOINT_POINT_MAX ){
        return( false );
    }

    // IDの検出：TO（※指定がなければ無視）
    eJOINT_POINT jointIdTo0 = pSlotAdj->getJointPointIdForTo( slotIndex );
    eJOINT_POINT jointIdTo1 = (eJOINT_POINT)(jointIdTo0+JOINT_POINT_OFS_FOR_OTHER_SIDE);
    if( jointIdTo0 < 0 || jointIdTo0 >= eJOINT_POINT_MAX || jointIdTo1 < 0 || jointIdTo1 >= eJOINT_POINT_MAX ){
        return( false );
    }
    
    //----------------------------------------------------------------------------
    // ここまできたら登録準備
    //----------------------------------------------------------------------------
    CJointPoint* pJointPoint = pBaseParam->pJointPoint;
    float bdpdScaleAbs = (bdpdScale < 0 )? -bdpdScale: bdpdScale;
    float rotBase = CConst::ConvBezierRotationRate( pSlotAdj->getJointRateRotOfs() );
    float fixedAdjustRateH = pBaseParam->adjustRateH;
    FIX_ADJUST_RATE( fixedAdjustRateH );

    int jointR, jointOfsR;
    CAdjustCalc::CalcRateRForH( &jointR, &jointOfsR, pSlotAdj->getJointRateRotR(), pSlotAdj->getJointRateOfsForRotCenter(),
                                pSlotAdj, fixedAdjustRateH );

    int ofsForX = CMath::RotX( 0, jointOfsR, rotBase );
    int ofsForY = CMath::RotY( 0, jointOfsR, rotBase );
    
    float fixedApplyRot = ((isFlipH != isFlipV)?-applyRot:applyRot);

    //--------------------------------------------------------
    // 結合元の座標（※[XxJointYyy]を呼ぶ側）
    //--------------------------------------------------------
    // 接続先との回転調整座標と未調整座標の両方を算出（※[applyRot]を正の方向に50%適用）
    float rot0 = rotBase + fixedApplyRot/2 - 90;    // 実角度計算なので反転による向きを考慮する
    float rot1 = rotBase + fixedApplyRot/2 + 90;    // 実角度計算なので反転による向きを考慮する
    float rot0_0 = rotBase - 90;
    float rot1_0 = rotBase + 90;

    // 座標算出
    float fromX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 ) + ofsForX;
    float fromY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 ) + ofsForY;
    float fromX0_0 = CMath::RotX( jointR+jointOfsR, 0, rot0_0 ) + ofsForX;
    float fromY0_0 = CMath::RotY( jointR+jointOfsR, 0, rot0_0 ) + ofsForY;
    pJointPoint->registJointPointFrom( jointIdFrom0, (int)fromX0, (int)fromY0, (int)fromX0_0, (int)fromY0_0,
                                       rateOfsX, rateOfsY, applyScale*bdpdScaleAbs, rotBase, isFlipH, isFlipV );
    
    float fromX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 ) + ofsForX;
    float fromY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 ) + ofsForY;
    float fromX1_0 = CMath::RotX( jointR-jointOfsR, 0, rot1_0 ) + ofsForX;
    float fromY1_0 = CMath::RotY( jointR-jointOfsR, 0, rot1_0 ) + ofsForY;
    pJointPoint->registJointPointFrom( jointIdFrom1, (int)fromX1, (int)fromY1, (int)fromX1_0, (int)fromY1_0,
                                       rateOfsX, rateOfsY, applyScale*bdpdScaleAbs, rotBase, isFlipH, isFlipV );
        
    //--------------------------------------------------------
    // 結合先の座標（※[XxJointYyy]から呼ばれる側）
    //--------------------------------------------------------
    // 接続先との回転調整座標と未調整座標の両方を算出（※[applyRot]を負の方向に50%適用）
    rot0 = rotBase - fixedApplyRot/2 - 90; // 実角度計算なので反転による向きを考慮する
    rot1 = rotBase - fixedApplyRot/2 + 90; // 実角度計算なので反転による向きを考慮する
    rot0_0 = rotBase - 90;
    rot1_0 = rotBase + 90;
    
    // 座標算出
    float toX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 ) + ofsForX;
    float toY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 ) + ofsForY;
    float toX0_0 = CMath::RotX( jointR+jointOfsR, 0, rot0_0 ) + ofsForX;
    float toY0_0 = CMath::RotY( jointR+jointOfsR, 0, rot0_0 ) + ofsForY;
    pJointPoint->registJointPointTo( jointIdTo0, (int)toX0, (int)toY0, (int)toX0_0, (int)toY0_0,
                                     rateOfsX, rateOfsY, bdpdScaleAbs, applyRot+rotBase, isFlipH, isFlipV );
    
    float toX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 ) + ofsForX;
    float toY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 ) + ofsForY;
    float toX1_0 = CMath::RotX( jointR-jointOfsR, 0, rot1_0 ) + ofsForX;
    float toY1_0 = CMath::RotY( jointR-jointOfsR, 0, rot1_0 ) + ofsForY;
    pJointPoint->registJointPointTo( jointIdTo1, (int)toX1, (int)toY1, (int)toX1_0, (int)toY1_0,
                                     rateOfsX, rateOfsY, bdpdScaleAbs, applyRot+rotBase, isFlipH, isFlipV );
    
#if 1
    // 方向線の判定用に計算しなおし（※座標の移動は呼び出し元と呼び出し先を半々に回転させているが、方向線では呼び出し先のみ回転させて計算）
    fromX0 = fromX0_0;
    fromY0 = fromY0_0;
    fromX1 = fromX1_0;
    fromY1 = fromY1_0;

    rot0 = rotBase - fixedApplyRot - 90;
    rot1 = rotBase - fixedApplyRot + 90;
    toX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 ) + ofsForX;
    toY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 ) + ofsForY;
    toX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 ) + ofsForX;
    toY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 ) + ofsForY;
#endif

    //-------------------------------------------------
    // 方向線調整の算出（※[from-to]の中央の角度で方向線を調整）
    //-------------------------------------------------
    float dirX, dirY;
    float dX, dY, len;
    float fromRate = pSlotAdj->getJointDirRateForFrom();
    float toRate = pSlotAdj->getJointDirRateForTo();
    float openRate = pSlotAdj->getJointDirRateForOpen();
    float closeRate = pSlotAdj->getJointDirRateForClose();
    
    // [0=左側]に対する調整：[from]->[to]の方向で計算
    dX = toX0 - fromX0;
    dY = toY0 - fromY0;
    len = CMath::Len( dX, dY );
    if( len > 0.0f ){
        // 呼び出し元への侵入[IN]
        dirX = dX * fromRate;
        dirY = dY * fromRate;
        pJointPoint->setDirRateXY( jointIdFrom0, (int)dirX, (int)dirY, openRate, closeRate );
        
        // 呼び出し先からの出立[OUT]（※描画時に打ち消されるように逆回転）
        dirX = CMath::RotX( dX, dY, -fixedApplyRot ) * toRate;
        dirY = CMath::RotY( dX, dY, -fixedApplyRot ) * toRate;
        pJointPoint->setDirRateXY( jointIdTo0, (int)-dirX, (int)-dirY, openRate, closeRate );
    }else{
        pJointPoint->setDirRateXY( jointIdFrom0, 0, 0, 0.0f, 0.0f );
        pJointPoint->setDirRateXY( jointIdTo0, 0, 0, 0.0f, 0.0f );
    }
    
    // [1=右側]に対する調整：[to]->[from]の方向で計算
    dX = fromX1 - toX1;
    dY = fromY1 - toY1;
    len = CMath::Len( dX, dY );
    if( len > 0.0f ){
        // 呼び出し元からの出立[OUT]
        dirX = dX * fromRate;
        dirY = dY * fromRate;
        pJointPoint->setDirRateXY( jointIdFrom1, (int)dirX, (int)dirY, openRate, closeRate );
        
        // 呼び出し先への侵入[0]（※描画時に打ち消されるように逆回転）
        dirX = CMath::RotX( dX, dY, -fixedApplyRot ) * toRate;
        dirY = CMath::RotY( dX, dY, -fixedApplyRot ) * toRate;
        pJointPoint->setDirRateXY( jointIdTo1, (int)-dirX, (int)-dirY, openRate, closeRate );
    }else{
        pJointPoint->setDirRateXY( jointIdFrom1, 0, 0, 0.0f, 0.0f );
        pJointPoint->setDirRateXY( jointIdTo1, 0, 0, 0.0f, 0.0f );
    }
    
    // ここまできたら登録完了
    return( true );
}

//----------------------------------------------------------------------------------------
// [ORDER] オーダーリスト設定
//----------------------------------------------------------------------------------------
// 各スロットの描画順は[CBmpGenerator::RegistOrder]により事前登録されていればその並びで表示される
// [RegistOrder]で登録されていない場合は[CLayerOrderList::addLayerData]への登録順に表示される
// また、スロット内の[CLayerData]は[getOrderSlot/getOrderpType]により表示順をここに設定できる
// 同じ表示枠に登録されたレイヤーは登録順に表示される（※登録が若いほど先＝手前に表示される）
// スロットの呼び出しは、呼び出し元のレイヤーデータの登録に先立って行われる
//（※BDPD[A]のレイヤー[A]がスロット呼び出しでBDPD[B]を呼び出した場合、
//   BDPD[B]の全レイヤーの登録が先に行われた後に、 呼び出し元であるレイヤー[A]の登録が行われる）
//（※仮に、レイヤー[A]のスロットオブジェクトに先行するストローク／ペイントオブジェクトが存在していても、
//   スロット呼び出しがレイヤー[A]の登録に先行されるため、BDPD[B]の各オブジェクト表示が優先されることになる）
//（※オブジェクトの微妙な表示順が気になる場合は、スロット呼び出し専用のレイヤーを作るのが無難）
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
    //（※ここで保持した値を後で傾けて基本座標とフォーカス座標が決定される）
    //--------------------------------------------------------
    // 基本座標（※左右の足元）
    if( slot == eBD_SLOT_FoGround ){
        if( slotIndex == 1 ){
            s_fTexGroundX0_R = rop.baseX;
            s_fTexGroundY0_R = rop.baseY;
        }else{
            s_fTexGroundX0_L = rop.baseX;
            s_fTexGroundY0_L = rop.baseY;
        }
    }
    // フォーカス座標（※顔の中心＝鼻）
    else if( slot == eBD_SLOT_FcNose ){
        s_fTexFocusX0 = rop.baseX;
        s_fTexFocusY0 = rop.baseY;
    }

    //-----------------------------------------------------
    // 対象スロットの生成パラメータ検索
    //-----------------------------------------------------
    stBMP_GEN_PARAM_CELL* pBGPCell = pGenParam->searchCell( slot, slotIndex );

    // 無効は無視（※登録が無い）
    if( pBGPCell == NULL ){
        //LOGE( "@ setOrderListForBmpGen: NO PARAMETER FOR SLOT: slot=%s(%d), slotIndex=%d\n",
        //      g_pArrLabelBdSlot[slot], slot, slotIndex );
        return;
    }
    
    // 参照でなければ
    if( ! isRef ){
        // 処理済みであれば無視（※[SlotA]→[SlotB]→[SLotA]→[SlotB]...のような循環をしてしまわないよう用心）
        if( pBGPCell->isDone ){
#ifdef BMP_GEN_LOG
            LOGD( "@ setOrderListForBmpGen: ALREADY DONE SLOT: slot=%d, slotIndex=%d\n", slot, slotIndex );
#endif
            return;
        }
        
        // ここまできたら処理済みフラグを立てておく
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
    
    // アジャスタ
    CSlotAdjuster* pSA = pBDPD->getSlotAdjuster();
    
    //--------------------------------------------
    // 回転（※以降のスロットにも影響）
    //--------------------------------------------
    float applyRot = 0;
    
    // テンション調整による補正
    int rateRot;
    CAdjustCalc::CalcRateRotForT( &rateRot, pSA, pBGPCell->adjForT );
    applyRot += CConst::ConvBezierRotationRate( rateRot );

    // 参照による補正
    if( isRef ){
        applyRot += pBGPCell->refRot;
    }
    
    // ここはオフセットの足しこみのみ（※フリップによる回転方向の反転のみを加味する）
    if( rop.isFlipH != rop.isFlipV ){ applyRot *= -1; }
    rop.rot += applyRot;
   
    // 回転補正
    float powRate = pBDPD->getRotPow();
    rop.rot = s_stBaseParam.rot + powRate*(rop.rot-s_stBaseParam.rot);
    applyRot *= powRate;

    //--------------------------------------------
    // 拡縮（※以降のスロットにも影響）
    //--------------------------------------------
    float applyScale = 1.0f;
    
    // 参照による補正
    if( isRef ){
        applyScale *= pBGPCell->refScale;
    }

    rop.scale *= applyScale;
    
    //--------------------------------------------
    // 拡縮（※このパーツにのみ影響)
    //--------------------------------------------
    float bdpdScale = 1.0f;

    // サイズ調整による補正
    int rateScale;
    CAdjustCalc::CalcRateScaleForS( &rateScale, pSA, pBGPCell->adjForS );
    bdpdScale *= CConst::ConvBezierRateScale( rateScale );

    // 作業スケールによる補正
    if( rop.isWorkScale ){
        bdpdScale *= pBDPD->getScaleForWork();
    }

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
        CBezier::BasePointForDev( rop.baseX, rop.baseY, pBaseParam, isSelected, isInvalid );
    }

    //-----------------------------------------------
    // 拡縮が確定したら結合点の登録
    //-----------------------------------------------
    RegistJointPoint( pBaseParam, pBGPCell, pSA, slotIndex,
                      rop.isFlipH, rop.isFlipV, applyRot, applyScale, bdpdScale, rop.rateOfsX, rop.rateOfsY );
    
    //-----------------------------------------------
    // [CBmpDotPartData]内のレイヤーを走査
    //-----------------------------------------------
    CLayerData* pLD = pBDPD->getDataHead();
    while( pLD != NULL ){
        // 無効は無視
        if( pLD->checkFlag( eLAYER_FLAG_DISABLE ) ){
            // 次のレイヤーへ
            pLD = pLD->getNext();
            continue;
        }
        
        // オプション設定のレイヤーで指定が無い場合は無視
        eBD_OPTION option = pLD->getOption();
        if( IS_BD_OPTION_VALID( option ) ){
            if( ! pBaseParam->arrOption[option] ){
                // 次のレイヤーへ
                pLD = pLD->getNext();
                continue;
            }
        }
       
        // 遅延によるレイヤーの回転
        int rateRot;
        bool isReverseH = false;
        bool isReverseV = (slotIndex%2)!=0; // 対の要素（おさげの片方等）であれば縦反転
        CAdjustCalc::CalcRateRotForDelayLog( &rateRot, isReverseH, isReverseV, pLD, pBaseParam->pDelayLog );
        float rotByDelay = CConst::ConvBezierRotationRate( rateRot );

        // レイヤー内部のオブジェクトを走査
        CLayerObject* pLO = pLD->getDataHead();
        while( pLO != NULL ){
            // スロットであれば解釈（※編集調整中は無視／参照時は指定がなければ無視）
            if( pLO->isSlotObject() && !rop.isSlotIgnore && (!isRef || pBGPCell->isRefSlotValid) ){
                CSlotObjectData* pSOD = pLO->getSlotObject();
                
                // 有効であればスロット対象の登録
                if( ! pSOD->checkFlag( eSOD_FLAG_DISABLE ) ){
                    // レイヤーパラメータの設定
                    stBEZIER_LAYER_PARAM stLayerParam;
                    CLEAR_BEZIER_LAYER_PARAM( &stLayerParam );
                    stLayerParam.pixelOfsX = rop.baseX;
                    stLayerParam.pixelOfsY = rop.baseY;
                    stLayerParam.pixelScaleX = bdpdScaleX;
                    stLayerParam.pixelScaleY = bdpdScaleY;
                    stLayerParam.pixelRot = rop.rot + rotByDelay;
                    stLayerParam.fixedAdjustRateH = pBaseParam->adjustRateH;
                    stLayerParam.fixedAdjustRateV = pBaseParam->adjustRateV;

                    // スロットポイントの変換（※有効なポイントがリスト形式で返される）
                    //（※[stLayerParam]で渡したパラメータに対し、各スロットの調整座標が加味された値が返される）
                    stBEZIER_SLOT_POINT* pBSPNodeHead;
                    pBSPNodeHead = CBezierDataConv::CreateBezierSlotPoint( pSOD, pBaseParam, &stLayerParam );
                    
                    // 各スロットポイントをオーダーリストへ登録
                    stBEZIER_SLOT_POINT* pBSPNode = pBSPNodeHead;
                    while( pBSPNode != NULL ){
                        // スロットインデックスが負であれば呼び出し元に依存
                        int slotId = pBSPNode->slotIndex;
                        if( slotId < 0 ){ slotId = slotIndex; }
                        
                        stREGIST_ORDER_PARAM tmpRop;
                        CLEAR_REGIST_ORDER_PARAM( &tmpRop );

                        // オーダー登録パラメータの設定
                        tmpRop.isFlipH = pBSPNode->isFlipH ^ rop.isFlipH;
                        tmpRop.isFlipV = pBSPNode->isFlipV ^ rop.isFlipV;;
                        tmpRop.baseX = pBSPNode->x;
                        tmpRop.baseY = pBSPNode->y;
                        tmpRop.rateOfsX = pBSPNode->rateOfsX;
                        tmpRop.rateOfsY = pBSPNode->rateOfsY;
                        tmpRop.rot = rop.rot + rotByDelay;
                        tmpRop.scale = rop.scale;

                        // オーダー登録
                        SetOrderList( pOrderList, pGenParam, pBSPNode->slot, slotId, false, pBaseParam, tmpRop, isWorkPath );
                        
                        // 次へ
                        pBSPNode = (stBEZIER_SLOT_POINT*) pBSPNode->pNext;
                    }
                    
                    // 解放
                    CBezierDataConv::ReleaseBezierSlotPoint( pBSPNodeHead );
                }
            }

            // 次のオブジェクトへ
            pLO = pLO->getNext();
        }
        
        //------------------
        // レイヤーの登録
        //------------------
        if( !isRef || pBGPCell->isRefDrawValid ){
            eBD_SLOT orderSlot = pLD->getOrderSlot();

            // デフォルト時であればオーダー対象調整
            if( orderSlot == eBD_SLOT_DEFAULT ){
                orderSlot = slot;
            }

            int orderSlotIndex = CBDConst::FixIndexForSlot( orderSlot, slotIndex );

#ifdef BMP_GEN_LOG
            LOGD( "@ [ADD:%s]: slot=%s[%d](%s[%d]), index=%d(%d), fH=%d, fV=%d\n",
                  pLD->getName(),
                  g_pArrLabelBdSlot[orderSlot], orderSlot,
                  g_pArrLabelBdSlot[slot], slot,
                  orderSlotIndex, slotIndex,
                  isFlipH, isFlipV );
#endif
                        
            //--------------------------------------------------
            // オーダーリストへ登録（※これでレイヤーが表示されることになる）
            //--------------------------------------------------
            // 制御情報
            bool isTargetDone = false;
            bool isCoverOpenRequired = pLD->checkFlag( eLAYER_FLAG_OPEN_COVER );
            bool isCoverCloseRequired = pLD->checkFlag( eLAYER_FLAG_CLOSE_COVER );
            int coverOpenLayerAt = 0;
            int coverCloseLayerAt = 0;
            eLAYER_ORDER_TYPE orderType = pLD->getOrderType();  // オーダータイプは固定（※カバーレイヤーが対象レイヤーの前後に登録されるように）

            // ワーク
            CLayerData* pCurLayer;
            float adjRotForLayer;
            float adjScaleForLayer;
            while( true ){
                pCurLayer = NULL;
                adjRotForLayer = 0.0f;
                adjScaleForLayer = 1.0f;
                
                // カバー開始要求があれば
                if( isCoverOpenRequired ){
                    if( GetCoverLayerAt( pGenParam, slot, slotIndex, false, coverOpenLayerAt, &pCurLayer, &adjRotForLayer, &adjScaleForLayer ) ){
                        coverOpenLayerAt++;
                    }
                }

                // レイヤーが未設定なら対象の設定
                if( pCurLayer==NULL && !isTargetDone ){
                    pCurLayer = pLD;
                    isTargetDone = true;
                }

                // カバー終了要求があれば
                if( pCurLayer==NULL && isCoverCloseRequired ){
                    if( GetCoverLayerAt( pGenParam, slot, slotIndex, true, coverCloseLayerAt, &pCurLayer, &adjRotForLayer, &adjScaleForLayer ) ){
                        coverCloseLayerAt++;
                    }
                }
                
                // この時点で対象が無効なら終了
                if( pCurLayer == NULL ){
                    break;
                }
                
                // レイヤーの登録（※返値が登録内容となる）
                stLAYER_ORDER_CELL_NODE* pNode = pOrderList->addLayerData( pCurLayer, orderType, orderSlot, orderSlotIndex );
                
                // 登録が成功したらレイヤーに対する表示情報の設定
                if( pNode != NULL ){
                    // カバーの角度調整
                    if( rop.isFlipH != rop.isFlipV ){
                        adjRotForLayer *= -1;
                    }
                    
                    // レイヤーパラメータの設定（※この値は枠だけ確保された状態なので登録側で設定する必要がある）
                    stBEZIER_LAYER_PARAM* pLP = &pNode->layerParam;
                    CLEAR_BEZIER_LAYER_PARAM( pLP );
                    
                    // スロット（※カバーのレイヤーであっても呼び出し元のスロットとして登録）
                    pLP->slot = slot;
                    pLP->slotIndex = slotIndex;

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
                    if( pBDPD->checkFlag( eBDPD_FLAG_ANGLE_FLIP_H ) ){
                        pLP->fixedAngleRateLR *= -1;
                    }

                    // 傾き：上下
                    pLP->fixedAngleRateUD = pBaseParam->adjustRateUD;
                    FIX_ADJUST_RATE( pLP->fixedAngleRateUD );
                    if( pBDPD->checkFlag( eBDPD_FLAG_ANGLE_FLIP_V ) ){
                        pLP->fixedAngleRateUD *= -1;
                    }

                    // 位置／拡縮
                    pLP->pixelOfsX = rop.baseX;
                    pLP->pixelOfsY = rop.baseY;
                    pLP->pixelScaleX = bdpdScaleX * adjScaleForLayer;
                    pLP->pixelScaleY = bdpdScaleY * adjScaleForLayer;
                    pLP->pixelRot = rop.rot + rotByDelay + adjRotForLayer;
                    
                    // 結合ポイント算出用にBDP関連情報を保持
                    pLP->scaleForBdpd = bdpdScale * adjScaleForLayer;
                    pLP->rateXForBdpd = -rop.rateOfsX;
                    pLP->rateYForBdpd = -rop.rateOfsY;

                    // 作業表示指定時は作業スケールをストロークサイズへ反映させる
                    //（※出力サイズが変わるのでストロークの太さを合わせて見た目の印象がずれないようにする）
                    if( rop.isWorkScale ){
                        pLP->strokeScale = pBDPD->getScaleForWork();
                    }else{
                        pLP->strokeScale = 1.0f;
                    }
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

//------------------
// カバー取得
//------------------
bool CBmpGenerator::GetCoverLayerAt( CBmpGenParam* pGenParam, eBD_SLOT parentSlot, int parentSlotIndex, bool isClose, int at,
                                     CLayerData** ppOutLayer, float* pBaseRot, float* pBaseScale ){
    //-----------------------------------------------------
    // カバー素材の検索
    //-----------------------------------------------------
    // 検索対象：ジョイント
    eBD_SLOT targetSlot = eBD_SLOT_INVALID;
    if( parentSlot == eBD_SLOT_HdBase ){
        targetSlot = eBD_SLOT_CvHead;
    }else if( parentSlot == eBD_SLOT_AmBase ){
        targetSlot = eBD_SLOT_CvArm;
    }else if( parentSlot == eBD_SLOT_LgBase ){
        targetSlot = eBD_SLOT_CvLeg;
    }
    
    // 検索対象が無効なら失敗
    if( targetSlot == eBD_SLOT_INVALID ){
        // ここに来るのは何か変（※カバー要請に対応していないスロットから呼ばれた）
        LOGE( "@ CBmpGenerator::GetCoverLayerAt: INVALID parentSlot=%d, parentSlotIndex=%d\n", parentSlot, parentSlotIndex );
        return( false );
    }

    // セルの取得（※みつからなければ終了＝登録がないケースはありうる）
    stBMP_GEN_PARAM_CELL* pCell = pGenParam->searchCell( targetSlot, parentSlotIndex );
    if( pCell == NULL ){
        return( false );
    }
    
    // カバーBDPDの取得（※夢応であれば終了＝登録がないケースはありうる）
    CBmpDotPartData* pBdpd = pCell->pData;
    if( pBdpd == NULL ){
        return( false );
    }
        
    // ここまできたらレイヤーデータ内部を操作
    // レイヤー内部のオブジェクトを走査
    CLayerData* pLD = pBdpd->getDataHead();
    while( pLD != NULL ){
        if( ! pLD->checkFlag( eLAYER_FLAG_DISABLE) ){
            // オーダータイプにより取得先の判定
            eLAYER_ORDER_TYPE order = pLD->getOrderType();
            if( isClose ){
                // クローズ要素であれば[BACK]を対象とする
                if( order == eLAYER_ORDER_TYPE_BACK ){
                    if( at <= 0 ){
                        break;
                    }
                    at--;
                }
            }else{
                // オープン要素であれば[FRONT/NORMAL]を対象とする
                if( order == eLAYER_ORDER_TYPE_FRONT || order == eLAYER_ORDER_TYPE_NORMAL ){
                    if( at <= 0 ){
                        break;
                    }
                    at--;
                }
            }
        }
        
        // 次のオブジェクトへ
        pLD = pLD->getNext();
    }
    
    // 対象がなければ終了
    if( pLD == NULL ){
        return( false );
    }
    
    //-----------------------------------------------------
    // 基本角度の抽出（※ジョイントで管理されるパーツは角度を持つ）
    //-----------------------------------------------------
    float rotBase = 0.0f;
    float scaleBase = 1.0f;
    float baseR = 250;

    // 検索対象：ジョイント
    targetSlot = eBD_SLOT_INVALID;
    if( parentSlot == eBD_SLOT_HdBase ){
        // 頭にはジョイントがない
    }else if( parentSlot == eBD_SLOT_AmBase ){
        targetSlot = eBD_SLOT_UpJointSholder;
        rotBase = 30;  // 基準となる角度（やや外を向かせる）
        baseR = 250;    // 基準となる穴の半径
    }else if( parentSlot == eBD_SLOT_LgBase ){
        targetSlot = eBD_SLOT_LwJointLeg;
        rotBase = 10;  // 基準となる角度（やや外を向かせる）
        baseR = 400;    // 基準となる足の半径
    }

    if( targetSlot != eBD_SLOT_INVALID ){
        pCell = pGenParam->searchCell( targetSlot, parentSlotIndex );

        if( pCell != NULL ){
             pBdpd = pCell->pData;
            if( pBdpd != NULL ){
                CSlotAdjuster* pSA = pBdpd->getSlotAdjuster();
                rotBase += CConst::ConvBezierRotationRate( pSA->getJointRateRotOfs() );
                scaleBase *= pSA->getJointRateRotR()/baseR;
            }
        }
    }

    // ここまできたら出力（成功）
    *ppOutLayer = pLD;
    *pBaseRot = rotBase;
    *pBaseScale = scaleBase;
    return( true );
}
