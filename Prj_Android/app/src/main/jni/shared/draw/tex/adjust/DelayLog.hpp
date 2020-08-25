/*+----------------------------------------------------------------+
  |	Title:		DelayLog.hpp [共通環境]
  |	Comment:	遅延ログ（※遅延調整で利用する移動差分ワーク：実体はバネ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DELAY_LOG_HPP__
#define __DELAY_LOG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "obj/Spring.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// ログ種別（※実体はバネ＝移動による形状の変化の特性毎にバネを用意する）
//-----------------------------------------------------------------
// TODO:ゲーム内容に合わせて再構成
enum eDELAY_LOG{
    eDELAY_LOG_INVALID = -1,    // 無効値

    eDELAY_LOG_CLOTH,           // 布
    eDELAY_LOG_FRILL,           // フリル
    eDELAY_LOG_HAIR,            // 髪
    eDELAY_LOG_SKIN,            // 肌
    eDELAY_LOG_BUST,            // 胸（※肌よりも柔らかい＆高反発）

    eDELAY_LOG_MAX,
};

#define IS_DELAY_LOG_VALID( _log )  ((_log)>=0 && (_log)<eDELAY_LOG_MAX)

// ログの深さ
#define DELAY_LOG_DEPTH         64

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 遅延ログ
//----------------------------------------
class CDelayLog{
protected:
    // バネ
    CSpring m_oArrSpring[eDELAY_LOG_MAX];
    
    // ログ座標
    int m_nArrArrLogX[eDELAY_LOG_MAX][DELAY_LOG_DEPTH];
    int m_nArrArrLogY[eDELAY_LOG_MAX][DELAY_LOG_DEPTH];

public:
    // コンストラクタ／デストラクタ
    CDelayLog( void );
    virtual ~CDelayLog( void ){}
    
    // クリア
    void clear( void );
    
    // リセット
    void reset( void );

    // 更新
    void updateWithFrame( float deltaFrame );
    
    // 指定方向に揺れる
    void shake( eDELAY_LOG type, float v, float rot );

    // 指定速度で引っ張る
    void pull( eDELAY_LOG type, float vX, float vY );
    
    // 遅延座標の取得（※現在のバネの位置）
    int getDelayRateX( eDELAY_LOG type, int depth=0 );
    int getDelayRateY( eDELAY_LOG type, int depth=0 );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelDelayLog[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __DELAY_LOG_HPP__
