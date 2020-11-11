/*+----------------------------------------------------------------+
  |	Title:		BezierTestLoop.cpp [共通環境]
  |	Comment:	ベジェテストループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "BezierTestLoop.hpp"
#include "draw/bezier/struct/BezierStructConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define TEX_DRAW_SCALE      1.0f
#define BMP_GEN_BASE_SCALE  1.0f

//--------------------------------------------------------------------------
// [CLayerData]による表示確認
//--------------------------------------------------------------------------
// この定義を有効にするとテストレイヤーデータによる描画がおこなわれる
//--------------------------------------------------------------------------
//#define LAYER_BASE

// 基本確認が定義されたら詳細の指定
#ifdef LAYER_BASE
    //----------------------------------------------------------------------
    // タッチ確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[allocForLayerForTouch]により素材を作成する
    //----------------------------------------------------------------------
    #define LAYER_BASE_FOR_TOUCH

    //----------------------------------------------------------------------
    // 遅延確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[allocForLayerForDelay]により素材を作成する
    //----------------------------------------------------------------------
    //#define LAYER_BASE_FOR_DELAY

    //----------------------------------------------------------------------
    // フック確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[allocForLayerForHookA/B/C]により素材を作成する
    //----------------------------------------------------------------------
    //#define LAYER_BASE_FOR_HOOK

    // 登録順の反転確認（※アンカーの並びによる意図せぬ表示不具合の確認）
    //#define LAYER_BASE_FOR_HOOK_CHECK_REVERSE

    //----------------------------------------------------------------------
    // 線修復確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[allocForLayerForLineRepair]により素材を作成する
    //----------------------------------------------------------------------
    //#define LAYER_BASE_FOR_LINE_REPAIR

    //----------------------------------------------------------------------
    // フリル形状確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[CBezier::GetLayerDataForFrill]から素材を参照する
    //----------------------------------------------------------------------
    //#define LAYER_BASE_FOR_FRILL

    // 参照先
    //#define LAYER_BASE_FOR_FRILL_TARGET     eSTROKE_FRILL_TEST_CIRCLE
    //#define LAYER_BASE_FOR_FRILL_TARGET     eSTROKE_FRILL_TEST_TRIANGLE
    //#define LAYER_BASE_FOR_FRILL_TARGET     eSTROKE_FRILL_TEST_SQUARE
    //#define LAYER_BASE_FOR_FRILL_TARGET     eSTROKE_FRILL_FLUFFY_SQUARE_MAIN
    //#define LAYER_BASE_FOR_FRILL_TARGET     eSTROKE_FRILL_FLUFFY_SQUARE_SUB
#endif

//------------------------------------------------------------------
// 調整値ボタン登録
//------------------------------------------------------------------
// 増加
#define REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( _pButton, _label )    \
    (_pButton) = new CButton( eGRP_DEBUG_BUTTON_L, (_label) );      \
    putNodeOnLayout( (_pButton), 0, -2 );                           \
    (_pButton)->setFlipX( true );

// 減少
#define REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( _pButton, _label )    \
    (_pButton) = new CButton( eGRP_DEBUG_BUTTON_L, (_label) );      \
    putNodeOnLayout( (_pButton), 0, -2 );

//--------------------------------------------------------------------------
// 調整値ボタン押下判定
//--------------------------------------------------------------------------
#define CHECK_ADJUST_BUTTON( _pButtonInc, _pButtonDec, _value, _value0 )    \
    float _value0 = _value;                                                 \
    if( (_pButtonInc)->isTapped() ){                                        \
        (_value) += 0.1f;                                                   \
        if( (_value) > 1.0f ){                                              \
            (_value) = 1.0f;                                                \
        }                                                                   \
    }else if( (_pButtonDec)->isTapped() ){                                  \
        (_value) -= 0.1f;                                                   \
        if( (_value) < -1.0f ){                                             \
            (_value) = -1.0f;                                               \
        }                                                                   \
    }                                                                       \
    if( (_value) != (_value0) ){                                            \
        isAdjChanged = true;                                                \
    }

//-----------------------------------------------------------
// 調整値更新
//-----------------------------------------------------------
#define UPDATE_ADJUST_VALUE( _value, _valueV )              \
    (_value) += (_valueV);                                  \
    if( (_value) > 0.0f ){                                  \
        if( (_value) > 1.0f ){                              \
            (_value) = 1.0f;                                \
            (_valueV) *= -1.0f;                             \
        }                                                   \
    }else{                                                  \
        if( (_value) < -1.0f ){                             \
            (_value) = -1.0f;                               \
            (_valueV) *= -1.0f;                             \
        }                                                   \
    }

//-----------------------------------------------------------
// 調整値速度減衰
//-----------------------------------------------------------
#define STOP_ADJUST_V( _v )                                 \
    if( (_v) >= 0.01f || (_v) <= -0.01f  ){                 \
        (_v) *= 0.8f;                                       \
    }else{                                                  \
        (_v) = 0.0f;                                        \
    }

//-----------------------------------------------------------
// 調整値ラベルの描画
//-----------------------------------------------------------
#define DRAW_ADJUST_LABEL_FROM_BOTTOM( _label, _value )     \
    sprintf( buf, (_label), (_value) );                     \
    pDC->clear();                                           \
    pDC->setStr( eFONT_DEBUG, buf );                        \
    pDC->setScaleXY( 1.0f, 2.0f );                          \
    pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_B );    \
    pDC->draw( ofsX, ofsY );                                \
    ofsY -= 34

// 情報位置（右上）
#define TIME_LOG_X     (0)
#define TIME_LOG_Y     (4+32+4+32+4+32+4-6)
#define TIME_LOG_S     (1.0f)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//---------------------------
// 項目データ
//---------------------------
// 選択値の最大／初期値
static int _menu_item_info[][2] = {
    { 6, 0 },   // eBTMI_SELECT_CHARA,              // キャラクタ選択
    { 7, 0 },   // eBTMI_SELECT_FACE,               // 顔選択
    { 7, 0 },   // eBTMI_SELECT_UP,                 // 上半身選択
    { 7, 0 },   // eBTMI_SELECT_LOW,                // 下半身選択
    { 4, 0 },   // eBTMI_SELECT_COSTUME,            // 着ぐるみ選択
    { 6, 2 },   // eBTMI_SELECT_STROKE,             // ストローク選択
    { 14, 1 },   // eBTMI_SELECT_COLOR,             // 色選択
    { 2, 1 },   // eBTMI_ENABLE_TOUCH,              // タッチを有効にするか？
    { 2, 1 },   // eBTMI_ENABLE_STRIPE,             // ストライプを有効にするか？
    { 2, 1 },   // eBTMI_ENABLE_FRILL,              // フリルを有効にするか？
    { 2, 1 },   // eBTMI_ENABLE_FILL_EDGE,          // エッジ塗りを有効にするか？
    { 2, 1 },   // eBTMI_ENABLE_FILL_OPTION,        // 塗りオプションを有効にするか？
    { 3, 0 },   // eBTMI_DISP_WORK_PATH,            // ワークパスを表示するか？
    { 2, 1 },   // eBTMI_DISP_BG,                   // 背景を表示するか？
    { 2, 0 },   // eBTMI_DISP_FLIP_H,               // 横反転するか？
    { 2, 0 },   // eBTMI_DISP_FLIP_V,               // 縦反転するか？
#ifdef LAYER_BASE
    { 2, 0 },   // eBTMI_DISP_ON_GROUND,            // 接地表示するか？
#else
    { 2, 1 },   // eBTMI_DISP_ON_GROUND,            // 接地表示するか？
#endif
};

// 参照する[bdp/bpd]のUID
static const int _menu_item_UID_CHARA[] = { 1004, 1003, 1002, 1001, 100, 101 };
static const int _menu_item_UID_COSTUME[] = { 102, 101, 100 };

// メニュー項目名
static const char* _menu_item_label[] = {
    "chara",        // eBTMI_SELECT_CHARA,          // キャラクタ選択
    "face",         // eBTMI_SELECT_FACE,           // 顔選択
    "up",           // eBTMI_SELECT_UP,             // 上半身選択
    "low",          // eBTMI_SELECT_LOW,            // 下半身選択
    "costume",      // eBTMI_SELECT_COSTUME,        // 着ぐるみ選択
    "stroke",       // eBTMI_SELECT_STROKE,         // ストローク選択
    "color",        // eBTMI_SELECT_COLOR,          // 色選択
    "touch",        // eBTMI_ENABLE_TOUCH,          // タッチを有効にするか？
    "stripe",       // eBTMI_ENABLE_STRIPE,         // ストライプを有効にするか？
    "frill",        // eBTMI_ENABLE_FRILL,          // フリルを有効にするか？
    "edge",         // eBTMI_ENABLE_FILL_EDGE,      // エッジ塗りを有効にするか？
    "fill option",  // eBTMI_ENABLE_FILL_OPTION,    // 塗りオプションを有効にするか？
    "work path",    // eBTMI_DISP_WORK_PATH,        // ワークパスを表示するか？
    "bg",           // eBTMI_DISP_BG,               // 背景を表示するか？
    "flip h",       // eBTMI_DISP_FLIP_H,           // 横反転するか？
    "flip v",       // eBTMI_DISP_FLIP_V,           // 縦反転するか？
    "on ground",    // eBTMI_DISP_ON_GROUND,        // 接地表示するか？

    NULL,
};

// メニュー項目の各種表示
static const char* _menu_item_SELECT_CHARA[] = {
    "TYPE04",
    "TYPE03",
    "TYPE02",
    "TYPE01",
    "GOTH",
    "MIKU"
};

static const char* _menu_item_SELECT_PART[] = {
    "---",
    "TYPE04",
    "TYPE03",
    "TYPE02",
    "TYPE01",
    "GOTH",
    "MIKU"
};

static const char* _menu_item_SELECT_COSTUME[] = {
    "---",
    "SHEEP",
    "NEGI",
    "BOX"
};

static const char* _menu_item_SELECT_STROKE[] = {
    "OFF",
    "TEST 1",
    "TEST 2",
    "TEST 3",
    "TEST 4",
    "TEST 5",
};

static const char* _menu_item_SELECT_COLOR[] = {
    "OFF",
    "TYPE04_0", "TYPE04_1",
    "TYPE03_0", "TYPE03_1",
    "TYPE02_0", "TYPE02_1",
    "TYPE01_0", "TYPE01_1",
    "GOTH_0", "GOTH_1",
    "MIKU_0", "MIKU_1", "MIKU_2"
};

static const char* _menu_item_ENABLE_TOUCH[] = { "OFF", "ON" };
static const char* _menu_item_ENABLE_STRIPE[] = { "OFF", "ON" };
static const char* _menu_item_ENABLE_FRILL[] = { "OFF", "ON" };
static const char* _menu_item_ENABLE_FILL_EDGE[] = { "OFF", "ON" };
static const char* _menu_item_ENABLE_FILL_OPTION[] = { "OFF", "ON" };
static const char* _menu_item_DISP_WORK_PATH[] = { "OFF", "STROKE", "FULL" };
static const char* _menu_item_DISP_BG[] = { "OFF", "ON" };
static const char* _menu_item_DISP_FLIP_H[] = { "OFF", "ON" };
static const char* _menu_item_DISP_FLIP_V[] = { "OFF", "ON" };
static const char* _menu_item_DISP_ON_GROUND[] = { "OFF", "ON" };

static const char** _menu_item[]= {
    _menu_item_SELECT_CHARA,        // キャラクタ選択
    _menu_item_SELECT_PART,         // 顔選択
    _menu_item_SELECT_PART,         // 上半身選択
    _menu_item_SELECT_PART,         // 下半身選択
    _menu_item_SELECT_COSTUME,      // 着ぐるみ選択
    _menu_item_SELECT_STROKE,       // ストローク選択
    _menu_item_SELECT_COLOR,        // 色選択
    _menu_item_ENABLE_TOUCH,        // タッチを有効にするか？
    _menu_item_ENABLE_STRIPE,       // ストライプを有効にするか？
    _menu_item_ENABLE_FRILL,        // フリルを有効にするか？
    _menu_item_ENABLE_FILL_EDGE,    // エッジ塗りを有効にするか？
    _menu_item_ENABLE_FILL_OPTION,  // 塗りオプションを有効にするか？
    _menu_item_DISP_WORK_PATH,      // ワークパスを表示するか？
    _menu_item_DISP_BG,             // 背景を表示するか？
    _menu_item_DISP_FLIP_H,         // 横反転するか？
    _menu_item_DISP_FLIP_V,         // 縦反転するか？
    _menu_item_DISP_ON_GROUND,      // 接地表示するか？
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CBezierTestLoop::CBezierTestLoop( void ){
	setId( eLOOP_ID_BEZIER_TEST );

	setDrawLayer( eDP_LAYER_LOOP );
    
    // ベジェ環境リセット
    CBezier::ResetEnv();
    
#ifdef LAYER_BASE
    // レイヤーオブジェクト描画時は方向線が見たい
    CBezier::SetDispDirVectorAlways( true );
#else
    // BMP描画時は方向線は見たくない
    CBezier::SetDispOffDirVectorAlways( true );
#endif
    
    // 全てのアンカーポイントを目立たせる
    CBezier::SetDispAllSelected( true );
    CBezier::SetDispOnlyAnchorPoint( true );

    // 領域確保
    allocForTest();
}

//------------------------
// デストラクタ
//------------------------
CBezierTestLoop::~CBezierTestLoop( void ){
    // 領域開放
    releaseForTest();
}

//------------------------
// 初期化
//------------------------
void CBezierTestLoop::init0( void ){
	//-----------------------------------------
	// node
	//-----------------------------------------
    // レイアウト：右上
	setLayoutRT( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

	// 終了
	m_pButtonExit = new CButton( eGRP_DEBUG_BUTTON, "EXIT" );
 	putNodeOnLayout( m_pButtonExit );

    // リセット
    m_pButtonReset = new CButton( eGRP_DEBUG_BUTTON, "RESET" );
    putNodeOnLayout( m_pButtonReset );

    // ランダム
    m_pButtonRand = new CButton( eGRP_DEBUG_BUTTON, "RANDOM" );
    putNodeOnLayout( m_pButtonRand );

    // レイアウト：左上
    setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );
    
    // アニメ
    m_pButtonAnim = new CButton( eGRP_DEBUG_BUTTON, "ANIM" );
    putNodeOnLayout( m_pButtonAnim );

    // 各種アニメ指定
    m_pButtonAnimH = new CButton( eGRP_DEBUG_BUTTON_M, "H" );
    putNodeOnLayout( m_pButtonAnimH );

    m_pButtonAnimT = new CButton( eGRP_DEBUG_BUTTON_M, "T" );
    putNodeOnLayout( m_pButtonAnimT );

    m_pButtonAnimLR = new CButton( eGRP_DEBUG_BUTTON_M, "LR" );
    putNodeOnLayout( m_pButtonAnimLR );

    m_pButtonAnimShot = new CButton( eGRP_DEBUG_BUTTON_M, "SWY" );
    putNodeOnLayout( m_pButtonAnimShot );

    m_pButtonAnimStop = new CButton( eGRP_DEBUG_BUTTON, "OFF" );
    putNodeOnLayout( m_pButtonAnimStop );

    // レイアウト：左上
    setLayoutLT( 32, 32+4+32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

    m_pButtonAnimV = new CButton( eGRP_DEBUG_BUTTON_M, "V" );
    putNodeOnLayout( m_pButtonAnimV );

    m_pButtonAnimS = new CButton( eGRP_DEBUG_BUTTON_M, "S" );
    putNodeOnLayout( m_pButtonAnimS );

    m_pButtonAnimUD = new CButton( eGRP_DEBUG_BUTTON_M, "UD" );
    putNodeOnLayout( m_pButtonAnimUD );

    m_pButtonAnimPull = new CButton( eGRP_DEBUG_BUTTON_M, "MOV" );
    putNodeOnLayout( m_pButtonAnimPull );

    // レイアウト：右下
    setLayoutRB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );
    
    // 調整値増加
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAngleUD, "UD" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAngleLR, "LR" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustS, "S" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustT, "T" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustV, "V" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustH, "H" );

    // レイアウト：右下
    setLayoutRB( -88, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );

    // 調整値減少
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAngleUD, "UD" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAngleLR, "LR" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustS, "S" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustT, "T" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustV, "V" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustH, "H" );

    // レイアウト：左下
    setLayoutLB( 88, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );
    
    // 表示情報増加
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncRotation, "R" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncScale, "S" );

    // レイアウト：左下
    setLayoutLB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );
    
    // 表示情報減少ボタン
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecRotation, "R" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecScale, "S" );
    
    // レイアウト：右上
    setLayoutRT( -(4+64+4+32+4) + 1, 5, eLAYOUT_DIR_B, eLAYOUT_DIR_N );
    
    // 機能メニュー
    m_pMenu = new CPullDownMenu( false, true, false );
    m_pMenu->setMaxCenterWH( 216, 144/*304*/ );
    putNodeOnLayout( m_pMenu );

    //-----------------------------------------
    // ノード追加
    //-----------------------------------------
	addNode( m_pButtonExit );
	addNode( m_pButtonReset );
    addNode( m_pButtonRand );
        
    addNode( m_pButtonAnim );
    addNode( m_pButtonAnimH );
    addNode( m_pButtonAnimV );
    addNode( m_pButtonAnimT );
    addNode( m_pButtonAnimS );
    addNode( m_pButtonAnimLR );
    addNode( m_pButtonAnimUD );
    addNode( m_pButtonAnimPull );
    addNode( m_pButtonAnimShot );
    addNode( m_pButtonAnimStop );

    addNode( m_pButtonIncAdjustH );     addNode( m_pButtonDecAdjustH );
    addNode( m_pButtonIncAdjustV );     addNode( m_pButtonDecAdjustV );
    addNode( m_pButtonIncAdjustT );     addNode( m_pButtonDecAdjustT );
    addNode( m_pButtonIncAdjustS );     addNode( m_pButtonDecAdjustS );
    addNode( m_pButtonIncAngleLR );     addNode( m_pButtonDecAngleLR );
    addNode( m_pButtonIncAngleUD );     addNode( m_pButtonDecAngleUD );
    
    addNode( m_pButtonIncRotation );    addNode( m_pButtonDecRotation );
    addNode( m_pButtonIncScale );       addNode( m_pButtonDecScale );

    addNode( m_pMenu );

	// 入場
	m_pButtonExit->enter();
	m_pButtonReset->enter();
    m_pButtonRand->enter();
    
    m_pButtonAnim->enter();
    m_pButtonAnimH->enter();
    m_pButtonAnimV->enter();
    m_pButtonAnimT->enter();
    m_pButtonAnimS->enter();
    m_pButtonAnimLR->enter();
    m_pButtonAnimUD->enter();
    m_pButtonAnimPull->enter();
    m_pButtonAnimShot->enter();
    m_pButtonAnimStop->enter();

    m_pButtonIncAdjustH->enter();       m_pButtonDecAdjustH->enter();
    m_pButtonIncAdjustV->enter();       m_pButtonDecAdjustV->enter();
    m_pButtonIncAdjustT->enter();       m_pButtonDecAdjustT->enter();
    m_pButtonIncAdjustS->enter();       m_pButtonDecAdjustS->enter();
    m_pButtonIncAngleLR->enter();      m_pButtonDecAngleLR->enter();
    m_pButtonIncAngleUD->enter();      m_pButtonDecAngleUD->enter();
    
    m_pButtonIncRotation->enter();      m_pButtonDecRotation->enter();
    m_pButtonIncScale->enter();         m_pButtonDecScale->enter();
    
    m_pMenu->enter();
    
    // 初期値：表示
    m_fDispRotation = 0.0f;
    m_fDispScale = 1.0f;

    // 初期値：アニメ
    m_bAnimH = false;
    m_bAnimV = false;
    m_bAnimT = false;
    m_bAnimS = false;
    m_bAnimLR = false;
    m_bAnimUD = false;

    // メニューの初期値＆確定
    for( int i=0; i<eBTMI_MAX; i++ ){
        m_nArrMenuVal[i] = _menu_item_info[i][1];
    }

    // リセット
    onReset();
    
    // 初期状態のテクスチャ生成
    createBmp();
    
#ifdef ENABLE_SS
    m_bDumpSS = false;
    m_nDumpSkipCount = 0;
#endif
}

//------------------------
// 終了
//------------------------
void CBezierTestLoop::exit0( void ){}

//------------------------
// 更新
//------------------------
void CBezierTestLoop::update0( void ){
	// ループの終了
	if( m_pButtonExit->isTapped() ){
		CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_EXIT, 0, 0 );
		return;
	}
    
    // 更新フラグ
    bool isUpdated = false;
    
    //------------------
    // 調整ボタン押下
    //------------------
    bool isAdjChanged = false;
    CHECK_ADJUST_BUTTON( m_pButtonIncAdjustH, m_pButtonDecAdjustH, m_fAdjustRateH, rateH0 );
    CHECK_ADJUST_BUTTON( m_pButtonIncAdjustV, m_pButtonDecAdjustV, m_fAdjustRateV, rateV0 );
    CHECK_ADJUST_BUTTON( m_pButtonIncAdjustT, m_pButtonDecAdjustT, m_fAdjustRateT, rateT0 );
    CHECK_ADJUST_BUTTON( m_pButtonIncAdjustS, m_pButtonDecAdjustS, m_fAdjustRateS, rateS0 );
    CHECK_ADJUST_BUTTON( m_pButtonIncAngleLR, m_pButtonDecAngleLR, m_fAdjustRateLR, rateLR0 );
    CHECK_ADJUST_BUTTON( m_pButtonIncAngleUD, m_pButtonDecAngleUD, m_fAdjustRateUD, rateUD0 );

    // 回転
    float rotation0 = m_fDispRotation;
    if( m_pButtonIncRotation->isTapped() ){
        m_fDispRotation += 5.0f;
        if( m_fDispRotation >= 360.0f ){
            m_fDispRotation = 0.0f;
        }
    }else if( m_pButtonDecRotation->isTapped() ){
        m_fDispRotation -= 5.0f;
        if( m_fDispRotation <= -360.0f ){
            m_fDispRotation = 0.0f;
        }
    }
    if( m_fDispRotation != rotation0 ){
        isAdjChanged = true;
    }

    // スケール
    float scale0 = m_fDispScale;
    if( m_pButtonIncScale->isTapped() ){
        m_fDispScale += 0.1f;
        if( m_fDispScale > 1.5f ){
            m_fDispScale = 1.5f;
        }
    }else if( m_pButtonDecScale->isTapped() ){
        m_fDispScale -= 0.1f;
        if( m_fDispScale < 0.1f ){
            m_fDispScale = 0.1f;
        }
    }
    if( m_fDispScale != scale0 ){
        isAdjChanged = true;
    }

    // 調整値に差分がでたらBMP更新
    if( isAdjChanged ){
        isUpdated = true;
    }

	//------------------------------------
    // UI
    //------------------------------------
	// [RESET]
	if( m_pButtonReset->isTapped() ){
        // メニューの一部を戻す
        m_nArrMenuVal[eBTMI_SELECT_FACE] = _menu_item_info[eBTMI_SELECT_FACE][1];
        m_nArrMenuVal[eBTMI_SELECT_UP] = _menu_item_info[eBTMI_SELECT_UP][1];
        m_nArrMenuVal[eBTMI_SELECT_LOW] = _menu_item_info[eBTMI_SELECT_LOW][1];
        m_nArrMenuVal[eBTMI_SELECT_COSTUME] = _menu_item_info[eBTMI_SELECT_LOW][1];
        m_nArrMenuVal[eBTMI_SELECT_COSTUME] = _menu_item_info[eBTMI_SELECT_COSTUME][1];
        m_nArrMenuVal[eBTMI_SELECT_STROKE] = _menu_item_info[eBTMI_SELECT_STROKE][1];
        m_nArrMenuVal[eBTMI_SELECT_COLOR] = _menu_item_info[eBTMI_SELECT_COLOR][1];
        m_nArrMenuVal[eBTMI_ENABLE_FRILL] = _menu_item_info[eBTMI_ENABLE_FRILL][1];
        m_nArrMenuVal[eBTMI_ENABLE_STRIPE] = _menu_item_info[eBTMI_ENABLE_STRIPE][1];
        m_nArrMenuVal[eBTMI_ENABLE_TOUCH] = _menu_item_info[eBTMI_ENABLE_TOUCH][1];
        m_nArrMenuVal[eBTMI_ENABLE_FILL_EDGE] = _menu_item_info[eBTMI_ENABLE_FILL_EDGE][1];
        m_nArrMenuVal[eBTMI_ENABLE_FILL_OPTION] = _menu_item_info[eBTMI_ENABLE_FILL_OPTION][1];

        onReset();
        isUpdated = true;
	}
    // [RAND]
    else if( m_pButtonRand->isTapped() ){
        // 設定＆メニュー確定
        fixForSetting( false, true );
        fixForMenu();

        isUpdated = true;
    }
    // [MENU]
    else if( m_pMenu->isDecided() ){
        // 選択対象の更新
        int select = m_pMenu->getSelect();
        int val = m_nArrMenuVal[select] + 1;
        if( val >= _menu_item_info[select][0] ){
            val = 0;
        }
        m_nArrMenuVal[select] = val;
        
        // 設定＆メニュー確定
        fixForSetting( (select==eBTMI_SELECT_CHARA), false );
        fixForMenu();
        
        isUpdated = true;
    }
    // [ANIM]
    else if( m_pButtonAnim->isTapped() ){
        onAnim();
    }
    // [ANIM H]
    else if( m_pButtonAnimH->isTapped() ){
        m_bAnimH = !m_bAnimH;
        if( m_bAnimH ){
            m_fAdjustRateVH = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [ANIM V]
    else if( m_pButtonAnimV->isTapped() ){
        m_bAnimV = !m_bAnimV;
        if( m_bAnimV ){
            m_fAdjustRateVV = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [ANIM T]
    else if( m_pButtonAnimT->isTapped() ){
        m_bAnimT = !m_bAnimT;
        if( m_bAnimT ){
            m_fAdjustRateVT = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [ANIM S]
    else if( m_pButtonAnimS->isTapped() ){
        m_bAnimS = !m_bAnimS;
        if( m_bAnimS ){
            m_fAdjustRateVS = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [ANIM LR]
    else if( m_pButtonAnimLR->isTapped() ){
        m_bAnimLR = !m_bAnimLR;
        if( m_bAnimLR ){
            m_fAdjustRateVLR = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [ANIM UD]
    else if( m_pButtonAnimUD->isTapped() ){
        m_bAnimUD = !m_bAnimUD;
        if( m_bAnimUD ){
            m_fAdjustRateVUD = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
        }
    }
    // [PULL]ボタン
    else if( m_pButtonAnimPull->isTapped() ){
        onAnimPull();
    }
    // [SHOT]ボタン
    else if( m_pButtonAnimShot->isTapped() ){
        onAnimShot();
    }
    // [STOP]ボタン
    else if( m_pButtonAnimStop->isTapped() ){
        // 全て停止させる
        m_bAnimH = m_bAnimV = m_bAnimT = m_bAnimS = false;
        m_bAnimLR = m_bAnimUD = false;
        m_bAnimPull = false;
    }
#ifdef ENABLE_SS
    // 画面中央がタップされたらスクショ指定の切り替え
    else{
        CTouchStatusCell* pCheckBase = CTouchMgr::CheckOval( eTOUCH_CHECK_TAPPED, SA_XC-96, SA_YM-96, 192, 192 );
        if( pCheckBase != NULL ){
            m_bDumpSS = !m_bDumpSS;
            m_nDumpSkipCount = 0;
        }
    }
#endif

    //------------------------------
    // アニメ更新
    //------------------------------
    if( updateAnim() ){
        isUpdated = true;
    }

    //------------------------------
    // 更新されていたら
    //------------------------------
    if( isUpdated ){
        // BMPの作成
        createBmp();

#ifdef ENABLE_SS
        if( m_bDumpSS ){
            if( m_nDumpSkipCount <= 0 ){
                CMainRenderer::ReserveScreenShot();
                m_nDumpSkipCount = 3;   // ３フレに１枚の割合にしておく
            }
        }
#endif
    }

#ifdef ENABLE_SS
    if( m_nDumpSkipCount > 0 ){
        m_nDumpSkipCount--;
    }
#endif

    fixForAnimButton();
}

//------------------------
// 描画
//------------------------
void CBezierTestLoop::onDraw0( void ){
    CTex* pTexForLine = m_pBmpTex->getTexForLine();
    CTex* pTexForColor = m_pBmpTex->getTexForColor();
    CTex* pTexForPath = m_pBmpTex->getTexForPath();

    CDrawCell* pDC = CDrawCell::GetFreeCell();
    char* buf = CMemMgr::GetBufFromTempStr();

    float x, y;
    float s = TEX_DRAW_SCALE;
    float pixelRate = GM_P_RATE;
    float w0 = s * CBmpGenerator::GetBaseTexSizeW();        // 名目上のサイズ
    float h0 = s * CBmpGenerator::GetBaseTexSizeH();        // 名目上のサイズ
    float w = s * CBmpGenerator::GetTotalTexSizeW();        // 実サイズ
    float h = s * CBmpGenerator::GetTotalTexSizeH();        // 実サイズ
    
    // [RECT] 下地
    if( m_nArrMenuVal[eBTMI_DISP_BG] != 0 ){
        // 実サイズ
        x = SA_XC - w/2.0f;
        y = SA_YM - h/2.0f;
        
        DWORD col;
#ifdef ENABLE_SS
        if( m_bDumpSS ){
            col = (m_nArrMenuVal[eBTMI_DISP_ON_GROUND] != 0)? 0xFFFFFFA0: 0xFFFFFF60;
        }else
#endif
        {
            col = (m_nArrMenuVal[eBTMI_DISP_ON_GROUND] != 0)? 0xFFFFFF80: 0xFFFFFF40;
        }

        pDC->clear();
        pDC->setRect( w, h );
        pDC->setRGBA( col );
        pDC->draw( x, y );

        if( m_nArrMenuVal[eBTMI_DISP_ON_GROUND] == 0 ){
            // 名目サイズ
            x = SA_XC - w0/2.0f;
            y = SA_YM - h0/2.0f;

            pDC->clear();
            pDC->setRect( w0, h0 );
            pDC->setRGBA( 0xFFFFFF80 );
            pDC->draw( x, y );
        }
    }

    // 接地指定があれば調整
    if( m_nArrMenuVal[eBTMI_DISP_ON_GROUND] != 0 ){
        x = SA_XC - pTexForLine->getBaseX()/pixelRate;
        y = SA_YM - pTexForLine->getBaseY()/pixelRate + (5*h0/8);
    }else{
        x = SA_XC - w/2.0f;
        y = SA_YM - h/2.0f;
    }
    
    // [P8D&P] ベジェ：塗り
    if( m_nArrMenuVal[eBTMI_SELECT_COLOR] != 0 ){
        pDC->clear();
        pDC->setTex( pTexForColor, CImgMgr::GetImgPal( eIMG_PAL_BEZIER_COLOR ) );
        pDC->setBlendAlpha();
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->drawF( x, y );
    }
    
    // [GRAY] ベジェ：線
    if( m_nArrMenuVal[eBTMI_SELECT_STROKE] != 0 ){
        pDC->clear();
        pDC->setTex( pTexForLine, NULL );
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->setBlendAlpha();
        pDC->setRGBA( 0x000000FF );
        pDC->drawF( x, y );

        if( m_nArrMenuVal[eBTMI_DISP_ON_GROUND] == 0 ){
            // カーソル表示：基本座標
            float groundX = SA_XC - w/2 + pTexForLine->getBaseX()/pixelRate;
            float groundY = SA_YM - h/2 + pTexForLine->getBaseY()/pixelRate;

            pDC->clear();
            pDC->setRect( 1, 3 );
            pDC->setRGBA( 0xFF0000FF );
            pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
            pDC->drawF( groundX, groundY );
            
            pDC->clear();
            pDC->setRect( 3, 1 );
            pDC->setRGBA( 0xFF0000FF );
            pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
            pDC->drawF( groundX, groundY );
            
            // カーソル表示：フォーカス座標
            float focusX = SA_XC - w/2 + pTexForLine->getFocusX()/pixelRate;
            float focusY = SA_YM - h/2 + pTexForLine->getFocusY()/pixelRate;

            pDC->clear();
            pDC->setRect( 1, 3 );
            pDC->setRGBA( 0xFF0000FF );
            pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
            pDC->drawF( focusX, focusY );
            
            pDC->clear();
            pDC->setRect( 3, 1 );
            pDC->setRGBA( 0xFF0000FF );
            pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
            pDC->drawF( focusX, focusY );
        }
    }
    
    // [P8D&P] ベジェ：パス
    if( m_nArrMenuVal[eBTMI_DISP_WORK_PATH] != 0  ){
        pDC->clear();
        pDC->setTex( pTexForPath, CImgMgr::GetImgPal( eIMG_PAL_BEZIER_WORK_PATH ) );
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->drawF( x, y );
    }
    
    // 情報
    if( m_nArrMenuVal[eBTMI_DISP_ON_GROUND] == 0 ){
        // ログ（名目上のサイズの左上）
        sprintf( buf, "B:%d-%d F:%d-%d",
                 (int)pTexForLine->getBaseX(), (int)pTexForLine->getBaseY(),
                 (int)pTexForLine->getFocusX(), (int)pTexForLine->getFocusY()
                );

        // 名目サイズ
        x = SA_XC - w0/2.0f;
        y = SA_YM - h0/2.0f;

        pDC->clear();
        pDC->setStr( eFONT_DEBUG, buf );
        pDC->setAlignXY( eDRAW_ALIGN_X_L, eDRAW_ALIGN_Y_B );
        pDC->draw( x+1, y-1 );

        if( m_bAnimOn ){
            // ログ（※遅延ログ）
            for( int i=0; i<eDELAY_LOG_MAX; i++ ){
                float x = SA_XC - 32*(2-i) + 16;
                float y = SA_YM - 32*5;
                // 遅延幅は適当（※とりあえず名目サイズの半分）
                x -= CConst::ConvBezierPositionRate( w0/2, m_pDelayLog->getDelayRateX((eDELAY_LOG)i) );
                y -= CConst::ConvBezierPositionRate( h0/2, m_pDelayLog->getDelayRateY((eDELAY_LOG)i) );
                
                // ログ（※表示フラグ）
                sprintf( buf, "*" );
                pDC->clear();
                pDC->setStr( eFONT_DEBUG, buf );
                pDC->setAlignXY( eDRAW_ALIGN_X_C, eDRAW_ALIGN_Y_M );
                if( m_bAnimPull ){
                    pDC->setRGBA( 0xFF0000FF );
                }
                pDC->draw( x, y );
            }
        }
    }
    
    // 調整値表示
    float ofsX = SA_W - 64;
    float ofsY = SA_H - 8;
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "UD:%0.2f", m_fAdjustRateUD );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "LR:%0.2f", m_fAdjustRateLR );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "S:%0.2f", m_fAdjustRateS );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "T:%0.2f", m_fAdjustRateT );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "V:%0.2f", m_fAdjustRateV );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "H:%0.2f", m_fAdjustRateH );

    ofsX = 64;
    ofsY = SA_H - 8;
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "R:%0.2f", m_fDispRotation );
    DRAW_ADJUST_LABEL_FROM_BOTTOM( "S:%0.2f", m_fDispScale );
    
    // 生成時間
    CDrawCell::SetLogAlignRT( TIME_LOG_S, TIME_LOG_X, TIME_LOG_Y );
    
    int genTime = m_pBmpTex->getBmpGenTime();
    sprintf( buf, "bmp:%d.%d ms", genTime/1000, (genTime%1000)/100 );
    CDrawCell::DrawLogAlign( buf );

    int genTimeForPath = m_pBmpTex->getBmpGenTimeForPath();
    sprintf( buf, "path: %d.%d ms", genTimeForPath/1000, (genTimeForPath%1000)/100 );
    CDrawCell::DrawLogAlign( buf );
}

//-------------------
// ベジェ確認：アニメ更新
//-------------------
bool CBezierTestLoop::updateAnim( void ){
    // 指定があれば
    if( m_bAnimOn ){
        if( !m_bAnimH ){ STOP_ADJUST_V( m_fAdjustRateVH ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateH, m_fAdjustRateVH );
        
        if( !m_bAnimV ){ STOP_ADJUST_V( m_fAdjustRateVV ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateV, m_fAdjustRateVV );
        
        if( !m_bAnimT ){ STOP_ADJUST_V( m_fAdjustRateVT ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateT, m_fAdjustRateVT );
        
        if( !m_bAnimS ){ STOP_ADJUST_V( m_fAdjustRateVS ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateS, m_fAdjustRateVS );
        
        if( !m_bAnimLR ){ STOP_ADJUST_V( m_fAdjustRateVLR ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateLR, m_fAdjustRateVLR );
        
        if( !m_bAnimUD ){ STOP_ADJUST_V( m_fAdjustRateVUD ); }
        UPDATE_ADJUST_VALUE( m_fAdjustRateUD, m_fAdjustRateVUD );
        
        // 引っ張り更新
        if( m_bAnimPull ){
            // 引っ張り方向の更新（※円運動）
            float pullX = (int)(8*CMath::GetSin( m_fAnimPullCount ));
            if( m_bAnimPullRevX ){ pullX *= -1; }
            
            float pullY = (int)(8*CMath::GetCos( m_fAnimPullCount ));
            if( m_bAnimPullRevY ){ pullY *= -1; }

            for( int i=0; i<eDELAY_LOG_MAX; i++){
                m_pDelayLog->pull( (eDELAY_LOG)i, pullX, pullY );
            }
            
            m_fAnimPullCount = m_fAnimPullCount + 6.0f;
        }
    
        m_pDelayLog->updateWithFrame( 1.0f );
        return( true );
    }
    
    return( false );
}

//-------------------
// ベジェ確認：リセット
//-------------------
void CBezierTestLoop::onReset( void ){
    // 調整値
    m_fAdjustRateH = m_fAdjustRateVH = 0.0f;
    m_fAdjustRateV = m_fAdjustRateVV = 0.0f;
    m_fAdjustRateT = m_fAdjustRateVT = 0.0f;
    m_fAdjustRateS = m_fAdjustRateVS = 0.0f;
    m_fAdjustRateLR = m_fAdjustRateVLR = 0.0f;
    m_fAdjustRateUD = m_fAdjustRateVUD = 0.0f;
        
    // 遅延
    m_pDelayLog->clear();

    // アニメ停止
    m_bAnimOn = false;
    
    // 設定＆メニュー確定
    fixForSetting( true, false );
    fixForMenu();
    
    fixForAnimButton();
}

//-------------------------
// アニメボタン確定
//-------------------------
void CBezierTestLoop::fixForAnimButton( void ){
    m_pButtonAnim->setFocused( m_bAnimOn );
    
    m_pButtonAnimH->setDark( !m_bAnimOn );
    m_pButtonAnimH->setActive( m_bAnimOn );

    m_pButtonAnimV->setDark( !m_bAnimOn );
    m_pButtonAnimV->setActive( m_bAnimOn );

    m_pButtonAnimT->setDark( !m_bAnimOn );
    m_pButtonAnimT->setActive( m_bAnimOn );

    m_pButtonAnimS->setDark( !m_bAnimOn );
    m_pButtonAnimS->setActive( m_bAnimOn );

    m_pButtonAnimLR->setDark( !m_bAnimOn );
    m_pButtonAnimLR->setActive( m_bAnimOn );

    m_pButtonAnimUD->setDark( !m_bAnimOn );
    m_pButtonAnimUD->setActive( m_bAnimOn );

    m_pButtonAnimPull->setDark( !m_bAnimOn );
    m_pButtonAnimPull->setActive( m_bAnimOn );
    
    m_pButtonAnimShot->setDark( !m_bAnimOn );
    m_pButtonAnimShot->setActive( m_bAnimOn );

    m_pButtonAnimStop->setDark( !m_bAnimOn );
    
    // フォーカス設定
    m_pButtonAnimH->setFocused( m_bAnimH );
    m_pButtonAnimV->setFocused( m_bAnimV );
    m_pButtonAnimT->setFocused( m_bAnimT );
    m_pButtonAnimS->setFocused( m_bAnimS );
    m_pButtonAnimLR->setFocused( m_bAnimLR );
    m_pButtonAnimUD->setFocused( m_bAnimUD );
    m_pButtonAnimPull->setFocused( m_bAnimPull );
}

//----------------------
// ベジェ確認：調整アニメ
//----------------------
void CBezierTestLoop::onAnim( void ){
    // アニメON/OFF
    m_bAnimOn = !m_bAnimOn;

    // 初速
    if( m_bAnimH ){
        m_fAdjustRateVH = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateVH = 0.0f;
    }
    
    if( m_bAnimV ){
        m_fAdjustRateVV = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateVV = 0.0f;
    }
    
    if( m_bAnimT ){
        m_fAdjustRateVT = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateVT = 0.0f;
    }
    
    if( m_bAnimS ){
        m_fAdjustRateVS = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateVS = 0.0f;
    }
    
    if( m_bAnimLR ){
        m_fAdjustRateVLR = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateVLR = 0.0f;
    }
    
    if( m_bAnimUD ){
        m_fAdjustRateVUD = (0.05f + CRand::GetRandF( 0.1f ))/2.0f;
    }else{
        m_fAdjustRateUD = 0.0f;
    }
}

//------------------
// ベジェ確認：引っ張り
//------------------
void CBezierTestLoop::onAnimPull( void ){
    // アニメが有効になっていれば
    if( m_bAnimOn ){
        // 引っ張りON/OFF
        m_bAnimPull = !m_bAnimPull;

        // 指定が有効になっていれば
        m_bAnimPullRevX = CRand::IsRandHappen( 1, 2 );
        m_bAnimPullRevY = CRand::IsRandHappen( 1, 2 );
        m_fAnimPullCount = 360*CRand::GetRandF();
    }
}

//------------------
// ベジェ確認：ショット
//------------------
void CBezierTestLoop::onAnimShot( void ){
    // アニメが有効になっていれば
    if( m_bAnimOn ){
        // バネ（※単発）
        for( int i=0; i<eDELAY_LOG_MAX; i++ ){
            float pow = 12.0f + CRand::GetRandF( 12.0f );
            float rot = CRand::GetRandF( 360.0f );
            m_pDelayLog->shake( (eDELAY_LOG)i, pow, rot );
        }
    }
}

//--------------------
// セッティング確定
//--------------------
void CBezierTestLoop::fixForSetting( bool isReset, bool isRandom ){
    CBmpDotSettingData* pSetting = m_pBmpTex->getDotSetting();

    // リセット指定があればデフォルトに
    if( isReset ){
        pSetting->setDefault();
    }

    CBmpDotSettingSlotData* pSlot;
    int charaUid = _menu_item_UID_CHARA[m_nArrMenuVal[eBTMI_SELECT_CHARA]];
    int faceUid = BD_UID_INVALID;
    int upUid = BD_UID_INVALID;
    int lowUid = BD_UID_INVALID;
    int suitUid = BD_UID_INVALID;
    eBD_CATEGORY categoryForFace = eBD_CATEGORY_BODY;
    eBD_CATEGORY categoryForBody = eBD_CATEGORY_BODY;

    // ランダム指定があれば設定
    if( isRandom ){
        // パーツのシャッフル
        pSetting->setRandomPart();

        // テンションとサイズの設定
        pSetting->setRandomAdjForT();
        pSetting->setRandomAdjForS();

        // 調整値
        m_fAdjustRateH = 2.0f*CRand::GetRandF() - 1.0f;
        m_fAdjustRateV = 2.0f*CRand::GetRandF() - 1.0f;
        m_fAdjustRateT = 2.0f*CRand::GetRandF() - 1.0f;
        m_fAdjustRateS = 2.0f*CRand::GetRandF() - 1.0f;
        m_fAdjustRateLR = 2.0f*CRand::GetRandF() - 1.0f;
        m_fAdjustRateUD = 2.0f*CRand::GetRandF() - 1.0f;
        
        // 線と色
        m_nArrMenuVal[eBTMI_SELECT_STROKE] = 1 + CRand::GetRand( _menu_item_info[eBTMI_SELECT_STROKE][0]-1 );
        m_nArrMenuVal[eBTMI_SELECT_COLOR] = 1 + CRand::GetRand( _menu_item_info[eBTMI_SELECT_COLOR][0]-1 );
        
#if 0
        // ベジェ機能
        m_nArrMenuVal[eBTMI_ENABLE_TOUCH] = (CRand::IsRandHappen( 1, 3 )? 1: 0);
        m_nArrMenuVal[eBTMI_ENABLE_STRIPE] = (CRand::IsRandHappen( 1, 3 )? 1: 0);
        m_nArrMenuVal[eBTMI_ENABLE_FRILL] = (CRand::IsRandHappen( 1, 3 )? 1: 0);
        m_nArrMenuVal[eBTMI_ENABLE_FILL_EDGE] = (CRand::IsRandHappen( 1, 3 )? 1: 0);
        m_nArrMenuVal[eBTMI_ENABLE_FILL_OPTION] = (CRand::IsRandHappen( 1, 3 )? 1: 0);
#endif
    }else{
        // ぬいぐるみ（右）は無効
        CBmpDotSettingSlotData* pSlot = pSetting->searchSlot( eBD_SLOT_AmOptionA, 1 );
        pSlot->setInvalid( true );
        
        // マイク（右）は無効
        pSetting->setOptionForSlotValidAt( eBD_SLOT_FcEar, 1, 0, false );

        // 基本設定
        pSetting->setIdForFace( charaUid );
        pSetting->setIdForBody( charaUid );
        faceUid = upUid = lowUid = charaUid;

        // [FACE]差し替え
        if( m_nArrMenuVal[eBTMI_SELECT_FACE] != 0 ){
            faceUid = _menu_item_UID_CHARA[m_nArrMenuVal[eBTMI_SELECT_FACE]-1];
            pSetting->setIdForFace( faceUid );
        }

        // [UP]差し替え
        if( m_nArrMenuVal[eBTMI_SELECT_UP] != 0 ){
            upUid = _menu_item_UID_CHARA[m_nArrMenuVal[eBTMI_SELECT_UP]-1];
            pSetting->setReplacedIdForUp( upUid );
        }else{
            pSetting->setReplacedIdForUp( BD_UID_INVALID );
        }

        // [LOW]差し替え
        if( m_nArrMenuVal[eBTMI_SELECT_LOW] != 0 ){
            lowUid = _menu_item_UID_CHARA[m_nArrMenuVal[eBTMI_SELECT_LOW]-1];
            pSetting->setReplacedIdForLow( lowUid );
        }else{
            pSetting->setReplacedIdForLow( BD_UID_INVALID );
        }
    }

    //-----------------------
    // [SUIT]設定
    //-----------------------
    // 指定が有効であれば設定を差し替える
    if( m_nArrMenuVal[eBTMI_SELECT_COSTUME] != 0 ){
        suitUid = _menu_item_UID_COSTUME[m_nArrMenuVal[eBTMI_SELECT_COSTUME]-1];
        faceUid = suitUid;
        upUid = suitUid;
        lowUid = suitUid;
        categoryForFace = eBD_CATEGORY_SUIT;
        categoryForBody = eBD_CATEGORY_SUIT;
    }
    pSetting->setIdForSuit( suitUid );

    // 顔が有効であれば
    if( IS_BD_UID_VALID( faceUid ) ){
        // HdTopCover
        pSlot = pSetting->searchSlot( eBD_SLOT_HdTopCover, 0 );
        pSlot->setCategory( categoryForFace );
        pSlot->setUid( faceUid );
        
        // FcUnderCover
        pSlot = pSetting->searchSlot( eBD_SLOT_FcUnderCover, 0 );
        pSlot->setCategory( categoryForFace );
        pSlot->setUid( faceUid );
    }
    
    // 上半身有効であれば
    if( IS_BD_UID_VALID( upUid ) ){
        // BlBase
        pSlot = pSetting->searchSlot( eBD_SLOT_BlBase, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( upUid );

        // UpBase
        pSlot = pSetting->searchSlot( eBD_SLOT_UpBase, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( upUid );
        
        // NkCover
        pSlot = pSetting->searchSlot( eBD_SLOT_NkCover, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( upUid );

        // AmCover[0]
        pSlot = pSetting->searchSlot( eBD_SLOT_AmCover, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( upUid );

        // AmCover[1]
        pSlot = pSetting->searchSlot( eBD_SLOT_AmCover, 1 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( upUid );
    }

    // 下半身が有効であれば
    if( IS_BD_UID_VALID( lowUid ) ){
        // LwBase
        pSlot = pSetting->searchSlot( eBD_SLOT_LwBase, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( lowUid );

        // LgCover[0]
        pSlot = pSetting->searchSlot( eBD_SLOT_LgCover, 0 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( lowUid );

        // LgCover[1]
        pSlot = pSetting->searchSlot( eBD_SLOT_LgCover, 1 );
        pSlot->setCategory( categoryForBody );
        pSlot->setUid( lowUid  );
    }

    //-----------------------------------------------
    // FgTopは必ず有効な値を指定する（※無効値だと表示されない）
    //-----------------------------------------------
    if( ! IS_BD_UID_VALID( upUid ) ){
        upUid = _menu_item_UID_CHARA[m_nArrMenuVal[eBTMI_SELECT_CHARA]];
    }
    
    pSlot = pSetting->searchSlot( eBD_SLOT_FgTop, 0 );
    pSlot->setCategory( categoryForBody );
    pSlot->setUid( upUid );
}

//----------------
// メニュー確定
//----------------
void CBezierTestLoop::fixForMenu( void ){
    // ストロークへ反映
    if( m_nArrMenuVal[eBTMI_SELECT_STROKE] > 0 ){
        CBezierStructConv::SetFixedBrush( (eBRUSH)(eBRUSH_C_BASE+m_nArrMenuVal[eBTMI_SELECT_STROKE]) );
    }

    // カラーへ反映
    if( m_nArrMenuVal[eBTMI_SELECT_COLOR] > 0 ){
        CBmpPalData* pPal = CBmpPalMgr::GetBmpPalDataAt( m_nArrMenuVal[eBTMI_SELECT_COLOR]-1 );
        if( pPal != NULL ){
            pPal->loadToImgPal( eIMG_PAL_BEZIER_COLOR );
        }
    }

    // ワークパスへ反映
    if( m_nArrMenuVal[eBTMI_DISP_WORK_PATH] != 0 ){
        // パスのみ
        if( m_nArrMenuVal[eBTMI_DISP_WORK_PATH] == 1 ){
            CBezier::SetDispOffDirVectorAlways( true );
            CBezier::SetDispDirVectorAlways( false );
        }
        // 方向線
        else if( m_nArrMenuVal[eBTMI_DISP_WORK_PATH] == 2 ){
            CBezier::SetDispOffDirVectorAlways( false );
            CBezier::SetDispDirVectorAlways( true );
        }
    }
    
    // ベジェ制限へ反映
    CBezier::SetDispOffTouch( m_nArrMenuVal[eBTMI_ENABLE_TOUCH] == 0 );
    CBezier::SetDispOffStripe( m_nArrMenuVal[eBTMI_ENABLE_STRIPE] == 0 );
    CBezier::SetDispOffFrill( m_nArrMenuVal[eBTMI_ENABLE_FRILL] == 0 );
    CBezier::SetDispOffEdge( m_nArrMenuVal[eBTMI_ENABLE_FILL_EDGE] == 0 );
    CBezier::SetDispOffFillOption( m_nArrMenuVal[eBTMI_ENABLE_FILL_OPTION] == 0 );

    // メニュー再構築
    char* buf = CMemMgr::GetBufFromTempStr();

    m_pMenu->setItemNum( eBTMI_MAX, false );
    for( int i=0; i<eBTMI_MAX; i++ ){
        sprintf( buf, "%s: %s", _menu_item_label[i], _menu_item[i][m_nArrMenuVal[i]] );
        m_pMenu->setItemAt( i, buf );
    }
    
    m_pMenu->fixMenu();
}

//----------------------
// ベジェ確認：BMP作成
//----------------------
void CBezierTestLoop::createBmp( void ){
    CBmpDotSettingData* pSetting = m_pBmpTex->getDotSetting();

    // 一時パラメータの設定
    pSetting->resetFixed();

    pSetting->setFlipHFixed( m_nArrMenuVal[eBTMI_DISP_FLIP_H] != 0 );
    pSetting->setFlipVFixed( m_nArrMenuVal[eBTMI_DISP_FLIP_V] != 0 );
    pSetting->setScaleFixed( m_fDispScale*BMP_GEN_BASE_SCALE );
    pSetting->setRotFixed( m_fDispRotation );
    
    pSetting->setEmoFixed( eBD_EMO_BASE );
    pSetting->setFormFixed( eBD_FORM_FRONT );

    pSetting->setAdjustHFixed( m_fAdjustRateH );
    pSetting->setAdjustVFixed( m_fAdjustRateV );
    pSetting->setAdjustTFixed( m_fAdjustRateT );
    pSetting->setAdjustSFixed( m_fAdjustRateS );
    pSetting->setAngleLRFixed( m_fAdjustRateLR );
    pSetting->setAngleUDFixed( m_fAdjustRateUD );
    
    // BMP作成パラメータ準備
    stBMP_GEN_CREATE_PARAM createParam;
    CBmpGenerator::SetBmpGenCreateParam( &createParam, pSetting, m_pDelayLog );

#ifdef LAYER_BASE
    //-----------------------------------
    // レイヤーリストによる描画
    //-----------------------------------
    CList list;
    
    // フリル
    if( m_pLayerRefForFrill != NULL ){ list.add( m_pLayerRefForFrill ); }
    // 線修復
    else if( m_pLayerLineRepair != NULL ){ list.add( m_pLayerLineRepair ); }
    // フック[Ａ／Ｂ／Ｃ]
    else if( m_pLayerHookA != NULL && m_pLayerHookB != NULL && m_pLayerHookC != NULL ){
        #ifdef LAYER_BASE_FOR_HOOK_CHECK_REVERSE
        // C -> A -> B
        list.add( m_pLayerHookC );
        list.add( m_pLayerHookA );
        list.add( m_pLayerHookB );
        #else
        // B -> A -> C
        list.add( m_pLayerHookB );
        list.add( m_pLayerHookA );
        list.add( m_pLayerHookC );
        #endif
    }
    // 遅延
    else if( m_pLayerDelay != NULL ){ list.add( m_pLayerDelay ); }
    // タッチ
    else if( m_pLayerTouch != NULL ){ list.add( m_pLayerTouch ); }
    // 基本
    else if( m_pLayerBase != NULL ){ list.add( m_pLayerBase ); }

    m_pBmpTex->createBmpWithLayerList( &createParam, &list, true );

    // リスト切断（※[CList]が破棄される際はなにもしないので明示的に切断）
    list.disconnectAll();
#else
    //-----------------------------------
    // BMP設定による描画
    //-----------------------------------
    m_pBmpTex->createBmpWithDotSetting( &createParam, eBD_SLOT_FgTop, 0, 0, true );
#endif
}

//----------------------
// バッファ確保
//----------------------
void CBezierTestLoop::allocForTest( void ){
    // 用心に解放
    releaseForTest();
    
    //--------------------------
    // 領域確保
    //--------------------------
    CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
    m_pDelayLog = new CDelayLog();
    m_pBmpTex = new CBmpTex();
    CMemMgr::PopTargetField();

#ifdef LAYER_BASE
    // 基本テストであれば利用優先順に確保
    bool isDone = false;
    
    #ifdef LAYER_BASE_FOR_FRILL
    if( ! isDone ){
        // フリルの場合はデータの参照を受け取る
        m_pLayerRefForFrill = CBezier::GetLayerDataForFrill( LAYER_BASE_FOR_FRILL_TARGET );
        isDone = true;
    }
    #endif
    
    #ifdef LAYER_BASE_FOR_LINE_REPAIR
    if( ! isDone ){
        allocForLayerForLineRepair();
        isDone = true;
    }
    #endif

    #ifdef LAYER_BASE_FOR_HOOK
    if( ! isDone ){
        allocForLayerForHookA();
        allocForLayerForHookB();
        allocForLayerForHookC();
        isDone = true;
    }
    #endif
    
    #ifdef LAYER_BASE_FOR_DELAY
    if( ! isDone ){
        allocForLayerForDelay();
        isDone = true;
    }
    #endif
    
    #ifdef LAYER_BASE_FOR_TOUCH
    if( ! isDone ){
        allocForLayerForTouch();
        isDone = true;
    }
    #endif

    if( ! isDone ){
        allocForLayerForBase();
        isDone = false;
    }
#endif
}

//----------------------
// バッファ解放
//----------------------
void CBezierTestLoop::releaseForTest( void ){
    // 領域解放
    SAFE_DELETE( m_pDelayLog );
    SAFE_DELETE( m_pBmpTex );
    
    // テストレイヤー解放
    CLayerData::Free( m_pLayerBase );       m_pLayerBase = NULL;
    CLayerData::Free( m_pLayerTouch );      m_pLayerTouch = NULL;
    CLayerData::Free( m_pLayerDelay );      m_pLayerDelay = NULL;
    CLayerData::Free( m_pLayerHookA );      m_pLayerHookA = NULL;
    CLayerData::Free( m_pLayerHookB );      m_pLayerHookB = NULL;
    CLayerData::Free( m_pLayerHookC );      m_pLayerHookC = NULL;
    CLayerData::Free( m_pLayerLineRepair ); m_pLayerLineRepair = NULL;
    m_pLayerRefForFrill = NULL;   // フリル素材は[CBezier]クラスで管理されているデータへの参照なので解放しない
}
