/*+----------------------------------------------------------------+
  |	Title:		BmpDotPack.hpp [共通環境]
  |	Comment:	BMP画素データパック
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_PACK_HPP__
#define __BMP_DOT_PACK_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotData.hpp"

#include "sys/data/DataCell.hpp"
#include "sys/data/DataList.hpp"
#include "sys/data/DataFile.hpp"
#include "loop/node/EditValueMenu.hpp"

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
// BMP画素データパック
//----------------------------------------
class CBmpDotPack: public CDataList, public CDataFile{
    // 実装
    IMPLEMENT_DATA_EDIT()                                       // 実装：データ編集
    IMPLEMENT_DATA_LIST_CONTROL_WITH_ALLOCATOR( CBmpDotData )   // 実装：インスタンス確保／解放込みのリスト操作
    
protected:
	//-----------------------------------------
	// メンバー [保存データ]
    //-----------------------------------------
    //CList m_oDataList;            // [X]:データリスト：CBmpDotData

    eBD_CATEGORY m_eCategory;       // [enum]:カテゴリ：eBD_CATEGORY_XXXX
    int m_nUid;                     // [4]:UID
    eBD_EMO m_eEmo;                 // [enum]:感情：eBD_EMO_XXXX

public:
    // コンストラクタ／デストラクタ
    CBmpDotPack( void );
    virtual ~CBmpDotPack( void );

    // クリア
    void clear( void );
    
    // 設定処理は無し（※値の操作はエディタで行う）

    // 取得
    inline eBD_CATEGORY getCategory( void ){ return( m_eCategory ); }
    inline int getUid( void ){ return( m_nUid ); }
    inline eBD_EMO getEmo( void ){ return( m_eEmo ); }
    
    // 判定
    bool isEqual( eBD_CATEGORY category, int uid, eBD_EMO emo );

    //----------------------------------------------------------------------
    // データ検索（※なければ[NULL]が返る）
    //----------------------------------------------------------------------
    CBmpDotData* searchBmpDotData( stBD_PART_SEARCH_PARAM* pParam );
    
    //----------------------------------------------------------------------
    // パーツ検索（※なければ[NULL]が返る）
    //----------------------------------------------------------------------
    CBmpDotPartData* searchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParam );

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );

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
#endif	// __BMP_DOT_PACK_HPP__
