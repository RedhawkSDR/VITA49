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
#reflects the name in the configure.ac file
Name:		redhawk-libVITA49_v1
#must match the version number in the configure.ac file
Version:       3.0.0
Release:       1%{?dist}
Summary:	A VITA49 library for REDHAWK components
Prefix:		%{_sdrroot}

Group:		Applications/Engineering
License:	LGPLv3+
URL:		http://redhawksdr.org/
Source0:	%{name}-%{version}.tar.gz

AutoReqProv: yes

BuildRequires:	autoconf automake libtool

%if "%{?rhel}" == "6"
Requires: libuuid
BuildRequires: libuuid-devel
%else
Requires: e2fsprogs
BuildRequires: e2fsprogs-devel
%endif

Requires(pre):  redhawk

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
%dir %{_prefix}/dom/deps/VITA49_v1
%{_prefix}/dom/deps/VITA49_v1/VITA49_v1.spd.xml
%dir %{_prefix}/dom/deps/VITA49_v1/cpp
%dir %{_prefix}/dom/deps/VITA49_v1/cpp/lib
%dir %{_prefix}/dom/deps/VITA49_v1/cpp/lib/pkgconfig/VITA49_v1.pc
%{_prefix}/dom/deps/VITA49_v1/cpp/lib/libVITA49_v1.so*


%files devel
%defattr(-,redhawk,redhawk)
%dir %{_prefix}/dom/deps/VITA49_v1/cpp
%dir %{_prefix}/dom/deps/VITA49_v1/cpp/include
%{_prefix}/dom/deps/VITA49_v1/cpp/include/AbstractPacketFactory.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/AbstractVRAFile.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicContextPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicDataPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicVRAFile.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicVRLFrame.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicVRTPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/BasicVRTState.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/HasFields.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/InetAddress.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/LeapSeconds.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/MetadataBlock.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/Record.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/StandardDataPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/TimeStamp.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/Utilities.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/UUID.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/Value.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/VRTConfig.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/VRTMath.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/VRTObject.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/PacketFactory.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/PackUnpack.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/PacketIterator.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/EphemerisPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/NoDataPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/ReferencePointPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/StandardContextPacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/StreamStatePacket.h
%{_prefix}/dom/deps/VITA49_v1/cpp/include/TimestampAccuracyPacket.h
%dir %{_prefix}/dom/deps/VITA49_v1/cpp/lib
%{_prefix}/dom/deps/VITA49_v1/cpp/lib/libVITA49_v1.a
%{_prefix}/dom/deps/VITA49_v1/cpp/lib/libVITA49_v1.la
%{_prefix}/dom/deps/VITA49_v1/cpp/lib/libVITA49_v1.so*
%dir %{_prefix}/dom/deps/VITA49_v1/cpp/lib/pkgconfig/
%{_prefix}/dom/deps/VITA49_v1/cpp/lib/pkgconfig/VITA49_v1.pc


%changelog
