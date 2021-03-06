/*+----------------------------------------------------------------+
  |	Title:		LoopReserve.hpp [共通環境]
  |	Comment:	ループ予約
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOOP_RESERVE_HPP__
#define __LOOP_RESERVE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Loop.hpp"

#include "sys/data/ListNode.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ループ予約タイプ
enum eLOOP_RESERVE{
	eLOOP_RESERVE_INVALID = -1,

	//-------------------------------------------
	// 1.呼び出し元の処理を終了(onExit)
	//-------------------------------------------
	eLOOP_RESERVE_EXIT,

	//-------------------------------------------
	// 1.指定されたIDのループを作成(onInit)
	//-------------------------------------------
	eLOOP_RESERVE_CREATE,

	//-------------------------------------------
	// 1.呼び出し元の処理を終了(onExit)
	// 2.指定されたIDのループを作成(onInit)
	//-------------------------------------------
	eLOOP_RESERVE_JUMP,

	//-------------------------------------------
	// 1.呼び出し元の処理を停止(onSuspend)
	// 2.指定されたIDのループを作成(onInit)
	// 3.呼び出された側が終了したら再開(onResume)
	//-------------------------------------------
	eLOOP_RESERVE_CALL,
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// ループ予約要素
//----------------------------------------
class CLoopReserve: public CListNode{
    // 実装
    IMPLEMENT_NODE( CLoopReserve )

protected:
	CLoop* m_pLoop;					// 予約を発行したループ（※指定ループが作成される際にスリープする）
	eLOOP_RESERVE m_eReserveType;	// 予約タイプ
	int m_nLoopId;					// 作成ループID
	int m_nOption;					// 作成オプション

public:
    // コンストラクタ／デストラクタ
	CLoopReserve( void );
	virtual ~CLoopReserve( void ){}

    // クリア
	void clear( void );

	void set( CLoop* pLoop, eLOOP_RESERVE reserveType, int loopId, int option );

	bool isValid( void );

	inline CLoop* getLoop( void ){ return( m_pLoop); }
	inline eLOOP_RESERVE getReserveType( void ){ return( m_eReserveType ); }
	inline int getLoopId( void ){ return( m_nLoopId ); }
	inline int getOption( void ){ return( m_nOption ); }
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __LOOP_RESERVE_HPP__
