/*+----------------------------------------------------------------+
  |	Title:		BDConst.cpp [共通環境]
  |	Comment:	BmpDot定数関連
  |             [bmp2hif.exe/img2hif.exe]の実装に依存
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BDConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//-----------------
// カテゴリラベル
//-----------------
const char* g_pArrLabelBdCategory[] = {
    #include "inc/BmpDotCategoryLabel.inc"
    NULL,
};

//-----------------
// 感情ラベル
//-----------------
const char* g_pArrLabelBdEmo[] = {
    #include "inc/BmpDotEmoLabel.inc"
    NULL,
};

//-----------------
// フォームラベル
//-----------------
const char* g_pArrLabelBdForm[] = {
    #include "inc/BmpDotFormLabel.inc"
    NULL,
};

//-----------------
// スロットラベル
//-----------------
const char* g_pArrLabelBdSlot[] = {
#include "inc/BmpDotSlotLabel.inc"
	NULL,
};

//-----------------
// ラベル：方向
//-----------------
const char* g_pArrLabelBdDir[] = {
#include "inc/BmpDotDirLabel.inc"
    NULL,
};

//-----------------
// ラベル：オプション
//-----------------
const char* g_pArrLabelBdOption[] = {
#include "inc/BmpDotOptionLabel.inc"
    NULL,
};

//--------------------------------------------------------------------------
// スロットの受付インデックス数
//--------------------------------------------------------------------------
int CBDConst::s_nArrSlotNum[] = {
#include "inc/BmpDotSlotNum.inc"
};

//-----------------------------
// スロットの親
//-----------------------------
eBD_SLOT CBDConst::s_eArrParentForSlot[] = {
#include "inc/BmpDotSlotParent.inc"
};

//-----------------------------
// スロットのカテゴリ
//-----------------------------
eBD_CATEGORY CBDConst::s_eArrCategoryForSlot[] = {
#include "inc/BmpDotSlotCategory.inc"
};

/*+----------------------------------------------------------------+
  |	Prototype	プロト宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// インデックス数の取得
//-----------------------------------------------------------------
int CBDConst::GetNumSlotIndex( eBD_SLOT slot ){
    // 複数スロットをもつ場合は受付数の確認
    if( IS_BD_SLOT_VALID( slot ) ){
        return( s_nArrSlotNum[slot] );
    }
        
    return( 0 );
}

//-----------------------------------------------------------------
// インデックスの調整（※スロットにより存在できる数が変わるため）
//-----------------------------------------------------------------
int CBDConst::FixIndexForSlot( eBD_SLOT slot, int slotIndex ){
    // 複数スロットをもつ場合は受付数の確認
    if( IS_BD_SLOT_VALID( slot ) ){
        if( slotIndex < 0 ){
            slotIndex = 0;
        }else if( slotIndex >= s_nArrSlotNum[slot] ){
            slotIndex = s_nArrSlotNum[slot]-1;
        }
    }else{
        slotIndex = 0;
    }

    return( slotIndex );
}

//----------------------------------------------------------------
// 全体アングル平面に対する親スロットの取得（※どのパーツを中心に傾けるかの基準）
//（※「あたま、かお、からだ」の３つの平面がそれぞれ別に傾くイメージ）
//----------------------------------------------------------------
eBD_SLOT CBDConst::GetParentSlotForAnglePlane( eBD_SLOT slot ){
    if( IS_BD_SLOT_VALID( slot ) ){
        eBD_SLOT parentSlot = s_eArrParentForSlot[slot];

        // 頭
        if( parentSlot == eBD_SLOT_HdBase ){
            return( eBD_SLOT_HdBase );
        }

        // 顔
        if( parentSlot == eBD_SLOT_FcBase || parentSlot == eBD_SLOT_MoBase || parentSlot == eBD_SLOT_EyBase ){
            return( eBD_SLOT_FcBase );
        }
        
        // ここまできたら一律で腹を親としておく
        return( eBD_SLOT_BlBase );
    }

    // ここまできたら無効
    return( eBD_SLOT_INVALID );
}

//-----------------------
// スロットからカテゴリの取得
//-----------------------
eBD_CATEGORY CBDConst::GetCategoryForSlot( eBD_SLOT slot ){
    if( IS_BD_SLOT_VALID( slot ) ){
        return( s_eArrCategoryForSlot[slot] );
    }

    // ここまできたら無効
    return( eBD_CATEGORY_INVALID );
}

//-----------------------
// BDPD識別名取得
//-----------------------
void CBDConst::SetBmpDotPartDataName( char* pBuf, eBD_SLOT slot, int subId, eBD_DIR dir ){
    // 確認：スロット
    if( ! IS_BD_SLOT_VALID( slot ) ){
        LOGE( "@ CBDConst::SetBmpDotNameShort: INVALID VAL: slot=%d, subId=%d, dir=%d\n", slot, subId, dir );
        return;
    }
    
    // 確認：サブID
    if( ! IS_BD_SUB_ID_VALID( subId ) ){
        LOGE( "@ CBDConst::SetBmpDotNameShort: INVALID VAL: slot=%d, subId=%d, dir=%d\n", slot, subId, dir );
        return;
    }
    
    // 確認：向き
    if( ! IS_BD_DIR_VALID( dir ) ){
        LOGE( "@ CBDConst::SetBmpDotNameShort: INVALID VAL: slot=%d, subId=%d, dir=%d\n", slot, subId, dir );
        return;
    }

    sprintf( pBuf, "%s_%02d%s", g_pArrLabelBdSlot[slot], subId, g_pArrLabelBdDir[dir] );
}
