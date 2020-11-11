/*+----------------------------------------------------------------+
  |	Title:		BmpPalData.hpp [共通環境]
  |	Comment:	BMPパレットデータ（※BezierConstに対応した形式の色管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_PAL_DATA_HPP__
#define __BMP_PAL_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BPConst.hpp"

#include "draw/bezier/BezierConst.hpp"

#include "sys/data/DataFile.hpp"
#include "sys/rand/Rand.hpp"
#include "loop/node/EditPalMenu.hpp"

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
// BMP設定データ
//----------------------------------------
class CBmpPalData: public CDataFile{
protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    // 管理情報
    eBP_CATEGORY m_eCategory;   // [enum]:カテゴリ：eBP_CATEGORY_XXXX
    int m_nUid;                 // [4]:UID
    int m_nSubId;               // [2]:サブID

    // 実データ
    BYTE m_nArrArrBrightRGBA[BEZIER_PAL_GRP_NUM][4];    // [4*BEZIER_PAL_GRP_NUM]:明色
    BYTE m_nArrArrBaseRGBA[BEZIER_PAL_GRP_NUM][4];      // [4*BEZIER_PAL_GRP_NUM]:基本色
    BYTE m_nArrArrDarkRGBA[BEZIER_PAL_GRP_NUM][4];      // [4*BEZIER_PAL_GRP_NUM]:暗色
    
    //-----------------------------------------
    // ワーク
    //-----------------------------------------
    bool m_bExecAsBmp;          // BMPとして読み書きするか？（※窓口を増やさず内部の分岐をさせるため）

public:
    // コンストラクタ／デストラクタ
    CBmpPalData( void );
    virtual ~CBmpPalData( void );
    
    // 設定
    inline void setCategory( eBP_CATEGORY category ){ m_eCategory = category; }
    inline void setUid( int uid ){ m_nUid = uid; }
    inline void setSubId( int subId ){ m_nSubId = subId; }

    // 取得
    inline eBP_CATEGORY getCategory( void ){ return( m_eCategory ); }
    inline int getUid( void ){ return( m_nUid ); }
    inline int getSubId( void ){ return( m_nSubId ); }
    
    // 読み書き調整
    inline void setExecAsBmp( bool flag ){ m_bExecAsBmp = flag; }
    
    // クリア
    void clear( void );
    
    // コピー
    void copy( CBmpPalData* pData );
    
    // 判定
    bool isEqual( eBP_CATEGORY category, int uid, int subId );
    
    // ID調整（※編集窓を経由しないで直接いじる）
    int addId( int val );
    int addSubId( int val );
            
    // 調整色々
    void copyBaseToMinMax( void );                              // 基本色をMin色／Max色へコピー
    void adjustBase( bool isDark, float rate, int minStep );    // 基本色を明るく／暗く
    void adjustRandForBase( int randRange );                    // 基本色に対してランダム
    void adjustOneStepForMinMax( float rate, int minStep );     // Min色／Max色を明るく／暗く

    // 編集メニューへの割り当て
    void setEditPalMenu( CEditPalMenu* pMenu );
    
    // パレットの読み込み
    bool loadToTex( CTex* pTex );
    bool loadToImgPal( eIMG target );

    // パレットの合成
    void mixAverage( CBmpPalData** pArrPal, int* arrWeight, int num );
    void mixSelect( CBmpPalData** pArrPal, int* arrWeight, int num, eRAND_ID randId=eRAND_ID_COMMON );

protected:
    // 読み書き実体（※[CDataFile]から継承）
    virtual void load0( CInputBuffer* pIB );
    virtual void save0( COutputBuffer* pOB );
    
    // BMP読み書き実体（※[m_bExecAsBmp]を立てると[load0/save0]が横取りされる）
    void loadAsBmp( CInputBuffer* pIB );
    void saveAsBmp( COutputBuffer* pOB );
    
    // パレット調整
    void fix( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_PAL_DATA_HPP__
