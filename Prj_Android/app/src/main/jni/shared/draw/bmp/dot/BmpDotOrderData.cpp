/*+----------------------------------------------------------------+
  |	Title:		BmpDotOrderData.cpp [共通環境]
  |	Comment:	BMPのドットパーツ並びデータ（※並びはフォーム毎に指定する）
  |             実装の変遷により編集機能をもつが現状はデフォルト指定のみを利用
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
        
    default:
        break;
    }
}

//-----------------------------------------------------------------------------------
// デフォルト設定：正面
//-----------------------------------------------------------------------------------
// 手前に描画するパーツほど先に登録する
// 複数スロットは右側を先に登録する（※親が参照する際に[XxXxx,0]でアクセスするため[0]を後に登録する）
//-----------------------------------------------------------------------------------
// 簡便のため
#define _ADD( _s, _i ) setCellAt( at++, (_s), (_i) )

// 本体
void CBmpDotOrderData::setDefaultForFront( void ){
    // 一旦クリア
    clear();
    
    // work(※[_ADD]内部で連番の値として利用される)
    int at = 0;

    //---------------------------------
    // 最前面
    //---------------------------------
    _ADD( eBD_SLOT_LyFront,             0 );    // S:最前面
    
    //---------------------------------
    // 呼び出しスロット
    //---------------------------------
    _ADD( eBD_SLOT_FgTop,               0 );    // S:姿形
    
    //---------------------------------
    // 腕
    //---------------------------------
    _ADD( eBD_SLOT_LyArmFront,          0 );    // S:腕の手前

    // 右腕（手〜ひじまで）
    _ADD( eBD_SLOT_AmOptionA,           1 );    // M:腕オプションＡ（右）
    _ADD( eBD_SLOT_AmOptionB,           1 );    // M:腕オプションＢ（右）
    _ADD( eBD_SLOT_HnBase,              1 );    // M:手の土台（右）
    _ADD( eBD_SLOT_ElBase,              1 );    // M:肘下の土台（右）

    // 左腕（手〜ひじまで）
    _ADD( eBD_SLOT_AmOptionA,           0 );    // M:腕オプションＡ（左）
    _ADD( eBD_SLOT_AmOptionB,           0 );    // M:腕オプションＢ（左）
    _ADD( eBD_SLOT_HnBase,              0 );    // M:手の土台（左）
    _ADD( eBD_SLOT_ElBase,              0 );    // M:肘下の土台（左）
        
    //---------------------------------
    // 頭（顔の前にくる部分）
    //---------------------------------
    _ADD( eBD_SLOT_LyHairFront,         0 );    // S:髪の手前

    _ADD( eBD_SLOT_HdAccentTop,         0 );    // S:頭：アクセント（上）
    _ADD( eBD_SLOT_HdAccentSide,        1 );    // M:頭：アクセント（横／右）
    _ADD( eBD_SLOT_HdAccentSide,        0 );    // M:頭：アクセント（横／左）
    
    _ADD( eBD_SLOT_HdCap,               0 );    // S:頭：帽子（※頭頂を隠す系）

    _ADD( eBD_SLOT_HdFront,             0 );    // S:頭：前髪
    _ADD( eBD_SLOT_HdRibbon,            0 );    // S:頭：リボン（※帽子がある場合は表示をオフ）
    _ADD( eBD_SLOT_HdBand,              0 );    // S:頭：バンド（※帽子がある場合は表示をオフ）

    _ADD( eBD_SLOT_LyHeadCoverFront,    0 );    // S:頭のカバー

    //---------------------------------
    // 顔
    //---------------------------------
    _ADD( eBD_SLOT_LyFaceFront,         0 );    // S:顔の手前
    _ADD( eBD_SLOT_LyFaceCoverFront,    0 );    // S:顔のカバー

    _ADD( eBD_SLOT_FcNose,              0 );    // S:鼻（※オプションとしてメガネ等）

    _ADD( eBD_SLOT_FcBrows,             1 );    // M:頭：まゆげ（右）
    _ADD( eBD_SLOT_EyBase,              1 );    // M:目土台（右）（※差し替えで眼帯等）
    _ADD( eBD_SLOT_EyBall,              1 );    // M:目玉（※目の土台に対するテスト）（右）（※差し替えで瞳孔の形状違い等）

    _ADD( eBD_SLOT_FcBrows,             0 );    // M:頭：まゆげ（左）
    _ADD( eBD_SLOT_EyBase,              0 );    // M:目土台（左）（※差し替えで眼帯等）
    _ADD( eBD_SLOT_EyBall,              0 );    // M:目玉（※目の土台に対するテスト）（左）（※差し替えで瞳孔の形状違い等）

    _ADD( eBD_SLOT_MoBase,              0 );    // S:口

    // 輪郭
    _ADD( eBD_SLOT_FcUnderCover,        0 );    // S:顔下カバー
    _ADD( eBD_SLOT_FcBase,              0 );    // S:顔土台
    _ADD( eBD_SLOT_FcUnder,             0 );    // S:顔下

    _ADD( eBD_SLOT_FcCheek,             1 );    // S:ほっぺた（※顔の土台に対するテスト）（右）（※オプションとして傷等）
    _ADD( eBD_SLOT_FcCheek,             0 );    // S:ほっぺた（※顔の土台に対するテスト）（左）（※オプションとして傷等）

    // もみあげは輪郭の後ろ
    _ADD( eBD_SLOT_HdSide,              1 );    // M:顔：もみあげ（右）
    _ADD( eBD_SLOT_FcEar,               1 );    // M:顔：耳（右）（※オプションとしてイヤリング等）

    _ADD( eBD_SLOT_HdSide,              0 );    // M:顔：もみあげ（左）
    _ADD( eBD_SLOT_FcEar,               0 );    // M:顔：耳（左）（※オプションとしてイヤリング等）

    //---------------------------------
    // 体
    //---------------------------------
    _ADD( eBD_SLOT_LyBodyFront,         0 );    // S:体の手前

    _ADD( eBD_SLOT_BdOptionA,           1 );    // M:汎用オプションＡ（右）
    _ADD( eBD_SLOT_BdOptionA,           0 );    // M:汎用オプションＡ（左）
    _ADD( eBD_SLOT_BdOptionB,           1 );    // M:汎用オプションＢ（右）
    _ADD( eBD_SLOT_BdOptionB,           0 );    // M:汎用オプションＢ（左）

    //---------------------------------
    // 上半身
    //---------------------------------
    _ADD( eBD_SLOT_LyUpFront,           0 );    // S:上半身の手前

    _ADD( eBD_SLOT_UpOptionA,           1 );    // M:上半身オプションＡ（右）
    _ADD( eBD_SLOT_UpOptionA,           0 );    // M:上半身オプションＡ（左）
    _ADD( eBD_SLOT_UpOptionB,           1 );    // M:上半身オプションＢ（右）
    _ADD( eBD_SLOT_UpOptionB,           0 );    // M:上半身オプションＢ（左）
    _ADD( eBD_SLOT_UpRibbon,            0 );    // S:胸元のリボン
    _ADD( eBD_SLOT_UpSymbol,            0 );    // S:胸元の宝石
    _ADD( eBD_SLOT_UpBase,              0 );    // S:上半身土台
    _ADD( eBD_SLOT_NkCover,             0 );    // S:首カバー
    _ADD( eBD_SLOT_NkBase,              0 );    // S:首土台

    // 右腕（肩）
    _ADD( eBD_SLOT_AmCover,             1 );    // M:腕カバー（右）
    _ADD( eBD_SLOT_AmBase,              1 );    // M:腕の土台（右）

    // 左腕（肩）
    _ADD( eBD_SLOT_AmCover,             0 );    // M:腕カバー（左）
    _ADD( eBD_SLOT_AmBase,              0 );    // M:腕の土台（左）

    //---------------------------------
    // 腹
    //---------------------------------
    _ADD( eBD_SLOT_LyBellyFront,        0 );    // S:腹の手前

    _ADD( eBD_SLOT_BlOptionA,           1 );    // M:腹オプションＡ（右）
    _ADD( eBD_SLOT_BlOptionA,           0 );    // M:腹オプションＡ（左）
    _ADD( eBD_SLOT_BlOptionB,           1 );    // M:腹オプションＢ（右）
    _ADD( eBD_SLOT_BlOptionB,           0 );    // M:腹オプションＢ（左）
    _ADD( eBD_SLOT_BlBase,              0 );    // S:腹土台

    //---------------------------------
    // 下半身
    //---------------------------------
    _ADD( eBD_SLOT_LyLowFront,          0 );     // S:下半身の手前

    _ADD( eBD_SLOT_LwOptionA,           1 );     // M:下半身オプションＡ（右）
    _ADD( eBD_SLOT_LwOptionA,           0 );     // M:下半身オプションＡ（左）
    _ADD( eBD_SLOT_LwOptionB,           1 );     // M:下半身オプションＢ（右）
    _ADD( eBD_SLOT_LwOptionB,           0 );     // M:下半身オプションＢ（左）
    _ADD( eBD_SLOT_LwBase,              0 );    // S:下半身土台
    _ADD( eBD_SLOT_LwSymbol,            0 );    // S:エプロンの模様（※エプロン色上への出力を想定）

    //---------------------------------
    // 脚
    //---------------------------------
    _ADD( eBD_SLOT_LyLegFront,          0 );    // S:両脚の手前

    // 右脚（つま先〜ひざまで）
    _ADD( eBD_SLOT_LgOptionA,           1 );    // M:足オプションＡ（右）
    _ADD( eBD_SLOT_LgOptionB,           1 );    // M:足オプションＢ（右）
    _ADD( eBD_SLOT_FoBase,              1 );    // M:足先の土台（右）
    _ADD( eBD_SLOT_KnBase,              1 );    // M:膝下の土台（右）

    // 左脚（つま先〜ひざまで）
    _ADD( eBD_SLOT_LgOptionA,           0 );    // M:足オプションＡ（左）
    _ADD( eBD_SLOT_LgOptionB,           0 );    // M:足オプションＢ（左）
    _ADD( eBD_SLOT_FoBase,              0 );    // M:足先の土台（左）
    _ADD( eBD_SLOT_KnBase,              0 );    // M:膝下の土台（左）

    // 右脚（付け根）
    _ADD( eBD_SLOT_LgCover,             1 );    // M:脚カバー（右）
    _ADD( eBD_SLOT_LgBase,              1 );    // M:脚の土台（右）

    // 左脚（付け根）
    _ADD( eBD_SLOT_LgCover,             0 );    // M:脚カバー（左）
    _ADD( eBD_SLOT_LgBase,              0 );    // M:脚の土台（左）

    // 地面（※再奥＝足下に何か置けるように）
    _ADD( eBD_SLOT_FoGround,            1 );    // M:接地点（右）
    _ADD( eBD_SLOT_FoGround,            0 );    // M:接地点（左）

    _ADD( eBD_SLOT_LyLegBack,           0 );    // S:両脚の後ろ
    
    _ADD( eBD_SLOT_LyFaceCoverBack,     0 );    // s:顔カバーの後ろ
    _ADD( eBD_SLOT_LyFaceBack,          0 );    // s:顔ーの後ろ

    //---------------------------------
    // 背中（※腰まで届く後髪より手前にする）
    //---------------------------------
    _ADD( eBD_SLOT_BlRibbon,            1 );    // M:背中のリボン（右）
    _ADD( eBD_SLOT_BlRibbon,            0 );    // M:背中のリボン（左）

    _ADD( eBD_SLOT_LyBodyBack,          0 );    // S:背中

    //---------------------------------
    // 頭（※頭頂〜後髪／基本シルエットの完成）
    //---------------------------------
    _ADD( eBD_SLOT_HdTopCover,          0 );    // S:頭上カバー
    _ADD( eBD_SLOT_HdTop,               0 );    // S:頭上
    _ADD( eBD_SLOT_HdBase,              0 );    // S:頭土台

    _ADD( eBD_SLOT_LyHeadCoverBack,     0 );    // S:頭カバーの後ろ
    _ADD( eBD_SLOT_LyHeadBack,          0 );    // S:頭の後ろ

    //---------------------------------
    // 頭の付属物（※後髪のさらに奥）
    //---------------------------------
    _ADD( eBD_SLOT_HdHairDownKnot,      1 );    // S:結び目（おさげ）（右）
    _ADD( eBD_SLOT_HdHairDown,          1 );    // S:髪（おさげ）（右）

    _ADD( eBD_SLOT_HdHairDownKnot,      0 );    // S:結び目（おさげ）（左）
    _ADD( eBD_SLOT_HdHairDown,          0 );    // S:髪（おさげ）（左）

    _ADD( eBD_SLOT_HdHairSideKnot,      1 );    // S:結び目（ツインテール）（右）
    _ADD( eBD_SLOT_HdHairSide,          1 );    // S:髪（ツインテール）（右）

    _ADD( eBD_SLOT_HdHairSideKnot,      0 );    // S:結び目（ツインテール）（左）
    _ADD( eBD_SLOT_HdHairSide,          0 );    // S:髪（ツインテール）（左）
    
    _ADD( eBD_SLOT_HdHairBackKnot,      0 );    // S:結び目（ポニー照）
    _ADD( eBD_SLOT_HdHairBack,          0 );    // S:髪（ポニーテール）
        
    //---------------------------------
    // 最後面
    //---------------------------------
    _ADD( eBD_SLOT_LyBack,              0 );    // S:最後面
}

//---------------------------
// デフォルト設定：正面（着ぐるみ）
//---------------------------
void CBmpDotOrderData::setDefaultForFrontSuited( void ){
    // データは共通
    setDefaultForFront();
}
