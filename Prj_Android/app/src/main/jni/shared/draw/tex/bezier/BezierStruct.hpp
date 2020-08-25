/*+----------------------------------------------------------------+
  |	Title:		BezierStruct.hpp [共通環境]
  |	Comment:	ベジェ曲線関連構造体
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_STRUCT_HPP__
#define __BEZIER_STRUCT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/tex/adjust/DelayLog.hpp"
#include "draw/tex/adjust/JointPoint.hpp"
#include "draw/tex/adjust/AnglePlane.hpp"
#include "draw/tex/stroke/Stroke.hpp"
#include "draw/tex/fill/Fill.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ベジェ基本パラメータ（※一連の絵を生成する流れで共通の値）
// [CBmpGenerator]により生成され[CBezier]に関わる処理で参照される
//-----------------------------------------------------------------
typedef struct{
    //-----------------------------------------------------------------
    // 各種バッファ（※[eTEX_DATA_P8D]形式）
    //-----------------------------------------------------------------
    BYTE* pBufLine;             // 線バッファ
    BYTE* pBufColor;            // 塗りバッファ（※テストバッファとしても利用される）
    BYTE* pBufFillGuide;        // 塗りガイド（※塗りバッファに出力する前の下塗り用）
    BYTE* pBufFillGuard;        // 塗りガード（※塗りバッファへの出力制限用）
    BYTE* pBufTemp;             // テンポラリ
    
    // 出力画像サイズ
    int pixelRate;              // ピクセル割合（※論理ピクセル単位に対する実際のピクセル数＝GM_P_RATE）
    int texW0;                  // テクスチャ名目サイズ：横幅（※ピクセル割合適用後のサイズ＝論理座標変換時に利用される）
    int texH0;                  // テクスチャ名目サイズ：縦幅（※ピクセル割合適用後のサイズ＝論理座標変換時に利用される）
    int texW;                   // テクスチャ実サイズ：横幅（※ピクセル割合適用後の余白込みのサイズ＝実描画時に利用される）
    int texH;                   // テクスチャ実サイズ：縦幅（※ピクセル割合適用後の余白込みのサイズ＝実描画時に利用される）
    
    // ストロークサイズ
    float strokeScale;          // 拡縮率（※盤面全体を拡縮する際にストロークの印象が同じになるように）
    
    //-----------------------------------------------------------------
    // 呼び出し情報（※[stBMP_GEN_CREATE_PARAM]の値をそのまま保持）
    //-----------------------------------------------------------------
    bool isFlipH;               // ヨコ反転
    bool isFlipV;               // タテ反転
    float rot;                  // 回転（※原点はバッファの中心）
    float scale;                // スケール

    // オプション
    bool arrOption[eBD_OPTION_MAX][BD_SLOT_INDEX_MAX];

    //-----------------------------------------------------------------
    // 調整率：ベジェ描画時に座標や角度を調整する割合（※画像に対して一律で適用される）
    //（※[0.0f]を基準として[-1.0f 〜 +1.0f]の値域で指定する）
    //-----------------------------------------------------------------
    float adjustRateH;          // ヨコ体型：[-1.0:細い] 〜 [0.0:基準] 〜 [+1.0:太い]
    float adjustRateV;          // タテ体型：[-1.0:低い] 〜 [0.0:基準] 〜 [+1.0:高い]
    float adjustRateLR;         // 左右の傾き：[-1.0:左側に傾く]〜[0.0:基準]〜[+1.0:右側に傾く]
    float adjustRateUD;         // 上下の傾き：[-1.0:上側に傾く]〜[0.0:基準]〜[+1.0:下側に傾く]

    //---------------------------------------------------------
    // 調整を補助するデータ
    //---------------------------------------------------------
    CDelayLog* pDelayLog;       // 遅延ログ（※[NULL]で無効＝利用しない）
    CJointPoint* pJointPoint;   // 結合ポイント（※[NULL]で無効＝利用しない）
    CAnglePlane* pAnglePlane;   // 傾き面（※[NULL]で無効＝利用しない）
} stBEZIER_BASE_PARAM;

// クリア
#define CLEAR_BEZIER_BASE_PARAM( _p )                   \
    memset( _p, 0, sizeof(stBEZIER_BASE_PARAM) );       \
    (_p)->strokeScale = 1.0f

//----------------------------------------------------------
// レイヤー調整パラメータ（※各レイヤー[CLayerData]毎に変化するもの）
//（※レイヤーデータに含まれる論理座標情報を実座標に変換する際の情報）
//----------------------------------------------------------
typedef struct{
    // 対象
    eBD_SLOT slot;                      // スロット
    int      slotIndex;                 // スロットインデックス
    
    eBD_SLOT parentSlot;                // 親スロット：全体傾け用
    int      parentSlotIndex;           // 親スロットインデックス：全体傾け用
    
    // 調整値（※テンションとサイズはオーダー登録時に解決されている）
    float fixedAdjustRateH;             // 調整値：H（※値の整合性が取れている状態）
    float fixedAdjustRateV;             // 調整値：V（※値の整合性が取れている状態）
    float fixedAngleRateLR;             // 傾け：左右の割合（※値の整合性が取れている状態）
    float fixedAngleRateUD;             // 傾け：上下の割合（※値の整合性が取れている状態）

    // 実座標変換用の値
    float pixelOfsX;                    // 表示オフセット：X
    float pixelOfsY;                    // 表示オフセット：Y
    float pixelScaleX;                  // 表示スケール：X
    float pixelScaleY;                  // 表示スケール：Y
    float pixelRot;                     // 回転：角度

    // 結合ポイント処理用の値（※変換処理の途中経過の値）
    float scaleForBdpd;                 // BDPDの基本スケール
    int rateXForBdpd;                   // BDPDの基本座標：X
    int rateYForBdpd;                   // BDPDの基本座標：Y

    // ストロークサイズ
    float strokeScale;                  // 拡縮率（※レイヤー全体を拡縮する際にストロークの印象が同じになるように）
} stBEZIER_LAYER_PARAM;

// クリア
#define CLEAR_BEZIER_LAYER_PARAM( _p )                  \
    memset( _p, 0, sizeof(stBEZIER_LAYER_PARAM) );      \
    (_p)->slot = eBD_SLOT_INVALID;                      \
    (_p)->slotIndex = -1;                               \
    (_p)->parentSlot = eBD_SLOT_INVALID;                \
    (_p)->parentSlotIndex = -1;                         \
    (_p)->pixelScaleX = 1.0f;                           \
    (_p)->pixelScaleY = 1.0f;                           \
    (_p)->scaleForBdpd = 1.0f;                          \
    (_p)->strokeScale = 1.0f

//-----------------------------------------------------------------
// アンカーポイント（※描画用のデータ：解像度や割合座標の処理が反映された値）
//（※論理データ[CAnchorPointData]に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_ANCHOR_POINT{
    // 座標
    float           x, y;                   // 座標：テクスチャの左上を原点とした値（※実ピクセル座標）
    float           xIn, yIn;               // 方向線：進入（※[x,y]からの相対位置）
    float           xOut, yOut;             // 方向線：出発（※[x,y]からの相対位置）
    
    // ストローク（※[CBrush]から取り出して調整した値）
    eSTROKE_TYPE    type;                   // ストロークタイプ（※[eSTROKE_TYPE_OFF]で出力しない＝以下のブラシ値が不要になる）
    float           size;                   // ストローク基本サイズ
    float           ofsRange;               // ストロークのオフセット範囲（※ドット出力位置が取りうる範囲）
    int             dotPutMax;              // ドット出力最大（※出力する画素の濃さの最大値）
    int             dotPutRateBase;         // ドット出力割合：基本（※出力済みの画素の適用率：百分率）
    int             dotPutRateStrong;       // ドット出力割合：濃い方（※出力済み画素／ブラシ画素の濃い方への適用率：百分率）
    int             dotPutRateWeak;         // ドット出力割合：薄い方（※出力済み画素／ブラシ画素の薄い方への適用率：百分率）
    bool            isDotPutForce;          // 強制的にドットを出力するか？（※テストを無視して必ずドットを出力する）
    bool            isDotPutWhetherMax;     // 画素が最大値に到達していてもドットを出力するか？（※デフォルトでは最大濃さになった画素は処理対象から外れる）
    bool            isDotPutOnOutCol;       // 塗り出力されたドットの上にストロークを出力するか？（※直前の塗りで実際に出力された画素を対象にする）
    bool            isDotErase;             // 消去するか？（※画素の値を減算する）
    
    // ストロークのブレ（※ドット出力時のバネによるブレ値への適用率）
    float           shakeScaleForPos;       // 位置ブレの強さ（※この値が大きいほど歪んだストロークになる）
    float           shakeScaleForSize;      // サイズブレの強さ（※この値が大きいほど太さが歪なストロークになる）

    // パレット
    BYTE            stripeOrFrillMainPalGrp;    // ストライプパレットグループへのオフセット：メイン（※０で無効）
    BYTE            stripeOrFrillSubPalGrp;     // ストライプパレットグループへのオフセット：サブ（※０で無効）
    BYTE            workPathPalOfsS;            // ワークパス色のオフセット（※毎回判定するのは面倒なので値で持っておく）

    bool            bNoFillGuide;           // 塗りガイドを出力しないか？（※領域に関わらないストロークがガイドに影響しないように）
    bool            bLineRepairTest;        // 線修復テストを行うか？（※出力済みの色テクスチャにより欠けてしまったストロークへの重複描画）
    bool            bAsTouchStroke;         // タッチストロークとして扱う（※タッチオフ時は透明）
    
    // ガイド対象
    eSTROKE_GUIDE_TARGET guideTarget;       // ガイド対象

    // フック対象（※２点間をつなげる指定）
    eSTROKE_HOOK_TARGET hookTarget;         // フック対象（※データ指定＝任意の二点によるフック）
    eSTROKE_HOOK_TARGET systemHookTarget;   // フック対象（※システム指定＝結合ポイントによるフック）

    // ストロークサイズ（※[CAnchorPointData]から取り出して調整した値：値域は[0.0〜3.0]）
    float           strokeSize;             // ストロークサイズ：基本サイズ
    float           strokeStartRange;       // ストロークサイズ：開始サイズ適用範囲（※[0.0f〜stronStartRange]間でストロークサイズを補正する）
    float           strokeStartSize;        // ストロークサイズ：開始サイズ
    float           strokeEndRange;         // ストロークサイズ：終了サイズ適用範囲（※[1.0f-stronkeEndRange〜1.0f]間でストロークサイズを補正する）
    float           strokeEndSize;          // ストロークサイズ：終了サイズ

    // ストローク先端の塗りつぶしサイズ
    float           strokeEdgeFillSize;     // このＡＰの前後のストロークの角度が浅い場合に先端を埋めるサイズ

    // タッチ（※線による質感）
    eSTROKE_TOUCH_TARGET touchTarget;       // タッチ：対象
    eSTROKE_TOUCH_TARGET touchTargetSub;    // タッチ：サブ
    float           touchBasePos;           // タッチ：基本位置（※ストロークの開始位置）
    float           touchBaseSize;          // タッチ：基本サイズ（※ストロークの開始サイズ）
    float           touchBaseRot;           // タッチ：基本角度（※ストロークの開始角度）
    float           touchFrontArea;         // タッチ：前方の領域（※基本位置から前方へのタッチ適用領域割合＝1.0で全領域）
    float           touchFrontSizeOfs;      // タッチ：前方のサイズオフセット（※前方領域の最終サイズオフセット）
    float           touchFrontRotRate;      // タッチ：前方の角度適用率（※前方領域の角度適用割合）
    float           touchBackArea;          // タッチ：後方の領域（※基本位置から後方へのタッチ適用領域割合＝1.0で全領域）
    float           touchBackSizeOfs;       // タッチ：後方のサイズオフセット（※後方領域の最終サイズオフセット）
    float           touchBackRotRate;       // タッチ：後方の角度適用率（※後方領域の角度適用割合）
    int             touchFrontNum;          // タッチ：前方の要素数
    int             touchFrontSkip;         // タッチ：前方のスキップ数
    float           touchFrontBorderRate;   // タッチ：前方のボーダー割合
    int             touchBackNum;           // タッチ：後方の要素数
    int             touchBackSkip;          // タッチ：後方のスキップ数
    float           touchBackBorderRate;    // タッチ：後方のボーダー割合
    float           touchRandomOfsFor0;     // タッチ：ランダムオフセット０
    float           touchRandomOfsFor1;     // タッチ：ランダムオフセット１
    float           touchHeadSkipRate;      // タッチ：先頭のスキップ率
    float           touchTailSkipRate;      // タッチ：末尾のスキップ率
    float           frillWidthRateForMain;  // フリル：メインの横幅補正
    float           frillWidthRateForSub;   // フリル：メインの横幅補正
    
    // フリルのメイン／サブの横幅補正（※フリルの割り当て後の横幅を補正する／縦幅は通常のサイズで補正する想定）
    int             m_nFrillWidthRateForMain;
    int             m_nFrillWidthRateForSub;

    bool            isTouchPointReverse;    // タッチ：ポイントを反転登録するか？（※ＡＰの構成がタッチ点の並びと逆の場合に反転登録する指定）
    bool            isTouchSubReverse;      // タッチ：タッチ処理時にサブの座標を反転参照するか？
    bool            isFrillDirReverse;      // フリル：フック登録の反転

    // フリル（※タッチ指定に対してフリルを適用する／変数の役割はタッチと同様）
    eSTROKE_FRILL   frillMainId;            // フリル：メイン素材ID
    eSTROKE_FRILL   frillSubId;             // フリル：サブ素材ID

    // 傾け（※局所）
    int             angleLocalSearchKey;    // 傾け：検索キー
    float           angleLocalRateLR;       // 傾け：左右の割合（※実効値）
    float           angleLocalRateUD;       // 傾け：上下の割合（※実効値）

    // 傾け（※広域）
    int             angleSearchKey;         // 傾け：検索キー
    float           angleRateLR;            // 傾け：左右の割合（※実効値）
    float           angleRateUD;            // 傾け：上下の割合（※実効値）
    
    stBEZIER_ANCHOR_POINT* pPrev;           // 前の要素
    stBEZIER_ANCHOR_POINT* pNext;           // 次の要素
};

// クリア
#define CLEAR_BEZIER_ANCHOR_POINT( _p )                     \
    memset( _p, 0, sizeof(stBEZIER_ANCHOR_POINT) );         \
    (_p)->type = eSTROKE_TYPE_INVALID;                      \
    (_p)->hookTarget = eSTROKE_HOOK_TARGET_INVALID;         \
    (_p)->systemHookTarget = eSTROKE_HOOK_TARGET_INVALID;   \
    (_p)->touchTarget = eSTROKE_TOUCH_TARGET_INVALID;       \
    (_p)->frillId = eSTROKE_FRIL_INVALID

//-----------------------------------------------------------------
// 塗りつぶし点（※描画用のデータ：解像度や割合座標の処理がされた値を想定）
//（※論理データ[CFillPointData]の値に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_FILL_POINT{
    float           x, y;                   // 塗りつぶしの始点：テクスチャの左上を原点とした位置
    
    // 塗り情報
    eFILL_TYPE      type;                   // 塗りのスタイル
    BYTE            palGrp;                 // パレットグループ
    int             darkOfs;                // 塗りの明暗オフセット
    BYTE            workPathPalOfsS;        // ワークパス色のオフセット（※毎回判定するのは面倒なので値で持っておく）
    bool            isFillOnOutCol;         // 塗り出力された領域にのみ出力する

    // 塗りオプション
    eFILL_OPTION            option;                 // オプション：タイプ
    int                     optionDarkOfs;          // オプション：明暗オフセット
    eSTROKE_TOUCH_TARGET    optionTouchId;          // オプション：タッチ対象ID（※アンチ処理をする際はあらかじめタッチ点を登録しておく）
    float                   optionStrokeSize;       // オプション：ストロークサイズ（※アンチ処理のストロークサイズ）
    float                   optionStrokeRange;      // オプション：ストロークレンジ（※アンチ処理のストロークレンジ）
    float                   optionOfsX;             // オプション：オフセットＸ（※広域塗り／全体塗り時のオフセット）
    float                   optionOfsY;             // オプション：オフセットＹ（※広域塗り／全体塗り時のオフセット）
    
    // 傾け（※局所）
    int             angleLocalSearchKey;    // 傾け：検索キー
    float           angleLocalRateLR;       // 傾け：左右の割合（※実効値）
    float           angleLocalRateUD;       // 傾け：上下の割合（※実効値）
    
    // 傾け（※広域）
    int             angleSearchKey;         // 傾け：検索キー
    float           angleRateLR;            // 傾け：左右の割合（※実効値）
    float           angleRateUD;            // 傾け：上下の割合（※実効値）

    stBEZIER_FILL_POINT* pPrev;             // 前の要素
    stBEZIER_FILL_POINT* pNext;             // 次の要素
};

// クリア
#define CLEAR_BEZIER_FILL_POINT( _p )                       \
    memset( _p, 0, sizeof(stBEZIER_FILL_POINT) );           \
    (_p)->type = eFILL_TYPE_INVALID;                        \
    (_p)->option = eFILL_OPTION_INVALID;                    \
    (_p)->optionTouchId = eSTROKE_TOUCH_TARGET_INVALID;

//-----------------------------------------------------------------
// スロット点（※描画用のデータ：解像度や割合座標の処理がされた値を想定）
//（※論理データ[CSlotPointData]の値に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_SLOT_POINT{
    eBD_SLOT    slot;                   // スロット
    int         slotIndex;              // スロットインデックス
    int         rateOfsX, rateOfsY;     // オフセット割合座標の最終値

    float       x, y;                   // 基準となる点：テクスチャの左上を原点とした位置
    bool        isFlipH, isFlipV;       // 反転フラグ
    BYTE        workPathPalOfs;         // ワークパス色のオフセット（※毎回判定するのは面倒なので値で持って置く）
    
    stBEZIER_SLOT_POINT* pPrev;         // 前の要素
    stBEZIER_SLOT_POINT* pNext;         // 次の要素
};

// クリア
#define CLEAR_BEZIER_SLOT_POINT( _p )                   \
    memset( _p, 0, sizeof(stBEZIER_SLOT_POINT) );       \
    (_p)->slot = eBD_SLOT_INVALID;                      \
    (_p)->slotIndex = -1

//------------------------------
// 分割点（※ストロークの分割ポイント）
//------------------------------
typedef struct{
    float x, y;                     // 座標：テクスチャの左上を原点とした位置（※傾きがある場合は適用済みの値となる）
    float x0, y0;                   // 元座標（※傾き適用前の値＝タッチの処理で利用する）
    float strokeSize;               // ストロークサイズ
    bool isEdgeFillCheck;           // エッジ埋めの確認をするか？
    stBEZIER_ANCHOR_POINT* pAP;     // 分割元のアンカーポイント（※描画情報参照用）
} stBEZIER_DIVISION_POINT;

// クリア
#define CLEAR_BEZIER_DIVISION_POINT( _p )               \
    memset( _p, 0, sizeof(stBEZIER_SLOT_POINT) )

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_STRUCT_HPP__ */
