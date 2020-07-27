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

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// スクショを有効かするか？
//#define ENABLE_SS

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
    CButton* m_pButtonSetRand;
    CButton* m_pButtonSetRead;
    CButton* m_pButtonBg;
    CButton* m_pButtonLine;
    CButton* m_pButtonColor;
    CButton* m_pButtonPath;
    CButton* m_pButtonFlipH;
    CButton* m_pButtonFlipV;
    CButton* m_pButtonGround;

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

    //-------------------------
    // ワーク
    //-------------------------
#ifdef ENABLE_SS
    bool m_bDumpSS;         // 画像更新時にスクショをダンプするか？
    int  m_nDumpSkipCount;  // アニメダンプの間引き用
#endif
    
    // 表示タイプ
    bool m_bDispBg;         // 背景を表示するか？
    int m_nLineType;        // 線の表示タイプ
    int m_nColorType;       // 色の表示タイプ
    bool m_bDispPath;       // ワークパスを表示するか？
    bool m_bFlipH;          // 横反転するか？
    bool m_bFlipV;          // 縦反転するか？
    bool m_bGround;         // 設置するか？
    
    // 調整値
    float m_fAdjustRateH, m_fAdjustRateVH;
    float m_fAdjustRateV, m_fAdjustRateVV;
    float m_fAdjustRateT, m_fAdjustRateVT;
    float m_fAdjustRateS, m_fAdjustRateVS;
    float m_fAdjustRateLR, m_fAdjustRateVLR;
    float m_fAdjustRateUD, m_fAdjustRateVUD;

    float m_fDispRotation;
    float m_fDispScale;

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
    
    //----------------------
    // セッティング
    //----------------------
    CBmpDotSettingData m_oSetting;

    //----------------------
    // テクスチャ
    //----------------------
    CTex* m_pTexForLine;                // 線
    CTex* m_pTexForColor;               // 塗り
    CTex* m_pTexForPath;                // パス

    //----------------------
    // テストレイヤーデータ
    //----------------------
    CLayerData* m_pLayerBase;           // 基本
    
    CLayerData* m_pLayerFrill;          // フリル

    CLayerData* m_pLayerDelay;          // 遅延

    CLayerData* m_pLayerHookA;          // フック（Ａ）
    CLayerData* m_pLayerHookB;          // フック（Ｂ）
    CLayerData* m_pLayerHookC;          // フック（Ｃ）
    
    CLayerData* m_pLayerLineRepair;     // 線修復

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
	virtual void drawUI0( void ){}
	virtual void drawLog0( void ){}
    virtual void registForDraw0( void ){}

private:
    // 確保／開放
    void allocForTest( void );
    void releaseForTest( void );

    // 更新
    bool updateAnim( void );
    
    // ボタン押下
    void onReset( void );
    void onAnim( void );
    void onAnimPull( void );
    void onAnimShot( void );
    
    // ブラシ強制
    void fixBrush( int type );
    
    // ボタン更新
    void fixButtonAnim( void );

    // BMP作成
    void createBmp( void );
    void createBmpWithLayerData( stBMP_GEN_CREATE_PARAM* pCreateParam );
    void createBmpWithBmpDot( stBMP_GEN_CREATE_PARAM* pCreateParam );

    //=================================================
    // 以下は[BezierTestLoopForBase.cpp]で実装
    //=================================================
    void allocForLayerForBase( void );

    //=================================================
    // 以下は[BezierTestLoopForDelay.cpp]で実装
    //=================================================
    void allocForLayerForDelay( void );

    //=================================================
    // 以下は[BezierTestLoopForHook.cpp]で実装
    //=================================================
    void allocForLayerForHookA( void );
    void allocForLayerForHookB( void );
    void allocForLayerForHookC( void );
    
    //=================================================
    // 以下は[BezierTestLoopForLineRepair.cpp]で実装
    //=================================================
    void allocForLayerForLineRepair( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __BEZIER_TEST_LOOP_HPP__ */
