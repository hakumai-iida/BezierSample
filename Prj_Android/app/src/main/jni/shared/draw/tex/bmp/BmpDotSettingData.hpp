/*+----------------------------------------------------------------+
  |	Title:		BmpDotSettingData.hpp [共通環境]
  |	Comment:	BMP設定データ（※各スロットのBDPDを指定するデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_SETTING_DATA_HPP__
#define __BMP_DOT_SETTING_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotSettingSlotData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataFile.hpp"
#include "loop/node/EditValueMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 保持するスロットデータ枠
#define BMP_DOT_SETTING_SLOT_DATA_MAX    128

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMP設定データ
//----------------------------------------
class CBmpDotSettingData: public CDataFile{
    // 各種実装
    IMPLEMENT_DATA_EDIT()               // 実装：データ編集

protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    // 基本設定
    int         m_nIdForFace;           // 顔
    int         m_nIdForBody;           // 素体
    int         m_nIdForSuit;           // スーツ
    eBD_EMO     m_eEmo;                 // 感情（※感情による差分の適用指定）
    eBD_FORM    m_eForm;                // フォーム（※ポーズによる差分の適用指定）
    
    // 調整値
    int         m_nRateAdjustH;         // 割合調整値：H
    int         m_nRateAdjustV;         // 割合調整値：V
    int         m_nRateAngleLR;         // 割合傾き：左右
    int         m_nRateAngleUD;         // 割合傾き：上下
    
    // 移動値（※指定の値から移動値を決定する）
    int         m_nRateVX;              // 割合速度：X
    int         m_nRateVY;              // 割合速度：Y
    int         m_nRateAX;              // 割合加速度：X
    int         m_nRateAY;              // 割合加速度：Y
    int         m_nMoveCount;           // 移動カウント
    
    // オプション指定
    bool        m_bArrOptionValid[eBD_OPTION_MAX];

    // スロット枠（※BDPDの参照先の指定）
    CBmpDotSettingSlotData m_oArrSlot[BMP_DOT_SETTING_SLOT_DATA_MAX];

public:
    // コンストラクタ／デストラクタ
    CBmpDotSettingData( void );
    virtual ~CBmpDotSettingData( void );

    // クリア
    void clear( void );
    
    // コピー
    void copy( CBmpDotSettingData* pData );

    // 設定
    inline void setIdForFace( int id ){ m_nIdForFace = id; }
    inline void setIdForBody( int id ){ m_nIdForBody = id; }
    inline void setIdForSuit( int id ){ m_nIdForSuit = id; }
    inline void setEmo( eBD_EMO emo ){ m_eEmo = emo; }
    inline void setForm( eBD_FORM form ){ m_eForm = form; }
    
    inline void setRateAdjustH( int rate ){ m_nRateAdjustH = rate; }
    inline void setRateAdjustV( int rate ){ m_nRateAdjustV = rate; }
    inline void setRateAngleLR( int rate ){ m_nRateAngleLR = rate; }
    inline void setRateAngleUD( int rate ){ m_nRateAngleUD = rate; }
    
    inline void setRateVX( int rate ){ m_nRateVX = rate; }
    inline void setRateVY( int rate ){ m_nRateVY = rate; }
    inline void setRateAX( int rate ){ m_nRateAX = rate; }
    inline void setRateAY( int rate ){ m_nRateAY = rate; }
    inline void setMoveCount( int count ){ m_nMoveCount = count; }
    
    // 取得
    inline int getIdForFace( void ){ return( m_nIdForFace ); }
    inline int getIdForBody( void ){ return( m_nIdForBody ); }
    inline int getIdForSuit( void ){ return( m_nIdForSuit ); }
    inline eBD_EMO getEmo( void ){ return( m_eEmo ); }
    inline eBD_FORM getForm( void ){ return( m_eForm ); }
    
    inline int getRateAdjustH( void ){ return( m_nRateAdjustH ); }
    inline int getRateAdjustV( void ){ return( m_nRateAdjustV ); }
    inline int getRateAngleLR( void ){ return( m_nRateAngleLR ); }
    inline int getRateAngleUD( void ){ return( m_nRateAngleUD ); }
    
    inline float getAdjustH( void ){ return( CConst::ConvBezierRateScale( m_nRateAdjustH ) ); }
    inline float getAdjustV( void ){ return( CConst::ConvBezierRateScale( m_nRateAdjustV ) ); }
    inline float getAngleLR( void ){ return( CConst::ConvBezierRateScale( m_nRateAngleLR ) ); }
    inline float getAngleUD( void ){ return( CConst::ConvBezierRateScale( m_nRateAngleUD ) ); }
    
    inline int getRateVX( void ){ return( m_nRateVX ); }
    inline int getRateVY( void ){ return( m_nRateVY ); }
    inline int getRateAX( void ){ return( m_nRateAX ); }
    inline int getRateAY( void ){ return( m_nRateAY ); }
    inline int getMoveCount( void ){ return( m_nMoveCount ); }
    
    inline float getVX( void ){ return( CConst::ConvBezierRateScale( m_nRateVX ) ); }
    inline float getVY( void ){ return( CConst::ConvBezierRateScale( m_nRateVY ) ); }
    inline float getAX( void ){ return( CConst::ConvBezierRateScale( m_nRateAX ) ); }
    inline float getAY( void ){ return( CConst::ConvBezierRateScale( m_nRateAY ) ); }

    // オプション関連
    void setOptionValid( eBD_OPTION option, bool flag );
    bool getOptionValid( eBD_OPTION option );

    // スロット関連
    CBmpDotSettingSlotData* getSlotAt( int at );
    CBmpDotSettingSlotData* searchSlot( eBD_SLOT slot, int slotIndex );
    
    // 判定
    bool isSuited( void );
    
    // 設定
    void setDefault( void );

    // ランダム設定
    void setRandomAdjForHV( void );
    void setRandomAdjForT( void );
    void setRandomAdjForS( void );
    void setRandomAdjForAngle( void );

protected:
    // 読み書き実体（※[CDataFile]から継承）
    virtual void load0( CInputBuffer* pIB );
    virtual void save0( COutputBuffer* pOB );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_DOT_SETTING_DATA_HPP__
