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
#include "draw/tex/adjust/AdjustablePoint.hpp"
#include "draw/tex/adjust/JointPoint.hpp"

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
    eAPD_FLAG_TRANSPARENCY,                 // 01:[?] 透明（※塗りガイドを出力したいが線は描画したくない時）
    eAPD_FLAG_NO_FILL_GUIDE,                // 02:[/] 線のみ（※線を描画したいが塗りガイドを出力したくない時）
    eAPD_FLAG_DOT_ON_OUT_COL,               // 03:[@] 出力色の上にのみストロークを描画するか？
    eAPD_FLAG_DOT_ERASE,                    // 04:[E] 消去ストロークか？
    eAPD_FLAG_LINE_REPAIR_TEST,             // 05:[lrT] 線修復テストを行うか？（※[ClineObjectData::getTestPalOfsIdForRepair]に対して線を引くか？）
    eAPD_FLAG_REVERSE_TOUCH_POINT,          // 06:[rvT] タッチ点の登録を逆に行うか？（※タッチ点の登録は根元から先端へ向けて行う）
    eAPD_FLAG_REVERSE_TOUCH_SUB,            // 07:[rvS] タッチサブの座標を反転参照するか？
    eAPD_FLAG_JOINT_OR_FRILL_DIR_SWAP,      // 08:[dS] ジョイント時の方向線／フリル時の方向をスワップする（※INとOUTの入れ替え／フック座標の入れ替え）
    eAPD_FLAG_JOINT_NO_HOOK_IF_OVERWRAPPED, // 09:[nH] ジョイント時に角度が重なっていた場合に結合先にフックしない（※肘下の内側の線が食いこむ感じを出したい時用）
    eAPD_FLAG_AS_TOUCH_STROKE,              // 10:[tS] タッチストロークとして扱う（※タッチオフ時に透明になる）

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
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                          // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;            // [X]: 座標（※画像サイズに対する割合座標）
    //CAdjustablePoint m_oAdjInXY;          // [X]: 進入方向線座標（※アンカーポイントからの相対座標）
    //CAdjustablePoint m_oAdjOutXY;         // [X]: 出立方向線座標（※アンカーポイントからの相対座標）

    // IDの指定
    eJOINT_POINT m_eJointPointId;           // [enum]: 結合ポイントID（※[BDP]オーダーリスト経由でないと有効処理されない）
    eSTROKE_HOOK_TARGET m_eHookTargetId;    // [enum]: フック対象ID
    eSTROKE_GUIDE_TARGET m_eGuideTargetId;  // [enum]: ガイド対象ID
    
    //------------------------------------------------------------------------------
    // ストローク強弱の指定（※基本サイズは[CLineObjectData]に指定された値から受け取る）
    //------------------------------------------------------------------------------
    // 開始範囲と終了範囲を０に指定すると[開始サイズ〜終了サイズ]で変動する基本サイズへの補正
    //------------------------------------------------------------------------------
    int m_nStrokeStartRange;                // [2]; ストローク：開始範囲:[0〜100%]（※開始サイズ適用範囲＝[0.0f〜StartRange]が範囲となる）
    int m_nStrokeStartSize;                 // [2]: ストローク：開始サイズ:[0〜300%]（※開始範囲内で[開始サイズ〜100%]で変動する基本サイズへの補正）
    int m_nStrokeEndRange;                  // [2]: ストローク：終了範囲:[0〜100%]（※終了サイズ適用範囲＝[(1.0-EndRange)〜1.0f]が範囲となる）
    int m_nStrokeEndSize;                   // [2]: ストローク：終了サイズ:[0〜300%]（※終了範囲内で[100%〜終了サイズ]で変動する基本サイズへの補正）
    int m_nStrokeEdgeFillSize;              // [2]: ストローク：エッジ塗りつぶしサイズ[0〜300%]（※基本サイズは[8*GM_P_RATE]）

    //------------------------------------------------------------------------------
    // タッチ＆ストライプ＆フリル指定
    //------------------------------------------------------------------------------
    // タッチ詳細（※タッチ描画[CLineObjectData::eLOD_FLAG_TOUCHが立っている]時に参照される）
    //（※[eLOD_FLAG_TOUCH_IS_FRILL]が立っているとフリル向けの処理となる）
    //------------------------------------------------------------------------------
    eSTROKE_TOUCH_TARGET m_eTouchTargetId;      // [enum]: タッチ：対象ID（※タッチ登録時／タッチ描画時の両方で指定）
    eSTROKE_TOUCH_TARGET m_eTouchTargetSubId;   // [enum]: タッチ：サブID（※ストライプ時のサブID）

    ePAL_OFS m_eStripeOrFrillMainPalOfsId;      // [enum]: ストライプ／フリル：パレットオフセットID（※メイン色）
    ePAL_OFS m_eStripeOrFrillSubPalOfsId;       // [enum]: ストライプ／フリル：パレットオフセットID（※サブ色）

    eSTROKE_FRILL m_eFrillMainId;               // [enum]: フリル：素材ID（※メイン）
    eSTROKE_FRILL m_eFrillSubId;                // [enum]: フリル：素材ID（※サブ）

    // [2]: 基本位置:[0〜100%]（※描画を開始する起点：5000で中央開始）
    int m_nTouchBasePos;
    
    // [2]: 基本サイズ:[0〜300%]（※基本描画サイズ：10000で100%）
    int m_nTouchBaseSize;
    
    // [2]: 基本角度:[-360〜360]（※基本描画角度：１０倍精度）
    int m_nTouchBaseRot;
    
    // [2]: 前方（手前）エリア:[0〜100%]（※基本位置から手前の領域にかけてどこまで表示するかの割合：10000で前方の端まで）
    int m_nTouchFrontArea;
    
    // [2]: 共通：前方（手前）サイズ:[-100〜300%]（※基本位置から手前の領域にかけての表示サイズオフセット）
    int m_nTouchFrontSizeOfs;

    // [2]: 前方（手前）回転適用率:[-100〜100%]（※タッチの角度補正適用率）
    int m_nTouchFrontRotRate;
    
    // [2]: 後方（奥）エリア:[0〜100%]（※基本位置から奥の領域にかけてどこまで表示するかの割合：10000で後方の端まで）
    int m_nTouchBackArea;
    
    // [2]: 後方（奥）サイズ:[-100〜300%]（※基本位置から奥の領域にかけての表示サイズオフセット）
    int m_nTouchBackSizeOfs;
    
    // [2]: 後方（奥）回転適用率:[-100〜100%]（※タッチの角度補正適用率）
    int m_nTouchBackRotRate;

    // [2]: 共通：前方の要素数:[0-99]
    int m_nTouchFrontNum;

    // [2]: 前方要素のスキップ数:[0-99]（※要素は始点と終端を表示するように配置されるが、パスが閉じている場合はこの値で終端をスキップする）
    int m_nTouchFrontSkip;

    // [2]: 前方のボーダーサイズ:[0〜100%]（※この値が有効だとタッチが二重線になる）
    // フリル：無効
    int m_nTouchFrontBorderRate;

    // [2]: 後方の要素数:[0-99]
    int m_nTouchBackNum;
    
    // [2]: 後方要素のキップ数:[0-99]（※要素始点と終端を表示するように配置されるが、パスが閉じている場合はこの値で終端をスキップする）
    int m_nTouchBackSkip;
    
    // [2]: 後方のボーダーサイズ:[0〜100%]（※この値が有効だとタッチが二重線になる）
    // フリル：無効
    int m_nTouchBackBorderRate;
    
    // [2]: 座標のランダム域:[0〜100%]（※座標をラダムで補正する）
    int m_nTouchRandomOfsFor0;      // フリルの場合は[MAIN]
    int m_nTouchRandomOfsFor1;      // フリルの場合は[SUB]

    // [2]: ストロークの先頭／尻尾のスキップ割合[0〜100%]（※先頭と尻尾の線を不揃いにする場合）
    int m_nTouchHeadSkipRate;
    int m_nTouchTailSkipRate;
    
    // [2]: フリルのメイン／サブの横幅補正[-300〜300%]（※フリルの割り当て後の横幅を補正する／縦幅は通常のサイズで補正する想定）
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
    inline void setHookTargetId( eSTROKE_HOOK_TARGET id ){ m_eHookTargetId = id; }
    inline void setGuideTargetId( eSTROKE_GUIDE_TARGET id ){ m_eGuideTargetId = id; }

    // ストローク
    void setStroke( int startRange, int startSize, int endRange, int endSize );
    inline void setStrokeStartRange( int range ){ m_nStrokeStartRange = range; }
    inline void setStrokeStartSize( int size ){ m_nStrokeStartSize = size; }
    inline void setStrokeEndRange( int range ){ m_nStrokeEndRange = range; }
    inline void setStrokeEndSize( int size ){ m_nStrokeEndSize = size; }
    inline void setStrokeEdgeFillSize( int size ){ m_nStrokeEdgeFillSize = size; }

    // タッチ＆ストライプ＆フリル
    inline void setTouchTargetId( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetId = touchTargetId; }
    inline void setTouchTargetSubId( eSTROKE_TOUCH_TARGET touchTargetId ){ m_eTouchTargetSubId = touchTargetId; }
    inline void setStripeOrFrillMainPalOfsId( ePAL_OFS palOfsId ){ m_eStripeOrFrillMainPalOfsId = palOfsId; }
    inline void setStripeOrFrillSubPalOfsId( ePAL_OFS palOfsId ){ m_eStripeOrFrillSubPalOfsId = palOfsId; }
    inline void setFrillMainId( eSTROKE_FRILL frillId ){ m_eFrillMainId = frillId; }
    inline void setFrillSubId( eSTROKE_FRILL frillId ){ m_eFrillSubId = frillId; }

    void setTouchBase( int pos, int size, int rot );
    void setTouchFront( int area, int sizeOfs, int rotRate, int num, int skip, int borderRate=0 );
    void setTouchBack( int area, int sizeOfs, int rotRate, int num, int skip, int borderRate=0 );
    void setTouchRandomOfs( int rand0, int rand1 );
    void setTouchSkipRate( int rateHead, int rateTail );
    void setFrillWidthRate( int rateMain, int rateSub );

	//----------------
	// 取得
	//----------------
    // ID
    eJOINT_POINT getJointPointId( int slotIndex=-1 );
    eSTROKE_HOOK_TARGET getHookTargetId( eBD_SLOT slot=eBD_SLOT_INVALID, int slotIndex=-1 );
    eSTROKE_GUIDE_TARGET getGuideTargetId( int slotIndex=-1 );

    // ストローク
    inline int getStrokeStartRange( void ){ return( m_nStrokeStartRange ); }
    inline int getStrokeStartSize( void ){ return( m_nStrokeStartSize ); }
    inline int getStrokeEndRange( void ){ return( m_nStrokeEndRange ); }
    inline int getStrokeEndSize( void ){ return( m_nStrokeEndSize ); }
    inline int getStrokeEdgeFillSize( void ){ return( m_nStrokeEdgeFillSize );}

    // タッチ＆ストライプ＆フリル
    eSTROKE_TOUCH_TARGET getTouchTargetId( int slotIndex=-1 );
    eSTROKE_TOUCH_TARGET getTouchTargetSubId( int slotIndex=-1 );
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
    inline int getTouchFrontRotRate( void ){ return( m_nTouchFrontRotRate ); }
    inline int getTouchBackArea( void ){ return( m_nTouchBackArea ); }
    inline int getTouchBackSizeOfs( void ){ return( m_nTouchBackSizeOfs ); }
    inline int getTouchBackRotRate( void ){ return( m_nTouchBackRotRate ); }
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
    inline int getFrillWidthRateForMain( void ){ return( m_nFrillWidthRateForMain ); }
    inline int getFrillWidthRateForSub( void ){ return( m_nFrillWidthRateForSub ); }

    //-------------------------------
    // [pPrev]と[pNext]の中間に配置
    //-------------------------------
    void setAtIntermediate( CAnchorPointData* pPrev, CAnchorPointData* pNext );
    
    //------------
    // 鏡面コピー
    //------------
    // ヨコ
    void fixForMirrorCopyH( void );
    void fixAsHeadForMirrorCopyH( void );
    void fixAsTailForMirrorCopyH( void );

    // タテ
    void fixForMirrorCopyV( void );
    void fixAsHeadForMirrorCopyV( void );
    void fixAsTailForMirrorCopyV( void );

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __ANCHOR_POINT_DATA_HPP__
