ifeq ($(strip $(AMJULIB_MAC_SCRIPT_PATH)),)
$(error "Please set AMJULIB_MAC_SCRIPT_PATH in your environment.")
endif

RUNDIR		:=	../../Build/CompiledAssets
BUILD		:=	../../Build/Mac
TARGET		:=	$(BUILD)/macgame
LIBDIRS   	:= 	../../../amjulib/Releases/amjulib-v.0.5-macosx
SOURCES		:=	../../Source 
BASSPATH	:=	$(AMJULIB_MAC_SCRIPT_PATH)/../../Source/SoundBass/Bass2.4/Macosx

LIBS	:= \
	-lamju \
	$(BASSPATH)/mp3-free/libbass.dylib \
	$(BASSPATH)/libbassmidi.dylib \
	-framework OpenGL \
	-framework GLUT \
	-framework Carbon \
	-lcurl \
	-lstdc++ -lm 

include $(AMJULIB_MAC_SCRIPT_PATH)/GenMakefile
CFLAGS += -D_DEBUG -DAMJU_USE_BASS

run:
	cp $(TARGET) $(RUNDIR); \
	cp $(BASSPATH)/libbassmidi.dylib $(RUNDIR); \
	cp $(BASSPATH)/mp3-free/libbass.dylib $(RUNDIR); \
	cd $(RUNDIR); \
	./macgame


runnoglue:
	cp $(TARGET) $(RUNDIR)/Mac/; \
	cp $(BASSPATH)/libbassmidi.dylib $(RUNDIR)/Mac/; \
	cp $(BASSPATH)/mp3-free/libbass.dylib $(RUNDIR)/Mac/; \
	cd $(RUNDIR)/Mac/; \
	./macgame


runsetup:
	cp $(TARGET) $(RUNDIR); \
	cp $(BASSPATH)/libbassmidi.dylib $(RUNDIR); \
	cp $(BASSPATH)/mp3-free/libbass.dylib $(RUNDIR); 


