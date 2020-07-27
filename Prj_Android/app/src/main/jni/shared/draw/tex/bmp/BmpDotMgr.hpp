/*+----------------------------------------------------------------+
  |	Title:		BmpDotMgr.hpp [共通環境]
  |	Comment:	BMP画素管理（※ファイル単位で管理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BMP_DOT_MGR_HPP__
#define __BMP_DOT_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotPack.hpp"
#include "BmpDotSettingData.hpp"

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
// BMP画素管理
//----------------------------------------
class CBmpDotMgr{
private:
    //-----------------------------------
    // BDパック
    //-----------------------------------
    static const char* s_pArrBmpDotPackName[];  // 管理ファイル一覧
    static int s_nNumPack;                      // パックデータ数（※必ず[GetBmpDotPackNum]経由で参照すること）
    static CBmpDotPack* s_oArrBmpDotPack;       // [bdp]データ配列

    //-----------------------------------
    // 設定ファイル（※主に作業用）
    //-----------------------------------
    static const char* s_pBmpDotSettingFileNameForWork;
    static CBmpDotSettingData* s_pBmpDotSettingDataForWork;

public:
    // 領域確保情報（※端末の解像度によってサイズが変動する）
    static int GetAllocCellNum( void );
    static int GetAllocSizeK( void );

	// 開始＆終了
    static bool OnCreate( void );
    static void OnDestroy( void );
    
    // データ取得
    static int GetBmpDotPackNum( void );
    inline static const char** GetBmpDotPackNameArr( void ){ return( s_pArrBmpDotPackName ); }
    static const char* GetBmpDotPackNameAt( int at );

    // データ取得：アリス／システムキャラ
    static CBmpDotPack* GetBmpDotPackAt( int at );
    static CBmpDotData* SearchBmpDotData( stBD_PART_SEARCH_PARAM* pParam );
    static CBmpDotPartData* SearchBmpDotPartData( stBD_PART_SEARCH_PARAM* pParam );
    static CBmpDotPartData* SearchBmpDotPartDataWithDefault( stBD_PART_SEARCH_PARAM* pParam, stBD_PART_SEARCH_PARAM* pParamOut=NULL );
    
    // ファイル[I/O]関連
    static bool LoadBdpAt( int at, const char* pName );
    static bool SaveBdpAt( int at, const char* pName, bool isTemporary=false );
    static bool SaveBdpAll( void );

    // 設定関連
    inline static const char* GetBmpDotSettingFileNameForWork( void ){ return( s_pBmpDotSettingFileNameForWork ); }
    inline static CBmpDotSettingData* GetBmpDotSettingForWork( void ){ return( s_pBmpDotSettingDataForWork ); }
    static bool LoadBmpDotSettingForWork( const char* pName );
    static bool SaveBmpDotSettingForWork( const char* pName, bool isTemporary=false );
    
private:
	static bool CreateAll( void );

	// インスタンス作成は不可
	CBmpDotMgr( void ){}
	virtual ~CBmpDotMgr( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __BMP_DOT_MGR_HPP__
