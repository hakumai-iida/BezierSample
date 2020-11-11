/*+----------------------------------------------------------------+
  |	Title:		AnglePlane.hpp [共通環境]
  |	Comment:	傾き面（※ベジェ描画時のワーク）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ANGLE_PLANE_HPP__
#define __ANGLE_PLANE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 全てのスロットを受け付けられる程度の枠（※足らなければ増やす）
#define ANGLE_PLANE_INFO_MAX     256

// 検索キーの算出（※[slot]、[slotIndex]の値を一つのキーにする）
//（※[Bezier]側には[slot]と[slotIndex]の情報が来ないので平面特定用に検索キーを用いる）
#define CALC_ANGLE_PLANE_INFO_SEARCH_KEY( _slot, _slotIndex )    (100*((_slot)+1)+((_slotIndex)+1))

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//---------------------------------------------
// 傾き調整に必要な情報（※スロット毎に設定される値）
//---------------------------------------------
struct stANGLE_PLANE_INFO{
    // 平面情報
    int searchKey;      // 検索キー（※スロットとスロットインデックスから算出した値／０以下で無効）
    float w, h;         // 領域サイズ
    float ofsX, ofsY;   // オフセット（※傾き調整時の原点）
    
    // 対象レイヤー情報（※座標を拡縮等のない素の値へ戻して計算したいのでレイヤーに適用された情報を保持しておく）
    bool isFlipH;       // 反転（ヨコ）
    bool isFlipV;       // 反転（タテ）
    float rot;          // 回転
    float scale;        // スケール
};

// クリア
#define CLEAR_ANGLE_PLANE_INFO( _pCell )                    \
    memset( (_pCell), 0, sizeof(stANGLE_PLANE_INFO) );      \
    (_pCell)->scale = 1.0f

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 傾き面
//----------------------------------------
class CAnglePlane{
protected:
    // 登録データ（※[eBD_SLOT]とインデックスから算出したキーで個々の平面を登録する）
    int m_nNumInfo;
    stANGLE_PLANE_INFO m_stArrInfo[ANGLE_PLANE_INFO_MAX];
    
    // デフォルトデータ（※未登録のスロットに対して適用する値）
    stANGLE_PLANE_INFO m_stDefaultInfo;

    // キャッシュ（※するほどのこともないが気分的に作っておく）
    //（※平面情報はパーツ単位でしか変わらないので１回のベジェ処理呼び出しではキャッシュがヒットしつづけることになる）
    int m_nCachedSearchKey;
    stANGLE_PLANE_INFO* m_pCachedInfo;

public:
    // コンストラクタ／デストラクタ
    CAnglePlane( void );
    virtual ~CAnglePlane( void ){}
    
    // クリア
    void clear( void );

    //-----------------------------------------------------------------------------
    // 情報の登録（※[CBmpGenerator]のレイヤー解釈時に登録される想定）
    //（※平面はスロット毎に登録される＆インデックスでも分けておく＝別BDPDになる場合もあるため）
    //-----------------------------------------------------------------------------
    // 登録
    bool registPlaneInfo( int searchKey, float w, float h, float ofsX, float ofsY,
                          bool isFlipH, bool isFlipV, float rot, float scale );

    // スロット＆スロット値から登録
    bool registPlaneInfoForSlot( eBD_SLOT slot, int slotIndex, float w, float h, float ofsX, float ofsY,
                                 bool isFlipH, bool isFlipV, float rot, float scale );

    // デフォルト情報の登録
    void registPlaneDefault( float w, float h, float ofsX, float ofsY,
                             bool isFlipH, bool isFlipV, float rot, float scale );

    //-----------------------------------------------------------------------------
    // 座標の傾きを計算（※[CBezier]の描画時に利用される想定）
    //-----------------------------------------------------------------------------
    void calcXY( float* pX, float* pY, float x0, float y0, int searchKey, float angleH, float angleV );

protected:
    // 登録情報の取得
    stANGLE_PLANE_INFO* getPlaneInfo( int searchKey, bool isEnableCacheAndDefault=true );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __ANGLE_PLANE_HPP__
