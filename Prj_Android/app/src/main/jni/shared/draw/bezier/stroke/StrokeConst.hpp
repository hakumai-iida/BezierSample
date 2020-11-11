/*+----------------------------------------------------------------+
  |	Title:		StrokeConst.hpp [共通環境]
  |	Comment:	ストローク定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __STROKE_CONST_HPP__
#define __STROKE_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// はみ出し精度：ドットの中心からずらせる階調（※縦横それぞれ４段階の１６精度）
//（※はみ出し具合に応じた値を指定の色（パレットグループ）に足しこむ概念）
//（※あまり精度を高くしても見た目に変化がないようなので４程度で十分と思われる）
//-----------------------------------------------------------------
#define STROKE_DOT_OUT_PRECISION_W      4
#define STROKE_DOT_OUT_PRECISION_H      4
#define STROKE_DOT_OUT_PRECISION_MAX    (STROKE_DOT_OUT_PRECISION_W*STROKE_DOT_OUT_PRECISION_H)

// ストローク画素基本サイズ最大（※論理サイズの最大値）
#define STROKE_DOT_BASE_SIZE_MAX        5

// ストローク画素実サイズ最大（※ピクセル率の適用）
#define STROKE_DOT_SIZE_MAX             (STROKE_DOT_BASE_SIZE_MAX*GM_P_RATE)

// ストローク画素確保サイズ最大（※さらに最大割合[300%]の適用）
#define STROKE_DOT_ALLOC_SIZE_MAX       (STROKE_DOT_SIZE_MAX*3)

// ストローク画素精度（※[1.0]のサイズに対して４階調[0.25刻み]の精度をもつ）
#define STROKE_DOT_SIZE_PRECISION       4

//----------------------
// ストロークタイプ
//----------------------
enum eSTROKE_TYPE{
    eSTROKE_TYPE_INVALID = -1,  // 無効値
    
    eSTROKE_TYPE_OFF,           // オフ（※線を引かない＝塗りガイドのみ出力したい場合）   
    eSTROKE_TYPE_SQUARE,        // 四角：[CStrokeMgr::GetStrokeDotForSquare]を利用
    eSTROKE_TYPE_CIRCLE,        // 円：[CStrokeMgr::GetStrokeDotForCircle]を利用

    eSTROKE_TYPE_MAX,
};

#define IS_STROKE_TYPE_VALID( _t )  ((_t)>=(eSTROKE_TYPE)0 && (_t)<eSTROKE_TYPE_MAX)

//----------------------------------------------------------------------
// フリルID
//----------------------------------------------------------------------
// タッチ表示の際にストロークではなくレイヤーデータ（線と塗り）を出力する際の指定ID
//----------------------------------------------------------------------
enum eSTROKE_FRILL{
    eSTROKE_FRILL_INVALID = -1,

    eSTROKE_FRILL_TEST_CIRCLE,          // テスト：丸
    eSTROKE_FRILL_TEST_TRIANGLE,        // テスト：三角
    eSTROKE_FRILL_TEST_SQUARE,          // テスト：四角
    
    eSTROKE_FRILL_FLUFFY_SQUARE_MAIN,   // フリル：角（基本）
    eSTROKE_FRILL_FLUFFY_SQUARE_SUB,    // フリル：角（サブ）

    eSTROKE_FRILL_MAX,
};

#define IS_STROKE_FRILL_VALID( _f )  ((_f)>=(eSTROKE_FRILL)0 && (_f)<eSTROKE_FRILL_MAX)

//-------------------------------------------------------------------------------
// フック対象
//-------------------------------------------------------------------------------
// アンカーポイントの実描画座標を、別のアンカーポイントから参照することで、座標を同期させる仕組み
//（※先に描画する側が座標を登録し、後に描画される側が参照するため描画順で座標値が変わる点に注意）
//-------------------------------------------------------------------------------
enum eSTROKE_HOOK_TARGET{
    eSTROKE_HOOK_TARGET_INVALID = -1,

    //-----------------------------------------------------------------------
    // 一時枠（※レイヤー単位でクリアされる／レイヤー内でオーダーが連続する要素にはこれを使う）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_TEMP_A,    eSTROKE_HOOK_TARGET_TEMP_B,    eSTROKE_HOOK_TARGET_TEMP_C,    eSTROKE_HOOK_TARGET_TEMP_D,
    eSTROKE_HOOK_TARGET_TEMP_E,    eSTROKE_HOOK_TARGET_TEMP_F,    eSTROKE_HOOK_TARGET_TEMP_G,    eSTROKE_HOOK_TARGET_TEMP_H,
    eSTROKE_HOOK_TARGET_TEMP_I,    eSTROKE_HOOK_TARGET_TEMP_J,    eSTROKE_HOOK_TARGET_TEMP_K,    eSTROKE_HOOK_TARGET_TEMP_L,
    eSTROKE_HOOK_TARGET_TEMP_M,    eSTROKE_HOOK_TARGET_TEMP_N,    eSTROKE_HOOK_TARGET_TEMP_O,    eSTROKE_HOOK_TARGET_TEMP_P,
    eSTROKE_HOOK_TARGET_TEMP_Q,    eSTROKE_HOOK_TARGET_TEMP_R,    eSTROKE_HOOK_TARGET_TEMP_S,    eSTROKE_HOOK_TARGET_TEMP_T,
    eSTROKE_HOOK_TARGET_TEMP_U,    eSTROKE_HOOK_TARGET_TEMP_V,    eSTROKE_HOOK_TARGET_TEMP_W,    eSTROKE_HOOK_TARGET_TEMP_X,
    eSTROKE_HOOK_TARGET_TEMP_Y,    eSTROKE_HOOK_TARGET_TEMP_Z,

    //-----------------------------------------------------------------------
    // パーツ枠（※パーツ毎に確保される枠／レイヤー内でオーダーが連続しない要素にはこれを使う）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_PART_A,    eSTROKE_HOOK_TARGET_PART_B,    eSTROKE_HOOK_TARGET_PART_C,    eSTROKE_HOOK_TARGET_PART_D,
    eSTROKE_HOOK_TARGET_PART_E,    eSTROKE_HOOK_TARGET_PART_F,    eSTROKE_HOOK_TARGET_PART_G,    eSTROKE_HOOK_TARGET_PART_H,
    eSTROKE_HOOK_TARGET_PART_I,    eSTROKE_HOOK_TARGET_PART_J,    eSTROKE_HOOK_TARGET_PART_K,    eSTROKE_HOOK_TARGET_PART_L,
    eSTROKE_HOOK_TARGET_PART_M,    eSTROKE_HOOK_TARGET_PART_N,    eSTROKE_HOOK_TARGET_PART_O,    eSTROKE_HOOK_TARGET_PART_P,
    eSTROKE_HOOK_TARGET_PART_Q,    eSTROKE_HOOK_TARGET_PART_R,    eSTROKE_HOOK_TARGET_PART_S,    eSTROKE_HOOK_TARGET_PART_T,
    eSTROKE_HOOK_TARGET_PART_U,    eSTROKE_HOOK_TARGET_PART_V,    eSTROKE_HOOK_TARGET_PART_W,    eSTROKE_HOOK_TARGET_PART_X,
    eSTROKE_HOOK_TARGET_PART_Y,    eSTROKE_HOOK_TARGET_PART_Z,

    //-----------------------------------------------------------------------
    // 固定枠：（※パーツをまたいで指定される要素／データ作成の仕様上で用途が限定できるもの）
    //-----------------------------------------------------------------------
    // 首と頭の接続：[HdBaseが設定/NkBaseが参照]
    eSTROKE_HOOK_TARGET_NECK_TO_HEAD_L,         eSTROKE_HOOK_TARGET_NECK_TO_HEAD_R,

    // もみあげの上部の接続（外側から耳にかかるライン）：[HdBaseが設定／HdSideが参照]
    eSTROKE_HOOK_TARGET_HAIR_SIDE_L,            eSTROKE_HOOK_TARGET_HAIR_SIDE_R,

    // ヘアバンドの両端の接続（幅を補正）：[HdTop/HdCapが設定／HdBandが参照]
    eSTROKE_HOOK_TARGET_HAIR_BAND_L,            eSTROKE_HOOK_TARGET_HAIR_BAND_R,

    // 生際から後ろへ流れる髪のストローク：[FcBaseが設定/HdTopが参照]
    // TODO:HV調整が反映されないのであまりきれいなストロークにならない＆タッチと組み合わせるとチグハグな感じになる
    eSTROKE_HOOK_TARGET_HAIR_STROKE_0_L,        eSTROKE_HOOK_TARGET_HAIR_STROKE_0_R,
    eSTROKE_HOOK_TARGET_HAIR_STROKE_1_L,        eSTROKE_HOOK_TARGET_HAIR_STROKE_1_R,
    eSTROKE_HOOK_TARGET_HAIR_STROKE_2_L,        eSTROKE_HOOK_TARGET_HAIR_STROKE_2_R,
    eSTROKE_HOOK_TARGET_HAIR_STROKE_3_L,        eSTROKE_HOOK_TARGET_HAIR_STROKE_3_R,
    eSTROKE_HOOK_TARGET_HAIR_STROKE_4_L,        eSTROKE_HOOK_TARGET_HAIR_STROKE_4_R,

    // 腕から肩（腕を上に持ち上げた際の脇越しにみえる広背筋のライン）：[UpBaseが設定／AmBaseが参照]
    // TODO:腕を大きく上げた際の肩甲骨の丸みを表現するためのものだが、特に需要もないので消しても良い気がする
    eSTROKE_HOOK_TARGET_ARM_TO_BACK_L,          eSTROKE_HOOK_TARGET_ARM_TO_BACK_R,

    // 上半身のエプロンのつなぎ：[BlBaseが設定／UpBaseが参照]
    eSTROKE_HOOK_TARGET_APRON_UP_OUT_L,         eSTROKE_HOOK_TARGET_APRON_UP_OUT_R,
    eSTROKE_HOOK_TARGET_APRON_UP_IN_L,          eSTROKE_HOOK_TARGET_APRON_UP_IN_R,
    eSTROKE_HOOK_TARGET_APRON_UP_BORDER_L,      eSTROKE_HOOK_TARGET_APRON_UP_BORDER_R,

    // 下半身のエプロンのつなぎ：[BlBaseが設定／LwBaseが参照]
    eSTROKE_HOOK_TARGET_APRON_LOW_OUT_L,        eSTROKE_HOOK_TARGET_APRON_LOW_OUT_R,
    eSTROKE_HOOK_TARGET_APRON_LOW_IN_L,         eSTROKE_HOOK_TARGET_APRON_LOW_IN_R,
    eSTROKE_HOOK_TARGET_APRON_LOW_BORDER_L,     eSTROKE_HOOK_TARGET_APRON_LOW_BORDER_R,
    
    //-----------------------------------------------------------------------
    // 固定枠：上半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_UP_A,    eSTROKE_HOOK_TARGET_UP_B,    eSTROKE_HOOK_TARGET_UP_C,    eSTROKE_HOOK_TARGET_UP_D,
    eSTROKE_HOOK_TARGET_UP_E,    eSTROKE_HOOK_TARGET_UP_F,    eSTROKE_HOOK_TARGET_UP_G,    eSTROKE_HOOK_TARGET_UP_H,
    eSTROKE_HOOK_TARGET_UP_I,    eSTROKE_HOOK_TARGET_UP_J,    eSTROKE_HOOK_TARGET_UP_K,    eSTROKE_HOOK_TARGET_UP_L,
    eSTROKE_HOOK_TARGET_UP_M,    eSTROKE_HOOK_TARGET_UP_N,    eSTROKE_HOOK_TARGET_UP_O,    eSTROKE_HOOK_TARGET_UP_P,
    eSTROKE_HOOK_TARGET_UP_Q,    eSTROKE_HOOK_TARGET_UP_R,    eSTROKE_HOOK_TARGET_UP_S,    eSTROKE_HOOK_TARGET_UP_T,
    eSTROKE_HOOK_TARGET_UP_U,    eSTROKE_HOOK_TARGET_UP_V,    eSTROKE_HOOK_TARGET_UP_W,    eSTROKE_HOOK_TARGET_UP_X,
    eSTROKE_HOOK_TARGET_UP_Y,    eSTROKE_HOOK_TARGET_UP_Z,

    //-----------------------------------------------------------------------
    // 固定枠：下半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_LOW_A,   eSTROKE_HOOK_TARGET_LOW_B,   eSTROKE_HOOK_TARGET_LOW_C,   eSTROKE_HOOK_TARGET_LOW_D,
    eSTROKE_HOOK_TARGET_LOW_E,   eSTROKE_HOOK_TARGET_LOW_F,   eSTROKE_HOOK_TARGET_LOW_G,   eSTROKE_HOOK_TARGET_LOW_H,
    eSTROKE_HOOK_TARGET_LOW_I,   eSTROKE_HOOK_TARGET_LOW_J,   eSTROKE_HOOK_TARGET_LOW_K,   eSTROKE_HOOK_TARGET_LOW_L,
    eSTROKE_HOOK_TARGET_LOW_M,   eSTROKE_HOOK_TARGET_LOW_N,   eSTROKE_HOOK_TARGET_LOW_O,   eSTROKE_HOOK_TARGET_LOW_P,
    eSTROKE_HOOK_TARGET_LOW_Q,   eSTROKE_HOOK_TARGET_LOW_R,   eSTROKE_HOOK_TARGET_LOW_S,   eSTROKE_HOOK_TARGET_LOW_T,
    eSTROKE_HOOK_TARGET_LOW_U,   eSTROKE_HOOK_TARGET_LOW_V,   eSTROKE_HOOK_TARGET_LOW_W,   eSTROKE_HOOK_TARGET_LOW_X,
    eSTROKE_HOOK_TARGET_LOW_Y,   eSTROKE_HOOK_TARGET_LOW_Z,

    //-----------------------------------------------------------------------
    // 固定枠：スーツ（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_SUIT_A,   eSTROKE_HOOK_TARGET_SUIT_B,   eSTROKE_HOOK_TARGET_SUIT_C,   eSTROKE_HOOK_TARGET_SUIT_D,
    eSTROKE_HOOK_TARGET_SUIT_E,   eSTROKE_HOOK_TARGET_SUIT_F,   eSTROKE_HOOK_TARGET_SUIT_G,   eSTROKE_HOOK_TARGET_SUIT_H,
    eSTROKE_HOOK_TARGET_SUIT_I,   eSTROKE_HOOK_TARGET_SUIT_J,   eSTROKE_HOOK_TARGET_SUIT_K,   eSTROKE_HOOK_TARGET_SUIT_L,
    eSTROKE_HOOK_TARGET_SUIT_M,   eSTROKE_HOOK_TARGET_SUIT_N,   eSTROKE_HOOK_TARGET_SUIT_O,   eSTROKE_HOOK_TARGET_SUIT_P,
    eSTROKE_HOOK_TARGET_SUIT_Q,   eSTROKE_HOOK_TARGET_SUIT_R,   eSTROKE_HOOK_TARGET_SUIT_S,   eSTROKE_HOOK_TARGET_SUIT_T,
    eSTROKE_HOOK_TARGET_SUIT_U,   eSTROKE_HOOK_TARGET_SUIT_V,   eSTROKE_HOOK_TARGET_SUIT_W,   eSTROKE_HOOK_TARGET_SUIT_X,
    eSTROKE_HOOK_TARGET_SUIT_Y,   eSTROKE_HOOK_TARGET_SUIT_Z,

    //-----------------------------------------------------------------------
    // 固定枠：腕（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[ARM_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_ARM_A_0,  eSTROKE_HOOK_TARGET_ARM_A_1,  eSTROKE_HOOK_TARGET_ARM_B_0,  eSTROKE_HOOK_TARGET_ARM_B_1,
    eSTROKE_HOOK_TARGET_ARM_C_0,  eSTROKE_HOOK_TARGET_ARM_C_1,  eSTROKE_HOOK_TARGET_ARM_D_0,  eSTROKE_HOOK_TARGET_ARM_D_1,
    eSTROKE_HOOK_TARGET_ARM_E_0,  eSTROKE_HOOK_TARGET_ARM_E_1,  eSTROKE_HOOK_TARGET_ARM_F_0,  eSTROKE_HOOK_TARGET_ARM_F_1,
    eSTROKE_HOOK_TARGET_ARM_G_0,  eSTROKE_HOOK_TARGET_ARM_G_1,  eSTROKE_HOOK_TARGET_ARM_H_0,  eSTROKE_HOOK_TARGET_ARM_H_1,
    eSTROKE_HOOK_TARGET_ARM_I_0,  eSTROKE_HOOK_TARGET_ARM_I_1,  eSTROKE_HOOK_TARGET_ARM_J_0,  eSTROKE_HOOK_TARGET_ARM_J_1,
    eSTROKE_HOOK_TARGET_ARM_K_0,  eSTROKE_HOOK_TARGET_ARM_K_1,  eSTROKE_HOOK_TARGET_ARM_L_0,  eSTROKE_HOOK_TARGET_ARM_L_1,
    eSTROKE_HOOK_TARGET_ARM_M_0,  eSTROKE_HOOK_TARGET_ARM_M_1,  eSTROKE_HOOK_TARGET_ARM_N_0,  eSTROKE_HOOK_TARGET_ARM_N_1,
    eSTROKE_HOOK_TARGET_ARM_O_0,  eSTROKE_HOOK_TARGET_ARM_O_1,  eSTROKE_HOOK_TARGET_ARM_P_0,  eSTROKE_HOOK_TARGET_ARM_P_1,
    eSTROKE_HOOK_TARGET_ARM_Q_0,  eSTROKE_HOOK_TARGET_ARM_Q_1,  eSTROKE_HOOK_TARGET_ARM_R_0,  eSTROKE_HOOK_TARGET_ARM_R_1,
    eSTROKE_HOOK_TARGET_ARM_S_0,  eSTROKE_HOOK_TARGET_ARM_S_1,  eSTROKE_HOOK_TARGET_ARM_T_0,  eSTROKE_HOOK_TARGET_ARM_T_1,
    eSTROKE_HOOK_TARGET_ARM_U_0,  eSTROKE_HOOK_TARGET_ARM_U_1,  eSTROKE_HOOK_TARGET_ARM_V_0,  eSTROKE_HOOK_TARGET_ARM_V_1,
    eSTROKE_HOOK_TARGET_ARM_W_0,  eSTROKE_HOOK_TARGET_ARM_W_1,  eSTROKE_HOOK_TARGET_ARM_X_0,  eSTROKE_HOOK_TARGET_ARM_X_1,
    eSTROKE_HOOK_TARGET_ARM_Y_0,  eSTROKE_HOOK_TARGET_ARM_Y_1,  eSTROKE_HOOK_TARGET_ARM_Z_0,  eSTROKE_HOOK_TARGET_ARM_Z_1,

    //-----------------------------------------------------------------------
    // 固定枠：脚（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[LEG_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-----------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_LEG_A_0,  eSTROKE_HOOK_TARGET_LEG_A_1,  eSTROKE_HOOK_TARGET_LEG_B_0,  eSTROKE_HOOK_TARGET_LEG_B_1,
    eSTROKE_HOOK_TARGET_LEG_C_0,  eSTROKE_HOOK_TARGET_LEG_C_1,  eSTROKE_HOOK_TARGET_LEG_D_0,  eSTROKE_HOOK_TARGET_LEG_D_1,
    eSTROKE_HOOK_TARGET_LEG_E_0,  eSTROKE_HOOK_TARGET_LEG_E_1,  eSTROKE_HOOK_TARGET_LEG_F_0,  eSTROKE_HOOK_TARGET_LEG_F_1,
    eSTROKE_HOOK_TARGET_LEG_G_0,  eSTROKE_HOOK_TARGET_LEG_G_1,  eSTROKE_HOOK_TARGET_LEG_H_0,  eSTROKE_HOOK_TARGET_LEG_H_1,
    eSTROKE_HOOK_TARGET_LEG_I_0,  eSTROKE_HOOK_TARGET_LEG_I_1,  eSTROKE_HOOK_TARGET_LEG_J_0,  eSTROKE_HOOK_TARGET_LEG_J_1,
    eSTROKE_HOOK_TARGET_LEG_K_0,  eSTROKE_HOOK_TARGET_LEG_K_1,  eSTROKE_HOOK_TARGET_LEG_L_0,  eSTROKE_HOOK_TARGET_LEG_L_1,
    eSTROKE_HOOK_TARGET_LEG_M_0,  eSTROKE_HOOK_TARGET_LEG_M_1,  eSTROKE_HOOK_TARGET_LEG_N_0,  eSTROKE_HOOK_TARGET_LEG_N_1,
    eSTROKE_HOOK_TARGET_LEG_O_0,  eSTROKE_HOOK_TARGET_LEG_O_1,  eSTROKE_HOOK_TARGET_LEG_P_0,  eSTROKE_HOOK_TARGET_LEG_P_1,
    eSTROKE_HOOK_TARGET_LEG_Q_0,  eSTROKE_HOOK_TARGET_LEG_Q_1,  eSTROKE_HOOK_TARGET_LEG_R_0,  eSTROKE_HOOK_TARGET_LEG_R_1,
    eSTROKE_HOOK_TARGET_LEG_S_0,  eSTROKE_HOOK_TARGET_LEG_S_1,  eSTROKE_HOOK_TARGET_LEG_T_0,  eSTROKE_HOOK_TARGET_LEG_T_1,
    eSTROKE_HOOK_TARGET_LEG_U_0,  eSTROKE_HOOK_TARGET_LEG_U_1,  eSTROKE_HOOK_TARGET_LEG_V_0,  eSTROKE_HOOK_TARGET_LEG_V_1,
    eSTROKE_HOOK_TARGET_LEG_W_0,  eSTROKE_HOOK_TARGET_LEG_W_1,  eSTROKE_HOOK_TARGET_LEG_X_0,  eSTROKE_HOOK_TARGET_LEG_X_1,
    eSTROKE_HOOK_TARGET_LEG_Y_0,  eSTROKE_HOOK_TARGET_LEG_Y_1,  eSTROKE_HOOK_TARGET_LEG_Z_0,  eSTROKE_HOOK_TARGET_LEG_Z_1,

    //-----------------------------------------------------------------------
    // 特殊枠：
    //-----------------------------------------------------------------------

    // フリル用（※フリルの処理中にのみ利用される／データへの指定は不可）
    eSTROKE_HOOK_TARGET_FRILL_TEMP_A,       eSTROKE_HOOK_TARGET_FRILL_TEMP_B,
    eSTROKE_HOOK_TARGET_FRILL_CONNECT_0,    eSTROKE_HOOK_TARGET_FRILL_CONNECT_1,
    
    // ミラーコピー用（※ミラコピー中にのみ利用される／データへの指定は不可）
    eSTROKE_HOOK_TARGET_MC_HEAD,            eSTROKE_HOOK_TARGET_MC_TAIL,

    eSTROKE_HOOK_TARGET_MAX,
    
    // 調整された[eSTROKE_HOOK_TARGET_PART_A〜Z]のデータは、ここ以降に配置される
};

// テンポラリの開始と終了ラベル
#define eSTROKE_HOOK_TARGET_TEMP_START              eSTROKE_HOOK_TARGET_TEMP_A
#define eSTROKE_HOOK_TARGET_TEMP_END                eSTROKE_HOOK_TARGET_TEMP_Z

// 左右の区別をする範囲：パーツ
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_PART    eSTROKE_HOOK_TARGET_PART_A
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_PART      eSTROKE_HOOK_TARGET_PART_Z

// 左右の区別をする範囲：特定
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_FIXED   eSTROKE_HOOK_TARGET_NECK_TO_HEAD_L
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_FIXED     eSTROKE_HOOK_TARGET_APRON_LOW_BORDER_R

// 左右の区別をする範囲：腕
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM     eSTROKE_HOOK_TARGET_ARM_A_0
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_ARM       eSTROKE_HOOK_TARGET_ARM_Z_1

// 左右の区別をする範囲：脚
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG     eSTROKE_HOOK_TARGET_LEG_A_0
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_LEG       eSTROKE_HOOK_TARGET_LEG_Z_1

// 確保総数（※パーツ×スロットインデックス毎に１セット確保する）
#define eSTROKE_HOOK_TARGET_NUM_IN_PART             (eSTROKE_HOOK_TARGET_PART_Z-eSTROKE_HOOK_TARGET_PART_A+1)
#define eSTROKE_HOOK_TARGET_TOTAL_MAX               (eSTROKE_HOOK_TARGET_MAX+2*eBD_SLOT_MAX*eSTROKE_HOOK_TARGET_NUM_IN_PART)

// 有効性の判定（※スロットインデックスを加味した値に対しては[FIXED]により判定すること）
#define IS_STROKE_HOOK_TARGET_VALID( _t )           ((_t)>=(eSTROKE_HOOK_TARGET)0 && (_t)<eSTROKE_HOOK_TARGET_MAX)
#define IS_STROKE_HOOK_TARGET_FIXED_VALID( _t )     ((_t)>=0 && (_t)<eSTROKE_HOOK_TARGET_TOTAL_MAX)

//----------------------------------------------------------------------
// タッチ対象
//----------------------------------------------------------------------
// ストロークの描画時に各座標を登録しておき、その座標に対してタッチ処理を行う仕組み
//----------------------------------------------------------------------
enum eSTROKE_TOUCH_TARGET{
    eSTROKE_TOUCH_TARGET_INVALID = -1,
    
    //-------------------------------------------------------------------
    // 一時枠（※レイヤー単位でクリアされる）
    // フックとは違いパーツ枠は持たない（※タッチはストロークなので容量的に厳しい）
    // メインとサブの概念があり、以外と枠が足りなくなるので一時枠のみ２倍確保しておく
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_TEMP_A,    eSTROKE_TOUCH_TARGET_TEMP_B,    eSTROKE_TOUCH_TARGET_TEMP_C,    eSTROKE_TOUCH_TARGET_TEMP_D,
    eSTROKE_TOUCH_TARGET_TEMP_E,    eSTROKE_TOUCH_TARGET_TEMP_F,    eSTROKE_TOUCH_TARGET_TEMP_G,    eSTROKE_TOUCH_TARGET_TEMP_H,
    eSTROKE_TOUCH_TARGET_TEMP_I,    eSTROKE_TOUCH_TARGET_TEMP_J,    eSTROKE_TOUCH_TARGET_TEMP_K,    eSTROKE_TOUCH_TARGET_TEMP_L,
    eSTROKE_TOUCH_TARGET_TEMP_M,    eSTROKE_TOUCH_TARGET_TEMP_N,    eSTROKE_TOUCH_TARGET_TEMP_O,    eSTROKE_TOUCH_TARGET_TEMP_P,

    //-------------------------------------------------------------------
    // 固定枠：顔／頭（※パーツ単位での細かな組み替えが行われるので用途を限定）
    //（※下記以外の要素で、オーダーをまたいだタッチを利用したい際は一時枠で工夫すること）
    //-------------------------------------------------------------------
    // 顔（※[FcBase]から[FcUnder]（生際から顎にかけてのライン）を左右で指定する想定）
    eSTROKE_TOUCH_TARGET_FACE_0,    eSTROKE_TOUCH_TARGET_FACE_1,
    
    // 頭（※[HdBase]から[HdTop]（頭頂から襟足にかけてのライン）を左右で指定する想定）
    eSTROKE_TOUCH_TARGET_HEAD_0,    eSTROKE_TOUCH_TARGET_HEAD_1,
    
    //-------------------------------------------------------------------
    // 固定枠：上半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_UP_A,    eSTROKE_TOUCH_TARGET_UP_B,    eSTROKE_TOUCH_TARGET_UP_C,    eSTROKE_TOUCH_TARGET_UP_D,
    eSTROKE_TOUCH_TARGET_UP_E,    eSTROKE_TOUCH_TARGET_UP_F,    eSTROKE_TOUCH_TARGET_UP_G,    eSTROKE_TOUCH_TARGET_UP_H,

    //-------------------------------------------------------------------
    // 固定枠：下半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_LOW_A,    eSTROKE_TOUCH_TARGET_LOW_B,    eSTROKE_TOUCH_TARGET_LOW_C,    eSTROKE_TOUCH_TARGET_LOW_D,
    eSTROKE_TOUCH_TARGET_LOW_E,    eSTROKE_TOUCH_TARGET_LOW_F,    eSTROKE_TOUCH_TARGET_LOW_G,    eSTROKE_TOUCH_TARGET_LOW_H,

    //-------------------------------------------------------------------
    // 固定枠：スーツ（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_SUIT_A,    eSTROKE_TOUCH_TARGET_SUIT_B,    eSTROKE_TOUCH_TARGET_SUIT_C,    eSTROKE_TOUCH_TARGET_SUIT_D,
    eSTROKE_TOUCH_TARGET_SUIT_E,    eSTROKE_TOUCH_TARGET_SUIT_F,    eSTROKE_TOUCH_TARGET_SUIT_G,    eSTROKE_TOUCH_TARGET_SUIT_H,

    //-------------------------------------------------------------------
    // 固定枠：腕（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[ARM_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_ARM_A_0,   eSTROKE_TOUCH_TARGET_ARM_A_1,    eSTROKE_TOUCH_TARGET_ARM_B_0,   eSTROKE_TOUCH_TARGET_ARM_B_1,
    eSTROKE_TOUCH_TARGET_ARM_C_0,   eSTROKE_TOUCH_TARGET_ARM_C_1,    eSTROKE_TOUCH_TARGET_ARM_D_0,   eSTROKE_TOUCH_TARGET_ARM_D_1,
    eSTROKE_TOUCH_TARGET_ARM_E_0,   eSTROKE_TOUCH_TARGET_ARM_E_1,    eSTROKE_TOUCH_TARGET_ARM_F_0,   eSTROKE_TOUCH_TARGET_ARM_F_1,
    eSTROKE_TOUCH_TARGET_ARM_G_0,   eSTROKE_TOUCH_TARGET_ARM_G_1,    eSTROKE_TOUCH_TARGET_ARM_H_0,   eSTROKE_TOUCH_TARGET_ARM_H_1,

    //-------------------------------------------------------------------
    // 固定枠：脚（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[LEG_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET_LEG_A_0,   eSTROKE_TOUCH_TARGET_LEG_A_1,    eSTROKE_TOUCH_TARGET_LEG_B_0,   eSTROKE_TOUCH_TARGET_LEG_B_1,
    eSTROKE_TOUCH_TARGET_LEG_C_0,   eSTROKE_TOUCH_TARGET_LEG_C_1,    eSTROKE_TOUCH_TARGET_LEG_D_0,   eSTROKE_TOUCH_TARGET_LEG_D_1,
    eSTROKE_TOUCH_TARGET_LEG_E_0,   eSTROKE_TOUCH_TARGET_LEG_E_1,    eSTROKE_TOUCH_TARGET_LEG_F_0,   eSTROKE_TOUCH_TARGET_LEG_F_1,
    eSTROKE_TOUCH_TARGET_LEG_G_0,   eSTROKE_TOUCH_TARGET_LEG_G_1,    eSTROKE_TOUCH_TARGET_LEG_H_0,   eSTROKE_TOUCH_TARGET_LEG_H_1,

    eSTROKE_TOUCH_TARGET_MAX,
};

// テンポラリの開始と終了ラベル
#define eSTROKE_TOUCH_TARGET_TEMP_START             eSTROKE_TOUCH_TARGET_TEMP_A
#define eSTROKE_TOUCH_TARGET_TEMP_END               eSTROKE_TOUCH_TARGET_TEMP_H

// 左右の区別をする範囲：腕
#define eSTROKE_TOUCH_TARGET_CHECK_START_FOR_ARM    eSTROKE_TOUCH_TARGET_ARM_A_0
#define eSTROKE_TOUCH_TARGET_CHECK_END_FOR_ARM      eSTROKE_TOUCH_TARGET_ARM_H_1

// 左右の区別をする範囲：脚
#define eSTROKE_TOUCH_TARGET_CHECK_START_FOR_LEG    eSTROKE_TOUCH_TARGET_LEG_A_0
#define eSTROKE_TOUCH_TARGET_CHECK_END_FOR_LEG      eSTROKE_TOUCH_TARGET_LEG_H_1

// トータルデータ数（※指定枠と同じではあるが一応）
#define eSTROKE_TOUCH_TARGET_TOTAL_MAX              eSTROKE_TOUCH_TARGET_MAX

// 有効性の判定（※スロットインデックスを加味した値に対しては[FIXED]により判定すること）
#define IS_STROKE_TOUCH_TARGET_VALID( _t )          ((_t)>=(eSTROKE_TOUCH_TARGET)0 && (_t)<eSTROKE_TOUCH_TARGET_MAX)
#define IS_STROKE_TOUCH_TARGET_FIXED_VALID( _t )    ((_t)>=0 && (_t)<eSTROKE_TOUCH_TARGET_TOTAL_MAX)

//-------------------------------------------------------------------------------
// ガイド対象
//-------------------------------------------------------------------------------
// パーツを横断して同じガイドに登録するためのしくみ（※[INVALID]で直接ガイドに登録する）
//-------------------------------------------------------------------------------
enum eSTROKE_GUIDE_TARGET{
    eSTROKE_GUIDE_TARGET_INVALID = -1,

    // 一時枠
    eSTROKE_GUIDE_TARGET_TEMP_A,    eSTROKE_GUIDE_TARGET_TEMP_B,    eSTROKE_GUIDE_TARGET_TEMP_C,    eSTROKE_GUIDE_TARGET_TEMP_D,

    // 固定枠：顔
    eSTROKE_GUIDE_TARGET_FACE_A,    eSTROKE_GUIDE_TARGET_FACE_B,    eSTROKE_GUIDE_TARGET_FACE_C,    eSTROKE_GUIDE_TARGET_FACE_D,

    // 固定枠：頭
    eSTROKE_GUIDE_TARGET_HEAD_A,    eSTROKE_GUIDE_TARGET_HEAD_B,    eSTROKE_GUIDE_TARGET_HEAD_C,    eSTROKE_GUIDE_TARGET_HEAD_D,

    // 固定枠：体（上半身〜腹〜下半身で共有）
    eSTROKE_GUIDE_TARGET_BODY_A,    eSTROKE_GUIDE_TARGET_BODY_B,    eSTROKE_GUIDE_TARGET_BODY_C,    eSTROKE_GUIDE_TARGET_BODY_D,

    // スーツ（上半身〜腹〜下半身で共有）
    eSTROKE_GUIDE_TARGET_SUIT_A,    eSTROKE_GUIDE_TARGET_SUIT_B,    eSTROKE_GUIDE_TARGET_SUIT_C,    eSTROKE_GUIDE_TARGET_SUIT_D,

    // 腕（左右）
    eSTROKE_GUIDE_TARGET_ARM_A_0,   eSTROKE_GUIDE_TARGET_ARM_A_1,    eSTROKE_GUIDE_TARGET_ARM_B_0,   eSTROKE_GUIDE_TARGET_ARM_B_1,
    eSTROKE_GUIDE_TARGET_ARM_C_0,   eSTROKE_GUIDE_TARGET_ARM_C_1,    eSTROKE_GUIDE_TARGET_ARM_D_0,   eSTROKE_GUIDE_TARGET_ARM_D_1,

    // 脚（左右）
    eSTROKE_GUIDE_TARGET_LEG_A_0,   eSTROKE_GUIDE_TARGET_LEG_A_1,    eSTROKE_GUIDE_TARGET_LEG_B_0,   eSTROKE_GUIDE_TARGET_LEG_B_1,
    eSTROKE_GUIDE_TARGET_LEG_C_0,   eSTROKE_GUIDE_TARGET_LEG_C_1,    eSTROKE_GUIDE_TARGET_LEG_D_0,   eSTROKE_GUIDE_TARGET_LEG_D_1,

    eSTROKE_GUIDE_TARGET_MAX,
};

// テンポラリの開始と終了ラベル
#define eSTROKE_GUIDE_TARGET_TEMP_START             eSTROKE_GUIDE_TARGET_TEMP_A
#define eSTROKE_GUIDE_TARGET_TEMP_END               eSTROKE_GUIDE_TARGET_TEMP_D

// 左右の区別をする範囲：腕
#define eSTROKE_GUIDE_TARGET_CHECK_START_FOR_ARM    eSTROKE_GUIDE_TARGET_ARM_A_0
#define eSTROKE_GUIDE_TARGET_CHECK_END_FOR_ARM      eSTROKE_GUIDE_TARGET_ARM_D_1

// 左右の区別をする範囲：脚
#define eSTROKE_GUIDE_TARGET_CHECK_START_FOR_LEG    eSTROKE_GUIDE_TARGET_LEG_A_0
#define eSTROKE_GUIDE_TARGET_CHECK_END_FOR_LEG      eSTROKE_GUIDE_TARGET_LEG_D_1

// トータルデータ数（※指定枠と同じではあるが一応）
#define eSTROKE_GUIDE_TARGET_TOTAL_MAX              eSTROKE_GUIDE_TARGET_MAX

// 有効性の判定（※スロットインデックスを加味した値に対しては[FIXED]により判定すること）
#define IS_STROKE_GUIDE_TARGET_VALID( _t )          ((_t)>=(eSTROKE_GUIDE_TARGET)0 && (_t)<eSTROKE_GUIDE_TARGET_MAX)
#define IS_STROKE_GUIDE_TARGET_FIXED_VALID( _t )    ((_t)>=0 && (_t)<eSTROKE_GUIDE_TARGET_TOTAL_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelStrokeType[];
extern const char* g_pArrLabelStrokeFrill[];

extern const char* g_pArrLabelStrokeHookTarget[];
extern const char* g_pArrLabelStrokeTouchTarget[];
extern const char* g_pArrLabelStrokeGuideTarget[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __STROKE_CONST_HPP__ */
