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
#include "draw/bmp/dot/BDConst.hpp"
#include "draw/bezier/adjust/AdjustablePoint.hpp"

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
    eLAYER_FLAG_PRE_DRAW,               // 01:[>] 先行描画を行う（※フック座標の登録等、他のレイヤーの処理前に行いたいことをする場合）
    eLAYER_FLAG_INVALID_IF_COVERED,     // 02:[-] カバー時に無効化する
    eLAYER_FLAG_VALID_IF_COVERED,       // 03:[+] カバー時に有効化する
    eLAYER_FLAG_STAY_HOOK_TEMP,         // 04:[sH] 一時フックをクリアしない（※レイヤー処理後に一時フックをクリアしない）
    eLAYER_FLAG_STAY_TOUCH_TEMP,        // 05:[sT] 一時タッチをクリアしない（※レイヤー処理後に一時タッチをクリアしない）
    eLAYER_FLAG_STAY_GUIDE_TEMP,        // 06:[sG] 一時ガイドをクリアしない（※レイヤー処理後に一時ガイドをクリアしない）
    eLAYER_FLAG_TOO_LARGE,              // 07:[tL] 描画要素が大きいので作業にいくつかの制限をする（※HOOKオフセットを無効化する等）
    eLAYER_FLAG_MASK_COVER,             // 08:[mC] カバーBDPDの[MASK=NORMAL]レイヤーを呼び出す（※呼び出し順は最優先）
    eLAYER_FLAG_FRONT_COVER,            // 09:[fC] カバーBDPDの[FRONT]レイヤーを自身の直前に配置する
    eLAYER_FLAG_BACK_COVER,             // 10:[bC] カバーBDPDの[BACK]レイヤーを自身の直後に配置する
    eLAYER_FLAG_OVER_HAIR,              // 11:[_] はみ出る髪か？（※描画時に指定がある場合に非表示になる）

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
    IMPLEMENT_TEMP_ADJUST                           // 実装：一時しのぎ調整
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
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    //int m_nFlag;                              // [2]: フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CList m_oDataList;                        // [X]: データリスト：CLayerObject（※[CLineObjectData/CPaintObjectData/CSlotObjectData]）
    
    char m_cArrName[LAYER_NAME_LEN];            // [32]: 名前（※実体で持つ）
    eBD_LAYER_ORDER_TYPE m_eOrderType;          // [enum]: オーダータイプ（※表示枠は[前／基本／後ろ]の３つに分かれる）
    eBD_SLOT m_eOrderSlot;                      // [enum]: オーダー対象（※スロットインデックスはレイヤーの属するパーツ設定により決定される)
    eBD_SLOT m_eOrderSlotForCovered;            // [enum]: カバー時オーダー対象（※[DEFAULT]の場合[m_eOrderSlot]を参照する)
    eBD_LAYER_ORDER_GROUP m_eOrderGroup;        // [enum]: オーダーグループ（※スーツ時のグループ分け用）
    eBD_MASK_TARGET m_eMaskTarget;              // [enum]: マスク対象（※スーツ時のカバー処理用）
    
    // オプション（※この設定が有効な場合、BMP作成パラメータに指定がある場合にレイヤーが有効になる）
    eBD_OPTION m_eOption;                       // [enum]: オプション

    // フック（※この設定が有効な場合、レイヤーの原点がフック座標に置き換わる）
    eSTROKE_HOOK_TARGET m_eHookTargetId;        // [enum]: フック対象
    eSTROKE_HOOK_TARGET m_eHookTargetIdForMC;   // [enum]: ミラーコピー用
    
    int m_nHookWorkRateOfsX;                    // [2]: フックワークオフセットX（※フック先が未登録の場合に参照する値）
    int m_nHookWorkRateOfsY;                    // [2]: フックワークオフセットX（※フック先が未登録の場合に参照する値）

    // 回転適用率（※０にすることで常に基本角度になる＝接地要素）
    int m_nRotPowRate;                          // [2]
    
    // テンション関連：レイヤー傾き範囲
    int m_nMinRateForT;                         // [2]: テンション調整：最小オフセット（※角度を下げる指定）
    int m_nMaxRateForT;                         // [2]: テンション調整：最大オフセット（※角度をあげる指定）
    
    // 遅延調整：動きに遅れて着いてくるイメージのオフセット値（※慣性的な調整用）
    eDELAY_LOG m_eDelayType;                    // [enum]: 遅延タイプ（※遅延配列への参照先）
    int m_nDelayDepth;                          // [2]: 遅延ログ震度
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
    inline void setOrder( eBD_LAYER_ORDER_TYPE type, eBD_SLOT target=eBD_SLOT_DEFAULT ){ m_eOrderType = type; m_eOrderSlot = target; }
    inline void setOrderForCovered( eBD_SLOT target ){ m_eOrderSlotForCovered = target; }
    inline void setOrderGroup( eBD_LAYER_ORDER_GROUP group ){ m_eOrderGroup = group; }
    inline void setMaskTarget( eBD_MASK_TARGET target ){ m_eMaskTarget = target; }

    inline void setOption( eBD_OPTION option ){ m_eOption = option; }
    inline void setHookTargetId( eSTROKE_HOOK_TARGET target ){ m_eHookTargetId = target; }
    inline void setHookTargetIdForMC( eSTROKE_HOOK_TARGET target ){ m_eHookTargetIdForMC = target; }
    
    inline void setHookWorkRateOfsXY( int rX, int rY ){ m_nHookWorkRateOfsX = rX; m_nHookWorkRateOfsY = rY; }
    
    inline void setRotPowRate( int rate ){ m_nRotPowRate = rate; }

    inline void setMinRateForT( int rate ){ m_nMinRateForT = rate; }
    inline void setMaxRateForT( int rate ){ m_nMaxRateForT = rate; }

    inline void setDelayType( eDELAY_LOG type, int depth=0 ){ m_eDelayType = type; m_nDelayDepth = depth; }
    inline void setDelayPowerRateForXY( int rateX, int rateY ){ m_nDelayPowerRateX = rateX; m_nDelayPowerRateY = rateY; }
    inline void setDelayPowerRateForRot( int rateX, int rateY ){ m_nDelayPowerRateXForRot = rateX; m_nDelayPowerRateYForRot = rateY; }
    inline void setDelayPowerRateForScale( int rateX, int rateY ){ m_nDelayPowerRateXForScale = rateX; m_nDelayPowerRateYForScale = rateY; }

	//----------------
	// 取得
	//----------------
    inline const char* getName( void ){ return( m_cArrName ); }
    inline eBD_LAYER_ORDER_TYPE getOrderType( void ){ return( m_eOrderType ); }
    inline eBD_SLOT getOrderSlot( void ){ return( m_eOrderSlot ); }
    inline eBD_SLOT getOrderSlotForCovered( void ){ return( m_eOrderSlotForCovered ); }
    inline eBD_LAYER_ORDER_GROUP getOrderGroup( void ){ return( m_eOrderGroup ); }
    
    BYTE getBitMask( eBD_SLOT slot, int slotIndex );
    inline eBD_MASK_TARGET getMaskTarget( void ){ return( m_eMaskTarget ); }    

    inline eBD_OPTION getOption( void ){ return( m_eOption ); }

    int fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC );
    inline eSTROKE_HOOK_TARGET getHookTargetId( void ){ return( m_eHookTargetId ); }
    inline eSTROKE_HOOK_TARGET getHookTargetIdForMC( void ){ return( m_eHookTargetIdForMC ); }
    
    inline int getHookWorkRateOfsX( void ){ return( m_nHookWorkRateOfsX ); }
    inline int getHookWorkRateOfsY( void ){ return( m_nHookWorkRateOfsY ); }

    inline int  getRotPowRate( void ){ return( m_nRotPowRate ); }
    inline float getRotPow( void ){ return( CConst::ConvBezierScaleRate( m_nRotPowRate ) ); }
    
    inline int getMinRateForT( void ){ return( m_nMinRateForT ); }
    inline int getMaxRateForT( void ){ return( m_nMaxRateForT ); }

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
    // 調整値クリア
    //-----------------------------
    void clearAdjust( void );

    //-----------------------------
    // オブジェクトの一時調整値が有効か？
    //-----------------------------
    bool checkTempAdjustForNodes( void );

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
