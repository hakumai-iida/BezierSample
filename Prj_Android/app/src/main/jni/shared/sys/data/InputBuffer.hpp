/*+----------------------------------------------------------------+
  |	Title:		InputBuffer.hpp [共通環境]
  |	Comment:	入力バッファ
  |				[COutputBuffer]と対になる
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __INPUT_BUFFER_HPP__
#define __INPUT_BUFFER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 入力バッファ
//----------------------------------------
class CInputBuffer{
protected:
	// ワーク
	BYTE* m_pBuf;		// 読み込み対象バッファ
	int m_nSize;		// バッファサイズ
	int m_nAt;			// 現在の読み込み位置

	// エンディアン調整フラグ
	bool m_bAdjustEndian;

public:
    // コンストラクタ／デストラクタ
	CInputBuffer( void );
	virtual ~CInputBuffer( void ){}
	
	// クリア
	void clear( void );

	// バッファ指定（※内部で[clear]が呼ばれている）
	void setBuffer( void* pBuf, int size );
	
	// エンディアン指定（※バッファ指定の後に呼ぶこと）
	void setForLittleEndian( void );
	void setForBigEndian( void );

	// 読み込み位置調整
	bool seek( int at );

	// 読み飛ばし
	bool skip( int size );

	// 取得
	inline int getCurPos( void ){ return( m_nAt ); }

	// 残りサイズ取得（※読み込み前に確認をしたい場合）
	inline int getRest( void ){ return( m_nSize - m_nAt ); }

	// 読み込み
	int8   readInt8( void );
	uint8  readUint8( void );
	int16  readInt16( void );
	uint16 readUint16( void );
	int32  readInt32( void );
	uint32 readUint32( void );
	int64  readInt64( void );
	uint64 readUint64( void );
	float  readFloat( void );
	double readDouble( void );
	bool   readBlock( void* pDest, int len );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __INPUT_BUFFER_HPP__ */
