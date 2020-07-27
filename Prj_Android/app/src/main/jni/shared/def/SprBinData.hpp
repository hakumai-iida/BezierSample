/*+----------------------------------------------------------------+
  |	Title:		SprBinData.hpp [共通環境]
  |	Comment:	スプライトバイナリデータ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SPR_BIN_DATA_HPP__
#define __SPR_BIN_DATA_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

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
// スプライトバイナリデータ
//----------------------------------------
class CSprBinData{
public:
    //------------------------
    // バイナリサイズ取得
    //------------------------
    inline static int GetBinSize( void ){
        return( 6 * 2 );
    }

protected:
    //------------------------
    // メンバー
    //------------------------
    eIMG m_eImg;        // [enum/2]: 画像ID
    int16 m_nNo;        // [2]: 画像番号
    int16 m_nU0;        // [2]: U0
    int16 m_nV0;        // [2]: V0
    int16 m_nU1;        // [2]: U1
    int16 m_nV1;        // [2]: V1

public:
    //------------------------
    // コンストラクタ／デストラクタ
    //------------------------
    CSprBinData(){}
    virtual ~CSprBinData(){}

    //------------------------
    // 取得
    //------------------------
    inline eIMG getImg( void ){ return( m_eImg ); }
    inline int getNo( void ){ return( m_nNo ); }
    inline int getU0( void ){ return( m_nU0 ); }
    inline int getV0( void ){ return( m_nV0 ); }
    inline int getU1( void ){ return( m_nU1 ); }
    inline int getV1( void ){ return( m_nV1 ); }
    
    //-----------------------------------------------------
    // 読み込み（※[pIB]は有効である前提＆値の整合性等の判断はしない）
    //-----------------------------------------------------
    inline void read( CInputBuffer* pIB ){
        m_eImg = (eIMG)pIB->readInt16();
        m_nNo = pIB->readInt16();
        m_nU0 = pIB->readInt16();
        m_nV0 = pIB->readInt16();
        m_nU1 = pIB->readInt16();
        m_nV1 = pIB->readInt16();
    }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SPR_BIN_DATA_HPP__
