!
! This file is protected by Copyright. Please refer to the COPYRIGHT file
! distributed with this source distribution.
!
! This file is part of REDHAWK.
!
! REDHAWK is free software: you can redistribute it and/or modify it
! under the terms of the GNU Lesser General Public License as published by the
! Free Software Foundation, either version 3 of the License, or (at your
! option) any later version.
!
! REDHAWK is distributed in the hope that it will be useful, but WITHOUT
! ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
! FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
! for more details.
!
! You should have received a copy of the GNU Lesser General Public License
! along with this program.  If not, see http://www.gnu.org/licenses/.
!

!  Test case for SOURCEVRT.
!
! @author         
startmacro s:HOSTPORT["XXX.XXX.XXX.XXX:XXXX"]
  pipe on
    panel/setup/controls=gc
    sourcevrt HOSTPORT="^hostport" STREAM0=_pipe0
    status _pipe0
    status _pipe0 TYPE=type XDELTA=xdelta
    if (type == 1000) then
      plot _pipe0 yMin=-150 yMax=150 xMin=0 xMax=100*xdelta
    else
      plot _pipe0
    endif
    
    gc button state "Action   " "Exit" /toff
  pipe off
endmacro

procedure processMessage m:msg
  !say "Got message ^msg data=^msg.data"

  if msg.name eqs "STATE"
    if msg.data eqs "EXIT"
      pipe stop
    else
      warning "Unknown state ^msg.data"
    endif
  else
    warning "Unknown message ^msg data=^msg.data"
  endif
return
