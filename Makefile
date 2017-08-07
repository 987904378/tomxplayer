
ARCH :=$(shell arch)
#
# GTK_VERSION can be switched between  '2' and '3'
#
GTK_VERSION := 3

#OPT_ROOT := /home/meticulus/raspbian_stretch_opt

OSD_CFLAGS := \
	-DSTANDALONE \
	-D__STDC_CONSTANT_MACROS \
	-D__STDC_LIMIT_MACROS -DTARGET_POSIX \
	-D_LINUX \
	-fPIC \
	-DPIC \
	-D_REENTRANT \
	-D_LARGEFILE64_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-U_FORTIFY_SOURCE \
	-Wall \
	-g \
	-DHAVE_LIBOPENMAX=2 \
	-DOMX -DOMX_SKIP64BIT \
	-ftree-vectorize \
	-pipe \
	-DUSE_EXTERNAL_OMX \
	-DHAVE_LIBBCM_HOST \
	-DUSE_EXTERNAL_LIBBCM_HOST \
	-DUSE_VCHIQ_ARM \
	-Wno-psabi \
	-I$(OPT_ROOT)/opt/vc/include/interface/vcos/pthreads \
	-I$(OPT_ROOT)/opt/vc/include/interface/vmcs_host/linux \
	-I$(OPT_ROOT)/opt/vc/include \
	-I$(OPT_ROOT)/opt/vc/include/EGL

OSD_LDPATHS := \
	-L$(OPT_ROOT)/opt/vc/lib

OSD_LDFLAGS := \
	-lbrcmGLESv2 \
	-lbrcmEGL \
	-lGLESv2 \
	-lGLESv2_static \
	-lkhrn_static \
	-lopenmaxil \
	-lbcm_host \
	-lvcos \
	-lvchiq_arm \
	-lrt \
	-lm \
	-lfreetype \
	-lz

OSD_SRC := \
	osd_rpi/font.c \
	osd_rpi/graphics.c \
	osd_rpi/text_render.c \
	osd_rpi/vgft.c

ifeq ($(ARCH),armv7l)
	ARCH_LIB_PATH := arm-linux-gnueabihf
else
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	NOTES += "Building for $(ARCH) is for development and testing purposes only.\n"
	NOTES += "Afaik, omxplayer only runs on the Raspberry Pi. Therefore\n"
	NOTES += "tomxplayer is only useful on the Raspberry Pi - armv7l.\n"
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	DEFS += -DNO_OSD
	OSD_SRC :=
	OSD_LDFLAGS :=
	OSD_LDPATHS :=
	OSD_CFLAGS :=
	ARCH_LIB_PATH := $(ARCH)-linux-gnu
endif

COMMON_CFLAGS := \
	-I/usr/include \
	-I/usr/include/freetype2 \
	-I/usr/include/dbus-1.0 \
	-I/usr/lib/$(ARCH_LIB_PATH)/dbus-1.0/include

COMMON_LDFLAGS := \
	-lfreetype \
	-ldbus-1 \
	-lm \
	-lpthread

COMMON_LDPATHS += \
	-L/usr/lib \
	-L/usr/lib/$(ARCH_LIB_PATH)

GTK3_CFLAGS := \
	-DGTK3 \
	-pthread \
	-I/usr/include/gtk-3.0 \
	-I/usr/$(ARCH_LIB_PATH)/gtk-3.0/include \
	-I/usr/include/pixman-1 \
	-I/usr/include/libpng12

GTK3_LDFLAGS := \
	-pthread \
	-lgtk-3 \
	-lgdk-3 \
	-lgio-2.0 \
	-lpangoft2-1.0 \
	-lpangocairo-1.0 \
	-lcairo \
	-lpango-1.0 \
	-lfontconfig \
	-lgthread-2.0 \
	-lrt \

GTK2_CFLAGS := \
	-I/usr/include/gtk-2.0 \
	-I/usr/$(ARCH_LIB_PATH)/gtk-2.0/include \
	-I/usr/lib/$(ARCH_LIB_PATH)/gtk-2.0/include

GTK2_LDFLAGS := \
	-lgtk-x11-2.0 \
	-lgdk-x11-2.0 \
	-lXi \
	-lXft \
	-lXrender \
	-lX11 \
	-ldl

GTK_COMMON_CFLAGS := \
	-I/usr/include/glib-2.0 \
	-I/usr/lib/$(ARCH_LIB_PATH)/glib-2.0/include \
	-I/usr/include/cairo \
	-I/usr/include/pango-1.0 \
	-I/usr/include/gdk-pixbuf-2.0 \
	-I/usr/include/atk-1.0

GTK_COMMON_LDFLAGS := \
	-latk-1.0 \
	-lgobject-2.0 \
	-lgmodule-2.0 \
	-lglib-2.0 \
	-lgdk_pixbuf-2.0

LDFLAGS := \
	$(OSD_LDPATHS) \
	$(COMMON_LDPATHS) \

ifeq ($(GTK_VERSION), 3)
	CFLAGS := $(OSD_CFLAGS) $(GTK3_CFLAGS) $(GTK_COMMON_CFLAGS) $(COMMON_CFLAGS)
	LDFLAGS += $(OSD_LDFLAGS) $(GTK3_LDFLAGS) $(GTK_COMMON_LDFLAGS) $(COMMON_LDFLAGS)
else
	CFLAGS := $(OSD_CFLAGS) $(GTK2_CFLAGS) $(GTK_COMMON_CFLAGS) $(COMMON_CFLAGS)
	LDFLAGS += $(OSD_LDFLAGS) $(GTK2_LDFLAGS) $(GTK_COMMON_LDFLAGS) $(COMMON_LDFLAGS)
endif

SRC += \
  main.c \
  op_control.c \
  op_dbus.c \
  time_utils.c \
  settings.c \
  media_playlist.c \
  preference_dialog.c \
  setting_treeview.c \
  setting_list_view.c \
  list.c \
  main_settings.c \
  $(OSD_SRC)

debug:
	@echo $(NOTES)
	@echo Building Debug...
	@echo
	gcc $(CFLAGS) -DDEBUG $(DEFS) $(SRC) -o tomxplayer.bin $(LDFLAGS)
	@echo
	@echo Build Debug Complete.
	@echo
	@echo './tomxplayer' to run...

release:
	@echo $(NOTES)
	@echo Building Release...
	@echo
	gcc $(CFLAGS) $(DEFS) -DRESOURCE_DIR=\"/usr/share/tomxplayer\" $(SRC) -o tomxplayer.bin $(LDFLAGS)
	@echo
	@echo Build Release Complete.
	@echo Building Debian Package...
	@echo
	@./build_deb.sh
	@echo
	@echo "Building Package Complete."
	@echo "Package is in ./out"
	@echo "Release builds will not run from the current directory."

