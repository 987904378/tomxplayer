
ARCH :=$(shell arch)

ifeq ($(ARCH),armv7l)
ARCH_LIB_PATH := arm-linux-gnueabihf
else
ARCH_LIB_PATH := x86_64-linux-gnu
endif


CFLAGS := -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/$(ARCH_LIB_PATH)/glib-2.0/include -I /usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/$(ARCH_LIB_PATH)/gtk-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -I/usr/include/dbus-1.0 -I /usr/lib/$(ARCH_LIB_PATH)/dbus-1.0/include
LDFLAGS :=  -L/usr/lib -L/usr/lib/$(ARCH_LIB_PATH) -lgtk-x11-2.0 -lgdk-x11-2.0 -lXi -lgdk_pixbuf-2.0 -lm -lXft -lXrender -lXext -lX11 -lfreetype -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lpthread -ldbus-1

SRC := \
  main.c \
  op_control.c \
  op_dbus.c \
  time_utils.c \
  settings.c \
  media_playlist.c \
  mimetype.c \
  preference_dialog.c \
  setting_treeview.c \
  setting_list_view.c \
  list.c \
  main_settings.c \

debug:
	@echo building debug
	@gcc $(CFLAGS) -DDEBUG $(SRC) -o tomxplayer $(LDFLAGS)
	@echo build complete.

release:
	@echo building release
	@gcc $(CFLAGS) $(SRC) -o tomxplayer $(LDFLAGS)
	@echo build complete.

