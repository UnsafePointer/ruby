CXX	:= g++
CXX_ARCH_SPECIFIC_FLAGS := -ggdb3
CROSSTOOL_HOME :=
SYSROOT :=
PREPROCESSOR_MACROS := -DHANA
OPENGL_API := 4.5
ARCH_SPECIFIC_LIBS := -lHana -lpthread

include shared.mk
include libHana/lib.mk

install_hana:
	cd libHana && make clean && make
	mv libHana/libHana.a lib/
	mv libHana/libHana.h include/
