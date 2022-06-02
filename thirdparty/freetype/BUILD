package(default_visibility = ["//visibility:public"])

licenses(["notice"])

FT_DEFINES = [
    "FT2_BUILD_LIBRARY",
    "FT_CONFIG_OPTION_USE_PNG",
    # "FT_CONFIG_OPTION_USE_ZLIB", not needed, ftoption.h sets this by default already
    "FT_CONFIG_OPTION_SYSTEM_ZLIB",
] + select({
    "@//:linux": [
        "HAVE_FCNTL_H",
        "HAVE_UNISTD_H",
    ],
    "//conditions:default": [],
})

FT_HEADERS = glob([
    "src/**/*.h",
    "include/**/*.h",
])

# Base library, roughly corresponding to instructions in meson.build
cc_library(
    name = "base",
    srcs = [
        "src/base/ftadvanc.c",
        "src/base/ftcalc.c",
        "src/base/ftcolor.c",
        "src/base/ftdbgmem.c",
        "src/base/fterrors.c",
        "src/base/ftfntfmt.c",
        "src/base/ftgloadr.c",
        "src/base/fthash.c",
        "src/base/ftlcdfil.c",
        "src/base/ftmac.c",
        "src/base/ftobjs.c",
        "src/base/ftoutln.c",
        "src/base/ftpsprop.c",
        "src/base/ftrfork.c",
        "src/base/ftsnames.c",
        "src/base/ftstream.c",
        "src/base/fttrigon.c",
        "src/base/ftutil.c",
    ] + select({
        "@//:windows": [
            "builds/windows/ftdebug.c",
            "builds/windows/ftsystem.c",
            # TODO: compile and include src/base/ftver.rc
        ],
        "@//:linux": [
            "src/base/ftdebug.c",
            "builds/unix/ftsystem.c",
        ],
    }),
    hdrs = glob([
        "src/base/*.h",
        "include/**/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
)

# Targets corresponding to FONT_MODULES in modules.cfg
cc_library(
    name = "font_modules",
    deps = [
        ":bdf",
        ":cff",
        ":cid",
        ":pcf",
        ":pfr",
        ":sfnt",
        ":truetype",
        ":type1",
        ":type42",
        ":winfonts",
    ],
)

cc_library(
    name = "truetype",
    srcs = [
        "src/truetype/ttdriver.c",
        "src/truetype/ttgload.c",
        "src/truetype/ttgxvar.c",
        "src/truetype/ttinterp.c",
        "src/truetype/ttobjs.c",
        "src/truetype/ttpload.c",
        "src/truetype/ttsubpix.c",
    ],
    hdrs = glob([
        "src/truetype/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":sfnt",
    ],
)

cc_library(
    name = "type1",
    srcs = [
        "src/type1/t1afm.c",
        "src/type1/t1driver.c",
        "src/type1/t1gload.c",
        "src/type1/t1load.c",
        "src/type1/t1objs.c",
        "src/type1/t1parse.c",
    ],
    hdrs = glob([
        "src/type1/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":psaux",
        ":pshinter",
        ":psnames",
    ],
)

cc_library(
    name = "cff",
    srcs = [
        "src/cff/cffcmap.c",
        "src/cff/cffdrivr.c",
        "src/cff/cffgload.c",
        "src/cff/cffload.c",
        "src/cff/cffobjs.c",
        "src/cff/cffparse.c",
    ],
    hdrs = glob([
        "src/cff/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":psaux",
        ":pshinter",
        ":psnames",
        ":sfnt",
    ],
)

cc_library(
    name = "cid",
    srcs = [
        "src/cid/cidgload.c",
        "src/cid/cidload.c",
        "src/cid/cidobjs.c",
        "src/cid/cidparse.c",
        "src/cid/cidriver.c",
    ],
    hdrs = glob([
        "src/cid/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":psaux",
        ":pshinter",
        ":psnames",
    ],
)

cc_library(
    name = "pfr",
    srcs = [
        "src/pfr/pfrcmap.c",
        "src/pfr/pfrdrivr.c",
        "src/pfr/pfrgload.c",
        "src/pfr/pfrload.c",
        "src/pfr/pfrobjs.c",
        "src/pfr/pfrsbit.c",
    ],
    hdrs = glob([
        "src/pfr/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "type42",
    srcs = [
        "src/type42/t42drivr.c",
        "src/type42/t42objs.c",
        "src/type42/t42parse.c",
    ],
    hdrs = glob([
        "src/type42/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":psaux",
        ":truetype",
    ],
)

cc_library(
    name = "winfonts",
    srcs = [
        "src/winfonts/winfnt.c",
    ],
    hdrs = glob([
        "src/winfonts/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "pcf",
    srcs = [
        "src/pcf/pcfdrivr.c",
        "src/pcf/pcfread.c",
        "src/pcf/pcfutil.c",
    ],
    hdrs = glob([
        "src/pcf/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
        ":gzip",
        ":lzw",
    ],
)

cc_library(
    name = "bdf",
    srcs = [
        "src/bdf/bdfdrivr.c",
        "src/bdf/bdflib.c",
    ],
    hdrs = glob([
        "src/bdf/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "sfnt",
    srcs = [
        "src/sfnt/pngshim.c",
        "src/sfnt/sfdriver.c",
        "src/sfnt/sfobjs.c",
        "src/sfnt/sfwoff.c",
        "src/sfnt/ttbdf.c",
        "src/sfnt/ttcmap.c",
        "src/sfnt/ttcolr.c",
        "src/sfnt/ttcpal.c",
        "src/sfnt/ttkern.c",
        "src/sfnt/ttload.c",
        "src/sfnt/ttmtx.c",
        "src/sfnt/ttpost.c",
        "src/sfnt/ttsbit.c",
        "src/sfnt/ttsvg.c",
    ],
    hdrs = glob([
        "src/sfnt/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base_extensions",
        ":psnames",
        "@libpng",
    ],
)

# Targets corresponding to HINTING_MODULES in modules.cfg
cc_library(
    name = "hinting_modules",
    deps = [
        ":autofit",
        ":pshinter",
    ],
)

cc_library(
    name = "autofit",
    srcs = [
        "src/autofit/afblue.c",
        "src/autofit/afcjk.c",
        "src/autofit/afdummy.c",
        "src/autofit/afglobal.c",
        "src/autofit/afhints.c",
        "src/autofit/afindic.c",
        "src/autofit/aflatin.c",
        "src/autofit/afloader.c",
        "src/autofit/afmodule.c",
        "src/autofit/afranges.c",
        "src/autofit/afshaper.c",
    ],
    hdrs = glob([
        "src/autofit/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "pshinter",
    srcs = [
        "src/pshinter/pshalgo.c",
        "src/pshinter/pshglob.c",
        "src/pshinter/pshmod.c",
        "src/pshinter/pshrec.c",
    ],
    hdrs = glob([
        "src/pshinter/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

# Targets corresponding to RASTER_MODULES in modules.cfg
cc_library(
    name = "raster_modules",
    deps = [
        ":raster",
        ":sdf",
        ":smooth",
        ":svg",
    ],
)

cc_library(
    name = "smooth",
    srcs = [
        "src/smooth/ftgrays.c",
        "src/smooth/ftsmooth.c",
    ],
    hdrs = glob([
        "src/smooth/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "raster",
    srcs = [
        "src/raster/ftraster.c",
        "src/raster/ftrend1.c",
    ],
    hdrs = glob([
        "src/raster/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "svg",
    srcs = [
        "src/svg/ftsvg.c",
    ],
    hdrs = glob([
        "src/svg/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

cc_library(
    name = "sdf",
    srcs = [
        "src/sdf/ftbsdf.c",
        "src/sdf/ftsdf.c",
        "src/sdf/ftsdfcommon.c",
        "src/sdf/ftsdfrend.c",
    ],
    hdrs = glob([
        "src/sdf/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
        ":base_extensions",
    ],
)

# Targets corresponding to AUX_MODULES in modules.cfg
cc_library(
    name = "aux_modules",
    deps = [
        # TODO: add bzip2 aux module
        ":cache",
        ":gzip",
        ":lzw",
        ":psaux",
        ":psnames",
    ],
)

cc_library(
    name = "cache",
    srcs = [
        "src/cache/ftcbasic.c",
        "src/cache/ftccache.c",
        "src/cache/ftccmap.c",
        "src/cache/ftcglyph.c",
        "src/cache/ftcimage.c",
        "src/cache/ftcmanag.c",
        "src/cache/ftcmru.c",
        "src/cache/ftcsbits.c",
    ],
    hdrs = glob([
        "src/cache/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
        ":base_extensions",
    ],
)

cc_library(
    name = "gzip",
    srcs = [
        "src/gzip/ftgzip.c",
    ],
    hdrs = glob([
        "src/gzip/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
        "@zlib",
    ],
)

cc_library(
    name = "lzw",
    srcs = [
        "src/lzw/ftlzw.c",
    ],
    hdrs = glob([
        "src/lzw/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    textual_hdrs = ["src/lzw/ftzopen.c"],
    deps = [
        ":base",
    ],
)

# TODO: add bzip2 aux module here, which depends on external bzip2 library

cc_library(
    name = "psaux",
    srcs = [
        "src/psaux/afmparse.c",
        "src/psaux/cffdecode.c",
        "src/psaux/psarrst.c",
        "src/psaux/psauxmod.c",
        "src/psaux/psblues.c",
        "src/psaux/psconv.c",
        "src/psaux/pserror.c",
        "src/psaux/psfont.c",
        "src/psaux/psft.c",
        "src/psaux/pshints.c",
        "src/psaux/psintrp.c",
        "src/psaux/psobjs.c",
        "src/psaux/psread.c",
        "src/psaux/psstack.c",
        "src/psaux/t1cmap.c",
        "src/psaux/t1decode.c",
    ],
    hdrs = glob([
        "src/psaux/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":psnames",
    ],
)

cc_library(
    name = "psnames",
    srcs = [
        "src/psnames/psmodule.c",
    ],
    hdrs = glob([
        "src/psnames/*.h",
    ]),
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
    ],
)

# Single target for all BASE_EXTENSIONS in modules.cfg
cc_library(
    name = "base_extensions",
    srcs = [
        "src/base/ftbbox.c",
        "src/base/ftbdf.c",
        "src/base/ftbitmap.c",
        "src/base/ftcid.c",
        "src/base/ftfstype.c",
        "src/base/ftgasp.c",
        "src/base/ftglyph.c",
        "src/base/ftgxval.c",
        "src/base/ftmm.c",
        "src/base/ftotval.c",
        "src/base/ftpatent.c",
        "src/base/ftpfr.c",
        "src/base/ftstroke.c",
        "src/base/ftsynth.c",
        "src/base/fttype1.c",
        "src/base/ftwinfnt.c",
    ],
    defines = FT_DEFINES,
    includes = ["include"],
    linkstatic = True,
    deps = [
        ":base",
        ":bdf",
        ":pfr",
        ":winfonts",
    ],
)

cc_library(
    name = "freetype",
    srcs = [
        "src/base/ftinit.c",
    ],
    defines = FT_DEFINES,
    linkstatic = True,
    deps = [
        ":aux_modules",
        ":base",
        ":base_extensions",
        ":font_modules",
        ":hinting_modules",
        ":raster_modules",
    ],
)
