/*+----------------------------------------------------------------+
  |	Title:		LineObjectData.hpp [共通環境]
  |	Comment:	線データ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LINE_OBJECT_DATA_HPP__
#define __LINE_OBJECT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "AnchorPointData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eLOD_FLAG{
	eLOD_FLAG_INVALID = -1,

    eLOD_FLAG_DISABLE,	                    // 00:[*] 無効（※[CLayerData::draw]で処理されない）
    eLOD_FLAG_CLOSE_PATH,                   // 01:[C] パスを閉じる（※終点から始点へストロークを引く）
    eLOD_FLAG_DOT,                          // 02:[D] ドットを描画する（※１ＡＰにつき１ドットのみ出力）
    eLOD_FLAG_TOUCH,                        // 03:[T] タッチを描画する
    eLOD_FLAG_TOUCH_IS_STRIPE,              // 04:[S] タッチ描画をストライプ扱いする
    eLOD_FLAG_TOUCH_IS_FRILL,               // 05:[F] タッチ描画をフリル扱いする
    eLOD_FLAG_RESET_GUIDE_AFTER_STROKE,     // 06:[rG] ストロークの後にガイドをリセットする
    eLOD_FLAG_FOCUS_OUT_COL_AFTER_STROKE,   // 07:[@] ストロークの後に出力色を持ち越す（※塗られた範囲にのみ処理を限定する場合）
    eLOD_FLAG_RESET_MASK_AFTER_STROKE,      // 08:[rM] ストロークの後にマスクをリセットする
    eLOD_FLAG_MIRROR_COPY,                  // 09:[=] ミラー処理をする（自身のデータを左右反転して再適用する）
    eLOD_FLAG_MIRROR_COPY_HOOK_HEAD,        // 10:[>] ミラーコピー時に先頭のポイントをフックする（※すきまができたら困る場合に適用）
    eLOD_FLAG_MIRROR_COPY_HOOK_TAIL,        // 11:[<] ミラーコピー時に末端のポイントをフックする（※すきまができたら困る場合に適用）
    eLOD_FLAG_TOUCH_DEST_MINUS,             // 12:[-A] 合成対象タッチデータを反転する
    eLOD_FLAG_TOUCH_OPT_MINUS,              // 13:[-B] 合成オプションタッチデータを反転する

    eLOD_FLAG_MAX,		                    // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eLOD_FLAG_TEMP_SELECTED = 16,           // 編集対象として選択されている（※表示切り替え用）
    eLOD_FLAG_TEMP_ACTIVE,                  // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 線データ
//----------------------------------------
class CLineObjectData: public CLayerObject, public CDataList{
    // 各種実装
    IMPLEMENT_TEMP_ADJUST                                           // 実装：一時しのぎ調整
    IMPLEMENT_DATA( CLineObjectData )                               // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eLOD_FLAG )                                // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eLOD_FLAG )                            // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CLineObjectData )              // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CAnchorPointData )  // 実装：インスタンス確保／解放込みのリスト操作

public:
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_eFlag;                                  // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;                            // [X]: データリスト：CAnchorPointData
    
    eBRUSH m_eBrushId;                              // [enum]: ブラシID
    int m_nStrokeSize;                              // [2]: ストローク：基本サイズ:[0〜300%]
    ePAL_OFS m_eTestPalOfsId;                       // [enum]: テストするパレットオフセットID（※指定色の上にのみ線を引く）
    ePAL_OFS m_eTestPalOfsIdForRepair;              // [enum]: ラインの接続時の修復テストをするパレットオフセットID（※指定色の上に再度線を引く指定）
    eSTROKE_GUIDE_TARGET m_eGuideTargetId;          // [enum]: ガイド対象ID（※処理前のガイド復旧対象）
    eSTROKE_GUIDE_TARGET m_eGuideTargetIdForMC;     // [enum]: ミラーコピー用
    
    // タッチ合成：[Dest += Opt]等の処理を行う
    eSTROKE_TOUCH_TARGET m_eTouchTargetDestId;      // [enum]: タッチ合成対象
    eSTROKE_TOUCH_TARGET m_eTouchTargetDestIdForMC; // [enum]: ミラーコピー用

    eSTROKE_TOUCH_TARGET m_eTouchTargetOptId;       // [enum]: タッチ合成オプション
    eSTROKE_TOUCH_TARGET m_eTouchTargetOptIdForMC;  // [enum]: ミラーコピー用

public:
    // コンストラクタ／デストラクタ
    CLineObjectData( void );
    virtual ~CLineObjectData( void );

    //----------------
    // 設定
    //----------------
    inline void setBrushId( eBRUSH brushId ){ m_eBrushId = brushId; }
    inline void setStrokeSize( int size ){ m_nStrokeSize = size; }
    inline void setTestPalOfsId( ePAL_OFS palOfsId ){ m_eTestPalOfsId = palOfsId; }
    inline void setTestPalOfsIdForRepair( ePAL_OFS repairPalOfsId ){ m_eTestPalOfsIdForRepair = repairPalOfsId; }
    inline void setGuideTargetId( eSTROKE_GUIDE_TARGET target ){ m_eGuideTargetId = target; }
    inline void setGuideTargetIdForMC( eSTROKE_GUIDE_TARGET target ){ m_eGuideTargetIdForMC = target; }
    
    inline void setTouchTargetDestId( eSTROKE_TOUCH_TARGET target ){ m_eTouchTargetDestId = target; }
    inline void setTouchTargetDestIdForMC( eSTROKE_TOUCH_TARGET target ){ m_eTouchTargetDestIdForMC = target; }

    inline void setTouchTargetOptId( eSTROKE_TOUCH_TARGET target ){ m_eTouchTargetOptId = target; }
    inline void setTouchTargetOptIdForMC( eSTROKE_TOUCH_TARGET target ){ m_eTouchTargetOptIdForMC = target; }

	//----------------
	// 取得
	//----------------
    inline eBRUSH getBrushId( void ){ return( m_eBrushId ); }
    inline int getStrokeSize( void ){ return( m_nStrokeSize ); }
    inline ePAL_OFS getTestPalOfsId( void ){ return( m_eTestPalOfsId ); }
    inline BYTE getTestPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_eTestPalOfsId ) ); }
    inline ePAL_OFS getTestPalOfsIdForRepair( void ){ return( m_eTestPalOfsIdForRepair ); }
    inline BYTE getTestPalGrpForRepair( void ){ return( CDefTable::GetPalOfsValue( m_eTestPalOfsIdForRepair ) ); }
    
    int fixGuideTargetId( int slotIndex, bool isMC );
    inline eSTROKE_GUIDE_TARGET getGuideTargetId( void ){ return( m_eGuideTargetId ); }
    inline eSTROKE_GUIDE_TARGET getGuideTargetIdForMC( void ){ return( m_eGuideTargetIdForMC ); }

    int fixTouchTargetDestId( int slotIndex, bool isMC );
    inline eSTROKE_TOUCH_TARGET getTouchTargetDestId( void ){ return( m_eTouchTargetDestId ); }
    inline eSTROKE_TOUCH_TARGET getTouchTargetDestIdForMC( void ){ return( m_eTouchTargetDestIdForMC ); }

    int fixTouchTargetOptId( int slotIndex, bool isMC );
    inline eSTROKE_TOUCH_TARGET getTouchTargetOptId( void ){ return( m_eTouchTargetOptId ); }
    inline eSTROKE_TOUCH_TARGET getTouchTargetOptIdForMC( void ){ return( m_eTouchTargetOptIdForMC ); }

    //------------
    // 調整値クリア
    //------------
    void clearAdjust( void );

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );

#if 0
    //----------------------------------------------------------
    // アンカーポイントの追加（※現在選択中の点と次の点の中間）
    //（※現状、利用はしていないが後で使うかもしれないのでコードは残しておく）
    //----------------------------------------------------------
    CAnchorPointData* addDataAtIntermediate( void );
#endif
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LINE_OBJECT_DATA_HPP__
