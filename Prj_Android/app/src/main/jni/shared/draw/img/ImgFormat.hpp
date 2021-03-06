/*+----------------------------------------------------------------+
  |	Title:		ImgFormat.hpp [共通環境]
  |	Comment:	画像フォーマット
  |				[bmp2hif.exe/img2hif.exe]の実装に依存
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __IMG_FORMAT_HPP__
#define __IMG_FORMAT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------
// [HIF] 画像データタイプ
//-----------------------------------
enum eHIF_IMG_TYPE{
	eHIF_IMG_TYPE_INVALID = 0x00,	// 無効値

	eHIF_IMG_TYPE_RGBA	= 0x10,		// フルカラー：１画素４バイト
	eHIF_IMG_TYPE_RGB	= 0x20,		// フルカラー（α無し）：１画素３バイト
	eHIF_IMG_TYPE_GRAY	= 0x30,		// グレースケール：１画素１バイト
	eHIF_IMG_TYPE_P8P	= 0x40,		// ２５６色パレット：パレット部：１画素４バイト
	eHIF_IMG_TYPE_P8D	= 0x50,		// ２５６色パレット：ドット部：１画素１バイト
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// [HIF] ヘッダ
//-----------------------------------------------------------------
// [.hif]ファイルヘッダは下記のデータで格納される
// BYTE   signature[3];	// [00]:署名：文字"HIF"固定
// BYTE   version;		// [03]:バージョン：文字"0"固定
// WORD   numBlock;		// [04]:ブロック数
// WORD   _reserved;	// [06]:予約（０埋め）
// DWORD* arrBlockOfs;	// [08]:配列：各ブロックへのオフセット
//-----------------------------------------------------------------
struct stHIF_HEADER{
	bool	isValid;		// 有効フラグ（※署名＆バージョン判定結果）
	BYTE*	pSrc;			// ソースバッファ

	// ブロック管理情報
	int		numBlock;		// ブロック数
	BYTE*	pBlockOfs;		// ブロックデータオフセット配列ポインタ（※値の参照時は都度解釈）
};

#define HIF_HEADER_BIN_SIZE 8

//-----------------------------------------------------------------
// [HIF] ブロック情報
//-----------------------------------------------------------------
// [.hif]ファイル内の各ブロックは下記のデータで格納される
// BYTE  type;			// [ 0]:画像種類(eHIF_IMG_TYPE)
// BYTE  _reserved;		// [ 1]:予約
// WORD  delay;			// [ 2]:表示時間(アニメGIF用)
// DWORD sizeBuf;		// [ 4]]バッファサイズ
// WORD  width;			// [ 8]:画像の横幅（余白をカットした実際の値）
// WORD  height;		// [10]:画像の縦幅（余白をカットした実際の値）
// WORD  ofsLeft;		// [12]:表示オフセット左（左カット値）
// WORD  ofsTop;		// [14]:表示オフセット上（上カット値）
// WORD  ofsRight;		// [16]:表示オフセット右（右カット値）
// WORD  ofsBottom;		// [18]:表示オフセット下（下カット値）
// WORD  baseX;			// [20]:基点X座標
// WORD  baseY;			// [22]:基点Y座標
// BYTE* pBuf;			// [24] バッファ
//-----------------------------------------------------------------
struct stHIF_BLOCK_INFO{
	eHIF_IMG_TYPE type;	// 画像種類
	int delay;			// 表示時間
	DWORD size;			// サイズ
	int width;			// 画像の横幅（余白をカットした実際の値）
	int height;			// 画像の縦幅（余白をカットした実際の値）
	int ofsLeft;		// 表示オフセット左（左カット値）
	int ofsTop;			// 表示オフセット上（上カット値）
	int ofsRight;		// 表示オフセット右（右カット値）
	int ofsBottom;		// 表示オフセット下（下カット値）
	int baseX;			// 基点X座標
	int baseY;			// 基点Y座標
	BYTE* pBuf;			// データ
};

#define HIF_BLOCK_INFO_BIN_SIZE 24

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------
// フォーマット定数
//----------------------------
class CImgFormat{

public:
	// [HIF] ヘッダ確認＆取得
	static bool CheckHifHeaderThenSet( stHIF_HEADER* pHeader, BYTE* pBuf );

	// [HIF_BLOCK_INFO]取得
	static void GetHifBlockInfoFromBuf( stHIF_BLOCK_INFO* pInfo, BYTE* pBuf );
    
    // [HIF_BLOCK_INFO]設定
    static void SetHifBlockInfoForBuf( stHIF_BLOCK_INFO* pInfo, BYTE* pBuf );

	// [HIF] ブロック情報取り出し
	static bool GetHifBlockInfoFromHifHeaderAt( stHIF_BLOCK_INFO* pInfo, stHIF_HEADER* pHeader, int at );

private:
	// インスタンス生成は不可
	CImgFormat( void ){}
	virtual ~CImgFormat( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __IMG_FORMAT_HPP__ */
