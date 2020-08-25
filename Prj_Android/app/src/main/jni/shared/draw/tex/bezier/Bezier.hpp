/*+----------------------------------------------------------------+
  |	Title:		Bezier.hpp [共通環境]
  |	Comment:	ベジェ曲線（※対象はパレット画像[P8D]）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_HPP__
#define __BEZIER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BezierStruct.hpp"
#include "LayerData.hpp"
#include "LineObjectData.hpp"
#include "PaintObjectData.hpp"
#include "SlotObjectData.hpp"
#include "obj/Spring.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 利用状況確認
#define BEZIER_USE_NUM_CHECK

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ベジェ曲線
//----------------------------------------
class CBezier{
public:
    // 領域確保情報（※端末の解像度によってサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSizeK( void );
    
private:
    // 表示管理フラグ
    static bool s_bDispOffTouch;                // タッチを表示しない
    static bool s_bDispOffStripe;               // ストライプを表示しない
    static bool s_bDispOffFrill;                // フリルを表示しない
    static bool s_bDispOffEdge;                 // エッジ塗りを表示しない
    static bool s_bDispOffFillOption;           // 塗りオプションを表示しない

    static bool s_bDispDirVectorAlways;         // 方向線を常に表示する
    static bool s_bDispOffDirVectorAlways;      // 方向線を常に表示しない
    static bool s_bDispPathActiveAlways;        // 全てのパスの表示をアクティブにする（一律の色味で表示する）
    static bool s_bDispOnlyAnchorPoint;         // アンカーポイントのみ表示
    
public:
    // 設定
    inline static void SetDispOffTouch( bool flag ){ s_bDispOffTouch = flag; }
    inline static void SetDispOffStripe( bool flag ){ s_bDispOffStripe = flag; }
    inline static void SetDispOffFrill( bool flag ){ s_bDispOffFrill = flag; }
    inline static void SetDispOffEdge( bool flag ){ s_bDispOffEdge = flag; }
    inline static void SetDispOffFillOption( bool flag ){ s_bDispOffFillOption = flag; }

    inline static void SetDispDirVectorAlways( bool flag ){ s_bDispDirVectorAlways = flag; }
    inline static void SetDispOffDirVectorAlways( bool flag ){ s_bDispOffDirVectorAlways = flag; }
    inline static void SetDispPathActiveAlways( bool flag ){ s_bDispPathActiveAlways = flag; }
    inline static void SetDispOnlyAnchorPoint( bool flag ){ s_bDispOnlyAnchorPoint = flag;}

    // 判定
    inline static bool IsDispOffTouch( void ){ return( s_bDispOffTouch ); }
    inline static bool IsDispOffStripe( void ){ return( s_bDispOffStripe ); }
    inline static bool IsDispOffFrill( void ){ return( s_bDispOffFrill ); }
    inline static bool IsDispOffEdge( void ){ return( s_bDispOffEdge ); }
    inline static bool IsDispOffFillOption( void ){ return( s_bDispOffFillOption ); }

    inline static bool IsDispDirVectorAlways( void ){ return( s_bDispDirVectorAlways ); }
    inline static bool IsDispOffDirVectorAlways( void ){ return( s_bDispOffDirVectorAlways ); }
    inline static bool IsDispPathActiveAlways( void ){ return( s_bDispPathActiveAlways ); }
    inline static bool IsDispOnlyAnchorPoint( void ){ return( s_bDispOnlyAnchorPoint );}

private:
    //-------------------------------------
    // ワーク
    //-------------------------------------
    // 分割点（※ストロークはＡＰ間の距離に応じて分割点に分けられて処理される）
    static stBEZIER_DIVISION_POINT* s_stArrDivisionPoint;
    static int s_nUseDivisionPoint;         // 現在の分割点利用数
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseDivisionPointMax;      // 最大利用数（※ログ用）
#endif
    
    // テンポラリ（※ドット出力処理内での[if]判定を極力省くため、座標とストロークサイズを事前計算してバッファに登録しておき、後でまとめて処理するためのバッファ）
    static float* s_fArrTempPointX;         // テンポラリ：ポイントＸ
    static float* s_fArrTempPointY;         // テンポラリ：ポイントＹ
    static float* s_fArrTempPointX0;        // テンポラリ：ポイントＸ（※傾き適用前）
    static float* s_fArrTempPointY0;        // テンポラリ：ポイントＹ（※傾き適用前）
    static float* s_fArrTempStrokeSize;     // ストロークサイズ
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseTempPointMax;          // 最大利用数（※ログ用）
#endif
    
    // フック（※データ指定／システム指定の２パターンで指定される）
    static bool* s_bArrHookPoint;          // 結合点有効性
    static float* s_fArrHookPointX;        // 結合点位置Ｘ
    static float* s_fArrHookPointY;        // 結合点位置Ｙ
    // フックは１座標の処理なので利用状況の確認は不要

    // タッチ（※タッチ描画の起点となる座標関連）
    static int* s_nArrTouchPointNum;        // タッチ登録数
    static float** s_fArrArrTouchPointX;    // タッチ位置Ｘ
    static float** s_fArrArrTouchPointY;    // タッチ位置Ｙ
    static float* s_fArrTouchPoint;         // タッチ位置（※バッファ実体）
    static float** s_fArrArrTouchPointX0;   // タッチ位置Ｘ（※傾き適用前）
    static float** s_fArrArrTouchPointY0;   // タッチ位置Ｙ（※傾き適用前）
    static float* s_fArrTouchPoint0;        // タッチ位置（※傾き適用前）（※バッファ実体）
    static float* s_fArrTouchWorkForLen0;   // タッチワーク（長さ）
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseTouchPointMax;         // 最大利用数（※ログ用）
#endif

    // ガイド（※レイヤーをまたぐ際の塗りガイド座標保持枠）
    static int* s_nArrGuidePointNum;        // ガイド登録数
    static float** s_fArrArrGuidePointX;    // ガイド位置Ｘ
    static float** s_fArrArrGuidePointY;    // ガイド位置Ｙ
    static float* s_fArrGuidePoint;         // ガイド位置（※バッファ実体）
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseGuidePointMax;         // 最大利用数（※ログ用）
#endif

    // ストライプ塗りつぶし情報
    static int s_nStripePointNum;           // ストライプ塗りつぶし点登録数
    static BYTE* s_nArrStripePalGrp;        // ストライプ塗りつぶしパレットグループ値
    static float* s_fArrStripePointX;       // ストライプ塗りつぶし点位置Ｘ
    static float* s_fArrStripePointY;       // ストライプ塗りつぶし点位置Ｙ
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseStripePointMax;        // 最大利用数（※ログ用）
#endif

    // ストライプ用ガイド（※塗りガイド座標参照用／ストライプの塗りつぶし位置の算出用）
    static int s_nNumStripeGuidePoint;      // ドット数
    static float* s_fArrStripeGuidePointX;    // ドットのＸ座標
    static float* s_fArrStripeGuidePointY;    // ドットのＹ座標
#ifdef BEZIER_USE_NUM_CHECK
    static int s_nUseStripeGuidePointMax;   // 最大利用数（※ログ用）
#endif

    // エッジ塗り
    static int s_nBufSizeForEdgeFillGuide;                  // エッジ塗りガイドサイズ
    static BYTE* s_pBufForEdgeFillGuide;                    // エッジ塗りガイド
    static int s_nOfsXForEdgeFillGuide;                     // エッジ塗り：オフセットX（※簡便のため／固定的な値を引数で渡すのを避けるため）
    static int s_nOfsYForEdgeFillGuide;                     // エッジ塗り：オフセットY（※簡便のため／固定的な値を引数で渡すのを避けるため）
    static stBEZIER_ANCHOR_POINT* s_pApForEdgeFillGuide;    // エッジ塗り：AP（※簡便のため／固定的な値を引数で渡すのを避けるため）
    
    // スキップストローク（※ストロークの出力制限）
    static int s_nSkipStrokePointCur;               // スキップストロークポイント現在値
    static int s_nSkipStrokePointMax;               // スキップストロークポイント最大数
    static int s_nSkipStrokePointLimitForHead;      // ストロークポイント出力制限：先頭
    static int s_nSkipStrokePointLimitForTail;      // ストロークポイント出力制限：末尾
    
    //-------------------------------------
    // 入力データ
    //-------------------------------------
    static stBEZIER_BASE_PARAM*  s_pBaseParam;
    static stBEZIER_LAYER_PARAM* s_pLayerParam;

    //-------------------------------------
    // 処理設定
    //-------------------------------------
    static BYTE* s_pBuf;                    // メインバッファ（※ストローク時は不透明色出力先）
    static BYTE* s_pBufTest;                // テストバッファ（※ストロークの出力可能判定用）
    static BYTE* s_pBufFillGuide;           // 塗りつぶしガイド（※ここに描かれた閉領域が塗りつぶしの対象となる）
    static BYTE* s_pBufFillGuard;           // バッファ：塗りガード（マスク）
    static BYTE* s_pBufTemp;                // テンポラリ（※塗り境界のアンチストローク用）
    
    static int s_nBufW, s_nBufH;            // バッファサイズ
    
    static BYTE s_nTestPalGrp;              // テスト値（※[s_pBufTest]内の、この値と同じパレットグループの領域のみが出力対象となる）
    static BYTE s_nTestPalGrpForRepair;     // ライン修復時のテスト値
    static bool s_bTouch;                   // タッチの描画
    static bool s_bStripe;                  // ストライプの描画
    static bool s_bFrill;                   // フリルの描画
    static bool s_bWorkPath;                // ワークパス出力指定
    
    //---------------------------------------------------
    // ワークパス表示用（※アンカーポイント作業用の小さいマーク各種）
    //---------------------------------------------------
    static int  s_nSizeForWorkPathLine;         // パス画素サイズ（ストローク）
    static int  s_nSizeForWorkPathPoint;        // ポイント画素サイズ（アンカーポイント）
    static int  s_nSizeForWorkPathSelected;     // 選択中のポイント画素サイズ（アンカーポイント）
    static int  s_nSizeForWorkPathTriangle;     // 三角画素サイズ（方向線の向き）
    static int  s_nSizeForWorkPathCross;        // バツ画素サイズ（塗りつぶしポイント）
    static int  s_nSizeForWorkPathPlus;         // プラス画素サイズ（スロットポイント／基準点）
    static BYTE* s_pDotForWorkPathTriangle;     // 三角画素（方向線の向き）
    static BYTE* s_pDotForWorkPathCross;        // バツ画素（塗りつぶしポイント）
    static BYTE* s_pDotForWorkPathPlus;         // プラス画素（スロットポイント／基準点）
   
    //---------------------------------------------------
    // フリルデータ（※処理が固まったらMgrで管理するか？）
    //---------------------------------------------------
    static CLayerData* s_pArrLayerDataForFrill[eSTROKE_FRILL_MAX];
    
    //---------------------------------------------------
    // 線のブレ用
    //---------------------------------------------------
    static CSpring s_oSpringForBrushPos;        // バネ：ブラシの位置ブレ
    static CSpring s_oSpringForBrushSize;       // バネ：ブライのサイズブレ
    static int m_nResetCountForBrushPos;        // リセットカウント：ブラシの位置ずれ
    static int m_nResetCountForBrushSize;       // リセットカウント：ブラシのサイズブレ
    static float m_nUpdateFrameForBrushPos;     // 更新カウント：ブラシの位置ずれ
    static float m_nUpdateFrameForBrushSize;    // 更新カウント：ブラシの位置ずれ

public:
    // 初期化／終了
	static bool OnCreate( void );
	static void OnDestroy( void );
    
#ifdef BEZIER_USE_NUM_CHECK
    // 利用状況取得
    static int GetDivisionPointNumMax( void );
    static int GetDivisionPointNumUseMax( void );
    static int GetTempPointNumMax( void );
    static int GetTempPointNumUseMax( void );
    static int GetTouchPointNumMax( void );
    static int GetTouchPointNumUseMax( void );
    static int GetGuidePointNumMax( void );
    static int GetGuidePointNumUseMax( void );
    static int GetStripePointNumMax( void );
    static int GetStripePointNumUseMax( void );
    static int GetStripeGuidePointNumMax( void );
    static int GetStripeGuidePointNumUseMax( void );
#endif

    // フック登録リセット
    static void ResetHookPoint( bool isAll=true );

    // タッチ登録リセット
    static void ResetTouchPoint( bool isAll=true );

    // ガイド登録リセット
    static void ResetGuidePoint( bool isAll=true );

    //------------------------------------------------------------------------------------------
    // ストローク（線）の描画
    //------------------------------------------------------------------------------------------
    // [stBEZIER_ANCHOR_POINT* pHead]で指定される線を描画する
    // [pBuf]に画素が出力される
    // [pBufFillGuide]には塗りつぶし用の領域が１ドットのラインとして出力される（※クローズパスが前提）
    // [pBufFillGuard]により出力が制限される
    // [pBufColor]により出力が制御される（※[pBufColor]の[testVal]に相当する位置の画素しか出力されなくなる）
    // [isDot]によりアンカーポイント間の線の代わりにアンカーポイント上に点を描く
    //------------------------------------------------------------------------------------------
    static int Stroke( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                       BYTE testPalGrp, BYTE testPalGrpForRepair, bool isDot, bool isWorkPath );
    
    //------------------------------------------------------------------------------------------
    // ストローク（タッチ）の描画（※引数の意味は[Stroke]と同じ）
    //（※ワークパス、フックの塗り返し、ドット描画には対応しない）
    //------------------------------------------------------------------------------------------
    static int Touch( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                      BYTE testPalGrp, bool isStripe, bool isWorkPath );
    
    // フリル（※内部でフリルタイプに対応した[CLayerData]を描画する）
    static int TouchForFrill( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                              BYTE testPalGrp, bool isWorkPath );

    //------------------------------------------------------------------------------------------
    // 塗りつぶし
    //------------------------------------------------------------------------------------------
    // [stBEZIER_FILL_POINT* pHead]で指定される情報で[pBufColor]を塗りつぶす
    // [pHead]で指定される点から[pBufFillGuide]で指定される閉領域が塗りつぶされる
    // 塗りつぶしは「下塗り(pBufFillGuide)→本塗り(pBufColor)」の流れで行われる
    //（※下塗り時の[testVal]の参照対象は[pBufColor]となる）
    // [pBufFillGuard]により出力が制限される
    //------------------------------------------------------------------------------------------
    static int Fill( stBEZIER_FILL_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                     BYTE testPalGrp, bool isWorkPath );

    //------------------------------------------------------------------------------------------
    // 情報：スロット（※開発用：スロット位置に印を出力）
    //------------------------------------------------------------------------------------------
    static void SlotForDev( stBEZIER_SLOT_POINT* pHead, stBEZIER_BASE_PARAM* pParam );

    //------------------------------------------------------------------------------------------
    // 情報：基準点（※開発用：基準点に印を出力）
    //------------------------------------------------------------------------------------------
    static void BasePointForDev( float x, float y, stBEZIER_BASE_PARAM* pParam, bool isActive, bool isSelected );
    
private:
    //-----------------------
    // 描画
    //-----------------------
    // 指定の分割点間を線形補間しながら描画（※描画設定は[pAP]から参照する）
    static void PutDotLinear( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP );

    // 分割点間の座標を算出して一時バッファに格納
    static int CalcThenSetPointForTempBuf( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP );

    // ２点間のドット数の算出
    static int  CalcPointNum( float x0, float y0, float x1, float y1 );
    
    // ドットを置く
    static void PutDotAt( float x0, float y0, float strokeSize, BYTE testPalGrp, CStroke* pStroke, stBEZIER_ANCHOR_POINT* pAP );

    //-----------------------
    // 登録関連
    //-----------------------
    // AP間の分割点の登録
    static void RegistDivisionPointList( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 );
    
    // AP間の分割点数の算出
    static int  CalcDivisionNumForSegment( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 );
    
    // 分割点の追加
    static void AddDivisionPoint( float x, float y, float strokeSize, bool isEdgeFillCheck, stBEZIER_ANCHOR_POINT* pAP );
    
    // タッチポイント反転
    static void ReverseTouchPoint( eSTROKE_TOUCH_TARGET targetTouchId, int atFrom, int atTo );
    
    // フックの登録／設定
    static void ResolveHookPoint( stBEZIER_ANCHOR_POINT* pBAP );

    // ストライプ塗りポイントの追加
    static void AddStripePoint( BYTE palOfsGrp, float x, float y );

    //====================================================================================
    // 以下は、[BezierForSub.cpp]で実装
    //====================================================================================
public:
    // ループ間を行き来する際の環境リセット
    static void ResetEnv( void );
    
    // 全フラグ解除（※開発用）
    static void ResetAllFlagForDev( void );

    //---------------------------
    // ガイド復旧
    //---------------------------
    static void RecoverGuide( eSTROKE_GUIDE_TARGET guideTarget );
    static void RecoverGuideWithTouch( eSTROKE_TOUCH_TARGET touchTarget );

private:
    // 描画実体
    static void DrawStroke( stBEZIER_ANCHOR_POINT* pHead, float skipHeadRate=0.0f, float skipTailRate=0.0f );
    static void DrawDot( stBEZIER_ANCHOR_POINT* pHead );

    // ワーククリア
    static void ClearWork( void );
    static void ClearWorkSetting( void );
    static void ClearWorkPoint( void );
    static void ClearWorkStripePoint( void );
    
    // 出力先設定
    static bool SetBufForStroke( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath );
    static bool SetBufForFill( stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath );
    static bool SetBufForDev( stBEZIER_BASE_PARAM* pParam );

    // ブラシのブレ用
    static void ResetBrushShake( void );
    static void SetBrushShake( void );
    static void UpdateBrushShake( void );
    static float GetBrushShakeForPosX( void );
    static float GetBrushShakeForPosY( void );
    static float GetBrushShakeForSize( void );
    
    // APからストロークの取得
    static CStroke* GetStrokeWithAnchorPoint( stBEZIER_ANCHOR_POINT* pAP );
    
    // FPから塗りの取得
    static CFill* GetFillWithFillPoint( stBEZIER_FILL_POINT* pFP );
    
    //====================================================================================
    // 以下は、[BezierForFill.cpp]で実装
    //====================================================================================
private:
    // 塗りつぶしメイン（※ここでオプションによる分岐をする）
    static void DrawFill( stBEZIER_FILL_POINT* pHead );
    
    // 明暗ドット
    static void DrawFillPointBright( float x, float y, int strokeSize, int strokeRange, int ofs );
    static void DrawFillPointDark( float x, float y, int strokeSize, int strokeRange, int ofs );

    // 明暗アンチストローク
    static void DrawFillAntiStrokeBright( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange, int ofs );
    static void DrawFillAntiStrokeDark( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange, int ofs );

    // 一時バッファ描画
    static bool DrawPointForTemp( float x, float y, int strokeSize, int strokeRange );
    static bool DrawStrokeWithTouchForTemp( eSTROKE_TOUCH_TARGET touchId, int strokeSize, int strokeRange );
    static void PutDotAtForTemp( float x0, float y0, CStroke* pStroke );

    //====================================================================================
    // 以下は、[BezierForEdge.cpp]で実装
    //====================================================================================
private:
    static void DrawStrokeEdge( int targetDivisionPoint );
    static bool CheckFillStrokeEdgeStartAt( int* pStartX, int* pStartY, int bL, int bR, int bT, int bB, bool isCheckLeft, bool isCheckUp );
    static bool CheckFillStrokeEdgeAt( int* pStartX, int* pStartY, int bL, int bR, int bT, int bB );
    static void FillStrokeEdgeAt( int from, int x, int y );

    //=================================================
    // 以下は、[BezierForTouch.cpp]で実装
    //=================================================
private:
    // 描画
    static void DrawTouch( stBEZIER_ANCHOR_POINT* pHead );
    static void DrawTouchAt( stBEZIER_ANCHOR_POINT* pPairHead, float x, float y, float size, float rot, float* pCenterX, float* pCenterY,
                             bool isSubValid=false, float subX=0.0f, float subY=0.0f );

    static float CalcTouchWorkForLen0( eSTROKE_TOUCH_TARGET target );
    static int SearchTouchIndexForLen0( eSTROKE_TOUCH_TARGET target, float len, int from, bool isBack );
    static int SearchTouchIndexForSub( eSTROKE_TOUCH_TARGET target, int at, eSTROKE_TOUCH_TARGET targetSub, bool isReverse );
    
    static float CalcRotForTouch( eSTROKE_TOUCH_TARGET target, int at, bool isFlipH, bool isFlipV, float rate, float baseRot );
    static float CalcTouchDirForLen0( eSTROKE_TOUCH_TARGET target, int at, bool isFlipH, bool isFlipV );

    //=================================================
    // 以下は、[BezierForFrill.cpp]で実装
    //=================================================
private:
    static void DrawFrill( stBEZIER_ANCHOR_POINT* pHead );
    static void DrawFrillAt( stBEZIER_ANCHOR_POINT* pBAP, CLayerData* pLD, float x, float y, float size, float baseLen, float rot,
                             bool isSub, float* pX0, float* pY0, float* pX1, float* pY1 );

    //=================================================
    // 以下は、[BezierForFrillLayerData.cpp]で実装
    //=================================================
public:
    // フリルデータの取得（※開発用に外部からも見えるようにしておく）
    static CLayerData* GetLayerDataForFrill( eSTROKE_FRILL frillId );

private:
    static bool AllocFrill( void );
    static void ReleaseFrill( void );

    static bool AllocFrillForTestCircle( void );
    static bool AllocFrillForTestTriangle( void );
    static bool AllocFrillForTestSquare( void );
    static bool AllocFrillForFluffySquareMain( void );
    static bool AllocFrillForFluffySquareSub( void );

    //====================================================================================
    // 以下は、[BezierForWork.cpp]で実装
    //====================================================================================
private:
    //----------------------------------------------
    // ワークパス用
    //----------------------------------------------
    static void ReadyDotForWorkPath( void );
    static void PutTexForWorkPath( float x0, float y0, int size, BYTE* pTex, BYTE pal, int cut=0 );
    
    static void PutLineDotForWorkPath( float x0, float y0, BYTE pal );
    static void PutPointForWorkPath( float x0, float y0, BYTE pal );
    static void PutTriangleForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal, bool isFlip );
    static void PutCrossForWorkPath( float x0, float y0, BYTE pal );
    static void PutPlusForWorkPath( float x0, float y0, BYTE pal );
    static void PutPlusForWorkPathS( float x0, float y0, BYTE pal );
    
    static void PutLinearForWorkPath( float x0, float y0, float ofsX, float ofsY, BYTE pal );

private:
    // インスタンス作成は不可
    CBezier( void ){}
    virtual ~CBezier( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_HPP__ */
