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

!  Test case for SINKVRT and SOURCEVRT.
!
! @author         
startmacro s:inHOSTPORT["XXX.XXX.XXX.XXX:XXXX"] s:outHOSTPORT["XXX.XXX.XXX.XXX:XXXX"]

  pipe on
    panel/setup/controls=gc
    sourcevrt HOSTPORT="^inHostPort" STREAM0=_pipe0
    plot/id=plot0 _pipe0 yMin=-150 yMax=150 xMin=0 xMax=1e-3
    status _pipe0

    fft _pipe0 _pipe1 1K "HANN" 0
    plot/id=plot1 _pipe1
    status _pipe1

    sinkvrt HOSTPORT="^outHostPort" STREAM0=_pipe1

    gc button state "Action   " "Exit" /toff
    gc choice wind  "Window   " reg.fft.windowList reg.fft.window
    gc choice fftsz "FFT Size " "512,1K,2K,4K"     "1K"
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
  elseif msg.name eqs "WIND"
    set reg.fft.window msg.data

  elseif msg.name eqs "FFTSZ"
    set reg.fft.nfft ^msg.data

  else
    warning "Unknown message ^msg data=^msg.data"
  endif
return
