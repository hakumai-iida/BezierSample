/*+----------------------------------------------------------------+
  |	Title:		BPConst.cpp [共通環境]
  |	Comment:	BmpPal定数関連
  |             [bmp2hif.exe/img2hif.exe]の実装に依存
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BPConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//--------------------
// カテゴリラベル
//--------------------
const char* g_pArrLabelBpCategory[] = {
    "ARITH",
    "SUIT",
    "OPTION",
    
    NULL,
};

//--------------------
// カテゴリラベル（短縮系）
//--------------------
const char* g_pArrLabelBpCategoryS[] = {
    "A",
    "S",
    "O",
    
    NULL,
};

/*+----------------------------------------------------------------+
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// BPD識別名取得
//-----------------------
void CBPConst::SetBmpPalDataName( char* pBuf, eBP_CATEGORY category, int uid, int subId ){
    // 確認：カテゴリ
    if( ! IS_BP_CATEGORY_VALID( category ) ){
        LOGE( "@ CBPConst::SetBmpPalDataName: INVALID VAL: category=%d, uid=%d, subId=%d\n", category, uid, subId );
        return;
    }

    // 確認：UID
    if( ! IS_BP_UID_VALID( uid ) ){
        LOGE( "@ CBPConst::SetBmpPalDataName: INVALID VAL: category=%d, uid=%d, subId=%d\n", category, uid, subId );
        return;
    }

    // 確認：サブID
    if( ! IS_BP_SUB_ID_VALID( subId ) ){
        LOGE( "@ CBPConst::SetBmpPalDataName: INVALID VAL: category=%d, uid=%d, subId=%d\n", category, uid, subId );
        return;
    }

    // ARITH(キャラクタ枠)は４桁
    if( category == eBP_CATEGORY_ARITH ){
        sprintf( pBuf, "%s%04d_%02d", g_pArrLabelBpCategoryS[category], uid, subId );
    }
    // その他(ロジック枠)は６桁
    else{
        sprintf( pBuf, "%s%06d_%02d", g_pArrLabelBpCategoryS[category], uid, subId );
    }
}
