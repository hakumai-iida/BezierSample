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

#include "draw/bezier/struct/BezierStructConv.hpp"

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
    clearTempAdjust();
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
    m_eOrderType = eBD_LAYER_ORDER_TYPE_NORMAL;
    m_eOrderSlot = eBD_SLOT_DEFAULT;
    m_eOrderSlotForCovered = eBD_SLOT_DEFAULT;
    m_eOrderGroup = eBD_LAYER_ORDER_GROUP_DEFAULT;
    m_eMaskTarget = eBD_MASK_TARGET_DEFAULT;

    m_eOption = eBD_OPTION_INVALID;
    m_eHookTargetId = eSTROKE_HOOK_TARGET_INVALID;
    m_eHookTargetIdForMC = eSTROKE_HOOK_TARGET_INVALID;
    
    m_nHookWorkRateOfsX = 0;
    m_nHookWorkRateOfsY = 0;
    
    m_nRotPowRate = BEZIER_SCALE_RATE;
    
    m_nMinRateForT = 0;
    m_nMaxRateForT = 0;
    
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
    m_eOrderSlotForCovered = pData->m_eOrderSlotForCovered;
    m_eOrderGroup = pData->m_eOrderGroup;
    m_eMaskTarget = pData->m_eMaskTarget;

    m_eOption = pData->m_eOption;
    m_eHookTargetId = pData->m_eHookTargetId;
    m_eHookTargetIdForMC = pData->m_eHookTargetIdForMC;
    
    m_nHookWorkRateOfsX = pData->m_nHookWorkRateOfsX;
    m_nHookWorkRateOfsY = pData->m_nHookWorkRateOfsY;

    m_nRotPowRate = pData->m_nRotPowRate;

    m_nMinRateForT = pData->m_nMinRateForT;
    m_nMaxRateForT = pData->m_nMaxRateForT;

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
    m_eOrderType = (eBD_LAYER_ORDER_TYPE)CEnum::ReadEnum( pIB, g_pArrLabelBdLayerOrderType );
    m_eOrderSlot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_eOrderSlotForCovered = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
    m_eOrderGroup = (eBD_LAYER_ORDER_GROUP)CEnum::ReadEnum( pIB, g_pArrLabelBdLayerOrderGroup );
    m_eMaskTarget = (eBD_MASK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelBdMaskTarget );

    m_eOption = (eBD_OPTION)CEnum::ReadEnum( pIB, g_pArrLabelBdOption );
    m_eHookTargetId = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );
    m_eHookTargetIdForMC = (eSTROKE_HOOK_TARGET)CEnum::ReadEnum( pIB, g_pArrLabelStrokeHookTarget );
    
    m_nHookWorkRateOfsX = pIB->readInt16();
    m_nHookWorkRateOfsY = pIB->readInt16();
    
    m_nRotPowRate = pIB->readInt16();
    
    m_nMinRateForT = pIB->readInt16();
    m_nMaxRateForT = pIB->readInt16();
    
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
    CEnum::WriteEnum( pOB, m_eOrderType, eBD_LAYER_ORDER_TYPE_MAX, g_pArrLabelBdLayerOrderType );
    CEnum::WriteEnum( pOB, m_eOrderSlot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    CEnum::WriteEnum( pOB, m_eOrderSlotForCovered, eBD_SLOT_MAX, g_pArrLabelBdSlot );
    CEnum::WriteEnum( pOB, m_eOrderGroup, eBD_LAYER_ORDER_GROUP_MAX, g_pArrLabelBdLayerOrderGroup );
    CEnum::WriteEnum( pOB, m_eMaskTarget, eBD_MASK_TARGET_MAX, g_pArrLabelBdMaskTarget );

    CEnum::WriteEnum( pOB, m_eOption, eBD_OPTION_MAX, g_pArrLabelBdOption );
    CEnum::WriteEnum( pOB, m_eHookTargetId, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    CEnum::WriteEnum( pOB, m_eHookTargetIdForMC, eSTROKE_HOOK_TARGET_MAX, g_pArrLabelStrokeHookTarget );
    
    pOB->writeInt16( (int16)m_nHookWorkRateOfsX );
    pOB->writeInt16( (int16)m_nHookWorkRateOfsY );

    pOB->writeInt16( (int16)m_nRotPowRate );
    
    pOB->writeInt16( (int16)m_nMinRateForT );
    pOB->writeInt16( (int16)m_nMaxRateForT );
    
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
// ビットマスク取得
//--------------------------------
BYTE CLayerData::getBitMask( eBD_SLOT slot, int slotIndex ){
    return( CBDConst::GetBitMaskForSlotIndex( m_eMaskTarget, slot, slotIndex ) );
}

//--------------------------------
// HOOK対象
//--------------------------------
int CLayerData::fixHookTargetId( eBD_SLOT slot, int slotIndex, bool isMC ){
    eSTROKE_HOOK_TARGET target = ((isMC)? m_eHookTargetIdForMC: m_eHookTargetId);
    return( CStroke::AdjustHookTargetForSlotIndex( target, slot, slotIndex ) );
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

    //----------------------------------------------
    // レイヤーのフック
    //----------------------------------------------
    bool isHookOfsValid = true;
    
    // 各オブジェクトが参照する位置（※ミラーコピー時にフック先を割り振るための歪な処理）
    float pixelOfsX0 = pLayerParam->pixelOfsX;
    float pixelOfsY0 = pLayerParam->pixelOfsY;
    float pixelOfsX = pLayerParam->pixelOfsX;
    float pixelOfsY = pLayerParam->pixelOfsY;
    float pixelOfsXForMC = pLayerParam->pixelOfsX;
    float pixelOfsYForMC = pLayerParam->pixelOfsY;

    // 大きくなる可能性のあるレイヤーを個別パーツ表示する場合は適用しない（※描画バッファの領域外に達する恐れがあるので）
    if( checkFlag( eLAYER_FLAG_TOO_LARGE ) && CBezier::IsDispIndividualPart() ){
        isHookOfsValid = false;
    }
    
    // あらかじめフラグを寝かせておく
    CBezier::SetDispHookOfsValid( false );
    CBezier::SetDispHookOfsInvalid( false );

    //------------------------------------------------------------------------------------------------
    // 指定のフック点が有効ならレイヤー座標を差し替える
    // パーツ原点から離れたレイヤーを回転させたり、タイプ３のポニーテールのように巨大な要素を分割作成したりする使い方を想定
    //-------------------------------------------------------------------------------------------------
    if( isHookOfsValid ){
        int idFixed = fixHookTargetId( pLayerParam->slot, pLayerParam->slotIndex, false );
        if( IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
            if( CBezier::IsHookPointRegistered( idFixed ) ){
                pixelOfsX = CBezier::GetHookPointX( idFixed );
                pixelOfsY = CBezier::GetHookPointY( idFixed );

                // 表示の差別化（※フックオフセットが有効）
                CBezier::SetDispHookOfsValid( true );
            }else{
                // TODO:ワークスケール等を加味すべきか？（※このままでも作業はできるので気が向けば対応）
                pixelOfsX += CConst::ConvBezierPositionRate( pBaseParam->texW0, m_nHookWorkRateOfsX );
                pixelOfsY += CConst::ConvBezierPositionRate( pBaseParam->texH0, m_nHookWorkRateOfsY );
                
                // 表示の差別化（※フックオフセットが無効）
                CBezier::SetDispHookOfsInvalid( true );
            }
        }

        // ミラーコピー用
        idFixed = fixHookTargetId( pLayerParam->slot, pLayerParam->slotIndex, true );
        if( IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
            if( CBezier::IsHookPointRegistered( idFixed ) ){
                pixelOfsXForMC = CBezier::GetHookPointX( idFixed );
                pixelOfsYForMC = CBezier::GetHookPointY( idFixed );
            }else{
                pixelOfsXForMC -= CConst::ConvBezierPositionRate( pBaseParam->texW0, m_nHookWorkRateOfsX );
                pixelOfsYForMC += CConst::ConvBezierPositionRate( pBaseParam->texH0, m_nHookWorkRateOfsY );
            }
        }
    }

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
                // ミラーコピーフックのクリア
                CBezier::ClearHookPoint( eSTROKE_HOOK_TARGET_MC_HEAD );
                CBezier::ClearHookPoint( eSTROKE_HOOK_TARGET_MC_TAIL );

                int loop = 0;   // ミラーコピー指定があれば処理を２周する
                do{
                    bool isMC = (loop==1);
                    
                    //--------------------------
                    // ライン：事前処理
                    //--------------------------
                    // 指定があればガイドの復旧（※ワークパス時は無視）
                    int guideTargetIdFixed = pLOD->fixGuideTargetId( pLayerParam->slotIndex, isMC );
                    if( IS_STROKE_GUIDE_TARGET_FIXED_VALID( guideTargetIdFixed ) ){
                        if( ! isWorkPath ){
                            CBezier::RecoverGuide( guideTargetIdFixed );
                        }
                    }

                    //--------------------------
                    // ライン：描画
                    //--------------------------
                    // アンカーポイントを描画形式データに変換
                    pLayerParam->pixelOfsX = ((isMC)? pixelOfsXForMC: pixelOfsX);
                    pLayerParam->pixelOfsY = ((isMC)? pixelOfsYForMC: pixelOfsY);
                    stBEZIER_ANCHOR_POINT* pHead = CBezierStructConv::CreateBezierAnchorPoint( pLOD, pBaseParam, pLayerParam, isMC );

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
                    CBezierStructConv::ReleaseBezierAnchorPoint( pHead );
                    
                    //-------------------------
                    // タッチ合成
                    //-------------------------
                    int destIdFixed = pLOD->fixTouchTargetDestId( pLayerParam->slotIndex, isMC );
                    if( IS_STROKE_TOUCH_TARGET_FIXED_VALID( destIdFixed ) ){
                        int optIdFixed = pLOD->fixTouchTargetOptId( pLayerParam->slotIndex, isMC );
                        bool isDestMinus = pLOD->checkFlag( eLOD_FLAG_TOUCH_DEST_MINUS );
                        bool isOptMinus = pLOD->checkFlag( eLOD_FLAG_TOUCH_OPT_MINUS );

                        CBezier::MergeTouchPoint( destIdFixed, isDestMinus, optIdFixed, isOptMinus );
                    }

                    loop++;
                }while( pLOD->checkFlag( eLOD_FLAG_MIRROR_COPY ) && loop==1 );

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
                int loop = 0;   // ミラーコピー指定があれば処理を２周する
                do{
                    bool isMC = (loop==1);
                    //--------------------------
                    // ペイント：描画
                    //--------------------------
                    pLayerParam->pixelOfsX = ((isMC)? pixelOfsXForMC: pixelOfsX);
                    pLayerParam->pixelOfsY = ((isMC)? pixelOfsYForMC: pixelOfsY);
                    stBEZIER_FILL_POINT* pHead = CBezierStructConv::CreateBezierFillPoint( pPOD, pBaseParam, pLayerParam, isMC );
                    timeForPaint += CBezier::Fill( pHead, pBaseParam, pLayerParam, pPOD->getTestPalGrp(), isWorkPath );
                    
                    CBezierStructConv::ReleaseBezierFillPoint( pHead );
                    
                    // 指定があれば直近色のクリア（※ミラー側の明暗予約の際に、ミラー元の完了色が重複処理されないように）
                    if( pPOD->checkFlag( ePOD_FLAG_RESET_LAST_IF_MIRROR ) && !isMC ){
                        CFill::ResetFillGuideForLastCol( pBaseParam->pBufFillGuide, pBaseParam->texW, pBaseParam->texH );
                    }
                    
                    loop++;
                }while( pPOD->checkFlag( ePOD_FLAG_MIRROR_COPY ) && loop==1 );

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
                int loop = 0;   // ミラーコピー指定があれば処理を２周する
                do{
                    bool isMC = (loop==1);
                    
                    // ワークパス指定があれば開発用に表示
                    if( isWorkPath ){
                        pLayerParam->pixelOfsX = ((isMC)? pixelOfsXForMC: pixelOfsX);
                        pLayerParam->pixelOfsY = ((isMC)? pixelOfsYForMC: pixelOfsY);
                        stBEZIER_SLOT_POINT* pHead = CBezierStructConv::CreateBezierSlotPoint( pSOD, pBaseParam, pLayerParam, isMC );
                        CBezier::SlotForDev( pHead, pBaseParam );
                        CBezierStructConv::ReleaseBezierSlotPoint( pHead );
                    }
                    
                    loop++;
                }while( pSOD->checkFlag( eSOD_FLAG_MIRROR_COPY ) && loop==1 );
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
    
    // 用心に復旧しておく
    pLayerParam->pixelOfsX = pixelOfsX0;
    pLayerParam->pixelOfsY = pixelOfsY0;

    return( total );
}

//---------------------------
// 一時調整の値を適用する
//---------------------------
// 調整値適用：基本
void CLayerData::applyTempAdjust( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyTempAdjust( m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY, m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pCell = pCell->getNext();
    }

    // 調整値クリア
    clearTempAdjust();
}

// 調整値適用：Ｈ最大
void CLayerData::applyTempAdjustForMaxH( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyTempAdjustForMaxH( m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY, m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pCell = pCell->getNext();
    }

    clearTempAdjust();
}

// 調整値適用：Ｈ最小
void CLayerData::applyTempAdjustForMinH( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyTempAdjustForMinH( m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY, m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pCell = pCell->getNext();
    }

    clearTempAdjust();
}

// 調整値適用：Ｖ最大
void CLayerData::applyTempAdjustForMaxV( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyTempAdjustForMaxV( m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY, m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pCell = pCell->getNext();
    }

    clearTempAdjust();
}

// 調整値適用：Ｖ最小
void CLayerData::applyTempAdjustForMinV( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyTempAdjustForMinV( m_nTempAdjustRateOfsX, m_nTempAdjustRateOfsY, m_nTempAdjustRateScaleX, m_nTempAdjustRateScaleY );
        pCell = pCell->getNext();
    }

    clearTempAdjust();
}

//-----------------------------
// 調整値クリア
//-----------------------------
void CLayerData::clearAdjust( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->clearAdjust();
        pCell = pCell->getNext();
    }

    m_eDelayType = eDELAY_LOG_INVALID;
    m_nDelayDepth = 0;
    m_nDelayPowerRateX = m_nDelayPowerRateY = 0;
    m_nDelayPowerRateXForRot = m_nDelayPowerRateYForRot = 0;
    m_nDelayPowerRateXForScale = m_nDelayPowerRateYForScale = 0;
}

//-----------------------------
// オブジェクトの一時調整値が有効か？
//-----------------------------
bool CLayerData::checkTempAdjustForNodes( void ){
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        if( pCell->isTempAdjustValid() ){
            return( true );
        }
        pCell = pCell->getNext();
    }
    
    return( false );
}

//-----------------------------
// スケール適用
//-----------------------------
void CLayerData::applyRateScale( int rateScale ){
    // [m_nFlag]は無視
    
    // [m_oDataList]
    CLayerObject* pCell = getDataHead();
    while( pCell != NULL ){
        pCell->applyRateScale( rateScale );
        pCell = pCell->getNext();
    }
    
    // [m_cArrName]は無視
    // [m_eOrderType]は無視
    // [m_eOrderSlot]は無視
    // [m_eOrderSlotForCovered]は無視
    // [m_eOrderGroup]は無視
    // [m_eMaskTarget]は無視

    // [m_eOption]は無視
    // [m_eHookTargetId]は無視
    // [m_eHookTargetIdForMC]は無視
    // [m_nHookWorkRateOfsX]は無視
    // [m_nHookWorkRateOfsY]は無視
    
    // [m_nRotPowRate]は無視（割合なので）
    
    // [m_nMinRateForT]は無視
    // [m_nMaxRateForT]は無視

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
    pMenu->setItemNum( 13 + 8 + 4 + eLAYER_FLAG_MAX );
    
    int id = 0;
    
    // 値
    pMenu->setItemAtAsLabel( id++, "ORDER_TYPE", &m_eOrderType, eEDIT_VALUE_TYPE_INT32,
                             eBD_LAYER_ORDER_TYPE_INVALID, eBD_LAYER_ORDER_TYPE_MAX-1, g_pArrLabelBdLayerOrderType );

    pMenu->setItemAtAsLabel( id++, "ORDER_SLOT", &m_eOrderSlot, eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );
    
    pMenu->setItemAtAsLabel( id++, "O_SLOT(COVERED)", &m_eOrderSlotForCovered, eEDIT_VALUE_TYPE_INT32,
                             eBD_SLOT_INVALID, eBD_SLOT_MAX-1, g_pArrLabelBdSlot );

    pMenu->setItemAtAsLabel( id++, "ORDER_GROUP", &m_eOrderGroup, eEDIT_VALUE_TYPE_INT32,
                             eBD_LAYER_ORDER_GROUP_INVALID, eBD_LAYER_ORDER_GROUP_MAX-1, g_pArrLabelBdLayerOrderGroup );
    
    pMenu->setItemAtAsLabel( id++, "MASK_TARGET", &m_eMaskTarget, eEDIT_VALUE_TYPE_INT32,
                             eBD_MASK_TARGET_INVALID, eBD_MASK_TARGET_MAX-1, g_pArrLabelBdMaskTarget );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "OPTION", &m_eOption, eEDIT_VALUE_TYPE_INT32,
                             eBD_OPTION_INVALID, eBD_OPTION_MAX-1, g_pArrLabelBdOption );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAtAsLabel( id++, "HOOK ", &m_eHookTargetId, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );

    pMenu->setItemAtAsLabel( id++, "HOOK(MC) ", &m_eHookTargetIdForMC, eEDIT_VALUE_TYPE_INT32,
                             eSTROKE_HOOK_TARGET_INVALID, eSTROKE_HOOK_TARGET_MAX-1, g_pArrLabelStrokeHookTarget );
    
    pMenu->setItemAt( id++, "HOOK_WORK_X", &m_nHookWorkRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );

    pMenu->setItemAt( id++, "HOOK_WORK_Y", &m_nHookWorkRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );

    pMenu->setSeparatorAt( id, true );

    pMenu->setItemAt( id++, "ROT_POW_RATE", &m_nRotPowRate, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    pMenu->setItemAt( id++, "T: MIN_R", &m_nMinRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );
    pMenu->setItemAt( id++, "T: MAX_R", &m_nMaxRateForT, eEDIT_VALUE_TYPE_INT32, BEZIER_ROTATION_RATE_MIN, BEZIER_ROTATION_RATE_MAX );

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
    
    // 一時調整値（※この値は保存されない＝修正後に[applyTempAdjust]で調整を適用する想定）
    pMenu->setItemAt( id++, "TEMP: X", &m_nTempAdjustRateOfsX, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: Y", &m_nTempAdjustRateOfsY, eEDIT_VALUE_TYPE_INT32, BEZIER_POSITION_RATE_MIN, BEZIER_POSITION_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_X", &m_nTempAdjustRateScaleX, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );
    pMenu->setItemAt( id++, "TEMP: SCL_Y", &m_nTempAdjustRateScaleY, eEDIT_VALUE_TYPE_INT32, BEZIER_SCALE_RATE_MIN, BEZIER_SCALE_RATE_MAX );

    pMenu->setSeparatorAt( id, true );
    
    // フラグ
    pMenu->setItemAtAsBit( id++, "FLAG: DISABLE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_DISABLE );
    pMenu->setItemAtAsBit( id++, "FLAG: PRE_DRAW", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_PRE_DRAW );
    pMenu->setItemAtAsBit( id++, "FLAG: INVALID_IF_COVERED", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_INVALID_IF_COVERED );
    pMenu->setItemAtAsBit( id++, "FLAG: VALID_IF_COVERED", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_VALID_IF_COVERED );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_HOOK_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_HOOK_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_TOUCH_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_TOUCH_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: STAY_GUIDE_TEMP", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_STAY_GUIDE_TEMP );
    pMenu->setItemAtAsBit( id++, "FLAG: TOO_LARGE", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_TOO_LARGE );
    pMenu->setItemAtAsBit( id++, "FLAG: MASK_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_MASK_COVER );
    pMenu->setItemAtAsBit( id++, "FLAG: FRONT_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_FRONT_COVER );
    pMenu->setItemAtAsBit( id++, "FLAG: BACK_COVER", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_BACK_COVER );
    pMenu->setItemAtAsBit( id++, "FLAG: OVER_HAIR", &m_nFlag, eEDIT_VALUE_TYPE_INT32, eLAYER_FLAG_OVER_HAIR );

    // 確定
    pMenu->fixMenu();
}
