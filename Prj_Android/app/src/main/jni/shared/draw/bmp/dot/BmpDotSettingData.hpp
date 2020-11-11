/*+----------------------------------------------------------------+
  |	Title:		BmpDotSettingData.hpp [共通環境]
  |	Comment:	BMPドット設定データ（※各スロットのBDPDを指定するデータ）
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

#include "draw/bmp/BmpGenParam.hpp"

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
// BMPドット設定データ
//----------------------------------------
class CBmpDotSettingData: public CDataFile{
    // 各種実装
    IMPLEMENT_DATA_EDIT()               // 実装：データ編集

protected:
    //------------------------------
    // 表示情報
    //------------------------------
    bool        m_bFlipH;               // [1] ヨコ反転
    bool        m_bFlipV;               // [1] タテ反転
    int         m_nRateScale;           // [2] スケール
    int         m_nRateRot;             // [2] 回転

    //------------------------------
	// メンバー [保存データ]
    //------------------------------
    // 基本設定
    int         m_nIdForFace;           // [4] 顔
    int         m_nIdForBody;           // [4] 素体
    int         m_nIdForSuit;           // [4] スーツ
    eBD_EMO     m_eEmo;                 // [enum] 感情（※感情による差分の適用指定）
    eBD_FORM    m_eForm;                // [enum] フォーム（※ポーズによる差分の適用指定）

    // 調整値
    int         m_nRateAdjustH;         // [2] 割合調整値：H
    int         m_nRateAdjustV;         // [2] 割合調整値：V
    int         m_nRateAdjustT;         // [2] 割合調整値：T
    int         m_nRateAdjustS;         // [2] 割合調整値：S
    int         m_nRateAngleLR;         // [2] 割合傾き：左右
    int         m_nRateAngleUD;         // [2] 割合傾き：上下
    
    //----------------------------------------------------------
    // [1 x MAX] オプション指定（※編集処理の都合上、bool配列を数値で管理）
    //----------------------------------------------------------
    int         m_nArrOptionValid[eBD_OPTION_MAX];

    //----------------------------------------------------------
    // [X x MAX]スロット枠（※BDPDの参照先の指定）
    //----------------------------------------------------------
    CBmpDotSettingSlotData m_oArrSlot[BMP_DOT_SETTING_SLOT_DATA_MAX];
    
    //-----------------------------------
    // work：局所的な設定
    //-----------------------------------
    bool m_bUseDefaultForSuit;
    
    bool m_bFlipHFixed;         bool m_bFlipHFixedFlag;
    bool m_bFlipVFixed;         bool m_bFlipVFixedFlag;
    bool m_bScaleFixed;         float m_fScaleFixed;
    bool m_bRotFixed;           float m_fRotFixed;

    bool m_bIdForFaceFixed;     int m_nIdForFaceFixed;
    bool m_bIdForBodyFixed;     int m_nIdForBodyFixed;
    bool m_bIdForSuitFixed;     int m_nIdForSuitFixed;
    bool m_bEmoFixed;           eBD_EMO m_eEmoFixed;
    bool m_bFormFixed;          eBD_FORM m_eFormFixed;

    bool m_bAdjustHFixed;       float m_fAdjustHFixed;
    bool m_bAdjustVFixed;       float m_fAdjustVFixed;
    bool m_bAdjustTFixed;       float m_fAdjustTFixed;
    bool m_bAdjustSFixed;       float m_fAdjustSFixed;
    bool m_bAngleLRFixed;       float m_fAngleLRFixed;
    bool m_bAngleUDFixed;       float m_fAngleUDFixed;

    // 置き換え（※[Fixed]とは少し取り扱いが違う）
    int m_nReplaceIdForUp;
    int m_nReplaceIdForLow;

public:
    // コンストラクタ／デストラクタ
    CBmpDotSettingData( void );
    virtual ~CBmpDotSettingData( void );

    // クリア
    void clear( void );
    
    // 確定値のリセット
    void resetFixed( void );
    
    // 置き換え値のリセット
    void resetReplaced( void );

    // コピー
    void copy( CBmpDotSettingData* pData );

    // 設定
    inline void setFlipH( bool flag ){ m_bFlipH = flag; }
    inline void setFlipV( bool flag ){ m_bFlipV = flag; }
    inline void setRateScale( int rate ){ m_nRateScale = rate; }
    inline void setRateRot( int rate ){ m_nRateRot = rate; }

    inline void setIdForFace( int id ){ m_nIdForFace = id; }
    inline void setIdForBody( int id ){ m_nIdForBody = id; }
    inline void setIdForSuit( int id ){ m_nIdForSuit = id; }
    inline void setEmo( eBD_EMO emo ){ m_eEmo = emo; }
    inline void setForm( eBD_FORM form ){ m_eForm = form; }

    inline void setRateAdjustH( int rate ){ m_nRateAdjustH = rate; }
    inline void setRateAdjustV( int rate ){ m_nRateAdjustV = rate; }
    inline void setRateAdjustT( int rate ){ m_nRateAdjustT = rate; }
    inline void setRateAdjustS( int rate ){ m_nRateAdjustS = rate; }
    inline void setRateAngleLR( int rate ){ m_nRateAngleLR = rate; }
    inline void setRateAngleUD( int rate ){ m_nRateAngleUD = rate; }
    
    // 調整
    inline void setUseDefaultForSuit( bool flag ){ m_bUseDefaultForSuit = flag; }
    
    inline void setFlipHFixed( bool flag ){ m_bFlipHFixed = true; m_bFlipHFixedFlag = flag; }
    inline void setFlipVFixed( bool flag ){ m_bFlipVFixed = true; m_bFlipVFixedFlag = flag; }
    inline void setScaleFixed( float scale ){ m_bScaleFixed = true; m_fScaleFixed = scale; }
    inline void setRotFixed( float rot ){ m_bRotFixed = true; m_fRotFixed = rot; }

    inline void setIdForFaceFixed( int id ){ m_bIdForFaceFixed = true; m_nIdForFaceFixed = id; }
    inline void setIdForBodyFixed( int id ){ m_bIdForBodyFixed = true; m_nIdForBodyFixed = id; }
    inline void setIdForSuitFixed( int id ){ m_bIdForSuitFixed = true; m_nIdForSuitFixed = id; }
    inline void setEmoFixed( eBD_EMO emo ){ m_bEmoFixed = true ; m_eEmoFixed = emo; }
    inline void setFormFixed( eBD_FORM form ){ m_bFormFixed = true; m_eFormFixed = form; }

    inline void setAdjustHFixed( float adj ){ m_bAdjustHFixed = true; m_fAdjustHFixed = adj; }
    inline void setAdjustVFixed( float adj ){ m_bAdjustVFixed = true; m_fAdjustVFixed = adj; }
    inline void setAdjustTFixed( float adj ){ m_bAdjustTFixed = true; m_fAdjustTFixed = adj; }
    inline void setAdjustSFixed( float adj ){ m_bAdjustSFixed = true; m_fAdjustSFixed = adj; }
    inline void setAngleLRFixed( float adj ){ m_bAngleLRFixed = true; m_fAngleLRFixed = adj; }
    inline void setAngleUDFixed( float adj ){ m_bAngleUDFixed = true; m_fAngleUDFixed = adj; }

    inline void setReplacedIdForUp( int id ){ m_nReplaceIdForUp = id; }
    inline void setReplacedIdForLow( int id ){ m_nReplaceIdForLow = id; }

    // 取得
    inline bool isFlipH( void ){ return( (m_bFlipHFixed)? m_bFlipHFixedFlag: m_bFlipH ); }
    inline bool isFlipV( void ){ return( (m_bFlipVFixed)? m_bFlipVFixedFlag: m_bFlipV ); }
    inline float getScale( void ){ return( (m_bScaleFixed)? m_fScaleFixed: CConst::ConvBezierScaleRate( m_nRateScale ) ); }
    inline float getRot( void ){ return( (m_bRotFixed)? m_fRotFixed: CConst::ConvBezierScaleRate( m_nRateRot ) ); }

    inline int getIdForFace( void ){ return( (m_bIdForFaceFixed)? m_nIdForFaceFixed: m_nIdForFace ); }
    inline int getIdForBody( void ){ return( (m_bIdForBodyFixed)? m_nIdForBodyFixed: m_nIdForBody ); }
    inline int getIdForSuit( void ){ return( (m_bIdForSuitFixed)? m_nIdForSuitFixed: m_nIdForSuit ); }
    inline eBD_EMO getEmo( void ){ return( (m_bEmoFixed)? m_eEmoFixed: m_eEmo ); }
    inline eBD_FORM getForm( void ){ return( (m_bFormFixed)? m_eFormFixed: m_eForm ); }
    
    inline float getAdjustH( void ){ return( (m_bAdjustHFixed)? m_fAdjustHFixed: CConst::ConvBezierScaleRate( m_nRateAdjustH ) ); }
    inline float getAdjustV( void ){ return( (m_bAdjustVFixed)? m_fAdjustVFixed: CConst::ConvBezierScaleRate( m_nRateAdjustV ) ); }
    inline float getAdjustT( void ){ return( (m_bAdjustTFixed)? m_fAdjustTFixed: CConst::ConvBezierScaleRate( m_nRateAdjustT ) ); }
    inline float getAdjustS( void ){ return( (m_bAdjustSFixed)? m_fAdjustSFixed: CConst::ConvBezierScaleRate( m_nRateAdjustS ) ); }
    inline float getAngleLR( void ){ return( (m_bAngleLRFixed)? m_fAngleLRFixed: CConst::ConvBezierScaleRate( m_nRateAngleLR ) ); }
    inline float getAngleUD( void ){ return( (m_bAngleUDFixed)? m_fAngleUDFixed: CConst::ConvBezierScaleRate( m_nRateAngleUD ) ); }
    
    inline bool isUseDefaultForSuit( void ){ return( m_bUseDefaultForSuit ); }

    inline int getReplacedIdForUp( void ){ return( m_nReplaceIdForUp ); }
    inline int getReplacedIdForLow( void ){ return( m_nReplaceIdForLow ); }

    // オプション関連
    void setOptionValid( eBD_OPTION option, int slotIndex, bool flag );
    bool isOptionValid( eBD_OPTION option, int slotIndex );
    
    void setOptionForSlotValidAt( eBD_SLOT slot, int slotIndex, int at, bool flag );
    bool isOptionForSlotValidAt( eBD_SLOT slot, int slotIndex, int at );

    // スロット関連
    CBmpDotSettingSlotData* getSlotAt( int at );
    CBmpDotSettingSlotData* searchSlot( eBD_SLOT slot, int slotIndex );
    
    // ロード／セーブ（※マネージャがいないので自身で読み書きの窓口をもつ）
    bool loadFile( const char* pName );
    bool saveFile( const char* pName, bool isTemporary );

    // 判定
    bool isSuited( void );
    
    // 設定
    void setDefault( void );

    // ランダム設定
    void setRandomPart( void );
    void setRandomPartForSuit( void );
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
