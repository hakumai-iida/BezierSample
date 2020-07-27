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
//-----------------------------------------------------------------
// [SETTING] BMP生成パラメータの作成（※処理後は呼び出し側で開放すること）
//-----------------------------------------------------------------
CBmpGenParam* CBmpGenerator::CreateBmpGenParam( CBmpDotSettingData* pSetting,
                                                int idForFace, int idForBody, int idForSuit, eBD_EMO emo, eBD_FORM form,
                                                float adjForT, float adjForS ){
    
    // 無効は無視
    if( pSetting == NULL ){
        LOGE( "@ CBmpGenerator::CreateBmpGenParam: NULL pSetting\n" );
        return( NULL );
    }
    
    //----------------------------------------------
    // BMP生成パラメータ確保（※ワークなので[APP]から確保）
    //----------------------------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    CBmpGenParam* pBmpGenParam = new CBmpGenParam();
    CMemMgr::PopTargetField();

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
    if( IS_BD_UID_VALID( defIdForSuit ) ){
        if( ! IS_BD_UID_VALID( idForFace ) ){ defIdForFace = 0; }
        if( ! IS_BD_UID_VALID( idForBody ) ){ defIdForBody = 0; }
        isSuited = true;
    }
    
    // 補助情報の設定（※BMP生成処理のオーダーリスト構築時に利用される）
    pBmpGenParam->setForm( defForm );
    pBmpGenParam->setSuited( isSuited );
    
    //--------------------------------------
    // オーダーのエントリからパーツを検索して登録
    //--------------------------------------
    CBmpDotOrderData order;
    order.setDefault( defForm, isSuited );

    float tempT, tempS;
    stBD_PART_SEARCH_PARAM param;
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        // セルの取得（※無効は無視）
        stBMP_DOT_ORDER_DATA_CELL* pCell = order.getCellAt( i );
        if( pCell == NULL ){ continue; }
        if( ! IS_BD_SLOT_VALID( pCell->slot ) ){ continue; }
        if( ! IS_BD_SLOT_INDEX_VALID( pCell->slotIndex ) ){ continue; }

        // スロット設定の検索（※無効は無視）
        param.slot = pCell->slot;
        CBmpDotSettingSlotData* pSlot = pSetting->searchSlot( pCell->slot, pCell->slotIndex );
        if( pSlot == NULL ){ continue; }

        // スロット指定からパラメータの設定
        param.category = pSlot->getCategory();
        param.uid = pSlot->getUid();
        param.emo = pSlot->getEmo();
        param.form = pSlot->getForm();
        param.subId = pSlot->getSubId();
        param.dir = pSlot->getDir();
        
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
        if( pBmpGenParam->addCell( &param, pCell->slotIndex, tempT, tempS ) == NULL ){
#ifdef BMP_GEN_LOG
            LOGD( "@ [x] ADD CELL( %s, %d, %d);\n", g_pArrLabelBdSlot[param.slot], param.subId, param.dir );
#endif
        }else{
#ifdef BMP_GEN_LOG
            LOGD( "@ [o] ADD CELL( %s, %d, %d);\n", g_pArrLabelBdSlot[param.slot], param.subId, param.dir );
#endif
        }
    }
    
    return( pBmpGenParam );
}

//---------------------------------------------------------
// [SETTING] テスト用パラメータの開放
//---------------------------------------------------------
void CBmpGenerator::ReleaseBmpGenParam( CBmpGenParam* pParam ){
    //-----------------------------------------------------------------------------
    // セルの開放（※独自に確保した個々の[CBmpDotPartData]を開放＝デストラクタがないので手で解放）
    //-----------------------------------------------------------------------------
    int num = pParam->getCellNumUse();
    stBMP_GEN_PARAM_CELL* pArrCell = pParam->getCellArr();
    for( int i=0; i<num; i++ ){
        CLEAR_BMP_GEN_PARAM_CELL( &pArrCell[i] );
    }
    
    // 保持クラス開放
    SAFE_DELETE( pParam );
}
