#!/bin/csh

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

setenv OUT_FILE "$1"
setenv JAVA_CMD "$2"
setenv DEF_DIR  "$3"

echo '#\!/bin/csh'                                >& ${OUT_FILE}
echo '# This file is auto-generated, do not edit' >> ${OUT_FILE}
echo '#'                                          >> ${OUT_FILE}
echo 'if ($?VRTHOME) then'                        >> ${OUT_FILE}
echo '  setenv jars ${VRTHOME}/java_lib/'         >> ${OUT_FILE}
echo 'else'                                       >> ${OUT_FILE}
echo "  setenv jars ${DEF_DIR}/"                  >> ${OUT_FILE}
echo 'endif'                                      >> ${OUT_FILE}
echo "${JAVA_CMD}" '$*'                           >> ${OUT_FILE}

chmod a+x $OUT_FILE
