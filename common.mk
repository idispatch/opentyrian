# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION = OpenTyrian Game
endef

SDL_PATH = $(PROJECT_ROOT)/../SDL
SDL_INC_PATH = $(SDL_PATH)/include
SDL_LIB_PATH = $(SDL_PATH)/$(CPU)/$(patsubst o%,a%,$(notdir $(CURDIR)))

BBAMI_PATH = $(PROJECT_ROOT)/../bbami
BBAMI_INC_PATH = $(BBAMI_PATH)/public
BBAMI_LIB_PATH = $(BBAMI_PATH)/$(CPU)/$(patsubst o%,a%,$(notdir $(CURDIR)))

RIMEULA_PATH = $(PROJECT_ROOT)/../rimeula
RIMEULA_INC_PATH = $(RIMEULA_PATH)/public
RIMEULA_LIB_PATH = $(RIMEULA_PATH)/$(CPU)/$(patsubst o%,a%,$(notdir $(CURDIR)))

TOUCH_OVERLAY_PATH = $(PROJECT_ROOT)/../TouchControlOverlay
TOUCH_OVERLAY_LIB_PATH = $(TOUCH_OVERLAY_PATH)/Device-Release

USEFILE=
CCFLAGS += -Wc,-std=c99 -D__BLACKBERRY__ -D__BB10__
EXTRA_SRCVPATH += $(PROJECT_ROOT)/src
EXTRA_INCVPATH += $(SDL_INC_PATH) $(BBAMI_INC_PATH) $(RIMEULA_INC_PATH)
EXTRA_LIBVPATH += $(BBAMI_LIB_PATH) $(RIMEULA_LIB_PATH) $(SDL_LIB_PATH) $(TOUCH_OVERLAY_LIB_PATH)

LIBS += m png14 z ^bbami bps ^rimeula screen ^SDL ^TouchControlOverlay cpp xml2 egl asound GLESv2

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk
OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

NDK_TOOLS_PATH = $(QNX_HOST)/usr/bin
PACKAGER = $(NDK_TOOLS_PATH)/blackberry-nativepackager

.PHONY: show-variables clean clean-release display-vars

clean-release:
	rm -f $(PROJECT_ROOT)/tyrian.bar

clean: clean-release

tyrian.bar: $(PROJECT_ROOT)/bar-descriptor.xml \
            $(PROJECT_ROOT)/tyrian-icon.png \
            $(PROJECT_ROOT)/tyrian-loading.png \
            $(PROJECT_ROOT)/Makefile \
            $(PROJECT_ROOT)/common.mk \
            $(PROJECT_ROOT)/controls.json
	$(PACKAGER) -package $(PROJECT_ROOT)/tyrian.bar $(PROJECT_ROOT)/bar-descriptor.xml -C . -configuration Device-Release

all: tyrian.bar

display-vars:
	$(foreach v,$(.VARIABLES),$(info $(v) = $($(v))))
