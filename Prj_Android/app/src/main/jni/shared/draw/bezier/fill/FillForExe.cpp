/*+----------------------------------------------------------------+
  | Title:      FillForExe.cpp [共通環境]
  | Comment:    塗り：実行
  | Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Fill.hpp"

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
//-----------------------------------------------------------------
// [fill0]: オフ塗り（※ガイドに出力フラグのみを書き込む）
//-----------------------------------------------------------------
//（※この処理は[OnOutCol]には非対応＝そもそも出力色の処理なので）
//-----------------------------------------------------------------
void CFill::exeFillOff( BYTE bitMask ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 直近出力色
                SET_BFG_LAST_OUT_COL( *pGuide );
            }
            
            pGuide++;
        }
    }
}

//----------------------------------------------------------------------
// [fill0]: 単色塗り（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//----------------------------------------------------------------------
void CFill::exeFillMono( BYTE bitMask, BYTE palGrp, BYTE testPalGrp ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // テストに通れば
                if( BP_CONV_DOT_TO_PAL_GRP( *pCol ) == testPalGrp ){
                    if( (*pGuard & bitMask) == 0 ){
                        // 画素の出力
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                        
                    // 直近出力色
                    SET_BFG_LAST_OUT_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
    
    // 管理に対して新たに色が塗られたであろう領域を通知（※厳密ではないが全ての出力で判定はしたくないので）
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatL, m_nUndercoatT );    // 左上
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatR, m_nUndercoatB );    // 右下
}

//--------------------------------------------------------------------------------
// [fill0/OnOutCol]: 単色塗り（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//--------------------------------------------------------------------------------
void CFill::exeFillMonoOnOutCol( BYTE bitMask, BYTE palGrp ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 出力色であれば
                if( IS_BFG_OUT_COL( *pGuide ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        // 画素の更新
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近色（※出力色は変化させない）
                    SET_BFG_LAST_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//-----------------------------------------------------------------------------------
// [fill0]: 単色塗り＋加算値（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//-----------------------------------------------------------------------------------
void CFill::exeFillMonoAdd( BYTE bitMask, BYTE palGrp, BYTE testPalGrp, int addVal  ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // テストに通れば
                if( BP_CONV_DOT_TO_PAL_GRP( *pCol ) == testPalGrp ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }

                        // 画素の出力
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近出力色
                    SET_BFG_LAST_OUT_COL( *pGuide );
                }
            }

            pCol++;
            pGuide++;
            pGuard++;
        }
    }
    
    // 管理に対して新たに色が塗られたであろう領域を通知（※厳密ではないが全ての出力で判定はしたくないので）
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatL, m_nUndercoatT );    // 左上
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatR, m_nUndercoatB );    // 右下
}

//-------------------------------------------------------------------------------------
// [fill0/OnOutCol]: 単色塗り＋加算値（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//-------------------------------------------------------------------------------------
void CFill::exeFillMonoAddOnOutCol( BYTE bitMask, BYTE palGrp, int addVal  ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 出力色であれば
                if( IS_BFG_OUT_COL( *pGuide ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }

                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近色（※出力色は変化させない）
                    SET_BFG_LAST_COL( *pGuide );
                }
            }

            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//-----------------------------------------------------------------------------------
// [fill0]: 単色塗り＋減算値（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//-----------------------------------------------------------------------------------
void CFill::exeFillMonoSub( BYTE bitMask, BYTE palGrp, BYTE testPalGrp, int subVal  ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // テストに通れば
                if( BP_CONV_DOT_TO_PAL_GRP( *pCol ) == testPalGrp ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }

                        // 画素の出力
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近出力色
                    SET_BFG_LAST_OUT_COL( *pGuide );
                }
            }

            pCol++;
            pGuide++;
            pGuard++;
        }
    }
    
    // 管理に対して新たに色が塗られたであろう領域を通知（※厳密ではないが全ての出力で判定はしたくないので）
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatL, m_nUndercoatT );    // 左上
    CBmpGenerator::UpdateColorBufInfo( m_nUndercoatR, m_nUndercoatB );    // 右下
}

//-------------------------------------------------------------------------------------
// [fill0/OnOutCol]: 単色塗り＋減算値（※画素が新たに塗られるためベジェ管理のバッファ状況が更新される）
//-------------------------------------------------------------------------------------
void CFill::exeFillMonoSubOnOutCol( BYTE bitMask, BYTE palGrp, int subVal  ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 出力色であれば
                if( IS_BFG_OUT_COL( *pGuide ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }

                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近色（※出力色は変化させない）
                    SET_BFG_LAST_COL( *pGuide );
                }
            }

            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------
// [fill0]: 下塗り領域に加算
//---------------------------------------
void CFill::exeFillAdd( BYTE bitMask, BYTE testPalGrp, int addVal ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 有効な画素でテストに通れば
                if( *pCol >= BEZIER_PAL_GRP_HEAD && (testPalGrp==0x00 ||BP_CONV_DOT_TO_PAL_GRP( *pCol )==testPalGrp) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );

                        // パレット値を加算（暗く）
                        palVal += addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近出力色
                    SET_BFG_LAST_OUT_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------
// [fill0/OnOutCol]: 下塗り領域に加算
//---------------------------------------
void CFill::exeFillAddOnOutCol( BYTE bitMask, int addVal ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 出力色であれば
                if( IS_BFG_OUT_COL( *pGuide ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );

                        // パレット値を加算（暗く）
                        palVal += addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近色（※出力色は変化させない）
                    SET_BFG_LAST_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------
// [fill0]: 下塗り領域に減算
//---------------------------------------
void CFill::exeFillSub( BYTE bitMask, BYTE testPalGrp, int subVal ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 有効な画素でテストに通れば
                if( *pCol >= BEZIER_PAL_GRP_HEAD && (testPalGrp==0x00||BP_CONV_DOT_TO_PAL_GRP( *pCol )==testPalGrp) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );

                        // パレット値を減算（明るく）
                        palVal -= subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近出力色
                    SET_BFG_LAST_OUT_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------
// [fill0/OnOutCol]: 下塗り領域に減算
//---------------------------------------
void CFill::exeFillSubOnOutCol( BYTE bitMask, int subVal ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + m_nUndercoatL];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // 下塗り色 → 完了色
                RESET_BFG_UNDERCOAT_COL( *pGuide );
                SET_BFG_DONE_COL( *pGuide );

                // 出力色であれば
                if( IS_BFG_OUT_COL( *pGuide ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol );

                        // パレット値を減算（明るく）
                        palVal -= subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                    
                    // 直近色（※出力色は変化させない）
                    SET_BFG_LAST_COL( *pGuide );
                }
            }
            
            pCol++;
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------------------------------------
// [ofsAdd]: 未出力色に加算（※出力された画素が投げかける影）
//---------------------------------------------------------------------
void CFill::exeOfsAddForReserve( BYTE bitMask, int ofsX, int ofsY, int addVal ){
    // 塗りガイド全体を参照
    int l = CBmpGenerator::GetLeftForFillGuideBuf();
    int r = CBmpGenerator::GetRightForFillGuideBuf();
    int t = CBmpGenerator::GetTopForFillGuideBuf();
    int b = CBmpGenerator::GetBottomForFillGuideBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 直近色であれば
            if( IS_BFG_LAST_COL( *pGuide ) ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    if( (*pGuard & bitMask) == 0 ){
                        // 加算（暗く）
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( m_pBuf[m_nDotW*targetY+targetX] ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        
                        // 画素の更新
                        m_pBuf[m_nDotW*targetY+targetX] = BP_CONV_PAL_INFO_TO_DOT( 0x00, palVal );
                    }
                }
            }
            
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------------------------------------
// [ofsAdd]: 未出力色に減算（※出力された画素が投げかける光）
//---------------------------------------------------------------------
void CFill::exeOfsSubForReserve( BYTE bitMask, int ofsX, int ofsY, int subVal ){
    // 塗りガイド全体を参照
    int l = CBmpGenerator::GetLeftForFillGuideBuf();
    int r = CBmpGenerator::GetRightForFillGuideBuf();
    int t = CBmpGenerator::GetTopForFillGuideBuf();
    int b = CBmpGenerator::GetBottomForFillGuideBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 直近色であれば
            if( IS_BFG_LAST_COL( *pGuide ) ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    if( (*pGuard & bitMask) == 0 ){
                        // 減算（明るく）
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( m_pBuf[m_nDotW*targetY+targetX] ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        
                        // 画素の更新
                        m_pBuf[m_nDotW*targetY+targetX] = BP_CONV_PAL_INFO_TO_DOT( 0x00, palVal );
                    }
                }
            }
            
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------------------------------------
// [ofsAdd]: 出力による加算（※出力された画素に投げかけられる影）
//---------------------------------------------------------------------
void CFill::exeOfsAddForInto( BYTE bitMask, int ofsX, int ofsY, int addVal ){
    // 塗りガイド全体を参照
    int l = CBmpGenerator::GetLeftForFillGuideBuf();
    int r = CBmpGenerator::GetRightForFillGuideBuf();
    int t = CBmpGenerator::GetTopForFillGuideBuf();
    int b = CBmpGenerator::GetBottomForFillGuideBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 直近色であれば
            if( IS_BFG_LAST_COL( *pGuide ) ){
                // 参照先が完了色でなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    IS_NOT_BFG_DONE_COL( m_pBufGuide[m_nDotW*targetY+targetX] ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        BYTE* pCol = &m_pBuf[m_nDotW*y+x];
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        
                        // 加算（暗く）
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) + addVal;
                        if( palVal > BEZIER_PAL_VAL_MAX ){
                            palVal = BEZIER_PAL_VAL_MAX;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
            
            pGuide++;
            pGuard++;
        }
    }
}

//---------------------------------------------------------------------
// [ofsAdd]: 出力による減算（※出力された画素に投げかけられる光）
//---------------------------------------------------------------------
void CFill::exeOfsSubForInto( BYTE bitMask, int ofsX, int ofsY, int subVal ){
    // 塗りガイド全体を参照
    int l = CBmpGenerator::GetLeftForFillGuideBuf();
    int r = CBmpGenerator::GetRightForFillGuideBuf();
    int t = CBmpGenerator::GetTopForFillGuideBuf();
    int b = CBmpGenerator::GetBottomForFillGuideBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 直近色であれば
            if( IS_BFG_LAST_COL( *pGuide ) ){
                // 参照先が完了色でなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    IS_NOT_BFG_DONE_COL( m_pBufGuide[m_nDotW*targetY+targetX] ) ){
                    if( (*pGuard & bitMask) == 0 ){
                        BYTE* pCol = &m_pBuf[m_nDotW*y+x];
                        int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                        
                        // 減算（明るく）
                        int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) - subVal;
                        if( palVal < BEZIER_PAL_VAL_MIN ){
                            palVal = BEZIER_PAL_VAL_MIN;
                        }
                        
                        // 画素の更新
                        *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                    }
                }
            }
            
            pGuide++;
            pGuard++;
        }
    }
}

//--------------------------------------------------------------------------
// [ofsAdd]: 色による加算（※色の形状による影）
//--------------------------------------------------------------------------
// TODO:汚く見える色の除外指定（※顔だけでなく頬等も除外しないと違和感がある）
#define SKIP_PAL_OFS_FOR_COLOR  (60)
void CFill::exeOfsAddForColor( BYTE bitMask, int ofsX, int ofsY, int addVal ){
    // 色バッファを参照
    int l = CBmpGenerator::GetLeftForColorBuf();
    int r = CBmpGenerator::GetRightForColorBuf();
    int t = CBmpGenerator::GetTopForColorBuf();
    int b = CBmpGenerator::GetBottomForColorBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 有効な画素であれば
            if( *pCol >= BEZIER_PAL_GRP_HEAD ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                    
                    // 顔は無視してみる（※顔周りの形状は複雑なので明暗が汚く見えてしまう）
                    if( palGrp > SKIP_PAL_OFS_FOR_COLOR ){
                        if( (*pGuard & bitMask) == 0 ){
                            // 加算（暗く）
                            int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) + addVal;
                            if( palVal > BEZIER_PAL_VAL_MAX ){
                                palVal = BEZIER_PAL_VAL_MAX;
                            }
                            
                            // 画素の更新
                            *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                        }
                    }
                }
            }
            
            pCol++;
            pGuard++;
        }
    }
}

//--------------------------------------------------------------------------
// [ofsAdd]: 色による減算（※色の形状による光）
//--------------------------------------------------------------------------
void CFill::exeOfsSubForColor( BYTE bitMask, int ofsX, int ofsY, int subVal ){
    // 色バッファを参照
    int l = CBmpGenerator::GetLeftForColorBuf();
    int r = CBmpGenerator::GetRightForColorBuf();
    int t = CBmpGenerator::GetTopForColorBuf();
    int b = CBmpGenerator::GetBottomForColorBuf();
    
    for( int y=t; y<=b; y++ ){
        BYTE* pCol = &m_pBuf[m_nDotW*y + l];
        BYTE* pGuard = &m_pBufGuard[m_nDotW*y + l];
        for( int x=l; x<=r; x++ ){
            // 有効な画素であれば
            if( *pCol >= BEZIER_PAL_GRP_HEAD  ){
                // 参照先に画素がなければ
                int targetX = x + ofsX;
                int targetY = y + ofsY;
                if( targetX < 0 || targetX >= m_nDotW || targetY < 0 || targetY >= m_nDotH ||
                    m_pBuf[m_nDotW*targetY+targetX] < BEZIER_PAL_GRP_HEAD ){
                    int palGrp = BP_CONV_DOT_TO_PAL_GRP( *pCol );
                    
                    // 一部の色は無視してみる（※顔周りの形状は複雑なので明暗が汚く見えてしまう）
                    if( palGrp > SKIP_PAL_OFS_FOR_COLOR ){
                        if( (*pGuard & bitMask) == 0  ){
                            // 減算（明るく）
                            int palVal = BP_CONV_DOT_TO_PAL_VAL( *pCol ) - subVal;
                            if( palVal < BEZIER_PAL_VAL_MIN ){
                                palVal = BEZIER_PAL_VAL_MIN;
                            }

                            // 画素の更新
                            *pCol = BP_CONV_PAL_INFO_TO_DOT( palGrp, palVal );
                        }
                    }
                }
            }
            
            pCol++;
            pGuard++;
        }
    }
}

//-----------------------------------------------------------------------------
// [mask]: 実行（※ガードフラグのみ書き込む）
//-----------------------------------------------------------------------------
void CFill::exeMask( BYTE bitMask ){
    // 下塗り領域に対して処理する
    for( int y=m_nUndercoatT; y<=m_nUndercoatB; y++ ){
        BYTE* pMask = &m_pBuf[m_nDotW*y+m_nUndercoatL];
        BYTE* pGuide = &m_pBufGuide[m_nDotW*y+m_nUndercoatL];
        for( int x=m_nUndercoatL; x<=m_nUndercoatR; x++ ){
            if( IS_BFG_UNDERCOAT_COL( *pGuide ) ){
                // マスク値の上書きのみ（※完了色／出力色は不要）
                *pMask |= bitMask;
            }
            
            pMask++;
            pGuide++;
        }
    }
    
    // ガードは領域を捜査する処理がないので情報の更新は不要
}
