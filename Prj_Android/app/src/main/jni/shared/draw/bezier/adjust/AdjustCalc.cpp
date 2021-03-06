/*+----------------------------------------------------------------+
  |	Title:		AdjustCalc.cpp [共通環境]
  |	Comment:	ベジェ関連データ変換
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AdjustCalc.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
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
//-------------------------------------------------------------
// 座標計算：調整値[Ｈ/Ｖ]（アンカーポイント）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateXYForHV( int* pOutRateX, int* pOutRateY, int rX0, int rY0,
                                   CAdjustablePoint* pAP, float fixedAdjustRateH, float fixedAdjustRateV ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateX == NULL || pOutRateY == NULL || pAP == NULL ){
        return;
    }
    
    FIX_ADJUST_RATE( fixedAdjustRateH );
    FIX_ADJUST_RATE( fixedAdjustRateV );
#endif
    
    int rX = rX0;
    int rY = rY0;
    
    //---------------------------
    // H:ヨコ方向への調整
    //---------------------------
    // 最小方向への調整
    if( fixedAdjustRateH < 0.0f ){
        rX += (int)(-fixedAdjustRateH * pAP->getOfsMinRateXForH());
        rY += (int)(-fixedAdjustRateH * pAP->getOfsMinRateYForH());
    }
    // 最大方向への調整
    else if( fixedAdjustRateH > 0.0f ){
        rX += (int)(fixedAdjustRateH * pAP->getOfsMaxRateXForH());
        rY += (int)(fixedAdjustRateH * pAP->getOfsMaxRateYForH());
    }
    
    //---------------------------
    // V:タテ方向への調整
    //---------------------------
    // 最小方向への調整
    if( fixedAdjustRateV < 0.0f ){
        rX += (int)(-fixedAdjustRateV * pAP->getOfsMinRateXForV());
        rY += (int)(-fixedAdjustRateV * pAP->getOfsMinRateYForV());
    }
    // 最大方向への調整
    else if( fixedAdjustRateV > 0.0f ){
        rX += (int)(fixedAdjustRateV * pAP->getOfsMaxRateXForV());
        rY += (int)(fixedAdjustRateV * pAP->getOfsMaxRateYForV());
    }
    
    // 出力
    *pOutRateX = rX;
    *pOutRateY = rY;
}

//-----------------------------------------------------------------------------
// [adjustH]: 調整値による結合半径の算出（アンカーポイント）
//-----------------------------------------------------------------------------
void CAdjustCalc::CalcRateRForH( int *pOutRateR, int *pOutRateOfsR, int rR0, int rOfsForCenter0,
                                 CSlotAdjuster* pSA, float fixedAdjustRateH ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateR == NULL || pOutRateOfsR == NULL || pSA == NULL ){
        return;
    }
    
    FIX_ADJUST_RATE( fixedAdjustRateH );
#endif
    
    int rateR = rR0;
    
    //----------------------------------
    // H:ヨコ調整
    //----------------------------------
    // 最小方向への調整
    if( fixedAdjustRateH < 0.0f ){
        rateR += (int)(-fixedAdjustRateH * pSA->getJointMinRateOfsRForH());
    }
    // 最大方向への調整
    else if( fixedAdjustRateH > 0.0f ){
        rateR += (int)(fixedAdjustRateH * pSA->getJointMaxRateOfsRForH());
    }
    
    // 出力
    *pOutRateR = rateR;
    *pOutRateOfsR = (int)(rateR * CConst::ConvBezierScaleRate( rOfsForCenter0 ));
}

//-------------------------------------------------------------
// 座標計算：遅延ログ（アンカーポイント）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateXYForDelayLog( int* pOutRateX, int* pOutRateY, int rX0, int rY0,
                                         bool isReverseH, bool isReverseV, float rot,
                                         CAdjustablePoint* pAP, CDelayLog* pDelayLog ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateX == NULL || pOutRateY == NULL || pAP == NULL || pDelayLog == NULL ){
        return;
    }
#endif
    
    int rX = rX0;
    int rY = rY0;
    
    //-----------------------
    // 遅延調整
    //-----------------------
    eDELAY_LOG logType = pAP->getDelayType();
    if( IS_DELAY_LOG_VALID( logType ) ){
        int depth = pAP->getDelayDepth();

        int rateX = pDelayLog->getDelayRateX( logType, depth );
        int rateY = pDelayLog->getDelayRateY( logType, depth );

        int ofsX0 = (rateX * pAP->getDelayPowerRateX()) / BEZIER_SCALE_RATE;
        int ofsY0 = (rateY * pAP->getDelayPowerRateY()) / BEZIER_SCALE_RATE;
  
        // 指定があれば回転（※指定角度を打ち消す方向に回転）
        int ofsX, ofsY;
        if( rot != 0.0f ){
            ofsX = CMath::RotX( ofsX0, ofsY0, -rot );
            ofsY = CMath::RotY( ofsX0, ofsY0, -rot );
        }else{
            ofsX = ofsX0;
            ofsY = ofsY0;
        }

        // 指定があれば反転
        if( isReverseH ){ ofsX *= -1; }
        if( isReverseV ){ ofsY *= -1; }

        // 適用
        rX += ofsX;
        rY += ofsY;
    }
    
    // 出力
    *pOutRateX = rX;
    *pOutRateY = rY;
}

//-------------------------------------------------------------
// 座標計算：遅延ログ（レイヤー）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateOfsXYForDelayLog( int* pOutRateOfsX, int* pOutRateOfsY,
                                            bool isReverseH, bool isReverseV,
                                            CLayerData* pLD, CDelayLog* pDelayLog ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateOfsX == NULL || pOutRateOfsY == NULL || pLD == NULL || pDelayLog == NULL ){
        return;
    }
#endif
    
    int rOfsX = 0;
    int rOfsY = 0;
    
    //-----------------------
    // 遅延調整
    //-----------------------
    eDELAY_LOG logType = pLD->getDelayType();
    if( IS_DELAY_LOG_VALID( logType ) ){
        int depth = pLD->getDelayDepth();

        int rateX = pDelayLog->getDelayRateX( logType, depth );
        int rateY = pDelayLog->getDelayRateY( logType, depth );

        rOfsX = (rateX * pLD->getDelayPowerRateX()) / BEZIER_SCALE_RATE;
        rOfsY = (rateY * pLD->getDelayPowerRateY()) / BEZIER_SCALE_RATE;
  
        // 指定があれば反転
        if( isReverseH ){ rOfsX *= -1; }
        if( isReverseV ){ rOfsY *= -1; }
    }
    
    // 出力
    *pOutRateOfsX = rOfsX;
    *pOutRateOfsY = rOfsY;
}

//-------------------------------------------------------------
// 角度計算：遅延ログ（レイヤー）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateRotForDelayLog( int* pOutRateRot,
                                          bool isReverseH, bool isReverseV,
                                          CLayerData* pLD, CDelayLog* pDelayLog ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateRot == NULL || pLD == NULL || pDelayLog == NULL ){
        return;
    }
#endif
    
    int rateRot = 0;

    //-----------------------
    // 遅延調整
    //-----------------------
    eDELAY_LOG logType = pLD->getDelayType();
    if( IS_DELAY_LOG_VALID( logType ) ){
        int depth = pLD->getDelayDepth();

        int rateX = pDelayLog->getDelayRateX( logType, depth );
        int rateY = pDelayLog->getDelayRateY( logType, depth );
        
        rateX = (rateX * pLD->getDelayPowerRateXForRot()) / BEZIER_SCALE_RATE;
        rateY = (rateY * pLD->getDelayPowerRateYForRot()) / BEZIER_SCALE_RATE;
        
        // 指定があれば反転
        if( isReverseH ){ rateX *= -1; }
        if( isReverseV ){ rateY *= -1; }

        // 移動値と印象を合わせるために反転
        rateRot = -(rateX + rateY);
    }

    *pOutRateRot = rateRot;
}

//-------------------------------------------------------------
// スケール計算：遅延ログ（レイヤー）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateScaleForDelayLog( int* pOutRateScale, CLayerData* pLD, CDelayLog* pDelayLog ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateRot == NULL || pLD == NULL || pDelayLog == NULL ){
        return;
    }
#endif
    
    int rateScale = BEZIER_SCALE_RATE;

    //-----------------------
    // 遅延調整
    //-----------------------
    eDELAY_LOG logType = pLD->getDelayType();
    if( IS_DELAY_LOG_VALID( logType ) ){
        int depth = pLD->getDelayDepth();

        int rateX = pDelayLog->getDelayRateX( logType, depth );
        int rateY = pDelayLog->getDelayRateY( logType, depth );
        
        rateX = (rateX * pLD->getDelayPowerRateXForScale()) / BEZIER_SCALE_RATE;
        rateY = (rateY * pLD->getDelayPowerRateYForScale()) / BEZIER_SCALE_RATE;
        
        // 移動値と印象を合わせるために反転
        rateScale += rateX + rateY;
    }

    *pOutRateScale = rateScale;
}

//-------------------------------------------------------------
// スケール計算：テンション（スロット＝パーツ）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateRotForT( int* pOutRateRot, float minRate, float maxRate, float fixedAdjustRate ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateRot == NULL ){
        return;
    }
#endif
        
    int rateRot = 0;

    //----------------------------------
    // T:テンション調整
    //----------------------------------
    // 最小方向への調整
    if( fixedAdjustRate < 0.0f ){
        rateRot += (int)(-fixedAdjustRate * minRate);
    }
    // 最大方向への調整
    else if( fixedAdjustRate > 0.0f ){
        rateRot += (int)(fixedAdjustRate * maxRate);
    }

    *pOutRateRot = rateRot;
}

//-------------------------------------------------------------
// サイズ計算：サイズ（スロット＝パーツ）
//-------------------------------------------------------------
void CAdjustCalc::CalcRateScaleForS( int* pOutRateScale, float minRate, float maxRate, float fixedAdjustRate ){
    // 無効値はこない前提（※ここにくる値は正常であるはず）
#if 0
    if( pOutRateRot == NULL ){
        return;
    }
#endif
        
    int rateScale = BEZIER_SCALE_RATE;

    //----------------------------------
    // S:サイズ調整
    //----------------------------------
    // 最小方向への調整
    if( fixedAdjustRate < 0.0f ){
        rateScale = (int)( (-fixedAdjustRate*minRate) + (BEZIER_SCALE_RATE*(1.0f+fixedAdjustRate)) );
    }
    // 最大方向への調整
    else if( fixedAdjustRate > 0.0f ){
        rateScale = (int)( (fixedAdjustRate*maxRate) + (BEZIER_SCALE_RATE*(1.0f-fixedAdjustRate)) );
    }

    *pOutRateScale = rateScale;
}
