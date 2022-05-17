package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "x11proto",
    hdrs = [
        "root/usr/include/GL/glxint.h",
        "root/usr/include/GL/glxmd.h",
        "root/usr/include/GL/glxproto.h",
        "root/usr/include/GL/glxtokens.h",
        "root/usr/include/GL/internal/glcore.h",
        "root/usr/include/X11/DECkeysym.h",
        "root/usr/include/X11/HPkeysym.h",
        "root/usr/include/X11/Sunkeysym.h",
        "root/usr/include/X11/X.h",
        "root/usr/include/X11/XF86keysym.h",
        "root/usr/include/X11/XWDFile.h",
        "root/usr/include/X11/Xalloca.h",
        "root/usr/include/X11/Xarch.h",
        "root/usr/include/X11/Xatom.h",
        "root/usr/include/X11/Xdefs.h",
        "root/usr/include/X11/Xfuncproto.h",
        "root/usr/include/X11/Xfuncs.h",
        "root/usr/include/X11/Xmd.h",
        "root/usr/include/X11/Xos.h",
        "root/usr/include/X11/Xos_r.h",
        "root/usr/include/X11/Xosdefs.h",
        "root/usr/include/X11/Xpoll.h",
        "root/usr/include/X11/Xproto.h",
        "root/usr/include/X11/Xprotostr.h",
        "root/usr/include/X11/Xthreads.h",
        "root/usr/include/X11/Xw32defs.h",
        "root/usr/include/X11/Xwindows.h",
        "root/usr/include/X11/Xwinsock.h",
        "root/usr/include/X11/ap_keysym.h",
        "root/usr/include/X11/dri/xf86dri.h",
        "root/usr/include/X11/dri/xf86driproto.h",
        "root/usr/include/X11/dri/xf86dristr.h",
        "root/usr/include/X11/extensions/EVI.h",
        "root/usr/include/X11/extensions/EVIproto.h",
        "root/usr/include/X11/extensions/XI.h",
        "root/usr/include/X11/extensions/XI2.h",
        "root/usr/include/X11/extensions/XI2proto.h",
        "root/usr/include/X11/extensions/XIproto.h",
        "root/usr/include/X11/extensions/XKB.h",
        "root/usr/include/X11/extensions/XKBgeom.h",
        "root/usr/include/X11/extensions/XKBproto.h",
        "root/usr/include/X11/extensions/XKBsrv.h",
        "root/usr/include/X11/extensions/XKBstr.h",
        "root/usr/include/X11/extensions/XResproto.h",
        "root/usr/include/X11/extensions/Xv.h",
        "root/usr/include/X11/extensions/XvMC.h",
        "root/usr/include/X11/extensions/XvMCproto.h",
        "root/usr/include/X11/extensions/Xvproto.h",
        "root/usr/include/X11/extensions/ag.h",
        "root/usr/include/X11/extensions/agproto.h",
        "root/usr/include/X11/extensions/applewmconst.h",
        "root/usr/include/X11/extensions/applewmproto.h",
        "root/usr/include/X11/extensions/bigreqsproto.h",
        "root/usr/include/X11/extensions/bigreqstr.h",
        "root/usr/include/X11/extensions/composite.h",
        "root/usr/include/X11/extensions/compositeproto.h",
        "root/usr/include/X11/extensions/cup.h",
        "root/usr/include/X11/extensions/cupproto.h",
        "root/usr/include/X11/extensions/damageproto.h",
        "root/usr/include/X11/extensions/damagewire.h",
        "root/usr/include/X11/extensions/dbe.h",
        "root/usr/include/X11/extensions/dbeproto.h",
        "root/usr/include/X11/extensions/dmx.h",
        "root/usr/include/X11/extensions/dmxproto.h",
        "root/usr/include/X11/extensions/dpmsconst.h",
        "root/usr/include/X11/extensions/dpmsproto.h",
        "root/usr/include/X11/extensions/dri2proto.h",
        "root/usr/include/X11/extensions/dri2tokens.h",
        "root/usr/include/X11/extensions/dri3proto.h",
        "root/usr/include/X11/extensions/ge.h",
        "root/usr/include/X11/extensions/geproto.h",
        "root/usr/include/X11/extensions/lbx.h",
        "root/usr/include/X11/extensions/lbxproto.h",
        "root/usr/include/X11/extensions/mitmiscconst.h",
        "root/usr/include/X11/extensions/mitmiscproto.h",
        "root/usr/include/X11/extensions/multibufconst.h",
        "root/usr/include/X11/extensions/multibufproto.h",
        "root/usr/include/X11/extensions/panoramiXproto.h",
        "root/usr/include/X11/extensions/presentproto.h",
        "root/usr/include/X11/extensions/presenttokens.h",
        "root/usr/include/X11/extensions/randr.h",
        "root/usr/include/X11/extensions/randrproto.h",
        "root/usr/include/X11/extensions/recordconst.h",
        "root/usr/include/X11/extensions/recordproto.h",
        "root/usr/include/X11/extensions/recordstr.h",
        "root/usr/include/X11/extensions/render.h",
        "root/usr/include/X11/extensions/renderproto.h",
        "root/usr/include/X11/extensions/saver.h",
        "root/usr/include/X11/extensions/saverproto.h",
        "root/usr/include/X11/extensions/secur.h",
        "root/usr/include/X11/extensions/securproto.h",
        "root/usr/include/X11/extensions/shapeconst.h",
        "root/usr/include/X11/extensions/shapeproto.h",
        "root/usr/include/X11/extensions/shapestr.h",
        "root/usr/include/X11/extensions/shm.h",
        "root/usr/include/X11/extensions/shmproto.h",
        "root/usr/include/X11/extensions/shmstr.h",
        "root/usr/include/X11/extensions/syncconst.h",
        "root/usr/include/X11/extensions/syncproto.h",
        "root/usr/include/X11/extensions/syncstr.h",
        "root/usr/include/X11/extensions/vldXvMC.h",
        "root/usr/include/X11/extensions/windowswm.h",
        "root/usr/include/X11/extensions/windowswmstr.h",
        "root/usr/include/X11/extensions/xcmiscproto.h",
        "root/usr/include/X11/extensions/xcmiscstr.h",
        "root/usr/include/X11/extensions/xf86bigfont.h",
        "root/usr/include/X11/extensions/xf86bigfproto.h",
        "root/usr/include/X11/extensions/xf86bigfstr.h",
        "root/usr/include/X11/extensions/xf86dga.h",
        "root/usr/include/X11/extensions/xf86dga1const.h",
        "root/usr/include/X11/extensions/xf86dga1proto.h",
        "root/usr/include/X11/extensions/xf86dga1str.h",
        "root/usr/include/X11/extensions/xf86dgaconst.h",
        "root/usr/include/X11/extensions/xf86dgaproto.h",
        "root/usr/include/X11/extensions/xf86dgastr.h",
        "root/usr/include/X11/extensions/xf86vm.h",
        "root/usr/include/X11/extensions/xf86vmproto.h",
        "root/usr/include/X11/extensions/xf86vmstr.h",
        "root/usr/include/X11/extensions/xfixesproto.h",
        "root/usr/include/X11/extensions/xfixeswire.h",
        "root/usr/include/X11/extensions/xtestconst.h",
        "root/usr/include/X11/extensions/xtestext1const.h",
        "root/usr/include/X11/extensions/xtestext1proto.h",
        "root/usr/include/X11/extensions/xtestproto.h",
        "root/usr/include/X11/fonts/FS.h",
        "root/usr/include/X11/fonts/FSproto.h",
        "root/usr/include/X11/fonts/font.h",
        "root/usr/include/X11/fonts/fontproto.h",
        "root/usr/include/X11/fonts/fontstruct.h",
        "root/usr/include/X11/fonts/fsmasks.h",
        "root/usr/include/X11/keysym.h",
        "root/usr/include/X11/keysymdef.h",
    ],
    includes = ["root/usr/include"],
)

cc_library(
    name = "X11",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libX11.so"],
    hdrs = [
        "root/usr/include/X11/ImUtil.h",
        "root/usr/include/X11/XKBlib.h",
        "root/usr/include/X11/Xcms.h",
        "root/usr/include/X11/Xlib.h",
        "root/usr/include/X11/XlibConf.h",
        "root/usr/include/X11/Xlibint.h",
        "root/usr/include/X11/Xlocale.h",
        "root/usr/include/X11/Xregion.h",
        "root/usr/include/X11/Xresource.h",
        "root/usr/include/X11/Xutil.h",
        "root/usr/include/X11/cursorfont.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":x11proto",
    ],
)

cc_library(
    name = "Xcursor",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXcursor.a"],
    hdrs = ["root/usr/include/X11/Xcursor/Xcursor.h"],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":x11proto",
    ],
)

cc_library(
    name = "Xext",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXext.a"],
    hdrs = [
        "root/usr/include/X11/extensions/MITMisc.h",
        "root/usr/include/X11/extensions/XEVI.h",
        "root/usr/include/X11/extensions/XLbx.h",
        "root/usr/include/X11/extensions/XShm.h",
        "root/usr/include/X11/extensions/Xag.h",
        "root/usr/include/X11/extensions/Xcup.h",
        "root/usr/include/X11/extensions/Xdbe.h",
        "root/usr/include/X11/extensions/Xext.h",
        "root/usr/include/X11/extensions/Xge.h",
        "root/usr/include/X11/extensions/dpms.h",
        "root/usr/include/X11/extensions/extutil.h",
        "root/usr/include/X11/extensions/multibuf.h",
        "root/usr/include/X11/extensions/security.h",
        "root/usr/include/X11/extensions/shape.h",
        "root/usr/include/X11/extensions/sync.h",
        "root/usr/include/X11/extensions/xtestext1.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":x11proto",
    ],
)

cc_library(
    name = "Xinerama",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXinerama.a"],
    hdrs = [
        "root/usr/include/X11/extensions/Xinerama.h",
        "root/usr/include/X11/extensions/panoramiXext.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":Xext",
        ":x11proto",
    ],
)

cc_library(
    name = "xcb",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libxcb.so"],
    hdrs = [
        "root/usr/include/xcb/bigreq.h",
        "root/usr/include/xcb/xc_misc.h",
        "root/usr/include/xcb/xcb.h",
        "root/usr/include/xcb/xcbext.h",
        "root/usr/include/xcb/xproto.h",
    ],
    includes = ["root/usr/include"],
)

cc_library(
    name = "X11-xcb",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libX11-xcb.a"],
    hdrs = [
        "root/usr/include/X11/Xlib-xcb.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":xcb",
    ],
)

cc_library(
    name = "Xau",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXau.a"],
    hdrs = [
        "root/usr/include/X11/Xauth.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":x11proto",
    ],
)

cc_library(
    name = "Xdmcp",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXdmcp.a"],
    hdrs = [
        "root/usr/include/X11/Xdmcp.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":x11proto",
    ],
)

cc_library(
    name = "Xfixes",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXfixes.a"],
    hdrs = [
        "root/usr/include/X11/extensions/Xfixes.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":x11proto",
    ],
)

cc_library(
    name = "Xi",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXi.a"],
    hdrs = [
        "root/usr/include/X11/extensions/XInput.h",
        "root/usr/include/X11/extensions/XInput2.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
        ":Xext",
    ],
)

cc_library(
    name = "Xrender",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXrender.a"],
    hdrs = [
        "root/usr/include/X11/extensions/Xrender.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":X11",
    ],
)

cc_library(
    name = "Xrandr",
    srcs = ["root/usr/lib/x86_64-linux-gnu/libXrandr.a"],
    hdrs = [
        "root/usr/include/X11/extensions/Xrandr.h",
    ],
    includes = ["root/usr/include"],
    deps = [
        ":Xext",
        ":Xrender",
        ":x11proto",
    ],
)

cc_library(
    name = "mesa-common",
    hdrs = [
        "root/usr/include/GL/gl.h",
        "root/usr/include/GL/glcorearb.h",
        "root/usr/include/GL/glext.h",
        "root/usr/include/GL/glx.h",
        "root/usr/include/GL/glxext.h",
        "root/usr/include/GL/internal/dri_interface.h",
        "root/usr/include/KHR/khrplatform.h",
    ],
    includes = ["root/usr/include"],
)

cc_library(
    name = "x11-linux-x86_x64",
    deps = [
        ":X11",
        ":X11-xcb",
        ":Xau",
        ":Xcursor",
        ":Xdmcp",
        ":Xext",
        ":Xfixes",
        ":Xi",
        ":Xinerama",
        ":Xrandr",
        ":Xrender",
        ":mesa-common",
        ":xcb",
    ],
)
