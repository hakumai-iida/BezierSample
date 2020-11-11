/*+----------------------------------------------------------------+
  |	Title:		PaintObjectData.hpp [共通環境]
  |	Comment:	塗りデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __PAINT_OBJECT_DATA_HPP__
#define __PAINT_OBJECT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "FillPointData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum ePOD_FLAG{
	ePOD_FLAG_INVALID = -1,                 // 無効値

	ePOD_FLAG_DISABLE,	                    // 00:[*] 無効（※[CLayerData::draw]で処理されない）
    ePOD_FLAG_STAY_GUIDE_AFTER_FILL,        // 01:[sG] 塗りの後にガイドを持ち越す（※ガイドをリセットしない）
    ePOD_FLAG_FOCUS_OUT_COL_AFTER_FILL,     // 02:[@] 塗りの後に出力色を持ち越す（※塗られた範囲にのみ処理を限定する場合）
    ePOD_FLAG_RESET_MASK_AFTER_FILL,        // 03:[rM] 塗りの後にマスクをリセットする
    ePOD_FLAG_MIRROR_COPY,                  // 04:[=] ミラー処理をする（自身のデータを左右反転して再適用する）
    ePOD_FLAG_RESET_LAST_IF_MIRROR,         // 05:[rL] ミラー時に直近出力色をクリアする（※オプション時に先行する領域が重複処理されないように）

    ePOD_FLAG_MAX,		                    // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    ePOD_FLAG_TEMP_SELECTED = 16,           // 編集対象として選択されている（※表示切り替え用）
    ePOD_FLAG_TEMP_ACTIVE,                  // 親が編集対象として選択されている（※表示切り替え用）
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 塗りデータ
//----------------------------------------
class CPaintObjectData: public CLayerObject, public CDataList{
    // 各種実装
    IMPLEMENT_TEMP_ADJUST                                           // 実装：一時しのぎ調整
    IMPLEMENT_DATA( CPaintObjectData )                              // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( ePOD_FLAG )                                // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( ePOD_FLAG )                            // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CPaintObjectData )             // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CFillPointData )    // 実装：インスタンス確保／解放込みのリスト操作
    
public:
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_eFlag;                  // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;            // [X]: データリスト：CFillPointData
    
    eBUCKET m_eBucketId;            // [enum]: バケツID
    ePAL_OFS m_ePalOfsId;           // [enum]: パレットオフセットID
    int m_nDarkOfs;                 // [2]: 明暗オフセット（※負で明るく、正で暗く）
    ePAL_OFS m_eTestPalOfsId;       // [enum]: テストするパレットオフセットID（※カラーバッファの指定色を塗り対象とする）

public:
    // コンストラクタ／デストラクタ
    CPaintObjectData( void );
    virtual ~CPaintObjectData( void );
    
    //----------------
    // 取得
    //----------------
    inline void setBucketId( eBUCKET bucketId ){ m_eBucketId = bucketId; }
    inline void setPalOfsId( ePAL_OFS palOfsId, int ofs=0 ){ m_ePalOfsId = palOfsId; m_nDarkOfs=ofs; }
    inline void setTestPalOfsId( ePAL_OFS palOfsId ){ m_eTestPalOfsId = palOfsId; }

	//----------------
	// 取得
	//----------------
    inline eBUCKET getBucketId( void ){ return( m_eBucketId ); }
    inline ePAL_OFS getPalOfsId( void ){ return( m_ePalOfsId ); }
    inline BYTE getPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_ePalOfsId ) ); }
    inline int getDarkOfs( void ){ return( m_nDarkOfs ); }
    inline ePAL_OFS getTestPalOfsId( void ){ return( m_eTestPalOfsId ); }
    inline BYTE getTestPalGrp( void ){ return( CDefTable::GetPalOfsValue( m_eTestPalOfsId ) ); }

    //------------
    // 調整値クリア
    //------------
    void clearAdjust( void );

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
#endif	// __PAINT_OBJECT_DATA_HPP__
