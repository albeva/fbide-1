# Makefile for FBIDE

# we need to know if we're building for linux or win32
ifeq (,$(findstring $(TARGET),linux win32))
$(error need TARGET=linux | win32)
endif

# also should we build release or debug version
ifeq (,$(findstring $(BUILD),debug release))
$(error need BUILD=debug | release)
endif

# set the name of the compiler depending on platform
ifeq ($(TARGET),win32)
CC := gcc.exe
RM := del.exe
else
CC := gcc
RM := rm -f
endif

#compile flags for all modes go here
CPPFLAGS := -Wall -Winvalid-pch -DUSE_PCH -DwxUSE_UNICODE -DWX_PRECOMP -DwxUSE_AUI

ifeq ($(TARGET),win32)
CPPFLAGS += -shared-libgcc -lstdc++_s -D_DLL
endif

#flags specific to building App/ go here
APPCPPFLAGS = -IApp/include -IApp -Isdk/include -include App/wx_pch.h

#flags specific to building the sdk library go here
SDKCPPFLAGS = -Isdk/include -Isdk -include wx_pch.h -fPIC

# debug flags
ifeq ($(BUILD),debug)
CPPFLAGS += -g
endif

# release flags
ifeq ($(BUILD),release)
CPPFLAGS += -O2
endif

# set WX Widgets flags

# on linux and most likely macos wx-config is used for the proper flags
# we'll have to program in the appropriate windows flags at some point
ifeq ($(TARGET),linux)
WXFLAGS := wx-config --cppflags
else
#windows specific wx flags go here
WXFLAGS := wx flags
endif

#these flags get added if we're building the debug version
ifeq ($(BUILD),debug)
ifeq ($(TARGET),linux)
#first linux is easy, wx-config has a option for us to use
WXFLAGS += --debug
else
#windows flags should go here
WXFLAGS += wx flags
endif
endif

APPSRCPATH := App/src
APPHDRSPATH := App/include

SDKSRCPATH := sdk/src
SDKHDRSPATH := sdk/include

APPSRCS := $(wildcard $(APPSRCPATH)/*.cpp)
SDKSRCS := $(wildcard $(SDKSRCPATH)/*.cpp)

APPHDRS := $(wildcard $(APPHDRSPATH)/*.h)
SDKHDRS := $(wildcard $(SDKHDRSPATH)/*.h)

ifndef APPOBJPATH
ifeq ($(BUILD),debug)
APPOBJPATH = App/obj/Debug
else
APPOBJPATH = App/obj/Release
endif
endif

ifndef SDKOBJPATH
ifeq ($(BUILD),debug)
SDKOBJPATH = sdk/obj/Debug
else
SDKOBJPATH = sdk/obj/Release
endif
endif

APPOBJS := $(patsubst $(APPSRCPATH)/%.cpp,$(APPOBJPATH)/%.o,$(APPSRCS))
SDKOBJS := $(patsubst $(SDKSRCPATH)/%.cpp,$(SDKOBJPATH)/%.o,$(SDKSRCS))

ifeq ($(TARGET),win32)
FBIDE := fbide.exe
FBIDEDLL := libfbide.dll
else
FBIDE := fbide
FBIDEDLL := libfbide.so
endif

ifeq ($(BUILD),debug)
OUTDIR := bin/Debug
else
OUTDIR := bin/Release
endif

.SUFFIXES:
.SUFFIXES: .cpp

VPATH = .

ifneq ($(TARGET),win32)
#these tell how to build the object files on linux
$(APPOBJPATH)/%.o : $(APPSRCPATH)/%.cpp $(APPHDRS)
	@echo Building $@
	@$(CC) $(CPPFLAGS) $(APPCPPFLAGS) `$(WXFLAGS)` -c $< -o $@

$(SDKOBJPATH)/%.o : $(SDKSRCPATH)/%.cpp $(SDKHDRS)
	@echo Building $@
	@$(CC) $(CPPFLAGS) $(SDKCPPFLAGS) `$(WXFLAGS)` -c $< -o $@

else
#these ones are for windows
$(APPOBJPATH)/%.o : $(APPSRCPATH)/%.cpp $(APPHDRS)
	@$(CC) $(CPPFLAGS) $(WXFLAGS) -c $< -o $@

$(SDKOBJPATH)/%.o : $(SDKSRCPATH)/%.cpp $(SDKHDRS)
	@$(CC) $(CPPFLAGS) $(SDKCPPFLAGS) $(WXFLAGS) -c $< -o $@

endif
#############

all: $(FBIDE) $(FBIDEDLL)

ifneq ($(TARGET),win32)
#this is the final linking stage of building FBIde on linux
$(FBIDE): $(FBIDEDLL) $(APPOBJS)
	@echo Linking $@
	@$(CC) $(CPPFLAGS) $(APPCPPFLAGS) `$(WXFLAGS) --libs` -L$(OUTDIR) -lfbide $(APPOBJS) -o $(OUTDIR)/$(FBIDE)
	@echo FBIde compiled into: $(OUTDIR)/$(FBIDE)

else
#this is the final linking stage of building FBIde on windows
# needs to have wx libs added to linker list
$(FBIDE): $(FBIDEDLL) $(APPOBJS)
	@$(CC) $(CPPFLAGS) $(APPCPPFLAGS) $(WXFLAGS) $(APPOBJS) -o $(OUTDIR)/$(FBIDE)

endif

ifneq ($(TARGET),win32)
#this is the final linking stage of building the FBIde SDK on linux
$(FBIDEDLL): $(SDKOBJS)
	@echo Linking $@
	@$(CC) -shared $(CPPFLAGS) $(SDKCPPFLAGS) `$(WXFLAGS) --libs` -o $(OUTDIR)/$(FBIDEDLL) $(SDKOBJS) 

else
#this is the final linking stage of building the FBIde SDK on windows
$(FBIDEDLL): $(SDKOBJS)
	@$(CC) $(CPPFLAGS) $(SDKCPPFLAGS) $(SDKOBJS) -o $(OUTDIR)/$(FBIDEDLL)

endif

.PHONY : clean
clean :
	-$(RM) $(APPOBJPATH)/*.o
	-$(RM) $(SDKOBJPATH)/*.o
	-$(RM) $(OUTDIR)/$(FBIDE)
	-$(RM) $(OUTDIR)/$(FBIDEDLL)

