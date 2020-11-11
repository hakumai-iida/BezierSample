/*+----------------------------------------------------------------+
  |	Title:		AnchorPointData.hpp [共通環境]
  |	Comment:	アンカーポイントデータ（※[CLineObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ANCHOR_POINT_DATA_HPP__
#define __ANCHOR_POINT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "draw/bezier/adjust/AdjustablePoint.hpp"
#include "draw/bezier/adjust/JointPoint.hpp"
#include "draw/bezier/stroke/Stroke.hpp"
#include "draw/bezier/fill/Fill.hpp"

#include "sys/data/ListNode.hpp"
#include "sys/data/DataCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eAPD_FLAG{
	eAPD_FLAG_INVALID = -1,                 // 無効値

    eAPD_FLAG_DISABLE,                      // 00:[*] 無効（※[CLineObjectData::CreateBezierAnchorPoint]で登録されない）
    eAPD_FLAG_HOOK_WITH_OFS,                // 01:[wO] フック時にオフセットを加味する
    eAPD_FLAG_TRANSPARENCY,                 // 02:[?] 透明（※塗りガイドを出力したいが線は描画したくない時）
    eAPD_FLAG_NO_FILL_GUIDE,                // 03:[/] 線のみ（※線を描画したいが塗りガイドを出力したくない時）
    eAPD_FLAG_DOT_ON_OUT_COL,               // 04:[@] 出力色のみをストローク描画対象とする（※直近の塗りの上にタッチを描く時等）
    eAPD_FLAG_DOT_ERASE,                    // 05:[-] 消去ストローク
    eAPD_FLAG_TEST_LINE_REPAIR,             // 06:[+] 線修復テストを行う（※[CLineObjectData::getTestPalOfsIdForRepair]に対して線を引くか？）
    eAPD_FLAG_REF_JOINT_POINT,              // 07:[rJ] ジョイントポイントを参照する
    eAPD_FLAG_REVERSE_JOINT_DIR,            // 08:[vD] ジョイント時の方向線をスワップする（※INとOUTの入れ替え／フック座標の入れ替え）
    eAPD_FLAG_REVERSE_FRILL_HOOK,           // 09:[vF] フリルのフックを反転する
    eAPD_FLAG_AS_TOUCH_STROKE,              // 10:[asT] タッチストロークとして扱う（※タッチオフ時に描画されない）
    eAPD_FLAG_TOUCH_SKIP_FRONT_IF_MC,       // 11:[tsF] ミラーコピー時にタッチの先頭をスキップする（※先端要素の重複回避用）
    eAPD_FLAG_TOUCH_SKIP_BACK_IF_MC,        // 12:[tsB] ミラーコピー時にタッチの末尾をスキップする（※末端要素の重複回避用）

    eAPD_FLAG_MAX,		                    // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eAPD_FLAG_TEMP_SELECTED = 16,           // 編集対象として選択されている（※表示切り替え用）
    eAPD_FLAG_TEMP_ACTIVE,                  // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// アンカーポイントデータ
//----------------------------------------
class CAnchorPointData: public CListNode{
    // 各種実装
    IMPLEMENT_ADJUSTABLE_XY                             // 実装：調整XY座標
    IMPLEMENT_ADJUSTABLE_IN_XY                          // 実装：調整進入XY座標
    IMPLEMENT_ADJUSTABLE_OUT_XY                         // 実装：調整出立XY座標
    IMPLEMENT_NODE( CAnchorPointData )                  // 実装：リストノード
    IMPLEMENT_DATA( CAnchorPointData )                  // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eAPD_FLAG )                    // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eAPD_FLAG )                // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                               // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CAnchorPointData ) // 実装：クラスメソッドによるインスタンス確保／解放

public:
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                              // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;                // [X]: 座標（※[IMPLEMENT_ADJUSTABLE_XY]で宣言）
    //CAdjustablePoint m_oAdjInXY;              // [X]: 進入方向線座標（※[IMPLEMENT_ADJUSTABLE_IN_XY]で宣言）
    //CAdjustablePoint m_oAdjOutXY;             // [X]: 出立方向線座標（※[IMPLEMENT_ADJUSTABLE_OUT_XY]で宣言）

    // IDの指定
    eJOINT_POINT m_eJointPointId;               // [enum]: 結合ポイントID（※[BDP]オーダーリスト経由でないと有効処理されない）
    eJOINT_POINT m_eJointPointIdForMC;          // [enum]: ミラーコピー用

    eSTROKE_HOOK_TARGET m_eHookTargetId;        // [enum]: フック対象ID
    eSTROKE_HOOK_TARGET m_eHookTargetIdForMC;   // [enum]: ミラーコピー用

    eSTROKE_HOOK_TARGET m_eReHookTargetId;      // [enum]: 再フック対象ID
    eSTROKE_HOOK_TARGET m_eReHookTargetIdForMC; // [enum]: 再ミラーコピー用

    eSTROKE_GUIDE_TARGET m_eGuideTargetId;      // [enum]: ガイド対象ID
    eSTROKE_GUIDE_TARGET m_eGuideTargetIdForMC; // [enum]: ミラーコピー用

    //------------------------------------------------------------------------------
    // ストローク強弱の指定（※基本サイズは[CLineObjectData]に指定された値から受け取る）
    //------------------------------------------------------------------------------
    // 開始範囲と終了範囲を０に指定すると[開始サイズ〜終了サイズ]で変動する基本サイズへの補正
    //------------------------------------------------------------------------------
    int m_nStrokeStartSize;                     // [2]: ストローク：開始サイズ:[0〜300%]（※開始範囲内で[開始サイズ〜100%]で変動する基本サイズへの補正）
    int m_nStrokeStartRange;                    // [2]; ストローク：開始範囲:[0〜100%]（※開始サイズ適用範囲＝[0.0f〜StartRange]が範囲となる）
    int m_nStrokeEndSize;                       // [2]: ストローク：終了サイズ:[0〜300%]（※終了範囲内で[100%〜終了サイズ]で変動する基本サイズへの補正）
    int m_nStrokeEndRange;                      // [2]: ストローク：終了範囲:[0〜100%]（※終了サイズ適用範囲＝[(1.0-EndRange)〜1.0f]が範囲となる）
    int m_nStrokeEdgeFillSize;                  // [2]: ストローク：エッジ塗りつぶしサイズ[0〜300%]（※基本サイズは[8*GM_P_RATE]）

    //------------------------------------------------------------------------------
    // タッチ＆ストライプ＆フリル指定
    //------------------------------------------------------------------------------
    // タッチ詳細（※タッチ描画[CLineObjectData::eLOD_FLAG_TOUCHが立っている]時に参照される）
    //（※[eLOD_FLAG_TOUCH_IS_STRIPE]が立っているとストライプ向けの処理となる）
    //（※[eLOD_FLAG_TOUCH_IS_FRILL]が立っているとフリル向けの処理となる）
    //------------------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET m_eTouchTargetId;          // [enum]: タッチ：対象ID（※タッチ登録時／タッチ描画時の両方で指定）
    eSTROKE_TOUCH_TARGET m_eTouchTargetIdForMC;     // [enum]: ミラーコピー用

    eSTROKE_TOUCH_TARGET m_eTouchTargetSubId;       // [enum]: タッチ：サブID（※指定があるとタッチの終点がここにフックされる）
    eSTROKE_TOUCH_TARGET m_eTouchTargetSubIdForMC;  // [enum]: ミラーコピー用

    ePAL_OFS m_eStripeOrFrillMainPalOfsId;          // [enum]: ストライプ／フリル：パレットオフセットID（※メイン色）
    ePAL_OFS m_eStripeOrFrillSubPalOfsId;           // [enum]: ストライプ／フリル：パレットオフセットID（※サブ色）

    eSTROKE_FRILL m_eFrillMainId;                   // [enum]: フリル：素材ID（※メイン）
    eSTROKE_FRILL m_eFrillSubId;                    // [enum]: フリル：素材ID（※サブ）

    // [2]: 基本位置:[0〜100%]（※描画を開始する起点：5000で中央開始）
    int m_nTouchBasePos;
    
    // [2]: 基本サイズ:[-300〜300%]（※基本描画サイズ：10000で100%）
    int m_nTouchBaseSize;
    
    // [2]: 基本角度:[-360〜360]（※基本描画角度：１０倍精度）
    int m_nTouchBaseRot;
    
    // [2]: 前方（手前）エリア:[0〜100%]（※基本位置から手前の領域にかけてどこまで表示するかの割合：10000で前方領域の端まで）
    int m_nTouchFrontArea;
    
    // [2]: 前方（手前）サイズオフセット:[-300〜300%]（※基本位置から手前の領域にかけての表示サイズオフセット）
    int m_nTouchFrontSizeOfs;

    // [2]: 前方（手前）回転:[-3600*8〜3600*8]（※基本角度から手前の領域にかけての回転オフセット）
    int m_nTouchFrontRotOfs;

    // [2]: 後方（奥）エリア:[0〜100%]（※基本位置から奥の領域にかけてどこまで表示するかの割合：10000で後方領域の端まで）
    int m_nTouchBackArea;
    
    // [2]: 後方（奥）サイズ:[-300〜300%]（※基本位置から奥の領域にかけての表示サイズオフセット）
    int m_nTouchBackSizeOfs;
    
    // [2]: 後方（奥）回転:[-3600*8〜3600*8（※基本角度から奥の領域にかけての回転オフセット）
    int m_nTouchBackRotOfs;

    // [2]: 前方の要素数:[0-99]
    int m_nTouchFrontNum;

    // [2]: 前方要素のスキップ数:[0-99]（※要素は始点と終端を表示するように配置されるが、パスが閉じている場合はこの値で重複を回避する）
    int m_nTouchFrontSkip;

    // [2]: 前方のボーダーサイズ:[0〜100%]（※この値が有効だとタッチが二重線になる）（※フリル時は無視される）
    int m_nTouchFrontBorderRate;

    // [2]: 後方の要素数:[0-99]
    int m_nTouchBackNum;
    
    // [2]: 後方要素のスキップ数:[0-99]（※要素始点と終端を表示するように配置されるが、パスが閉じている場合はこの値で重複を回避する）
    int m_nTouchBackSkip;
    
    // [2]: 後方のボーダーサイズ:[0〜100%]（※この値が有効だとタッチが二重線になる）（※フリル時は無視される）
    int m_nTouchBackBorderRate;
    
    // [2]: 座標のランダム域:[0〜100%]（※座標をラダムで補正する）
    int m_nTouchRandomOfsFor0;      // フリルの場合は[MAIN]の座標を補正
    int m_nTouchRandomOfsFor1;      // フリルの場合は[SUB]の座標を補正

    // [2]: ストロークの先頭／末尾のスキップ割合[0〜100%]（※先頭と末尾の線を不揃いにする場合）（※フリル時は無視される）
    int m_nTouchHeadSkipRate;
    int m_nTouchTailSkipRate;
    
    // [2]: 参照位置の補正[-100〜100%]（※サブのタッチ参照をずらすために利用する想定）
    int m_nTouchRefOfsRateForMain;
    int m_nTouchRefOfsRateForSub;

    // [2]: フリルのメイン／サブの横幅補正[-300〜300%]（※フリルの割り当て後の横幅を補正する／縦幅は通常のサイズで補正する想定）（※タッチ時は無視される）
    int m_nFrillWidthRateForMain;
    int m_nFrillWidthRateForSub;

public:
    // コンストラクタ／デストラクタ
    CAnchorPointData( void );
    virtual ~CAnchorPointData( void ){}

    //----------------
    // 設定
    //----------------
    // 基本
    void set( int rX, int rY, int inRX, int inRY, int outRX, int outRY );

    // ID
    inline void setJointPointId( eJOINT_POINT id ){ m_eJointPointId = id; }
    inline void setJointPointIdForMC( eJOINT_POINT id ){ m_eJointPointIdForMC = id; }

    inline void setHookTargetId( eSTROKE_HOOK_TARGET id ){ m_eHookTargetId = id; }
    inline void setHookTargetIdForMC( eSTROKE_HOOK_TARGET id ){ m_eHookTargetIdForMC = id; }

    inline void setReHookTargetId( eSTROKE_HOOK_TARGET id ){ m_eReHookTargetId = id; }
    inline void setReHookTargetIdForMC( eSTROKE_HOOK_TARGET id ){ m_eReHookTargetIdForMC = id; }

    inline void setGuideTargetId( eSTROKE_GUIDE_TARGET id ){ m_eGuideTargetId = id; }
    inline void setGuideTargetIdForMC( eSTROKE_GUIDE_TARGET id ){ m_eGuideTargetIdForMC = id; }

    // ストローク
    void setStroke( int startSize, int startRange, int endSize, int endRange );
    inline void setStrokeStartSize( int size ){ m_nStrokeStartSize = size; }
    inline void setStrokeStartRange( int range ){ m_nStrokeStartRange = range; }
    inline void setStrokeEndSize( int size ){ m_nStrokeEndSize = size; }
    inline void setStrokeEndRange( int range ){ m_nStrokeEndRange = range; }
    inline void setStrokeEdgeFillSize( int size ){ m_nStrokeEdgeFillSize = size; }

    // タッチ＆ストライプ＆フリル
    inline void setTouchTargetId( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetId = touchTargetId; }
    inline void setTouchTargetIdForMC( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetIdForMC = touchTargetId; }

    inline void setTouchTargetSubId( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetSubId = touchTargetId; }
    inline void setTouchTargetSubIdForMC( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetSubIdForMC = touchTargetId; }

    inline void setStripeOrFrillMainPalOfsId( ePAL_OFS palOfsId ){ m_eStripeOrFrillMainPalOfsId = palOfsId; }
    inline void setStripeOrFrillSubPalOfsId( ePAL_OFS palOfsId ){ m_eStripeOrFrillSubPalOfsId = palOfsId; }
    inline void setFrillMainId( eSTROKE_FRILL frillId ){ m_eFrillMainId = frillId; }
    inline void setFrillSubId( eSTROKE_FRILL frillId ){ m_eFrillSubId = frillId; }

    void setTouchBase( int pos, int size, int rot );
    void setTouchFront( int area, int sizeOfs, int rotOfs, int num, int skip, int borderRate=0 );
    void setTouchBack( int area, int sizeOfs, int rotOfs, int num, int skip, int borderRate=0 );
    void setTouchRandomOfs( int rand0, int rand1 );
    void setTouchSkipRate( int rateHead, int rateTail );
    void setTouchRefOfsRate( int rateMain, int rateSub );
    void setFrillWidthRate( int rateMain, int rateSub );

	//----------------
	// 取得
	//----------------
    // ID
    int fixJointPointId( int slotIndex, bool isMC );
    int fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC );
    int fixReHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC );
    int fixGuideTargetId( int slotIndex, bool isMC );
    
    inline eJOINT_POINT getJointPointId( void ){ return( m_eJointPointId ); }
    inline eJOINT_POINT getJointPointIdForMC( void ){ return( m_eJointPointIdForMC ); }
    
    inline eSTROKE_HOOK_TARGET getHookTargetId( void ){ return( m_eHookTargetId ); }
    inline eSTROKE_HOOK_TARGET getHookTargetIdForMC( void ){ return( m_eHookTargetIdForMC ); }

    inline eSTROKE_HOOK_TARGET getReHookTargetId( void ){ return( m_eReHookTargetId ); }
    inline eSTROKE_HOOK_TARGET getReHookTargetIdForMC( void ){ return( m_eReHookTargetIdForMC ); }

    inline eSTROKE_GUIDE_TARGET getGuideTargetId( void ){ return( m_eGuideTargetId ); }
    inline eSTROKE_GUIDE_TARGET getGuideTargetIdForMC( void ){ return( m_eGuideTargetIdForMC ); }

    // ストローク
    inline int getStrokeStartSize( void ){ return( m_nStrokeStartSize ); }
    inline int getStrokeStartRange( void ){ return( m_nStrokeStartRange ); }
    inline int getStrokeEndSize( void ){ return( m_nStrokeEndSize ); }
    inline int getStrokeEndRange( void ){ return( m_nStrokeEndRange ); }
    inline int getStrokeEdgeFillSize( void ){ return( m_nStrokeEdgeFillSize );}

    // タッチ＆ストライプ＆フリル
    int fixTouchTargetId( int slotIndex, bool isMC );
    int fixTouchTargetSubId( int slotIndex, bool isMC );
    
    inline eSTROKE_TOUCH_TARGET getTouchTargetId( void ){ return( m_eTouchTargetId ); }
    inline eSTROKE_TOUCH_TARGET getTouchTargetIdForMC( void ){ return( m_eTouchTargetIdForMC ); }

    inline eSTROKE_TOUCH_TARGET getTouchTargetSubId( void ){ return( m_eTouchTargetSubId ); }
    inline eSTROKE_TOUCH_TARGET getTouchTargetSubIdForMC( void ){ return( m_eTouchTargetSubIdForMC ); }

    inline ePAL_OFS getStripeOrFrillMainPalOfsId( void ){ return( m_eStripeOrFrillMainPalOfsId ); }
    inline BYTE getStripeOrFrillMainPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_eStripeOrFrillMainPalOfsId ) ); }
    inline ePAL_OFS getStripeOrFrillSubPalOfsId( void ){ return( m_eStripeOrFrillSubPalOfsId ); }
    inline BYTE getStripeOrFrillSubPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_eStripeOrFrillSubPalOfsId ) ); }
    inline eSTROKE_FRILL getFrillMainId( void ){ return( m_eFrillMainId ); }
    inline eSTROKE_FRILL getFrillSubId( void ){ return( m_eFrillSubId ); }

    inline int getTouchBasePos( void ){ return( m_nTouchBasePos ); }
    inline int getTouchBaseSize( void ){ return( m_nTouchBaseSize ); }
    inline int getTouchBaseRot( void ){ return( m_nTouchBaseRot ); }
    inline int getTouchFrontArea( void ){ return( m_nTouchFrontArea ); }
    inline int getTouchFrontSizeOfs( void ){ return( m_nTouchFrontSizeOfs ); }
    inline int getTouchFrontRotOfs( void ){ return( m_nTouchFrontRotOfs ); }
    inline int getTouchBackArea( void ){ return( m_nTouchBackArea ); }
    inline int getTouchBackSizeOfs( void ){ return( m_nTouchBackSizeOfs ); }
    inline int getTouchBackRotOfs( void ){ return( m_nTouchBackRotOfs ); }
    inline int getTouchFrontNum( void ){ return( m_nTouchFrontNum ); }
    inline int getTouchFrontSkip( void ){ return( m_nTouchFrontSkip ); }
    inline int getTouchFrontBorderRate( void ){ return( m_nTouchFrontBorderRate ); }
    inline int getTouchBackNum( void ){ return( m_nTouchBackNum ); }
    inline int getTouchBackSkip( void ){ return( m_nTouchBackSkip ); }
    inline int getTouchBackBorderRate( void ){ return( m_nTouchBackBorderRate ); }
    inline int getTouchRandomOfsFor0( void ){ return( m_nTouchRandomOfsFor0 ); }
    inline int getTouchRandomOfsFor1( void ){ return( m_nTouchRandomOfsFor1 ); }
    inline int getTouchHeadSkipRate( void ){ return( m_nTouchHeadSkipRate ); }
    inline int getTouchTailSkipRate( void ){ return( m_nTouchTailSkipRate ); }
    inline int getTouchRefOfsRateForMain( void ){ return( m_nTouchRefOfsRateForMain ); }
    inline int getTouchRefOfsRateForSub( void ){ return( m_nTouchRefOfsRateForSub ); }
    inline int getFrillWidthRateForMain( void ){ return( m_nFrillWidthRateForMain ); }
    inline int getFrillWidthRateForSub( void ){ return( m_nFrillWidthRateForSub ); }
    
    //------------
    // 調整値クリア
    //------------
    inline void clearAdjust( void ){
        m_oAdjXY.clearAdjust();
        m_oAdjInXY.clearAdjust();
        m_oAdjOutXY.clearAdjust();
    }

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
    
#if 0
    //----------------------------------------------------------
    // [pPrev]と[pNext]の中間に配置
    //（※現状、利用はしていないが後で使うかもしれないのでコードは残しておく）
    //----------------------------------------------------------
    void setAtIntermediate( CAnchorPointData* pPrev, CAnchorPointData* pNext );
#endif
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __ANCHOR_POINT_DATA_HPP__
