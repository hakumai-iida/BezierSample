/*+----------------------------------------------------------------+
  |	Title:		BmpGenerator.hpp [共通環境]
  |	Comment:	BMP生成
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_GENERATOR_HPP__
#define __BMP_GENERATOR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenParam.hpp"
#include "BmpDotSettingData.hpp"

#include "draw/tex/bezier/LayerOrderList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ログ
//#define BMP_GEN_LOG

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//-------------------------------
// テクスチャ生成パラメータ
//-------------------------------
typedef struct{
    bool isFlipH;                   // ヨコ反転
    bool isFlipV;                   // タテ反転
    float rot;                      // 回転（※原点はバッファの中心）
    float scale;                    // スケール

    bool arrOption[eBD_OPTION_MAX]; // オプション
    
    float adjustRateH;              // 調整値：ヨコ
    float adjustRateV;              // 調整値：タテ
    float adjustRateLR;             // 調整値：左右の傾き
    float adjustRateUD;             // 調整値：上下の傾き
    CDelayLog* pDelayLog;           // 遅延ログ
} stBMP_GEN_CREATE_PARAM;

// CLEAR
#define CLEAR_BMP_GEN_CREATE_PARAM( _pParam )                   \
    memset( (_pParam), 0, sizeof(stBMP_GEN_CREATE_PARAM) );     \
    (_pParam)->scale = 1.0f

//--------------------------------------------
// オーダーリスト登録パラメータ（※パーツ個々の登録情報）
//--------------------------------------------
typedef struct{
    bool isFlipH;                   // ヨコ反転
    bool isFlipV;                   // タテ反転
    float rot;                      // 回転（※[baseX,baseY]を原点とする）
    float scale;                    // スケール

    // 呼び出された際の画像に対するオフセット
    float baseX;                    // 基準座標：X
    float baseY;                    // 基準座標：Y
    
    // スロットとして呼び出された際の割合座標オフセット（※結合ポイントの接続先原点判定用）
    int   rateOfsX;                 // 割合オフセット：X
    int   rateOfsY;                 // 割合オフセット：Y

    // 作業用フラグ（※ワーク時に対象パーツの表示だけを行う時用）
    bool isWorkScale;               // 作業スケールか？（※[BDPD]に指定されたワークサイズを使うか？）
    bool isSlotIgnore;              // スロット処理を無視するか？（※表示要素のみを処理するか？）
} stREGIST_ORDER_PARAM;

// CLEAR
#define CLEAR_REGIST_ORDER_PARAM( _pParam )                 \
    memset( (_pParam), 0, sizeof(stREGIST_ORDER_PARAM) );   \
    (_pParam)->scale = 1.0f

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMP生成
//----------------------------------------
class CBmpGenerator{
private:
    //--------------------
    // 設定値
    //--------------------
    static int s_nTexResolution;        // テクスチャ解像度（※基本的に[GM_P_RATE]の値）
    static int s_nBaseTexMargin;        // 画像の基本マージン(※テクスチャ解像度適用前の値)
    static int s_nBaseTexSizeW;         // 画像の基本サイズ：ヨコ(※テクスチャ解像度適用前の値)
    static int s_nBaseTexSizeH;         // 画像の基本サイズ：タテ(※テクスチャ解像度適用前の値)
    static float s_fBaseStrokeScale;    // 基本ストロークスケール（※ブラシ等のサイズを補正する値）

    //--------------------
    // バッファ
    //--------------------
    static BYTE* s_pBufLine;            // バッファ：線
    static BYTE* s_pBufColor;           // バッファ：塗り
    static BYTE* s_pBufFillGuide;       // バッファ：塗りガイド
    static BYTE* s_pBufFillGuard;       // バッファ：塗りガード（マスク）

    static int s_nTexMargin;            // テクスチャマージン（※実解像度サイズ＝テクスチャ解像度適用後）
    static int s_nTexW0, s_nTexH0;      // テクスチャ名目サイズ（※実解像度サイズ＝テクスチャ解像度適用後）
    static int s_nTexW, s_nTexH;        // テクスチャ実サイズ（※余白までを含めたもの）
    static int s_nSizeBuf;              // バッファサイズ（※各バッファ共通）

    //--------------------
    // ワーク
    //--------------------
    // 基本パラメータ
    static stBEZIER_BASE_PARAM      s_stBaseParam;
    
    // 状況管理
    static CJointPoint  s_oJointPoint;   // 結合点
    static CAnglePlane  s_oAnglePlane;   // 傾き面

    //------------------------
    // 出力データ（※検出用の座標）
    //------------------------
    // 基本座標（※接地点検出用）
    static float s_fTexBaseX, s_fTexBaseY;
    
    // 注目座標（※顔の中心検出用）
    static float s_fTexFocusX, s_fTexFocusY;

    // 中間情報（※傾け処理適用前の値）
    static float s_fTexGroundX0_L, s_fTexGroundY0_L;
    static float s_fTexGroundX0_R, s_fTexGroundY0_R;
    static float s_fTexFocusX0, s_fTexFocusY0;

public:
    // 開始／終了
    static bool OnCreate( void );
    static void OnDestroy( void );
    
    // リセット
    static void Reset( void );
    static void ResetGuide( bool isFullReset=true );
    
    // 設定
    inline static void SetTexResolution( int resolution ){ s_nTexResolution = resolution; }
    inline static void SetBaseTexMargin( int m ){ s_nBaseTexMargin = m; }
    inline static void SetBaseTexSize( int w, int h ){ s_nBaseTexSizeW = w; s_nBaseTexSizeH = h; }
    inline static void SetBaseStrokeScale( float s ){ s_fBaseStrokeScale = s; }
    
    // 取得
    inline static int GetTexResolution( void ){ return( s_nTexResolution ); }
    inline static int GetBaseTexMargin( void ){ return( s_nBaseTexMargin ); }
    inline static int GetBaseTexSizeW( void ){ return( s_nBaseTexSizeW ); }
    inline static int GetBaseTexSizeH( void ){ return( s_nBaseTexSizeH ); }
    inline static int GetTotalTexSizeW( void ){ return( s_nBaseTexSizeW + 2*s_nBaseTexMargin ); }
    inline static int GetTotalTexSizeH( void ){ return( s_nBaseTexSizeH + 2*s_nBaseTexMargin ); }
    inline static float GetBaseStrokeScale( void ){ return( s_fBaseStrokeScale ); }

    //-----------------------------------------------------
    // BMP生成パラメータからテクスチャの生成
    //-----------------------------------------------------
    static int CreateTexWithGenParam( CTex* pTexLine, CTex* pTexColor,
                                      CBmpGenParam* pGenParam, eBD_SLOT slot, int subId, int slotInex,
                                      stBMP_GEN_CREATE_PARAM* pCreateParam,
                                      bool isWorkPath, bool isWorkDisp );

    //-----------------------------------------------------
    // リスト（※要素は[CLayerData]を想定）からテクスチャの生成
    //-----------------------------------------------------
    static int CreateTexWithLayerList( CTex* pTexLine, CTex* pTexColor,
                                       CList* pLayerDataList,
                                       stBMP_GEN_CREATE_PARAM* pCreateParam,
                                       bool isWorkPath );
private:
    // 処理準備
    static bool Ready( stBMP_GEN_CREATE_PARAM* pCreateParam );
    static bool AllocWorkBuf( void );
    static void ResetWorkBuf( void );
    
    // 処理の終了
    static int Finish( CTex* pTexLine, CTex* pTexColor );
    static int FinishForWorkPath( CTex* pTexWorkPath );
    
private:
	// インスタンスの生成は不可
	CBmpGenerator( void ){}
	virtual ~CBmpGenerator( void ){}
    
    // 有効な領域を検出してバッファにコピーする
    static void CopyTexWithBezierBufInfo( BYTE* pBuf, BYTE* pBufSrc, int* pOfsL, int* pOfsR, int* pOfsT, int* pOfsB, bool isColor );
    static void CopyTexWithOfsCheck( BYTE* pBuf, BYTE* pBufSrc, int* pOfsL, int* pOfsR, int* pOfsT, int* pOfsB );

    //============================================================
    // 以下、[BmpGeneratorForAngle.cpp]にて実装
    //============================================================
private:
    // アングル平面の設定
    static bool SetAnglePlaneInfo( eBD_SLOT slot, int slotIndex, float baseX0, float baseY0,
                                   bool isFlipH, bool isFlipV, float rot, float scale );

    //============================================================
    // 以下、[BmpGeneratorForOrder.cpp]にて実装
    //============================================================
private:
    // オーダーの事前登録
    static bool RegistOrder( CLayerOrderList* pOrderList, eBD_FORM form, bool isSuited );

    // 結合点登録
    static bool RegistJointPoint( stBEZIER_BASE_PARAM* pBaseParam, stBMP_GEN_PARAM_CELL* pBGPCell, CSlotAdjuster* pSlotAdj,
                                  int slotIndex, bool isFlipH, bool isFlipV,
                                  float applyRot, float applyScale, float bddScale, int rateOfsX, int rateOfsY );

    // オーダーリスト設定
    static void SetOrderList( CLayerOrderList* pOrderList, CBmpGenParam* pGenParam,
                              eBD_SLOT slot, int slotIndex, bool isRef,
                              stBEZIER_BASE_PARAM* pBaseParam,
                              stREGIST_ORDER_PARAM rop, // 呼び出し先で編集されるので値で渡す
                              bool isWorkPath );
    
    // カバー取得
    static bool GetCoverLayerAt( CBmpGenParam* pGenParam, eBD_SLOT parentSlot, int parentSlotIndex, bool isClose, int at,
                                 CLayerData** ppOutLayer, float* pBaseRot, float* pBaseScale );

    //============================================================
    // 以下、[BmpGeneratorForSettinh.cpp]にて実装
    //============================================================
public:
    // 設定とオーダからパラメータの作成（※メモリを確保するので利用後は解放すること）
    static CBmpGenParam* CreateBmpGenParam( CBmpDotSettingData* pSetting,
                                            int idForFace=BD_UID_INVALID,
                                            int idForBody=BD_UID_INVALID,
                                            int idForSuit=BD_UID_INVALID,
                                            eBD_EMO emo=eBD_EMO_INVALID,
                                            eBD_FORM form=eBD_FORM_INVALID,
                                            float adjForT = 0.0f,
                                            float adjForS = 0.0f );
    
    // 解放
    static void ReleaseBmpGenParam( CBmpGenParam* pParam );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_GENERATOR_HPP__
