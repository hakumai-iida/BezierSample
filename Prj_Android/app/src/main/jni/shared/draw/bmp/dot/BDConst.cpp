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

//----------------------
// ラベル：オーダータイプ
//----------------------
const char* g_pArrLabelBdLayerOrderType[] = {
    "FRONT",
    "NORMAL",
    "BACK",
    
    NULL,
};

//--------------------------
// ラベル：オーダータイプ（短縮形）
//--------------------------
const char* g_pArrLabelBdLayerOrderTypeS[] = {
    "F",
    "N",
    "B",
    
    NULL,
};

//----------------------
// ラベル：オーダーグループ
//----------------------
const char* g_pArrLabelBdLayerOrderGroup[] = {
    "DEFAULT",
    
    "FRONT",
    "MIDDLE",
    "COVER",
    "SUIT",
    "BACK",

    NULL,
};

//--------------------------
// ラベル：オーダータイプ（短縮形）
//--------------------------
const char* g_pArrLabelBdLayerOrderGroupS[] = {
    "D",
    
    "F",
    "M",
    "C",
    "S",
    "B",

    NULL,
};

//----------------------
// ラベル：マスク対象
//----------------------
const char* g_pArrLabelBdMaskTarget[] = {
    "DEFAULT",
    
    "NECK",
    "HEAD",
    "FACE",
    "HEAD_FACE",
    "ARM_L",
    "ARM_R",
    "LEG_L",
    "LEG_R",

    NULL,
};

//--------------------------
// ラベル：マスク対象（短縮形）
//--------------------------
const char* g_pArrLabelBdMaskTargetS[] = {
    "D",
    
    "nk",
    "hd",
    "fc",
    "h/f"
    "aL",
    "aR",
    "aL",
    "aR",

    NULL,
};

//--------------------------------------------------------------------------
// データ
//--------------------------------------------------------------------------
// スロットの受付インデックス数
int CBDConst::s_nArrSlotNum[] = {
#include "inc/BmpDotSlotNum.inc"
};

// スロットの親
eBD_SLOT CBDConst::s_eArrParentForSlot[] = {
#include "inc/BmpDotSlotParent.inc"
};

// スロットのカテゴリ
eBD_CATEGORY CBDConst::s_eArrCategoryForSlot[] = {
#include "inc/BmpDotSlotCategory.inc"
};

// 上半身要素か？
bool CBDConst::s_bArrIsUpForBody[] = {
#include "inc/BmpDotSlotIsUp.inc"
};

// 下半身要素か？
bool CBDConst::s_bArrIsLowForBody[] = {
#include "inc/BmpDotSlotIsLow.inc"
};

// スロットのグループ（※デフォルト値）
eBD_LAYER_ORDER_GROUP CBDConst::s_eArrOrderGroupForSlot[] = {
#include "inc/BmpDotSlotOrderGroup.inc"
};

// スロットのマスク対象（※デフォルト値）
eBD_MASK_TARGET CBDConst::s_eArrMaskTargetForSlot[] = {
#include "inc/BmpDotSlotMaskTarget.inc"
};

// マスクターゲットに対する参照ビットマスク
int CBDConst::s_nArrBitMaskForMaskTarget[] = {
    0,      // eBD_MASK_TARGET_DEFAULT,    // デフォルト

    0x01,      // eBD_MASK_TARGET_NECK,       // 首：eBD_SLOT_NkCover
    0x02,      // eBD_MASK_TARGET_HEAD,       // 頭：eBD_SLOT_HdTopCover
    0x04,      // eBD_MASK_TARGET_FACE,       // 顔：eBD_SLOT_FcUnderCover
    0x06,      // eBD_MASK_TARGET_HEAD_FACE,  // 頭と顔：eBD_SLOT_HdTopCover/eBD_SLOT_FcUnderCover
    0x08,      // eBD_MASK_TARGET_ARM_L,      // 腕：eBD_SLOT_AmCover（左：slotIndex==0）
    0x10,      // eBD_MASK_TARGET_ARM_R,      // 腕：eBD_SLOT_AmCover（右：slotIndex==1）
    0x20,      // eBD_MASK_TARGET_LEG_L,      // 脚：eBD_SLOT_LgCover（左：slotIndex==0)
    0x40,      // eBD_MASK_TARGET_LEG_R,      // 脚：eBD_SLOT_LgCover（右：slotIndex==1)
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

//-------------------------------
// 親スロットの取得
//-------------------------------
eBD_SLOT CBDConst::GetParentSlot( eBD_SLOT slot ){
    if( IS_BD_SLOT_VALID( slot ) ){
        return( s_eArrParentForSlot[slot] );
    }
    
    return( eBD_SLOT_INVALID );
}

//----------------------------------------------------------------
// 全体アングル平面に対する親スロットの取得（※どのパーツを中心に傾けるかの基準）
//（※「あたま、かお、からだ」の３つの平面がそれぞれ別に傾くイメージ）
//----------------------------------------------------------------
eBD_SLOT CBDConst::GetParentSlotForAnglePlane( eBD_SLOT slot ){
    eBD_SLOT parentSlot = GetParentSlot( slot );

    // 頭
    if( parentSlot == eBD_SLOT_HdBase ){
        return( eBD_SLOT_HdBase );
    }

    // 顔
    if( parentSlot == eBD_SLOT_FcBase ){
        return( eBD_SLOT_FcBase );
    }
    
    // この時点での有効スロットは腹
    if( IS_BD_SLOT_VALID( parentSlot ) ){
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
// 上半身のスロットか？
//-----------------------
bool CBDConst::IsSlotBodyUp( eBD_SLOT slot ){
    // ボディ以外は無視
    if( GetCategoryForSlot( slot ) != eBD_CATEGORY_BODY ){
        return( false );
    }
    
    return( s_bArrIsUpForBody[slot] );
}

//-----------------------
// 下半身のスロットか？
//-----------------------
bool CBDConst::IsSlotBodyLow( eBD_SLOT slot ){
    // ボディ以外は無視
    if( GetCategoryForSlot( slot ) != eBD_CATEGORY_BODY ){
        return( false );
    }

    return( s_bArrIsLowForBody[slot] );
}

//------------------------------
// スロットからオーダーグループの取得
//------------------------------
eBD_LAYER_ORDER_GROUP CBDConst::GetOrderGroupForSlot( eBD_SLOT slot ){
    if( IS_BD_SLOT_VALID( slot ) ){
        return( s_eArrOrderGroupForSlot[slot] );
    }

    // ここまできたら無効値を返す
    return( eBD_LAYER_ORDER_GROUP_INVALID );
}

//---------------------------
// マスクターゲットビット番号調整
//---------------------------
BYTE CBDConst::GetBitMaskForSlotIndex( eBD_MASK_TARGET target, eBD_SLOT slot, int slotIndex ){
    // デフォルトなら割り振り
    if( target == eBD_MASK_TARGET_DEFAULT ){
        if( IS_BD_SLOT_VALID( slot ) ){
            target = s_eArrMaskTargetForSlot[slot];
        }
    }

    // 左右調整（※スロットインデックスの値により左右の割り振り）
    if( target >= eBD_MASK_TARGET_ARM_L && target <= eBD_MASK_TARGET_ARM_R ){
        target = (eBD_MASK_TARGET)(eBD_MASK_TARGET_ARM_L + ((slotIndex!=0)? 1: 0));
    }else if(  target >= eBD_MASK_TARGET_LEG_L &&  target <= eBD_MASK_TARGET_LEG_R ){
        target = (eBD_MASK_TARGET)(eBD_MASK_TARGET_LEG_L + ((slotIndex!=0)? 1: 0));
    }

    // ビットマスクの抽出
    if( IS_BD_MASK_TARGET_VALID( target ) ){
        return( s_nArrBitMaskForMaskTarget[target] );
    }
    
    // ここまで来たら無効値
    return( 0x00 );
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
