
ARCH :=$(shell arch)
#
# GTK_VERSION can be switched between  '2' and '3'
#
GTK_VERSION := 3

ifeq ($(ARCH),armv7l)
	ARCH_LIB_PATH := arm-linux-gnueabihf
	CFLAGS := -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
	CFLAGS += -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/usr/include -I/opt/vc/include -I/usr/include/freetype2 -I/opt/vc/include/EGL
	LDFLAGS := -L/opt/vc/lib -lbrcmGLESv2 -lbrcmEGL  -lGLESv2 -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lrt -lm -lfreetype -lz
	SRC := \
		osd_rpi/font.c \
		osd_rpi/graphics.c \
		osd_rpi/text_render.c \
		osd_rpi/vgft.c
else
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	NOTES += "Building for $(ARCH) is for development and testing purposes only.\n"
	NOTES += "Afaik, omxplayer only runs on the Raspberry Pi. Therefore\n"
	NOTES += "tomxplayer is only useful on the Raspberry Pi - armv7l.\n"
	NOTES += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	DEFS += -Dx86
	ARCH_LIB_PATH := x86_64-linux-gnu
endif

LDFLAGS += -L/usr/lib -L/usr/lib/$(ARCH_LIB_PATH)

ifeq ($(GTK_VERSION), 3)
	DEFS += -DGTK3
	CFLAGS += -pthread -I/usr/include/gtk-3.0 -I/usr/$(ARCH_LIB_PATH)/gtk-3.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/$(ARCH_LIB_PATH)/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/$(ARCH_LIB_PATH)/dbus-1.0/include	
	LDFLAGS += -pthread -lgtk-3 -lgdk-3 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0
else
	CFLAGS += -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/$(ARCH_LIB_PATH)/glib-2.0/include -I /usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/$(ARCH_LIB_PATH)/gtk-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -I/usr/include/dbus-1.0 -I /usr/lib/$(ARCH_LIB_PATH)/dbus-1.0/include
	LDFLAGS += -lgtk-x11-2.0 -lgdk-x11-2.0 -lXi -lgdk_pixbuf-2.0 -lm -lXft -lXrender -lXext -lX11 -lfreetype -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lpthread
 
endif

LDFLAGS += -ldbus-1

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
	gcc $(CFLAGS) -DRESOURCE_DIR=\"/usr/share/tomxplayer\" $(SRC) -o tomxplayer.bin $(LDFLAGS)
	@echo
	@echo Build Release Complete.
	@echo Building Debian Package...
	@echo
	@./build_deb.sh
	@echo
	@echo "Building Package Complete."
	@echo "Package is in ./out"
	@echo "Release builds will not run from the current directory."

