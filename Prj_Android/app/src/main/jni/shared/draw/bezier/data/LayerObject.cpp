/*+----------------------------------------------------------------+
  | Title:      LayerObject.cpp [共通環境]
  | Comment:    レイヤーオブジェクト（※レイヤーに配置する要素の基底）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerObject.hpp"
#include "LineObjectData.hpp"
#include "PaintObjectData.hpp"
#include "SlotObjectData.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
//----------------------
// ラベル：レイヤー種別
//----------------------
const char* g_pArrLabelLayerObjectType[] = {
    "LINE",
    "PAINT",
    "SLOT",
    
    NULL,
};

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------------------------
// コンストラクタ
//-----------------------------------------
CLayerObject::CLayerObject( eLAYER_OBJECT_TYPE type ): CListNode(){
    m_eObjectType = type;
}

//-----------------------------------------
// 解放（※対応するアロケーターに受け渡す）
//-----------------------------------------
void CLayerObject::Free( CLayerObject* pObj ){
    if( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData::Free( (CLineObjectData*)pObj );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData::Free( (CPaintObjectData*)pObj );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData::Free( (CSlotObjectData*)pObj );
        }else{
            LOGE( "@ CLayerObject::Free: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
    }
}

//------------
// 調整値クリア
//------------
void CLayerObject::clearAdjust( void ){
    // 線であれば
    if( isLineObject() ){
        (getLineObject())->clearAdjust();
    }
    // 塗りであれば
    else if( isPaintObject() ){
        (getPaintObject())->clearAdjust();
    }
    // スロットであれば
    else if( isSlotObject() ){
        (getSlotObject())->clearAdjust();
    }
}

//------------
// スケール適用
//------------
void CLayerObject::applyRateScale( int rateScale ){    
    // [m_eObjectType]は無視

    if( isLineObject() ){
        (getLineObject())->applyRateScale( rateScale );
    }else if( isPaintObject() ){
        (getPaintObject())->applyRateScale( rateScale );
    }else if( isSlotObject() ){
        (getSlotObject())->applyRateScale( rateScale );
    }
}

//------------------
// 一時調整値が有効か？
//------------------
bool CLayerObject::isTempAdjustValid( void ){
    if( isLineObject() ){
        return( (getLineObject())->isTempAdjustValid() );
    }else if( isPaintObject() ){
        return( (getPaintObject())->isTempAdjustValid() );
    }else if( isSlotObject() ){
        return( (getSlotObject())->isTempAdjustValid() );
    }
    
    return( false );
}

//------------
// 一時調整適用
//------------
// 基本
void CLayerObject::applyTempAdjust( void ){
    if( isLineObject() ){
        (getLineObject())->applyTempAdjust();
    }else if( isPaintObject() ){
        (getPaintObject())->applyTempAdjust();
    }else if( isSlotObject() ){
        (getSlotObject())->applyTempAdjust();
    }
}

// H:MAX
void CLayerObject::applyTempAdjustForMaxH( void ){
    if( isLineObject() ){
        (getLineObject())->applyTempAdjustForMaxH();
    }else if( isPaintObject() ){
        (getPaintObject())->applyTempAdjustForMaxH();
    }else if( isSlotObject() ){
        (getSlotObject())->applyTempAdjustForMaxH();
    }
}

// H:MIN
void CLayerObject::applyTempAdjustForMinH( void ){
    if( isLineObject() ){
        (getLineObject())->applyTempAdjustForMinH();
    }else if( isPaintObject() ){
        (getPaintObject())->applyTempAdjustForMinH();
    }else if( isSlotObject() ){
        (getSlotObject())->applyTempAdjustForMinH();
    }
}

// V:MAX
void CLayerObject::applyTempAdjustForMaxV( void ){
    if( isLineObject() ){
        (getLineObject())->applyTempAdjustForMaxV();
    }else if( isPaintObject() ){
        (getPaintObject())->applyTempAdjustForMaxV();
    }else if( isSlotObject() ){
        (getSlotObject())->applyTempAdjustForMaxV();
    }
}

// V:MIN
void CLayerObject::applyTempAdjustForMinV( void ){
    if( isLineObject() ){
        (getLineObject())->applyTempAdjustForMinV();
    }else if( isPaintObject() ){
        (getPaintObject())->applyTempAdjustForMinV();
    }else if( isSlotObject() ){
        (getSlotObject())->applyTempAdjustForMinV();
    }
}

//-----------------------
// 一時調整適用（※引数付き）
//-----------------------
// 基本
void CLayerObject::applyTempAdjust( int oRX, int oRY, int sRX, int sRY ){
    if( isLineObject() ){
        (getLineObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getLineObject())->applyTempAdjust();
    }else if( isPaintObject() ){
        (getPaintObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getPaintObject())->applyTempAdjust();
    }else if( isSlotObject() ){
        (getSlotObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getSlotObject())->applyTempAdjust();
    }
}

// H:MAX
void CLayerObject::applyTempAdjustForMaxH( int oRX, int oRY, int sRX, int sRY ){
    if( isLineObject() ){
        (getLineObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getLineObject())->applyTempAdjustForMaxH();
    }else if( isPaintObject() ){
        (getPaintObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getPaintObject())->applyTempAdjustForMaxH();
    }else if( isSlotObject() ){
        (getSlotObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getSlotObject())->applyTempAdjustForMaxH();
    }
}

// H:MIN
void CLayerObject::applyTempAdjustForMinH( int oRX, int oRY, int sRX, int sRY ){
    if( isLineObject() ){
        (getLineObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getLineObject())->applyTempAdjustForMinH();
    }else if( isPaintObject() ){
        (getPaintObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getPaintObject())->applyTempAdjustForMinH();
    }else if( isSlotObject() ){
        (getSlotObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getSlotObject())->applyTempAdjustForMinH();
    }
}

// V:MAX
void CLayerObject::applyTempAdjustForMaxV( int oRX, int oRY, int sRX, int sRY ){
    if( isLineObject() ){
        (getLineObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getLineObject())->applyTempAdjustForMaxV();
    }else if( isPaintObject() ){
        (getPaintObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getPaintObject())->applyTempAdjustForMaxV();
    }else if( isSlotObject() ){
        (getSlotObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getSlotObject())->applyTempAdjustForMaxV();
    }
}

// V:MIN
void CLayerObject::applyTempAdjustForMinV( int oRX, int oRY, int sRX, int sRY ){
    if( isLineObject() ){
        (getLineObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getLineObject())->applyTempAdjustForMinV();
    }else if( isPaintObject() ){
        (getPaintObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getPaintObject())->applyTempAdjustForMinV();
    }else if( isSlotObject() ){
        (getSlotObject())->fixTempAdjust( oRX, oRY, sRX, sRY );
        (getSlotObject())->applyTempAdjustForMinV();
    }
}
