/*+----------------------------------------------------------------+
  |	Title:		SlotAdjuster.hpp [共通環境]
  |	Comment:	スロット調整（※外部指定のデータによりスロットを調整）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __SLOT_ADJUSTER_HPP__
#define __SLOT_ADJUSTER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/data/DataCell.hpp"

#include "JointPoint.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//==============================================================================
// スロット調整定義（※[CSlotAdjuster]をメンバーとしてもたせたいクラス定義内に記載）
//==============================================================================
#define IMPLEMENT_SLOT_ADJUSTER                                                 \
    protected:                                                                  \
        CSlotAdjuster m_oSlotAdj;                                               \
    public:                                                                     \
        inline CSlotAdjuster* getSlotAdjuster( void ){ return( &m_oSlotAdj ); }

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// スロット調整調整
//----------------------------------------
class CSlotAdjuster{
    // 各種実装
    IMPLEMENT_DATA( CSlotAdjuster )         // 実装：データのクリア／コピー／読み書き
    IMPLEMENT_DATA_EDIT()                   // 実装：データ編集

protected:
	//--------------------------------------------------------------------
	// 保存データ（※簡便のため[int]で宣言するが入出力時は値域に見合った型で処理される）
	//--------------------------------------------------------------------
    // テンション関連
    int m_nMinRateForT;                     // [2]: テンション調整：最小オフセット（※角度を下げる指定）
    int m_nMaxRateForT;                     // [2]: テンション調整：最大オフセット（※角度をあげる指定）

    // サイズ関連
    int m_nMinRateForS;                     // [2]: サイズ調整：最小オフセット（※小さくする指定）
    int m_nMaxRateForS;                     // [2]: サイズ調整：最大オフセット（※大きくする指定）
    
    //----------------------------------------------------------------------
    // ジョイント関連（※スロットの中心を基準に、接続元の２点と接続先の２点をつなげる仕組み）
    // 半径が腕や足の太さに相当する値なのでＨ調整値に影響される（※Ｖ調整値には影響されない）
    //----------------------------------------------------------------------
    // 座標登録先（※IDが有効な場合に処理される）
    eJOINT_POINT m_eJointPointIdForFrom;    // [enum]: 接続元（呼び出し元）に対する座標登録ID
    eJOINT_POINT m_eJointPointIdForTo;      // [enum]: 接続先（呼び出し先）に対する座標登録ID
    
    int m_nJointRateRotR;                   // [2]: 可動半径（※接続する二点間の幅＝腕や足の太さに相当）
    int m_nJointMinRateOfsRForH;            // [2]: ヨコ調整：最小オフセット（※半径を細くする指定）
    int m_nJointMaxRateOfsRForH;            // [2]: ヨコ調整：最大オフセット（※半径を太くする指定）
    int m_nJointRateRotOfs;                 // [2]: 基本角度（この角度を中心に前後９０度の二点を[TargetIdForFrom/To]へ登録する）
    int m_nJointRateOfsForRotCenter;        // [2]: 回転時にずらす中心割合オフセット（※円の中心から見て、[+]で上、[-]で下に中心がずれる）
    
    // プラステンション[adjT > 0.0f] 向けの調整値
    int m_nJointOpenFromRateDirX_p;         // [2]: 方向線調整：二点間が開いた際の呼び出し元への方向線X
    int m_nJointOpenFromRateDirY_p;         // [2]: 方向線調整：二点間が開いた際の呼び出し元への方向線Y
    int m_nJointCloseFromRateDirX_p;        // [2]: 方向線調整：二点間が閉じた際の呼び出し元への方向線X
    int m_nJointCloseFromRateDirY_p;        // [2]: 方向線調整：二点間が閉じた際の呼び出し元への方向線Y
    int m_nJointOpenToRateDirX_p;           // [2]: 方向線調整：二点間が開いた際の呼び出し先への方向線X
    int m_nJointOpenToRateDirY_p;           // [2]: 方向線調整：二点間が開いた際の呼び出し先への方向線Y
    int m_nJointCloseToRateDirX_p;          // [2]: 方向線調整：二点間が閉じた際の呼び出し先への方向線X
    int m_nJointCloseToRateDirY_p;          // [2]: 方向線調整：二点間が閉じた際の呼び出し先への方向線Y

    // マイナステンション[adjT < 0.0f] 向けの調整値
    int m_nJointOpenFromRateDirX_m;         // [2]: 方向線調整：二点間が開いた際の呼び出し元への方向線X
    int m_nJointOpenFromRateDirY_m;         // [2]: 方向線調整：二点間が開いた際の呼び出し元への方向線Y
    int m_nJointCloseFromRateDirX_m;        // [2]: 方向線調整：二点間が閉じた際の呼び出し元への方向線X
    int m_nJointCloseFromRateDirY_m;        // [2]: 方向線調整：二点間が閉じた際の呼び出し元への方向線Y
    int m_nJointOpenToRateDirX_m;           // [2]: 方向線調整：二点間が開いた際の呼び出し先への方向線X
    int m_nJointOpenToRateDirY_m;           // [2]: 方向線調整：二点間が開いた際の呼び出し先への方向線Y
    int m_nJointCloseToRateDirX_m;          // [2]: 方向線調整：二点間が閉じた際の呼び出し先への方向線X
    int m_nJointCloseToRateDirY_m;          // [2]: 方向線調整：二点間が閉じた際の呼び出し先への方向線Y

    //-----------------------------------------------------------------------------------------
    // カバー調整値
    // この値は着ぐるみから出る[顔(頭)、腕、足]のつなぎ目を誤魔化すためのカバー(ハリボテの穴)の調整値となる
    // 値を設定する対象はジョイントパーツとなる（※腕であれば[UpJointSholder]が対象＝[AmBase]ではないので注意）
    //-----------------------------------------------------------------------------------------
    int m_nCoverRateOfsX;                   // [2]:カバーのオフセットX
    int m_nCoverRateOfsY;                   // [2]:カバーのオフセットY
    int m_nCoverRateScaleX;                 // [2]:カバーに対するスケールX
    int m_nCoverRateScaleY;                 // [2]:カバーに対するスケールY
    int m_nCoverRateRot;                    // [2]:カバーに対する回転

public:
    // コンストラクタ／デストラクタ
    CSlotAdjuster( void );
    virtual ~CSlotAdjuster( void ){}

    //---------------------------------------------------------------------------------------
    // 設定（※レイヤーデータから直接利用しないので設定はもたない＝[CBezierTestLoop]で直接操作する需要がない）
    //---------------------------------------------------------------------------------------

    //----------------
	// 取得
	//----------------
    // for T
    inline int getMinRateForT( void ){ return( m_nMinRateForT ); }
    inline int getMaxRateForT( void ){ return( m_nMaxRateForT ); }

    // for S
    inline int getMinRateForS( void ){ return( m_nMinRateForS ); }
    inline int getMaxRateForS( void ){ return( m_nMaxRateForS ); }

    // joint
    int fixJointPointIdForFrom( int slotIndex );
    int fixJointPointIdForTo( int slotIndex );

    inline int getJointRateRotR( void ){ return( m_nJointRateRotR ); }
    inline int getJointMinRateOfsRForH( void ){ return( m_nJointMinRateOfsRForH ); }
    inline int getJointMaxRateOfsRForH( void ){ return( m_nJointMaxRateOfsRForH ); }
    inline int getJointRateRotOfs( void ){ return( m_nJointRateRotOfs ); }
    inline int getJointRateOfsForRotCenter( void ){ return( m_nJointRateOfsForRotCenter ); }

    inline float getJointOpenFromScaleDirX( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointOpenFromRateDirX_m: m_nJointOpenFromRateDirX_p ) );
    }
    
    inline float getJointOpenFromScaleDirY( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointOpenFromRateDirY_m: m_nJointOpenFromRateDirY_p ) );
    }
    
    inline float getJointCloseFromScaleDirX( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointCloseFromRateDirX_m: m_nJointCloseFromRateDirX_p ) );
    }
    
    inline float getJointCloseFromScaleDirY( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointCloseFromRateDirY_m: m_nJointCloseFromRateDirY_p ) );
    }
    
    inline float getJointOpenToScaleDirX( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointOpenToRateDirX_m: m_nJointOpenToRateDirX_p ) );
    }
    
    inline float getJointOpenToScaleDirY( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointOpenToRateDirY_m: m_nJointOpenToRateDirY_p ) );
    }
    
    inline float getJointCloseToScaleDirX( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointCloseToRateDirX_m: m_nJointCloseToRateDirX_p ) );
    }
    
    inline float getJointCloseToScaleDirY( bool isMinus ){
        return( CConst::ConvBezierScaleRate( (isMinus)? m_nJointCloseToRateDirY_m: m_nJointCloseToRateDirY_p ) );
    }
    
    inline int  getCoverRateOfsX( void ){ return( m_nCoverRateOfsX ); }
    inline int  getCoverRateOfsY( void ){ return( m_nCoverRateOfsY ); }

    inline int  getCoverRateScaleX( void ){ return( m_nCoverRateScaleX ); }
    inline float getCoverScaleX( void ){ return( CConst::ConvBezierScaleRate( m_nCoverRateScaleX ) ); }

    inline int  getCoverRateScaleY( void ){ return( m_nCoverRateScaleY ); }
    inline float getCoverScaleY( void ){ return( CConst::ConvBezierScaleRate( m_nCoverRateScaleY ) ); }

    inline int  getCoverRateRot( void ){ return( m_nCoverRateRot ); }
    inline float getCoverRot( void ){ return( CConst::ConvBezierRotationRate( m_nCoverRateRot ) ); }

    //------------
    // スケール適用
    //------------
    void applyRateScale( int rateScale );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __SLOT_ADJUSTER_HPP__
