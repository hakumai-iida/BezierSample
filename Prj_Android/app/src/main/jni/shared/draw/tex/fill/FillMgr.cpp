/*+----------------------------------------------------------------+
  |	Title:		FillMgr.cpp [共通環境]
  |	Comment:	塗り管理（形骸化）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FillMgr.hpp"

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
CFill CFillMgr::s_oCurFill;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CFillMgr::GetAllocCellNum( void ){
    int num = 0;
    
    // 特に無し
    
    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CFillMgr::GetAllocSizeK( void ){
    int size = 0;

    // 特に無し

    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//------------------------
// 初期化
//------------------------
bool CFillMgr::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFillMgr::OnCreate()\n" );
#endif

	return( true );
}

//------------------------
// 終了
//------------------------
void CFillMgr::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CFillMgr::OnDestroy()\n" );
#endif
}

//--------------------------
// 塗り取得
//--------------------------
CFill* CFillMgr::GetFill( eFILL_TYPE type ){
    if( s_oCurFill.set( type ) ){
        return( &s_oCurFill );
    }
    
    return( NULL );
}
