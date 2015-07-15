#
# This file is protected by Copyright. Please refer to the COPYRIGHT
# file distributed with this source distribution.
# 
# This file is part of REDHAWK VITA49Libraries.
# 
# REDHAWK VITA49Libraries are free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK VITA49Libraries are distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.  
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program.  If not, see http://www.gnu.org/licenses/.
#
 
AM_CXXFLAGS = -DINTERNAL_UUID_LIB=0 -DINTERNAL_FLOAT16_LIB=1 -O3 -msse3 -Wall -fPIC -Ivrt_src/cpp_src/vrt/lib -Ivrt_src/cpp_src/vrt/libm -rdynamic
libVITA49_v1_la_LIBADD = -lpthread -luuid


ACLOCAL_AMFLAGS = -I ../aclocal.d

lib_LTLIBRARIES = libVITA49_v1.la 
libdir = $(spkg_libdir)
pkgconfigdir = $(spkg_pkgconfig_dir)/
#references the actual pc file to be created by the configure - defined in the configure.ac AC_CONFIG_FILES
pkgconfig_DATA = VITA49_v1.pc

xmldir = $(spkg_dir)/
#references the actual spd file to be created by the configure - defined in the configure.ac AC_CONFIG_FILES
dist_xml_DATA = VITA49_v1.spd.xml

datadir = $(spkg_impldir)

incdir = $(spkg_includedir)

libVITA49_v1_la_SOURCES = \
	$(vrt)

vrt = \
vrt_src/cpp_src/vrt/lib/AbstractPacketFactory.cc \
vrt_src/cpp_src/vrt/lib/AbstractPacketFactory.h \
vrt_src/cpp_src/vrt/lib/AbstractVRAFile.cc \
vrt_src/cpp_src/vrt/lib/AbstractVRAFile.h \
vrt_src/cpp_src/vrt/lib/BasicContextPacket.cc \
vrt_src/cpp_src/vrt/lib/BasicContextPacket.h \
vrt_src/cpp_src/vrt/lib/BasicDataPacket.cc \
vrt_src/cpp_src/vrt/lib/BasicDataPacket.h \
vrt_src/cpp_src/vrt/lib/BasicVRAFile.cc \
vrt_src/cpp_src/vrt/lib/BasicVRAFile.h \
vrt_src/cpp_src/vrt/lib/BasicVRLFrame.cc \
vrt_src/cpp_src/vrt/lib/BasicVRLFrame.h \
vrt_src/cpp_src/vrt/lib/BasicVRTPacket.cc \
vrt_src/cpp_src/vrt/lib/BasicVRTPacket.h \
vrt_src/cpp_src/vrt/lib/BasicVRTState.cc \
vrt_src/cpp_src/vrt/lib/BasicVRTState.h \
vrt_src/cpp_src/vrt/libm/EphemerisPacket.cc \
vrt_src/cpp_src/vrt/libm/EphemerisPacket.h \
vrt_src/cpp_src/vrt/lib/HasFields.cc \
vrt_src/cpp_src/vrt/lib/HasFields.h \
vrt_src/cpp_src/vrt/lib/InetAddress.cc \
vrt_src/cpp_src/vrt/lib/InetAddress.h \
vrt_src/cpp_src/vrt/lib/LeapSeconds.cc \
vrt_src/cpp_src/vrt/lib/LeapSeconds.h \
vrt_src/cpp_src/vrt/lib/MetadataBlock.cc \
vrt_src/cpp_src/vrt/lib/MetadataBlock.h \
vrt_src/cpp_src/vrt/lib/PacketIterator.cc \
vrt_src/cpp_src/vrt/lib/PacketIterator.h \
vrt_src/cpp_src/vrt/lib/PackUnpack.cc \
vrt_src/cpp_src/vrt/lib/PackUnpack.h \
vrt_src/cpp_src/vrt/lib/Record.cc \
vrt_src/cpp_src/vrt/lib/Record.h \
vrt_src/cpp_src/vrt/lib/StandardDataPacket.cc \
vrt_src/cpp_src/vrt/lib/StandardDataPacket.h \
vrt_src/cpp_src/vrt/lib/TimeStamp.cc \
vrt_src/cpp_src/vrt/lib/TimeStamp.h \
vrt_src/cpp_src/vrt/lib/Utilities.cc \
vrt_src/cpp_src/vrt/lib/Utilities.h \
vrt_src/cpp_src/vrt/lib/UUID.cc \
vrt_src/cpp_src/vrt/lib/UUID.h \
vrt_src/cpp_src/vrt/lib/Value.cc \
vrt_src/cpp_src/vrt/lib/Value.h \
vrt_src/cpp_src/vrt/lib/VRTConfig.cc \
vrt_src/cpp_src/vrt/lib/VRTConfig.h \
vrt_src/cpp_src/vrt/lib/VRTMath.cc \
vrt_src/cpp_src/vrt/lib/VRTMath.h \
vrt_src/cpp_src/vrt/lib/VRTObject.cc \
vrt_src/cpp_src/vrt/lib/VRTObject.h \
vrt_src/cpp_src/vrt/libm/StandardContextPacket.h \
vrt_src/cpp_src/vrt/libm/StandardContextPacket.cc \
vrt_src/cpp_src/vrt/libm/NoDataPacket.cc \
vrt_src/cpp_src/vrt/libm/NoDataPacket.h \
vrt_src/cpp_src/vrt/libm/ReferencePointPacket.cc \
vrt_src/cpp_src/vrt/libm/ReferencePointPacket.h \
vrt_src/cpp_src/vrt/libm/StreamStatePacket.cc \
vrt_src/cpp_src/vrt/libm/StreamStatePacket.h \
vrt_src/cpp_src/vrt/libm/TimestampAccuracyPacket.cc \
vrt_src/cpp_src/vrt/libm/TimestampAccuracyPacket.h \
vrt_src/cpp_src/vrt/libm/PacketFactory.cc \
vrt_src/cpp_src/vrt/libm/PacketFactory.h

# Library headers must also be listed here
dist_inc_DATA = vrt_src/cpp_src/vrt/lib/AbstractPacketFactory.h 
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/AbstractVRAFile.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicContextPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicDataPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicVRAFile.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicVRLFrame.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicVRTPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/BasicVRTState.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/HasFields.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/InetAddress.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/LeapSeconds.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/MetadataBlock.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/PacketIterator.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/Record.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/StandardDataPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/TimeStamp.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/Utilities.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/UUID.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/Value.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/VRTConfig.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/VRTMath.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/VRTObject.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/PacketFactory.h
dist_inc_DATA += vrt_src/cpp_src/vrt/lib/PackUnpack.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/EphemerisPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/NoDataPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/ReferencePointPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/StandardContextPacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/StreamStatePacket.h
dist_inc_DATA += vrt_src/cpp_src/vrt/libm/TimestampAccuracyPacket.h

distclean-local:
	rm -f Makefile
	rm -rf Debug
	rm -f config.*
	rm -rf autom4te.cache
	rm -f acinclude.m4
	rm -f aclocal.m4
	rm -f configure
	rm -f depcomp
	rm -f install-sh
	rm -f ltmain.sh
	rm -f Makefile.in
	rm -f missing
	rm -rf .deps
	rm -rf .libs
	rm -rf vrt_src/java_lib
	rm -rf config.*
	rm -rf libtool
