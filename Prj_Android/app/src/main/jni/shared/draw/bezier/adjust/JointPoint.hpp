/*+----------------------------------------------------------------+
  |	Title:		JointPoint.hpp [共通環境]
  |	Comment:	結合点（※ベジェ描画時のワーク）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __JOINT_POINT_HPP__
#define __JOINT_POINT_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//---------------------------------------------------------------------------------------
// 結合対象
//---------------------------------------------------------------------------------------
// ・ジョイントとなる[BmpDotPartData]にて可動情報を登録しておき、その値を呼び出し元と共有するための枠組み
//（※腕などの幅の概念をもつパーツの二点を円で定義し、ジョイント先と呼び出し元で座標を同期させる）
//（※オーダー段階で基準座標が登録され、描画AP変換時に方向線の調整＆システム[HOOK]により座標が同期される）
//---------------------------------------------------------------------------------------
// ・データは[呼び出し元の左側の点、それに対応する結合対象の点]と[右側の点]で１セットとなる
//（※[TO:結合対象の値]ラベルから[１]を引いた値が、[FROM:呼び出し元の値]ラベルとなる）
//（※[0:呼び出し元の値]ラベルに[２]を足した値が、[1：結合対象]の値となる）
//（※スロットイインデックスの値に対して[４]を足した値が、逆側の値となる＝左腕に対する右腕等）
//---------------------------------------------------------------------------------------
enum eJOINT_POINT{
    eJOINT_POINT_INVALID = -1,  // 無効値
    
    // [UP_BODY]：腹から上半身へのつなぎ（※[UpBase]で設定される／[From:BlBase]→[To:UpBase]）
    eJOINT_POINT_UP_BODY_0_FROM,        eJOINT_POINT_UP_BODY_0_TO,
    eJOINT_POINT_UP_BODY_1_FROM,        eJOINT_POINT_UP_BODY_1_TO,

    // [LOW_BODY]：腹から下半身へのつなぎ（※[LwBase]で設定される／[From:BlBase]→[To:LwBase]）
    eJOINT_POINT_LOW_BODY_0_FROM,       eJOINT_POINT_LOW_BODY_0_TO,
    eJOINT_POINT_LOW_BODY_1_FROM,       eJOINT_POINT_LOW_BODY_1_TO,

    // [NECK]：上半身から首へのつなぎ（※[NkBase]で設定される／[From:UpBase]→[To:NkBase]）
    eJOINT_POINT_NECK_0_FROM,           eJOINT_POINT_NECK_0_TO,
    eJOINT_POINT_NECK_1_FROM,           eJOINT_POINT_NECK_1_TO,
    
    // [HEAD_TOP]：頭から頭頂へのつなぎ（※[HdTop]で設定される／[From:HdBase]→[To:HdTop]）
    eJOINT_POINT_HEAD_TOP_0_FROM,       eJOINT_POINT_HEAD_TOP_0_TO,
    eJOINT_POINT_HEAD_TOP_1_FROM,       eJOINT_POINT_HEAD_TOP_1_TO,

    // [TEMPLE]：顔から顔下へのつなぎ（※[FcUnder]で設定される／[From:FcBase]→[To:FcUnder]）
    eJOINT_POINT_TEMPLE_0_FROM,         eJOINT_POINT_TEMPLE_0_TO,
    eJOINT_POINT_TEMPLE_1_FROM,         eJOINT_POINT_TEMPLE_1_TO,

    // [SHOLDER]：上半身から腕へのつなぎ（※[AmBase]で設定される／[From:UpBase]→[To:AmBase]）
    eJOINT_POINT_SHOLDER_L0_FROM,       eJOINT_POINT_SHOLDER_L0_TO,
    eJOINT_POINT_SHOLDER_L1_FROM,       eJOINT_POINT_SHOLDER_L1_TO,
    
    //（※腕は左右があるので２セットもつ）
    eJOINT_POINT_SHOLDER_R0_FROM,       eJOINT_POINT_SHOLDER_R0_TO,
    eJOINT_POINT_SHOLDER_R1_FROM,       eJOINT_POINT_SHOLDER_R1_TO,

    // [ELBOW]：腕から肘先へのつなぎ（※[ElBase]で設定される／[From:AmBase]→[To:ElBase]）
    eJOINT_POINT_ELBOW_L0_FROM,         eJOINT_POINT_ELBOW_L0_TO,
    eJOINT_POINT_ELBOW_L1_FROM,         eJOINT_POINT_ELBOW_L1_TO,
    
    //（※肘先は左右があるので２セットもつ）
    eJOINT_POINT_ELBOW_R0_FROM,         eJOINT_POINT_ELBOW_R0_TO,
    eJOINT_POINT_ELBOW_R1_FROM,         eJOINT_POINT_ELBOW_R1_TO,

    // [WRIST]：肘先から手へのつなぎ（※[HnBase]でで設定される／[From:ElBase]→[To:HnBase]）
    eJOINT_POINT_WRIST_L0_FROM,         eJOINT_POINT_WRIST_L0_TO,
    eJOINT_POINT_WRIST_L1_FROM,         eJOINT_POINT_WRIST_L1_TO,

    //（※手は左右があるので２セットもつ）
    eJOINT_POINT_WRIST_R0_FROM,         eJOINT_POINT_WRIST_R0_TO,
    eJOINT_POINT_WRIST_R1_FROM,         eJOINT_POINT_WRIST_R1_TO,

    // [LEG]：下半身から脚へのつなぎ（※[LgBase]で設定される／[From:LwBase]→[To:LgBase]）
    eJOINT_POINT_LEG_L0_FROM,           eJOINT_POINT_LEG_L0_TO,
    eJOINT_POINT_LEG_L1_FROM,           eJOINT_POINT_LEG_L1_TO,
    
    //（※脚は左右があるので２セットもつ）
    eJOINT_POINT_LEG_R0_FROM,           eJOINT_POINT_LEG_R0_TO,
    eJOINT_POINT_LEG_R1_FROM,           eJOINT_POINT_LEG_R1_TO,

    // [KNEE]：脚から膝下へのつなぎ（※[KnBase]で設定される／[From:LgBase]→[To:KnBase]）
    eJOINT_POINT_KNEE_L0_FROM,          eJOINT_POINT_KNEE_L0_TO,
    eJOINT_POINT_KNEE_L1_FROM,          eJOINT_POINT_KNEE_L1_TO,
    
    //（※膝下は左右があるので２セットもつ）
    eJOINT_POINT_KNEE_R0_FROM,          eJOINT_POINT_KNEE_R0_TO,
    eJOINT_POINT_KNEE_R1_FROM,          eJOINT_POINT_KNEE_R1_TO,

    // [ANKLE]：膝下から足先へのつなぎ（※[FoBase]で設定される／[From:KnBase]→[To:FoBase]）
    eJOINT_POINT_ANKLE_L0_FROM,         eJOINT_POINT_ANKLE_L0_TO,
    eJOINT_POINT_ANKLE_L1_FROM,         eJOINT_POINT_ANKLE_L1_TO,

    //（※足先は左右があるので２セットもつ）
    eJOINT_POINT_ANKLE_R0_FROM,         eJOINT_POINT_ANKLE_R0_TO,
    eJOINT_POINT_ANKLE_R1_FROM,         eJOINT_POINT_ANKLE_R1_TO,
    
    // 胴体の汎用枠：Ａ
    eJOINT_POINT_BD_OPTION_A_L0_FROM,   eJOINT_POINT_BD_OPTION_A_L0_TO,
    eJOINT_POINT_BD_OPTION_A_L1_FROM,   eJOINT_POINT_BD_OPTION_A_L1_TO,

    //（※汎用枠先は左右があるので２セットもつ）
    eJOINT_POINT_BD_OPTION_A_R0_FROM,   eJOINT_POINT_BD_OPTION_A_R0_TO,
    eJOINT_POINT_BD_OPTION_A_R1_FROM,   eJOINT_POINT_BD_OPTION_A_R1_TO,

    // 胴体の汎用枠：Ｂ
    eJOINT_POINT_BD_OPTION_B_L0_FROM,   eJOINT_POINT_BD_OPTION_B_L0_TO,
    eJOINT_POINT_BD_OPTION_B_L1_FROM,   eJOINT_POINT_BD_OPTION_B_L1_TO,

    //（※汎用枠先は左右があるので２セットもつ）
    eJOINT_POINT_BD_OPTION_B_R0_FROM,   eJOINT_POINT_BD_OPTION_B_R0_TO,
    eJOINT_POINT_BD_OPTION_B_R1_FROM,   eJOINT_POINT_BD_OPTION_B_R1_TO,

    eJOINT_POINT_MAX,
};

// 始点／終点：始点を偶数として扱って＋１したのが終点となる（※始点に＋１すると終点となる）
#define JOINT_POINT_OFS_FOR_FROM_TO         1

// 基本セット／対応セット：基本セットに対して＋２したものが対応セットとなる（※型の開始点に対して＋２すると、逆側の点となる）
#define JOINT_POINT_OFS_FOR_OTHER_SIDE      2

// 左／右：左の要素に対して＋４したものが右の要素となる（※左肩の基本セットの開始点に対して＋４すると、右肩の対応する点となる）
#define JOINT_POINT_OFS_FOR_SLOT_INDEX      4

// トータルデータ数（※指定枠と同じではあるが一応）
// ベジェ描画時の[HOOK]枠の末尾に[eJOINT_POINT_TOTAL_MAX]ぶんの領域が確保される
#define eJOINT_POINT_TOTAL_MAX              eJOINT_POINT_MAX

// 有効性の判定（※スロットインデックスを加味した値に対しては[FIXED]により判定すること）
#define IS_JOINT_POINT_VALID( _p )          ((_p)>=(eJOINT_POINT)0 && (_p)<eJOINT_POINT_MAX)
#define IS_JOINT_POINT_FIXED_VALID( _p )    ((_p)>=0 && (_p)<eJOINT_POINT_TOTAL_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//---------------------------------------------
// 登録情報の登録単位（※オーダー解釈時の情報を保持する）
//---------------------------------------------
struct stJOINT_POINT_CELL{
    // [registJointPoint]で登録
    bool  bValid;                   // 有効か？
    
    bool  bTo;                      // 接続先[TO]か？
    bool  bFlipH, bFlipV;           // 横／縦に反転しているか？

    int   nRateX, nRateY;           // 割合座標（※結合用＝角度調整後の座標）
    int   nRateOfsX, nRateOfsY;     // 割合座標（※表示のオフセット）

    float fScaleAbs;                // 表示スケール（※絶対値＝符号は[bFlipX/bFlipV]で判断）
    float fRot0;                    // 表示角度（※重なり判定のために使う調整前の値）

    // [setDirRateXY]で登録
    float openDirX;                 // 結合部分が開く際の方向線スケールX
    float openDirY;                 // 結合部分が開く際の方向線スケールY
    float closeDirX;                // 結合部分が閉じる際の方向線スケールX
    float closeDirY;                // 結合部分が閉じる際の方向線スケールY
};

// クリア
#define CLEAR_JOINT_POINT_CELL( _pCell )                        \
    memset( (_pCell), 0, sizeof(stJOINT_POINT_CELL) );          \
    (_pCell)->fScaleAbs = 1.0f

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// 結合点
//----------------------------------------
class CJointPoint{
public:
    // 調整：返値は[int]型となる（※調整後の[id]は[eJOINT_POINT_MAX]とは別管理となるので）
    static int AdjustJointPoint( eJOINT_POINT target, int slotIndex );

protected:
    // 登録データ
    stJOINT_POINT_CELL m_stArrCell[eJOINT_POINT_TOTAL_MAX];

public:
    // コンストラクタ／デストラクタ
    CJointPoint( void );
    virtual ~CJointPoint( void ){}
    
    // クリア
    void clear( void );

    // 登録情報の取得（※無効時は[NULL]を返す)
    stJOINT_POINT_CELL* getJointPoint( int idFixed );

    //-----------------------------
    // [FROM]向けの登録
    //-----------------------------
    inline void registJointPointFrom( int idFixed, int rateX, int rateY, int ofsRX, int ofsRY,
                                      float scaleAbs, float rot0, bool isFlipH, bool isFlipV ){
        registJointPoint( idFixed, rateX, rateY, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, false );
    }

    //-----------------------------
    // [TO]向けの登録
    //-----------------------------
    inline void registJointPointTo( int idFixed, int rateX, int rateY, int ofsRX, int ofsRY,
                                    float scaleAbs, float rot0, bool isFlipH, bool isFlipV ){
        registJointPoint( idFixed, rateX, rateY, ofsRX, ofsRY, scaleAbs, rot0, isFlipH, isFlipV, true );
    }
    
    //---------------------------------
    // 方向線調整値の設定（※[FROM/TO]で共通
    //---------------------------------
    void setDirRateXY( int idFixed, float openDirX, float openDirY, float closeDirX, float closeDirY );
    
protected:
    // 座標の登録（実体）
    void registJointPoint( int idFixed, int rateX, int rateY, int ofsRX, int ofsRY,
                           float scaleAbs, float rot, bool isFlipH, bool isFlipV, bool isTo );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelJointPoint[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __JOINT_POINT_HPP__
