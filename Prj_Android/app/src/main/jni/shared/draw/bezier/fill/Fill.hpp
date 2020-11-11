/*+----------------------------------------------------------------+
  |	Title:		Fill.hpp [共通環境]
  |	Comment:	塗り（※対象はパレット画像）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILL_HPP__
#define __FILL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "FillConst.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
 +----------------------------------------------------------------+*/
// 下塗りセルサイズ（※とりあえず）
#define FILL_UNDERCOAT_CELL_MAX  4096

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//----------------------------
// 下塗り処理単位
//----------------------------
struct stFILL_UNDERCOAT_CELL{
    int hint;   // 処理ヒント
    int xL, xR; // X座標
    int y;      // Y座標
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 塗り
//-----------------------------------------------------------------
class CFill{
public:
    // ガイド調整
    static void ResetFillGuideForLastCol( BYTE* pBufGuide, int w, int h );
    static void AdjustFillGuideForOutCol( BYTE* pBufGuide, int w, int h );

protected:
    //-------------------------------------------
    // 設定値
    //-------------------------------------------
    eFILL_TYPE m_eType;         // タイプ
    
    //-------------------------------------------
    // 塗りつぶし対象
    //-------------------------------------------
    BYTE* m_pBuf;               // 塗りつぶしバッファ（※実際の出力先）
    BYTE* m_pBufGuide;          // ガイドバッファ（※指定座標を内側にもつ閉じた領域が渡される想定）
    BYTE* m_pBufGuard;          // ガードバッファ
    int m_nDotW, m_nDotH;       // バッファサイズ

    //-------------------------------------------
    // 下塗りをした左右上下端（※本塗りの処理範囲）
    //-------------------------------------------
    int m_nUndercoatL, m_nUndercoatR;
    int m_nUndercoatT, m_nUndercoatB;

    //---------------------------------------------------
    // 下塗りワーク（※循環管理＝[Head]と[Tail]による追いかけっこ）
    //---------------------------------------------------
    stFILL_UNDERCOAT_CELL* m_stArrUndercoatCell;
    int m_nUndercoatCellHeadAt;
    int m_nUndercoatCellTailAt;
    
public:
    // コンストラクタ／デストラクタ
    CFill( void );
    virtual ~CFill( void );
    
    // クリア
    void clear( void );

    //---------------------------------------------------
    // 塗り設定
    //---------------------------------------------------
    bool set( eFILL_TYPE type );
    
    // 判定
    inline bool isValid( void ){ return( m_eType != eFILL_TYPE_INVALID ); }
    
    // 取得
    inline eFILL_TYPE getType( void ){ return( m_eType ); }

    //--------------------------------------------------------------------------------------------------
    // 塗りつぶし（※フラグにより[広域／全色]処理へ切り替え＆[addVal]の値で加算／減算の切り替え）
    //--------------------------------------------------------------------------------------------------
    // ガイドバッファ[pBufGuide]を[x,y]を始点に下塗り色で塗りつぶす
    // その後、下塗りの結果を用いて[pBuf]の領域を塗り設定で塗りつぶす
    //（※この処理の後は[pBufGuide]が破壊されている点に注意すること＝塗りガイドを再利用したい場合は退避＆復旧する必要がある）
    // [pBufGuard]の値に対して[bitMask]で出力を制限する
    //--------------------------------------------------------------------------------------------------
    bool fill0( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE bitMask,
                BYTE palGrp, BYTE palVal, BYTE testPalGrp, bool isForAdd, bool isOnOutCol, int addVal );

    // 通常塗り
    inline bool fill( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE bitMask,
                      BYTE palGrp, BYTE palVal, BYTE testPalGrp, bool isOnOutCol ){
        return( fill0( pBuf, pBufGuide, pBufGuard, w, h, x, y, bitMask, palGrp, palVal, testPalGrp, false, isOnOutCol, 0 ) );
    }

    // 加算塗り
    inline bool fillAdd( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, int x, int y, BYTE bitMask,
                         BYTE testPalGrp, int addVal, bool isOnOutCol ){
        return( fill0( pBuf, pBufGuide, pBufGuard, w, h, x, y, bitMask, 0, 0, testPalGrp, true, isOnOutCol, addVal ) );
    }

    //--------------------------------------------------------------------------------------------------
    // オフセット（別画素参照）による加算（※フラグにより[全色／予約]処理へ切り替え＆[addVal]の値で加算／減算の切り替え）
    //--------------------------------------------------------------------------------------------------
    bool ofsAdd( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, BYTE bitMask,
                 int ofsX, int ofsY, int addVal, eFILL_OPTION type );
    
    // 未出力領域への明暗予約
    inline bool ofsAddForReserve( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, BYTE bitMask,
                                  int ofsX, int ofsY, int addVal ){
        return( ofsAdd( pBuf, pBufGuide, pBufGuard, w, h, bitMask, ofsX, ofsY, addVal, eFILL_OPTION_RESERVE ) );
    }
    
    // 出力領域への明暗
    inline bool ofsAddForInto( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, BYTE bitMask,
                               int ofsX, int ofsY, int addVal ){
        return( ofsAdd( pBuf, pBufGuide, pBufGuard, w, h, bitMask, ofsX, ofsY, addVal, eFILL_OPTION_INTO ) );
    }

    // 色による明暗
    inline bool ofsAddForColor( BYTE* pBuf, BYTE* pBufGuide, BYTE* pBufGuard, int w, int h, BYTE bitMask,
                                int ofsX, int ofsY, int addVal ){
        return( ofsAdd( pBuf, pBufGuide, pBufGuard, w, h, bitMask, ofsX, ofsY, addVal, eFILL_OPTION_COLOR ) );
    }

    //--------------------------------------------------------------------------------------------------
    // マスク
    //--------------------------------------------------------------------------------------------------
    bool mask( BYTE* pBuf, BYTE* pBufGuide, int w, int h, int x, int y, BYTE bitMask );
    
    //==============================================================
    // 以下、[FillForExe.cpp]にて実装
    //==============================================================
protected:
    //--------------------------------------------------
    // [fill0]から呼ばれる
    //--------------------------------------------------
    // 領域に対するオフ描き込み（出力色のみを書き込む）
    void exeFillOff( BYTE bitMask );

    // 領域に対する描き込み
    void exeFillMono( BYTE bitMask, BYTE palGrp, BYTE testPalGrp );
    void exeFillMonoOnOutCol( BYTE bitMask, BYTE palGrp );

    void exeFillMonoAdd( BYTE bitMask, BYTE palGrp, BYTE testPalGrp, int addVal );
    void exeFillMonoAddOnOutCol( BYTE bitMask, BYTE palGrp, int addVal );

    void exeFillMonoSub( BYTE bitMask, BYTE palGrp, BYTE testPalGrp, int subVal );
    void exeFillMonoSubOnOutCol( BYTE bitMask, BYTE palGrp, int subVal );

    // 領域に対する明暗
    void exeFillAdd( BYTE bitMask, BYTE testPalGrp, int addVal );
    void exeFillAddOnOutCol( BYTE bitMask, int addVal );

    void exeFillSub( BYTE bitMask, BYTE testPalGrp, int subVal );
    void exeFillSubOnOutCol( BYTE bitMask, int subVal );

    //--------------------------------------------------
    // [ofsAdd]から呼ばれる
    //--------------------------------------------------
    // 直近色による明暗予約
    void exeOfsAddForReserve( BYTE bitMask, int ofsX, int ofsY, int addVal );
    void exeOfsSubForReserve( BYTE bitMask, int ofsX, int ofsY, int subVal );

    // 直近色への明暗
    void exeOfsAddForInto( BYTE bitMask, int ofsX, int ofsY, int addVal );
    void exeOfsSubForInto( BYTE bitMask, int ofsX, int ofsY, int subVal );
    
    // 色全体への明暗
    void exeOfsAddForColor( BYTE bitMask, int ofsX, int ofsY, int addVal );
    void exeOfsSubForColor( BYTE bitMask, int ofsX, int ofsY, int subVal );
    
    //--------------------------------------------------
    // [mask]から呼ばれる
    //--------------------------------------------------
    // ガード値の描き込み
    void exeMask( BYTE bitMask );
    
    //==============================================================
    // 以下、[FillForUndercoat.cpp]にて実装
    //==============================================================
protected:
    // ワーク管理
    void readyForUndercoat( void );
    inline bool isUndercoatCellEmpty( void ){ return( m_nUndercoatCellHeadAt == m_nUndercoatCellTailAt ); }
    
    void addUndercoatCell( int hint, int xL, int xR, int y );
    stFILL_UNDERCOAT_CELL* getUndercoatCell( void );
    
    // 塗りつぶし
    void fillUndercoat( int x, int y );
    
    // ラインのスキャン
    void scanLineForUndercoat( int xL, int xR, int y, bool isMoveUp );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelFillType[];
extern const char* g_pArrLabelFillOption[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __FILL_HPP__ */
