# Android.mk

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     :=	AppEnv
LOCAL_C_INCLUDES := $(LOCAL_PATH)/shared
LOCAL_SRC_FILES  :=	native/env_native_android.cpp \
                    native/native_android.cpp \
                    \
				   	native/sys/file/file_io_android.cpp \
                    \
				   	native/touch/TouchInfoCell_android.cpp \
                    \
                    shared/env.cpp \
				   	\
				   	shared/app/AppMgr.cpp \
				   	\
				   	shared/app/bg/BgGrid.cpp \
				   	shared/app/bg/BgMgr.cpp \
				   	\
				   	shared/app/loop/DebugMenuLoop.cpp \
				   	\
				   	shared/app/loop/bezier/BezierTestLoop.cpp \
				   	shared/app/loop/bezier/BezierTestLoopForBase.cpp \
				   	shared/app/loop/bezier/BezierTestLoopForDelay.cpp \
				   	shared/app/loop/bezier/BezierTestLoopForHook.cpp \
				   	shared/app/loop/bezier/BezierTestLoopForLineRepair.cpp \
				   	shared/app/loop/bezier/BezierTestLoopForTouch.cpp \
				   	shared/app/loop/bezier/BrushCheckLoop.cpp \
				   	shared/app/loop/bezier/StrokeCheckLoop.cpp \
				   	\
				   	shared/config/config_for_app.cpp \
   				   	\
				   	shared/const/Const.cpp \
				   	\
				   	shared/def/BrushBinData.cpp \
				   	shared/def/DefConst.cpp \
				   	shared/def/DefTable.cpp \
				   	\
				   	shared/draw/DrawCell.cpp \
				   	shared/draw/DrawCommon.cpp \
				   	shared/draw/DrawConst.cpp \
				   	shared/draw/MainRenderer.cpp \
				   	\
				   	shared/draw/bezier/Bezier.cpp \
				   	shared/draw/bezier/BezierConst.cpp \
				   	shared/draw/bezier/BezierForEdge.cpp \
				   	shared/draw/bezier/BezierForFill.cpp \
				   	shared/draw/bezier/BezierForFrill.cpp \
				   	shared/draw/bezier/BezierForFrillLayerData.cpp \
				   	shared/draw/bezier/BezierForSub.cpp \
				   	shared/draw/bezier/BezierForTouch.cpp \
				   	shared/draw/bezier/BezierForWork.cpp \
				   	\
				   	shared/draw/bezier/adjust/AdjustablePoint.cpp \
				   	shared/draw/bezier/adjust/AdjustCalc.cpp \
				   	shared/draw/bezier/adjust/AnglePlane.cpp \
				   	shared/draw/bezier/adjust/DelayLog.cpp \
				   	shared/draw/bezier/adjust/JointPoint.cpp \
				   	shared/draw/bezier/adjust/SlotAdjuster.cpp \
				   	\
				   	shared/draw/bezier/data/AnchorPointData.cpp \
				   	shared/draw/bezier/data/FillPointData.cpp \
				   	shared/draw/bezier/data/LayerData.cpp \
				   	shared/draw/bezier/data/LayerObject.cpp \
				   	shared/draw/bezier/data/LayerOrderCell.cpp \
				   	shared/draw/bezier/data/LayerOrderList.cpp \
				   	shared/draw/bezier/data/LineObjectData.cpp \
				   	shared/draw/bezier/data/PaintObjectData.cpp \
				   	shared/draw/bezier/data/SlotObjectData.cpp \
				   	shared/draw/bezier/data/SlotPointData.cpp \
				   	\
				   	shared/draw/bezier/fill/Fill.cpp \
				   	shared/draw/bezier/fill/FillConst.cpp \
				   	shared/draw/bezier/fill/FillForExe.cpp \
				   	shared/draw/bezier/fill/FillForUndercoat.cpp \
				   	shared/draw/bezier/fill/FillMgr.cpp \
				   	\
				   	shared/draw/bezier/stroke/Stroke.cpp \
				   	shared/draw/bezier/stroke/StrokeConst.cpp \
				   	shared/draw/bezier/stroke/StrokeDot.cpp \
				   	shared/draw/bezier/stroke/StrokeMgr.cpp \
				   	\
				   	shared/draw/bezier/struct/BezierStructConv.cpp \
				   	\
				   	shared/draw/bmp/BmpGenerator.cpp \
				   	shared/draw/bmp/BmpGeneratorForAngle.cpp \
				   	shared/draw/bmp/BmpGeneratorForOrder.cpp \
				   	shared/draw/bmp/BmpGeneratorForSetting.cpp \
				   	shared/draw/bmp/BmpGenParam.cpp \
				   	shared/draw/bmp/BmpPal.cpp \
				   	shared/draw/bmp/BmpTex.cpp \
				   	\
				   	shared/draw/bmp/dot/BDConst.cpp \
				   	shared/draw/bmp/dot/BmpDotData.cpp \
				   	shared/draw/bmp/dot/BmpDotMgr.cpp \
				   	shared/draw/bmp/dot/BmpDotOrderData.cpp \
				   	shared/draw/bmp/dot/BmpDotPack.cpp \
				   	shared/draw/bmp/dot/BmpDotPartData.cpp \
				   	shared/draw/bmp/dot/BmpDotSettingData.cpp \
				   	shared/draw/bmp/dot/BmpDotSettingSlotData.cpp \
				   	\
				   	shared/draw/bmp/pal/BmpPalData.cpp \
				   	shared/draw/bmp/pal/BmpPalMgr.cpp \
				   	shared/draw/bmp/pal/BPConst.cpp \
				   	\
				   	shared/draw/font/Font.cpp \
				   	shared/draw/font/FontMgr.cpp \
				   	\
				   	shared/draw/frame_buffer/FrameBuffer.cpp \
				   	shared/draw/frame_buffer/FrameBufferMgr.cpp \
				   	\
				   	shared/draw/grp/Grp.cpp \
				   	shared/draw/grp/GrpMgr.cpp \
				   	\
				   	shared/draw/img/Img.cpp \
				   	shared/draw/img/ImgFormat.cpp \
				   	shared/draw/img/ImgMgr.cpp \
				   	\
				   	shared/draw/num/Num.cpp \
				   	shared/draw/num/NumMgr.cpp \
				   	\
				   	shared/draw/shader/Shader.cpp \
				   	shared/draw/shader/ShaderMgr.cpp \
				   	\
				   	shared/draw/spr/Spr.cpp \
				   	shared/draw/spr/SprMgr.cpp \
				   	\
				   	shared/draw/tex/Tex.cpp \
				   	shared/draw/tex/TexCache.cpp \
				   	shared/draw/tex/TexCacheBase.cpp \
				   	shared/draw/tex/TexCacheMgr.cpp \
				   	shared/draw/tex/TexMgr.cpp \
				   	shared/draw/tex/TexString.cpp \
				   	shared/draw/tex/TexStringMgr.cpp \
				   	\
				   	shared/helper/DialogHelper.cpp \
				   	\
				   	shared/loop/GestureLoop.cpp \
				   	shared/loop/Loop.cpp \
				   	shared/loop/LoopMgr.cpp \
				   	shared/loop/LoopReserve.cpp \
				   	shared/loop/LoopTaskReserve.cpp \
				   	\
				   	shared/loop/node/Button.cpp \
				   	shared/loop/node/EditPalDialog.cpp \
				   	shared/loop/node/EditPalMenu.cpp \
				   	shared/loop/node/EditValueDialog.cpp \
				   	shared/loop/node/EditValueMenu.cpp \
				   	shared/loop/node/Menu.cpp \
				   	shared/loop/node/Node.cpp \
				   	shared/loop/node/PullDownMenu.cpp \
				   	shared/loop/node/ScrollBar.cpp \
				   	shared/loop/node/ScrollBarMenu.cpp \
				   	shared/loop/node/StrMenu.cpp \
				   	shared/loop/node/TouchNode.cpp \
				   	shared/loop/node/Win.cpp \
					\
				   	shared/obj/Drawer.cpp \
				   	shared/obj/Fade.cpp \
				   	shared/obj/Mover.cpp \
				   	shared/obj/Obj.cpp \
				   	shared/obj/Spring.cpp \
				   	shared/obj/TouchExObj.cpp \
				   	shared/obj/TouchObj.cpp \
				   	shared/obj/Updater.cpp \
				   	\
				   	shared/sys/data/CsvCell.cpp \
				   	shared/sys/data/DataFile.cpp \
				   	shared/sys/data/DataList.cpp \
				   	shared/sys/data/EditValue.cpp \
				   	shared/sys/data/Enum.cpp \
				   	shared/sys/data/FileHeader.cpp \
				   	shared/sys/data/InputBuffer.cpp \
				   	shared/sys/data/List.cpp \
				   	shared/sys/data/ListNode.cpp \
				   	shared/sys/data/OutputBuffer.cpp \
				   	\
				   	shared/sys/file/FileMgr.cpp \
				   	shared/sys/file/fpack.cpp \
				   	\
				   	shared/sys/log/time_log.cpp \
				   	\
				   	shared/sys/math/Math.cpp \
				   	\
				   	shared/sys/mem/mcell.cpp \
				   	shared/sys/mem/MemMgr.cpp \
				   	shared/sys/mem/new_delete.cpp \
				   	\
				   	shared/sys/rand/Rand.cpp \
				   	\
				   	shared/sys/sdarc/sdarc.cpp \
				   	shared/sys/sdarc/sdarc_pak3.cpp \
				   	shared/sys/sdarc/sdarc_unpak3.cpp \
				   	\
					shared/sys/util/util_encrypt.cpp \
					shared/sys/util/util_endian.cpp \
					shared/sys/util/util_pcm.cpp \
					shared/sys/util/util_rand.cpp \
                    shared/sys/util/util_str.cpp \
                    shared/sys/util/util_sys.cpp \
					shared/sys/util/util_utf8.cpp \
				   	\
				   	shared/touch/TouchMgr.cpp \
				   	shared/touch/TouchStatusCell.cpp \
				   	\
# libs
LOCAL_LDLIBS	:= -llog \
				   -landroid \
				   -lEGL \
				   -lGLESv2 \
				   -lOpenSLES

# static libraries				   
LOCAL_STATIC_LIBRARIES	:= android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

# import module
$(call import-module, android/native_app_glue)
