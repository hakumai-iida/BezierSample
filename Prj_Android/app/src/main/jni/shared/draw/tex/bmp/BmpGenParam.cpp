/*+----------------------------------------------------------------+
  |	Title:		BmpGenData.cpp [共通環境]
  |	Comment:	BMP生成パラメータ（※BMP生成に必要な全スロット情報を保持する）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpGenParam.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// コンストラクタ
//-----------------------
CBmpGenParam::CBmpGenParam( void ){
	clear();
}

//-----------------------
// クリア
//-----------------------
void CBmpGenParam::clear( void ){
    m_eForm = eBD_FORM_INVALID;
    m_bSuited = false;
    
    // セルは要素数をクリアするのみ
    m_nCellNumUse = 0;
}

//-----------------------
// セルを指定してクリア
//-----------------------
void CBmpGenParam::clearCell( eBD_SLOT slot, int slotIndex ){
	// 指定の登録があれば無効化（※間詰め）
	for( int i=0; i<m_nCellNumUse; i++ ){
		if( CHECK_BMP_GEN_PARAM_CELL( &m_stArrCell[i], slot, slotIndex ) ){
            for( int j=i+1; j<m_nCellNumUse; j++ ){
                m_stArrCell[j-1] = m_stArrCell[j];
            }
            m_nCellNumUse--;
            i--;
		}
	}
}

//-----------------------
// ワークフラグリセット
//-----------------------
void CBmpGenParam::resetForWork( void ){
    for( int i=0; i<m_nCellNumUse; i++ ){
        stBMP_GEN_PARAM_CELL* pCell = &m_stArrCell[i];
        pCell->isDone = false;
    }
}

//-------------------------------
// 指定パラメータでパーツを追加する
//-------------------------------
stBMP_GEN_PARAM_CELL* CBmpGenParam::addCell( stBD_PART_SEARCH_PARAM* pParam, int slotIndex, float adjForT, float adjForS ){
    stBD_PART_SEARCH_PARAM paramResult; // パーツの検索結果を受け取る
    CBmpDotPartData* pPart = CBmpDotMgr::SearchBmpDotPartDataWithDefault( pParam, &paramResult );
    
    // 無効は無視
    if( pPart == NULL ){
        return( NULL );
    }

#if 0
    // 同じ設定の登録があるのは何か変（※エラーとしておく）
	for( int i=0; i<m_nCellNumUse; i++ ){
		if( CHECK_BMP_GEN_PARAM_CELL( &m_stArrCell[i], pParam->slot, slotIndex ) ){
            LOGE( "@ CBmpGenParam::addCell: DUPLICATED CELL: at=%d, slot=%s, slotIndex=%d\n",
                  i, g_pArrLabelBdSlot[pParam->slot], slotIndex );
            return( NULL );
		}
	}
#endif

    // 空き枠がないのは困る
    if( m_nCellNumUse >= BMP_GEN_PARAM_CELL_MAX ){
        LOGE( "@ CBmpGenParam::addCell: BUF SHORTAGE: use=%d/%d, slot=%s, slotIndex=%d\n",
              m_nCellNumUse, BMP_GEN_PARAM_CELL_MAX, g_pArrLabelBdSlot[pParam->slot], slotIndex );
        return( NULL );
    }

    // 追加先の取得
    stBMP_GEN_PARAM_CELL* pCell = &m_stArrCell[m_nCellNumUse++];
    CLEAR_BMP_GEN_PARAM_CELL( pCell );  // 用心にクリア

    //-----------------------------
	// 設定
    //-----------------------------
    pCell->pData = pPart;
    pCell->slot = paramResult.slot;
    pCell->slotIndex = slotIndex;
    pCell->adjForT = adjForT;
    pCell->adjForS = adjForS;

    //-----------------------------
    // 参照データの確認
    //-----------------------------
    CBmpDotPartData* pRef = NULL;
    
    // 参照指定が有効であれば検索
    if( (pPart->checkFlag( eBDPD_FLAG_REF_DRAW ) || pPart->checkFlag(eBDPD_FLAG_REF_SLOT)) ){
        stBD_PART_SEARCH_PARAM paramRef;
        CLEAR_BD_PART_SEARCH_PARAM( &paramRef );

        // カテゴリ設定（※無効なら参照元の値を流用）
        paramRef.category = pPart->getRefCategory();
        if( ! IS_BD_CATEGORY_VALID( paramRef.category ) ){
            paramRef.category = paramResult.category;
        }

        // UID設定（※無効なら参照元の値を流用）
        paramRef.uid = pPart->getRefUid();
        if( ! IS_BD_UID_VALID( paramRef.uid ) ){
            paramRef.uid = paramResult.uid;
        }
        
        // 感情設定（※無効なら参照元の値を流用）
        paramRef.emo = pPart->getRefEmo();
        if( ! IS_BD_EMO_VALID( paramRef.emo ) ){
            paramRef.emo = paramResult.emo;
        }

        // フォーム設定（※無効なら参照元の値を流用）
        paramRef.form = pPart->getRefForm();
        if( ! IS_BD_FORM_VALID( paramRef.form ) ){
            paramRef.form = paramResult.form;
        }

        // スロット設定（※無効なら参照元の値を流用）
        paramRef.slot = pPart->getRefSlot();
        if( ! IS_BD_SLOT_VALID( paramRef.slot ) ){
            paramRef.slot = pPart->getSlot();
        }

        // サブID設定（※無効なら参照元の値を流用）
        paramRef.subId = pPart->getRefSubId();
        if( ! IS_BD_SUB_ID_VALID( paramRef.subId ) ){
            paramRef.subId = pPart->getSubId();
        }

        // 方向設定（※無効なら参照元の値を流用）
        paramRef.dir = pPart->getRefDir();
        if( paramRef.dir < (eBD_DIR)0 || paramRef.dir >= eBD_DIR_MAX ){
            paramRef.dir = pPart->getDir();
        }

        // 参照先の検索はデフォルト無効（※指定をするからには対象が存在するはず）
        pRef = CBmpDotMgr::SearchBmpDotPartData( &paramRef );
        
        // 参照先がない
        if( pRef == NULL ){
            LOGE( "@ CBmpGenParam::addCell: REF DATA NOT FOUND: ctg=%s, uid=%08d, emo=%s, f=%s, slt=%s, sid=%d, dir=%s\n",
                  g_pArrLabelBdCategory[paramRef.category],
                  paramRef.uid,
                  g_pArrLabelBdEmo[paramRef.emo],
                  g_pArrLabelBdForm[paramRef.form],
                  g_pArrLabelBdSlot[paramRef.slot],
                  paramRef.subId,
                  g_pArrLabelBdDir[paramRef.dir] );
        }
        // 自身が参照された
        else if( pRef == pPart ){
            LOGE( "@ CBmpGenParam::addCell: REF OWNSELF: ctg=%s, uid=%08d, emo=%s, f=%s, slt=%s, sid=%d, dir=%s\n",
                  g_pArrLabelBdCategory[paramRef.category],
                  paramRef.uid,
                  g_pArrLabelBdEmo[paramRef.emo],
                  g_pArrLabelBdForm[paramRef.form],
                  g_pArrLabelBdSlot[paramRef.slot],
                  paramRef.subId,
                  g_pArrLabelBdDir[paramRef.dir] );

            // 無効化しておく（※二重処理されても困る）
            pRef = NULL;
        }
#if 0
        // LOG
        else{
            LOGD( "@    *REF HIT: ctg=%s/%d, uid=%08d/%08d, emo=%s/%d, f=%s/%d, slt=%s/%d, sid=%d/%d, dir=%s/%d\n",
                  g_pArrLabelBdCategory[paramRef.category], pPart->getRefCategory(),
                  paramRef.uid, pPart->getRefUid(),
                  g_pArrLabelBdEmo[paramRef.emo], pPart->getRefEmo(),
                  g_pArrLabelBdForm[paramRef.form], pPart->getRefForm(),
                  g_pArrLabelBdSlot[paramRef.slot], pPart->getRefSlot(),
                  paramRef.subId, pPart->getRefSubId(),
                  g_pArrLabelBdDir[paramRef.dir], pPart->getRefDir() );
        }
#endif
    }

    // 参照先が有効であれば割り当て
    if( pRef != NULL ){
        pCell->pDataRef = pRef;

        pCell->isRefDrawValid = pPart->checkFlag( eBDPD_FLAG_REF_DRAW );
        pCell->isRefSlotValid = pPart->checkFlag( eBDPD_FLAG_REF_SLOT );
        
        pCell->refScale = pPart->getRefScale();
        pCell->refRot = pPart->getRefRot();
    }

	return( pCell );
}

//-----------------------
// セルの検索
//-----------------------
stBMP_GEN_PARAM_CELL* CBmpGenParam::searchCell( eBD_SLOT slot, int slotIndex ){
	for( int i=0; i<m_nCellNumUse; i++ ){
        if( CHECK_BMP_GEN_PARAM_CELL( &m_stArrCell[i], slot, slotIndex ) ){
            return( &m_stArrCell[i] );
		}
	}

	return( NULL );
}
