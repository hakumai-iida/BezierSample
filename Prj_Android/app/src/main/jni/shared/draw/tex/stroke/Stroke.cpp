/*+----------------------------------------------------------------+
  | Title:      Stroke.hpp [共通環境]
  | Comment:    ストローク（※アンカーポイントを結ぶ線の見た目部分）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Stroke.hpp"
#include "StrokeDot.hpp"

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
//-------------------
// フックターゲット調整
//-------------------
eSTROKE_HOOK_TARGET CStroke::AdjustHookTargetForSlotIndex( eSTROKE_HOOK_TARGET target, eBD_SLOT slot, int slotIndex ){
    // 対象が有効
    if( IS_STROKE_HOOK_TARGET_VALID( target ) ){
        // テンポラリ
        if( target >= eSTROKE_HOOK_TARGET_TEMP_START && target <= eSTROKE_HOOK_TARGET_TEMP_END ){
            // 何もしない
        }
        // パーツ
        else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_PART && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_PART ){
            if( IS_BD_SLOT_VALID( slot ) ){
                // １パーツ内に１セットのHOOK枠がある（※スロットインデックス毎にも枠をもつ）
                int ofs = 2*slot;
                if( slotIndex > 0 ){ ofs += 1; }
                ofs *= eSTROKE_HOOK_TARGET_NUM_IN_PART;
                
                target = (eSTROKE_HOOK_TARGET)(target - eSTROKE_HOOK_TARGET_CHECK_START_FOR_PART + ofs);
            }
        }
        // スロットインデックスが有効（右の要素）であれば
        else if( slotIndex > 0 ){
            // 特定
            if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_FIXED && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_FIXED ){
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_FIXED + 2*((target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_FIXED)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);
            }
            // 腕
            else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_ARM ){
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM + 2*((target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);
            }
            // 脚
            else if( target >= eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG && target <= eSTROKE_HOOK_TARGET_CHECK_END_FOR_LEG ){
                target = (eSTROKE_HOOK_TARGET)(eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG + 2*((target-eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG)/2));
                target = (eSTROKE_HOOK_TARGET)(target+1);
            }
        }
    }
    
    return( target );
}

//-------------------
// タッチターゲット調整
//-------------------
eSTROKE_TOUCH_TARGET CStroke::AdjustTouchTargetForSlotIndex( eSTROKE_TOUCH_TARGET target, int slotIndex ){
    // 対象が有効
    if( IS_STROKE_TOUCH_TARGET_VALID( target ) ){
        // テンポラリ
        if( target >= eSTROKE_TOUCH_TARGET_TEMP_START && target <= eSTROKE_TOUCH_TARGET_TEMP_END ){
            // 何もしない
        }
        // スロットインデックスが有効（右の要素）であれば
        else if( slotIndex > 0 ){
            // 腕
            if( target >= eSTROKE_TOUCH_TARGET_CHECK_START_FOR_ARM && target <= eSTROKE_TOUCH_TARGET_CHECK_END_FOR_ARM ){
                target = (eSTROKE_TOUCH_TARGET)(eSTROKE_TOUCH_TARGET_CHECK_START_FOR_ARM + 2*((target-eSTROKE_TOUCH_TARGET_CHECK_START_FOR_ARM)/2));
                target = (eSTROKE_TOUCH_TARGET)(target + 1);
            }
            // 脚
            else if( target >= eSTROKE_TOUCH_TARGET_CHECK_START_FOR_LEG && target <= eSTROKE_TOUCH_TARGET_CHECK_END_FOR_LEG ){
                target = (eSTROKE_TOUCH_TARGET)(eSTROKE_TOUCH_TARGET_CHECK_START_FOR_LEG + 2*((target-eSTROKE_TOUCH_TARGET_CHECK_START_FOR_LEG)/2));
                target = (eSTROKE_TOUCH_TARGET)(target + 1);
            }
        }
    }
    
    return( target );
}

//------------------
// ガイドターゲット調整
//------------------
eSTROKE_GUIDE_TARGET CStroke::AdjustGuideTargetForSlotIndex( eSTROKE_GUIDE_TARGET target, int slotIndex ){
    // 対象が有効
    if( IS_STROKE_GUIDE_TARGET_VALID( target ) ){
        // テンポラリ
        if( target >= eSTROKE_GUIDE_TARGET_TEMP_START && target <= eSTROKE_GUIDE_TARGET_TEMP_END ){
            // 何もしない
        }
        // スロットインデックスが有効（右の要素）であれば
        else if( slotIndex > 0 ){
            // 腕
            if( target >= eSTROKE_GUIDE_TARGET_CHECK_START_FOR_ARM && target <= eSTROKE_GUIDE_TARGET_CHECK_END_FOR_ARM ){
                target = (eSTROKE_GUIDE_TARGET)(eSTROKE_GUIDE_TARGET_CHECK_START_FOR_ARM + 2*((target-eSTROKE_GUIDE_TARGET_CHECK_START_FOR_ARM)/2));
                target = (eSTROKE_GUIDE_TARGET)(target + 1);
            }
            // 脚
            else if( target >= eSTROKE_GUIDE_TARGET_CHECK_START_FOR_LEG && target <= eSTROKE_GUIDE_TARGET_CHECK_END_FOR_LEG ){
                target = (eSTROKE_GUIDE_TARGET)(eSTROKE_GUIDE_TARGET_CHECK_START_FOR_LEG + 2*((target-eSTROKE_GUIDE_TARGET_CHECK_START_FOR_LEG)/2));
                target = (eSTROKE_GUIDE_TARGET)(target + 1);
            }
        }
    }
    
    return( target );
}

//-------------------
// コンストラクタ
//-------------------
CStroke::CStroke( void ){
    clear();
}

//-------------------
// クリア
//-------------------
void CStroke::clear( void ){
    m_eType = eSTROKE_TYPE_INVALID;
    m_fSize = 0.0f;
    m_fOfsRange = 0.0f;
}

//-------------------
// ストローク設定
//-------------------
bool CStroke::set( eSTROKE_TYPE type, float size, float range ){
    // 一旦クリア
    clear();

    // 無効は無視
    if( type < 0 || type >= eSTROKE_TYPE_MAX ){
        LOGE( "@ CStroke::set: FAILED: INVALID TYPE: type=%d, size=%f, range=%f\n", type, size, range );
        return( false );
    }

    // サイズが０以下の場合は無効
    if( size <= 0.0f ){
        LOGE( "@ CStroke::set: FAILED: TOO SMALL SIZE: type=%d, size=%f, range=%f\n", type, size, range );
        return( false );
    }

    // サイズが大きすぎる場合は制限
    if( size > STROKE_DOT_ALLOC_SIZE_MAX ){
        LOGE( "@ CStroke::set: FAILED: TOO HUGE SIZE: type=%d, size=%f, range=%f\n", type, size, range );
        size = STROKE_DOT_ALLOC_SIZE_MAX;
    }

    // 用心（※レンジが負なら反転しておく）
    if( range < 0.0f ){
        range *= -1.0f;
    }

    // 設定を保持して完了
    m_eType = type;
    m_fSize = size;
    m_fOfsRange = range;
    
    return( true );
}

//-----------------------------------------------------------------
// ストロークの出力セルの取得
//-----------------------------------------------------------------
stSTROKE_OUTPUT_CELL* CStroke::getStrokeCell( float rateOutX, float rateOutY, float scale ){
    CStrokeDot* pStrokeDot = NULL;

    if( m_eType == eSTROKE_TYPE_SQUARE ){
        pStrokeDot = CStrokeMgr::GetStrokeDotForSquare( scale*m_fSize );
    }else if ( m_eType == eSTROKE_TYPE_CIRCLE ){
        pStrokeDot = CStrokeMgr::GetStrokeDotForCircle( scale*m_fSize );
    }

    if( pStrokeDot == NULL ){
        return( NULL );
    }

    // レンジが有効ならブレさせる
    if( m_fOfsRange > 0.0f ){
        // Rangeの値で半径を設定して適当に回転させる
        float r = CRand::GetRandF( m_fOfsRange );
        float deg = CRand::GetRandF( 360.0f );
        float oX = r * CMath::GetCosLite( deg ) + rateOutX;
        float oY = r * CMath::GetSinLite( deg ) + rateOutY;
        
        // 整数分のブレはオフセットでずらす
        m_stOutputCell.ofsX = (int)oX;
        m_stOutputCell.ofsY = (int)oY;
        
        // 負の端数の調整
        float outX = oX - m_stOutputCell.ofsX;
        float outY = oY - m_stOutputCell.ofsY;
        if( outX < 0.0f ){
            m_stOutputCell.ofsX -= 1;
            outX += 1.0f;
        }
        if( outY < 0.0f ){
            m_stOutputCell.ofsY -= 1;
            outY += 1.0f;
        }

        // バッファとサイズ設定
        m_stOutputCell.pBuf = pStrokeDot->getDot( outX, outY );
        m_stOutputCell.sizeW = m_stOutputCell.sizeH = pStrokeDot->getSizeDot();
    }
    // レンジの指定がなければそのまま
    else{
        // バッファとサイズ設定
        m_stOutputCell.pBuf = pStrokeDot->getDot( rateOutX, rateOutY );
        m_stOutputCell.sizeW = m_stOutputCell.sizeH = pStrokeDot->getSizeDot();
        
        // オフセットは不要（ブレないので）
        m_stOutputCell.ofsX = m_stOutputCell.ofsY = 0;
    }

    return( &m_stOutputCell );
}
