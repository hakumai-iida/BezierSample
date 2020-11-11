/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoop.hpp [共通環境]
  |	Comment:	ベジェテストループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_TEST_LOOP_HPP__
#define __BEZIER_TEST_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "loop/Loop.hpp"
#include "loop/node/Button.hpp"
#include "loop/node/PullDownMenu.hpp"

#include "draw/bmp/BmpTex.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// スクショを有効にするか？
//#define ENABLE_SS

//-----------------------
// 機能項目
//-----------------------
enum eBEZIER_TEST_MENU_ITEM{
    // データ選択
    eBTMI_SELECT_CHARA,         // キャラクタ選択
    eBTMI_SELECT_FACE,          // 顔選択
    eBTMI_SELECT_UP,            // 上半身選択
    eBTMI_SELECT_LOW,           // 下半身洗濯
    eBTMI_SELECT_COSTUME,       // 着ぐるみ選択
    eBTMI_SELECT_STROKE,        // ストローク選択
    eBTMI_SELECT_COLOR,         // 色選択

    // ベジェの機能
    eBTMI_ENABLE_TOUCH,         // タッチを有効にするか？
    eBTMI_ENABLE_STRIPE,        // ストライプを有効にするか？
    eBTMI_ENABLE_FRILL,         // フリルを有効にするか？
    eBTMI_ENABLE_FILL_EDGE,     // エッジ塗りを有効にするか？
    eBTMI_ENABLE_FILL_OPTION,   // 塗りオプションを有効にするか？
    
    // 表示
    eBTMI_DISP_WORK_PATH,       // ワークパスを表示するか？
    eBTMI_DISP_BG,              // 背景を表示するか？
    eBTMI_DISP_FLIP_H,          // 横反転するか？
    eBTMI_DISP_FLIP_V,          // 縦反転するか？
    eBTMI_DISP_ON_GROUND,       // 接地表示するか？
    
    eBTMI_MAX,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ベジェテストループ
//----------------------------------------
class CBezierTestLoop: public CLoop{
protected:
    // ボタン：基本制御
	CButton* m_pButtonExit;
	CButton* m_pButtonReset;
    CButton* m_pButtonRand;

    // ボタン：アニメ制御
    CButton* m_pButtonAnim;
    CButton* m_pButtonAnimH;
    CButton* m_pButtonAnimV;
    CButton* m_pButtonAnimT;
    CButton* m_pButtonAnimS;
    CButton* m_pButtonAnimLR;
    CButton* m_pButtonAnimUD;
    CButton* m_pButtonAnimPull;
    CButton* m_pButtonAnimShot;
    CButton* m_pButtonAnimStop;

    // ボタン：調整値増減
    CButton* m_pButtonDecAdjustH;
    CButton* m_pButtonIncAdjustH;
    
    CButton* m_pButtonDecAdjustV;
    CButton* m_pButtonIncAdjustV;

    CButton* m_pButtonDecAdjustT;
    CButton* m_pButtonIncAdjustT;

    CButton* m_pButtonDecAdjustS;
    CButton* m_pButtonIncAdjustS;

    CButton* m_pButtonDecAngleLR;
    CButton* m_pButtonIncAngleLR;

    CButton* m_pButtonDecAngleUD;
    CButton* m_pButtonIncAngleUD;

    CButton* m_pButtonDecRotation;
    CButton* m_pButtonIncRotation;
    
    CButton* m_pButtonDecScale;
    CButton* m_pButtonIncScale;

    // 機能メニュー
    CPullDownMenu*  m_pMenu;
    
    //-------------------------
    // ワーク
    //-------------------------
    float m_fDispRotation;
    float m_fDispScale;

    // 調整値
    float m_fAdjustRateH, m_fAdjustRateVH;
    float m_fAdjustRateV, m_fAdjustRateVV;
    float m_fAdjustRateT, m_fAdjustRateVT;
    float m_fAdjustRateS, m_fAdjustRateVS;
    float m_fAdjustRateLR, m_fAdjustRateVLR;
    float m_fAdjustRateUD, m_fAdjustRateVUD;

    // 遅延ログ
    CDelayLog* m_pDelayLog;

    // アニメ管理
    bool m_bAnimOn;

    bool m_bAnimH;
    bool m_bAnimV;
    bool m_bAnimT;
    bool m_bAnimS;
    bool m_bAnimLR;
    bool m_bAnimUD;

    bool m_bAnimPull;
    bool m_bAnimPullRevX;
    bool m_bAnimPullRevY;
    float m_fAnimPullCount;

    // 設定
    int m_nArrMenuVal[eBTMI_MAX];

    //-------------------------
    // BMPテクスチャ
    //-------------------------
    CBmpTex* m_pBmpTex;

#ifdef ENABLE_SS
    bool m_bDumpSS;             // 画像更新時にスクショをダンプするか？
    int  m_nDumpSkipCount;      // アニメダンプの間引き用
#endif

public:
    // コンストラクタ／デストラクタ
	CBezierTestLoop( void );
	virtual ~CBezierTestLoop( void );

protected:
    // 初期化／終了
	virtual void init0( void );
	virtual void exit0( void );
    
    // 更新
	virtual void update0( void );

	// 表示関連
	virtual void onDraw0( void );

private:
    // 更新
    bool updateAnim( void );
    
    // ボタン押下
    void onReset( void );
    void onAnim( void );
    void onAnimPull( void );
    void onAnimShot( void );
    
    // ボタン確定
    void fixForAnimButton( void );
        
    // セッティング確定
    void fixForSetting( bool isReset, bool isRandom );
    
    // メニュー確定
    void fixForMenu( void );

    // BMP作成
    void createBmp( void );
    
    //------------------------------------------------
    // テストバッファの確保／開放
    //------------------------------------------------
    void allocForTest( void );
    void releaseForTest( void );

    //=================================================
    // 以下は[BezierTestLoopForBase.cpp]で実装
    //=================================================
    CLayerData* m_pLayerBase;
    void allocForLayerForBase( void );

    //=================================================
    // 以下は[BezierTestLoopForTouch.cpp]で実装
    //=================================================
    CLayerData* m_pLayerTouch;
    void allocForLayerForTouch( void );

    //=================================================
    // 以下は[BezierTestLoopForDelay.cpp]で実装
    //=================================================
    CLayerData* m_pLayerDelay;
    void allocForLayerForDelay( void );

    //=================================================
    // 以下は[BezierTestLoopForHook.cpp]で実装
    //=================================================
    CLayerData* m_pLayerHookA;
    CLayerData* m_pLayerHookB;
    CLayerData* m_pLayerHookC;
    void allocForLayerForHookA( void );
    void allocForLayerForHookB( void );
    void allocForLayerForHookC( void );
    
    //=================================================
    // 以下は[BezierTestLoopForLineRepair.cpp]で実装
    //=================================================
    CLayerData* m_pLayerLineRepair;
    void allocForLayerForLineRepair( void );

    //=================================================
    // 以下は[CBezier::GetLayerDataForFrill]を参照
    //=================================================
    CLayerData* m_pLayerRefForFrill;
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_TEST_LOOP_HPP__ */
