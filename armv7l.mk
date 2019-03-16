CXX	:= armv7l-unknown-linux-gnueabihf-g++
CROSSTOOL_HOME := $(HOME)/x-tools/armv7l-unknown-linux-gnueabihf
SYSROOT := $(CROSSTOOL_HOME)/armv7l-unknown-linux-gnueabihf/sysroot
PREPROCESSOR_MACROS := -DGAMESHELL
OPENGL_API := 3.3

include shared.mk
