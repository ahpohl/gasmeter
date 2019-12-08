#!/bin/bash
RRD=/var/lib/gasmeter/mag.rrd
rrdtool graph mag.gif -A -w 1200 -h 400 -s 'now - 30 min' -e 'now' \
  DEF:bx=$RRD:bx:AVERAGE \
  DEF:by=$RRD:by:AVERAGE \
  DEF:bz=$RRD:bz:AVERAGE \
  CDEF:b=bx,bx,*,by,by,*,+,bz,bz,*,+,SQRT \
  LINE2:by#000000:By
#  LINE2:bx#FF0000:Bx \
#  LINE2:bz#0000FF:Bz
#  LINE1:b#000000:B
