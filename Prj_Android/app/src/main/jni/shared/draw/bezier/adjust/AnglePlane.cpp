/*+----------------------------------------------------------------+
  |	Title:		AnglePlane.cpp [共通環境]
  | Comment:    傾き面（※ベジェ描画時のワーク）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AnglePlane.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 適用割合（※調整値に対してどのぐらい変化するかの割合）
#define ANGLE_APPLY_RATE_FOR_H          0.07f
#define ANGLE_APPLY_RATE_FOR_V          0.06f
#define ANGLE_APPLY_RATE_FOR_TRANS_H    0.14f
#define ANGLE_APPLY_RATE_FOR_TRANS_V    0.06f

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
CAnglePlane::CAnglePlane( void ){
    clear();
}

//-------------------------
// クリア
//-------------------------
void CAnglePlane::clear( void ){
    // 登録クリア
    m_nNumInfo = 0;
    for( int i=0; i<ANGLE_PLANE_INFO_MAX; i++ ){
        CLEAR_ANGLE_PLANE_INFO( &m_stArrInfo[i] );
    }
    
    // デフォルトクリア
    CLEAR_ANGLE_PLANE_INFO( &m_stDefaultInfo );

    // キャッシュクリア
    m_nCachedSearchKey = -1;
    m_pCachedInfo = NULL;
}

//------------
// 情報の登録
//------------
bool CAnglePlane::registPlaneInfo( int searchKey, float w, float h, float ofsX, float ofsY,
                                   bool isFlipH, bool isFlipV, float rot, float scale ){
    // 登録済みの値の確認（※用心）
    stANGLE_PLANE_INFO* pInfo = getPlaneInfo( searchKey, false );
    if( pInfo != NULL ){
        LOGE( "@ CAnglePlane::registPlaneInfo: ALREADY REGISTED: searchKey=%d, w=%f, h=%f, ofsX=%f, ofsY=%f\n",
              searchKey, w, h, ofsX, ofsY );
        
        return( false );
    }
    
    // 枠が足りない（※足りない場合は増やす）
    if( m_nNumInfo >= ANGLE_PLANE_INFO_MAX ){
        LOGE( "@ CAnglePlane::registPlaneInfo: BUF SHORTAGE: searchKey=%d, w=%f, h=%f, ofsX=%f, ofsY=%f\n",
              searchKey, w, h, ofsX, ofsY );
        return( false );
    }
    
    // ここまできたら登録（※一応クリア）
    pInfo = &m_stArrInfo[m_nNumInfo++];
    CLEAR_ANGLE_PLANE_INFO( pInfo );

    pInfo->searchKey = searchKey;
    
    pInfo->w = w;
    pInfo->h = h;
    pInfo->ofsX = ofsX;
    pInfo->ofsY = ofsY;

    pInfo->isFlipH = isFlipH;
    pInfo->isFlipV = isFlipV;
    pInfo->rot = rot;
    pInfo->scale = scale;

    return( true );
}

//-------------------
// スロットによる登録
//-------------------
bool CAnglePlane::registPlaneInfoForSlot( eBD_SLOT slot, int slotIndex, float w, float h, float ofsX, float ofsY,
                                          bool isFlipH, bool isFlipV, float rot, float scale ){
    int searchKey = CALC_ANGLE_PLANE_INFO_SEARCH_KEY( slot, slotIndex );
    return( registPlaneInfo( searchKey, w, h, ofsX, ofsY, isFlipH, isFlipV, rot, scale ) );
}

//-------------------
// デフォルトの設定
//-------------------
void CAnglePlane::registPlaneDefault( float w, float h, float ofsX, float ofsY,
                                      bool isFlipH, bool isFlipV, float rot, float scale ){
    // 一応クリア
    CLEAR_ANGLE_PLANE_INFO( &m_stDefaultInfo );
    
    // 設定
    m_stDefaultInfo.w = w;
    m_stDefaultInfo.h = h;
    m_stDefaultInfo.ofsX = ofsX;
    m_stDefaultInfo.ofsY = ofsY;
    
    m_stDefaultInfo.isFlipH = isFlipH;
    m_stDefaultInfo.isFlipV = isFlipV;
    m_stDefaultInfo.rot = rot;
    m_stDefaultInfo.scale = scale;
}

//-----------------------------------
// 情報の取得
//-----------------------------------
stANGLE_PLANE_INFO* CAnglePlane::getPlaneInfo( int searchKey, bool isEnableCacheAndDefault ){
    // キャッシュ確認
    if( isEnableCacheAndDefault ){
        if( m_nCachedSearchKey == searchKey ){
            return( m_pCachedInfo );
        }
    }
    
    // データの検索
    for( int i=0; i<m_nNumInfo; i++ ){
        // ヒットしたら
        if( m_stArrInfo[i].searchKey == searchKey ){
            // キャッシュして返す
            m_nCachedSearchKey = searchKey;
            m_pCachedInfo = &m_stArrInfo[i];
            return( m_pCachedInfo );
        }
    }
    
    // 指定のあるミス時にはデフォルト値を返す（※一応キャッシュにも残しておく）
    if( isEnableCacheAndDefault ){
        m_nCachedSearchKey = searchKey;
        m_pCachedInfo = &m_stDefaultInfo;
        return( m_pCachedInfo );
    }
    
    // ここまできたら[NULL]
    return( NULL );
}

//---------------------------------------------------------------------------------------------------
// 座標の傾け（※ベジェ描画用）
//---------------------------------------------------------------------------------------------------
// [pX, pY]: 出力先
// [x0, y0]: 元の座標
// [searchKey]: 領域情報検索キー
// [angleH]: [-1.0f 〜 1.0f]（※負で左側、0.0fで傾きなし、正で右側に傾く）
// [angleV]: [-1.0f 〜 1.0f]（※負で上側、0.0fで傾きなし、正で下側に傾く）
//---------------------------------------------------------------------------------------------------
// 座標の傾きを計算
void CAnglePlane::calcXY( float* pX, float* pY, float x0, float y0, int searchKey, float angleH, float angleV ){
    //--------------------------------------
    // 事前判定（※傾きがなければそのまま返す）
    //--------------------------------------
    if( angleH == 0.0f && angleV == 0.0f ){
        *pX = x0;
        *pY = y0;
        return;
    }

    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    FIX_ADJUST_RATE( angleH );
    FIX_ADJUST_RATE( angleV );
#endif

    //---------------------------------------------------------
    // 登録情報の取得
    //---------------------------------------------------------
    stANGLE_PLANE_INFO* pInfo = getPlaneInfo( searchKey, true );
    float ofsX = pInfo->ofsX;
    float ofsY = pInfo->ofsY;
    
    // オフセット調整（※オフセットの位置が処理原点となる）
    float x = x0 - ofsX;
    float y = y0 - ofsY;

    // 座標に適用されている角度をリセット
    if( pInfo->rot != 0.0f ){
        float rot = - pInfo->rot;    // 相殺したいのでマイナス指定
        float tmpX = x;
        float tmpY = y;
        x = CMath::RotX( tmpX, tmpY, rot );
        y = CMath::RotY( tmpX, tmpY, rot );
    }

    // 座標に適用されている反転をリセット
    if( pInfo->isFlipH ){ x *= -1; }
    if( pInfo->isFlipV ){ y *= -1; }

    //----------------------------------
    // X座標の算出
    //----------------------------------
    if( angleH != 0.0f ){
        float hW = (pInfo->w * pInfo->scale)/2.0f;

        // 平面上のX座標割合
        float rate;
        if( x < 0.0f ){
            rate = ANGLE_APPLY_RATE_FOR_H*x/hW;
        }else{
            rate = -ANGLE_APPLY_RATE_FOR_H*x/hW;
        }
        
        // 移動
        float trans = angleH * hW;
        x += rate * trans;
        
        // 平面がずれるように（※アリスの基準位置がかなりずれるがパーツが独立して存在する感じがするので採用）
        x -= trans*ANGLE_APPLY_RATE_FOR_TRANS_H;
    }
    
    //----------------------------------
    // Y座標の算出
    //----------------------------------
    if( angleV != 0.0f ){
        float hH = (pInfo->h * pInfo->scale)/2.0f;

        // 平面上のY座標割合
        float rate;
        if( y < 0.0f ){
            rate = ANGLE_APPLY_RATE_FOR_V*y/hH;
        }else{
            rate = -ANGLE_APPLY_RATE_FOR_V*y/hH;
        }
        
        // 移動
        float trans = angleV * hH;
        y += rate * trans;
        
        // 平面がずれるように（※アリスの基準位置がかなりずれるがパーツが独立して存在する感じがするので採用）
        y -= trans*ANGLE_APPLY_RATE_FOR_TRANS_V;
    }
    
    // 反転を元に戻す
    if( pInfo->isFlipH ){ x *= -1; }
    if( pInfo->isFlipV ){ y *= -1; }

    // 角度を元に戻す
    if( pInfo->rot != 0.0f ){
        float rot = pInfo->rot;
        float tmpX = x;
        float tmpY = y;
        x = CMath::RotX( tmpX, tmpY, rot );
        y = CMath::RotY( tmpX, tmpY, rot );
    }

    // オフセットを元に戻して出力
    *pX = x + ofsX;
    *pY = y + ofsY;
}
