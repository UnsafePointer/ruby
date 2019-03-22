CXX	:= armv7l-unknown-linux-gnueabihf-g++
CXX_ARCH_SPECIFIC_FLAGS := -O3
CROSSTOOL_HOME := $(HOME)/x-tools/armv7l-unknown-linux-gnueabihf
SYSROOT := $(CROSSTOOL_HOME)/armv7l-unknown-linux-gnueabihf/sysroot
PREPROCESSOR_MACROS := -DGAMESHELL
OPENGL_API := 3.3
ARCH_SPECIFIC_LIBS :=

include shared.mk
