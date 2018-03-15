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
# By default, the RPM will install to the standard REDHAWK SDR root location (/var/redhawk/sdr)
%{!?_sdrroot: %define _sdrroot /var/redhawk/sdr}
%define _prefix %{_sdrroot}/dom/deps/rh/VITA49

# Point install paths to locations within our target SDR root
%define _libdir        %{_prefix}/cpp/lib
%define _sysconfdir    %{_prefix}/etc
%define _localstatedir %{_prefix}/var
%define _mandir        %{_prefix}/man
%define _infodir       %{_prefix}/info

Name:           rh.VITA49
Version:        4.1.0
Release:        1%{?dist}
Summary:        REDHAWK shared library %{name}

Group:          REDHAWK/Shared Libraries
License:        LGPLv3+
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  redhawk-devel >= 2.0
Obsoletes:      redhawk-libVITA49_v1 < 3.0.0


%description
REDHAWK shared library %{name}. VITA49 packet processing library for REDHAWK. Uses VITA49 Library version 2759.
 * Commit: __REVISION__
 * Source Date/Time: __DATETIME__

%package devel
Summary:        REDHAWK Shared library %{name}
Group:          REDHAWK/Shared Libraries
Requires:       %{name} = %{version}-%{release}
Obsoletes:      redhawk-libVITA49_v1-devel < 3.0.0

%description devel
Libraries and header files for REDHAWK shared library %{name}

%prep
%setup -q


%build
# Implementation cpp
pushd cpp
./reconf
%configure --with-sdr=%{_sdrroot}
make %{?_smp_mflags}
popd


%install
rm -rf $RPM_BUILD_ROOT
# Implementation cpp
pushd cpp
make install DESTDIR=$RPM_BUILD_ROOT
popd


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,redhawk,redhawk,-)
%dir %{_sdrroot}/dom/deps/rh
%dir %{_sdrroot}/dom/deps/rh/VITA49
%{_prefix}/VITA49.spd.xml
%{_prefix}/cpp
%exclude %{_libdir}/libVITA49.la
%exclude %{_libdir}/libVITA49.so
%exclude %{_libdir}/pkgconfig

%files devel
%defattr(-,redhawk,redhawk,-)
%{_libdir}/libVITA49.la
%{_libdir}/libVITA49.so
%{_libdir}/pkgconfig
%{_prefix}/include

%changelog
* Wed Jun 21 2017 Ryan Bauman - 3.0.2-1
- Update version post release
* Wed Jun 21 2017 Ryan Bauman - 3.0.1-2
- Mass rebuild for REDHAWK 2.1.1

