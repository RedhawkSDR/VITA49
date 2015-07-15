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

AC_INIT(VITA49, 3.0.0)
AM_INIT_AUTOMAKE([nostdinc])
LT_INIT

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_PREFIX_DEFAULT(${SDRROOT})
#change this number to reflect the variation you are creating for this library
AC_SUBST([spkg_variation],['v1'])
#name of the impl the package should use
AC_SUBST([spkg_base],[cpp])
AC_SUBST([OS_NAME],[`uname -s`])
AC_SUBST([ARCH],[`uname -p`])
AC_SUBST([spkg_dir],['${prefix}'/dom/deps/${PACKAGE_NAME}_${spkg_variation}])
AC_SUBST([spkg_impl],[${spkg_base}])
AC_SUBST([spkg_impl2],['${spkg_base}'])
AC_SUBST([spkg_lib],[lib])
AC_SUBST([spkg_impldir],['${spkg_dir}/${spkg_impl}'])
AC_SUBST([spkg_libdir],['${spkg_dir}/${spkg_impl}/${spkg_lib}'])
AC_SUBST([spkg_pkgconfig_dir],['${spkg_libdir}/pkgconfig'])
AC_SUBST([spkg_includedir],['${spkg_dir}/${spkg_impl}/include'])

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES([${PACKAGE_NAME}_${spkg_variation}.pc:packageName.pc.in])
AC_CONFIG_FILES([${PACKAGE_NAME}_${spkg_variation}.spd.xml:packageName.spd.xml.in])
AC_OUTPUT

