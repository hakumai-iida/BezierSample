/*+----------------------------------------------------------------+
  |	Title:		BmpGeneratorForSetting.cpp [共通環境]
  |	Comment:	Bmp生成：設定関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenerator.hpp"

#include "draw/bezier/adjust/AdjustCalc.hpp"


/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// デフォルトID
#define DEFAULT_FACE_UID    100
#define DEFAULT_BODY_UID    100
#define DEFAULT_SUIT_UID    100

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
//-----------------------------------------------------------
// [SETTING] 設定データから作成パラメータの設定（※全体を補正するデータ）
//-----------------------------------------------------------
bool CBmpGenerator::SetBmpGenCreateParam( stBMP_GEN_CREATE_PARAM* pCreateParam, CBmpDotSettingData* pSetting, CDelayLog* pDelayLog ){
    // 無効は無視（※[pDelayLog]は[NULL]を許可する＝遅延処理が無視される）
    if( pCreateParam == NULL || pSetting == NULL /*|| pDelayLog == NULL */){
        LOGE( "@ CBmpGenerator::SetBmpGenCreateParam: INVALID PARAM\n" );
        return( false );
    }
       
    // 用心にクリア
    CLEAR_BMP_GEN_CREATE_PARAM( pCreateParam );

    pCreateParam->isFlipH = pSetting->isFlipH();
    pCreateParam->isFlipV = pSetting->isFlipV();
    pCreateParam->scale = pSetting->getScale();
    pCreateParam->rot = pSetting->getRot();
    
    pCreateParam->adjustRateH = pSetting->getAdjustH();
    pCreateParam->adjustRateV = pSetting->getAdjustV();
    pCreateParam->adjustRateLR = pSetting->getAngleLR();
    pCreateParam->adjustRateUD = pSetting->getAngleUD();
    
    pCreateParam->pDelayLog = pDelayLog;

    // オプションの設定
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            pCreateParam->arrOption[i][j] = pSetting->isOptionValid( (eBD_OPTION)i, j );
        }
    }
    
    // スロットオプションの設定
    for( int i=0; i<eBD_SLOT_MAX; i++ ){
        for( int j=0; j<BD_SLOT_INDEX_MAX; j++ ){
            for( int k=0; k<BD_OPTION_FOR_SLOT_MAX; k++ ){
                pCreateParam->arrOptionForSlot[i][j][k] = pSetting->isOptionForSlotValidAt( (eBD_SLOT)i, j, k );
            }
        }
    }

    return( true );
}

//------------------------------------------------------------------
// [SETTING] BMP生成パラメータの設定（※BMPを構成する個々のパーツに対する設定）
//------------------------------------------------------------------
bool CBmpGenerator::SetBmpGenParam( CBmpGenParam* pGenParam, CBmpDotSettingData* pSetting ){
    // 無効は無視
    if( pGenParam == NULL || pSetting == NULL ){
        LOGE( "@ CBmpGenerator::CreateBmpGenParam: INVALID PARAM\n" );
        return( false );
    }
    
    // 用心にクリア
    pGenParam->clear();

    // 基本データ取り出し
    int idForFace = pSetting->getIdForFace();
    int idForBody = pSetting->getIdForBody();
    int idForSuit = pSetting->getIdForSuit();
    eBD_EMO emo = pSetting->getEmo();
    eBD_FORM form = pSetting->getForm();
    float adjForT = pSetting->getAdjustT();
    float adjForS = pSetting->getAdjustS();

    //----------------------------------------------
    // 対象の設定
    //----------------------------------------------
    int defIdForFace = BD_UID_INVALID;
    int defIdForBody = BD_UID_INVALID;
    int defIdForSuit = BD_UID_INVALID;
    eBD_EMO defEmo = eBD_EMO_INVALID;
    eBD_FORM defForm = eBD_FORM_INVALID;

    defIdForFace = pSetting->getIdForFace();
    defIdForBody = pSetting->getIdForBody();
    defIdForSuit = pSetting->getIdForSuit();
    defEmo = pSetting->getEmo();
    defForm = pSetting->getForm();
    
    // 引数が有効であれば指定の差し替え
    if( IS_BD_UID_VALID( idForFace ) ){ defIdForFace = idForFace; }
    if( IS_BD_UID_VALID( idForBody ) ){ defIdForBody = idForBody; }
    if( IS_BD_UID_VALID( idForSuit ) ){ defIdForSuit = idForSuit; }
    if( IS_BD_EMO_VALID( emo ) ){ defEmo = emo; }
    if( IS_BD_FORM_VALID( form ) ){ defForm = form; }
    
    // スーツ例外（※スーツが指定された場合、素材が歯抜けにならないように顔と体に値を設定しておく）
    bool isSuited = pSetting->isSuited();
    if( IS_BD_UID_VALID( defIdForSuit ) && pSetting->isUseDefaultForSuit() ){
        if( ! IS_BD_UID_VALID( idForFace ) ){ defIdForFace = DEFAULT_FACE_UID; }
        if( ! IS_BD_UID_VALID( idForBody ) ){ defIdForBody = DEFAULT_BODY_UID; }
        isSuited = true;
    }
    
    // 補助情報の設定（※BMP生成処理のオーダーリスト構築時に利用される）
    pGenParam->setForm( defForm );
    pGenParam->setSuited( isSuited );

    // 上半身／下半身の差し替えID
    int replacedIdForUp = pSetting->getReplacedIdForUp();
    int replacedIdForLow = pSetting->getReplacedIdForLow();

    //--------------------------------------
    // オーダーのエントリからパーツを検索して登録
    //--------------------------------------
    CBmpDotOrderData order;
    order.setDefault( defForm, isSuited );

    float tempT, tempS;
    stBD_PART_SEARCH_PARAM param;
    param.suitUid = defIdForSuit;
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        // セルの取得（※無効は無視）
        stBMP_DOT_ORDER_DATA_CELL* pCell = order.getCellAt( i );
        if( pCell == NULL ){ continue; }
        if( ! IS_BD_SLOT_VALID( pCell->slot ) ){ continue; }
        if( ! IS_BD_SLOT_INDEX_VALID( pCell->slotIndex ) ){ continue; }

        // スロット設定の検索（※無効は無視）
        param.slot = pCell->slot;
        CBmpDotSettingSlotData* pSlot = pSetting->searchSlot( pCell->slot, pCell->slotIndex );
        if( pSlot == NULL || pSlot->isInvalid() ){ continue; }

        // スロット指定からパラメータの設定
        param.category = pSlot->getCategory();
        param.uid = pSlot->getUid();
        param.emo = pSlot->getEmo();
        param.form = pSlot->getForm();
        param.subId = pSlot->getSubId();
        param.dir = pSlot->getDir();
        
        // スーツ時は特殊なサブID[99]の腕／脚を探す（※着ぐるみ用に角度をつけて別素材が存在する可能性がある）
        if( isSuited ){
            if( param.slot == eBD_SLOT_AmBase || param.slot == eBD_SLOT_LgBase ){
                param.category = eBD_CATEGORY_BODY;
                param.subId = BD_SUB_ID_FOR_SUITED;
            }
        }
        
        tempT = pSlot->getAdjForT() + adjForT;
        tempS = pSlot->getAdjForS() + adjForS;

        // カテゴリが無効ならスロットに準ずる
        if( ! IS_BD_CATEGORY_VALID( param.category ) ){
            // トップの指定であれば調整（※有効なIDにより参照先を変える）
            if( pCell->slot == eBD_SLOT_FgTop ){
                if( IS_BD_UID_VALID( defIdForSuit ) ){
                    param.category = eBD_CATEGORY_SUIT;
                }else if( IS_BD_UID_VALID( defIdForBody ) ){
                    param.category = eBD_CATEGORY_BODY;
                }else/* if( IS_BD_UID_VALID( defIdForFace ) )*/{
                    param.category = eBD_CATEGORY_FACE;
                }
            }else{
                param.category = CBDConst::GetCategoryForSlot( pCell->slot );
                param.uid = BD_UID_INVALID; // 用心（※カテゴリが上書きされたので無効化）
            }
        }
        
        // UIDの確認（※無効であればデフォルトへ問い合わせる）
        if( ! IS_BD_UID_VALID( param.uid ) ){
            // TODO:各UIDの値域
            if( param.category == eBD_CATEGORY_SUIT ){
                param.uid = defIdForSuit;
            }else if( param.category == eBD_CATEGORY_BODY ){
                param.uid = defIdForBody;
            }else/* if( param.category == eBD_CATEGORY_FACE )*/{
                param.uid = defIdForFace;
            }
            
            // 指定があれば上半身を差し替え
            if( CBDConst::IsSlotBodyUp( param.slot ) ){
                if( IS_BD_UID_VALID( replacedIdForUp ) ){
                    param.uid = replacedIdForUp;
                }
            }
            // 指定があれば下半身を差し替え
            else if( CBDConst::IsSlotBodyLow( param.slot ) ){
                if( IS_BD_UID_VALID( replacedIdForLow ) ){
                    param.uid = replacedIdForLow;
                }
            }
        }
        
        // 感情の確認（※無効であればデフォルトへ問い合わせる）
        if( ! IS_BD_EMO_VALID( param.emo ) ){
            param.emo = defEmo;
        }

        // 姿形の確認（※無効であればデフォルトへ問い合わせる）
        if( ! IS_BD_FORM_VALID( param.form ) ){
            param.form = defForm;
        }
        
        // サブIDの確認（※無効であればデフォルトへ問い合わせる）
        if( ! IS_BD_SUB_ID_VALID( param.subId ) ){
            param.subId = BD_SUB_ID_MIN;
        }

        // 方向の確認（※無効であればデフォルトへ問い合わせる）
        if( ! IS_BD_DIR_VALID( param.dir ) ){
            param.dir = GET_BD_DIR_DEFAULT_FOR_SLOT_INDEX( pCell->slotIndex );
        }

        // 登録（※指定のデータが存在しない場合もある＝ポニーテール枠に対してポニーテールパーツの指定がない等）
        if( pGenParam->addCell( &param, pCell->slotIndex, tempT, tempS ) == NULL ){
#ifdef BMP_GEN_LOG
            LOGD( "@ [x] ADD CELL( %s, %d, %d);\n", g_pArrLabelBdSlot[param.slot], param.subId, param.dir );
#endif
        }else{
#ifdef BMP_GEN_LOG
            LOGD( "@ [o] ADD CELL( %s, %d, %d);\n", g_pArrLabelBdSlot[param.slot], param.subId, param.dir );
#endif
        }
    }
        
    //--------------------------------------
    // 無視設定（※頭関連を事前に判定）
    //--------------------------------------
    // 帽子が表示されるか？
    bool isCapVisible = CheckTargetPartVisible( pGenParam, eBD_SLOT_HdCap, 0 );
    if( CheckTargetPartHidden( pGenParam, eBD_SLOT_HdCap, 0 ) ){
        isCapVisible = false;
    }
    
    // 帽子が有効なら頭上を無視する（※帽子と頭上は共存できない）
    if( isCapVisible ){
        if( SetIgnorePart( pGenParam, eBD_SLOT_HdTop, 0 ) ){
#ifdef BMP_GEN_LOG
            LOGD( "@ HIDE: eBD_SLOT_HdTop[0]\n" );
#endif
        }
    }
    
    // 前髪の無視判定を事前に行う（※目やまゆげを隠す可能性があるので）
    if( CheckTargetPartHidden( pGenParam, eBD_SLOT_HdFront, 0 ) ){
        if( SetIgnorePart( pGenParam, eBD_SLOT_HdFront, 0 ) ){
#ifdef BMP_GEN_LOG
            LOGD( "@ HIDE: eBD_SLOT_HdFront[0]\n" );
#endif
        }
    }
    
    // はみ出す髪はフラグを指定する（※スロットのオン／オフですまないので）
    if( CheckTargetPartFlag( pGenParam, eBDPD_FLAG_HIDE_OVER_HAIR ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ HIDE: OVER HAIR\n" );
#endif
        pGenParam->setHideOverHair( true );
    }
    
    //----------------------------------------------------------------------------------------------
    // その他の無視設定（※無視設定をもつパーツは基本的にカバー系のみを想定する）
    // [xxBase]等の通常パーツが無視設定をもってしまうと相互に影響しあって予期せぬ挙動をする可能性があるので注意すること
    //----------------------------------------------------------------------------------------------
    int numCell = pGenParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* arrCell = pGenParam->getCellArr();
    for( int i=0; i<numCell; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &arrCell[i];
        
        // 無効は無視
        if( pCell->isIgnore ){
            continue;
        }
        
        // 指定があれば無視フラグを立てる
        if( CheckTargetPartHidden( pGenParam, pCell->slot, pCell->slotIndex ) ){
            pCell->isIgnore = true;
#ifdef BMP_GEN_LOG
            LOGD( "@ HIDE: eBD_SLOT_%s[%d]\n", g_pArrLabelBdSlot[pCell->slot], pCell->slotIndex );
#endif
        }
    }

    //--------------------------------------
    // カバーフラグの設定
    //--------------------------------------
    // 首カバー
    if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_NkCover, eBD_SLOT_NkBase ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ COVERED: eBD_SLOT_NkBase\n" );
#endif
    }
    
    // 頭カバー
    if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_HdTopCover, eBD_SLOT_HdBase ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ COVERED: eBD_SLOT_HdBase\n" );
#endif
    }
    
    // 顔カバー
    if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_FcUnderCover, eBD_SLOT_FcBase ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ COVERED: eBD_SLOT_FcBase\n" );
#endif
    }
    
    // 腕カバー
    if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_AmCover, eBD_SLOT_AmBase ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ COVERED: eBD_SLOT_AmBase\n" );
#endif
    }
    
    // 脚カバー
    if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_LgCover, eBD_SLOT_LgBase ) ){
#ifdef BMP_GEN_LOG
        LOGD( "@ COVERED: eBD_SLOT_LgBase\n" );
#endif
    }
    
    // 体（※スーツ時は対応するパーツがあればカバー扱い）
    if( isSuited ){
        if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_BlBase, eBD_SLOT_BlBase ) ){
#ifdef BMP_GEN_LOG
            LOGD( "@ COVERED: eBD_SLOT_BlBase\n" );
#endif
        }
        
        if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_UpBase, eBD_SLOT_UpBase ) ){
#ifdef BMP_GEN_LOG
            LOGD( "@ COVERED: eBD_SLOT_UpBase\n" );
#endif
        }

        if( CheckCovereThenSetFlag( pGenParam, eBD_SLOT_LwBase, eBD_SLOT_LwBase ) ){
#ifdef BMP_GEN_LOG
            LOGD( "@ COVERED: eBD_SLOT_LwBase\n" );
#endif
        }
    }

    return( true );
}

//-----------------------------------------
// 対象のパーツが表示されるか？
//-----------------------------------------
bool CBmpGenerator::CheckTargetPartVisible( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex ){
    // 用心
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::CheckTargetPartVisible: INVALID pGenParam\n" );
        return( false );
    }

    int numCell = pGenParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* arrCell = pGenParam->getCellArr();
    for( int i=0; i<numCell; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &arrCell[i];

        // 無効は無視
        if( pCell->isIgnore ){
            continue;
        }
        
        // スロットインデックスが合わなければ無視
        if( pCell->slotIndex != targetSlotIndex ){
            continue;
        }

        // データが有効なら
        if( pCell->pData != NULL ){
            if( pCell->slot == targetSlot ){
                return( true );
            }
        }
        
        // 参照データの表示が有効なら
        if( pCell->pDataRef != NULL && pCell->isRefDrawValid ){
            if( pCell->slot == targetSlot ){
                return( true );
            }
        }
    }

    // ここまできたら未検出
    return( false );
}

//-----------------------------------------
// 対象のパーツが隠されるか？
//-----------------------------------------
bool CBmpGenerator::CheckTargetPartHidden( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex ){
    // 用心
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::CheckTargetPartHidden: INVALID pGenParam\n" );
        return( false );
    }

    int numCell = pGenParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* arrCell = pGenParam->getCellArr();
    for( int i=0; i<numCell; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &arrCell[i];
        
        // 無効は無視
        if( pCell->isIgnore ){
            continue;
        }

        int slotIndex = pCell->slotIndex;

        // データが有効なら
        if( pCell->pData != NULL ){
            eBD_SLOT* pArrHideSlot;

            if( slotIndex == targetSlotIndex ){
                pArrHideSlot = pCell->pData->getArrHideSlot();
            }else{
                pArrHideSlot = pCell->pData->getArrHideSlotOposite();
            }
            
            for( int j=0; j<BDPD_HIDE_SLOT_NUM; j++ ){
                if( targetSlot == pArrHideSlot[j] ){
                    return( true );
                }
            }
        }
        
        // 参照データの表示が有効なら
        if( pCell->pDataRef != NULL && pCell->isRefDrawValid ){
            eBD_SLOT* pArrHideSlot;

            if( slotIndex == targetSlotIndex ){
                pArrHideSlot = pCell->pDataRef->getArrHideSlot();
            }else{
                pArrHideSlot = pCell->pDataRef->getArrHideSlotOposite();
            }
            
            for( int j=0; j<BDPD_HIDE_SLOT_NUM; j++ ){
                if( targetSlot == pArrHideSlot[j] ){
                    return( true );
                }
            }
        }
    }

    // ここまできたら未検出
    return( false );
}

//-----------------------------------------
// パーツのフラグ判定
//-----------------------------------------
bool CBmpGenerator::CheckTargetPartFlag( CBmpGenParam* pGenParam, eBDPD_FLAG targetFlag ){
    // 用心
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::CheckTargetPartFlag: INVALID pGenParam\n" );
        return( false );
    }

    int numCell = pGenParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* arrCell = pGenParam->getCellArr();
    for( int i=0; i<numCell; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &arrCell[i];

        // 無視されていたら無視
        if( pCell->isIgnore ){
            continue;
        }

        // データが有効なら
        if( pCell->pData != NULL ){
            if( pCell->pData->checkFlag( targetFlag ) ){
                return( true );
            }
        }
        
        // 参照データの表示が有効なら
        if( pCell->pDataRef != NULL && pCell->isRefDrawValid ){
            if( pCell->pDataRef->checkFlag( targetFlag ) ){
                return( true );
            }
        }
    }

    // ここまできたら未検出
    return( false );
}

//-----------------------------------------
// パーツの無視設定
//-----------------------------------------
bool CBmpGenerator::SetIgnorePart( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex ){
    // 用心
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::SetIgnorePart: INVALID pGenParam\n" );
        return( false );
    }

    stBMP_GEN_PARAM_CELL* pCell = pGenParam->searchCell( targetSlot, targetSlotIndex );
    if( pCell != NULL ){
        pCell->isIgnore = true;
        return( true );
    }
    
    return( false );
}

//-----------------------------------------
// カバーBDPDの判定と対応パーツのカバーフラグの設定
//-----------------------------------------
bool CBmpGenerator::CheckCovereThenSetFlag( CBmpGenParam* pGenParam, eBD_SLOT coverSlot, eBD_SLOT parentSlot ){
    // 用心
    if( pGenParam == NULL ){
        LOGE( "@ CBmpGenerator::CheckCovereThenSetFlag: INVALID pGenParam\n" );
        return( false );
    }

    // 対象素材の検索（※なければ返す）
    // ここでの判定は常にスロットインデック０（左）見る（※腕／脚のカバーは常に両側を覆う想定）
    stBMP_GEN_PARAM_CELL* pCell = pGenParam->searchCell( coverSlot, 0 );
    if( pCell == NULL || pCell->pData == NULL ){
        return( false );
    }
    
    // 全ての要素に対して判定
    int numCell = pGenParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* arrCell = pGenParam->getCellArr();
    for( int i=0; i<numCell; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &arrCell[i];
        
        // 対象の親が指定スロットであればカバーされている
        if( CBDConst::GetParentSlot( pCell->slot ) == parentSlot ){
            pCell->isCovered = true;
        }
    }
    
    return( true );
}

//------------------------
// [SETTING] カバーBDPD取得
//------------------------
CBmpDotPartData* CBmpGenerator::CheckThenGetCoverBDPDAndInfo( CBmpGenParam* pGenParam, CBmpDotPartData* pParentData, int parentSlotIndex,
                                                              int* pRateOfsX, int* pRateOfsY, float* pScaleX, float* pScaleY, float* pRot ){
    // 無効は無視
    if( pGenParam == NULL || pParentData == NULL ){
        LOGE( "@ CBmpGenerator::CheckThenGetCoverBDPDAndInfo: INVALID pGenParam=%p, pParentData=%p\n", pGenParam, pParentData );
        return( NULL );
    }
    
    //------------------------
    // 対応スロットの検出
    //------------------------
    eBD_SLOT parentSlot = pParentData->getSlot();
    eBD_SLOT targetSlot = eBD_SLOT_INVALID;
    float jointBaseR = 0.0f;

    switch( parentSlot ){
    // 首
    case eBD_SLOT_NkBase:
        targetSlot = eBD_SLOT_NkCover;
        jointBaseR = 500;
        break;
            
    // 頭上（※[HdTop/HdCap]の両方を受け付ける）
    case eBD_SLOT_HdTop:
    case eBD_SLOT_HdCap:
        targetSlot = eBD_SLOT_HdTopCover;
        jointBaseR = 1400;
        break;

    // 顔下
    case eBD_SLOT_FcUnder:
        targetSlot = eBD_SLOT_FcUnderCover;
        jointBaseR = 1300;
        break;

    // 腕
    case eBD_SLOT_AmBase:
        targetSlot = eBD_SLOT_AmCover;
        jointBaseR = 400;
        break;
            
    // 脚
    case eBD_SLOT_LgBase:
        targetSlot = eBD_SLOT_LgCover;
        jointBaseR = 500;
        break;
            
    default:
        break;
    }
    
    // 検索対象が無効なら無視
    if( targetSlot == eBD_SLOT_INVALID ){
        //LOGE( "@ CBmpGenerator::CheckThenGetCoverBDPDAndInfo: NO COVER for parentSlot=%d\n", parentSlot );
        return( NULL );
    }

    // セルの取得（※みつからなければ終了＝登録がないケースはありうる）
    stBMP_GEN_PARAM_CELL* pCell = pGenParam->searchCell( targetSlot, parentSlotIndex );
    if( pCell == NULL ){
        // LOGE( "@ CBmpGenerator::CheckThenGetCoverBDPDAndInfo: NO GEN PARAM CELL for parentSlot=%d\n", parentSlot );
        return( NULL );
    }
    
    // カバーBDPDの取得（※無効であれば終了＝登録がないケースはありうる）
    CBmpDotPartData* pCoverBdpd = pCell->pData;
    if( pCoverBdpd == NULL ){
        // LOGE( "@ CBmpGenerator::CheckThenGetCoverBDPDAndInfo: INVALID BDPD for parentSlot=%d\n", parentSlot );
        return( NULL );
    }
        
    // ここまできたらジョイントから情報の取り出し
    int ofsX = 0;
    int ofsY = 0;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rot = 0.0f;

    // カバーの表示調整
    CSlotAdjuster* pSA = pParentData->getSlotAdjuster();

    // オフセット
    ofsX = pSA->getCoverRateOfsX();
    ofsY = pSA->getCoverRateOfsY();

    // スケール
    scaleX = pSA->getCoverScaleX();
    scaleY = pSA->getCoverScaleY();
    if( jointBaseR > 0.0f ){
        scaleX *= pSA->getJointRateRotR()/jointBaseR;
        scaleY *= pSA->getJointRateRotR()/jointBaseR;
    }
    
    // 角度
    rot = CConst::ConvBezierRotationRate( pSA->getJointRateRotOfs() );
    rot += pSA->getCoverRot();
    
    // ここまできたら出力
    *pRateOfsX = ofsX;
    *pRateOfsY = ofsY;
    *pScaleX = scaleX;
    *pScaleY = scaleY;
    *pRot = rot;

    return( pCoverBdpd );
}

//------------------------------------------------------------------
// [SETTING] ジョイントポイント登録（※[XxJointYyy]のスロットから呼ばれる想定）
//------------------------------------------------------------------
// [FROM=結合元]と[TO=結合先]の間で座標を同期させる必要があるので、
// [XxJointYyy]のBDPDに対する調整途中経過の値を保持しておく必要がある
//（※ここで保持した値は[CBezierStructConv]での座標算出に利用される）
//------------------------------------------------------------------
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
    int jointIdFixedFrom0 = pSlotAdj->fixJointPointIdForFrom( slotIndex );
    int jointIdFixedFrom1 = jointIdFixedFrom0 + JOINT_POINT_OFS_FOR_OTHER_SIDE;
    if( !IS_JOINT_POINT_FIXED_VALID( jointIdFixedFrom0) || !IS_JOINT_POINT_FIXED_VALID( jointIdFixedFrom1 ) ){
        return( false );
    }
    
    // IDの検出：TO（※指定がなければ無視）
    int jointIdFixedTo0 = pSlotAdj->fixJointPointIdForTo( slotIndex );
    int jointIdFixedTo1 = jointIdFixedTo0 + JOINT_POINT_OFS_FOR_OTHER_SIDE;
    if( !IS_JOINT_POINT_FIXED_VALID( jointIdFixedTo0) || !IS_JOINT_POINT_FIXED_VALID( jointIdFixedTo1 ) ){
        return( false );
    }
    
    //----------------------------------------------------------------------------
    // ここまできたら登録準備
    //----------------------------------------------------------------------------
    CJointPoint* pJointPoint = pBaseParam->pJointPoint;
    float bdpdScaleAbs = (bdpdScale < 0 )? -bdpdScale: bdpdScale;
    float rotBase = CConst::ConvBezierRotationRate( pSlotAdj->getJointRateRotOfs() );
    float fixedAdjustRateH = pBaseParam->adjustRateH;   // ジョイントは[幅調整値]により補正される
    FIX_ADJUST_RATE( fixedAdjustRateH );

    int jointR, jointOfsR;
    CAdjustCalc::CalcRateRForH( &jointR, &jointOfsR, pSlotAdj->getJointRateRotR(), pSlotAdj->getJointRateOfsForRotCenter(),
                                pSlotAdj, fixedAdjustRateH );

    // 反転により回転方向を変える
    float fixedApplyRot = ((isFlipH != isFlipV)?-applyRot:applyRot);
    
    //--------------------------------------------------------
    // 結合元[FROM]への対応（※呼ぶ側）
    //--------------------------------------------------------
    // 接続先との回転調整座標と未調整座標の両方を算出（※[applyRot]を正の方向に50%適用）
    float rot0 = rotBase + fixedApplyRot/2 - 90;
    float rot1 = rotBase + fixedApplyRot/2 + 90;

    // 座標算出
    float fromX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 );
    float fromY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 );
    pJointPoint->registJointPointFrom( jointIdFixedFrom0, (int)fromX0, (int)fromY0,
                                       rateOfsX, rateOfsY, applyScale*bdpdScaleAbs, rotBase, isFlipH, isFlipV );
    
    float fromX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 );
    float fromY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 );
    pJointPoint->registJointPointFrom( jointIdFixedFrom1, (int)fromX1, (int)fromY1,
                                       rateOfsX, rateOfsY, applyScale*bdpdScaleAbs, rotBase, isFlipH, isFlipV );

    //--------------------------------------------------------
    // 結合先[TO]への対応（※呼ばれる側）
    //--------------------------------------------------------
    // 接続先との回転調整座標と未調整座標の両方を算出（※[applyRot]を負の方向に50%適用）
    rot0 = rotBase - fixedApplyRot/2 - 90;
    rot1 = rotBase - fixedApplyRot/2 + 90;
    
    // 座標算出
    float toX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 );
    float toY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 );
    pJointPoint->registJointPointTo( jointIdFixedTo0, (int)toX0, (int)toY0,
                                     rateOfsX, rateOfsY, bdpdScaleAbs, applyRot+rotBase, isFlipH, isFlipV );
    
    float toX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 );
    float toY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 );
    pJointPoint->registJointPointTo( jointIdFixedTo1, (int)toX1, (int)toY1,
                                     rateOfsX, rateOfsY, bdpdScaleAbs, applyRot+rotBase, isFlipH, isFlipV );
    
    //-------------------------------------------------
    // 方向線の調整（※[from-to]の中央の角度で方向線を調整）
    //-------------------------------------------------
    // 方向線の判定用に計算しなおし（※座標の移動は呼び出し元と呼び出し先を半々に回転させているが、方向線的には呼び出し先のみ回転させて計算）
    rot0 = rotBase - 90;
    rot1 = rotBase + 90;
    fromX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 );
    fromY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 );
    fromX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 );
    fromY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 );

    rot0 = rotBase - fixedApplyRot - 90;
    rot1 = rotBase - fixedApplyRot + 90;
    toX0 = CMath::RotX( jointR+jointOfsR, 0, rot0 );
    toY0 = CMath::RotY( jointR+jointOfsR, 0, rot0 );
    toX1 = CMath::RotX( jointR-jointOfsR, 0, rot1 );
    toY1 = CMath::RotY( jointR-jointOfsR, 0, rot1 );

    // 調整スケールはテンションで決め打ち（※現状、ジョイントの回転はテンションでのみ行われる）
    // TODO:回転の影響要素が他にも出てきた場合にどうするか？
    float dirScale = pBGPCell->adjForT;
    bool isTensionMinus = ( dirScale < 0.0f );
    float openFromDirX = pSlotAdj->getJointOpenFromScaleDirX( isTensionMinus );
    float openFromDirY = pSlotAdj->getJointOpenFromScaleDirY( isTensionMinus );
    float closeFromDirX = pSlotAdj->getJointCloseFromScaleDirX( isTensionMinus );
    float closeFromDirY = pSlotAdj->getJointCloseFromScaleDirY( isTensionMinus );
    float openToDirX = pSlotAdj->getJointOpenToScaleDirX( isTensionMinus );
    float openToDirY = pSlotAdj->getJointOpenToScaleDirY( isTensionMinus );
    float closeToDirX = pSlotAdj->getJointCloseToScaleDirX( isTensionMinus );
    float closeToDirY = pSlotAdj->getJointCloseToScaleDirY( isTensionMinus );

    //-----------------------------------------------------------------------------
    // [0=左側]に対する調整：[from]->[to]の方向で計算
    //-----------------------------------------------------------------------------
    // ４つのAPで矩形を描いた際、[from]側が[AP:0->1]、[to]側が[AP:0->1]の並びになっている想定
    //（※向きが逆になる場合はAP毎に[SWAP]フラグを立てること）
    //-----------------------------------------------------------------------------
    // 呼び出し元への侵入[IN]
    pJointPoint->setDirRateXY( jointIdFixedFrom0, openFromDirX*dirScale, openFromDirY*dirScale, closeFromDirX*dirScale, closeFromDirY*dirScale );
        
    // 呼び出し先からの出立[OUT]
    pJointPoint->setDirRateXY( jointIdFixedTo0, openToDirX*dirScale, openToDirY*dirScale, closeToDirX*dirScale, closeToDirY*dirScale );
    
    //-----------------------------------------------------------------------------
    // [1=右側]に対する調整：[to]->[from]の方向で計算
    //-----------------------------------------------------------------------------
    // ４つのAPで矩形を描いた際、[to]側が[AP:2->3]、[from]側が[AP:2->3]の並びになっている想定
    //（※向きが逆になる場合はAP毎に[SWAP]フラグを立てること）
    //-----------------------------------------------------------------------------
    // 呼び出し元からの出立[OUT]（※[IN]と同じ値）
    pJointPoint->setDirRateXY( jointIdFixedFrom1, openFromDirX*dirScale, openFromDirY*dirScale, closeFromDirX*dirScale, closeFromDirY*dirScale );

    // 呼び出し先への侵入[0]（※[IN]と同じ値）
    pJointPoint->setDirRateXY( jointIdFixedTo1, openToDirX*dirScale, openToDirY*dirScale, closeToDirX*dirScale, closeToDirY*dirScale );
    
    // ここまできたら登録完了
    return( true );
}
