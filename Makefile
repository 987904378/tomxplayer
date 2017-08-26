
ARCH :=$(shell arch)

ifeq ($(ARCH),armv7l)
	ARCH_LIB_PATH := arm-linux-gnueabihf
	CFLAGS := -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
	CFLAGS += -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/src/hello_pi/libs/ilclient -I/usr/include -I/opt/vc/include -I/usr/include/freetype2 -I/opt/vc/include/EGL
	LDFLAGS := -L/opt/vc/lib -lbrcmGLESv2 -lbrcmEGL  -lGLESv2 -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lrt -lm -L/opt/vc/src/hello_pi/libs/ilclient -lfreetype -lz
	SRC := \
		osd_rpi/font.c \
		osd_rpi/graphics.c \
		osd_rpi/text_render.c \
		osd_rpi/vgft.c
else
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	NOTES += "Building for x86_64 is for development and testing purposes only.\n"
	NOTES += "Afaik, omxplayer only runs on the Raspberry Pi. Therefore\n"
	NOTES += "tomxplayer is only useful on the Raspberry Pi - armv7l.\n"
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	DEFS += -Dx86
	ARCH_LIB_PATH := x86_64-linux-gnu
endif


CFLAGS += -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/$(ARCH_LIB_PATH)/glib-2.0/include -I /usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/$(ARCH_LIB_PATH)/gtk-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -I/usr/include/dbus-1.0 -I /usr/lib/$(ARCH_LIB_PATH)/dbus-1.0/include
LDFLAGS += -L/usr/lib -L/usr/lib/$(ARCH_LIB_PATH) -lgtk-x11-2.0 -lgdk-x11-2.0 -lXi -lgdk_pixbuf-2.0 -lm -lXft -lXrender -lXext -lX11 -lfreetype -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lpthread -ldbus-1

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

debug:
	@echo $(NOTES)
	@echo building debug
	gcc $(CFLAGS) -DDEBUG $(DEFS) $(SRC) -o tomxplayer $(LDFLAGS)
	@echo build debug complete.

release:
	@echo $(NOTES)
	@echo building release
	gcc $(CFLAGS) $(SRC) -o tomxplayer $(LDFLAGS)
	@echo build release complete.

