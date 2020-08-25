/*+----------------------------------------------------------------+
  |	Title:		DebugMenuLoop.hpp [共通環境]
  |	Comment:	デバッグメニューループ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "DebugMenuLoop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ステップ
enum STEP{
	eSTEP_SELECT,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//---------------------------------
// ラベル：ループ名
//---------------------------------
static const char* _pArrMenu[] = {
    //"Bezier: Check Stroke",
    "Bezier: Check Brush",
    "Bezier: Test",

	NULL,
};

//--------------------------
// 飛び先：ループID
//--------------------------
static int _nArrLoopId[] = {
    //eLOOP_ID_STROKE_CHECK,
    eLOOP_ID_BRUSH_CHECK,
    eLOOP_ID_BEZIER_TEST,
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
CDebugMenuLoop::CDebugMenuLoop( void ){
	setId( eLOOP_ID_DEBUG_MENU );

    setDrawLayer( eDP_LAYER_LOOP );
}

//------------------------
// デストラクタ
//------------------------
CDebugMenuLoop::~CDebugMenuLoop( void ){
	SAFE_DELETE( m_pMenu );
}

//------------------------
// 初期化
//------------------------
void CDebugMenuLoop::init0( void ){
	// 復帰処理を読んでおく
	awake0();

	//-----------------------
    // レイアウト：左上
    //-----------------------
	setLayoutLT( 0, 32, eLAYOUT_DIR_N, eLAYOUT_DIR_N );

    // 文字列メニュー
    m_pMenu = new CStrMenu( eGRP_DEBUG_WINDOW, 256, 288 );
	m_pMenu->setTitle( "DEBUG MENU" );
	m_pMenu->setItemArrayThenFix( _pArrMenu );
    m_pMenu->setSelect( m_pMenu->getItemNum()-1 );	// 初期選択位置指定
    putNodeOnLayout( m_pMenu );

    // ノード追加
    addNode( m_pMenu );
}

//------------------------
// 終了
//------------------------
void CDebugMenuLoop::exit0( void ){
}

//------------------------
// 休眠（※別のループへ）
//------------------------
void CDebugMenuLoop::sleep0( void ){
    // デバッグ無効
    CAppMgr::SetDebug( false );
    CAppMgr::SetDebugLog( true, false );
}

//------------------------
// 復帰（※休眠からの復旧）
//------------------------
void CDebugMenuLoop::awake0( void ){
	// 背景設定
	CBgMgr::SetBg( eBG_GRID );

	// デバッグ有効
	CAppMgr::SetDebug( true );
    CAppMgr::SetDebugLog( true, true );
}

//------------------------
// 更新
//------------------------
void CDebugMenuLoop::update0( void ){
	if( isStepChanged() ){
		exitNodeAll();

		switch( m_nStep ){
		case eSTEP_SELECT:
			m_pMenu->enter();
			break;
		}

		fixStep();
	}else{
		switch( m_nStep ){
		case eSTEP_SELECT:
			if( m_pMenu->isDecided() ){
				int select = m_pMenu->getSelect();
				CLoopMgr::ReserveLoop( this, eLOOP_RESERVE_CALL, _nArrLoopId[select], 0 );
			}
			break;
		}

		if( isStepChanged() ){
			setActiveNodeAll( false );
		}
	}
}
