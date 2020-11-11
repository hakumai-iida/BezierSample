/*+----------------------------------------------------------------+
  |	Title:		Bezier.cpp [共通環境]
  |	Comment:	ベジェ曲線（※対象はパレット画像[P8D]）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Bezier.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理
bool CBezier::s_bDispOffTouch;
bool CBezier::s_bDispOffStripe;
bool CBezier::s_bDispOffFrill;
bool CBezier::s_bDispOffEdge;
bool CBezier::s_bDispOffFillOption;

bool CBezier::s_bDispDirVectorAlways;
bool CBezier::s_bDispOffDirVectorAlways;
bool CBezier::s_bDispAllSelected;
bool CBezier::s_bDispOnlyAnchorPoint;

bool CBezier::s_bDispIndividualPart;
bool CBezier::s_bDispHookOfsValid;
bool CBezier::s_bDispHookOfsInvalid;

// 分割点
stBEZIER_DIVISION_POINT* CBezier::s_stArrDivisionPoint;
int CBezier::s_nUseDivisionPoint;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseDivisionPointMax;
#endif

// テンポラリ
float* CBezier::s_fArrTempPointX;
float* CBezier::s_fArrTempPointY;
float* CBezier::s_fArrTempPointX0;
float* CBezier::s_fArrTempPointY0;
float* CBezier::s_fArrTempStrokeSize;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseTempPointMax;
#endif

// フック
bool* CBezier::s_bArrHookPoint;
float* CBezier::s_fArrHookPointX;
float* CBezier::s_fArrHookPointY;

// タッチ
int* CBezier::s_nArrTouchPointNum;
float** CBezier::s_fArrArrTouchPointX;
float** CBezier::s_fArrArrTouchPointY;
float* CBezier::s_fArrTouchPoint;
float** CBezier::s_fArrArrTouchPointX0;
float** CBezier::s_fArrArrTouchPointY0;
float* CBezier::s_fArrTouchPoint0;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseTouchPointMax;
#endif
float* CBezier::s_fArrTouchWorkForLen0;
float* CBezier::s_fArrSubTouchWorkForLen0;

// ガイド
int* CBezier::s_nArrGuidePointNum;
float** CBezier::s_fArrArrGuidePointX;
float** CBezier::s_fArrArrGuidePointY;
float* CBezier::s_fArrGuidePoint;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseGuidePointMax;
#endif

// ストライプ
int CBezier::s_nStripePointNum;
BYTE* CBezier::s_nArrStripePalGrp;
float* CBezier::s_fArrStripePointX;
float* CBezier::s_fArrStripePointY;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseStripePointMax;
#endif

// ストライプ用ガイド
int CBezier::s_nNumStripeGuidePoint;
float* CBezier::s_fArrStripeGuidePointX;
float* CBezier::s_fArrStripeGuidePointY;
#ifdef BEZIER_USE_NUM_CHECK
int CBezier::s_nUseStripeGuidePointMax;
#endif

// エッジ塗り
int CBezier::s_nBufSizeForEdgeFillGuide;
BYTE* CBezier::s_pBufForEdgeFillGuide;
int CBezier::s_nOfsXForEdgeFillGuide;
int CBezier::s_nOfsYForEdgeFillGuide;
stBEZIER_ANCHOR_POINT* CBezier::s_pApForEdgeFillGuide;

// スキップストローク
int CBezier::s_nSkipStrokePointCur;
int CBezier::s_nSkipStrokePointMax;
int CBezier::s_nSkipStrokePointLimitForHead;
int CBezier::s_nSkipStrokePointLimitForTail;

// パラメータ
stBEZIER_BASE_PARAM*  CBezier::s_pBaseParam;
stBEZIER_LAYER_PARAM* CBezier::s_pLayerParam;

// 塗りつぶし対象
BYTE* CBezier::s_pBuf;
BYTE* CBezier::s_pBufTest;
BYTE* CBezier::s_pBufFillGuide;
BYTE* CBezier::s_pBufFillGuard;
BYTE* CBezier::s_pBufTemp;

int CBezier::s_nBufW;
int CBezier::s_nBufH;

BYTE CBezier::s_nTestPalGrp;
BYTE CBezier::s_nTestPalGrpForRepair;
bool CBezier::s_bTouch;
bool CBezier::s_bStripe;
bool CBezier::s_bFrill;
bool CBezier::s_bWorkPath;

// ワークパス用
int CBezier::s_nSizeForWorkPathLine;
int CBezier::s_nSizeForWorkPathPoint;
int CBezier::s_nSizeForWorkPathSelected;
int CBezier::s_nSizeForWorkPathTriangle;
int CBezier::s_nSizeForWorkPathCross;
int CBezier::s_nSizeForWorkPathPlus;
BYTE* CBezier::s_pDotForWorkPathTriangle;
BYTE* CBezier::s_pDotForWorkPathCross;
BYTE* CBezier::s_pDotForWorkPathPlus;

// フリルデータ
CLayerData* CBezier::s_pArrLayerDataForFrill[eSTROKE_FRILL_MAX];

// ブラシのブレ用
CSpring CBezier::s_oSpringForBrushPos;
CSpring CBezier::s_oSpringForBrushSize;
int CBezier::m_nResetCountForBrushPos;
int CBezier::m_nResetCountForBrushSize;
float CBezier::m_nUpdateFrameForBrushPos;
float CBezier::m_nUpdateFrameForBrushSize;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------------------
// 確保数（※管理内で[new]するセル数）
//--------------------------------------
int CBezier::GetAllocCellNum( void ){
    int num = 0;

    //---------------------------------
    // CAnchorPointData::Init()
    // CFillPointData::Init()
    // CSlotPointData::Init()
    // CLineObjectData::Init()
    // CPaintObjectData::Init()
    // CSlotObjectData::Init()
    // CLayerData::Init()
    //---------------------------------
    num += CAnchorPointData::GetAllocCellNum();
    num += CFillPointData::GetAllocCellNum();
    num += CSlotPointData::GetAllocCellNum();
    num += CLineObjectData::GetAllocCellNum();
    num += CPaintObjectData::GetAllocCellNum();
    num += CSlotObjectData::GetAllocCellNum();
    num += CLayerData::GetAllocCellNum();

    //---------------------------------------------------------------------------------
    // s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 1;

    //---------------------------------------------------------------------------------
    // s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 5;

    //---------------------------------------------------------------------------------
    // s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 3;

    //---------------------------------------------------------------------------------
    // s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_TOTAL_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TARGET_TOTAL_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    // s_fArrSubTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 9;

    //---------------------------------------------------------------------------------
    // s_nArrGuidePointNum = new int[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrArrGuidePointX = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrArrGuidePointY = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrGuidePoint = new float[eSTROKE_GUIDE_TARGET_TOTAL_MAX*2*BEZIER_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 4;

    //---------------------------------------------------------------------------------
    // s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 3;

    //---------------------------------------------------------------------------------
    // s_fArrStripeGuidePointX = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];
    // s_fArrStripeGuidePointY = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    num += 2;

    //---------------------------------------------------------------------------------
    // s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    // s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];
    //---------------------------------------------------------------------------------
    num += 1;

    //---------------------------------------------------------------------------------
    // s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    //---------------------------------------------------------------------------------
    num += 3;
    
    return( num );
}

//--------------------------------------
// 確保サイズ（※管理内で[new]する領域サイズ）
//--------------------------------------
int CBezier::GetAllocSizeK( void ){
    int size = 0;

#ifdef BEZIER_USE_NUM_CHECK
    int size0 = 0;
    LOGD( "@ === CHECK BEZIER ALLOC SIZE ==\n" );
#endif
    
    //---------------------------------
    // CAnchorPointData::Init()
    // CFillPointData::Init()
    // CSlotPointData::Init()
    // CLineObjectData::Init()
    // CPaintObjectData::Init()
    // CSlotObjectData::Init()
    // CLayerData::Init()
    //---------------------------------
    size += CAnchorPointData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CAnchorPointData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    size += CFillPointData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CFillPointData]\n", (size-size0)/KBf );
    size0 = size;
#endif
    
    size += CSlotPointData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CSlotPointData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    size += CLineObjectData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CLineObjectData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    size += CPaintObjectData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CPaintObjectData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    size += CSlotObjectData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CSlotObjectData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    size += CLayerData::GetAllocSize();
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [CLayerData]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(stBEZIER_DIVISION_POINT) * BEZIER_DIVISION_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_stArrDivisionPoint]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    // s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += 5 * sizeof(float) * BEZIER_TEMP_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_fArrTemp*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    // s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(bool) * BEZIER_HOOK_POINT_MAX;
    size += 2 * sizeof(float) * BEZIER_HOOK_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nArrHookPoint*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_TOTAL_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    // s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TOTAL_TARGET_MAX*2*BEZIER_TOUCH_POINT_MAX];
    // s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    // s_fArrSubTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(int) * eSTROKE_TOUCH_TARGET_TOTAL_MAX;
    size += 2 * sizeof(float*) * eSTROKE_TOUCH_TARGET_TOTAL_MAX;
    size += sizeof(float) * eSTROKE_TOUCH_TARGET_TOTAL_MAX * 2 * BEZIER_TOUCH_POINT_MAX;
    size += 2 * sizeof(float*) * eSTROKE_TOUCH_TARGET_TOTAL_MAX;
    size += sizeof(float) * eSTROKE_TOUCH_TARGET_TOTAL_MAX * 2 * BEZIER_TOUCH_POINT_MAX;
    size += sizeof(float) * BEZIER_TOUCH_POINT_MAX;
    size += sizeof(float) * BEZIER_TOUCH_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nArrTouch*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_nArrGuidePointNum = new int[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrArrGuidePointX = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrArrGuidePointY = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    // s_fArrGuidePoint = new float[eSTROKE_GUIDE_TARGET_TOTAL_MAX*2*BEZIER_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(int) * eSTROKE_GUIDE_TARGET_TOTAL_MAX;
    size += 2 * sizeof(float*) * eSTROKE_GUIDE_TARGET_TOTAL_MAX;
    size += sizeof(float) * eSTROKE_GUIDE_TARGET_TOTAL_MAX * 2 * BEZIER_GUIDE_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nArrGuide*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    // s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += sizeof(BYTE) * BEZIER_STRIPE_POINT_MAX;
    size += 2 * sizeof(float) * BEZIER_STRIPE_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nArrStripe*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_fArrStripeGuidePointX = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];
    // s_fArrStripeGuidePointY = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];
    //---------------------------------------------------------------------------------
    size += 2 * sizeof(int) * BEZIER_STRIPE_GUIDE_POINT_MAX;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nArrStripeGuidePoint*]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    // s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];
    //---------------------------------------------------------------------------------
    size += sizeof(BYTE) * BEZIER_EDGE_FILL_GUIDE_SIZE * BEZIER_EDGE_FILL_GUIDE_SIZE;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_pBufForEdgeFillGuide]\n", (size-size0)/KBf );
    size0 = size;
#endif

    //---------------------------------------------------------------------------------
    // s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    // s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    //---------------------------------------------------------------------------------
    size += 3 * sizeof(BYTE) * BEZIER_WORK_DOT_SIZE;
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@  [%.1f KB] for [s_nDotForWorkPath*]\n", (size-size0)/KBf );
    size0 = size;
#endif
    
#ifdef BEZIER_USE_NUM_CHECK
    LOGD( "@ === TOTAL [%.1f KB] FOR BEZIER ==\n", size/KBf );
#endif

    // キロ単位にして返す
    int sizeK = (size+KB-1)/KB;
    return( sizeK );
}

//-------------------
// 開始
//-------------------
bool CBezier::OnCreate( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBezier::OnCreate()\n" );
#endif

    //-------------------------------------------------
    // [IMPLEMENT_DATA_ALLOCATOR]系管理クラスの初期化
    //-------------------------------------------------
    // アンカーポイントデータ管理初期化
    if( ! CAnchorPointData::Init() ){
        LOGE( "@ CAnchorPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CAnchorPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // 塗りポイントデータ管理初期化
    if( ! CFillPointData::Init() ){
        LOGE( "@ CFillPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CFillPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // スロットポイントデータ管理初期化
    if( ! CSlotPointData::Init() ){
        LOGE( "@ CSlotPointData::Init(): FAILED!!\n" );
        env_push_error_log( "[CSlotPointData::Init()]に失敗しました。\n" );
        return( false );
    }

    // ラインオブジェクトデータ管理初期化
    if( ! CLineObjectData::Init() ){
        LOGE( "@ CLineObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CLineObjectData::Init()]に失敗しました。\n" );
        return( false );
    }

    // ペイントオブジェクトデータ管理初期化
    if( ! CPaintObjectData::Init() ){
        LOGE( "@ CPaintObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CPaintObjectData::Init()]に失敗しました。\n" );
        return( false );
    }
    
    // スロットオブジェクトデータ管理初期化
    if( ! CSlotObjectData::Init() ){
        LOGE( "@ CSlotObjectData::Init(): FAILED!!\n" );
        env_push_error_log( "[CSlotObjectData::Init()]に失敗しました。\n" );
        return( false );
    }

    // レイヤーデータ管理初期化
    if( ! CLayerData::Init() ){
        LOGE( "@ CLayerData::Init(): FAILED!!\n" );
        env_push_error_log( "[CLayerData::Init()]に失敗しました。\n" );
        return( false );
    }
    
    // フリルデータ確保
    if( ! AllocFrill() ){
        LOGE( "@ AllocInit(): FAILED!!\n" );
        env_push_error_log( "[CBezier::AllocFrill()]に失敗しました。\n" );
        return( false );
    }
    
    // 領域確保
    CMemMgr::PushTargetField( eMEM_FIELD_D_BEZIER );
    s_stArrDivisionPoint = new stBEZIER_DIVISION_POINT[BEZIER_DIVISION_POINT_MAX];
    
    s_fArrTempPointX = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointY = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointX0 = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempPointY0 = new float[BEZIER_TEMP_POINT_MAX];
    s_fArrTempStrokeSize = new float[BEZIER_TEMP_POINT_MAX];
        
    s_bArrHookPoint = new bool[BEZIER_HOOK_POINT_MAX];
    s_fArrHookPointX = new float[BEZIER_HOOK_POINT_MAX];
    s_fArrHookPointY = new float[BEZIER_HOOK_POINT_MAX];
    
    s_nArrTouchPointNum = new int[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    s_fArrArrTouchPointX = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    s_fArrArrTouchPointY = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    s_fArrTouchPoint = new float[eSTROKE_TOUCH_TARGET_TOTAL_MAX*2*BEZIER_TOUCH_POINT_MAX];
    s_fArrArrTouchPointX0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    s_fArrArrTouchPointY0 = new float*[eSTROKE_TOUCH_TARGET_TOTAL_MAX];
    s_fArrTouchPoint0 = new float[eSTROKE_TOUCH_TARGET_TOTAL_MAX*2*BEZIER_TOUCH_POINT_MAX];
    s_fArrTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];
    s_fArrSubTouchWorkForLen0 = new float[BEZIER_TOUCH_POINT_MAX];

    s_nArrGuidePointNum = new int[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    s_fArrArrGuidePointX = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    s_fArrArrGuidePointY = new float*[eSTROKE_GUIDE_TARGET_TOTAL_MAX];
    s_fArrGuidePoint = new float[eSTROKE_GUIDE_TARGET_TOTAL_MAX*2*BEZIER_GUIDE_POINT_MAX];

    s_nArrStripePalGrp = new BYTE[BEZIER_STRIPE_POINT_MAX];
    s_fArrStripePointX = new float[BEZIER_STRIPE_POINT_MAX];
    s_fArrStripePointY = new float[BEZIER_STRIPE_POINT_MAX];
    
    s_fArrStripeGuidePointX = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];
    s_fArrStripeGuidePointY = new float[BEZIER_STRIPE_GUIDE_POINT_MAX];

    s_nBufSizeForEdgeFillGuide = BEZIER_EDGE_FILL_GUIDE_SIZE;
    s_pBufForEdgeFillGuide = new BYTE[s_nBufSizeForEdgeFillGuide*s_nBufSizeForEdgeFillGuide];

    // ワークパス用
    s_pDotForWorkPathTriangle = new BYTE[BEZIER_WORK_DOT_SIZE];
    s_pDotForWorkPathCross = new BYTE[BEZIER_WORK_DOT_SIZE];
    s_pDotForWorkPathPlus = new BYTE[BEZIER_WORK_DOT_SIZE];
    CMemMgr::PopTargetField();

    // タッチXY配列へバッファの割り当て
    for( int i=0; i<eSTROKE_TOUCH_TARGET_TOTAL_MAX; i++ ){
        s_fArrArrTouchPointX[i] = &s_fArrTouchPoint[(2*i+0)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointY[i] = &s_fArrTouchPoint[(2*i+1)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointX0[i] = &s_fArrTouchPoint0[(2*i+0)*BEZIER_TOUCH_POINT_MAX];
        s_fArrArrTouchPointY0[i] = &s_fArrTouchPoint0[(2*i+1)*BEZIER_TOUCH_POINT_MAX];
    }

    // ガイドXY配列へバッファの割り当て
    for( int i=0; i<eSTROKE_GUIDE_TARGET_TOTAL_MAX; i++ ){
        s_fArrArrGuidePointX[i] = &s_fArrGuidePoint[(2*i+0)*BEZIER_GUIDE_POINT_MAX];
        s_fArrArrGuidePointY[i] = &s_fArrGuidePoint[(2*i+1)*BEZIER_GUIDE_POINT_MAX];
    }

    // ワークパス用ドット準備
    ReadyDotForWorkPath();
    
    return( true );
}

//-------------------
// 終了
//-------------------
void CBezier::OnDestroy( void ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CBezier::OnDestroy()\n" );
#endif

    // 領域解放
    SAFE_DELETE_ARRAY( s_stArrDivisionPoint );

    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointX );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointY );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointX0 );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempPointY0 );
    SAFE_DELETE_ARRAY( CBezier::s_fArrTempStrokeSize );
    
    SAFE_DELETE_ARRAY( s_bArrHookPoint );
    SAFE_DELETE_ARRAY( s_fArrHookPointX );
    SAFE_DELETE_ARRAY( s_fArrHookPointY );

    SAFE_DELETE_ARRAY( s_nArrTouchPointNum );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointX );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointY );
    SAFE_DELETE_ARRAY( s_fArrTouchPoint );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointX0 );
    SAFE_DELETE_ARRAY( s_fArrArrTouchPointY0 );
    SAFE_DELETE_ARRAY( s_fArrTouchPoint0 );
    SAFE_DELETE_ARRAY( s_fArrTouchWorkForLen0 );
    SAFE_DELETE_ARRAY( s_fArrSubTouchWorkForLen0 );

    SAFE_DELETE_ARRAY( s_nArrGuidePointNum );
    SAFE_DELETE_ARRAY( s_fArrArrGuidePointX );
    SAFE_DELETE_ARRAY( s_fArrArrGuidePointY );
    SAFE_DELETE_ARRAY( s_fArrGuidePoint );

    SAFE_DELETE_ARRAY( s_nArrStripePalGrp );
    SAFE_DELETE_ARRAY( s_fArrStripePointX );
    SAFE_DELETE_ARRAY( s_fArrStripePointY );
    
    SAFE_DELETE_ARRAY( s_fArrStripeGuidePointX );
    SAFE_DELETE_ARRAY( s_fArrStripeGuidePointY );

    SAFE_DELETE_ARRAY( s_pBufForEdgeFillGuide );

    // ワークパス用
    SAFE_DELETE_ARRAY( s_pDotForWorkPathTriangle );
    SAFE_DELETE_ARRAY( s_pDotForWorkPathCross );
    SAFE_DELETE_ARRAY( s_pDotForWorkPathPlus );

    // フリル解放
    ReleaseFrill();
    
    //-------------------------------------------------
    // [IMPLEMENT_DATA_ALLOCATOR]系管理クラスの終了
    //-------------------------------------------------
    CLayerData::Exit();
    CSlotObjectData::Exit();
    CPaintObjectData::Exit();
    CLineObjectData::Exit();
    CSlotPointData::Exit();
    CFillPointData::Exit();
    CAnchorPointData::Exit();
}

#ifdef BEZIER_USE_NUM_CHECK
//------------------------
// 利用状況取得
//------------------------
int CBezier::GetDivisionPointNumMax( void ){ return( BEZIER_DIVISION_POINT_MAX ); }
int CBezier::GetDivisionPointNumUseMax( void ){ return( s_nUseDivisionPointMax ); }

int CBezier::GetTempPointNumMax( void ){ return( BEZIER_TEMP_POINT_MAX ); }
int CBezier::GetTempPointNumUseMax( void ){ return( s_nUseTempPointMax ); }

int CBezier::GetTouchPointNumMax( void ){ return( BEZIER_TOUCH_POINT_MAX ); }
int CBezier::GetTouchPointNumUseMax( void ){ return( s_nUseTouchPointMax ); }

int CBezier::GetGuidePointNumMax( void ){ return( BEZIER_GUIDE_POINT_MAX ); }
int CBezier::GetGuidePointNumUseMax( void ){ return( s_nUseGuidePointMax ); }

int CBezier::GetStripePointNumMax( void ){ return( BEZIER_STRIPE_POINT_MAX ); }
int CBezier::GetStripePointNumUseMax( void ){ return( s_nUseStripePointMax ); }

int CBezier::GetStripeGuidePointNumMax( void ){ return( BEZIER_STRIPE_GUIDE_POINT_MAX ); }
int CBezier::GetStripeGuidePointNumUseMax( void ){ return( s_nUseStripeGuidePointMax ); }
#endif

//----------------------
// フック登録リセット
//----------------------
void CBezier::ResetHookPoint( bool isAll ){
    if( isAll ){
        // 全リセット
        for( int i=0; i<BEZIER_HOOK_POINT_MAX; i++ ){
            s_bArrHookPoint[i] = false;
        }
    }else{
        // テンポラリだけリセット
        for( int i=eSTROKE_HOOK_TARGET_TEMP_START; i<=eSTROKE_HOOK_TARGET_TEMP_END; i++ ){
            s_bArrHookPoint[i] = false;
        }
    }
}

//----------------------
// タッチ登録リセット
//----------------------
void CBezier::ResetTouchPoint( bool isAll ){
    if( isAll ){
        // 全リセット
        for( int i=0; i<eSTROKE_TOUCH_TARGET_TOTAL_MAX; i++ ){
            s_nArrTouchPointNum[i] = 0;
        }
    }else{
        // テンポラリだけリセット
        for( int i=eSTROKE_TOUCH_TARGET_TEMP_START; i<=eSTROKE_TOUCH_TARGET_TEMP_END; i++ ){
            s_nArrTouchPointNum[i] = 0;
        }
    }
}

//----------------------
// ガイド登録リセット
//----------------------
void CBezier::ResetGuidePoint( bool isAll ){
    if( isAll ){
        // 全リセット
        for( int i=0; i<eSTROKE_GUIDE_TARGET_TOTAL_MAX; i++ ){
            s_nArrGuidePointNum[i] = 0;
        }
    }else{
        // テンポラリだけリセット
        for( int i=eSTROKE_GUIDE_TARGET_TEMP_START; i<=eSTROKE_GUIDE_TARGET_TEMP_END; i++ ){
            s_nArrGuidePointNum[i] = 0;
        }
    }
}

//----------------------
// ストロークの描画
//----------------------
int CBezier::Stroke( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                     BYTE testPalGrp, BYTE testPalGrpForRepair, bool isDot, bool isWorkPath ){
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Stroke: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();

    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Stroke: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    s_nTestPalGrpForRepair = testPalGrpForRepair;

    //--------------------------
    // 描画：ストローク
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );

    // 実処理
    if( isDot ){
        DrawDot( pHead );
    }else{
        DrawStroke( pHead );
    }
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Stroke(isDot:%d): %dus, divPoint=%d\n", isDot, time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//-------------------------------------
// タッチの描画（※ストライプもここで処理する）
//-------------------------------------
int CBezier::Touch( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                    BYTE testPalGrp, bool isStripe, bool isWorkPath ){

    // 指定があれば無視
    if( IsDispOffTouch() ){
        if( ! isStripe ){
            return( 0 );
        }
    }

    // 指定があれば無視
    if( IsDispOffStripe() ){
        if( isStripe ){
            return( 0 );
        }
    }

    // ワークパスには対応しない
    if( isWorkPath ){
        return( 0 );
    }
    
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Touch: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();

    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Touch: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;

    // タッチフラグオン
    s_bTouch = true;
    s_bStripe = isStripe;   // ストライプ指定もフラグで管理

    //--------------------------
    // 描画：タッチ
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawTouch( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Touch: %dus, divPoint=%d\n", time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//----------------------
// フリルの描画
//----------------------
int CBezier::TouchForFrill( stBEZIER_ANCHOR_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                            BYTE testPalGrp, bool isWorkPath ){
    
    // 指定があれば無視
    if( IsDispOffFrill() ){
        return( 0 );
    }

    // ワークパスには対応しない
    if( isWorkPath ){
        return( 0 );
    }

    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::TouchForFrill: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForStroke( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::TouchForFrill: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    
    // フリルフラグオン
    s_bFrill = true;
    
    //--------------------------
    // 描画：フリル
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawFrill( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::TouchForFrill: %dus, divPoint=%d\n", time, s_nUseDivisionPoint );
    }
#endif
    
    return( time );
}

//----------------------
// 塗りつぶし
//----------------------
int CBezier::Fill( stBEZIER_FILL_POINT* pHead, stBEZIER_BASE_PARAM* pParam, stBEZIER_LAYER_PARAM* pLayerParam,
                   BYTE testPalGrp, bool isWorkPath ){
    // 用心
    if( pHead == NULL ){
        LOGE( "@ CBezier::Fill: INVALID DATA: pHead=%p\n", pHead );
        return( 0 );
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForFill( pParam, pLayerParam, isWorkPath ) ){
        LOGE( "@ CBezier::Fill: FAILED: BUF SETTING\n" );
        return( 0 );
    }

    // パラメータの保持
    s_nTestPalGrp = testPalGrp;
    
    //--------------------------
    // 描画：塗り
    //--------------------------
    // 計測開始
    time_log_start( eTIME_LOG_ID_BEZIER );
    
    // 実処理
    DrawFill( pHead );
    
    // 計測終了
    int time = (int)time_log_stop( eTIME_LOG_ID_BEZIER );
#ifdef BEZIER_TIME_LOG
    if( ! isWorkPath ){
        LOGD( "@BTL-----[DONE] CBezier::Fill: %dus\n", time );
    }
#endif
    
    return( time );
}

//----------------------
// スロット（※開発用）
//----------------------
void CBezier::SlotForDev( stBEZIER_SLOT_POINT* pHead, stBEZIER_BASE_PARAM* pParam ){
    // 指定があれば無視（※ここではスロットのマークを描くだけなので）
    if( IsDispOnlyAnchorPoint() ){
        return;
    }

    // 用心
    if( pHead == NULL  ){
        LOGE( "@ CBezier::SlotForDev: INVALID DATA: pHead=%p\n", pHead );
        return;
    }
    
    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForDev( pParam ) ){
        LOGE( "@ CBezier::SlotForDev: FAILED: BUF SETTING\n" );
        return;
    }
    
    //-------------------------------
    // 描画：スロット
    //-------------------------------
    stBEZIER_SLOT_POINT* pSP = pHead;
        
    while( pSP != NULL ){
        PutPlusForWorkPath( pSP->x, pSP->y, BEZIER_WP_SLOT_PAL + pSP->workPathPalOfs );
        
        // 次のポイントへ
        pSP = pSP->pNext;
    }
}

//------------------------------------------------------------------------
// 情報：基準点（※常にスロットの位置に表示されるので意味は薄いが、目印として表示してみる）
//------------------------------------------------------------------------
void CBezier::BasePointForDev( float x, float y, stBEZIER_BASE_PARAM* pParam, bool isActive, bool isSelected ){
    // 指定があれば無視（※ここでは基準点のマークを描くだけなので）
    if( IsDispOnlyAnchorPoint() ){
        return;
    }

    // ワーククリア
    ClearWork();
    
    // バッファ設定
    if( ! SetBufForDev( pParam ) ){
        LOGE( "@ CBezier::BasePointForDev: FAILED: BUF SETTING\n" );
        return;
    }

    //-------------------------------
    // 描画：基準点
    //-------------------------------
    BYTE palOfs = BEZIER_WP_PAL_VAL_FOR_SLEEP;
    
    if( IsDispAllSelected() ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;
    }else if( isSelected ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_SELECTED;
    }else if( isActive ){
        palOfs = BEZIER_WP_PAL_VAL_FOR_ACTIVE;
    }

    PutPlusForWorkPathS( x, y, BEZIER_WP_BASE_PAL + palOfs );
}

//----------------------------------------------------------------------------
// ２点間を線形補間で描画（※ループ内部での判定を避けるために座標計算後に状況毎の描画をしている）
//---------------------------------------------------------------------------
static int __stepDotLineForWorkPath = 0;
void CBezier::PutDotLinear( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP ){
    // 分割点間の座標を算出して一時バッファに格納（※返値は登録された座標数）
    int point = CalcThenSetPointForTempBuf( pDP0, pDP1, pAP );
    if( point <=0 ){
        return;
    }
    
    // ストロークの取り出し
    CStroke* pStroke = GetStrokeWithAnchorPoint( pAP );

    //--------------------------------------
    // ワークパス指定があればポイントの出力
    //--------------------------------------
    if( s_bWorkPath ){
        BYTE pal = BEZIER_WP_LINE_PAL + pAP->workPathPalOfsS;
        
        // フックで調整されているのであれば色を調整
        if( IsDispHookOfsValid() || IsDispHookOfsInvalid()  ){
            if( IsDispHookOfsInvalid() ){
                pal = BEZIER_WP_HOOK_TEMP_POINT_PAL + pAP->workPathPalOfsS;
            }else/* if( IsDispHookOfsValid() )*/{
                pal = BEZIER_WP_HOOK_POINT_PAL + pAP->workPathPalOfsS;
            }
        }

        // ストロークが無効であれば破線
        if( pStroke == NULL ){
            int step = 8;
            for( int i=0; i<point; i++ ){
                __stepDotLineForWorkPath++;
                if( __stepDotLineForWorkPath >= step ){
                    PutLineDotForWorkPath( (int)s_fArrTempPointX[i], (int)s_fArrTempPointY[i], pal );
                    __stepDotLineForWorkPath = 0;
                }
            }
        }
        // ストロークが有効であれば実線
        else {
            for( int i=0; i<point; i++ ){
                PutLineDotForWorkPath( (int)s_fArrTempPointX[i], (int)s_fArrTempPointY[i], pal );
            }
        }
        
        // ワークパス時はここで終了
        return;
    }
    
    //--------------------------------------
    // ここまできたら通常出力
    //--------------------------------------
    // ストロークが有効なら
    if( pStroke != NULL ){
        for( int i=0; i<point; i++ ){
            if( s_fArrTempStrokeSize[i] > 0.0f ){
                PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrp, pStroke, pAP );
            }
        }
        
        // ライン修復指定があれば線の引き直し
        //（※BDP間で線を描く際、先行するBDPの線のエッジに色が塗られてしまうことで線の継ぎ目が欠けたようになるため、指定の塗り色を対象に線を引き直す）
        if( pAP->bLineRepairTest && s_nTestPalGrpForRepair != s_nTestPalGrp ){
            for( int i=0; i<point; i++ ){
                if( s_fArrTempStrokeSize[i] > 0.0f ){
                    PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrpForRepair, pStroke, pAP );
                }
            }
        }
    }

    // 非タッチストロークにタッチ対象指定があればポイントの登録
    if( !s_bTouch && IS_STROKE_TOUCH_TARGET_FIXED_VALID( pAP->touchTargetIdFixed ) ){
        int idFixed = pAP->touchTargetIdFixed;
        
        // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
        if( (s_nArrTouchPointNum[idFixed]+point) > BEZIER_TOUCH_POINT_MAX ){
            LOGE( "@ CBezier::PutDotLinear: TOUCH POINT BUF SHORTAGE: use[%d]=(%d+%d)/%d\n",
                  idFixed, s_nArrTouchPointNum[idFixed], point, BEZIER_TOUCH_POINT_MAX );
            return;
        }

        // タッチバッファへ出力（傾き適用前後の両方の値）
        memcpy( &s_fArrArrTouchPointX[idFixed][s_nArrTouchPointNum[idFixed]], s_fArrTempPointX, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointY[idFixed][s_nArrTouchPointNum[idFixed]], s_fArrTempPointY, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointX0[idFixed][s_nArrTouchPointNum[idFixed]], s_fArrTempPointX0, point*sizeof(float) );
        memcpy( &s_fArrArrTouchPointY0[idFixed][s_nArrTouchPointNum[idFixed]], s_fArrTempPointY0, point*sizeof(float) );
        s_nArrTouchPointNum[idFixed] += point;
        
#ifdef BEZIER_USE_NUM_CHECK
        if( s_nArrTouchPointNum[idFixed] > s_nUseTouchPointMax ){
            s_nUseTouchPointMax = s_nArrTouchPointNum[idFixed];
        }
#endif
    }
    
    // タッチでなければガイド出力（※ストライプ時は許可する＝ストローク後の領域を塗りつぶせるように）
    if( (!s_bTouch || s_bStripe) && !pAP->bNoFillGuide ){
        // ガイド対象が有効であれば
        if( IS_STROKE_GUIDE_TARGET_FIXED_VALID( pAP->guideTargetIdFixed ) ){
            int target = pAP->guideTargetIdFixed;

            // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
            if( (s_nArrGuidePointNum[target]+point) > BEZIER_GUIDE_POINT_MAX ){
                LOGE( "@ CBezier::PutDotLinear: GUIDE POINT BUF SHORTAGE: use[%d]=(%d+%d)/%d\n",
                      target, s_nArrGuidePointNum[target], point, BEZIER_GUIDE_POINT_MAX );
                return;
            }

            // ガイドバッファへ出力（傾き適用後の値のみ）
            memcpy( &s_fArrArrGuidePointX[target][s_nArrGuidePointNum[target]], s_fArrTempPointX, point*sizeof(float) );
            memcpy( &s_fArrArrGuidePointY[target][s_nArrGuidePointNum[target]], s_fArrTempPointY, point*sizeof(float) );
            s_nArrGuidePointNum[target] += point;
            
#ifdef BEZIER_USE_NUM_CHECK
            if( s_nArrGuidePointNum[target] > s_nUseGuidePointMax ){
                s_nUseGuidePointMax = s_nArrGuidePointNum[target];
            }
#endif
        }
        // ガイド対象が無効であれば
        else{
            // 塗りガイドに下塗り色を出力
            for( int i=0; i<point; i++ ){
                int at = s_nBufW*((int)s_fArrTempPointY[i]) + ((int)s_fArrTempPointX[i]);
                SET_BFG_UNDERCOAT_COL( s_pBufFillGuide[at] );
            }
        }
    }

    // ストライプ時はガイド値を保持
    if( s_bStripe ){
        // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
        if( (s_nNumStripeGuidePoint+point) > BEZIER_STRIPE_GUIDE_POINT_MAX ){
            LOGE( "@ CBezier::PutDotLinear: STRIPE GUIDE POINT BUF SHORTAGE: use=(%d+%d)/%d\n",
                  s_nNumStripeGuidePoint, point, BEZIER_STRIPE_GUIDE_POINT_MAX );
            return;
        }

        // ストライプガイドバッファへ出力（傾き適用後の値のみ）
        memcpy( &s_fArrStripeGuidePointX[s_nNumStripeGuidePoint], s_fArrTempPointX, point*sizeof(float) );
        memcpy( &s_fArrStripeGuidePointY[s_nNumStripeGuidePoint], s_fArrTempPointY, point*sizeof(float) );
        s_nNumStripeGuidePoint += point;

#ifdef BEZIER_USE_NUM_CHECK
        if( s_nNumStripeGuidePoint > s_nUseStripeGuidePointMax ){
            s_nUseStripeGuidePointMax = s_nNumStripeGuidePoint;
        }
#endif
    }

#if 0
    // TODO:確認＆別のアプローチを考える
    // ストロークが有効なら二重線を引いてみる
    if( pStroke != NULL ){
        stBEZIER_ANCHOR_POINT ap0 = *pAP;

        /*
        // クリアなブレ線（※一番まともそうか？）
        pAP->size = GM_P_RATE;
        pAP->ofsRange = 0.0f;
        pAP->dotPutMax = 255;
        pAP->dotPutRateBase = 100;
        pAP->dotPutRateStrong = 100;
        pAP->dotPutRateWeak = 100;
        pAP->shakeScaleForPos = 1.4f;
        pAP->shakeScaleForSize = 0.0f;
        */

        /*
        // 雑味のある線
        pAP->size = GM_P_RATE;
        pAP->ofsRange = 0.0f;
        pAP->dotPutMax = 240;
        pAP->dotPutRateBase = 80;
        pAP->dotPutRateStrong = 90;
        pAP->dotPutRateWeak = 30;
        pAP->shakeScaleForPos = 1.8f;
        pAP->shakeScaleForSize = 0.0f;
        */
        
        /*
        // 雑味のある線
        pAP->size = 1.2f*GM_P_RATE;
        pAP->ofsRange = 0.4f*GM_P_RATE;
        pAP->dotPutMax = 240;
        pAP->dotPutRateBase = 90;
        pAP->dotPutRateStrong = 80;
        pAP->dotPutRateWeak = 25;
        pAP->shakeScaleForPos = 1.8f;
        pAP->shakeScaleForSize = 0.4f;
        */
        
        /*
        // かなりブレた線
        pAP->size *= 1.2f;
        pAP->ofsRange = 0.0f;
        pAP->dotPutMax = (8*pAP->dotPutMax)/10;
        pAP->dotPutRateBase = (8*pAP->dotPutRateBase)/10;
        pAP->dotPutRateStrong = (8*pAP->dotPutRateStrong)/10;
        pAP->dotPutRateWeak = (8*pAP->dotPutRateWeak)/10;
        pAP->shakeScaleForPos += 1.6f;
        pAP->shakeScaleForSize += 0.4f;
        */

        /*
        // 雑味のある線
        pAP->size *= 0.8f;
        pAP->ofsRange += 0.4f;
        pAP->dotPutMax = (8*pAP->dotPutMax)/10;
        pAP->dotPutRateBase = (8*pAP->dotPutRateBase)/10;
        pAP->dotPutRateStrong = (8*pAP->dotPutRateStrong)/10;
        pAP->dotPutRateWeak = (8*pAP->dotPutRateWeak)/10;
        pAP->shakeScaleForPos += 2.0f;
        pAP->shakeScaleForSize += 1.0f;
        */

        /*
        // 雑味のある線
        pAP->size *= 0.8f;
        pAP->ofsRange += 0.2f*GM_P_RATE;
        pAP->dotPutMax = (8*pAP->dotPutMax)/10;
        pAP->dotPutRateBase = (8*pAP->dotPutRateBase)/10;
        pAP->dotPutRateStrong = (8*pAP->dotPutRateStrong)/10;
        pAP->dotPutRateWeak = (8*pAP->dotPutRateWeak)/10;
        pAP->shakeScaleForPos += 1.8f;
        pAP->shakeScaleForSize += 0.5f;
        */

        pStroke = GetStrokeWithAnchorPoint( pAP );
        for( int i=0; i<point; i++ ){
            if( s_fArrTempStrokeSize[i] > 0.0f ){
                PutDotAt( s_fArrTempPointX[i], s_fArrTempPointY[i], s_fArrTempStrokeSize[i], s_nTestPalGrp, pStroke, pAP );
            }
        }
        
        *pAP = ap0;
    }
#endif
}

//---------------------------------------
// ２点間のポイント等を算出して一時バッファに格納
//---------------------------------------
int CBezier::CalcThenSetPointForTempBuf( stBEZIER_DIVISION_POINT* pDP0, stBEZIER_DIVISION_POINT* pDP1, stBEZIER_ANCHOR_POINT* pAP ){
    // ポイント数の算出
    int point = CalcPointNum( pDP0->x, pDP0->y, pDP1->x, pDP1->y );
    
    // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
    if( point >= BEZIER_TEMP_POINT_MAX ){
        LOGE( "@ CBezier::CalcThenSetPointForTempBuf: TEMP POINT BUF SHORTAGE: use=%d/%d\n", point, BEZIER_TEMP_POINT_MAX);
        return( -1 );
    }
    
    //-----------------------------------------------------------------------------------------------------------
    // 出力に必要な座標とストロークサイズの算出
    //（※終了判定が[i<div]なので[t==1.0f]の値である[x1,y1]は描画されないが、これは次のポイントの始点[x0,y0]と重複しないための措置）
    //-----------------------------------------------------------------------------------------------------------
    for( int i=0; i<point; i++ ){
        float t = ((float)i)/point;
        float tR = 1.0f - t;
        
        s_fArrTempPointX[i] = pDP0->x*tR + pDP1->x*t;
        s_fArrTempPointY[i] = pDP0->y*tR + pDP1->y*t;
        s_fArrTempPointX0[i] = pDP0->x0*tR + pDP1->x0*t;
        s_fArrTempPointY0[i] = pDP0->y0*tR + pDP1->y0*t;
        s_fArrTempStrokeSize[i] = pDP0->strokeSize*tR + pDP1->strokeSize*t;
    }
    
    // 透明ストローク例外（※次のＡＰが透明であれば終点も登録しておく＝これがないと１ドット欠けた印象になってしまう）
    if( pAP->pNext != NULL ){
        stBEZIER_ANCHOR_POINT* pNextAP = pAP->pNext;
        if( pNextAP->type == eSTROKE_TYPE_OFF ){
            s_fArrTempPointX[point] = pDP1->x;
            s_fArrTempPointY[point] = pDP1->y;
            s_fArrTempPointX0[point] = pDP1->x0;
            s_fArrTempPointY0[point] = pDP1->y0;
            s_fArrTempStrokeSize[point] = pDP1->strokeSize;
            point++;
        }
    }
    
#ifdef BEZIER_USE_NUM_CHECK
    if( point > s_nUseTempPointMax ){
        s_nUseTempPointMax = point;
    }
#endif

    return( point );
}

//-----------------------------------
// ２点間のポイント数の算出
//-----------------------------------
int CBezier::CalcPointNum( float x0, float y0, float x1, float y1 ){
    // 二点間のドット数の算出（※整数で判定）
    int distX = ((int)x1) - ((int)x0);
    int distY = ((int)y1) - ((int)y0);
    if( distX < 0 ){ distX = -distX; }
    if( distY < 0 ){ distY = -distY; }
    int num = ((distX>distY)? distX: distY);
    
    // ０以下は１点と考える（※同じ座標が指定された＝オープンパスの最後の点等を想定）
    if( num <= 0 ){
        num = 1;
    }

    return( num );
}

//-------------------
// ドットを置く
//-------------------
void CBezier::PutDotAt( float x0, float y0, float strokeSize, BYTE testPalGrp, CStroke* pStroke, stBEZIER_ANCHOR_POINT* pAP ){
    // ストローク制限指定があれば
    if( s_nSkipStrokePointMax > 0 ){
        s_nSkipStrokePointCur++;

        // 先頭の制限を抜け出せなければ無視
        if( s_nSkipStrokePointCur <= s_nSkipStrokePointLimitForHead ){
            return;
        }
        
        // 末尾の制限に達したら無視
        if( s_nSkipStrokePointCur > s_nSkipStrokePointLimitForTail ){
            return;
        }
    }

#if 0
    // [pStroke]の[NULL]チェックは呼び出し側で行なっている「はず」なので保護コードはオフ
    if( pStroke == NULL ){
        LOGE( "@ CBezier::PutDotAt: INVALID STROKE\n" );
        return;
    }
#endif

    // ブラシのブレを反映
    UpdateBrushShake();
    x0 += pAP->shakeScaleForPos*GetBrushShakeForPosX();
    y0 += pAP->shakeScaleForPos*GetBrushShakeForPosY();
    strokeSize *= 1.0f + pAP->shakeScaleForSize*GetBrushShakeForSize();

    // ドットサイズ調整（※ストロークによるサイズ補正を反映）
    float size0 = pStroke->getSize();
    size0 *= strokeSize;

    // 座標調整（※ドットの中央に描画されるように）
    int size = (int)size0;
    float x = x0 - size/2.0f;
    float y = y0 - size/2.0f;

    //-----------------------------
    // 座標のはみ出し具合の算出
    //-----------------------------
    int iX0 = (int)x;
    int iY0 = (int)y;
    float rateOutX = x - iX0;   // 小数点以下がＸ方向のはみ出し
    float rateOutY = y - iY0;   // 小数点以下がＹ方向のはみ出し
    
    //--------------------------------------------------------------------------------------------------
    // ストローク画像取得＆描画（※はみ出し具合によるテクスチャが[pCell]に設定されて返る）
    // [pDot]で指定される画素の値域は[0〜STROKE_DOT_OUT_PRECISION_MAX]で値が大きいほど濃い（※MAXで不透明／０で透明）
    //--------------------------------------------------------------------------------------------------
    stSTROKE_OUTPUT_CELL* pCell = pStroke->getStrokeCell( rateOutX, rateOutY, strokeSize );
    BYTE* pDot = pCell->pBuf;
    int w = pCell->sizeW;
    int h = pCell->sizeH;
    int iX = iX0 + pCell->ofsX; // １ピクセルを超えるオフセットの適用
    int iY = iY0 + pCell->ofsY; // １ピクセルを超えるオフセットの適用

    //============================================================================
    // 消去ストローク（※減算）
    //============================================================================
    if( pAP->isDotErase ){
        // 画素の出力
        for( int i=0; i<h; i++ ){
            for( int j=0; j<w; j++ ){
                int dotAt = w*i + j;
                int bufAt = s_nBufW*(iY+i) + (iX+j);
                
                //-----------------------------------------------------------------------
                // ストローク値が有効であれば ＆ 消去に意味があれば（※アルファがなければ消去の意味はない）
                //-----------------------------------------------------------------------
                if( pDot[dotAt] > 0x00 && s_pBuf[bufAt] > 0x00 && (s_pBufFillGuard[bufAt]&s_pLayerParam->bitMask) == 0 ){
                    bool checkResult;
                    if( pAP->isDotPutForce ){
                        checkResult = true;
                    }else if( pAP->isDotPutOnOutCol ){
                        // 出力色の上なら書き込む
                        checkResult = IS_BFG_OUT_COL( s_pBufFillGuide[bufAt] );
                    }else{
                        // パレットテストに通れば書き込む
                        checkResult = (BP_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt]) == testPalGrp);
                    }

                    //------------------
                    // テストに通過したら
                    //------------------
                    if( checkResult ){
                        //---------------------------------------------
                        // 出力値の算出：[dotPutRateBase]で出力の強さの調整
                        //---------------------------------------------
                        int val = (0xFF*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
                        val = (val*pAP->dotPutRateBase)/100;

                        //---------------------------------------------
                        // 色味の減算（※ここは単純な引き算）
                        //---------------------------------------------
                        if( val >= s_pBuf[bufAt] ){
                            s_pBuf[bufAt] = 0;
                        }else{
                            s_pBuf[bufAt] -= val;
                        }
                        
                        // ストロークバッファの利用状況更新は不要（※画素が増えていないので）
                    }
                }
            }
        }
    }
#if 1
    //============================================================================
    // 固定ストローク（※加算）
    //============================================================================
    // 基本的な設定の場合は処理を浮かせられるのでここで分岐させる
    else if( !pAP->isDotPutWhetherMax && !pAP->isDotPutForce &&
             pAP->dotPutRateBase >= 100 && pAP->dotPutRateStrong >= 100 && pAP->dotPutRateWeak >= 100 ){
        // さらに分岐：出力領域への書き込み
        if( pAP->isDotPutOnOutCol ){
            // 画素の出力
            for( int i=0; i<h; i++ ){
                for( int j=0; j<w; j++ ){
                    int dotAt = w*i + j;
                    int bufAt = s_nBufW*(iY+i) + (iX+j);
                    
                    // ストローク値が有効であれば ＆ 出力に意味があれば ＆ ガードされていなければ
                    if( pDot[dotAt] > 0x00 && s_pBuf[bufAt] < pAP->dotPutMax && (s_pBufFillGuard[bufAt]&s_pLayerParam->bitMask) == 0 ){
                        // 出力色の上なら書き込む
                        if( IS_BFG_OUT_COL( s_pBufFillGuide[bufAt] ) ){
                            // 出力値の算出
                            int val = s_pBuf[bufAt] + (0xFF*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;

                            // 線バッファへ出力
                            if( val >= pAP->dotPutMax ){
                                s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
                            }else{
                                s_pBuf[bufAt] = (BYTE)val;
                            }
                        }
                    }
                }
            }
        }
        // 未出力領域への書き込み
        else{
            // 画素の出力
            for( int i=0; i<h; i++ ){
                for( int j=0; j<w; j++ ){
                    int dotAt = w*i + j;
                    int bufAt = s_nBufW*(iY+i) + (iX+j);
                    
                    // ストローク値が有効であれば ＆ 出力に意味があれば ＆ ガードされていなければ
                    if( pDot[dotAt] > 0x00 && s_pBuf[bufAt] < pAP->dotPutMax && (s_pBufFillGuard[bufAt]&s_pLayerParam->bitMask) == 0 ){
                        // パレットテストに通れば出力
                        if( BP_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt]) == testPalGrp ){
                            // 出力値の算出
                            int val = s_pBuf[bufAt] + (0xFF*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;

                            // 線バッファへ出力
                            if( val >= pAP->dotPutMax ){
                                s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
                            }else{
                                s_pBuf[bufAt] = (BYTE)val;
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    //============================================================================
    // 変動ストローク（※加算）
    //============================================================================
    // ここまできたら変動するパラメータの影響をうける
    else{
        // 画素の出力
        for( int i=0; i<h; i++ ){
            for( int j=0; j<w; j++ ){
                int dotAt = w*i + j;
                int bufAt = s_nBufW*(iY+i) + (iX+j);
                
                //------------------------------------------------------------------------------------------------------
                // ストローク値が有効であれば ＆ 出力に意味があれば（※基本的に不透明になった時点で画素は確定）
                //（※[isDotPutWhetherMax]の指定によりすでに出力された濃い色を薄めることができる＝黒地に黒を引いた時の境界を浮き出させる等）
                //------------------------------------------------------------------------------------------------------
                if( pDot[dotAt] > 0x00 && (pAP->isDotPutWhetherMax || s_pBuf[bufAt] < pAP->dotPutMax) &&
                    (s_pBufFillGuard[bufAt]&s_pLayerParam->bitMask) == 0 ){
                    bool checkResult;
                    if( pAP->isDotPutForce ){
                        checkResult = true;
                    }else if( pAP->isDotPutOnOutCol ){
                        // 出力色の上なら書き込む
                        checkResult = IS_BFG_OUT_COL( s_pBufFillGuide[bufAt] );
                    }else{
                        // パレットテストに通れば書き込む
                        checkResult = (BP_CONV_DOT_TO_PAL_GRP(s_pBufTest[bufAt]) == testPalGrp);
                    }

                    //------------------
                    // テストに通過したら
                    //------------------
                    if( checkResult ){
                        //---------------------------------------------
                        // 出力値の算出：[dotPutRateBase]で出力の強さの調整
                        //---------------------------------------------
                        int val = (0xFF*pDot[dotAt]) / STROKE_DOT_OUT_PRECISION_MAX;
                        val = (val*pAP->dotPutRateBase)/100;

                        //--------------------------------------------------------------------------------------
                        // 色味の加算（※濃い方に薄い方を足すイメージ）：[dotPutRateStrong/dotPutRateWeak]で重ね合わせの調整
                        //--------------------------------------------------------------------------------------
                        int temp;
                        if( val > s_pBuf[bufAt] ){
                            temp = (val*pAP->dotPutRateStrong + s_pBuf[bufAt]*pAP->dotPutRateWeak)/100;
                        }else{
                            temp = (s_pBuf[bufAt]*pAP->dotPutRateStrong + val*pAP->dotPutRateWeak)/100;
                        }

                        // 線バッファへ出力
                        if( temp >= pAP->dotPutMax ){
                            s_pBuf[bufAt] = (BYTE)pAP->dotPutMax;
                        }else{
                            s_pBuf[bufAt] = (BYTE)temp;
                        }
                    }
                }
            }
        }
    }
    
    // ベジェに対して新たに線が描かれたであろう領域を通知（※厳密ではないが全ての出力で判定はしたくないので）
    CBmpGenerator::UpdateLineBufInfo( iX, iY );        // 左上
    CBmpGenerator::UpdateLineBufInfo( iX+w, iY+h );    // 右下
}

//--------------------------------------------------------------------------------------
// 分割点の登録：指定の２ＡＰ間（※[pP1]は追加されない点に注意＝次回の始点になるので重複描画されないように）
//--------------------------------------------------------------------------------------
void CBezier::RegistDivisionPointList( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 ){
    // 始点の登録（※開始点なのでエッジ埋め指定あり）
    float strokeSize = pP0->strokeSize * pP0->strokeStartSize;
    AddDivisionPoint( pP0->x, pP0->y, strokeSize, true, pP0 );

    //--------------------------------------------
    // 分割点の登録（※直線であっても諸々の都合上分割する）
    //--------------------------------------------
    // 分割数の算出
    int divNum = CalcDivisionNumForSegment( pP0, pP1 );

    // 分割点の登録（※ここでは始点[i==0]と終点[i==divNum]は追加されない）
    for( int i=1; i<divNum; i++ ){
        // t, (1-t)
        float t1 = ((float)i)/divNum;
        float tR1 = 1.0f - t1;

        // t^2, (1-t)^2
        float t2 = t1 * t1;
        float tR2 = tR1 * tR1;
        
        // t^3, (1-t)^3
        float t3 = t2 * t1;
        float tR3 = tR2 * tR1;
        
        // (1-t)^2*t, (1-t)*t^2
        float tR2_t1 = tR2 * t1;
        float tR1_t2 = tR1 * t2;

        // 分割点座標の算出（※ベジェ曲線）
        float x = pP0->x*tR3 + 3*(pP0->x+pP0->xOut)*tR2_t1 + 3*(pP1->x+pP1->xIn)*tR1_t2 + pP1->x*t3;
        float y = pP0->y*tR3 + 3*(pP0->y+pP0->yOut)*tR2_t1 + 3*(pP1->y+pP1->yIn)*tR1_t2 + pP1->y*t3;
        
        // ストロークサイズ
        strokeSize = pP0->strokeSize;
        
        // レンジ指定が共に０なら
        if( pP0->strokeStartRange <= 0 && pP0->strokeEndRange <= 0 ){
            strokeSize *= (1.0f-t1)*pP0->strokeStartSize + t1*pP0->strokeEndSize;
        }
        // 開始レンジが有効なら
        else if( t1 < pP0->strokeStartRange ){
            // 開始サイズから徐々に基本サイズへ
            float range = t1 / pP0->strokeStartRange;
            strokeSize *= (1.0f-range)*pP0->strokeStartSize + range;
        }
        // 終了レンジが有効なら
        else if( tR1 < pP0->strokeEndRange ){
            // 基本サイズから徐々に終了サイズへ
            float range = tR1 / pP0->strokeEndRange;
            strokeSize *= (1.0f-range)*pP0->strokeEndSize + range;
        }
        
        // 分割点リストに登録（※各分割点はエッジ埋め指定は不要）
        AddDivisionPoint( x, y, strokeSize, false, pP0 );
    }
    
    // 終点は登録をせずに終了（※次のアンカーポイントの始点と重複するため）
}

//------------------------------------------------------------
// 分割数取得（※曲線を何個の直線に分割するかの判定）
// 各点間のX、Y距離の大きい要素の合計から分割数を決定
// メモ：浮動小数の割り算の分母になる値なので返値が偶数になるようにしている
//------------------------------------------------------------
#define ADD_MAX_DIST_TO_SUM( _dX, _dY ) \
    distX = (_dX);                      \
    distY = (_dY);                      \
    if( distX < 0 ){ distX = -distX; }  \
    if( distY < 0 ){ distY = -distY; }  \
    sum += (distX>distY)? distX: distY
int CBezier::CalcDivisionNumForSegment( stBEZIER_ANCHOR_POINT* pP0, stBEZIER_ANCHOR_POINT* pP1 ){
    int sum = 0;
    int distX, distY;
    
    // ポイント間（直線距離）
    ADD_MAX_DIST_TO_SUM( pP1->x-pP0->x, pP1->y-pP0->y );
    
    // 方向線：出発
    ADD_MAX_DIST_TO_SUM( pP0->xOut, pP0->yOut );
    
    // 方向線：進入
    ADD_MAX_DIST_TO_SUM( pP1->xIn, pP1->yIn );

    // 分割数判定（※描画品質と処理負荷の兼ね合いで調整）
    if( sum < 16 ){
        return( (sum+1)/2 );
    }
    if( sum < 256 ){
        return( 16 );
    }
    if( sum < 1024 ){
        return( 32 );
    }
    
    return( 64 );
}

//---------------------------
// 分割点を追加
//---------------------------
void CBezier::AddDivisionPoint( float x, float y, float strokeSize, bool isEdgeFillCheck, stBEZIER_ANCHOR_POINT* pAP ){
    // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
    if( s_nUseDivisionPoint >= BEZIER_DIVISION_POINT_MAX ){
        LOGE( "@ CBezier::AddDivisionPoint: DIV POINT BUF SHORTAGE: use=%d/%d\n", s_nUseDivisionPoint, BEZIER_DIVISION_POINT_MAX);
        return;
    }
    
    // 登録枠の確保
    stBEZIER_DIVISION_POINT *pDP = &s_stArrDivisionPoint[s_nUseDivisionPoint];

    // 傾き前の座標（※計算で利用する）
    pDP->x0 = x;
    pDP->y0 = y;

    //-----------------------------------------------
    // アングルの指定が有効であれば変換（※分割点の座標を傾ける）
    //-----------------------------------------------
    if( s_pLayerParam->fixedAngleRateLR != 0.0f || s_pLayerParam->fixedAngleRateUD != 0.0f ){
        s_pBaseParam->pAnglePlane->calcXY( &x, &y, x, y,
                                           pAP->angleSearchKey, s_pLayerParam->fixedAngleRateLR, s_pLayerParam->fixedAngleRateUD );
    }

    // 傾き後の座標（※実描画座標）
    pDP->x = x;
    pDP->y = y;

    pDP->strokeSize = strokeSize;

    // エッジ塗り確認要求の設定（※自身と一つ手前のストロークが共に有効である必要がある）
    pDP->isEdgeFillCheck = false;
    if( isEdgeFillCheck && pAP->strokeEdgeFillSize > 0.0f ){
        if( GetStrokeWithAnchorPoint( pAP ) != NULL && GetStrokeWithAnchorPoint( pAP->pPrev ) != NULL ){
            pDP->isEdgeFillCheck = true;
        }
    }

    pDP->pAP = pAP;

    // 登録枠数更新
    s_nUseDivisionPoint++;
    
#ifdef BEZIER_USE_NUM_CHECK
    if( s_nUseDivisionPoint > s_nUseDivisionPointMax ){
        s_nUseDivisionPointMax = s_nUseDivisionPoint;
    }
#endif
}

//---------------------------
// タッチポイントの反転
//---------------------------
void CBezier::ReverseTouchPoint( int idFixed, int atFrom, int atTo ){
    // 範囲が有効であれば
    if( atTo > atFrom ){
        float temp;
        float* arrX = s_fArrArrTouchPointX[idFixed];
        float* arrY = s_fArrArrTouchPointY[idFixed];
        float* arrX0 = s_fArrArrTouchPointX0[idFixed];
        float* arrY0 = s_fArrArrTouchPointY0[idFixed];

        int numSwap = ((atTo+1)-atFrom)/2;
        while( numSwap > 0 ){
            // Ｘ座標スワップ
            temp = arrX[atTo];
            arrX[atTo] = arrX[atFrom];
            arrX[atFrom] = temp;
            
            // Ｙ座標スワップ
            temp = arrY[atTo];
            arrY[atTo] = arrY[atFrom];
            arrY[atFrom] = temp;

            // Ｘ座標（傾き前）スワップ
            temp = arrX0[atTo];
            arrX0[atTo] = arrX0[atFrom];
            arrX0[atFrom] = temp;
            
            // Ｙ座標（傾き前）スワップ
            temp = arrY0[atTo];
            arrY0[atTo] = arrY0[atFrom];
            arrY0[atFrom] = temp;

            // 値の更新
            numSwap--;
            atFrom++;
            atTo--;
        }
    }
}

//---------------------------
// フックの登録／設定
//---------------------------
void CBezier::ResolveHookPoint( float* pX, float* pY,
                                int systemHookIdFixed, int idFixed, bool isOfsValid, float ofsX, float ofsY, int reHookIdFixed ){
    // 用心
    if( pX == NULL || pY == NULL ){
        LOGE( "@ CBezier::ResolveHookPoint: INVALID: pX=NULL or pY=NULL\n" );
        return;
    }
    
    //--------------------------------------------------------------------------
    // システムフック(JOINTの参照)が有効であれば（※フック枠の末尾から結合ポイント分確保される）
    //--------------------------------------------------------------------------
    bool isSystemHooked = (systemHookIdFixed == BEZIER_HOOK_POINT_ALREADY_HOOKED);
    if( IS_JOINT_POINT_FIXED_VALID( systemHookIdFixed - eSTROKE_HOOK_TARGET_TOTAL_MAX ) ){
        // 登録済みであれば参照（※２つ目以降のアンカーポイントが初出の座標を参照する）
        if( s_bArrHookPoint[systemHookIdFixed] ){
            *pX = s_fArrHookPointX[systemHookIdFixed];
            *pY = s_fArrHookPointY[systemHookIdFixed];
        }
        // 未登録であれば登録（※初出のアンカーポイントの座標が登録される）
        else{
            s_fArrHookPointX[systemHookIdFixed] = *pX;
            s_fArrHookPointY[systemHookIdFixed] = *pY;
            s_bArrHookPoint[systemHookIdFixed] = true;
        }
        
        // システムフック処理済み
        isSystemHooked = true;
    }
    
    //--------------------------------------------------------------------------
    // フック指定が有効であれば（※システムフックと併用された場合登録のみが有効になる）
    //--------------------------------------------------------------------------
    bool isHooked = (idFixed == BEZIER_HOOK_POINT_ALREADY_HOOKED);
    if( IS_STROKE_HOOK_TARGET_FIXED_VALID( idFixed ) ){
        // 登録済みであれば（※２つ目以降のアンカーポイントが初出の座標を参照する）
        if( s_bArrHookPoint[idFixed] ){
            // システムフックが未処理であればデータ側を参照（※システムフックが有効であればそちらを優先）
            if( ! isSystemHooked ){
                // 座標の上書き
                *pX = s_fArrHookPointX[idFixed];
                *pY = s_fArrHookPointY[idFixed];
            }
        }
        // 未登録であれば登録（※初出のアンカーポイントの座標が登録される）
        else{
            s_fArrHookPointX[idFixed] = *pX;
            s_fArrHookPointY[idFixed] = *pY;
            s_bArrHookPoint[idFixed] = true;
        }
        
        // フック処理済み
        isHooked = true;
    }

    //--------------------------------------
    // フックされていたら
    //--------------------------------------
    if( isSystemHooked || isHooked ){
        // 指定があればオフセットの足し込み
        if( isOfsValid ){
            *pX += ofsX;
            *pY += ofsY;
        }

        // 再フック指定があれば（※オフセットを加味したフックをさせたい場合はこちら）
        if( IS_STROKE_HOOK_TARGET_FIXED_VALID( reHookIdFixed ) ){
            // 登録済みでなければ登録
            if( ! s_bArrHookPoint[reHookIdFixed] ){
                s_fArrHookPointX[reHookIdFixed] = *pX;
                s_fArrHookPointY[reHookIdFixed] = *pY;
                s_bArrHookPoint[reHookIdFixed] = true;
            }else{
                LOGE( "@ CBezier::ResolveHookPoint: ALRWADY REGISTRED HOOK POINT: reHookIdFixed=%d\n", reHookIdFixed );
            }
        }
    }
}

//---------------------------
// ストライプ塗りポイントの追加
//---------------------------
void CBezier::AddStripePoint( BYTE palOfsGrp, float x, float y ){
    // 登録枠の確認（※このエラーがでたら登録枠数を増やすこと）
    if( s_nStripePointNum >= BEZIER_STRIPE_POINT_MAX ){
        LOGE( "@ CBezier::AddStripePoint: STRIPE POINT BUF SHORTAGE: use=%d/%d\n", s_nStripePointNum, BEZIER_STRIPE_POINT_MAX );
        return;
    }

    
    s_nArrStripePalGrp[s_nStripePointNum] = palOfsGrp;
    s_fArrStripePointX[s_nStripePointNum] = x;
    s_fArrStripePointY[s_nStripePointNum] = y;
    s_nStripePointNum++;
    
#ifdef BEZIER_USE_NUM_CHECK
    if( s_nStripePointNum > s_nUseStripePointMax ){
        s_nUseStripePointMax = s_nStripePointNum;
    }
#endif
}
