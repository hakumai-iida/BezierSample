/*+----------------------------------------------------------------+
  |	Title:		BmpTex.hpp [共通環境]
  |	Comment:	BMPテクスチャ（※バッファ保持クラス）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_TEX_HPP__
#define __BMP_TEX_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenerator.hpp"

#include "draw/tex/Tex.hpp"
#include "draw/bmp/dot/BmpDotSettingData.hpp"

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
// BMPテクスチャ
//----------------------------------------
class CBmpTex{
protected:
    // テクスチャ
    CTex m_oTexForLine;         // 線
    CTex m_oTexForColor;        // 塗り
    CTex m_oTexForPath;         // パス
    
    // ドット設定
    CBmpDotSettingData m_oDotSetting;
    
    // ログ
    int m_nBmpGenTime;
    int m_nBmpGenTimeForPath;

public:
    // コンストラクタ／デストラクタ
    CBmpTex( void );
    virtual ~CBmpTex( void );
    
    // クリア／開放
    void clear( void );
    void release( void );

    // 取得
    inline CTex* getTexForLine( void ){ return( &m_oTexForLine ); }
    inline CTex* getTexForColor( void ){ return( &m_oTexForColor ); }
    inline CTex* getTexForPath( void ){ return( &m_oTexForPath ); }
    
    inline CBmpDotSettingData* getDotSetting( void ){ return( &m_oDotSetting ); }

    inline int getBmpGenTime( void ){ return( m_nBmpGenTime ); }
    inline int getBmpGenTimeForPath( void ){ return( m_nBmpGenTimeForPath ); }
    
    //--------------------------
    // BMP生成
    //--------------------------
    // レイヤーリストから作成
    bool createBmpWithLayerList( stBMP_GEN_CREATE_PARAM* pCreateParam, CList* pList,
                                 bool isPathDump=false );
    
    // ドット設定から作成
    bool createBmpWithDotSetting( stBMP_GEN_CREATE_PARAM* pCreateParam,
                                  eBD_SLOT slot=eBD_SLOT_FgTop, int subId=0, int slotIndex=0,
                                  bool isPathDump=false,
                                  float workScale=1.0f, bool isSlotIgnore=false );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_TEX_HPP__
