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
#include "draw/tex/bezier/BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define TEX_DRAW_SCALE      1.0f
#define BMP_GEN_BASE_SCALE  1.0f

//--------------------------------------------------------------------------
// [CLayerData]による表示確認
//--------------------------------------------------------------------------
// この定義を有効にすると[createBmpWithLayerData]が呼ばれる
// この定義を無効にすると[createBmpWithBmpDot]が呼ばれる（※BD素材によるテクスチャ作成）
//--------------------------------------------------------------------------
//#define LAYER_BASE

// 基本確認が定義されたら詳細の指定
#ifdef LAYER_BASE
    //----------------------------------------------------------------------
    // フリル形状確認
    //----------------------------------------------------------------------
    // この定義を有効にすると[CBezier::GetLayerDataForFrill]から素材を参照する
    //----------------------------------------------------------------------
    //#define LAYER_BASE_FOR_FRILL

    // 参照先
    //#define LAYER_BASE_FOR_FRILL_TARGET        eSTROKE_FRILL_TEST_CIRCLE
    //#define LAYER_BASE_FOR_FRILL_TARGET        eSTROKE_FRILL_TEST_TRIANGLE
    //#define LAYER_BASE_FOR_FRILL_TARGET        eSTROKE_FRILL_TEST_SQUARE

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

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
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
}

//------------------------
// デストラクタ
//------------------------
CBezierTestLoop::~CBezierTestLoop( void ){}

//------------------------
// 初期化
//------------------------
void CBezierTestLoop::init0( void ){
    // ベジェ環境
    CBezier::ResetAllFlagForDev();
#ifdef LAYER_BASE
    CBezier::SetDispDirVectorAlways( true );
#else
    CBezier::SetDispOffDirVectorAlways( true );
#endif
    CBezier::SetDispPathActiveAlways( true );
    CBezier::SetDispOnlyAnchorPoint( true );
    CBmpGenerator::Reset();
    CBezierDataConv::SetFixedBrush( eBRUSH_INVALID );

    //-------------------------
    // 領域確保
    //-------------------------
    allocForTest();

	//-----------------------------------------
	// node
	//-----------------------------------------
    // レイアウト：右上
	setLayoutRT( 0, 0, eLAYOUT_DIR_B, eLAYOUT_DIR_N );

	// 終了ボタン
	m_pButtonExit = new CButton( eGRP_DEBUG_BUTTON, "EXIT" );
 	putNodeOnLayout( m_pButtonExit );

    // リセットボタン
    m_pButtonReset = new CButton( eGRP_DEBUG_BUTTON, "RESET" );
    putNodeOnLayout( m_pButtonReset );

    // 表示：背景
    m_pButtonBg = new CButton( eGRP_DEBUG_BUTTON_M, "BG" );
    putNodeOnLayout( m_pButtonBg );

    // 表示：線
    m_pButtonLine = new CButton( eGRP_DEBUG_BUTTON_M, "STR" );
    putNodeOnLayout( m_pButtonLine );

    // 表示：色
    m_pButtonColor = new CButton( eGRP_DEBUG_BUTTON_M, "COL" );
    putNodeOnLayout( m_pButtonColor );

    // 表示：パス
    m_pButtonPath = new CButton( eGRP_DEBUG_BUTTON_M, "PTH" );
    putNodeOnLayout( m_pButtonPath );

    // フリップ：横
    m_pButtonFlipH = new CButton( eGRP_DEBUG_BUTTON_M, "FH" );
    putNodeOnLayout( m_pButtonFlipH );

    // フリップ：縦
    m_pButtonFlipV = new CButton( eGRP_DEBUG_BUTTON_M, "FV" );
    putNodeOnLayout( m_pButtonFlipV );

    // 設置
    m_pButtonGround = new CButton( eGRP_DEBUG_BUTTON_M, "GND" );
    putNodeOnLayout( m_pButtonGround );

    // レイアウト：右上
    setLayoutRT( 0, 32+4, eLAYOUT_DIR_N, eLAYOUT_DIR_F );

    // ランダム
    m_pButtonSetRand = new CButton( eGRP_DEBUG_BUTTON, "RANDOM" );
    putNodeOnLayout( m_pButtonSetRand );

    // 設定読み込み
    m_pButtonSetRead = new CButton( eGRP_DEBUG_BUTTON, "COSTUME" );
    putNodeOnLayout( m_pButtonSetRead );

    // レイアウト：左上
    setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_F );
    
    // アニメボタン
    m_pButtonAnim = new CButton( eGRP_DEBUG_BUTTON, "ANIM" );
    putNodeOnLayout( m_pButtonAnim );

    // アニメ指定
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
    
    // 調整値増加ボタン
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAngleUD, "UD" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAngleLR, "LR" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustS, "S" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustT, "T" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustV, "V" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncAdjustH, "H" );

    // レイアウト：右下
    setLayoutRB( -88, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );

    // 調整値減少ボタン
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAngleUD, "UD" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAngleLR, "LR" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustS, "S" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustT, "T" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustV, "V" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecAdjustH, "H" );

    // レイアウト：左下
    setLayoutLB( 88, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );
    
    // 表示情報増加ボタン
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncRotation, "R" );
    REGIST_ADJUST_INC_BOTTON_FROM_BUTTOM( m_pButtonIncScale, "S" );

    // レイアウト：左下
    setLayoutLB( 0, 0, eLAYOUT_DIR_N, eLAYOUT_DIR_B );
    
    // 表示情報減少ボタン
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecRotation, "R" );
    REGIST_ADJUST_DEC_BOTTON_FROM_BUTTOM( m_pButtonDecScale, "S" );

    //-----------------------------------------
    // ノード追加
    //-----------------------------------------
	addNode( m_pButtonExit );
	addNode( m_pButtonReset );
    addNode( m_pButtonSetRand );
    addNode( m_pButtonSetRead );
    addNode( m_pButtonBg );
    addNode( m_pButtonLine );
    addNode( m_pButtonColor );
    addNode( m_pButtonPath );
    addNode( m_pButtonFlipH );
    addNode( m_pButtonFlipV );
    addNode( m_pButtonGround );

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

	// 入場
	m_pButtonExit->enter();
	m_pButtonReset->enter();
    m_pButtonSetRand->enter();
    m_pButtonSetRead->enter();
    m_pButtonBg->enter();
    m_pButtonLine->enter();
    m_pButtonColor->enter();
    m_pButtonPath->enter();
    m_pButtonFlipH->enter();
    m_pButtonFlipV->enter();
    m_pButtonGround->enter();

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
    
#ifdef ENABLE_SS
    m_bDumpSS = false;
    m_nDumpSkipCount = 0;
#endif

    // 初期値：表示
    m_bDispBg = true;       m_pButtonBg->setFocused( m_bDispBg );
    m_nLineType = 1;        m_pButtonLine->setFocused( m_nLineType != 0 );
    fixBrush( m_nLineType );
    m_nColorType = 1;       m_pButtonColor->setFocused( m_nColorType != 0 );
    m_bDispPath = false;    m_pButtonPath->setFocused( m_bDispPath );
    m_bFlipH = false;       m_pButtonFlipH->setFocused( m_bFlipH );
    m_bFlipV = false;       m_pButtonFlipV->setFocused( m_bFlipV );
#ifdef LAYER_BASE
    m_bGround = false;       m_pButtonGround->setFocused( m_bGround );
#else
    m_bGround = true;       m_pButtonGround->setFocused( m_bGround );
#endif
    
    m_fDispRotation = 0.0f;
    m_fDispScale = 1.0f;

    // 初期値：アニメ
    m_bAnimH = false;
    m_bAnimV = false;
    m_bAnimT = false;
    m_bAnimS = false;
    m_bAnimLR = false;
    m_bAnimUD = false;

    // リセット
    onReset();
    
    // 初期状態のテクスチャ生成
    createBmp();
}

//------------------------
// 終了
//------------------------
void CBezierTestLoop::exit0( void ){
    //-------------------------
    // 領域開放
    //-------------------------
    releaseForTest();

    // ベジェ関連
    CBezier::ResetAllFlagForDev();
    CBezier::SetDispDirVectorAlways( false );
    CBezier::SetDispOffDirVectorAlways( false );
    CBezier::SetDispPathActiveAlways( false );
    CBezier::SetDispOnlyAnchorPoint( false );
    CBmpGenerator::Reset();
    CBezierDataConv::SetFixedBrush( eBRUSH_INVALID );
}

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
    // 機能ボタン押下
    //------------------------------------
	// [RESET]ボタン
	if( m_pButtonReset->isTapped() ){
        onReset();
        isUpdated = true;
	}
    // [RAND]
    else if( m_pButtonSetRand->isTapped() ){
        // テンションとサイズの設定
        m_oSetting.setRandomAdjForT();
        m_oSetting.setRandomAdjForS();

        // アニメ中でなければ全ての値を設定
        if( ! m_bAnimOn ){
            m_fAdjustRateH = 2.0f*CRand::GetRandF() - 1.0f;
            m_fAdjustRateV = 2.0f*CRand::GetRandF() - 1.0f;
            m_fAdjustRateT = 2.0f*CRand::GetRandF() - 1.0f;
            m_fAdjustRateS = 2.0f*CRand::GetRandF() - 1.0f;
            m_fAdjustRateLR = 2.0f*CRand::GetRandF() - 1.0f;
            m_fAdjustRateUD = 2.0f*CRand::GetRandF() - 1.0f;
        }
        
        // 線と色の設定
        m_nLineType = 1 + CRand::GetRand( 5 );
        m_pButtonLine->setFocused( m_nLineType != 0 );
        fixBrush( m_nLineType );

        m_nColorType = 1 + CRand::GetRand( 4 );
        m_pButtonColor->setFocused( m_nColorType != 0 );

        isUpdated = true;
    }
    // [SETTING]
    else if( m_pButtonSetRead->isTapped() ){
        CBmpDotSettingData* pSetting = CBmpDotMgr::GetBmpDotSettingForWork();
        m_oSetting.copy( pSetting );
        isUpdated = true;
    }
    // [BG]ボタン
    else if( m_pButtonBg->isTapped() ){
        m_bDispBg = !m_bDispBg;
        m_pButtonBg->setFocused( m_bDispBg );
        isUpdated = true;
    }
    // [STROKE]ボタン
    else if( m_pButtonLine->isTapped() ){
        m_nLineType = (m_nLineType+1)%6;
        m_pButtonLine->setFocused( m_nLineType != 0 );
        fixBrush( m_nLineType );
        isUpdated = true;
    }
    // [DOT]ボタン
    else if( m_pButtonColor->isTapped() ){
        m_nColorType = (m_nColorType+1)%5;
        m_pButtonColor->setFocused( m_nColorType != 0 );
        isUpdated = true;
    }
    // [PATH]ボタン
    else if( m_pButtonPath->isTapped() ){
        m_bDispPath = !m_bDispPath;
        m_pButtonPath->setFocused( m_bDispPath );
        isUpdated = true;
    }
    // [FLIP-H]ボタン
    else if( m_pButtonFlipH->isTapped() ){
        m_bFlipH = !m_bFlipH;
        m_pButtonFlipH->setFocused( m_bFlipH );
        isUpdated = true;
    }
    // [FLIP-V]ボタン
    else if( m_pButtonFlipV->isTapped() ){
        m_bFlipV = !m_bFlipV;
        m_pButtonFlipV->setFocused( m_bFlipV );
        isUpdated = true;
    }
    // [GROUND]ボタン
    else if( m_pButtonGround->isTapped() ){
        m_bGround = !m_bGround;
        m_pButtonGround->setFocused( m_bGround );
        isUpdated = true;
    }
    // [ANIM]ボタン
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

    fixButtonAnim();
}

//------------------------
// 描画
//------------------------
void CBezierTestLoop::onDraw0( void ){
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
    if( m_bDispBg ){
        // 実サイズ
        x = SA_XC - w/2.0f;
        y = SA_YM - h/2.0f;
        
        DWORD col;
#ifdef ENABLE_SS
        if( m_bDumpSS ){
            col = (m_bGround)? 0xFFFFFFA0: 0xFFFFFF60;
        }else
#endif
        {
            col = (m_bGround)? 0xFFFFFF80: 0xFFFFFF40;
        }

        pDC->clear();
        pDC->setRect( w, h );
        pDC->setRGBA( col );
        pDC->draw( x, y );

        if( !m_bGround ){
            // 名目サイズ
            x = SA_XC - w0/2.0f;
            y = SA_YM - h0/2.0f;

            pDC->clear();
            pDC->setRect( w0, h0 );
            pDC->setRGBA( 0xFFFFFF80 );
            pDC->draw( x, y );
        }
    }

    // 設置指定があれば調整
    if( m_bGround ){
        x = SA_XC - m_pTexForLine->getBaseX()/pixelRate;
        y = SA_YM - m_pTexForLine->getBaseY()/pixelRate + (5*h0/8);
    }else{
        x = SA_XC - w/2.0f;
        y = SA_YM - h/2.0f;
    }
    
    // [P8D&P] ベジェ：塗り
    if( m_nColorType != 0 ){
        pDC->clear();
        pDC->setTex( m_pTexForColor, CImgMgr::GetImgPal( (eIMG)(eIMG_PAL_DOT_MIKU+(m_nColorType-1) ) ) );
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->drawF( x, y );
    }
    
    // [GRAY] ベジェ：線
    if( m_nLineType != 0 ){
        pDC->clear();
        pDC->setTex( m_pTexForLine, NULL );
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->setBlendAlpha();
        pDC->setRGBA( 0x000000FF );
        pDC->drawF( x, y );

        if( !m_bGround ){
            // カーソル表示：基本座標
            float groundX = SA_XC - w/2 + m_pTexForLine->getBaseX()/pixelRate;
            float groundY = SA_YM - h/2 + m_pTexForLine->getBaseY()/pixelRate;

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
            float focusX = SA_XC - w/2 + m_pTexForLine->getFocusX()/pixelRate;
            float focusY = SA_YM - h/2 + m_pTexForLine->getFocusY()/pixelRate;

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
    if( m_bDispPath ){
        pDC->clear();
        pDC->setTex( m_pTexForPath, CImgMgr::GetImgPal( eIMG_PAL_WORK_PATH ) );
        pDC->setWidth( w );
        pDC->setHeight( h );
        pDC->drawF( x, y );
    }
    
    // 情報
    if( !m_bGround ){
        // ログ（名目上のサイズの左上）
        sprintf( buf, "B:%d-%d F:%d-%d",
                 (int)m_pTexForLine->getBaseX(), (int)m_pTexForLine->getBaseY(),
                 (int)m_pTexForLine->getFocusX(), (int)m_pTexForLine->getFocusY()
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
    m_pTexForLine = new CTex();
    m_pTexForColor = new CTex();
    m_pTexForPath = new CTex();
    CMemMgr::PopTargetField();

#ifdef LAYER_BASE
    // 基本テストであれば利用優先順に確保
    bool isDone = false;
    
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

    #ifdef LAYER_BASE_FOR_FRILL
    if( ! isDone ){
        // フリルの場合はデータの参照を受け取る（※このデータは解放しない）
        m_pLayerFrill = CBezier::GetLayerDataForFrill( LAYER_BASE_FOR_FRILL_TARGET );
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
    SAFE_DELETE( m_pTexForLine );
    SAFE_DELETE( m_pTexForColor );
    SAFE_DELETE( m_pTexForPath );
    
    // レイヤー解放
    CLayerData::Free( m_pLayerBase );       m_pLayerBase = NULL;

    m_pLayerFrill = NULL;   // フリル素材は[CBezier]クラスで管理されているデータへの参照なので解放しない
    
    CLayerData::Free( m_pLayerDelay );      m_pLayerDelay = NULL;

    CLayerData::Free( m_pLayerHookA );      m_pLayerHookA = NULL;
    CLayerData::Free( m_pLayerHookB );      m_pLayerHookB = NULL;
    CLayerData::Free( m_pLayerHookC );      m_pLayerHookC = NULL;
    
    CLayerData::Free( m_pLayerLineRepair ); m_pLayerLineRepair = NULL;
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
    if( m_pTexForLine != NULL ){
        m_pTexForLine->release();
    }

    if( m_pTexForColor != NULL ){
        m_pTexForColor->release();
    }
    
    if( m_pTexForPath != NULL ){
        m_pTexForPath->release();
    }

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
    
    // 表示更新
    m_pButtonAnimPull->setDark( ! m_bAnimOn );
    m_pButtonAnimPull->setActive( m_bAnimOn );
    
    // 設定はデフォルトに
    m_oSetting.setDefault();
    
    fixButtonAnim();
}

//------------------------
// ブラシ強制
//------------------------
void CBezierTestLoop::fixBrush( int type ){
    if( type == 0 ){
        CBezierDataConv::SetFixedBrush( eBRUSH_INVALID );
    }else{
        CBezierDataConv::SetFixedBrush( (eBRUSH)(eBRUSH_C_BASE + type) );
    }
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

//-------------------------
// ベジェ確認：アニメボタン確定
//-------------------------
void CBezierTestLoop::fixButtonAnim( void ){
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
// ベジェ確認：BMP作成
//----------------------
void CBezierTestLoop::createBmp( void ){
    //----------------------------------
    // BMP作成パラメータ準備
    //----------------------------------
    stBMP_GEN_CREATE_PARAM createParam;
    CLEAR_BMP_GEN_CREATE_PARAM( &createParam );

    createParam.isFlipH = m_bFlipH;
    createParam.isFlipV = m_bFlipV;
    createParam.scale = m_fDispScale * BMP_GEN_BASE_SCALE;
    createParam.rot = m_fDispRotation;
        
    // オプション
    for( int i=0; i<eBD_OPTION_MAX; i++ ){
        //createParam.arrOption[i] = true;
    }

    createParam.adjustRateH = m_fAdjustRateH;
    createParam.adjustRateV = m_fAdjustRateV;
    createParam.adjustRateLR = m_fAdjustRateLR;
    createParam.adjustRateUD = m_fAdjustRateUD;
    createParam.pDelayLog = m_pDelayLog;
    
#ifdef LAYER_BASE
    createBmpWithLayerData( &createParam );
#else
    createBmpWithBmpDot( &createParam );
#endif
}

//----------------------------------
// レイヤーデータによるBMP作成
//----------------------------------
void CBezierTestLoop::createBmpWithLayerData( stBMP_GEN_CREATE_PARAM* pCreateParam ){
    CList list;
    
    //-----------------------------------
    // 各レイヤーデータが有効であればリストへ登録
    //-----------------------------------
    // 線修復
    if( m_pLayerLineRepair != NULL ){ list.add( m_pLayerLineRepair ); }
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
    // フリル
    else if( m_pLayerFrill != NULL ){ list.add( m_pLayerFrill ); }
    // 基本
    else if( m_pLayerBase != NULL ){ list.add( m_pLayerBase ); }
    
    // 描画（※作業パス＆実画像）
    CBmpGenerator::CreateTexWithLayerList( m_pTexForPath, NULL, &list, pCreateParam, true );
    CBmpGenerator::CreateTexWithLayerList( m_pTexForLine, m_pTexForColor, &list, pCreateParam, false );

    // リスト切断（※[CList]が破棄される際はなにもしないので明示的に切断）
    list.disconnectAll();
}

//----------------------------------
// BDPDによるBMP作成
//----------------------------------
void CBezierTestLoop::createBmpWithBmpDot( stBMP_GEN_CREATE_PARAM* pCreateParam ){
    // 表示対象
    eBD_SLOT slot = eBD_SLOT_FgTop;

    // 生成パラメータ確保
    CBmpGenParam* pBmpGenParam = CBmpGenerator::CreateBmpGenParam( &m_oSetting,
                                                                   0, 0, -1, (eBD_EMO)0, (eBD_FORM)0,
                                                                   m_fAdjustRateT, m_fAdjustRateS );
    
    // 描画（※作業パス＆実画像）
    CBmpGenerator::CreateTexWithGenParam( m_pTexForPath, NULL, pBmpGenParam, slot, 0, 0, pCreateParam, true, false );
    CBmpGenerator::CreateTexWithGenParam( m_pTexForLine, m_pTexForColor, pBmpGenParam, slot, 0, 0, pCreateParam, false, false );
    
    // 生成パラメータ解放
    CBmpGenerator::ReleaseBmpGenParam( pBmpGenParam );
}
