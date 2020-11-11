/*+----------------------------------------------------------------+
  |	Title:		FillPointData.hpp [共通環境]
  |	Comment:	塗りポイントデータ（※[CPaintObjectData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILL_POINT_DATA_HPP__
#define __FILL_POINT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "draw/bezier/adjust/AdjustablePoint.hpp"
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
enum eFPD_FLAG{
	eFPD_FLAG_INVALID = -1,         // 無効値

    eFPD_FLAG_DISABLE,	            // 00:[*] 無効（※[CPaintObjectData::CreateBezierFillPoint]で登録されない）
    eFPD_FLAG_HOOK_WITH_OFS,        // 01:[wO] フック時にオフセットを加味する
    eFPD_FLAG_TRANSPARENCY,         // 02:[?] 透明（※表示はしないが塗り処理を行う＝塗りガイドを埋めてマスク的な利用をする際）
    eFPD_FLAG_FILL_ON_OUT_COL,      // 03:[@] 出力色のみを塗り対象とする
    
    eFPD_FLAG_MAX,		            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eFPD_FLAG_TEMP_SELECTED = 16,   // 編集対象として選択されている（※表示切り替え用）
    eFPD_FLAG_TEMP_ACTIVE,          // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 塗りポイントデータ
//----------------------------------------
class CFillPointData: public CListNode{
    // 各種実装
    IMPLEMENT_ADJUSTABLE_XY                             // 実装：調整XY座標
    IMPLEMENT_NODE( CFillPointData )                    // 実装：リストノード
    IMPLEMENT_DATA( CFillPointData )                    // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eFPD_FLAG )                    // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eFPD_FLAG )                // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                               // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CFillPointData )   // 実装：クラスメソッドによるインスタンス確保／解放

public:
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                                          // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CAdjustablePoint m_oAdjXY;                            // [X]: 座標（※[IMPLEMENT_ADJUSTABLE_XY]で宣言）
    
    // フック
    eSTROKE_HOOK_TARGET m_eHookTargetId;                    // [enum]: フック対象ID
    eSTROKE_HOOK_TARGET m_eHookTargetIdForMC;               // [enum]: ミラーコピー用

    // 塗りオプション
    eFILL_OPTION m_eFillOption;                             // [enum]: 塗りオプション
    int m_nDarkOfsForOption;                                // [2]: オプション用の明暗オフセット（※負で明るく、正で暗く）
    eSTROKE_TOUCH_TARGET m_eTouchTargetIdForOption;         // [enum]: オプションの対象タッチID（※塗り境界のアンチ用）
    eSTROKE_TOUCH_TARGET m_eTouchTargetIdForOptionForMC;    // [enum]: ミラーコピー用
    eBD_MASK_TARGET m_eMaskTargetForOption;                 // [enum]: 塗りつぶしマスク対象
    int m_nStrokeSizeForOption;                             // [2]: オプションに対するストロークサイズ（※ゲーム解像度サイズ）
    int m_nStrokeRangeForOption;                            // [2]: オプションに対するストロークレンジ（※ゲーム解像度サイズ）
    int m_nOfsXForOption;                                   // [2]: オプションに対するＸオフセット（※ゲーム解像度サイズ）
    int m_nOfsYForOption;                                   // [2]: オプションに対するＹオフセット（※ゲーム解像度サイズ）

public:
    // コンストラクタ／デストラクタ
    CFillPointData( void );
    virtual ~CFillPointData( void ){}
    
    //------------
    // 設定
    //------------
    inline void set( int rX, int rY ){ m_oAdjXY.setRateXY( rX, rY ); }

    inline void setHookTargetId( eSTROKE_HOOK_TARGET id ){ m_eHookTargetId = id; }
    inline void setHookTargetIdForMC( eSTROKE_HOOK_TARGET id ){ m_eHookTargetIdForMC = id; }

    inline void setOption( eFILL_OPTION option, int ofs=0 ){ m_eFillOption = option; m_nDarkOfsForOption = ofs; }
    inline void setTouchForOption( eSTROKE_TOUCH_TARGET touchId ){ m_eTouchTargetIdForOption = touchId; }
    inline void setTouchForOptionForMC( eSTROKE_TOUCH_TARGET touchId ){ m_eTouchTargetIdForOptionForMC = touchId; }
    inline void setMaskTargetForOption( eBD_MASK_TARGET target ){ m_eMaskTargetForOption = target; }
    inline void setStrokeForOption( int size, int range ){ m_nStrokeSizeForOption = size; m_nStrokeRangeForOption = range; }
    inline void setOfsXYForOption( int ofsX, int ofsY ){ m_nOfsXForOption = ofsX; m_nOfsYForOption = ofsY; }

	//------------
	// 取得
	//------------
    int fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC );
    inline eSTROKE_HOOK_TARGET getHookTargetId( void ){ return( m_eHookTargetId ); }
    inline eSTROKE_HOOK_TARGET getHookTargetIdForMC( void ){ return( m_eHookTargetIdForMC ); }

    inline eFILL_OPTION getFillOption( void ){ return( m_eFillOption ); }
    inline int getDarkOfsForOption( void ){ return( m_nDarkOfsForOption ); }
    
    int fixTouchTargetIdForOption( int slotIndex, bool isMC );    
    inline eSTROKE_TOUCH_TARGET getTouchTargetIdForOption( void ){ return( m_eTouchTargetIdForOption ); }
    inline eSTROKE_TOUCH_TARGET getTouchTargetIdForOptionForMC( void ){ return( m_eTouchTargetIdForOptionForMC ); }
    
    BYTE getBitMaskForOption( eBD_SLOT slot, int slotIndex );
    inline eBD_MASK_TARGET getMaskTargetForOption( void ){ return( m_eMaskTargetForOption ); }

    inline int getStrokeSizeForOption( void ){ return( m_nStrokeSizeForOption ); }
    inline int getStrokeRangeForOption( void ){ return( m_nStrokeRangeForOption ); }
    inline int getOfsXForOption( void ){ return( m_nOfsXForOption ); }
    inline int getOfsYForOption( void ){ return( m_nOfsYForOption ); }

    //------------
    // 調整値クリア
    //------------
    inline void clearAdjust( void ){
        m_oAdjXY.clearAdjust();
    }

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
#endif	// __FILL_POINT_DATA_HPP__
