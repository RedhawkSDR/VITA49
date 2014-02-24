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

# Define default SDRROOT
%define _sdrroot    /var/redhawk/sdr
%define _prefix    %{_sdrroot}

Name:		libVITA49
Version:	1.0.0
Release:	4%{?dist}
Summary:	A VITA49 library for REDHAWK components
Prefix:		%{_sdrroot}

Group:		Applications/Engineering
License:	LGPLv3+
URL:		http://redhawksdr.org/	
Source0:	%{name}-%{version}.tar.gz

AutoReqProv: No

BuildRequires:	autoconf automake libtool
BuildRequires:	java-devel >= 1.6

%if "%{?rhel}" == "6"
Requires: libuuid
BuildRequires: libuuid-devel
%else
Requires: e2fsprogs
BuildRequires: e2fsprogs-devel
%endif

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
VITA49 Packet Processing Library for REDHAWK.  Uses VITA49 Library version 2759.  
 * Commit: __REVISION__
 * Source Date/Time: __DATETIME__


%package devel
Summary: REDHAWK VITA49 development package
Group: Development/Languages
AutoReqProv: No
Requires: %{name}

%description devel
Development headers and libraries for VITA49 Processing. Uses VITA49 Library version 2759.


%prep
%setup -q


%build
./reconf
SDRROOT=%{_sdrroot} %configure
make 


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}


%files
%defattr(-,redhawk,redhawk)
%dir %{_prefix}/dom/sharedPkgs/libVITA49
%{_prefix}/dom/sharedPkgs/libVITA49/libVITA49.spd.xml
%ifarch x86_64
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.so.*
%else
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.so.*
%endif

%files devel
%defattr(-,redhawk,redhawk)
%ifarch x86_64
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/libVITA49.pc
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/AbstractPacketFactory.h 
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/AbstractVRAFile.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicContextPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicVRAFile.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicVRLFrame.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicVRTPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/BasicVRTState.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/HasFields.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/InetAddress.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/LeapSeconds.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/MetadataBlock.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/Record.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/StandardDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/TimeStamp.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/Utilities.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/UUID.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/Value.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/VRTConfig.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/VRTMath.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/VRTObject.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/PacketFactory.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/PackUnpack.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/PacketIterator.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/EphemerisPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/NoDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/ReferencePointPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/StandardContextPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/StreamStatePacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/include/TimestampAccuracyPacket.h
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.a
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.la
#%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.so.0.0.0
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.so
#%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_x86_64/lib/libVITA49.so.0
%else
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/libVITA49.pc
%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/AbstractPacketFactory.h 
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/AbstractVRAFile.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicContextPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicVRAFile.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicVRLFrame.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicVRTPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/BasicVRTState.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/HasFields.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/InetAddress.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/LeapSeconds.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/MetadataBlock.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/Record.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/StandardDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/TimeStamp.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/Utilities.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/UUID.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/Value.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/VRTConfig.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/VRTMath.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/VRTObject.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/PacketFactory.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/PackUnpack.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/EphemerisPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/NoDataPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/ReferencePointPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/StandardContextPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/StreamStatePacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/TimestampAccuracyPacket.h
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/include/PacketIterator.h

%dir %{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.a
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.la
#%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.so.0.0.0
%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.so
#%{_prefix}/dom/sharedPkgs/libVITA49/default_impl_i686/lib/libVITA49.so.0
%endif
%changelog
