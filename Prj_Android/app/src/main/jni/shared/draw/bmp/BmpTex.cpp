/*+----------------------------------------------------------------+
  | Title:      BmpTex.cpp [共通環境]
  | Comment:    BMPテクスチャ（※バッファ保持クラス）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpTex.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------------
// コンストラクタ
//-------------------------
CBmpTex::CBmpTex( void ){
    clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpTex::~CBmpTex( void ){
    release();
}

//-------------------------
// クリア
//-------------------------
void CBmpTex::clear( void ){
    m_oTexForLine.clear();
    m_oTexForColor.clear();
    m_oTexForPath.clear();
    
    m_oDotSetting.clear();
    
    m_nBmpGenTime = 0;
    m_nBmpGenTimeForPath = 0;
}

//-------------------------
// 開放
//-------------------------
void CBmpTex::release( void ){
    m_oTexForLine.release();
    m_oTexForColor.release();
    m_oTexForPath.release();
}

//----------------------------------
// レイヤーリストによるBMP作成
//----------------------------------
bool CBmpTex::createBmpWithLayerList( stBMP_GEN_CREATE_PARAM* pCreateParam, CList* pList, bool isPathDump ){
    bool isSuccess = true;

    // 指定があればパスの描画
    if( isPathDump ){
        m_nBmpGenTimeForPath = CBmpGenerator::CreateTexWithLayerList( &m_oTexForPath, NULL, pList, pCreateParam, true );
        if( m_nBmpGenTimeForPath < 0 ){
            isSuccess = false;
        }
    }

    // 線と色の描画
    if( isSuccess ){
        m_nBmpGenTime = CBmpGenerator::CreateTexWithLayerList( &m_oTexForLine, &m_oTexForColor, pList, pCreateParam, false );
        if( m_nBmpGenTime < 0 ){
            isSuccess = false;
        }
    }

    // 失敗していたら後始末
    if( ! isSuccess ){
        m_oTexForPath.release();
        m_oTexForLine.release();
        m_oTexForColor.release();
        
        m_nBmpGenTime = 0;
        m_nBmpGenTimeForPath = 0;
    }

    return( isSuccess );
}

//----------------------------------
// 設定によるBMP作成
//----------------------------------
bool CBmpTex::createBmpWithDotSetting( stBMP_GEN_CREATE_PARAM* pCreateParam,
                                       eBD_SLOT slot, int subId, int slotIndex, bool isPathDump, float workScale, bool isSlotIgnore ){
    bool isSuccess = true;
    
    // 生成パラメータ設定
    CBmpGenParam genParam;
    if( ! CBmpGenerator::SetBmpGenParam( &genParam, &m_oDotSetting ) ){
        isSuccess = false;
    }
    
    // 指定があればパスの描画
    if( isPathDump && isSuccess ){
        m_nBmpGenTimeForPath = CBmpGenerator::CreateTexWithGenParam( &m_oTexForPath, NULL, &genParam,
                                                                     slot, subId, slotIndex, pCreateParam, true, workScale, isSlotIgnore );
        if( m_nBmpGenTimeForPath < 0 ){
            isSuccess = false;
        }
    }

    // 線と色の描画
    if( isSuccess ){
        m_nBmpGenTime = CBmpGenerator::CreateTexWithGenParam( &m_oTexForLine, &m_oTexForColor, &genParam,
                                                              slot, subId, slotIndex, pCreateParam, false, workScale, isSlotIgnore );
        if( m_nBmpGenTime < 0 ){
            isSuccess = false;
        }
    }

    // 失敗していたら後始末
    if( ! isSuccess ){
        m_oTexForPath.release();
        m_oTexForLine.release();
        m_oTexForColor.release();
        
        m_nBmpGenTime = 0;
        m_nBmpGenTimeForPath = 0;
    }

    return( isSuccess );
}
