/*+----------------------------------------------------------------+
  |	Title:		BucketBinData.hpp [共通環境]
  |	Comment:	バケツバイナリデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BUCKET_BIN_DATA_HPP__
#define __BUCKET_BIN_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "draw/bezier/fill/FillConst.hpp"

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
// バケツバイナリデータ
//----------------------------------------
class CBucketBinData{
public:
    //------------------------
    // バイナリサイズ取得
    //------------------------
    inline static int GetBinSize( void ){
        return( 2 );
    }

protected:
    //------------------------
    // メンバー
    //------------------------
    eFILL_TYPE m_eFillType;     // [enum/2]: 塗りつぶしタイプ

public:
    //------------------------
    // コンストラクタ／デストラクタ
    //------------------------
    CBucketBinData(){}
    virtual ~CBucketBinData(){}

    //------------------------
    // 取得
    //------------------------
    inline eFILL_TYPE getFillType( void ){ return( m_eFillType ); }

    //-----------------------------------------------------
    // 読み込み（※[pIB]は有効である前提＆値の整合性等の判断はしない）
    //-----------------------------------------------------
    inline void read( CInputBuffer* pIB ){
        m_eFillType = (eFILL_TYPE)pIB->readInt16();
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BUCKET_BIN_DATA_HPP__
