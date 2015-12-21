# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION = OpenTyrian
endef
DEVICE_IP = 169.254.0.1
DEVICE_PASS = pass
BAR_CONFIGURATION = Arm-Release
BAR_NAME = opentyrian.bar

SDL_PATH = $(PROJECT_ROOT)/../SDL
SDL_INC_PATH = $(SDL_PATH)/include
SDL_LIB_PATH = $(SDL_PATH)/$(CPU)/$(patsubst o%,a%,$(notdir $(CURDIR)))

USEFILE=
CCFLAGS += -Wc,-std=c99 -D__BLACKBERRY__ -D__BB10__ -D__QNXNTO__

EXTRA_SRCVPATH += $(PROJECT_ROOT)/src
EXTRA_INCVPATH += $(SDL_INC_PATH)
EXTRA_LIBVPATH += $(SDL_LIB_PATH)

LIBS += m png14 z bps screen ^SDL egl asound GLESv2

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
DEPLOYER = $(NDK_TOOLS_PATH)/blackberry-deploy

.PHONY: deploy undeploy show-variables clean clean-release display-vars

clean-release:
	rm -f $(PROJECT_ROOT)/$(BAR_NAME)

clean: clean-release

opentyrian.bar: $(PROJECT_ROOT)/bar-descriptor.xml \
                $(PROJECT_ROOT)/tyrian-icon.png \
                $(PROJECT_ROOT)/splash-720x720.png \
                $(PROJECT_ROOT)/splash-1280x720.png \
                $(PROJECT_ROOT)/splash-1280x768.png \
                $(PROJECT_ROOT)/splash-1440x1440.png \
                $(PROJECT_ROOT)/Makefile \
                $(PROJECT_ROOT)/common.mk \
                $(PROJECT_ROOT)/controls-720x720.json \
                $(PROJECT_ROOT)/controls-1280x720.json \
                $(PROJECT_ROOT)/controls-1280x768.json \
                $(PROJECT_ROOT)/controls-1440x1440.json
	$(PACKAGER) -package $(PROJECT_ROOT)/$(BAR_NAME) $(PROJECT_ROOT)/bar-descriptor.xml -C . -configuration $(BAR_CONFIGURATION) 

deploy:
	$(DEPLOYER) -installApp -device $(DEVICE_IP) -password $(DEVICE_PASS) $(PROJECT_ROOT)/$(BAR_NAME) 

undeploy:
	$(DEPLOYER) -uninstallApp -device $(DEVICE_IP) -password $(DEVICE_PASS) $(PROJECT_ROOT)/$(BAR_NAME) 

display-vars:
	$(foreach v,$(.VARIABLES),$(info $(v) = $($(v))))
