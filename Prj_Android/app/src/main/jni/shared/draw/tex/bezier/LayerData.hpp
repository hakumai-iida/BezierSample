/*+----------------------------------------------------------------+
  |	Title:		LayerData.hpp [共通環境]
  |	Comment:	レイヤーデータ（※描画における処理単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LAYER_DATA_HPP__
#define __LAYER_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "LayerOrderCell.hpp"

#include "sys/data/ListNode.hpp"
#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//----------------------------------------
enum eLAYER_FLAG{
	eLAYER_FLAG_INVALID = -1,           // 無効値

	eLAYER_FLAG_DISABLE,	            // 00:[*] 無効
    eLAYER_FLAG_STAY_HOOK_TEMP,         // 01:[sH] 一時フックをクリアしない（※レイヤー処理後に一時フックをクリアしない）
    eLAYER_FLAG_STAY_TOUCH_TEMP,        // 02:[sT] 一時タッチをクリアしない（※レイヤー処理後に一時タッチをクリアしない）
    eLAYER_FLAG_STAY_GUIDE_TEMP,        // 03:[sG] 一時ガイドをクリアしない（※レイヤー処理後に一時ガイドをクリアしない）
    eLAYER_FLAG_INVALID_FOR_SUITED,     // 04:[-] スーツ（着ぐるみ）時に無効化する
    eLAYER_FLAG_VALID_FOR_SUITED,       // 05:[+] スーツ（着ぐるみ）時に有効化する
    eLAYER_FLAG_OPEN_COVER,             // 06:[oC] カバーを開始する（※このレイヤーの描画登録直前に対応カバーBDPDのレイヤーを登録する）
    eLAYER_FLAG_CLOSE_COVER,            // 07:[cC] カバーを終了する（※このレイヤーの描画登録直後に対応カバーBDPDのレイヤーを登録する）

    eLAYER_FLAG_MAX,		            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eLAYER_FLAG_TEMP_SELECTED = 16,     // 編集対象として選択されている（※表示切り替え用）
    eLAYER_FLAG_TEMP_ACTIVE,            // 親が編集対象として選択されている（※表示切り替え用）
};

// 名前サイズ
#define LAYER_NAME_LEN      32

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// レイヤーデータ
//----------------------------------------
class CLayerData: public CDataList, public CListNode{
    // 各種実装
    IMPLEMENT_NODE( CLayerData )                    // 実装：リストノード
    IMPLEMENT_DATA( CLayerData )                    // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eLAYER_FLAG )              // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eLAYER_FLAG )          // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                           // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CLayerData )   // 実装：クラスメソッドによるインスタンス確保／解放
       
    //--------------------------------------------------------------------
    // 汎用実装は利用不可
    //（※[addData/deleteData]で派生先のアロケータに処理を分岐させないといけないため）
    //--------------------------------------------------------------------
    //IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CLayerObject )    // 実装：インスタンス確保／解放込みのリスト操作
    
public:
    // 領域確保情報（※端末の解像度によって確保数とサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                              // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;                        // [X]: データリスト：CLayerObject（※[CLineObjectData/CPaintObjectData/CSlotObjectData]）
    
    char m_cArrName[LAYER_NAME_LEN];            // [32]: 名前（※実体で持つ）
    eLAYER_ORDER_TYPE m_eOrderType;             // [enum]: オーダータイプ
    eBD_SLOT m_eOrderSlot;                      // [enum]: オーダー対象（※スロット)
    
    // オプション（※描画設定に指定がある場合に有効になる）
    eBD_OPTION m_eOption;                       // [enum]: オプション
    
    // 回転適用率（※０にすることで常に基本角度になる＝接地要素）
    int m_nRotPowRate;
    
    // 遅延調整：動きに遅れて着いてくるイメージのオフセット値（※慣性的な調整用）
    eDELAY_LOG m_eDelayType;    // [enum]: 遅延タイプ（※遅延配列への参照先）
    int m_nDelayDepth;          // [2]: 遅延ログ震度
    int m_nDelayPowerRateX, m_nDelayPowerRateY;                 // [2x2]: 移動の強さ
    int m_nDelayPowerRateXForRot, m_nDelayPowerRateYForRot;     // [2x2]: 回転の強さ
    int m_nDelayPowerRateXForScale, m_nDelayPowerRateYForScale; // [2x2]: 拡縮の強さ

public:
    // コンストラクタ／デストラクタ
    CLayerData( void );
    virtual ~CLayerData( void );

    //----------------
    // 設定
    //----------------
    inline void setName( const char* pName ){ util_str_cpy( m_cArrName, pName, LAYER_NAME_LEN ); }
    inline void setOrder( eLAYER_ORDER_TYPE type, eBD_SLOT target=eBD_SLOT_DEFAULT ){ m_eOrderType = type; m_eOrderSlot = target; }

    inline void setOption( eBD_OPTION option ){ m_eOption = option; }
    
    inline void setRotPowRate( float rate ){ m_nRotPowRate = rate; }

    inline void setDelayType( eDELAY_LOG type, int depth=0 ){ m_eDelayType = type; m_nDelayDepth = depth; }
    inline void setDelayPowerRateForXY( int rateX, int rateY ){ m_nDelayPowerRateX = rateX; m_nDelayPowerRateY = rateY; }
    inline void setDelayPowerRateForRot( int rateX, int rateY ){ m_nDelayPowerRateXForRot = rateX; m_nDelayPowerRateYForRot = rateY; }
    inline void setDelayPowerRateForScale( int rateX, int rateY ){ m_nDelayPowerRateXForScale = rateX; m_nDelayPowerRateYForScale = rateY; }

	//----------------
	// 取得
	//----------------
    inline const char* getName( void ){ return( m_cArrName ); }
    inline eLAYER_ORDER_TYPE getOrderType( void ){ return( m_eOrderType ); }
    inline eBD_SLOT getOrderSlot( void ){ return( m_eOrderSlot ); }

    inline eBD_OPTION getOption( void ){ return( m_eOption ); }

    inline int  getRotPowRate( void ){ return( m_nRotPowRate ); }
    inline float getRotPow( void ){ return( CConst::ConvBezierScaleRate( m_nRotPowRate ) ); }

    inline eDELAY_LOG getDelayType( void ){ return( m_eDelayType ); }
    inline int getDelayDepth( void ){ return( m_nDelayDepth ); }
    inline int getDelayPowerRateX( void ){ return( m_nDelayPowerRateX ); }
    inline int getDelayPowerRateY( void ){ return( m_nDelayPowerRateY ); }
    inline int getDelayPowerRateXForRot( void ){ return( m_nDelayPowerRateXForRot ); }
    inline int getDelayPowerRateYForRot( void ){ return( m_nDelayPowerRateYForRot ); }
    inline int getDelayPowerRateXForScale( void ){ return( m_nDelayPowerRateXForScale ); }
    inline int getDelayPowerRateYForScale( void ){ return( m_nDelayPowerRateYForScale ); }

    //--------------------------------------------------------------------------------------
    // リスト操作（※アロケータの分岐があるので[IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR]は使えない）
    //（※リスト系の操作は元処理で大丈夫だが確保／解放が絡む処理はオブジェクトの種別を判定した処理が必要）
    //--------------------------------------------------------------------------------------
    inline CLayerObject* getData( int select ){ return( (CLayerObject*) getData0( select ) ); }
    inline CLayerObject* getSelectedData( void ){ return( (CLayerObject*) getSelectedData0() ); }
    inline CLayerObject* getDataHead( void ){ return( (CLayerObject*) m_oDataList.getHead() ); }
    inline CLayerObject* addData( CLayerObject* pNewData ){ return( (CLayerObject*)addData0( pNewData ) ); }
    CLayerObject* addData( void );
    CLayerObject* addLineObjectData( void );
    CLayerObject* addPaintObjectData( void );
    CLayerObject* addSlotObjectData( void );
    inline CLayerObject* moveUpData( void ){ return( (CLayerObject*) moveUpData0() ); }
    inline CLayerObject* moveDownData( void ){ return( (CLayerObject*) moveDownData0() ); }
    void deleteData( void );
    CLayerObject* copyData( void );
    CLayerObject* pasteData( CLayerObject* pObjCopy );

    //-----------------------------
    // 描画
    //-----------------------------
    int draw( stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath );
    
    //-----------------------------
    // スケール適用
    //-----------------------------
    void applyRateScale( int rateScale );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LAYER_DATA_HPP__
