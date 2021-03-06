/*+----------------------------------------------------------------+
  |	Title:		BDConst.hpp [共通環境]
  |	Comment:	BmpDot定数関連
  |             [bmp2hif.exe/img2hif.exe]の実装に依存
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BD_CONST_HPP__
#define __BD_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 区分[４文字]（※データの種別）
// TODO: OPTIONはいるか？（※着ぐるみのスーツに対して、付属物のオプション）
//-----------------------------------------------------------------
enum eBD_CATEGORY{
    eBD_CATEGORY_INVALID = -1,
#include "inc/BmpDotCategoryId.inc"
    
    eBD_CATEGORY_MAX,
};

#define IS_BD_CATEGORY_VALID( _c )  ((_c)>=(eBD_CATEGORY)0 && (_c)<eBD_CATEGORY_MAX)

//-----------------------------------------------------------------
// 感情[４文字]（※アリスの表情＆ポーズ種別）
//-----------------------------------------------------------------
enum eBD_EMO{
    eBD_EMO_INVALID = -1,
#include "inc/BmpDotEmoId.inc"
    eBD_EMO_MAX,
};

#define IS_BD_EMO_VALID( _e )   ((_e)>=(eBD_EMO)0 && (_e)<eBD_EMO_MAX)

// [EMO]にはデフォルトの概念がある（※非０要素が見つからない場合０要素で置き換えられる想定）
#define eBD_EMO_DEFAULT         eBD_EMO_BASE

//-----------------------------------------------------------------
// フォーム[４文字]（※パーツの並び種別＝この値で[CBmpDotOrderData]を参照する）
//-----------------------------------------------------------------
enum eBD_FORM{
    eBD_FORM_INVALID = -1,
#include "inc/BmpDotFormId.inc"
    eBD_FORM_MAX
};

#define IS_BD_FORM_VALID( _f )  ((_f)>=(eBD_FORM)0 && (_f)<eBD_FORM_MAX)

//--------------------------------------------------------------------------------------------
// Bmp画素スロット[任意の文字数]（※Bmp画素パーツの管理単位）
//--------------------------------------------------------------------------------------------
// ・各スロット毎に枠を固定数持つ（※目や腕などの複数存在するパーツに対しての指定枠を複数持つ）
// ・スロットの表示順はフォーム毎に指定される（※あるフォームは腕が上半身の前だが、他のフォームでは逆等）
// ・スロット内部の表示を前後させたい場合は表示別に[CLayerData]を分割することで可能（※スカートの前方／後方等）
//--------------------------------------------------------------------------------------------
enum eBD_SLOT{
    eBD_SLOT_INVALID = -1,
#include "inc/BmpDotSlotId.inc"
    eBD_SLOT_MAX,
};

#define IS_BD_SLOT_VALID( _s )  ((_s)>=(eBD_SLOT)0 && (_s)<eBD_SLOT_MAX)

//---------------------------------------------------------------------------------------
// UID／サブID／スロットインデックス
//---------------------------------------------------------------------------------------
// ・列挙子ではなく数値を割り当てて管理するもの
// ・[UID]によりカテゴリ別に[BDP]を特定する
// ・[サブID]は[BDPD]内で画像的な差分を持つ際に利用する：[AmBase]における接続部位の表示バリエーション等
// ・スロットインデックスは複数のスロットを指定する際にどこ(左右)を対象とするかの判断に使われる
//---------------------------------------------------------------------------------------
// [UID]は[4byte]で管理するが、運用上の値域はカテゴリによってかわる想定
// [FACE/BODY]であれば[2byte]、[SUIT]であれば[3byte]を想定する
//（※データ追加の頻度として、アリス[FACE/BODY]は少なめ、ロジック[SUIT]は多めを想定している）
//---------------------------------------------------------------------------------------
// 00000000 〜 99999999（※ブロックチェーンのデータ枠との都合で利用桁が制限されるので注意）
// イメージは[CATEGORY_FACE/BODY]が４桁[9999]、[CATEGORY_SUIT]が６桁[999999]
#define BD_UID_INVALID                  UNIQUE_ID_INVALID
#define BD_UID_MIN                      UNIQUE_ID_MIN
#define BD_UID_MAX                      UNIQUE_ID_MAX

// 慣習的に [ MIN >= id < MAX ] となる点に注意
#define IS_BD_UID_VALID( _id )          ((_id)>=BD_UID_MIN && (_id)<BD_UID_MAX)

// 00 〜 99（※[BYTE]に収まるように）
#define BD_SUB_ID_INVALID               -1
#define BD_SUB_ID_MIN                   0
#define BD_SUB_ID_MAX                   100

// 慣習的に [ MIN >= id < MAX ] となる点に注意
#define IS_BD_SUB_ID_VALID( _id )       ((_id)>=BD_SUB_ID_MIN && (_id)<BD_SUB_ID_MAX)

// スーツ例外（※スーツ時用のとして決め打ちでサブID[99]を予約しておく）
#define BD_SUB_ID_DEFAULT               BD_SUB_ID_MIN
#define BD_SUB_ID_FOR_SUITED            (BD_SUB_ID_MAX-1)

// 0 〜 1（※左右要素のみを想定）
#define BD_SLOT_INDEX_INVALID           -1
#define BD_SLOT_INDEX_MIN               0
#define BD_SLOT_INDEX_MAX               2
#define IS_BD_SLOT_INDEX_VALID( _id )   ((_id)>=BD_SLOT_INDEX_MIN && (_id)<BD_SLOT_INDEX_MAX)

//--------------------------------------------------------------------------------
// データの向き（※左右で別のパーツを指定する場合の割り振り用）
//--------------------------------------------------------------------------------
// ・素材は[左向き]で作成：向かって左(奥)を[０]、右(手前)を[１]とする
// ・右向きのパーツであってもデータ上は左向きで作成する（※ラフ画像を反転させて作業）
// （※データの指定は常に左向きを正方向として行われるため、右向き素材も左向きで作って反転して指定する）
//--------------------------------------------------------------------------------
enum eBD_DIR{
    eBD_DIR_INVALID = -1,
#include "inc/BmpDotDirId.inc"
    eBD_DIR_MAX,
};

#define IS_BD_DIR_VALID( _d )   ((_d)>=(eBD_DIR)0 && (_d)<eBD_DIR_MAX)

// [DIR]にはデフォルトの概念がある（※非０要素が見つからない場合０要素で置き換えられる想定）
#define eBD_DIR_DEFAULT         eBD_DIR_L
#define GET_BD_DIR_DEFAULT_FOR_SLOT_INDEX( _slotIndex ) (((_slotIndex)==0)? eBD_DIR_L: eBD_DIR_R)

//--------------------------------------------------------------------------------
// オプション（※[BmpDotPartData]内での素材管理用の区分）
//--------------------------------------------------------------------------------
// ・ディテールが前後の表示物の形状に依存する描画要素のオン／オフを指定する
// （※対象のパーツ内で表示するか／しないかの指定となり、他のパーツへの代替を想定しない表示物）
// ・オプションは対象のパーツ内容（形状）に限定された表示追加要素のため、
//   対象のドットデータ内で区分毎にレイヤーを作成し、基本要素等の前後で追加描画させる想定となる
// ・オプションの指定はラベルにより指定される（※[まつげ上]、[まつげ下]等）
// （※簡便のためオプション内での差分はもたないでおく＝差分を持ちたい場合はサブID等で対応）
//------------------------------------------------------------------------------
enum eBD_OPTION{
    eBD_OPTION_INVALID = -1,
#include "inc/BmpDotOptionId.inc"   
    eBD_OPTION_MAX,
};

#define IS_BD_OPTION_VALID( _o )            ((_o)>=(eBD_OPTION)0 && (_o)<eBD_OPTION_MAX)

#define eBD_OPTION_FOR_SLOT_START           eBD_OPTION_SlotA
#define eBD_OPTION_FOR_SLOT_END             eBD_OPTION_SlotB
#define IS_BD_OPTION_FOR_SLOT_VALID( _o )   ((_o)>=eBD_OPTION_FOR_SLOT_START && (_o)<=eBD_OPTION_FOR_SLOT_END)

#define BD_OPTION_FOR_SLOT_MAX              (eBD_OPTION_FOR_SLOT_END-eBD_OPTION_FOR_SLOT_START+1)

//---------------------------------------------------
// レイヤーの並びタイプ（※実際のデータ出力時の重ね合わせ判定用）
//---------------------------------------------------
enum eBD_LAYER_ORDER_TYPE{
    eBD_LAYER_ORDER_TYPE_INVALID = -1,
    
    eBD_LAYER_ORDER_TYPE_FRONT,         // 手前に配置する
    eBD_LAYER_ORDER_TYPE_NORMAL,        // 通常の配置先
    eBD_LAYER_ORDER_TYPE_BACK,          // 後方に配置する
    
    eBD_LAYER_ORDER_TYPE_MAX,
};

#define IS_BD_LAYER_ORDER_TYPE_VALID( _t )  ((_t)>=(eBD_LAYER_ORDER_TYPE)0 && (_t)<eBD_LAYER_ORDER_TYPE_MAX)

//---------------------------------------------------
// レイヤーの並びグループ（※スーツ時のレイヤー構成調整用）
//---------------------------------------------------
enum eBD_LAYER_ORDER_GROUP{
    eBD_LAYER_ORDER_GROUP_INVALID = -1,
    
    eBD_LAYER_ORDER_GROUP_DEFAULT,     // デフォルト
    
    eBD_LAYER_ORDER_GROUP_FRONT,       // 手前（腕＆顔）
    eBD_LAYER_ORDER_GROUP_MIDDLE,      // 中間（脚＆頭）
    eBD_LAYER_ORDER_GROUP_COVER,       // カバー（ハリボテ の直前）
    eBD_LAYER_ORDER_GROUP_SUIT,        // スーツ（ハリボテ）
    eBD_LAYER_ORDER_GROUP_BACK,        // 後ろ（ポニーテール等）
    
    eBD_LAYER_ORDER_GROUP_MAX,
};

#define IS_BD_LAYER_ORDER_GROUP_VALID( _g )    ((_g)>=(eBD_LAYER_ORDER_GROUP)0 && (_g)<eBD_LAYER_ORDER_GROUP_MAX)

// スーツ時の表示範囲
#define eBD_LAYER_ORDER_GROUP_START_FOR_SUIT   eBD_LAYER_ORDER_GROUP_FRONT
#define eBD_LAYER_ORDER_GROUP_END_FOR_SUIT     eBD_LAYER_ORDER_GROUP_BACK

//---------------------------------------------------
// マスクの対象（※マスクの画素は８ビットなので８枠まで利用可能）
//---------------------------------------------------
enum eBD_MASK_TARGET{
    eBD_MASK_TARGET_INVALID = -1,
    
    eBD_MASK_TARGET_DEFAULT,    // デフォルト

    eBD_MASK_TARGET_NECK,       // 首：eBD_SLOT_NkCover
    eBD_MASK_TARGET_HEAD,       // 頭：eBD_SLOT_HdTopCover
    eBD_MASK_TARGET_FACE,       // 顔：eBD_SLOT_FcUnderCover
    eBD_MASK_TARGET_HEAD_FACE,  // 頭と顔：eBD_SLOT_HdTopCover/eBD_SLOT_FcUnderCover
    eBD_MASK_TARGET_ARM_L,      // 腕：eBD_SLOT_AmCover（左：slotIndex==0）
    eBD_MASK_TARGET_ARM_R,      // 腕：eBD_SLOT_AmCover（右：slotIndex==1）
    eBD_MASK_TARGET_LEG_L,      // 脚：eBD_SLOT_LgCover（左：slotIndex==0)
    eBD_MASK_TARGET_LEG_R,      // 脚：eBD_SLOT_LgCover（右：slotIndex==1)

    eBD_MASK_TARGET_MAX,
};

#define IS_BD_MASK_TARGET_VALID( _t )           ((_t)>=(eBD_MASK_TARGET)0 && (_t)<eBD_MASK_TARGET_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//-------------------------------------------
// パーツの検索パラメータ（※数が多いので構造体で指定）
//-------------------------------------------
struct stBD_PART_SEARCH_PARAM{
    // [BDP:BmpDotPack]特定用
    eBD_CATEGORY    category;       // カテゴリ
    int             uid;            // UID
    eBD_EMO         emo;            // 感情（※デフォルト概念あり）

    // [BDD:BmpDotData]特定用
    eBD_FORM        form;           // フォーム

    // [BDPD:BmpDotPartData]特定用
    eBD_SLOT        slot;           // スロット
    int             subId;          // サブID
    eBD_DIR         dir;            // 向き（※デフォルト概念あり）
    
    // 優先検索するスーツID
    int suitUid;
};

// クリア
#define CLEAR_BD_PART_SEARCH_PARAM( _pParam )                   \
    memset( (_pParam), 0, sizeof(stBD_PART_SEARCH_PARAM) );     \
    (_pParam)->suitUid = -1

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------
// Bmp画素定数
//----------------------------
class CBDConst{
private:
    // データ
    static int s_nArrSlotNum[];                             // スロットのインデックス数（※腕や目のように左右の区分けがあるものは２）
    static eBD_SLOT s_eArrParentForSlot[];                  // スロットの親（※表示情報参照先）
    static eBD_CATEGORY s_eArrCategoryForSlot[];            // スロットのカテゴリ（※基本素材がどのBDPで管理されるか？）
    static bool s_bArrIsUpForBody[];                        // 上半身要素か？
    static bool s_bArrIsLowForBody[];                       // 下半身要素か?
    static eBD_LAYER_ORDER_GROUP s_eArrOrderGroupForSlot[]; // スロットのグループ（デフォルト値）
    static eBD_MASK_TARGET s_eArrMaskTargetForSlot[];       // スロットのマスク対象（デフォルト値）
    
    // マスクターゲットに対する参照ビットマスク
    static int s_nArrBitMaskForMaskTarget[];

public:
    // スロットインデックス数の取得
    static int GetNumSlotIndex( eBD_SLOT slot );
    
    // スロットインデックス補正（※スロットの種別によりインデックスの値域を補正する）
    static int  FixIndexForSlot( eBD_SLOT slot, int slotIndex );
    
    // 親スロットの取得
    static eBD_SLOT GetParentSlot( eBD_SLOT slot );

    // 傾け処理用の親スロットの取得
    static eBD_SLOT GetParentSlotForAnglePlane( eBD_SLOT slot );
    
    // スロットからカテゴリの取得
    static eBD_CATEGORY GetCategoryForSlot( eBD_SLOT slot );
    
    // スロットが上半身か？下半身か？
    static bool IsSlotBodyUp( eBD_SLOT slot );
    static bool IsSlotBodyLow( eBD_SLOT slot );

    // スロットからオーダーグループの取得
    static eBD_LAYER_ORDER_GROUP GetOrderGroupForSlot( eBD_SLOT slot );

    // ビットマスクの取得
    static BYTE GetBitMaskForSlotIndex( eBD_MASK_TARGET target, eBD_SLOT slot, int slotIndex );

    // BDPD識別名の設定
    static void SetBmpDotPartDataName( char* pBuf, eBD_SLOT slot, int subId, eBD_DIR dir );

private:
	// インスタンス生成は不可
	CBDConst( void ){}
	virtual ~CBDConst( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelBdCategory[];
extern const char* g_pArrLabelBdEmo[];
extern const char* g_pArrLabelBdForm[];
extern const char* g_pArrLabelBdSlot[];
extern const char* g_pArrLabelBdDir[];
extern const char* g_pArrLabelBdOption[];

extern const char* g_pArrLabelBdLayerOrderType[];
extern const char* g_pArrLabelBdLayerOrderTypeS[];

extern const char* g_pArrLabelBdLayerOrderGroup[];
extern const char* g_pArrLabelBdLayerOrderGroupS[];

extern const char* g_pArrLabelBdMaskTarget[];
extern const char* g_pArrLabelBdMaskTargetS[];

#endif /* __BD_CONST_HPP__ */
