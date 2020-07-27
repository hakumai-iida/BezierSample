/*+----------------------------------------------------------------+
  |	Title:		BmpDotOrderData.cpp [共通環境]
  |	Comment:	BMPのパーツ並びデータ（※並びはフォーム毎に指定する）
  |             実装の変遷により編集機能をもつが実際にはデフォルト指定のみを利用
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "BmpDotOrderData.hpp"

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
//-------------------------
// コンストラクタ
//-------------------------
CBmpDotOrderData::CBmpDotOrderData( void ){
	clear();
}

//-------------------------
// デストラクタ
//-------------------------
CBmpDotOrderData::~CBmpDotOrderData( void ){
    clear();    // リストの解放
}

//-------------------------
// クリア
//-------------------------
void CBmpDotOrderData::clear( void ){
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        CLEAR_BMP_DOT_ORDER_CELL( &m_stArrCell[i] );
    }
    
#ifdef BMP_DOT_ORDER_EDIT_ENABLE
    m_nSelect = 0;
    CLEAR_BMP_DOT_ORDER_CELL( &m_stCellCoppied );
#endif
}

//-------------------------
// コピー
//-------------------------
void CBmpDotOrderData::copy( CBmpDotOrderData* pData ){
    // 無効は無視
    if( pData == NULL ){
        return;
    }

    // 丸毎コピー
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        m_stArrCell[i] = pData->m_stArrCell[i];
    }

#ifdef BMP_DOT_ORDER_EDIT_ENABLE
    m_nSelect = pData->m_nSelect;
    m_stCellCoppied = pData->m_stCellCoppied;
#endif
}

//---------------------------
// 読み込み
//---------------------------
void CBmpDotOrderData::read( CInputBuffer* pIB ){
    // 用心にクリア
    clear();
    
    // 要素数の読み込み（※データ数が変更された時に扱いが楽なように）
    int num0 = pIB->readInt16();
    
    // 要素の読み込み
    int num = ((num0>BMP_DOT_ORDER_DATA_CELL_MAX)? BMP_DOT_ORDER_DATA_CELL_MAX: num0);
    for( int i=0; i<num; i++ ){
        m_stArrCell[i].slot = (eBD_SLOT)CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
        m_stArrCell[i].slotIndex = pIB->readInt16();
    }
    
    // 超過分があれば無視（※データ構造に変更がない前提）
    for( int i=num; i<num0; i++ ){
        CEnum::ReadEnum( pIB, g_pArrLabelBdSlot );
        pIB->readInt16();
    }
}

//---------------------------
// 書き込み
//---------------------------
void CBmpDotOrderData::write( COutputBuffer* pOB ){
    // 要素数の出力（※データ数が変更された時に扱いが楽なように）
    pOB->writeInt16( BMP_DOT_ORDER_DATA_CELL_MAX );
    
    // 要素の出力
    for( int i=0; i<BMP_DOT_ORDER_DATA_CELL_MAX; i++ ){
        CEnum::WriteEnum( pOB, m_stArrCell[i].slot, eBD_SLOT_MAX, g_pArrLabelBdSlot );
        pOB->writeInt16( m_stArrCell[i].slotIndex );
    }
}

//---------------------------
// 取得（※位置指定）
//---------------------------
stBMP_DOT_ORDER_DATA_CELL* CBmpDotOrderData::getCellAt( int at ){
    if( at < 0 || at >= BMP_DOT_ORDER_DATA_CELL_MAX ){
        return( NULL );
    }
    
    return( &m_stArrCell[at] );
}

#ifdef BMP_DOT_ORDER_EDIT_ENABLE
//---------------------------
// 取得（※選択対象）
//---------------------------
stBMP_DOT_ORDER_DATA_CELL* CBmpDotOrderData::getSelectedCell( void ){
    return( getCellAt( m_nSelect ) );
}

//---------------------------
// 選択位置設定
//---------------------------
int CBmpDotOrderData::setSelect( int select ){
    if( select < 0 ){
        select = 0;
    }else if( select >= BMP_DOT_ORDER_DATA_CELL_MAX ){
        select = BMP_DOT_ORDER_DATA_CELL_MAX-1;
    }
    
    m_nSelect = select;
    return( m_nSelect );
}

//---------------------------
// 選択位置設定
//---------------------------
int CBmpDotOrderData::getSelect( void ){
    return( m_nSelect );
}

//---------------------------
// 選択位置増減
//---------------------------
int CBmpDotOrderData::addSelect( int add ){
    return( setSelect( m_nSelect + add ) );
}

//---------------------------------------------------------------
// 操作（※対象は[setSelect]で指定しておく＆返値は操作後の選択インデックス）
//---------------------------------------------------------------
// セルのクリア
int CBmpDotOrderData::clearCell( void ){
    CLEAR_BMP_DOT_ORDER_CELL( &m_stArrCell[m_nSelect] );
    return( m_nSelect );
}

// セルのスロットの増減
int CBmpDotOrderData::addCellSlot( int add ){
    // ここはあえて値のチェックはしない（※自由に編集させる）
    m_stArrCell[m_nSelect].slot = (eBD_SLOT)(m_stArrCell[m_nSelect].slot + add);
    return( m_nSelect );
}

// セルのスロットインデックスの増減
int CBmpDotOrderData::addCellSlotIndex( int add ){
    // ここはあえて値のチェックはしない（※自由に編集させる）
    m_stArrCell[m_nSelect].slotIndex += add;
    return( m_nSelect );
}

// セルのコピー
int CBmpDotOrderData::copyCell( void ){
    m_stCellCoppied = m_stArrCell[m_nSelect];
    return( m_nSelect );
}

// コピーしたセルの上書き
int CBmpDotOrderData::pasteCell( void ){
    m_stArrCell[m_nSelect] = m_stCellCoppied;
    return( m_nSelect );
}

// セル移動：上
int CBmpDotOrderData::moveCellUp( void ){
    if( m_nSelect > 0 ){
        stBMP_DOT_ORDER_DATA_CELL tmp = m_stArrCell[m_nSelect];
        m_stArrCell[m_nSelect] = m_stArrCell[m_nSelect-1];
        m_stArrCell[m_nSelect-1] = tmp;
        
        m_nSelect--;
    }
    
    return( m_nSelect );
}

// セル移動：下
int CBmpDotOrderData::moveCellDown( void ){
    if( m_nSelect < (BMP_DOT_ORDER_DATA_CELL_MAX-1) ){
        stBMP_DOT_ORDER_DATA_CELL tmp = m_stArrCell[m_nSelect];
        m_stArrCell[m_nSelect] = m_stArrCell[m_nSelect+1];
        m_stArrCell[m_nSelect+1] = tmp;
        
        m_nSelect++;
    }

    return( m_nSelect );
}

// セルブロックのスライド移動：上
int CBmpDotOrderData::slideCellBlockUp( void ){
    // 選択位置までのセルを１つ上へスライド
    for( int i=1; i<=m_nSelect; i++ ){
        m_stArrCell[i-1] = m_stArrCell[i];
    }
    
    // 選択位置はクリア
    CLEAR_BMP_DOT_ORDER_CELL( &m_stArrCell[m_nSelect] );

    return( m_nSelect );
}

// セルブロックのスライド移動：下
int CBmpDotOrderData::slideCellBlockDown( void ){
    // 選択位置までのセルを１つ下へスライド
    for( int i=BMP_DOT_ORDER_DATA_CELL_MAX-2; i>=m_nSelect; i-- ){
        m_stArrCell[i+1] = m_stArrCell[i];
    }
    
    // 選択位置はクリア
    CLEAR_BMP_DOT_ORDER_CELL( &m_stArrCell[m_nSelect] );
    
    return( m_nSelect );
}
#endif

//-----------------------------------
// 設定
//-----------------------------------
void CBmpDotOrderData::setCellAt( int at, eBD_SLOT slot, int slotIndex ){
    if( at >= 0 && at < BMP_DOT_ORDER_DATA_CELL_MAX ){
        m_stArrCell[at].slot = slot;
        m_stArrCell[at].slotIndex = slotIndex;
    }
}

//-----------------------------------
// デフォルト設定
//-----------------------------------
void CBmpDotOrderData::setDefault( eBD_FORM form, bool isSuited ){
    switch( form ){
    case eBD_FORM_FRONT:
        if( isSuited ){
            setDefaultForFrontSuited();
        }else{
            setDefaultForFront();
        }
        break;
        
    case eBD_FORM_QUATER:
        if( isSuited ){
            setDefaultForQuaterSuited();
        }else{
            setDefaultForQuater();
        }
        break;
        
    default:
        break;
    }
}

//------------------------------------------
// デフォルト設定：正面（複数スロットは右側を手前に）
// TODO:エクセルで管理するか？
//------------------------------------------
void CBmpDotOrderData::setDefaultForFront( void ){
    // 一旦クリア
    clear();
    
    int at = 0;
    
    //---------------------------------
    // 管理系（※表示要素を想定しないもの）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FgTop,            0 );        // S:姿形
    
    //---------------------------------
    // 頭（顔の前にくる部分）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdFront,          0 );        // S:頭：前髪
    
    setCellAt( at++, eBD_SLOT_HdAccentTop,      0 );        // S:頭：アクセント（上）
    setCellAt( at++, eBD_SLOT_HdAccentSide,     1 );        // M:頭：アクセント（横／右）
    setCellAt( at++, eBD_SLOT_HdAccentSide,     0 );        // M:頭：アクセント（横／左）
    
    setCellAt( at++, eBD_SLOT_HdCap,            0 );        // S:頭：帽子（※頭頂を隠す系）
    setCellAt( at++, eBD_SLOT_HdRibbon,         0 );        // S:頭：リボン（※帽子がある場合は表示をオフ）
    
    //---------------------------------
    // 顔
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FcOption,         0 );        // S:オプション（メガネ等）
    setCellAt( at++, eBD_SLOT_FcNose,           0 );        // S:鼻

    setCellAt( at++, eBD_SLOT_FcBrows,          1 );        // M:頭：まゆげ（右）
    setCellAt( at++, eBD_SLOT_EyBase,           1 );        // M:目土台（右）
    setCellAt( at++, eBD_SLOT_EyBall,           1 );        // M:目玉（※目の土台に対するテスト）（右）

    setCellAt( at++, eBD_SLOT_FcBrows,          0 );        // M:頭：まゆげ（左）
    setCellAt( at++, eBD_SLOT_EyBase,           0 );        // M:目土台（左）
    setCellAt( at++, eBD_SLOT_EyBall,           0 );        // M:目玉（※目の土台に対するテスト）（左）

    setCellAt( at++, eBD_SLOT_MoBase,           0 );        // S:口

    setCellAt( at++, eBD_SLOT_FcHairLine,       0 );        // S:生え際
    setCellAt( at++, eBD_SLOT_FcBase,           0 );        // S:顔土台（※輪郭＆塗りつぶし）

    setCellAt( at++, eBD_SLOT_EyOption,         1 );        // M:目オプション（※顔の土台に対するテスト）（右）
    setCellAt( at++, eBD_SLOT_FcCheek,          1 );        // S:ほっぺた（※顔の土台に対するテスト）（右）

    setCellAt( at++, eBD_SLOT_EyOption,         0 );        // M:目オプション（※顔の土台に対するテスト）（左）
    setCellAt( at++, eBD_SLOT_FcCheek,          0 );        // S:ほっぺた（※顔の土台に対するテスト）（左）

    setCellAt( at++, eBD_SLOT_HdSide,           1 );        // M:顔：もみあげ（右）
    setCellAt( at++, eBD_SLOT_FcEar,            1 );        // M:顔：耳（右）

    setCellAt( at++, eBD_SLOT_HdSide,           0 );        // M:顔：もみあげ（左）
    setCellAt( at++, eBD_SLOT_FcEar,            0 );        // M:顔：耳（左）
    
    //---------------------------------
    // 体
    //---------------------------------
    setCellAt( at++, eBD_SLOT_UpSymbol,         0 );        // S:襟元のシンボル
    setCellAt( at++, eBD_SLOT_UpRibbon,         0 );        // S:襟元のリボン
    
    setCellAt( at++, eBD_SLOT_BlJointUpBody,    0 );        // S:上半身の結合
    setCellAt( at++, eBD_SLOT_BlJointLowBody,   0 );        // S:下半身の結合
    setCellAt( at++, eBD_SLOT_BlBase,           0 );        // S:腹土台
    
    setCellAt( at++, eBD_SLOT_UpJointNeck,      0 );        // S:首の結合
    setCellAt( at++, eBD_SLOT_UpBase,           0 );        // S:上半身土台
    setCellAt( at++, eBD_SLOT_NkBase,           0 );        // S:首土台

    setCellAt( at++, eBD_SLOT_LwBase,           0 );        // S:下半身土台
    setCellAt( at++, eBD_SLOT_LwSymbol,         0 );        // S:エプロンのシンボル（※下半身の土台に対するテスト）

    //---------------------------------
    // 足
    //---------------------------------
    setCellAt( at++, eBD_SLOT_KnJointAnkle,     1 );        // M:足首の結合（右）
    setCellAt( at++, eBD_SLOT_FoBase,           1 );        // M:足先の土台（右）
    setCellAt( at++, eBD_SLOT_LgJointKnee,      1 );        // M:膝の結合（右）
    setCellAt( at++, eBD_SLOT_KnBase,           1 );        // M:膝下の土台（右）
    setCellAt( at++, eBD_SLOT_LwJointLeg,       1 );        // M:脚の結合（右）
    setCellAt( at++, eBD_SLOT_CvLeg,            1 );        // M:カバー足（※脚の直前）（右）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_LgBase,           1 );        // M:脚の土台（右）

    setCellAt( at++, eBD_SLOT_KnJointAnkle,     0 );        // M:足首の結合（左）
    setCellAt( at++, eBD_SLOT_FoBase,           0 );        // M:足先の土台（左）
    setCellAt( at++, eBD_SLOT_LgJointKnee,      0 );        // M:膝の結合（左）
    setCellAt( at++, eBD_SLOT_KnBase,           0 );        // M:膝下の土台（左）
    setCellAt( at++, eBD_SLOT_LwJointLeg,       0 );        // M:脚の結合（左）
    setCellAt( at++, eBD_SLOT_CvLeg,            0 );        // M:カバー足（※脚の直前）（左）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_LgBase,           0 );        // M:脚の土台（左）

    //---------------------------------
    // 手〜肩
    //---------------------------------
    setCellAt( at++, eBD_SLOT_ElJointWrist,     1 );        // M:手首の結合（右）
    setCellAt( at++, eBD_SLOT_HnBase,           1 );        // M:手の土台（右）
    setCellAt( at++, eBD_SLOT_AmJointElbow,     1 );        // M:肘の結合（右）
    setCellAt( at++, eBD_SLOT_ElBase,           1 );        // M:肘下の土台（右）
    setCellAt( at++, eBD_SLOT_UpJointSholder,   1 );        // M:肩の結合（右）
    setCellAt( at++, eBD_SLOT_CvArm,            1 );        // M:カバー腕（※腕の直前）（右）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_AmBase,           1 );        // M:腕の土台（右）

    setCellAt( at++, eBD_SLOT_ElJointWrist,     0 );        // M:手首の結合（左）
    setCellAt( at++, eBD_SLOT_HnBase,           0 );        // M:手の土台（左）
    setCellAt( at++, eBD_SLOT_AmJointElbow,     0 );        // M:肘の結合（左）
    setCellAt( at++, eBD_SLOT_ElBase,           0 );        // M:肘下の土台（左）
    setCellAt( at++, eBD_SLOT_UpJointSholder,   0 );        // M:肩の結合（左）
    setCellAt( at++, eBD_SLOT_CvArm,            0 );        // M:カバー腕（※腕の直前）（左）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_AmBase,           0 );        // M:腕の土台（左）

    //---------------------------------
    // 背中
    //---------------------------------
    setCellAt( at++, eBD_SLOT_BlRibbonTie,      1 );        // S:リボン結び目（右）
    setCellAt( at++, eBD_SLOT_BlRibbonString,   1 );        // S:リボン紐（なびかせる部分）（右）

    setCellAt( at++, eBD_SLOT_BlRibbonTie,      0 );        // S:リボン結び目（左）
    setCellAt( at++, eBD_SLOT_BlRibbonString,   0 );        // S:リボン紐（なびかせる部分（左）

    setCellAt( at++, eBD_SLOT_LwTail,           0 );        // S:尻尾

    //---------------------------------
    // 頭（顔の後ろ側）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdTop,            0 );        // S:頭上
    setCellAt( at++, eBD_SLOT_CvHead,           0 );        // S:カバー頭（※頭の直前）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_HdBase,           0 );        // S:頭土台

    setCellAt( at++, eBD_SLOT_HdHairSideKnot,   1 );        // S:結び目（横）（右）
    setCellAt( at++, eBD_SLOT_HdHairSide,       1 );        // S:おさげ（横）（右）

    setCellAt( at++, eBD_SLOT_HdHairSideKnot,   0 );        // S:結び目（横）（左）
    setCellAt( at++, eBD_SLOT_HdHairSide,       0 );        // S:おさげ（横）（左）
    
    setCellAt( at++, eBD_SLOT_HdHairBackKnot,   0 );        // S:結び目（後）
    setCellAt( at++, eBD_SLOT_HdHairBack,       0 );        // S:おさげ（後）

    //---------------------------------
    // 地面（※再奥＝足下に何か置けるように）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FoGround,         1 );        // M:接地点（右）
    setCellAt( at++, eBD_SLOT_FoGround,         0 );        // M:接地点（左）
}

//---------------------------
// デフォルト設定：正面（着ぐるみ）
//---------------------------
void CBmpDotOrderData::setDefaultForFrontSuited( void ){
    // 一旦クリア
    clear();
    
    int at = 0;
    
    //---------------------------------
    // 管理系（※表示要素を想定しないもの）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FgTop,            0 );        // S:姿形

    //---------------------------------
    // 頭（顔の前にくる部分）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdFront,          0 );        // S:頭：前髪
    
    setCellAt( at++, eBD_SLOT_HdAccentTop,      0 );        // S:頭：アクセント（上）
    setCellAt( at++, eBD_SLOT_HdAccentSide,     1 );        // M:頭：アクセント（横／右）
    setCellAt( at++, eBD_SLOT_HdAccentSide,     0 );        // M:頭：アクセント（横／左）
    
    setCellAt( at++, eBD_SLOT_HdCap,            0 );        // S:頭：帽子（※頭頂を隠す系）
    setCellAt( at++, eBD_SLOT_HdRibbon,         0 );        // S:頭：リボン（※帽子がある場合は表示をオフ）

    //---------------------------------
    // 顔
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FcOption,         0 );        // S:オプション（メガネ等）
    setCellAt( at++, eBD_SLOT_FcNose,           0 );        // S:鼻

    setCellAt( at++, eBD_SLOT_FcBrows,          1 );        // M:頭：まゆげ（右）
    setCellAt( at++, eBD_SLOT_EyBase,           1 );        // M:目土台（右）
    setCellAt( at++, eBD_SLOT_EyBall,           1 );        // M:目玉（※目の土台に対するテスト）（右）

    setCellAt( at++, eBD_SLOT_FcBrows,          0 );        // M:頭：まゆげ（左）
    setCellAt( at++, eBD_SLOT_EyBase,           0 );        // M:目土台（左）
    setCellAt( at++, eBD_SLOT_EyBall,           0 );        // M:目玉（※目の土台に対するテスト）（左）

    setCellAt( at++, eBD_SLOT_MoBase,           0 );        // S:口

    setCellAt( at++, eBD_SLOT_FcHairLine,       0 );        // S:生え際
    setCellAt( at++, eBD_SLOT_FcBase,           0 );        // S:顔土台（※輪郭＆塗りつぶし）

    setCellAt( at++, eBD_SLOT_EyOption,         1 );        // M:目オプション（※顔の土台に対するテスト）（右）
    setCellAt( at++, eBD_SLOT_FcCheek,          1 );        // S:ほっぺた（※顔の土台に対するテスト）（右）

    setCellAt( at++, eBD_SLOT_EyOption,         0 );        // M:目オプション（※顔の土台に対するテスト）（左）
    setCellAt( at++, eBD_SLOT_FcCheek,          0 );        // S:ほっぺた（※顔の土台に対するテスト）（左）

    setCellAt( at++, eBD_SLOT_HdSide,           1 );        // M:顔：もみあげ（右）
    setCellAt( at++, eBD_SLOT_FcEar,            1 );        // M:顔：耳（右）

    setCellAt( at++, eBD_SLOT_HdSide,           0 );        // M:顔：もみあげ（左）
    setCellAt( at++, eBD_SLOT_FcEar,            0 );        // M:顔：耳（左）

    //---------------------------------
    // 手〜肩
    //---------------------------------
    setCellAt( at++, eBD_SLOT_ElJointWrist,     1 );        // M:手首の結合（右）
    setCellAt( at++, eBD_SLOT_HnBase,           1 );        // M:手の土台（右）
    setCellAt( at++, eBD_SLOT_AmJointElbow,     1 );        // M:肘の結合（右）
    setCellAt( at++, eBD_SLOT_ElBase,           1 );        // M:肘下の土台（右）
    setCellAt( at++, eBD_SLOT_UpJointSholder,   1 );        // M:肩の結合（右）
    setCellAt( at++, eBD_SLOT_CvArm,            1 );        // M:カバー腕（※腕の直前）（右）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_AmBase,           1 );        // M:腕の土台（右）

    setCellAt( at++, eBD_SLOT_ElJointWrist,     0 );        // M:手首の結合（左）
    setCellAt( at++, eBD_SLOT_HnBase,           0 );        // M:手の土台（左）
    setCellAt( at++, eBD_SLOT_AmJointElbow,     0 );        // M:肘の結合（左）
    setCellAt( at++, eBD_SLOT_ElBase,           0 );        // M:肘下の土台（左）
    setCellAt( at++, eBD_SLOT_UpJointSholder,   0 );        // M:肩の結合（左）
    setCellAt( at++, eBD_SLOT_CvArm,            0 );        // M:カバー腕（※腕の直前）（左）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_AmBase,           0 );        // M:腕の土台（左）

    //---------------------------------
    // 足
    //---------------------------------
    setCellAt( at++, eBD_SLOT_KnJointAnkle,     1 );        // M:足首の結合（右）
    setCellAt( at++, eBD_SLOT_FoBase,           1 );        // M:足先の土台（右）
    setCellAt( at++, eBD_SLOT_LgJointKnee,      1 );        // M:膝の結合（右）
    setCellAt( at++, eBD_SLOT_KnBase,           1 );        // M:膝下の土台（右）
    setCellAt( at++, eBD_SLOT_LwJointLeg,       1 );        // M:脚の結合（右）
    setCellAt( at++, eBD_SLOT_CvLeg,            1 );        // M:カバー足（※脚の直前）（右）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_LgBase,           1 );        // M:脚の土台（右）

    setCellAt( at++, eBD_SLOT_KnJointAnkle,     0 );        // M:足首の結合（左）
    setCellAt( at++, eBD_SLOT_FoBase,           0 );        // M:足先の土台（左）
    setCellAt( at++, eBD_SLOT_LgJointKnee,      0 );        // M:膝の結合（左）
    setCellAt( at++, eBD_SLOT_KnBase,           0 );        // M:膝下の土台（左）
    setCellAt( at++, eBD_SLOT_LwJointLeg,       0 );        // M:脚の結合（左）
    setCellAt( at++, eBD_SLOT_CvLeg,            0 );        // M:カバー足（※脚の直前）（左）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_LgBase,           0 );        // M:脚の土台（左）

    //---------------------------------
    // 頭（※ハリボテの前に来る部分）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdTop,            0 );        // S:頭上
    setCellAt( at++, eBD_SLOT_CvHead,           0 );        // S:カバー頭（※頭の直前）（※この要素が直接呼ばれることはない想定）
    setCellAt( at++, eBD_SLOT_HdBase,           0 );        // S:頭土台

    //---------------------------------
    // 体
    //---------------------------------
    setCellAt( at++, eBD_SLOT_UpSymbol,         0 );        // S:襟元のシンボル
    setCellAt( at++, eBD_SLOT_UpRibbon,         0 );        // S:襟元のリボン
    
    setCellAt( at++, eBD_SLOT_BlJointUpBody,    0 );        // S:上半身の結合
    setCellAt( at++, eBD_SLOT_BlJointLowBody,   0 );        // S:下半身の結合
    setCellAt( at++, eBD_SLOT_BlBase,           0 );        // S:腹土台
    
    setCellAt( at++, eBD_SLOT_UpJointNeck,      0 );        // S:首の結合
    setCellAt( at++, eBD_SLOT_UpBase,           0 );        // S:上半身土台
    setCellAt( at++, eBD_SLOT_NkBase,           0 );        // S:首土台

    setCellAt( at++, eBD_SLOT_LwBase,           0 );        // S:下半身土台
    setCellAt( at++, eBD_SLOT_LwSymbol,         0 );        // S:エプロンのシンボル（※下半身の土台に対するテスト）

    //---------------------------------
    // 背中
    //---------------------------------
    setCellAt( at++, eBD_SLOT_BlRibbonTie,      1 );        // S:リボン結び目（右）
    setCellAt( at++, eBD_SLOT_BlRibbonString,   1 );        // S:リボン紐（なびかせる部分）（右）

    setCellAt( at++, eBD_SLOT_BlRibbonTie,      0 );        // S:リボン結び目（左）
    setCellAt( at++, eBD_SLOT_BlRibbonString,   0 );        // S:リボン紐（なびかせる部分（左）

    setCellAt( at++, eBD_SLOT_LwTail,           0 );        // S:尻尾

    //---------------------------------
    // 頭（顔の後ろ側）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdHairSideKnot,   1 );        // S:結び目（横）（右）
    setCellAt( at++, eBD_SLOT_HdHairSide,       1 );        // S:おさげ（横）（右）

    setCellAt( at++, eBD_SLOT_HdHairSideKnot,   0 );        // S:結び目（横）（左）
    setCellAt( at++, eBD_SLOT_HdHairSide,       0 );        // S:おさげ（横）（左）
    
    setCellAt( at++, eBD_SLOT_HdHairBackKnot,   0 );        // S:結び目（後）
    setCellAt( at++, eBD_SLOT_HdHairBack,       0 );        // S:おさげ（後）

    //---------------------------------
    // 地面（※再奥＝足下に何か置けるように）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FoGround,         1 );        // M:接地点（右）
    setCellAt( at++, eBD_SLOT_FoGround,         0 );        // M:接地点（左）
}

//-----------------------------------
// デフォルト設定：斜め
// TODO:エクセルで管理するか？
//-----------------------------------
void CBmpDotOrderData::setDefaultForQuater( void ){
    // 一旦クリア
    clear();
    
    int at = 0;
    
    //---------------------------------
    // 管理系（※表示要素のないもの）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_FgTop,           0 );         // S:姿形
    setCellAt( at++, eBD_SLOT_FoGround,        1 );         // S:接地点（右）
    setCellAt( at++, eBD_SLOT_FoGround,        0 );         // S:接地点（左）

    //---------------------------------
    // 髪（右）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdHairSideKnot,  1 );         // S:結び目（横／右）
    setCellAt( at++, eBD_SLOT_HdHairSide,      1 );         // S:おさげ（横／右）

    //---------------------------------
    // 手〜肩（右）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_ElJointWrist,    1 );         // M:手首の結合
    setCellAt( at++, eBD_SLOT_HnBase,          1 );         // M:手の土台
    setCellAt( at++, eBD_SLOT_AmJointElbow,    1 );         // M:肘の結合
    setCellAt( at++, eBD_SLOT_ElBase,          1 );         // M:肘下の土台
    setCellAt( at++, eBD_SLOT_UpJointSholder,  1 );         // M:肩の結合
    setCellAt( at++, eBD_SLOT_AmBase,          1 );         // M:腕の土台

    //---------------------------------
    // 頭（顔の前にくる部分）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdAccentSide,    1 );         // M:頭：アクセント（横／右）
    
    setCellAt( at++, eBD_SLOT_HdAccentTop,     0 );         // S:頭：アクセント（上）
    setCellAt( at++, eBD_SLOT_HdFront,         0 );         // S:頭：前髪
    setCellAt( at++, eBD_SLOT_HdCap,           0 );         // S:頭：帽子
    
    setCellAt( at++, eBD_SLOT_HdRibbon,        0 );         // S:頭：リボン

    //---------------------------------
    // 顔
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdSide,          1 );         // M:顔：もみあげ（右）
    setCellAt( at++, eBD_SLOT_FcEar,           1 );         // M:顔：耳（右）

    setCellAt( at++, eBD_SLOT_FcOption,        0 );         // S:オプション
    setCellAt( at++, eBD_SLOT_FcNose,          0 );         // S:鼻

    setCellAt( at++, eBD_SLOT_FcBrows,         1 );         // S:頭：まゆげ（右）
    setCellAt( at++, eBD_SLOT_EyBase,          1 );         // M:目土台（右）
    setCellAt( at++, eBD_SLOT_EyBall,          1 );         // M:目玉（右）

    setCellAt( at++, eBD_SLOT_MoBase,          0 );         // S:口土台

    setCellAt( at++, eBD_SLOT_FcBrows,         0 );         // S:頭：まゆげ（左）
    setCellAt( at++, eBD_SLOT_EyBase,          0 );         // M:目土台（左）
    setCellAt( at++, eBD_SLOT_EyBall,          0 );         // M:目玉（左）

    setCellAt( at++, eBD_SLOT_FcHairLine,      0 );         // S:生え際
    setCellAt( at++, eBD_SLOT_FcBase,          0 );         // S:顔土台

    setCellAt( at++, eBD_SLOT_EyOption,        1 );         // M:目オプション（右）
    setCellAt( at++, eBD_SLOT_EyOption,        0 );         // M:目オプション（左）
    setCellAt( at++, eBD_SLOT_FcCheek,         1 );         // S:ほっぺた（右）
    setCellAt( at++, eBD_SLOT_FcCheek,         0 );         // S:ほっぺた（左）

    setCellAt( at++, eBD_SLOT_HdSide,          0 );         // M:顔：もみあげ（左）
    setCellAt( at++, eBD_SLOT_FcEar,           0 );         // N:顔：耳（左）

    setCellAt( at++, eBD_SLOT_HdAccentSide,    0 );         // M:頭：アクセント（横／左）

    //---------------------------------
    // 体
    //---------------------------------
    setCellAt( at++, eBD_SLOT_UpSymbol,        0 );         // S:襟元のシンボル
    setCellAt( at++, eBD_SLOT_UpRibbon,        0 );         // S:襟元のリボン
    setCellAt( at++, eBD_SLOT_BlBase,          0 );         // S:腹土台
    setCellAt( at++, eBD_SLOT_BlJointUpBody,   0 );         // S:上半身の結合
    setCellAt( at++, eBD_SLOT_UpBase,          0 );         // S:上半身土台
    setCellAt( at++, eBD_SLOT_UpJointNeck,     0 );         // S:首の結合
    setCellAt( at++, eBD_SLOT_NkBase,          0 );         // S:首土台
    setCellAt( at++, eBD_SLOT_BlJointLowBody,  0 );         // S:下半身の結合
    setCellAt( at++, eBD_SLOT_LwBase,          0 );         // S:下半身土台
    setCellAt( at++, eBD_SLOT_LwSymbol,        0 );         // S:エプロンのシンボル

    //---------------------------------
    // リボン（右）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_BlRibbonString,  1 );         // M:リボン（横／右）
    setCellAt( at++, eBD_SLOT_BlRibbonTie,     1 );         // M:リボン（横／右）

    //---------------------------------
    // 足（右）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_KnJointAnkle,    1 );         // M:足首の結合
    setCellAt( at++, eBD_SLOT_FoBase,          1 );         // M:足先の土台
    setCellAt( at++, eBD_SLOT_LgJointKnee,     1 );         // M:膝の結合
    setCellAt( at++, eBD_SLOT_KnBase,          1 );         // M:膝下の土台
    setCellAt( at++, eBD_SLOT_LwJointLeg,      1 );         // M:脚の結合
    setCellAt( at++, eBD_SLOT_LgBase,          1 );         // M:脚の土台

    //---------------------------------
    // 頭（顔の後ろ側）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdTop,           0 );         // S:頭上
    setCellAt( at++, eBD_SLOT_HdBase,          0 );         // S:頭土台

    setCellAt( at++, eBD_SLOT_HdHairBackKnot,  0 );         // S:結び目（後ろ）
    setCellAt( at++, eBD_SLOT_HdHairBack,      0 );         // S:おさげ（後ろ）

    //---------------------------------
    // 足（左）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_KnJointAnkle,    0 );         // M:足首の結合
    setCellAt( at++, eBD_SLOT_FoBase,          0 );         // M:足先の土台
    setCellAt( at++, eBD_SLOT_LgJointKnee,     0 );         // M:膝の結合
    setCellAt( at++, eBD_SLOT_KnBase,          0 );         // M:膝下の土台
    setCellAt( at++, eBD_SLOT_LwJointLeg,      0 );         // M:脚の結合
    setCellAt( at++, eBD_SLOT_LgBase,          0 );         // M:脚の土台

    //---------------------------------
    // 手〜肩（左）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_UpJointSholder,  0 );         // M:肩の結合
    setCellAt( at++, eBD_SLOT_AmBase,          0 );         // M:腕の土台
    setCellAt( at++, eBD_SLOT_AmJointElbow,    0 );         // M:肘の結合
    setCellAt( at++, eBD_SLOT_ElBase,          0 );         // M:肘下の土台
    setCellAt( at++, eBD_SLOT_ElJointWrist,    0 );         // M:手首の結合
    setCellAt( at++, eBD_SLOT_HnBase,          0 );         // M:手の土台

    //---------------------------------
    // 尻尾
    //---------------------------------
    setCellAt( at++, eBD_SLOT_LwTail,          0 );         // S:尻尾

    //---------------------------------
    // リボン（左）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_BlRibbonString,  0 );         // M:リボン（横／左）
    setCellAt( at++, eBD_SLOT_BlRibbonTie,     0 );         // M:リボン（横／左）

    //---------------------------------
    // 髪（左）
    //---------------------------------
    setCellAt( at++, eBD_SLOT_HdHairSideKnot,  0 );         // S:結び目（横／左）
    setCellAt( at++, eBD_SLOT_HdHairSide,      0 );         // S:おさげ（横／左）
}

//---------------------------
// デフォルト設定：斜め（着ぐるみ）
//---------------------------
void CBmpDotOrderData::setDefaultForQuaterSuited( void ){
    setDefaultForQuater();  // とりあえず
}
