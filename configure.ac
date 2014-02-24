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

AC_INIT(libVITA49, 1.0.0)
AM_INIT_AUTOMAKE([nostdinc])
LT_INIT

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_PREFIX_DEFAULT(${SDRROOT})
AC_SUBST([spkg_base],[default_impl])
AC_SUBST([OS_NAME],[`uname -s`])
AC_SUBST([ARCH],[`uname -p`])
AC_SUBST([spkg_dir],['${prefix}'/dom/sharedPkgs/$PACKAGE_NAME])
AC_SUBST([spkg_impl],[${spkg_base}_${ARCH}])
AC_SUBST([spkg_impl2],['${spkg_base}_${ARCH}'])
AC_SUBST([spkg_lib],[lib])
AC_SUBST([spkg_impldir],['${spkg_dir}/${spkg_impl}'])
AC_SUBST([spkg_libdir],['${spkg_dir}/${spkg_impl}/${spkg_lib}'])
AC_SUBST([spkg_includedir],['${spkg_dir}/${spkg_impl}/include'])

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(libVITA49.pc)
AC_CONFIG_FILES(libVITA49.spd.xml)
AC_OUTPUT

