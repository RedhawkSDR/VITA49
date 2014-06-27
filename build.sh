#!/bin/sh


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


config_ac='configure.ac'
make_am='Makefile.am'
makefile='Makefile'

if [ "$1" == "rpm" ]; then
    # A very simplistic RPM build scenario
    if [ -e redhawk-libVITA49.spec ]; then
        mydir=`dirname $0`
        tmpdir=`mktemp -d`
        cp -r ${mydir} ${tmpdir}/redhawk-libVITA49_v1-1.0.0
        tar czf ${tmpdir}/redhawk-libVITA49_v1-1.0.0.tar.gz --exclude=".svn" -C ${tmpdir} redhawk-libVITA49_v1-1.0.0
	rpmbuild -ta ${tmpdir}/redhawk-libVITA49_v1-1.0.0.tar.gz
	rm -rf $tmpdir
    else
        echo "Missing RPM spec file in" `pwd`
        exit 1
    fi
elif [ -e build.sh ]; then
    if [ "$1" == 'clean' ]; then
            make clean
    else
    # Checks if build is newer than makefile (based on modification time)
        if [[ $config_ac -nt $makefile || $make_am -nt $makefile ]]; then
            ./reconf
            ./configure
        fi
            make
            exit 0
    fi
else
    echo "No build.sh found for $impl"
fi
