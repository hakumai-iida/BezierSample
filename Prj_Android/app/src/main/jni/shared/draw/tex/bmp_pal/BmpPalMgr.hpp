/*+----------------------------------------------------------------+
  |	Title:		BmpPalMgr.hpp [共通環境]
  |	Comment:	BMPパレット管理（※ファイル単位で管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_PAL_MGR_HPP__
#define __BMP_PAL_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpPalData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// BMPパレット管理
//----------------------------------------
class CBmpPalMgr{
private:
    //-----------------------------------
    // BPデータ
    //-----------------------------------
    static const char* s_pArrBmpPalDataName[];  // 管理ファイル一覧
    static int s_nNumData;                      // データ数（※必ず[GetBmpPalDataNum]経由で参照すること）
    static CBmpPalData* s_oArrBmpPalData;       // [bpd]データ配列

public:
    // 領域確保情報（※端末の解像度によってサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSizeK( void );

	// 開始＆終了
    static bool OnCreate( void );
    static void OnDestroy( void );
    
    // データ取得
    static int GetBmpPalDataNum( void );
    inline static const char** GetBmpPalDataNameArr( void ){ return( s_pArrBmpPalDataName ); }
    static const char* GetBmpPalDataNameAt( int at );

    // データ取得：アリス／システムキャラ
    static CBmpPalData* GetBmpPalDataAt( int at );
    static CBmpPalData* SearchBmpPalData( eBP_CATEGORY category, int uid, int subId, bool useDefaultIfNotFound=true );
    
    // ファイル[I/O]関連
    static bool LoadBpdAt( int at, const char* pName );
    static bool SaveBpdAt( int at, const char* pName, bool isTemporary=false );
    static bool SaveBpdAll( void );
    
    // BMP経由の読み書き
    static bool LoadBpdAtAsBmp( int at, const char* pName );
    static bool SaveBpdAtAsBmp( int at, const char* pName );

private:
	static bool CreateAll( void );

	// インスタンス作成は不可
	CBmpPalMgr( void ){}
	virtual ~CBmpPalMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_PAL_MGR_HPP__
