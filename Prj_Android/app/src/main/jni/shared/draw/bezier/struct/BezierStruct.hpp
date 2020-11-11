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

#include "draw/bezier/stroke/Stroke.hpp"
#include "draw/bezier/fill/Fill.hpp"
#include "draw/bezier/adjust/DelayLog.hpp"
#include "draw/bezier/adjust/JointPoint.hpp"
#include "draw/bezier/adjust/AnglePlane.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ベジェ基本パラメータ（※絵を生成する流れにおける共通の値）
// [CBmpGenerator]により生成され[CBezier]に関わる処理を通して参照される
//-----------------------------------------------------------------
struct stBEZIER_BASE_PARAM{
    //-------------------------------------------------------------
    // 各種バッファ（※[eTEX_DATA_P8D]形式＝１バイト１画素）
    //-------------------------------------------------------------
    BYTE* pBufLine;             // 線バッファ
    BYTE* pBufColor;            // 塗りバッファ（※テストバッファとしても利用される）
    BYTE* pBufFillGuide;        // 塗りガイド（※塗りバッファに出力する前の下塗り用）
    BYTE* pBufFillGuard;        // 塗りガード（※塗りバッファへの出力制限用）
    BYTE* pBufTemp;             // テンポラリ（※オプション塗りで利用する）
    
    // 出力画像サイズ
    int pixelRate;              // ピクセル割合（※論理ピクセル単位に対する実際のピクセル数＝GM_P_RATE）
    int texW0;                  // テクスチャ名目サイズ：横幅（※ピクセル割合適用後のサイズ＝論理座標変換時に利用される）
    int texH0;                  // テクスチャ名目サイズ：縦幅（※ピクセル割合適用後のサイズ＝論理座標変換時に利用される）
    int texW;                   // テクスチャ実サイズ：横幅（※ピクセル割合適用後の余白込みのサイズ＝実描画時に利用される）
    int texH;                   // テクスチャ実サイズ：縦幅（※ピクセル割合適用後の余白込みのサイズ＝実描画時に利用される）
        
    //-------------------------------------------------------------
    // 呼び出し情報（※[stBMP_GEN_CREATE_PARAM]の値をそのまま保持）
    //-------------------------------------------------------------
    bool isFlipH;               // ヨコ反転
    bool isFlipV;               // タテ反転
    float scale;                // スケール
    float rot;                  // 回転（※原点はバッファの中心＝[texW/2.0f,texH/2.0f]）

    // オプション指定
    bool arrOption[eBD_OPTION_MAX][BD_SLOT_INDEX_MAX];
    bool arrOptionForSlot[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX][BD_OPTION_FOR_SLOT_MAX];

    float adjustRateH;          // ヨコ体型：[-1.0:細い] 〜 [0.0:基準] 〜 [+1.0:太い]
    float adjustRateV;          // タテ体型：[-1.0:低い] 〜 [0.0:基準] 〜 [+1.0:高い]
    float adjustRateLR;         // 左右の傾き：[-1.0:左側に傾く]〜[0.0:基準]〜[+1.0:右側に傾く]
    float adjustRateUD;         // 上下の傾き：[-1.0:上側に傾く]〜[0.0:基準]〜[+1.0:下側に傾く]

    //-------------------------------------------------------------
    // 補助データ
    //-------------------------------------------------------------
    CDelayLog* pDelayLog;       // 遅延ログ（※[NULL]で無効＝利用しない）
    CJointPoint* pJointPoint;   // 結合ポイント（※[NULL]で無効＝利用しない）
    CAnglePlane* pAnglePlane;   // 傾き面（※[NULL]で無効＝利用しない）
    
    // ストロークサイズ
    float strokeScale;          // 拡縮率（※拡縮表示時にストロークの印象が同じになるように）
};

// クリア
#define CLEAR_BEZIER_BASE_PARAM( _p )                   \
    memset( _p, 0, sizeof(stBEZIER_BASE_PARAM) );       \
    (_p)->scale = 1.0f;                                 \
    (_p)->strokeScale = 1.0f

//----------------------------------------------------------
// レイヤー調整パラメータ（※各レイヤー[CLayerData]毎に変化するもの）
//（※レイヤーデータに含まれる論理座標情報を実座標に変換する際の情報）
//----------------------------------------------------------
struct stBEZIER_LAYER_PARAM{
    // 対象
    eBD_SLOT slot;              // スロット
    eBD_SLOT slot0;             // 本来のスロット（※カバーレイヤー等は呼び出し元のスロット扱いをされるため別途保持しておく）
    int      slotIndex;         // スロットインデックス

    eBD_SLOT hookSlot;          // HOOK先スロット
    int      hookSlotIndex;     // HOOK先スロットインデックス

    eBD_SLOT parentSlot;        // 親スロット：全体傾け用
    int      parentSlotIndex;   // 親スロットインデックス：全体傾け用
        
    // 調整値（※テンションとサイズはオーダー登録時に解決されている）
    float fixedAdjustRateH;     // 調整値：H（※値の整合性が取れている状態）
    float fixedAdjustRateV;     // 調整値：V（※値の整合性が取れている状態）
    float fixedAngleRateLR;     // 傾け：左右の割合（※値の整合性が取れている状態）
    float fixedAngleRateUD;     // 傾け：上下の割合（※値の整合性が取れている状態）

    // 実座標変換用の値
    float pixelOfsX;            // 表示オフセット：X（※レイヤーの原点）
    float pixelOfsY;            // 表示オフセット：Y（※レイヤーの原点）
    float pixelScaleX;          // 表示スケール：X
    float pixelScaleY;          // 表示スケール：Y
    float pixelRot;             // 回転：角度
    float pixelRotWithT;        // 回転：角度（テンション影響：変換時に適用される）

    // ビットマスク
    BYTE bitMask;

    // 結合ポイント処理用の値（※変換処理の途中経過の値）
    float scaleForBdpd;         // BDPDの基本スケール

    // ストロークサイズ
    float strokeScale;          // 拡縮率（※レイヤー全体を拡縮する際にストロークの印象が同じになるように）
        
    // 一時調整値
    int tempRateOfsX;
    int tempRateOfsY;
    int tempRateScaleX;
    int tempRateScaleY;
};

// クリア
#define CLEAR_BEZIER_LAYER_PARAM( _p )                  \
    memset( _p, 0, sizeof(stBEZIER_LAYER_PARAM) );      \
    (_p)->slot = eBD_SLOT_INVALID;                      \
    (_p)->slot0 = eBD_SLOT_INVALID;                     \
    (_p)->slotIndex = -1;                               \
    (_p)->hookSlot = eBD_SLOT_INVALID;                  \
    (_p)->hookSlotIndex = -1;                           \
    (_p)->parentSlot = eBD_SLOT_INVALID;                \
    (_p)->parentSlotIndex = -1;                         \
    (_p)->pixelScaleX = 1.0f;                           \
    (_p)->pixelScaleY = 1.0f;                           \
    (_p)->scaleForBdpd = 1.0f;                          \
    (_p)->strokeScale = 1.0f;                           \
    (_p)->tempRateScaleX = BEZIER_SCALE_RATE;           \
    (_p)->tempRateScaleY = BEZIER_SCALE_RATE

//-----------------------------------------------------------------
// アンカーポイント（※描画用のデータ：解像度や割合座標の処理が反映された値）
//（※論理データ[CAnchorPointData]に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_ANCHOR_POINT{
    // 座標
    float           x, y;                       // 座標：テクスチャの左上を原点とした値（※実ピクセル座標）
    float           xIn, yIn;                   // 方向線：進入（※[x,y]からの相対位置）
    float           xOut, yOut;                 // 方向線：出発（※[x,y]からの相対位置）
    
    // オフセット設定
    bool            isOfsValid;                 // オフセットの有効性（※オフセットは明示的に有効性を管理する）
    float           ofsX, ofsY;                 // オフセット：ジョイント／フック用
    
    // ストローク（※[CBrush]から取り出して調整した値）
    eSTROKE_TYPE    type;                       // ストロークタイプ（※[eSTROKE_TYPE_OFF]で出力しない＝以下のブラシ値が不要になる）
    float           size;                       // ストローク基本サイズ
    float           ofsRange;                   // ストロークのオフセット範囲（※ドット出力位置が取りうる範囲）
    int             dotPutMax;                  // ドット出力最大（※出力する画素の濃さの最大値）
    int             dotPutRateBase;             // ドット出力割合：基本（※出力済みの画素の適用率：百分率）
    int             dotPutRateStrong;           // ドット出力割合：濃い方（※出力済み画素／ブラシ画素の濃い方への適用率：百分率）
    int             dotPutRateWeak;             // ドット出力割合：薄い方（※出力済み画素／ブラシ画素の薄い方への適用率：百分率）
    bool            isDotPutForce;              // 強制的にドットを出力するか？（※テストを無視して必ずドットを出力する）
    bool            isDotPutWhetherMax;         // 画素が最大値に到達していてもドットを出力するか？（※デフォルトでは最大濃さになった画素は処理対象から外れる）
    bool            isDotPutOnOutCol;           // 塗り出力されたドットの上にストロークを出力するか？（※直前の塗りで実際に出力された画素を対象にする）
    bool            isDotErase;                 // 消去するか？（※画素の値を減算する）
    
    // ストロークのブレ（※ドット出力時のバネによるブレ値への適用率）
    float           shakeScaleForPos;           // 位置ブレの強さ（※この値が大きいほど歪んだストロークになる）
    float           shakeScaleForSize;          // サイズブレの強さ（※この値が大きいほど太さが歪なストロークになる）

    // フラグ
    bool            bNoFillGuide;               // 塗りガイドを出力しないか？（※領域に関わらないストロークがガイドに影響しないように）
    bool            bLineRepairTest;            // 線修復テストを行うか？（※出力済みの色テクスチャにより欠けてしまったストロークへの重複描画）
    bool            bAsTouchStroke;             // タッチストロークとして扱う（※タッチオフ時は透明）
        
    // ガイド対象
    int             guideTargetIdFixed;         // ガイド対象

    // フック対象（※２点間をつなげる指定）
    int             hookTargetIdFixed;          // フック対象（※データ指定によるフック）
    int             reHookTargetIdFixed;        // 再フック対象（※フックした後の座標でさらにフック）
    int             systemHookTargetIdFixed;    // フック対象（※結合ポイントによるフック）
    bool            isRefJoint;                 // ジョイント参照か？（※表示用）
    bool            isJointed;                  // ジョイントが指定されている（※表示用＝システムフックはジョイントパーツ経由でないと設定されないので）

    // ストロークサイズ（※[CAnchorPointData]から取り出して調整した値：値域は[0.0〜3.0]）
    float           strokeSize;                 // ストロークサイズ：基本サイズ
    float           strokeStartRange;           // ストロークサイズ：開始サイズ適用範囲（※[0.0f〜stronStartRange]間でストロークサイズを補正する）
    float           strokeStartSize;            // ストロークサイズ：開始サイズ
    float           strokeEndRange;             // ストロークサイズ：終了サイズ適用範囲（※[1.0f-stronkeEndRange〜1.0f]間でストロークサイズを補正する）
    float           strokeEndSize;              // ストロークサイズ：終了サイズ
    float           strokeEdgeFillSize;         // ストローク塗りサイズ（※ＡＰの前後のストロークの角度が浅い場合に先端を埋めるサイズ）

    // タッチ（※線による質感）
    int             touchTargetIdFixed;         // タッチ：対象
    int             touchTargetSubIdFixed;      // タッチ：サブ
    float           touchBasePos;               // タッチ：基本位置（※ストロークの開始位置）
    float           touchBaseSize;              // タッチ：基本サイズ（※ストロークの開始サイズ）
    float           touchBaseRot;               // タッチ：基本角度（※ストロークの開始角度）
    float           touchFrontArea;             // タッチ：前方の領域（※基本位置から前方へのタッチ適用領域割合＝1.0で全領域）
    float           touchFrontSizeOfs;          // タッチ：前方のサイズオフセット（※前方領域の最終サイズオフセット）
    float           touchFrontRotOfs;           // タッチ：前方の角度オフセット
    float           touchBackArea;              // タッチ：後方の領域（※基本位置から後方へのタッチ適用領域割合＝1.0で全領域）
    float           touchBackSizeOfs;           // タッチ：後方のサイズオフセット（※後方領域の最終サイズオフセット）
    float           touchBackRotOfs;            // タッチ：後方の角度オフセット
    int             touchFrontNum;              // タッチ：前方の要素数
    int             touchFrontSkip;             // タッチ：前方のスキップ数
    float           touchFrontBorderRate;       // タッチ：前方のボーダー割合
    int             touchBackNum;               // タッチ：後方の要素数
    int             touchBackSkip;              // タッチ：後方のスキップ数
    float           touchBackBorderRate;        // タッチ：後方のボーダー割合
    float           touchRandomOfsFor0;         // タッチ：ランダムオフセット０
    float           touchRandomOfsFor1;         // タッチ：ランダムオフセット１
    float           touchHeadSkipRate;          // タッチ：先頭のスキップ率
    float           touchTailSkipRate;          // タッチ：末尾のスキップ率
    float           touchRefOfsRateForMain;     // タッチ：参照オフセット（メイン）
    float           touchRefOfsRateForSub;      // タッチ：参照オフセット（サブ）
    float           frillWidthRateForMain;      // フリル：横幅補正（メイン）
    float           frillWidthRateForSub;       // フリル：横幅補正（サブ）

    bool            isFrillReverseHook;         // フリル：フックを反転するか？
    bool            isTouchReverse;             // タッチ処理を反転するか？（※ミラーコピーによるタッチか？）
    bool            isTouchSkipFront;           // タッチの先頭をスキップするか？
    bool            isTouchSkipBack;            // タッチの末尾をスキップするか？

    // パレット
    BYTE            stripeOrFrillMainPalGrp;    // ストライプパレットグループへのオフセット：メイン（※０で無効）
    BYTE            stripeOrFrillSubPalGrp;     // ストライプパレットグループへのオフセット：サブ（※０で無効）

    // フリル（※タッチ指定に対してフリルを適用する／変数の役割はタッチと同様）
    eSTROKE_FRILL   frillMainId;                // フリル：メイン素材ID
    eSTROKE_FRILL   frillSubId;                 // フリル：サブ素材ID

    // 傾け（※[LR/UD]の値は描画中共通なのでレイヤーパラメータから参照する）
    int             angleSearchKey;             // 傾け：検索キー

    // ワークパス色のオフセット（※毎回判定するのは面倒なので値で持っておく）
    BYTE            workPathPalOfsS;

    // 2-way list
    stBEZIER_ANCHOR_POINT* pPrev;               // 前の要素
    stBEZIER_ANCHOR_POINT* pNext;               // 次の要素
};

// クリアマクロは用意しない
// この構造体は[CBezierStructConv::CreateBezierAnchorPoint]でのみ生成され、その際は全てのメンバーが適切に処理される想定

//-----------------------------------------------------------------
// 塗りつぶし点（※描画用のデータ：解像度や割合座標の処理がされた値を想定）
//（※論理データ[CFillPointData]の値に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_FILL_POINT{
    float           x, y;                   // 塗りつぶしの始点：テクスチャの左上を原点とした位置

    // オフセット設定
    bool            isOfsValid;             // オフセットの有効性（※オフセットは明示的に有効性を管理する）
    float           ofsX, ofsY;             // オフセット：フック用
    
    // フック
    int             hookTargetIdFixed;      // フック対象（※データ指定によるフック）

    // 塗り情報
    eFILL_TYPE      type;                   // 塗りのスタイル（※バケツ設定から取得する）
    BYTE            palGrp;                 // パレットグループ
    int             darkOfs;                // 塗りの明暗オフセット
    bool            isFillOnOutCol;         // 塗り出力された領域にのみ出力する

    // 塗りオプション
    eFILL_OPTION    option;                 // オプション：タイプ
    int             optionDarkOfs;          // オプション：明暗オフセット
    int             optionTouchIdFixed;     // オプション：タッチ対象ID（※アンチ処理をする際はあらかじめタッチ点を登録しておく）
    BYTE            optionBitMask;          // オプション：ビットマスク
    float           optionStrokeSize;       // オプション：ストロークサイズ（※アンチ処理のストロークサイズ）
    float           optionStrokeRange;      // オプション：ストロークレンジ（※アンチ処理のストロークレンジ）
    float           optionOfsX;             // オプション：オフセットＸ（※広域塗り／全体塗り時のオフセット）
    float           optionOfsY;             // オプション：オフセットＹ（※広域塗り／全体塗り時のオフセット）
    
    // 傾け（※[LR/UD]の値は描画中共通なのでレイヤーパラメータから参照する）
    int             angleSearchKey;         // 傾け：検索キー

    // ワークパス色のオフセット（※毎回判定するのは面倒なので値で持っておく）
    BYTE            workPathPalOfsS;

    // 2-way list
    stBEZIER_FILL_POINT* pPrev;             // 前の要素
    stBEZIER_FILL_POINT* pNext;             // 次の要素
};

// クリアマクロは用意しない
// この構造体は[CBezierStructConv::CreateBezierFillPoint]でのみ生成され、その際は全てのメンバーが適切に処理される想定

//-----------------------------------------------------------------
// スロット点（※描画用のデータ：解像度や割合座標の処理がされた値を想定）
//（※論理データ[CSlotPointData]の値に対して各種調整等をした描画用情報）
//-----------------------------------------------------------------
struct stBEZIER_SLOT_POINT{
    eBD_SLOT    slot;                   // スロット
    int         slotIndex;              // スロットインデックス
    int         rateOfsX, rateOfsY;     // オフセット割合座標の返却値（※スロット呼び出し処理は割合座標で行われるため）

    float       x, y;                   // 基準となる点：テクスチャの左上を原点とした位置
    bool        isFlipH, isFlipV;       // 反転フラグ
    
    // ワークパス色のオフセット（※描画時に都度判定するのは面倒なので値で持っておく）
    BYTE        workPathPalOfs;

    // 2-way list
    stBEZIER_SLOT_POINT* pPrev;         // 前の要素
    stBEZIER_SLOT_POINT* pNext;         // 次の要素
};

// クリアマクロは用意しない
// この構造体は[CBezierStructConv::CreateBezierSlotPoint]でのみ生成され、その際は全てのメンバーが適切に処理される想定

//------------------------------
// 分割点（※ストロークの分割ポイント）
//------------------------------
struct stBEZIER_DIVISION_POINT{
    float x0, y0;                   // 元座標（※傾き適用前の値＝タッチの処理で利用する）
    float x, y;                     // 座標：テクスチャの左上を原点とした位置（※傾きがある場合は適用済みの値となる）
    float strokeSize;               // ストロークサイズ
    bool isEdgeFillCheck;           // エッジ埋めの確認をするか？
    stBEZIER_ANCHOR_POINT* pAP;     // 分割元のアンカーポイント（※描画情報参照用）
};

// クリアマクロは用意しない
// この構造体は[CBezier::AddDivisionPoint]でのみ設定され、その際は全てのメンバーが適切に処理される想定

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
