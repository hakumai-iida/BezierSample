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

#include "draw/bmp/dot/BmpDotSettingData.hpp"
#include "draw/bmp/dot/BmpDotOrderData.hpp"
#include "draw/bezier/data/LayerOrderList.hpp"

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
struct stBMP_GEN_CREATE_PARAM{
    // 表示指定
    bool isFlipH;                   // ヨコ反転
    bool isFlipV;                   // タテ反転
    float scale;                    // スケール
    float rot;                      // 回転（※原点はバッファの中心）

    // オプション
    bool arrOption[eBD_OPTION_MAX][BD_SLOT_INDEX_MAX];
    bool arrOptionForSlot[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX][BD_OPTION_FOR_SLOT_MAX];

    // 調整値
    float adjustRateH;              // 調整値：ヨコ
    float adjustRateV;              // 調整値：タテ
    float adjustRateLR;             // 調整値：左右の傾き
    float adjustRateUD;             // 調整値：上下の傾き

    // 遅延ログ
    CDelayLog* pDelayLog;
};

// CLEAR
#define CLEAR_BMP_GEN_CREATE_PARAM( _pParam )                   \
    memset( (_pParam), 0, sizeof(stBMP_GEN_CREATE_PARAM) );     \
    (_pParam)->scale = 1.0f

//--------------------------------------------
// オーダーリスト登録パラメータ（※パーツ個々の登録情報）
//--------------------------------------------
struct stREGIST_ORDER_PARAM{
    bool isFlipH;                   // ヨコ反転
    bool isFlipV;                   // タテ反転
    float scale;                    // スケール
    float rot;                      // 回転（※パーツ原点[baseX,baseY]を中心とする）

    // 呼び出された際の画像に対するオフセット
    float baseX;                    // 基準座標：X
    float baseY;                    // 基準座標：Y
    
    // スロットとして呼び出された際の割合座標オフセット（※結合ポイントの接続先原点処理用）
    int rateOfsX;                   // 割合オフセット：X
    int rateOfsY;                   // 割合オフセット：Y

    // 作業用
    float strokeScale;              // ストロークスケール（※拡縮表示時にストロークの印象をあわせるため）
    bool isSlotIgnore;              // スロット処理を無視するか？（※選択対象パーツのみを処理するか？）
};

// CLEAR
#define CLEAR_REGIST_ORDER_PARAM( _pParam )                 \
    memset( (_pParam), 0, sizeof(stREGIST_ORDER_PARAM) );   \
    (_pParam)->scale = 1.0f;                                \
    (_pParam)->strokeScale = 1.0f

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
    static int s_nTexMargin;            // テクスチャマージン（※実解像度サイズ＝テクスチャ解像度適用後）
    static int s_nTexW0, s_nTexH0;      // テクスチャ名目サイズ（※実解像度サイズ＝テクスチャ解像度適用後）
    static int s_nTexW, s_nTexH;        // テクスチャ実サイズ（※余白までを含めたもの）
    static int s_nSizeBuf;              // バッファサイズ（※各バッファ共通）

    static BYTE* s_pBufLine;            // バッファ：線
    static BYTE* s_pBufColor;           // バッファ：色
    static BYTE* s_pBufFillGuide;       // バッファ：塗りガイド
    static BYTE* s_pBufFillGuard;       // バッファ：塗りガード（マスク）
    static BYTE* s_pBufTemp;            // バッファ：テンポラリ

    // バッファの利用状況（※画像の利用状況＝画素が有効な四隅の位置）
    static int s_nLeftForLineBuf;           // 線バッファの左端
    static int s_nRightForLineBuf;          // 線バッファの右端
    static int s_nTopForLineBuf;            // 線バッファの上端
    static int s_nBottomForLineBuf;         // 線バッファの下端

    static int s_nLeftForColorBuf;          // 色バッファの左端
    static int s_nRightForColorBuf;         // 色バッファの右端
    static int s_nTopForColorBuf;           // 色バッファの上端
    static int s_nBottomForColorBuf;        // 色バッファの下端

    static int s_nLeftForFillGuideBuf;      // 塗りガイドバッファの左端
    static int s_nRightForFillGuideBuf;     // 塗りガイドバッファの右端
    static int s_nTopForFillGuideBuf;       // 塗りガイドバッファの上端
    static int s_nBottomForFillGuideBuf;    // 塗りガイドバッファの下端

    static int s_nLeftForTempBuf;           // 一時バッファの左端
    static int s_nRightForTempBuf;          // 一時バッファの右端
    static int s_nTopForTempBuf;            // 一時バッファの上端
    static int s_nBottomForTempBuf;         // 一時バッファの下端

    //--------------------
    // ワーク
    //--------------------
    // 基本パラメータ
    static stBEZIER_BASE_PARAM      s_stBaseParam;
    
    // 状況管理
    static CJointPoint  s_oJointPoint;      // 結合点
    static CAnglePlane  s_oAnglePlane;      // 傾き面
    
    // スロット基本位置
    static float s_fArrSlotBaseX[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX];
    static float s_fArrSlotBaseY[eBD_SLOT_MAX][BD_SLOT_INDEX_MAX];

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
    static void ResetLineBuf( void );
    static void ResetColorBuf( void );
    static void ResetFillGuideBuf( void );
    static void ResetFillGuardBuf( void );
    static void ResetTempBuf( void );
    
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

    // バッファの利用状況取得
    inline static int GetLeftForLineBuf( void ){ return( s_nLeftForLineBuf ); }
    inline static int GetRightForLineBuf( void ){ return( s_nRightForLineBuf ); }
    inline static int GetTopForLineBuf( void ){ return( s_nTopForLineBuf ); }
    inline static int GetBottomForLineBuf( void ){ return( s_nBottomForLineBuf ); }
    
    inline static int GetLeftForColorBuf( void ){ return( s_nLeftForColorBuf ); }
    inline static int GetRightForColorBuf( void ){ return( s_nRightForColorBuf ); }
    inline static int GetTopForColorBuf( void ){ return( s_nTopForColorBuf ); }
    inline static int GetBottomForColorBuf( void ){ return( s_nBottomForColorBuf ); }

    inline static int GetLeftForFillGuideBuf( void ){ return( s_nLeftForFillGuideBuf ); }
    inline static int GetRightForFillGuideBuf( void ){ return( s_nRightForFillGuideBuf ); }
    inline static int GetTopForFillGuideBuf( void ){ return( s_nTopForFillGuideBuf ); }
    inline static int GetBottomForFillGuideBuf( void ){ return( s_nBottomForFillGuideBuf ); }

    inline static int GetLeftForTempBuf( void ){ return( s_nLeftForTempBuf ); }
    inline static int GetRightForTempBuf( void ){ return( s_nRightForTempBuf ); }
    inline static int GetTopForTempBuf( void ){ return( s_nTopForTempBuf ); }
    inline static int GetBottomForTempBuf( void ){ return( s_nBottomForTempBuf ); }

    static void ResetLineBufInfo( void );
    static void ResetColorBufInfo( void );
    static void ResetFillGuideBufInfo( void );
    static void ResetTempBufInfo( void );

    // バッファの利用状況更新
    static void UpdateLineBufInfo( int x, int y );
    static void UpdateColorBufInfo( int x, int y );
    static void UpdateFillGuideBufInfo( int x, int y );
    static void UpdateTempBufInfo( int x, int y );

    //-----------------------------------------------------
    // BMP生成パラメータからテクスチャの生成
    //-----------------------------------------------------
    static int CreateTexWithGenParam( CTex* pTexLine, CTex* pTexColor,
                                      CBmpGenParam* pGenParam, eBD_SLOT slot, int subId, int slotInex,
                                      stBMP_GEN_CREATE_PARAM* pCreateParam,
                                      bool isWorkPath, float workScale, bool isSlotIgnore );

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
    // オーダーの初期化
    static bool InitOrderList( CLayerOrderList* pOrderList, eBD_FORM form, bool isSuited );

    // オーダーリスト設定
    static void SetOrderList( CLayerOrderList* pOrderList, CBmpGenParam* pGenParam,
                              eBD_SLOT slot, int slotIndex, bool isRef,
                              stBEZIER_BASE_PARAM* pBaseParam,
                              stREGIST_ORDER_PARAM rop, // 呼び出し先で編集されるので値で渡す
                              bool isWorkPath );
    
    // オーダーリストの確定
    static void FixOrderList( CLayerOrderList* pOrderList );

    // オーダーリストの描画
    static void DrawOrderList( CLayerOrderList* pOrderList, bool isSuited, bool isWorkPath );
    static void DrawOrderList0( CLayerOrderList* pOrderList, bool isWorkPath, bool isPreDraw, eBD_LAYER_ORDER_GROUP targetGroup );

    //============================================================
    // 以下、[BmpGeneratorForSettinh.cpp]にて実装
    //============================================================
public:
    // 設定データから作成パラメータの設定（※BMP全体にかかわるパラメータの設定）
    static bool SetBmpGenCreateParam( stBMP_GEN_CREATE_PARAM* pCreateParam, CBmpDotSettingData* pSetting, CDelayLog* pLog );
    
    // 設定データから生成パラメータの作成（※BMP構成要素毎のパラメータの設定）
    static bool SetBmpGenParam( CBmpGenParam* pGenParam, CBmpDotSettingData* pSetting );
        
    // 対象のパーツが表示されるか？
    static bool CheckTargetPartVisible( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex );

    // 対象のパーツが隠されるか？
    static bool CheckTargetPartHidden( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex );

    // パーツのフラグ判定
    static bool CheckTargetPartFlag( CBmpGenParam* pGenParam, eBDPD_FLAG targetFlag );

    // パーツの無視設定
    static bool SetIgnorePart( CBmpGenParam* pGenParam, eBD_SLOT targetSlot, int targetSlotIndex );

    // カバーBDPDの判定と対応パーツのカバーフラグの設定
    static bool CheckCovereThenSetFlag( CBmpGenParam* pGenParam, eBD_SLOT coverSlot, eBD_SLOT parentSlot );
    
    // カバーBDPD取得
    static CBmpDotPartData* CheckThenGetCoverBDPDAndInfo( CBmpGenParam* pGenParam, CBmpDotPartData* pParentData, int parentSlotIndex,
                                                          int* pRateOfsX, int* pRateOfsY, float* pScaleX, float* pScaleY, float* pRot );

    // 結合点登録
    static bool RegistJointPoint( stBEZIER_BASE_PARAM* pBaseParam, stBMP_GEN_PARAM_CELL* pBGPCell, CSlotAdjuster* pSlotAdj,
                                  int slotIndex, bool isFlipH, bool isFlipV,
                                  float applyRot, float applyScale, float bddScale, int rateOfsX, int rateOfsY );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_GENERATOR_HPP__
