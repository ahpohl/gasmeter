#!/usr/bin/python2 -u
import time
import math
import rrdtool
import os
import sys
import re

count_rrd = "/var/lib/gasmeter/gas.rrd"
rrd_daemon = "unix:/tmp/rrdcached.sock"
count_step = 0.01

def last_rrd_count():
  val = 0.0
  handle = os.popen("rrdtool lastupdate " + count_rrd)
  for line in handle:
    m = re.match(r"^[0-9]*: [0-9.]*", line)
    if m:
      val = float(m.group(0).split(" ")[1])
      break
  handle.close()
  return val * count_step

def main():
  val = last_rrd_count()
  print("gas(%.2f*m3)" % val)

if __name__ == '__main__':
  main()
