/*+----------------------------------------------------------------+
  |	Title:		BmpDotSettingSlotData.hpp [共通環境]
  |	Comment:	BMP設定のスロットデータ（※[CBmpDotSettingData]の構成要素）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_SETTING_SLOT_DATA_HPP__
#define __BMP_DOT_SETTING_SLOT_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "sys/data/DataCell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMP設定スロットデータ
//----------------------------------------
class CBmpDotSettingSlotData{
    // 各種実装
    IMPLEMENT_DATA( CBmpDotSettingSlotData )        // 実装：データのクリア／コピー／読み書き
    
protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    // スロット指定
    eBD_SLOT m_eSlot;               // [enum]: スロット
    int m_nSlotIndex;               // [enum]: スロットインデックス

    // 詳細指定
    eBD_CATEGORY m_eCategory;       // [enum]: カテゴリ
    int m_nUid;                     // [4]: UID
    eBD_EMO m_eEmo;                 // [enum]: 感情
    eBD_FORM m_eForm;               // [enum]: フォーム
    int m_nSubId;                   // [2]: サブID（※絵的なバリエーションを指定する）
    eBD_DIR m_eDir;                 // [enum]: 向き（※向きを指定する）
    
    // 個別調整
    int m_nRateAdjForT;             // [2]: テンション（傾き）
    int m_nRateAdjForS;             // [2]: サイズ
    
    // 無効指定（※無効データはデフォルトが参照されるのでスロット自体の無効化はフラグで行う）
    bool m_bInvalid;                // [1]: 無効フラグ

public:
    // コンストラクタ／デストラクタ
    CBmpDotSettingSlotData( void );
    virtual ~CBmpDotSettingSlotData( void ){}
    
    // リセット（※設定の一部を消す）
    void reset( void );
    
    //------------
    // 設定
    //------------
    inline void setSlot( eBD_SLOT slot ){ m_eSlot = slot; }
    inline void setSlotIndex( int slotIndex ){ m_nSlotIndex = slotIndex; }

    inline void setCategory( eBD_CATEGORY category ){ m_eCategory = category; }
    inline void setUid( int uid ){ m_nUid = uid; }
    inline void setEmo( eBD_EMO emo ){ m_eEmo = emo; }
    inline void setForm( eBD_FORM form ){ m_eForm = form; }
    inline void setSubId( int subId ){ m_nSubId = subId; }
    inline void setDir( eBD_DIR dir ){ m_eDir = dir; }
    
    inline void setRateAdjForT( int rate ){ m_nRateAdjForT = rate; }
    inline void setRateAdjForS( int rate ){ m_nRateAdjForS = rate; }
    
    inline void setInvalid( bool flag ){ m_bInvalid = flag; }

	//------------
	// 取得
	//------------
    inline eBD_SLOT getSlot( void ){ return( m_eSlot ); }
    inline int getSlotIndex( void ){ return( m_nSlotIndex ); }

    inline eBD_CATEGORY getCategory( void ){ return( m_eCategory ); }
    inline int getUid( void ){ return( m_nUid ); }
    inline eBD_EMO getEmo( void ){ return( m_eEmo ); }
    inline eBD_FORM getForm( void ){ return( m_eForm ); }
    inline int getSubId( void ){ return( m_nSubId ); }
    inline eBD_DIR getDir( void ){ return( m_eDir ); }
    
    inline int getRateAdjForT( void ){ return( m_nRateAdjForT ); }
    inline float getAdjForT( void ){ return( CConst::ConvBezierScaleRate( m_nRateAdjForT ) ); }

    inline int getRateAdjForS( void ){ return( m_nRateAdjForS ); }
    inline float getAdjForS( void ){ return( CConst::ConvBezierScaleRate( m_nRateAdjForS ) ); }
    
    inline bool isInvalid( void ){ return( m_bInvalid ); }

    //------------
    // 判定
    //------------
    bool checkSlot( eBD_SLOT slot, int slotIndex );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_DOT_SETTING_SLOT_DATA_HPP__
