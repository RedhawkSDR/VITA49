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

!  Test case for SINKVRT.
!
! @author         
startmacro s:HOSTPORT["XXX.XXX.XXX.XXX:XXXX"] d:sampleRate[1e5]
  set formatColors {SD=green,CD=cyan,SF=green,CF=cyan,SX=green,CX=cyan,SL=green,CL=cyan,SI=green,CI=cyan,SB=green,CB=cyan,SN=green,CN=cyan}
  table formatColors GETKEYS/S formatList
  table formatColors GETSIZE   formatListSize

  pipe on
    panel/setup/controls=gc
    waveform/RT _pipe0{PS=1M} FORM=CF ELEM=Inf SHAPE=Sine AMP=100.0 FREQ=SampleRate/10 DELTA=1/sampleRate

    timex now time
    set FILE(_pipe0).time time

    sinkvrt HOSTPORT="^hostport" STREAM0=_pipe0
    plot _pipe0 yMin=-150 yMax=150 xMin=0 xMax=100/sampleRate
    status _pipe0

    gc button state "Action   " "Exit" /toff
    gc choice form  "Format   " "^{formatList}"        reg.waveform.format  table={ITEMCOLORS=^{formatColors}} /nc=^{formatListSize}/2
    gc choice shape "Shape    " reg.waveform.shapeList reg.waveform.shape
    gc dvalue sfreq "Frequency"                        reg.waveform.freq    1e3 1e9 1e3
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
  
  elseif msg.name eqs "FORM"
    set reg.waveform.format msg.data

  elseif msg.name eqs "SHAPE"
    set reg.waveform.shape msg.data

  elseif msg.name eqs "SFREQ"
    set reg.waveform.freq msg.data

  else
    warning "Unknown message ^msg data=^msg.data"
  endif
return
