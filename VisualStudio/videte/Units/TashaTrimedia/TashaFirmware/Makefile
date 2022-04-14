#
#	vivot example program Makefile
#
#	usage:  make TCS=<path>
#

TCS         = d:/Programme/Developer/TriMedia/SDE
PSOS_SYSTEM = $(TCS)/OS/pSOS/pSOSystem



####################################################################
# Compilation- and link flags
####################################################################

CINCS       = -I. -I$(PSOS_SYSTEM)/include 
#CFLAGS      = -O5
CFLAGS      = -O5 -btype boot
LDFLAGS     = 

ifeq ($(CFG),debug)
CFLAGS		= -g -O0 -ldev_g -D_DEBUG=1
endif

####################################################################
# Desired name of application, plus objects to link
####################################################################

APPLICATION = TashaFirmware.out
OBJECTS =	$(OBJDIR)/CTmApp.o \
			$(OBJDIR)/CCodec.o \
			$(OBJDIR)/CVideoOut.o \
			$(OBJDIR)/CVideoIn.o \
			$(OBJDIR)/CMotionDetection.o \
			$(OBJDIR)/CICP.o \
			$(OBJDIR)/CJpeg.o \
			$(OBJDIR)/CPerfMon.o \
			$(OBJDIR)/CInt64.o \
			$(OBJDIR)/Helper.o \
			$(OBJDIR)/CTMList.o \
			$(OBJDIR)/CTMArray.o \
			$(OBJDIR)/CMDPoints.o \
			$(OBJDIR)/CAlarm.o \
			$(OBJDIR)/CRelay.o

target: $(APPLICATION)


####################################################################
# Selected application configuration
####################################################################

#-------------------------------------------
# Generate pSOS application for
#  Win95  --> Windows 95 hosted system
#  WinNT  --> Windows NT hosted system
#  MacOS  --> Mac hosted system
#  tmsim  --> tmsim simulation only
#  nohost --> Any of the above, or for
#             standalone execution.
#             'nohost' does not link in
#             any communication library,
#             and therefore is much smaller

#HOST  = Win95
HOST = WinNT
#HOST = MacOS
#HOST = tmsim
#HOST = nohost


#-------------------------------------------
# Endianness of generated executable:
#   el --> compile and link for little endian
#   eb --> compile and link for big endian 

ENDIAN  = el
#ENDIAN = eb


#-------------------------------------------
# In generated pSOS application:
#   psos  --> use single-processor version of pSOS kernel
#   psosm --> use multi-processor version of pSOS kernel 

PSOS  = psos
#PSOS = psosm


#-------------------------------------------
# In generated pSOS application:
#   nodynamic --> don't use the dynamic loader
#   dynamic --> allow use of dynamic loader 

DYNAMIC  = nodynamic
#DYNAMIC = dynamic


#-------------------------------------------
# (Sub)component enabling/disabling switches:
#

#PNA   = pna
PNA    = nopna

#PPP   = ppp
PPP    = noppp



####################################################################
# Include invariant part of this makefile
####################################################################

include $(PSOS_SYSTEM)/include/Makefile.inc



####################################################################
# Application building
####################################################################

$(OBJDIR)/CTmApp.o: CTmApp.cpp CTmApp.h	tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CCodec.o: CCodec.cpp CCodec.h	tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o	$@

$(OBJDIR)/CVideoIn.o: CVideoIn.cpp CVideoIn.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CVideoOut.o: CVideoOut.cpp CVideoOut.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CMotionDetection.o: CMotionDetection.cpp CMotionDetection.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CICP.o: CICP.cpp CICP.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CJpeg.o: CJpeg.cpp CJpeg.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CPerfMon.o: CPerfMon.cpp CPerfMon.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CInt64.o: CInt64.cpp CInt64.h	tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/Helper.o: Helper.cpp	Helper.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CTMList.o: CTMList.cpp CTMList.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CTMArray.o: CTMArray.cpp CTMArray.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CMDPoints.o: CMDPoints.cpp CMDPoints.h tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CAlarm.o: CAlarm.cpp CAlarm.h	tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(OBJDIR)/CRelay.o: CRelay.cpp CRelay.h	tm.h
	@ echo "Compiling $<..."
	@$(CC) $(PSOS_DEF_FLAGS) $(CFLAGS) $(CINCS) -c $< -o $@

$(APPLICATION) : $(PSOS_CHECK) .$(PSOS_CONFIG) .$(OBJDIR) $(OBJECTS) Makefile
	@ echo "Linking $(APPLICATION)..."
	@ $(CC) $(PSOS_DEF_FLAGS) \
		  $(OBJECTS) $(PSOS_LINK) $(PSOS_CONFIG)/bsp.a \
		  $(LDFLAGS) $(CFLAGS) -o $(APPLICATION) \
		  $(TCS)/lib/$(ENDIAN)/libC++.a	\
		  $(TCS)/apps/lib/$(ENDIAN)/libtmVtransICP.a \
		  $(TCS)/MJPGLib/lib/mjpg_enc.o




####################################################################
# Cleanup
####################################################################

clean :
	@ echo "Deleting intermediate files..."
	@rm -rf *.o *.a *% $(APPLICATION)  \
                        .PSOS_CONFIG* PSOS_CONFIG* \
                        .OBJDIR*      OBJDIR*
	@make

