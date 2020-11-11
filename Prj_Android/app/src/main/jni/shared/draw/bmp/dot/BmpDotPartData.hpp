/*+----------------------------------------------------------------+
  |	Title:		BmpDotPartData.hpp [共通環境]
  |	Comment:	BMPの画素パーツデータ：パーツに対応したデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_PART_DATA_HPP__
#define __BMP_DOT_PART_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "draw/bezier/adjust/AdjustablePoint.hpp"
#include "draw/bezier/adjust/SlotAdjuster.hpp"
#include "draw/bezier/data/LayerData.hpp"

#include "sys/data/DataList.hpp"
#include "sys/data/DataCell.hpp"
#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//---------------------------------------
// フラグID（※デフォルトが[FALSE]になるように）
//---------------------------------------
enum eBDPD_FLAG{
    eBDPD_FLAG_INVALID = -1,            // 無効値

    eBDPD_FLAG_DISABLE,                 // 00:[*] 無効
    eBDPD_FLAG_ROUGH_FLIP,              // 01:[rF] ラフの反転（ヨコのみ）
    eBDPD_FLAG_REF_DRAW,                // 02:[rD] 参照パーツのオブジェクトを描画する
    eBDPD_FLAG_REF_SLOT,                // 03:[rS] 参照パーツのスロットを呼び出す
    eBDPD_FLAG_HIDE_OVER_HAIR,          // 04:[-oH] はみ出る髪の毛を隠す（※[HdBase]に設定される特殊な前髪等）

    eBDPD_FLAG_MAX,			            // 最大（※フラグ変数は[short]なので[00〜15]まで）
    
    // 一時フラグ（※[short]の範囲外を使うことで揮発性フラグが利用可能＝[16〜31]まで）
    eBDPD_FLAG_TEMP_SELECTED = 16,      // 編集対象として選択されている（※表示切り替え用）
    eBDPD_FLAG_TEMP_ACTIVE,             // 親が編集対象として選択されている（※表示切り替え用）
    eBDPD_FLAG_TEMP_EDIT_TARGET,        // 編集対象（※オーダー登録をデフォルトに変更）
};

// 非表示スロット枠
#define BDPD_HIDE_SLOT_NUM      8

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMP画素データ
//----------------------------------------
class CBmpDotPartData: public CDataList, public CListNode{
    // 各種実装
    IMPLEMENT_SLOT_ADJUSTER                                     // 実装：スロット調整
    IMPLEMENT_NODE( CBmpDotPartData )                           // 実装：リストノード
    IMPLEMENT_DATA( CBmpDotPartData )                           // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_FLAG( eBDPD_FLAG )                           // 実装：フラグ操作
    IMPLEMENT_DATA_FLAG_ALL( eBDPD_FLAG )                       // 実装：フラグ操作（一括）
    IMPLEMENT_DATA_EDIT()                                       // 実装：データ編集
    IMPLEMENT_DATA_ALLOCATOR_IN_HPP( CBmpDotPartData )          // 実装：クラスメソッドによるインスタンス確保／解放
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CLayerData )    // 実装：インスタンス確保／解放込みのリスト操作

public:
    // 領域確保情報
    static int GetAllocCellNum( void );
    static int GetAllocSize( void );

protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    //int m_nFlag;                                  // [2]:フラグ（※[IMPLEMENT_DATA_FLAG]で宣言）
    //CSlotAdjuster m_oSlotAdj;                     // [X]:スロット調整（※[IMPLEMENT_SLOT_ADJUSTER]で宣言）
    //CList m_oDataList;                            // [X]:データリスト：CLayerData

    //--------------------------------------------------------------------------------
	// 識別用（※このデータに割り振られる各種ID＝[CBmpDotData]から検索[isEqual]される際に利用する）
    //--------------------------------------------------------------------------------
    eBD_SLOT        m_eSlot;                        // [enum]:スロット：eBD_SLOT_XxYyyyy
    int             m_nSubId;                       // [2]:サブID（※素材的な差分が必要な場合はサブIDで管理する）
    
    // マルチスロットに対して、左と右で違うパーツを割り当てたい場合に右側を作成する（※なければ検索時に左側がわりあてられる）
    eBD_DIR         m_eDir;                         // [enum]:素材の向き：eBD_DIR_X（※左がデフォルトとなる）
    
    //--------------------------------------------------------------------------------
    // フック調整用（※スロットを指定することでその座標へ配置する）
    // 汎用位置として呼び出されたパーツが、自身が想定するスロットを指定することで、対象座標へフックされる
    //--------------------------------------------------------------------------------
    eBD_SLOT        m_eHookSlot;                    // [enum]:割り当て先
    int             m_nHookSlotIndex;               // [2]: インデックス（※[0=左、1=右]等、[-1]で親の値を引き継ぐ）

    //-----------------------------------------------------------------------------------------
    // 参照用（※別のパーツを流用する際の指定／無効値を指定することで参照元と同じ値でデータを検索する＝subIdのみ０）
    // データ上は[スロット]も指定できるが、参照元と参照先のスロットが一致しない場合に、傾き面の親判定の兼ね合いから
    // 予期しない見た目になる可能性があるので注意（※現状の処理だと、参照元の傾き平面で参照先が傾けられてしまう）
    //-----------------------------------------------------------------------------------------
    eBD_CATEGORY    m_eRefCategory;                 // [enum]:参照カテゴリ：eBD_CATEGORY_XXXX
    int             m_nRefUid;                      // [4]:参照UID
    eBD_EMO         m_eRefEmo;                      // [enum]:参照感情：eBD_EMO_XXXX
    eBD_FORM        m_eRefForm;                     // [enum]:参照フォーム：eBD_FORM_XXXX
    eBD_SLOT        m_eRefSlot;                     // [2]:参照スロット
    int             m_nRefSubId;                    // [2]:参照サブID
    eBD_DIR         m_eRefDir;                      // [enum]参照方向：eBD_DIR_X
    
    int             m_nRefRateScale;                // [2]:参照スケール
    int             m_nRefRateRot;                  // [2]:参照回転

    //--------------------------------------------------------------------------------
    // 下絵（※作業用に保存しておく＝本番では不要となるデータなので配信形式で出力する際は無視できる）
    //--------------------------------------------------------------------------------
    eIMG m_eRoughImgId;                             // [enum]:画像ID
    int  m_nRoughRateScale;                         // [4] 表示スケール（※３００％以上にしたいので[int]扱い）
    int  m_nRoughRateOfsX, m_nRoughRateOfsY;        // [2x2]:表示オフセット
    BYTE m_nRoughAlpha;                             // [1]:アルファ
    BYTE m_nRoughBgCol;                             // [1]:背景色
    BYTE m_nRoughGridCol;                           // [1]:グリッド色（※グレイスケールの明度）

    //--------------------------------------------------------------------------------
    // 制限（※カバーパーツ等により完全に隠れる対象が想定される場合はスロットを指定して無効化する）
    //--------------------------------------------------------------------------------
    eBD_SLOT m_eArrHideSlot[BDPD_HIDE_SLOT_NUM];            // [2*X]:非表示スロット（同方向）
    eBD_SLOT m_eArrHideSlotOposite[BDPD_HIDE_SLOT_NUM];     // [2*X]:非表示スロット（逆方向）

public:
    // コンストラクタ／デストラクタ
    CBmpDotPartData( void );
    virtual ~CBmpDotPartData( void );
    
    //--------------------------------------------------------------
    // コード上で設定することはないので設定処理は無し（※値の操作はエディタで行う）
    //--------------------------------------------------------------
    // 設定（※デフォルトパーツはコードからいじるのでスロットだけ指定できるようにしておく）
    inline void setSlot( eBD_SLOT slot ){ m_eSlot = slot; }

	// 取得
    inline eBD_SLOT getSlot( void ){ return( m_eSlot ); }
    inline int getSubId( void ){ return( m_nSubId ); }
    inline eBD_DIR getDir( void ){ return( m_eDir ); }

    inline eBD_SLOT getHookSlot( void ){ return( m_eHookSlot ); }
    inline int getHookSlotIndex( void ){ return( m_nHookSlotIndex ); }

    inline eBD_CATEGORY getRefCategory( void ){ return( m_eRefCategory ); }
    inline int getRefUid( void ){ return( m_nRefUid ); }
    inline eBD_EMO getRefEmo( void ){ return( m_eRefEmo ); }
    inline eBD_FORM  getRefForm( void ){ return( m_eRefForm ); }
    inline eBD_SLOT getRefSlot( void ){ return( m_eRefSlot ); }
    inline int getRefSubId( void ){ return( m_nRefSubId ); }
    inline eBD_DIR getRefDir( void ){ return( m_eRefDir ); }

    inline int  getRefRateScale( void ){ return( m_nRefRateScale ); }
    inline float getRefScale( void ){ return( CConst::ConvBezierScaleRate( m_nRefRateScale ) ); }

    inline int  getRefRateRot( void ){ return( m_nRefRateRot ); }
    inline float getRefRot( void ){ return( CConst::ConvBezierRotationRate( m_nRefRateRot ) ); }
    
    // 下絵関連
    inline eIMG getRoughImgId( void ){ return( m_eRoughImgId ); }
    inline int getRoughRateScale( void ){ return( m_nRoughRateScale ); }
    inline int getRoughRateOfsX( void ){ return( m_nRoughRateOfsX ); }
    inline int getRoughRateOfsY( void ){ return( m_nRoughRateOfsY ); }
    inline BYTE getRoughAlpha( void ){ return( m_nRoughAlpha ); }
    inline BYTE getRoughBgCol( void ){ return( m_nRoughBgCol ); }
    inline BYTE getRoughGridCol( void ){ return( m_nRoughGridCol ); }

    // 非表示スロット
    inline eBD_SLOT* getArrHideSlot( void ){ return( m_eArrHideSlot ); }
    inline eBD_SLOT* getArrHideSlotOposite( void ){ return( m_eArrHideSlotOposite ); }

	// 識別名取得
	const char* getNameWithTempBuf( void );

	// 判定
	bool isEqual( eBD_SLOT slot, int subId, eBD_DIR dir );

    // レイヤーデータの取得（※カバー処理時にオーダータイプ別に取り出す際に利用）
    CLayerData* getLayerDataWithOrderTypeAt( eBD_LAYER_ORDER_TYPE orderType, int at );

    //-----------------------------
    // オブジェクトの一時調整値が有効か？
    //-----------------------------
    bool checkTempAdjustForNodes( void );

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
#endif	// __BMP_DOT_PART_DATA_HPP__
