#
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of REDHAWK.
#
# REDHAWK is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# REDHAWK is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#


# Make file for the Java and C++ code and documentation.
#
# @author 
#

################################################################################
# Java Compiler Options
################################################################################
ifdef JAVAHOME
    jar_cmd     = $(JAVAHOME)/bin/jar
    java_cmd    = $(JAVAHOME)/bin/java
    javac_cmd   = $(JAVAHOME)/bin/javac
    javadoc_cmd = $(JAVAHOME)/bin/javadoc
else
  ifdef JAVA_HOME
    jar_cmd     = $(JAVA_HOME)/bin/jar
    java_cmd    = $(JAVA_HOME)/bin/java
    javac_cmd   = $(JAVA_HOME)/bin/javac
    javadoc_cmd = $(JAVA_HOME)/bin/javadoc
  else
    jar_cmd     = jar
    java_cmd    = java
    javac_cmd   = javac
    javadoc_cmd = javadoc
  endif
endif

java_build_flags       := $(VRT_JAVAC_FLAGS)
java_doc_flags         := $(VRT_JAVADOC_FLAGS)
java_run_flags         := $(VRT_JAVA_FLAGS) -Djava.net.preferIPv4Stack=true

################################################################################
# C++ Compiler Options
################################################################################
ifeq ($(VRT_CC),icc)
  cpp_compiler         := icc
  cpp_build_flags      := -O3 -w1   -fPIC -lpthread $(VRT_CC_FLAGS)
else
  # -rdynamic is required if using INCLUDE_STACK_TRACE and doesn't hurt to
  # include at other times (this is NOT the same as using -idynamic in ICC).
  cpp_compiler         := g++
  cpp_build_flags      := -O3 -Wall -fPIC -lpthread $(VRT_CC_FLAGS) -rdynamic
endif

ifeq ($(VRT_FAST),true)
  ifeq ($(VRT_CC),icc)
    cpp_build_flags    := $(cpp_build_flags) -xHost #-parallel -simd -vec
  else
    cpp_build_flags    := $(cpp_build_flags) -mtune=native -march=native #-mfpmath=sse -mmovbe
  endif
endif

ifeq ($(INTERNAL_FLOAT16_LIB),0)
  ifeq ($(VRT_CC),icc)
    cpp_build_flags    := -DINTERNAL_FLOAT16_LIB=0 $(cpp_build_flags)
  else
    # Additional flags required if building with GCC's IEEE 754-2008 16-bit
    # floating-point number support (only valid with GCC 4.5+).
    cpp_build_flags    := -DINTERNAL_FLOAT16_LIB=0 $(cpp_build_flags) -mfp16-format=ieee
  endif
else
    cpp_build_flags    := -DINTERNAL_FLOAT16_LIB=1 $(cpp_build_flags)
endif

ifeq ($(INTERNAL_UUID_LIB),1)
    cpp_build_flags    := -DINTERNAL_UUID_LIB=1 $(cpp_build_flags)
else
  ifeq ($(INTERNAL_UUID_LIB),2)
    gcrypt_build_flags := $(shell libgcrypt-config --cflags --libs)
    cpp_build_flags    := -DINTERNAL_UUID_LIB=2 $(cpp_build_flags) $(gcrypt_build_flags)
  else
    cpp_build_flags    := -DINTERNAL_UUID_LIB=0 $(cpp_build_flags) -luuid
  endif
endif

################################################################################
# C++ Compiler Options (Dynamic vs Static Libraries)
################################################################################
cpp_build_flags_static := $(cpp_build_flags)

ifeq ($(DYNAMIC_LIBS),1)
  cpp_build_flags        := -DDYNAMIC_LIBS $(cpp_build_flags) -ldl
  cpp_build_flags_static := -DDYNAMIC_LIBS $(cpp_build_flags_static)
endif

ifndef VRT_PREFIX
  VRT_PREFIX := /usr/local
endif

ifndef VRT_PROC
  VRT_PROC := $(shell uname -p)
endif

ifndef VRTHOME
  VRTHOME := $(shell pwd)
endif

vrt_expected := $(shell /bin/pwd)
vrt_actual   := $(shell cd $(VRTHOME:/=) && /bin/pwd)

ifneq ($(vrt_expected),$(vrt_actual))
  vrt_error       := "ERROR: setting of VRTHOME=$(VRTHOME) conflicts with current build directory, $(vrt_expected)"
else
  vrt_error       := ""
endif

ifeq (TRUE,$(shell (test $(VRT_PROC) != "i386") \
                && (test $(VRT_PROC) != "i686") \
                && (test $(VRT_PROC) != "x86_64") && echo "TRUE"))
  processor_error := "ERROR: Could not determine processor type (got $(VRT_PROC)), \
	              use 'setenv VRT_PROC i386' or 'setenv VRT_PROC x86_64' to override."
else
  processor_error := ""
endif


#######################################################
# Build options
#.NOTPARALLEL :
#.SILENT :
.SUFFIXES:	# don't want and don't need these set
.DEFAULT:	# don't want and don't need these set
.VPATH: cpp_src/vrt/lib cpp_src/vrt/net cpp_src/vrt/libm

#######################################################
# The next three lines based on an example in the GNU Make user's guide
empty :=
colon := :
space := $(empty) $(empty)
comma := ,
#######################################################
cpp_includes      := -Icpp_src/vrt/lib/ -Icpp_src/vrt/net/ -Icpp_src/vrt/libm/
cpp_header_files  := $(wildcard cpp_src/vrt/lib/*.h ) $(wildcard cpp_src/vrt/net/*.h) \
                     cpp_src/vrt/lib/PackUnpack.h cpp_src/vrt/libm/PacketFactory.h
cpp_source_files  := $(patsubst %.h,%.cc,$(cpp_header_files))
cpp_obj_files     := $(patsubst %.h,%.o,$(cpp_header_files))
cpp_header_tests  := $(wildcard cpp_test/vrttest/*/*.h )
cpp_source_tests  := $(patsubst %.h,%.cc,$(cpp_header_tests))
cpp_obj_tests     := $(patsubst %.h,%.o,$(cpp_header_tests))
cpp_output_files  := cpp_lib/libvrt.so cpp_lib/tai-utc.dat cpp_doc
cpp_output_tests  := cpp_lib/vrttest
cpp_output_static := cpp_lib/libvrt-static.a 

java_libg_sourceX := $(wildcard java_src/nxm/vrt/dat/*.java) \
                     $(wildcard java_src/nxm/vrt/lib/*.java) \
                     $(wildcard java_src/nxm/vrt/libg/*.java) \
                     $(wildcard java_src/nxm/vrt/net/*.java) \
                     java_src/nxm/vrt/lib/PackUnpack.java
java_libg_source  := $(subst java_src/nxm/vrt/lib/PackUnpack.java,$(space),$(java_libg_sourceX))
java_source_files := $(java_libg_source) \
                     $(wildcard java_src/nxm/vrt/libm/*.java)
java_class_files  := $(patsubst %.java,%.class,$(java_source_files))
java_source_tests := $(wildcard java_test/nxm/vrttest/*/*.java)
java_class_tests  := $(patsubst %.java,%.class,$(java_source_tests))
java_jar_files1   := 
java_jar_files2   := $(java_jar_files1) $(if $(wildcard java_lib/distro.txt), java_lib/distro.txt, java_lib/vrt.jar)
java_classpath1   := java_src/
java_classpath2   := $(subst $(space),$(colon),$(java_jar_files1)):java_lib/vrt.jar
java_output_files := java_lib/vrt.jar $(java_jar_files1) java_doc \
	             java_src/nxm/vita/cfg/manifest.mf \
	             java_src/nxm/vrt/cfg/manifest.mf
java_output_tests := java_lib/vrttest.jar java_doc
java_build_flags  := $(java_build_flags) -classpath $(java_classpath1) -d java_src/ \
	             -deprecation -Xlint:all
java_test_flags   := $(java_build_flags) -classpath java_lib/vrt.jar:java_test/ -d java_test/ \
	             -deprecation -Xlint:all
java_doc_flags    := $(java_doc_flags) -classpath $(java_classpath1) -d java_doc/ \
	             -version -author -quiet -noindex 

pkt_source        := $(wildcard pkt_src/*.xml)
pkt_output_files  := pkt_doc java_src/nxm/vrt/libm cpp_src/vrt/libm

# Commands for running PacketGen internally, the first is "bootstrap" variant
# used when building Java code, the second is the "normal" variant.
run_pkt_gen1      := $(java_cmd) -classpath $(java_classpath1) -Dvrt.packet_factory="" \
                     nxm.vrt.libg.PacketGen --package nxm.vrt.libm --namespace vrt
run_pkt_gen2      := $(java_cmd) -classpath $(java_classpath2) -Dvrt.packet_factory="" \
                     nxm.vrt.libg.PacketGen --package nxm.vrt.libm --namespace vrt
run_make_script   := $(java_cmd) -classpath $(java_classpath2) -DVRTHOME=$(VRTHOME) \
                     -DFLAGS="$(java_run_flags)"

scripts           := bin/niccheck bin/packetgen bin/udpdump
run_java          := $(java_cmd) $(java_run_flags) \
	                  -classpath $(subst $(space),:,$(addprefix \$$jars/,vrt.jar))

ifeq ($(VRT_PROC),x86_64)
  install_inc_dir := $(VRT_PREFIX)/include/vrt/
  install_bin_dir := $(VRT_PREFIX)/bin/
  install_lib_dir := $(VRT_PREFIX)/lib64/
  install_jar_dir := $(VRT_PREFIX)/java/
else
  install_inc_dir := $(VRT_PREFIX)/include/vrt/
  install_bin_dir := $(VRT_PREFIX)/bin/
  install_lib_dir := $(VRT_PREFIX)/lib/
  install_jar_dir := $(VRT_PREFIX)/java/
endif

#######################################################
.PHONY : usage all test cpp java cpptest javatest dist tidy clean cleanjava cleancpp cleanall scripts install

usage:	
	#@echo "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
	@echo "Builds the Java and C++ VRT libraries. Build targets include:"
	@echo "  usage     - Prints this message"
	@echo "  all       - Builds everything"
	@echo "  install   - Builds everything and then installs it (see 'Install Options', below)"
	@echo "  java      - Build the Java code only"
	@echo "  cpp       - Build the C++ code (will build Java dependencies not built yet)"
	@echo "  cppstatic - Build the 'cpp' target plus static shared library"
	@echo "  dist      - Prep for a distribution for a C++ only install that does not require Java"
	@echo "  tidy      - Remove Java .class files (these are not required if using the JAR)"
	@echo "  cleanjava - Remove all Java binaries, generated source code and docs"
	@echo "  cleancpp  - Remove all C++ binaries and docs (generated source code not removed"
	@echo "              since it depends on Java for building)"
	@echo "  clean     - Remove all Java/C++ binaries and docs; and generated Java source code"
	@echo "  cleanall  - Remove all binaries, generated source code and docs"
	@echo ""
	@echo "A distribution that has been prepared for a C++ only install will not re-build any of"
	@echo "the autogenerated code since the code generators are Java-based. Such a distribution"
	@echo "can be 'reset' to support a Java/full build by calling 'make cleanall'."
	@echo ""
	@echo "For a full build:"
	@echo "  Build everything:                  make all"
	@echo "  Remove binaries if rebuilding:     make clean"
	@echo "  Remove ALL build products:         make cleanall"
	@echo ""
	@echo "For a Java only build:"
	@echo "  Build everything:                  make java"
	@echo "  Remove .class files if using JAR:  make tidy"
	@echo "  Remove binaries if rebuilding:     make cleanjava"
	@echo ""
	@echo "For a C++ only build:"
	@echo "  Build everything:                  make cpp"
	@echo "  Remove binaries if rebuilding:     make cleancpp"
	@echo ""
	@echo "Install Options:"
	@echo "  The installation files will be installed in the following locations (VRT_PREFIX"
	@echo "  defaults to /usr/local and can be set any time before calling 'make install'):"
	@echo "     $$VRT_PREFIX/include/vrt/        C++ include files"
	@echo "     $$VRT_PREFIX/lib/                C++ shared objects (32-bit systems)"
	@echo "     $$VRT_PREFIX/lib64/              C++ shared objects (64-bit systems)"
	@echo "     $$VRT_PREFIX/java/               Java JAR files"
	@echo "     $$VRT_PREFIX/bin/                Executable scripts/commands"
	@echo ""
	@echo "Java Compiler Selection:"
	@echo "    Setting JAVAHOME (or JAVA_HOME) will override the default Java components in"
	@echo "    the path. This setting should NOT be used under Mac OS X (particularly with"
	@echo "    JDK 7 and later)."
	@echo ""
	@echo "    Additional Java compiler settings can be specified for use during the build"
	@echo "    by setting VRT_JAVAC_FLAGS."
	@echo ""
	@echo "C++ Compiler Selection:"
	@echo "  By default the GNU C++ compiler (g++) is used for building C++ code, but this"
	@echo "  can be changed by setting the VRT_CC environment variable before calling make:"
	@echo "    GNU C++ Compiler:                setenv VRT_CC g++"
	@echo "    Intel C++ Compiler:              setenv VRT_CC icc"
	@echo ""
	@echo "C++ Floating-Point Library Options:"
	@echo "  By default the build process will use an set of internal library calls to"
	@echo "  provide support for 16-bit floating-point number ('half precision') as"
	@echo "  defined in IEEE 754-2008. Since the internal library calls are portable C++,"
	@echo "  they do not take advantage of any hardware-acceleration that may be available."
	@echo "  Both the Intel and GNU C++ compilers (GNU only since GCC 4.5) have library"
	@echo "  routines that will make use of hardware-acceleration (where possible), but"
	@echo "  their use is very platform-specific. Accordingly the use of the internal"
	@echo "  libraries is the (current) default, but this behavior can be altered by"
	@echo "  setting INTERNAL_FLOAT16_LIB:"
	@echo "    Use compiler-specific code:      setenv INTERNAL_FLOAT16_LIB 0"
	@echo "    Use internal library code:       setenv INTERNAL_FLOAT16_LIB 1"
	@echo ""
	@echo "C++ UUID Library Options:"
	@echo "  By default the standard UUID library (-luuid) will be used when working with"
	@echo "  UUIDs in the C++ code. Since some platforms do not have this library installed,"
	@echo "  there is a mini UUID library internal to the VRT C++ code. (The internal library"
	@echo "  supports the minimum set of functions required here and is not intended for use"
	@echo "  by external users.) The internal library has two versions: the second version"
	@echo "  supports generation of RFC 4122-compliant UUIDs using gcrypt as a source of"
	@echo "  secure-random numbers; the first version does not depend on gcrypt but does"
	@echo "  not support generation of UUIDs (use the first version only as a last resort)."
	@echo "    Standard UUID library:            setenv INTERNAL_UUID_LIB 0"
	@echo "    Internal UUID library (no gen):   setenv INTERNAL_UUID_LIB 1"
	@echo "    Internal UUID library (random):   setenv INTERNAL_UUID_LIB 2"
	@echo ""
	@echo "C++ Dynamic Library Options:"
	@echo "  This is experimental, and may be removed, do not use."
	@echo "    ???:                              setenv DYNAMIC_LIBS 0"
	@echo "    ???:                              setenv DYNAMIC_LIBS 1"
	@echo ""
	@echo ""
	@echo "Platform-Specific Notes:"
	@echo "  Mac OS X:"
	@echo "    Prior to building the C++ code it is necessary to make sure the required"
	@echo "    developer modules are installed, which are usually bundled with XCode."
	@echo "    It is also necessary to set the CPATH to point to the include directories"
	@echo "    for the applicable version of OS X. The CPATH setting will be similar to:"
	@echo "      setenv CPATH /Developer/SDKs/MacOSX10.6.sdk/usr/include/:\"
	@echo "        /Developer/SDKs/MacOSX10.6.sdk/usr/include/c++/4.2.1/:\"
	@echo "        /Developer/SDKs/MacOSX10.6.sdk/usr/include/c++/4.2.1/x86_64-apple-darwin10/"
	@echo ""

all:						java cpp scripts
test:						all javatest cpptest
java:						$(java_output_files)                     pkt_doc scripts
cpp:						$(cpp_output_files)                      pkt_doc scripts
cppstatic:					$(cpp_output_files) $(cpp_output_static) pkt_doc scripts

javatest:					java $(java_output_tests)
	@echo "================================================================"
	@echo "Run Java Tests"
	@echo "================================================================"
	java -jar java_lib/vrttest.jar

cpptest:					cpp  $(cpp_output_tests)
	@echo "================================================================"
	@echo "Run C++ Tests"
	@echo "================================================================"
ifdef LD_LIBRARY_PATH
	export LD_LIBRARY_PATH="cpp_lib/:${LD_LIBRARY_PATH}" && cpp_lib/vrttest
else
	export LD_LIBRARY_PATH="cpp_lib/"                    && cpp_lib/vrttest
endif

dist:						$(java_source_files) \
						$(wildcard java_src/nxm/vrt/libm/*.java) \
						$(cpp_header_files) \
						$(cpp_source_files) \
						$(wildcard cpp_src/vrt/libm/*.h) \
						$(wildcard cpp_src/vrt/libm/*.cc) \
						java_doc cpp_doc pkt_doc scripts
	@echo "================================================================"
	@echo "Build Distribution"
	@echo "================================================================"
	rm -Rf cpp_lib
	rm -f  java_src/nxm/vrt/*/*.class
	rm -Rf java_src/nxm/vrt/libm java_src/nxm/vrt/lib/PackUnpack.java
	rm -f  java_lib/distro.txt
	echo "Distro built on `date +%FT%T%z` " > java_lib/distro.txt
	touch cpp_src/vrt/lib/PackUnpack.h cpp_src/vrt/lib/PackUnpack.cc $(cpp_libm_header) $(cpp_libm_source)

tidy:
	@echo "================================================================"
	@echo "Remove Java Class Files"
	@echo "================================================================"
	rm -f  java_src/nxm/vrt/*/*.class

cleanjava:					tidy
	@echo "================================================================"
	@echo "Remove Java Docs/Libs"
	@echo "================================================================"
	rm -Rf java_doc java_lib
	@echo "================================================================"
	@echo "Remove Auto-Generated Java Code"
	@echo "================================================================"
	rm -Rf java_src/nxm/vrt/libm java_src/nxm/vrt/lib/PackUnpack.java
	rm -f  java_lib/distro.txt
	rm -f  java_src/manifest.mf

cleancpp:
	@echo "================================================================"
	@echo "Remove C++ Docs/Libs"
	@echo "================================================================"
	rm -Rf cpp_doc cpp_lib $(cpp_obj_files)

clean:						cleanjava cleancpp

cleanall:					clean
	@echo "================================================================"
	@echo "Remove Auto-Generated C++ Code"
	@echo "================================================================"
	rm -Rf cpp_src/vrt/libm
	rm -f  cpp_src/vrt/lib/PackUnpack.h cpp_src/vrt/lib/PackUnpack.cc
	@echo "================================================================"
	@echo "Remove Auto-Generated Packet Type Docs"
	@echo "================================================================"
	rm -Rf pkt_doc
	@echo "================================================================"
	@echo "Check for files that may cause distribution issues"
	@echo "================================================================"
	@find . -iname "*.ade" # Rejected by GMail
	@find . -iname "*.adp" # Rejected by GMail
	@find . -iname "*.bat" # Rejected by GMail
	@find . -iname "*.chm" # Rejected by GMail
	@find . -iname "*.cmd" # Rejected by GMail
	@find . -iname "*.com" # Rejected by GMail
	@find . -iname "*.cpl" # Rejected by GMail
	@find . -iname "*.exe" # Rejected by GMail
	@find . -iname "*.hta" # Rejected by GMail
	@find . -iname "*.ins" # Rejected by GMail
	@find . -iname "*.isp" # Rejected by GMail
	@find . -iname "*.jse" # Rejected by GMail
	@find . -iname "*.lib" # Rejected by GMail
	@find . -iname "*.mde" # Rejected by GMail
	@find . -iname "*.msc" # Rejected by GMail
	@find . -iname "*.msp" # Rejected by GMail
	@find . -iname "*.mst" # Rejected by GMail
	@find . -iname "*.scr" # Rejected by GMail
	@find . -iname "*.sct" # Rejected by GMail
	@find . -iname "*.shb" # Rejected by GMail
	@find . -iname "*.sys" # Rejected by GMail
	@find . -iname "*.vb"  # Rejected by GMail
	@find . -iname "*.vbe" # Rejected by GMail
	@find . -iname "*.vbs" # Rejected by GMail
	@find . -iname "*.vxd" # Rejected by GMail
	@find . -iname "*.wsc" # Rejected by GMail
	@find . -iname "*.wsf" # Rejected by GMail
	@find . -iname "*.wsh" # Rejected by GMail
	@find . -iname "*.zip" # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.tar" # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.tgz" # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.taz" # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.z"   # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.gz"  # Container rejected by GMail if it contains a "rejected" type
	@find . -iname "*.rar" # Container rejected by GMail if it contains a "rejected" type

scripts:					$(java_jar_files2)
	@echo "================================================================"
	@echo "Build Scripts"
	@echo "================================================================"
ifneq ($(vrt_error),"")
	@echo $(vrt_error)
	@test 1 == 0                # <- forces make to abort
endif
	mkdir -p bin
	$(run_make_script) nxm.vrt.libg.PacketGen --dumpcshscript >& bin/packetgen
	$(run_make_script) nxm.vrt.net.NICCheck   --dumpcshscript >& bin/niccheck
	$(run_make_script) nxm.vrt.net.UDPDump    --dumpcshscript >& bin/udpdump
	chmod a+x bin/packetgen bin/niccheck bin/udpdump

install:					all $(java_jar_files2)
	@echo "================================================================"
	@echo "Install C++ Libraries"
	@echo "================================================================"
ifneq ($(processor_error),"")
	@echo $(processor_error)
	@test 1 == 0                # <- forces make to abort
endif
	mkdir -p $(install_inc_dir)
	mkdir -p $(install_bin_dir)
	mkdir -p $(install_lib_dir)
	mkdir -p $(install_jar_dir)

	cp -p cpp_src/vrt/*/*.h $(install_inc_dir)
	cp -p cpp_lib/*.so      $(install_lib_dir)
	cp -p java_lib/*.jar    $(install_jar_dir)

	$(run_make_script) -DJARDIR=$(install_jar_dir) nxm.vrt.libg.PacketGen --dumpcshscript >& $(install_bin_dir)/packetgen
	$(run_make_script) -DJARDIR=$(install_jar_dir) nxm.vrt.net.NICCheck   --dumpcshscript >& $(install_bin_dir)/niccheck
	$(run_make_script) -DJARDIR=$(install_jar_dir) nxm.vrt.net.UDPDump    --dumpcshscript >& $(install_bin_dir)/udpdump
	chmod a+x $(install_bin_dir)/packetgen $(install_bin_dir)/niccheck $(install_bin_dir)/udpdump


## Packet Definitions #################################
# The "Pkt" build relies on the JAR files from the Java build, note that the dependency
# for the java_src files is different since they need to be done mid-build for Java,
# while the cpp_src and pkt_doc can be done after-the-fact (the "after-the-fact approach"
# is compatible with the C++ only build).
pkt_doc:					pkt_doc/index.html

pkt_doc/index.html:				$(pkt_source) $(java_jar_files2)
	@echo "================================================================"
	@echo "Auto-Generate Packet Documentation"
	@echo "================================================================"
	mkdir -p pkt_doc
	$(run_pkt_gen2) --HTML pkt_doc/ $(pkt_source)

## C++ ################################################
# The C++ build relies on the JAR files from the Java build
%.o:%.cc
	mkdir -p cpp_lib
	$(cpp_compiler) -c $(cpp_build_flags_static) $(cpp_includes) -static -o $(@) $(<)

cpp_lib/libvrt.so:				$(cpp_header_files) $(cpp_source_files) \
						cpp_src/vrt/lib/PackUnpack.h \
						cpp_src/vrt/lib/PackUnpack.cc \
						cpp_src/vrt/libm/PacketFactory.h \
						cpp_src/vrt/libm/PacketFactory.cc \
						$(wildcard cpp_src/vrt/libm/*.h) \
						$(wildcard cpp_src/vrt/libm/*.cc)
	mkdir -p cpp_lib
	$(cpp_compiler) $(cpp_build_flags) $(cpp_includes) -shared -o cpp_lib/libvrt.so cpp_src/vrt/*/*.cc

# Delete this (?):
#cpp_bin/vrttest:		cpp_src/testvrt.cc cpp_src/vita/host/PacketStream.cc cpp_src/vita/host/PacketStream.h
#	mkdir -p cpp_bin
#	$(cpp_compiler) $(cpp_build_flags) $(cpp_includes) cpp_src/vita/host/PacketStream.cc cpp_src/testvrt.cc -Icpp_src/vita/host -Icpp_src/vrt -lvrt -Lcpp_lib -o cpp_bin/vrttest

cpp_lib/libvrt-static.a:			$(cpp_obj_files)
	@echo "================================================================"
	@echo "Build C++ Static Library"
	@echo "================================================================"
	mkdir -p cpp_lib
	ar cru $(@) $(cpp_obj_files) 
	ranlib $(@)

cpp_lib/vrttest:				$(cpp_header_tests) $(cpp_source_tests) cpp_lib/libvrt.so
	@echo "================================================================"
	@echo "Build C++ Test Code"
	@echo "================================================================"
	mkdir -p cpp_lib
	$(cpp_compiler) $(cpp_build_flags) \
	    $(cpp_includes) \
	    -Icpp_test/vrttest/inc/ -Icpp_test/vrttest/lib/ -Icpp_test/vrttest/net/ -Icpp_test/vrttest/libm/ \
	    -Lcpp_lib/ -lvrt -o cpp_lib/vrttest cpp_test/vrttest/*/*.cc

cpp_lib/tai-utc.dat:				java_src/nxm/vrt/dat/tai-utc.dat
	@echo "================================================================"
	@echo "Copy tai-utc.dat for use from C++"
	@echo "================================================================"
	mkdir -p cpp_lib
	cp $< $@
	touch -m $@

cpp_src/vrt/lib/PackUnpack.h:			$(java_jar_files2) java_src/nxm/vrt/dat/PackUnpackGen.code
	@echo "================================================================"
	@echo "Auto-Generate PackUnpack.h"
	@echo "================================================================"
	$(java_cmd) -classpath $(java_classpath2) nxm.vrt.dat.PackUnpackGen --hdr $@

cpp_src/vrt/lib/PackUnpack.cc:			$(java_jar_files2) java_src/nxm/vrt/dat/PackUnpackGen.code
	@echo "================================================================"
	@echo "Auto-Generate PackUnpack.cc"
	@echo "================================================================"
	$(java_cmd) -classpath $(java_classpath2) nxm.vrt.dat.PackUnpackGen --cpp $@

cpp_doc:					$(cpp_header_files) $(cpp_source_files) doxygen.cfg \
						cpp_src/vrt/lib/PackUnpack.h \
						cpp_src/vrt/lib/PackUnpack.cc \
						cpp_src/vrt/libm/PacketFactory.h \
						cpp_src/vrt/libm/PacketFactory.cc \
						$(wildcard cpp_src/vrt/libm/*.h) \
						$(wildcard cpp_src/vrt/libm/*.cc)
	@echo "================================================================"
	@echo "Build C++ Docs"
	@echo "================================================================"
	rm -Rf cpp_doc
ifneq ($(shell (doxygen --version >> /dev/null) && (echo "TRUE")),TRUE)
	@echo "WARNING: doxygen not found, skipping build of C++ documentation"
	mkdir cpp_doc
	mkdir cpp_doc/html
	echo "<html>"                                 >  cpp_doc/html/index.html
	echo "  <head>"                               >> cpp_doc/html/index.html
	echo "    <title>VRT: Main Page</title>"      >> cpp_doc/html/index.html
	echo "  </head>"                              >> cpp_doc/html/index.html
	echo "  <body>"                               >> cpp_doc/html/index.html
	echo "    <h3>Could not find doxygen, build"  >> cpp_doc/html/index.html
	echo "    of C++ documentation skipped.</h3>" >> cpp_doc/html/index.html
	echo "  </body>"                              >> cpp_doc/html/index.html
	echo "</html>"                                >> cpp_doc/html/index.html
else
	doxygen doxygen.cfg
endif
	touch cpp_doc

cpp_src/vrt/libm/PacketFactory.cc:		cpp_src/vrt/libm/PacketFactory.h

cpp_src/vrt/libm/PacketFactory.h:		$(pkt_source) $(java_jar_files2)
	@echo "================================================================"
	@echo "Auto-Generate C++ Packet Handlers"
	@echo "================================================================"
	$(run_pkt_gen2) --CPP cpp_src/vrt/libm/ $(pkt_source)

## Java ###############################################
# The Java build is more complicated since some of the Java code is auto-generated
# from other Java code and both need to be built and inserted into the JAR file
java_src/manifest.mf:
	@echo "================================================================"
	@echo "Build Java Manifest Files"
	@echo "================================================================"
	echo "Manifest-Version: 1.0"               >| java_src/manifest.mf
	echo "Main-Class: nxm.vrt.lib.Main"        >> java_src/manifest.mf
	echo ""                                    >> java_src/manifest.mf
	
	echo "Manifest-Version: 1.0"               >| java_test/manifest.mf
	echo "Main-Class: nxm.vrttest.lib.Main"    >> java_test/manifest.mf
	echo "Class-Path: vrt.jar"                 >> java_test/manifest.mf
	echo ""                                    >> java_test/manifest.mf
	
	echo "Manifest-Version: 1.0"               >| java_src/nxm/vita/cfg/manifest.mf
	echo "Main-Class: nxm.sys.lib.NeXtMidas"   >> java_src/nxm/vita/cfg/manifest.mf
	echo "Class-Path: nxm-vrt.jar nxm-sys.jar" >> java_src/nxm/vita/cfg/manifest.mf
	echo ""                                    >> java_src/nxm/vita/cfg/manifest.mf
	
	echo "Manifest-Version: 1.0"               >| java_src/nxm/vrt/cfg/manifest.mf
	echo "Main-Class: nxm.sys.lib.NeXtMidas"   >> java_src/nxm/vrt/cfg/manifest.mf
	echo "Class-Path: nxm-sys.jar"             >> java_src/nxm/vrt/cfg/manifest.mf
	echo ""                                    >> java_src/nxm/vrt/cfg/manifest.mf

java_test/manifest.mf:				java_src/manifest.mf
java_src/nxm/vita/cfg/manifest.mf:		java_src/manifest.mf
java_src/nxm/vrt/cfg/manifest.mf:		java_src/manifest.mf

java_lib/vrt.jar:				$(java_source_files) \
						java_src/nxm/vrt/dat/PackUnpackGen.code \
						java_src/nxm/vrt/dat/PackUnpackGen.java \
						$(wildcard java_src/nxm/vrt/libm/*.java) \
						java_src/manifest.mf
	@echo "================================================================"
	@echo "Build Java Code"
	@echo "================================================================"
	mkdir -p java_lib
	$(javac_cmd) $(java_build_flags) java_src/nxm/vrt/dat/PackUnpackGen.java
	$(java_cmd) -classpath $(java_classpath1) nxm.vrt.dat.PackUnpackGen --java java_src/nxm/vrt/lib/PackUnpack.java
	
	$(javac_cmd) $(java_build_flags) java_src/nxm/vrt/libg/PacketGen.java
	$(run_pkt_gen1) --Java java_src/ $(pkt_source)
	
	$(javac_cmd) $(java_build_flags) java_src/nxm/vrt/*/*.java
	cd java_src && $(jar_cmd) cmf manifest.mf ../$@ nxm
	touch -m java_src $@ java_src/nxm/vrt/dat/PackUnpackGen.java \
	                     java_src/nxm/vrt/dat/PackUnpackGen.class \
	                     java_src/nxm/vrt/lib/PackUnpack.java \
	                     java_src/nxm/vrt/lib/PackUnpack.class

java_lib/vrttest.jar:				$(java_source_tests) \
						java_lib/vrt.jar \
						java_test/manifest.mf
	@echo "================================================================"
	@echo "Build Java Test Code"
	@echo "================================================================"
	$(javac_cmd) $(java_test_flags) java_test/nxm/vrttest/*/*.java
	cd java_test && $(jar_cmd) cmf manifest.mf ../$@ nxm
	touch -m java_test $@

# Delete This (?):
#java_lib/%.jar:					java_src/nxm/vrt/jars/%.jar
#	mkdir -p java_lib
#	cp $< $@
#	touch -m $@

java_doc:					java_lib/vrt.jar \
						java_src/manifest.mf
	@echo "================================================================"
	@echo "Build Java Docs"
	@echo "================================================================"
	mkdir -p java_doc
	$(javadoc_cmd) $(java_doc_flags) java_src/nxm/vrt/*/*.java
	touch -m java_doc

#######################################################
