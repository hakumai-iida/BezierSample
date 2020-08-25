/*+----------------------------------------------------------------+
  |	Title:		LayerData.cpp [共通環境]
  |	Comment:	レイヤーデータ（※描画における処理単位）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "LayerData.hpp"
#include "LayerOrderCell.hpp"
#include "BezierDataConv.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 確保サイズ
#define MAX_ALLOC_LAYER_DATA   (4*1024)

// 領域管理実装
IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------------------
// 確保メモリセル数（※管理内で[new]するセル数）
//----------------------------------------------
int CLayerData::GetAllocCellNum( void ){
    int num = 0;

    //---------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )
    //---------------------------------------------------------------------------------------------
    num += 1;
    
    return( num );
}

//----------------------------------------------
// 確保メモリサイズ算出（※管理内で[new]するサイズ）
//----------------------------------------------
int CLayerData::GetAllocSize( void ){
    int allocSize = 0;
    
    //---------------------------------------------------------------------------------------------
    // IMPLEMENT_DATA_ALLOCATOR_IN_CPP( CLayerData, eMEM_FIELD_D_BEZIER, MAX_ALLOC_LAYER_DATA )
    //---------------------------------------------------------------------------------------------
    allocSize += sizeof(CLayerData) * MAX_ALLOC_LAYER_DATA;
    
    return( allocSize );
}

//-------------------------
// コンストラクタ
//-------------------------
CLayerData::CLayerData( void ): CDataList(true), CListNode(){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CLayerData::~CLayerData( void ){
    clear();    // リストの解放
}

//--------------------
// クリア（※データの破棄）
//--------------------
void CLayerData::clear( void ){
    m_nFlag = 0;
    
    // リスト解放
    CLayerObject* pObj =  getDataHead();
    while( pObj != NULL ){
        CLayerObject* pNext = pObj->getNext();
        
        if( pObj->isLineObject() ){
            CLineObjectData::Free( (CLineObjectData*) pObj );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData::Free( (CPaintObjectData*) pObj );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData::Free( (CSlotObjectData*) pObj );
        }else{
            LOGE( "@ CLayerData::clear: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pNext;
    }
    m_oDataList.disconnectAll();
    
    // 各種値
    m_cArrName[0] = '\0';
    m_eOrderType = eLAYER_ORDER_TYPE_NORMAL;
    m_eOrderSlot = eBD_SLOT_DEFAULT;

    m_eOption = eBD_OPTION_INVALID;
    
    m_nRotPowRate = BEZIER_SCALE_RATE;
    
    m_eDelayType = eDELAY_LOG_INVALID;
    m_nDelayDepth = 0;
    m_nDelayPowerRateX = m_nDelayPowerRateY = 0;
    m_nDelayPowerRateXForRot = m_nDelayPowerRateYForRot = 0;
    m_nDelayPowerRateXForScale = m_nDelayPowerRateYForScale = 0;
}

//--------------------
// コピー
//--------------------
void CLayerData::copy( CLayerData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }
    
    // クリア（※リストの解放）
    clear();

    m_nFlag = pData->m_nFlag;

    // リストのコピー
    CLayerObject* pObj = pData->getDataHead();
    while( pObj != NULL ){
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = CLineObjectData::Alloc();
            pLOD->copy( (CLineObjectData*)pObj);
            addData( pLOD );
        }else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = CPaintObjectData::Alloc();
            pPOD->copy( (CPaintObjectData*)pObj);
            addData( pPOD );
        }else if( pObj->isSlotObject() ){
            CSlotObjectData* pSOD = CSlotObjectData::Alloc();
            pSOD->copy( (CSlotObjectData*)pObj);
            addData( pSOD );
        }else{
            LOGE( "@ CLayerData::copy: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pObj->getNext();
    }

    // 各種値
    setName( pData->m_cArrName );
    m_eOrderType = pData->m_eOrderType;
    m_eOrderSlot = pData->m_eOrderSlot;

    m_eOption = pData->m_eOption;
    
    m_nRotPowRate = pData->m_nRotPowRate;

    m_eDelayType = pData->m_eDelayType;
    m_nDelayDepth = pData->m_nDelayDepth;
    m_nDelayPowerRateX = pData->m_nDelayPowerRateX;
    m_nDelayPowerRateY = pData->m_nDelayPowerRateY;
    m_nDelayPowerRateXForRot = pData->m_nDelayPowerRateXForRot;
    m_nDelayPowerRateYForRot = pData->m_nDelayPowerRateYForRot;
    m_nDelayPowerRateXForScale = pData->m_nDelayPowerRateXForScale;
    m_nDelayPowerRateYForScale = pData->m_nDelayPowerRateYForScale;
}

//---------------------------
// 読み込み
//---------------------------
void CLayerData::read( CInputBuffer* pIB ){
    // クリア（※リストの解放）
    clear();

    m_nFlag = pIB->readInt16();

    // 要素読み込み
    int n = pIB->readInt16();
    for( int i=0; i<n; i++ ){
        // タイプ読み込み
        eLAYER_OBJECT_TYPE type = (eLAYER_OBJECT_TYPE) CEnum::ReadEnum( pIB, g_pArrLabelLayerObjectType );
        
        // 割り振り
        if( type == eLAYER_OBJECT_TYPE_LINE ){
            CLineObjectData* pLOD = CLineObjectData::Alloc();
            pLOD->read( pIB );
            addData( pLOD );
        }else if( type == eLAYER_OBJECT_TYPE_PAINT ){
            CPaintObjectData* pPOD = CPaintObjectData::Alloc();
            pPOD->read( pIB );
            addData( pPOD );
        }else if( type == eLAYER_OBJECT_TYPE_SLOT ){
            CSlotObjectData* pSOD = CSlotObjectData::Alloc();
            pSOD->read( pIB );
            addData( pSOD );
        }else{
            LOGE( "@ CLayerData::read: UNKNOWN OBJECT: type=%d\n", type );
        }
    }

    // 各種値
    pIB->readBlock( m_cArrName, LAYER_NAME_LEN );
    m_eOrderType = (eLAYER_ORDER_TYPE)CEnum::ReadEnum( pIB, g_pArrLabelLayerOrderType );
    m_eOrderSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_eOption = (eBD_OPTION)CEnum::ReadEnum( pIB, g_pArrLabelBdOption );

    m_nRotPowRate = pIB->readInt16();
    
    m_eDelayType = (eDELAY_LOG)CEnum::ReadEnum( pIB, g_pArrLabelDelayLog );
    m_nDelayDepth = pIB->readInt16();
    m_nDelayPowerRateX = pIB->readInt16();
    m_nDelayPowerRateY = pIB->readInt16();
    m_nDelayPowerRateXForRot = pIB->readInt16();
    m_nDelayPowerRateYForRot = pIB->readInt16();
    m_nDelayPowerRateXForScale = pIB->readInt16();
    m_nDelayPowerRateYForScale = pIB->readInt16();
}

//---------------------------
// 書き込み
//---------------------------
void CLayerData::write( COutputBuffer* pOB ){
    pOB->writeInt16( (int16)m_nFlag );

    // 要素出力
    int num = getDataSize();
    pOB->writeInt16( (int16)num );
    
    CLayerObject* pObj = getDataHead();
    while( pObj != NULL ){
        // タイプ別に出力
        if( pObj->isLineObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_LINE, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CLineObjectData*)pObj)->write( pOB );
        }else if( pObj->isPaintObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_PAINT, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CPaintObjectData*)pObj)->write( pOB );
        }else if( pObj->isSlotObject() ){
            CEnum::WriteEnum( pOB, eLAYER_OBJECT_TYPE_SLOT, eLAYER_OBJECT_TYPE_MAX, g_pArrLabelLayerObjectType );
            ((CSlotObjectData*)pObj)->write( pOB );
        }else{
            LOGE( "@ CLayerData::write: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }
        
        pObj = pObj->getNext();
    }

    // 各種値
    pOB->writeBlock( m_cArrName, LAYER_NAME_LEN );
    CEnum::WriteEnum( pOB, m_eOrderType, eLAYER_ORDER_TYPE_MAX, g_pArrLabelLayerOrderType );
    CEnum::WriteEnum( pOB, m_eOrderSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );

    CEnum::WriteEnum( pOB, m_eOption, eBD_OPTION_MAX, g_pArrLabelBdOption );
    
    pOB->writeInt16( (int16)m_nRotPowRate );
    
    CEnum::WriteEnum( pOB, m_eDelayType, eDELAY_LOG_MAX, g_pArrLabelDelayLog );
    pOB->writeInt16( (int16)m_nDelayDepth );
    pOB->writeInt16( (int16)m_nDelayPowerRateX );
    pOB->writeInt16( (int16)m_nDelayPowerRateY );
    pOB->writeInt16( (int16)m_nDelayPowerRateXForRot );
    pOB->writeInt16( (int16)m_nDelayPowerRateYForRot );
    pOB->writeInt16( (int16)m_nDelayPowerRateXForScale );
    pOB->writeInt16( (int16)m_nDelayPowerRateYForScale );
}

//--------------------------------
// 追加（※基本）
//--------------------------------
CLayerObject* CLayerData::addData( void ){
    // 追加対象の作成（※選択中の要素が有効であれば追加要素にコピーするのが[CDataList]での仕様）
    CLayerObject* pNew = copyData();

    //  コピーに失敗していたら無視
    if( pNew == NULL ){
        return( NULL );
    }
    
    return( addData( pNew ) );
}

//--------------------------------
// 追加: LineObjectData
//--------------------------------
CLayerObject* CLayerData::addLineObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CLineObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 追加: PaintObjectData
//--------------------------------
CLayerObject* CLayerData::addPaintObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CPaintObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 追加: SlotObjectData
//--------------------------------
CLayerObject* CLayerData::addSlotObjectData( void ){
    CLayerObject* pNew = (CLayerObject*) CSlotObjectData::Alloc();
    return( addData( pNew ) );
}

//--------------------------------
// 削除
//--------------------------------
void CLayerData::deleteData( void ){
    CLayerObject* pObj = (CLayerObject*) deleteData0();
    CLayerObject::Free( pObj );
}

//--------------------------------
// コピー
//--------------------------------
CLayerObject* CLayerData::copyData( void ){
    CLayerObject* pObj = getSelectedData();
    
    if( pObj != NULL ){
        if(  pObj->isLineObject() ){
            CLineObjectData* pLineObj = CLineObjectData::Alloc();
            pLineObj->copy( (CLineObjectData*)pObj );
            return( (CLayerObject*) pLineObj );
        }
        
        if( pObj->isPaintObject() ){
            CPaintObjectData* pPaintObj = CPaintObjectData::Alloc();
            pPaintObj->copy( (CPaintObjectData*)pObj );
            return( (CLayerObject*) pPaintObj );
        }
        
        if( pObj->isSlotObject() ){
            CSlotObjectData* pSlotObj = CSlotObjectData::Alloc();
            pSlotObj->copy( (CSlotObjectData*)pObj );
            return( (CLayerObject*) pSlotObj );
        }
    }
    
    return( NULL );
}

//--------------------------------
// ペースト
//--------------------------------
CLayerObject* CLayerData::pasteData( CLayerObject* pObjCopy ){
    CLayerObject* pObj = getSelectedData();
    CLayerObject* pNew = NULL;
    if( pObj != NULL && pObjCopy != NULL ){
        if( pObjCopy->isLineObject() ){
            CLineObjectData* pLineObj = CLineObjectData::Alloc();
            pLineObj->copy( (CLineObjectData*)pObjCopy );
            pNew = (CLayerObject*) pLineObj;
        }else if( pObjCopy->isPaintObject() ){
            CPaintObjectData* pPaintObj = CPaintObjectData::Alloc();
            pPaintObj->copy( (CPaintObjectData*)pObjCopy );
            pNew = (CLayerObject*) pPaintObj;
        }else if( pObjCopy->isSlotObject() ){
            CSlotObjectData* pSlotObj = CSlotObjectData::Alloc();
            pSlotObj->copy( (CSlotObjectData*)pObjCopy );
            pNew = (CLayerObject*) pSlotObj;
        }

        // 追加した上で対象を削除（※タイプが違うデータには上書きできないので）
        if( pNew != NULL ){
            int select = getSelect();
            addData( pNew );
            setSelect( select );    // 選択を戻す（※追加により１つ下へ移動するので）
            deleteData();
            setSelect( select );    // 選択を戻す（※削除により１つ下へ移動するので）
        }
    }
    
    return( pNew );
}

//--------------
// 描画
//--------------
int CLayerData::draw( stBEZIER_BASE_PARAM* pBaseParam, stBEZIER_LAYER_PARAM* pLayerParam, bool isWorkPath ){
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL---[START:%s] CLayerData::draw\n", m_cArrName );
    }
#endif

    // work
    int timeForLine = 0;
    int timeForPaint = 0;

    //--------------------------
    // レイヤー内のオブジェクトを走査
    //--------------------------
    CLayerObject* pObj = getDataHead();
    while( pObj != NULL ){
        //==================================================================
        // ライン：CLineObjectData
        //==================================================================
        if( pObj->isLineObject() ){
            CLineObjectData* pLOD = pObj->getLineObject();
            
            // オブジェクトが有効であれば
            if( ! pLOD->checkFlag( eLOD_FLAG_DISABLE ) ){
                //--------------------------
                // ライン：事前処理
                //--------------------------
                // 指定があればガイドの復旧（※ワークパス時は無視）
                eSTROKE_GUIDE_TARGET guideTarget = pLOD->getGuideTargetId( pLayerParam->slotIndex );
                if( IS_STROKE_GUIDE_TARGET_VALID( guideTarget ) ){
                    if( ! isWorkPath ){
                        CBezier::RecoverGuide( guideTarget );
                    }
                }

                //--------------------------
                // ライン：描画
                //--------------------------
                // アンカーポイントを描画形式データに変換
                stBEZIER_ANCHOR_POINT* pHead = CBezierDataConv::CreateBezierAnchorPoint( pLOD, pBaseParam, pLayerParam );

                // タッチであれば
                if( pLOD->checkFlag( eLOD_FLAG_TOUCH ) ){
                    // フリルであれば
                    if( pLOD->checkFlag( eLOD_FLAG_TOUCH_IS_FRILL ) ){
                        timeForLine += CBezier::TouchForFrill( pHead, pBaseParam, pLayerParam, pLOD->getTestPalGrp(), isWorkPath );
                    }
                    // 通常のタッチ／ストライプであれば
                    else{
                        bool isStripe = pLOD->checkFlag( eLOD_FLAG_TOUCH_IS_STRIPE );
                        timeForLine += CBezier::Touch( pHead, pBaseParam, pLayerParam, pLOD->getTestPalGrp(), isStripe, isWorkPath );
                    }
                }
                // ストロークであれば
                else{
                    // ドットか？（※指定があれば線ではなく点を描く）
                    bool isDot = pLOD->checkFlag( eLOD_FLAG_DOT );

                    timeForLine += CBezier::Stroke( pHead, pBaseParam, pLayerParam,
                                                    pLOD->getTestPalGrp(), pLOD->getTestPalGrpForRepair(), isDot, isWorkPath );
                }
                
                // 描画形式データを解放
                CBezierDataConv::ReleaseBezierAnchorPoint( pHead );

                //--------------------------
                // ライン：後始末
                //--------------------------
                // 指定があればガ出力色をフォーカス（※出力色をもちこすガイドリセット）
                if( pLOD->checkFlag( eLOD_FLAG_FOCUS_OUT_COL_AFTER_STROKE ) ){
                    CFill::AdjustFillGuideForOutCol( pBaseParam->pBufFillGuide, pBaseParam->texW, pBaseParam->texH );
                }
                // 指定があればガイドのリセット
                else if( pLOD->checkFlag( eLOD_FLAG_RESET_GUIDE_AFTER_STROKE ) ){
                    CBmpGenerator::ResetFillGuideBuf();
                }

                // 指定があればマスクのリセット
                if( pLOD->checkFlag( eLOD_FLAG_RESET_MASK_AFTER_STROKE ) ){
                    CBmpGenerator::ResetFillGuardBuf();
                }
            }
        }
        //==================================================================
        // ペイント：CPaintObjectData
        //==================================================================
        else if( pObj->isPaintObject() ){
            CPaintObjectData* pPOD = pObj->getPaintObject();
            
            // オブジェクトが有効であれば
            if( ! pPOD->checkFlag( ePOD_FLAG_DISABLE ) ){
                //--------------------------
                // ペイント：事前処理
                //--------------------------
                // 指定があればガイドの復旧（※ワークパス時は無視）
                eSTROKE_GUIDE_TARGET guideTarget = pPOD->getGuideTargetId( pLayerParam->slotIndex );
                if( IS_STROKE_GUIDE_TARGET_VALID( guideTarget ) ){
                    if( ! isWorkPath ){
                        CBezier::RecoverGuide( guideTarget );
                    }
                }

                //--------------------------
                // ペイント：描画
                //--------------------------
                stBEZIER_FILL_POINT* pHead = CBezierDataConv::CreateBezierFillPoint( pPOD, pBaseParam, pLayerParam );
                timeForPaint += CBezier::Fill( pHead, pBaseParam, pLayerParam, pPOD->getTestPalGrp(), isWorkPath );
                CBezierDataConv::ReleaseBezierFillPoint( pHead );

                //--------------------------
                // ペイント：後始末
                //--------------------------
                // 指定があればガ出力色をフォーカス（※出力色をもちこすガイドリセット）
                if( pPOD->checkFlag( ePOD_FLAG_FOCUS_OUT_COL_AFTER_FILL ) ){
                    CFill::AdjustFillGuideForOutCol( pBaseParam->pBufFillGuide, pBaseParam->texW, pBaseParam->texH );
                }
                // 塗りガイドのリセット（※指定があれば持ち越す）
                else if( ! pPOD->checkFlag( ePOD_FLAG_STAY_GUIDE_AFTER_FILL ) ){
                    CBmpGenerator::ResetFillGuideBuf();
                }
                
                // 指定があればマスクのリセット
                if( pPOD->checkFlag( ePOD_FLAG_RESET_MASK_AFTER_FILL ) ){
                    CBmpGenerator::ResetFillGuardBuf();
                }
            }
        }
        //==============================================================================
        // スロット：CSlotObjectData
        // スロットはパーツ間の呼び出しを制御するものなので、この時点で役割を終えている（※描画要素は無い）
        //==============================================================================
        else if( pObj->isSlotObject() ){
            CSlotObjectData* pSOD = pObj->getSlotObject();

            // オブジェクトが有効であれば
            if( ! pSOD->checkFlag( eSOD_FLAG_DISABLE ) ){
                // ワークパス指定があれば開発用に表示
                if( isWorkPath ){
                    stBEZIER_SLOT_POINT* pHead = CBezierDataConv::CreateBezierSlotPoint( pSOD, pBaseParam, pLayerParam );
                    CBezier::SlotForDev( pHead, pBaseParam );
                    CBezierDataConv::ReleaseBezierSlotPoint( pHead );
                }
            }
        }
        //===========================================================================
        // ここまできたら困る
        //===========================================================================
        else{
            LOGE( "@ CLayerData::draw: UNKNOWN OBJECT: type=%d\n", pObj->getObjectType() );
        }

        // 次のオブジェクトへ
        pObj = pObj->getNext();
    }
    
    //----------------------------
    // レイヤー：後始末
    //----------------------------
    // 一時フックのリセット（※指定があれば持ち越す）
    if( ! checkFlag( eLAYER_FLAG_STAY_HOOK_TEMP ) ){
        CBezier::ResetHookPoint( false );
    }

    // 一時タッチのリセット（※指定があれば持ち越す）
    if( ! checkFlag( eLAYER_FLAG_STAY_TOUCH_TEMP ) ){
        CBezier::ResetTouchPoint( false );
    }

    // 一時ガイドのリセット（※指定があれば持ち越す）
    if( ! checkFlag( eLAYER_FLAG_STAY_GUIDE_TEMP ) ){
        CBezier::ResetGuidePoint( false );
    }

    //----------------------------
    // レイヤー：処理時間
    //----------------------------
    int total = timeForLine + timeForPaint;

#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL---[FINISH:%s] CLayerData::draw: %d(%d+%d)us\n", m_cArrName, total, timeForLine, timeForPaint );
    }
#endif
    
    return( total );
}

//-----------------------------
// スケール適用
//-----------------------------
void CLayerData::applyRateScale( int rateScale ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_cArrName]は無視
    // [m_eOrderType]は無視
    // [m_eOrderSlot]は無視

    // [m_eOption]は無視

    // [m_nRotPowRate]は無視（割合なので）

    // [m_eDelayType]は無視
    // [m_eDelayDepth]は無視
    m_nDelayPowerRateX = m_nDelayPowerRateX*rateScale/BEZIER_SCALE_RATE;
    m_nDelayPowerRateY = m_nDelayPowerRateY*rateScale/BEZIER_SCALE_RATE;
    // [m_nDelayPowerRateXForRot]は無視（角度なので）
    // [m_nDelayPowerRateYForRot]は無視（角度なので）
    // [m_nDelayPowerRateXForScale]は無視（割合なので）
    // [m_nDelayPowerRateYForScale]は無視（割合なので）
}

//---------------------------------------------------------
// 編集メニューに関連付け
//---------------------------------------------------------
void CLayerData::setEditValueMenu( CEditValueMenu* pMenu ){
    // 編集項目数設定
    pMenu->setItemNum( 4 + 8 + eLAYER_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "ORDER_TYPE", &m_eOrderType, eEDIT_VALUE_TYPE_INT32,
                             eLAYER_ORDER_TYPE_INVALID, eLAYER_ORDER_TYPE_MAX-1, g_pArrLabelLayerOrderType );

    pMenu->setItemAtAsLabel( id++, "ORDER_SLOT", &m_eOrderSlot, eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "OPTION", &m_eOption, eEDIT_VALUE_TYPE_INT32,
                             eBD_OPTION_INVALID, eBD_OPTION_MAX-1, g_pArrLabelBdOption );
    
    pMenu->setItemAt( id++, "ROT_POW_RATE", &m_nRotPowRate, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    // 遅延調整
    pMenu->setItemAtAsLabel( id++, "DLY: TYPE", &m_eDelayType, eEDIT_VALUE_TYPE_INT32,
                             eDELAY_LOG_INVALID, eDELAY_LOG_MAX-1, g_pArrLabelDelayLog );

    pMenu->setItemAt( id++, "DLY: DEPTH", &m_nDelayDepth, eEDIT_VALUE_TYPE_INT32, 0, DELAY_LOG_DEPTH );
    
    pMenu->setItemAt( id++, "DLY: POW_RATE_X", &m_nDelayPowerRateX, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "DLY: POW_RATE_Y", &m_nDelayPowerRateY, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "DLY: POW_RATE_OFS_ROT_X", &m_nDelayPowerRateXForRot, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "DLY: POW_RATE_OFS_ROT_Y", &m_nDelayPowerRateYForRot, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "DLY: POW_RATE_OFS_SCALE_X", &m_nDelayPowerRateXForScale, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setItemAt( id++, "DLY: POW_RATE_OFS_SCALE_Y", &m_nDelayPowerRateYForScale, eEDIT_VALUE_TYPE_INT32,
                      BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_HOOK_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_HOOK_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_TOUCH_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_TOUCH_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_GUIDE_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_GUIDE_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: INVALID FOR SUITED", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_INVALID_FOR_SUITED );
    pMenu->setItemAtAsBit( id++, "FLAG: VALID FOR SUITED", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_VALID_FOR_SUITED );
    pMenu->setItemAtAsBit( id++, "FLAG: OPEN_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_OPEN_COVER );
    pMenu->setItemAtAsBit( id++, "FLAG: CLOSE_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_CLOSE_COVER );

    // 確定
    pMenu->fixMenu();
}
