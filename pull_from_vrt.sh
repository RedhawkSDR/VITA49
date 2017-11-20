#!/bin/bash


# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.

# This file is part of REDHAWK.

# REDHAWK is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.

# REDHAWK is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.


vrt_path="../vrt/"
USAGE="Usage: $0 [path to vrt source]
    Default vrt source path: $vrt_path"

if [ $# == 1 ]; then
  if [ -d "$1" ]; then
    vrt_path=$1
  else
    echo "Bad path: $1"
    echo "$USAGE"
    exit 1
  fi
elif [ $# != 0 ]; then
    echo "Too many arguments."
    echo "$USAGE"
    exit 1
fi

if [ ! -d "${vrt_path}/cpp_src/vrt/lib" ]; then
  echo "LibVRT no at specified location: $vrt_path"
  echo "$USAGE"
  exit 1
fi

if [ ! -d "${vrt_path}/cpp_src/vrt/libm" ]; then
  echo "Must 'cd $vrt_path' and 'make cpp' first"
  echo "$USAGE"
  exit 1
fi

cp -v  ${vrt_path}/cpp_src/vrt/lib/*.h cpp/include/
cp -v  ${vrt_path}/cpp_src/vrt/lib/*.cc cpp/src/
cp -v  ${vrt_path}/cpp_src/vrt/libm/*.h cpp/include/
cp -v  ${vrt_path}/cpp_src/vrt/libm/*.cc cpp/src/
cp -vr ${vrt_path}/cpp_doc/html doc/doxygen_html
cp -vr ${vrt_path}/pkt_doc doc/

