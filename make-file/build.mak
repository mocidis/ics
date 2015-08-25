export PJDIR := /home/ubuntu/workspace/pjproject-2.4
export PJ_DIR := $(PJDIR)

# build.mak.  Generated from build.mak.in by configure.
export MACHINE_NAME := auto
export OS_NAME := auto
export HOST_NAME := unix
export CC_NAME := gcc
export TARGET_NAME := x86_64-unknown-linux-gnu
export CROSS_COMPILE := 
export LINUX_POLL := select 
export SHLIB_SUFFIX := so

export prefix := /usr/local
export exec_prefix := ${prefix}
export includedir := ${prefix}/include
export libdir := ${exec_prefix}/lib

LIB_SUFFIX = $(TARGET_NAME).a

ifeq (,1)
export PJ_SHARED_LIBRARIES := 1
endif

# Determine which party libraries to use
export APP_THIRD_PARTY_EXT :=
export APP_THIRD_PARTY_LIBS :=
export APP_THIRD_PARTY_LIB_FILES :=

ifeq (0,1)
# External SRTP library
APP_THIRD_PARTY_EXT += -lsrtp
else
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libsrtp-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lsrtp-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lsrtp
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libsrtp.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libsrtp.$(SHLIB_SUFFIX)
endif
endif

ifeq (libresample,libresample)
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libresample-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
ifeq (,1)
export PJ_RESAMPLE_DLL := 1
APP_THIRD_PARTY_LIBS += -lresample
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libresample.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libresample.$(SHLIB_SUFFIX)
else
APP_THIRD_PARTY_LIBS += -lresample-$(TARGET_NAME)
endif
else
APP_THIRD_PARTY_LIBS += -lresample
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libresample.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libresample.$(SHLIB_SUFFIX)
endif
endif

ifneq (,1)
ifeq (0,1)
# External GSM library
APP_THIRD_PARTY_EXT += -lgsm
else
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libgsmcodec-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lgsmcodec-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lgsmcodec
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libgsmcodec.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libgsmcodec.$(SHLIB_SUFFIX)
endif
endif
endif

ifneq (,1)
ifeq (0,1)
APP_THIRD_PARTY_EXT += -lspeex -lspeexdsp
else
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libspeex-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lspeex-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lspeex
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libspeex.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libspeex.$(SHLIB_SUFFIX)
endif
endif
endif

ifneq (,1)
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libilbccodec-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lilbccodec-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lilbccodec
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libilbccodec.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libilbccodec.$(SHLIB_SUFFIX)
endif
endif

ifneq (,1)
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libg7221codec-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lg7221codec-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lg7221codec
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libg7221codec.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libg7221codec.$(SHLIB_SUFFIX)
endif
endif

ifneq ($(findstring pa,pa_unix),)
ifeq (0,1)
# External PA
APP_THIRD_PARTY_EXT += -lportaudio
else
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libportaudio-$(LIB_SUFFIX)
ifeq ($(PJ_SHARED_LIBRARIES),)
APP_THIRD_PARTY_LIBS += -lportaudio-$(TARGET_NAME)
else
APP_THIRD_PARTY_LIBS += -lportaudio
APP_THIRD_PARTY_LIB_FILES += $(PJ_DIR)/third_party/lib/libportaudio.$(SHLIB_SUFFIX).$(PJ_VERSION_MAJOR) $(PJ_DIR)/third_party/lib/libportaudio.$(SHLIB_SUFFIX)
endif
endif
endif

# CFLAGS, LDFLAGS, and LIBS to be used by applications
export APP_CC := gcc
export APP_CFLAGS := -DPJ_AUTOCONF=1\
	-O2 -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1 -DOPENSSL_NO_SSL2=1\
	$(PJ_VIDEO_CFLAGS) \
	-I$(PJDIR)/pjlib/include\
	-I$(PJDIR)/pjlib-util/include\
	-I$(PJDIR)/pjnath/include\
	-I$(PJDIR)/pjmedia/include\
	-I$(PJDIR)/pjsip/include
export APP_LDFLAGS := -L$(PJDIR)/pjlib/lib\
	-L$(PJDIR)/pjlib-util/lib\
	-L$(PJDIR)/pjnath/lib\
	-L$(PJDIR)/pjmedia/lib\
	-L$(PJDIR)/pjsip/lib\
	-L$(PJDIR)/third_party/lib\
	$(PJ_VIDEO_LDFLAGS) \
	
ifeq ($(PJ_SHARED_LIBRARIES),)
export PJLIB_LDLIB := -lpj-$(TARGET_NAME)
export PJLIB_UTIL_LDLIB := -lpjlib-util-$(TARGET_NAME)
export PJNATH_LDLIB := -lpjnath-$(TARGET_NAME)
export PJMEDIA_AUDIODEV_LDLIB := -lpjmedia-audiodev-$(TARGET_NAME)
export PJMEDIA_VIDEODEV_LDLIB := -lpjmedia-videodev-$(TARGET_NAME)
export PJMEDIA_LDLIB := -lpjmedia-$(TARGET_NAME)
export PJMEDIA_CODEC_LDLIB := -lpjmedia-codec-$(TARGET_NAME)
export PJSIP_LDLIB := -lpjsip-$(TARGET_NAME)
export PJSIP_SIMPLE_LDLIB := -lpjsip-simple-$(TARGET_NAME)
export PJSIP_UA_LDLIB := -lpjsip-ua-$(TARGET_NAME)
export PJSUA_LIB_LDLIB := -lpjsua-$(TARGET_NAME)
export PJSUA2_LIB_LDLIB := -lpjsua2-$(TARGET_NAME)
else
export PJLIB_LDLIB := -lpj
export PJLIB_UTIL_LDLIB := -lpjlib-util
export PJNATH_LDLIB := -lpjnath
export PJMEDIA_AUDIODEV_LDLIB := -lpjmedia-audiodev
export PJMEDIA_VIDEODEV_LDLIB := -lpjmedia-videodev
export PJMEDIA_LDLIB := -lpjmedia
export PJMEDIA_CODEC_LDLIB := -lpjmedia-codec
export PJSIP_LDLIB := -lpjsip
export PJSIP_SIMPLE_LDLIB := -lpjsip-simple
export PJSIP_UA_LDLIB := -lpjsip-ua
export PJSUA_LIB_LDLIB := -lpjsua
export PJSUA2_LIB_LDLIB := -lpjsua2

endif

export APP_LDLIBS := $(PJSUA_LIB_LDLIB) \
	$(PJSIP_UA_LDLIB) \
	$(PJSIP_SIMPLE_LDLIB) \
	$(PJSIP_LDLIB) \
	$(PJMEDIA_CODEC_LDLIB) \
	$(PJMEDIA_LDLIB) \
	$(PJMEDIA_VIDEODEV_LDLIB) \
	$(PJMEDIA_AUDIODEV_LDLIB) \
	$(PJMEDIA_LDLIB) \
	$(PJNATH_LDLIB) \
	$(PJLIB_UTIL_LDLIB) \
	$(APP_THIRD_PARTY_LIBS)\
	$(APP_THIRD_PARTY_EXT)\
	$(PJLIB_LDLIB) \
	-lssl -lcrypto -ldl -lz -lm -lrt -lpthread  -lasound  

# Here are the variabels to use if application is using the library
# from within the source distribution
export PJ_CFLAGS := $(APP_CFLAGS)
export PJ_LDFLAGS := $(APP_LDFLAGS)
export PJ_LDLIBS := $(APP_LDLIBS)

