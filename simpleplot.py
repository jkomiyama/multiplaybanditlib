#!/usr/env/python
#coding:utf-8

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc
import matplotlib as mpl
import sys, os, copy, math, re

SHOW_THEORETICAL = True

def thin(anarray):
  i = 1.0
  retarray = []
  while(i<=len(anarray)):
    retarray.append(anarray[int(i)-1])
    i*=1.2
  if int(i) != len(anarray):
    retarray.append(anarray[-1])
  return retarray

def readFile(afile):
  lines = [line.strip() for line in file(afile, "r").readlines()]
  algs = []
  data = []
  for line in lines:
    if line[0]=="#":
      if line.find("#policy") != -1:
        algs.append(line.split(" ")[2])
    else:
      sps = line.split(" ")
      data.append(sps)
  data = map(list, zip(*data)) #transpose 2d array
  data = map(thin, data) #reduce data points to speed-up plotting
  return (algs, data)

def getLabel(alg):
  if alg == "Random": return False
  elif alg == "UCB": return "CUCB"
  elif alg == "TS(basic)": return "MP-TS"
  elif alg == "TS(extended)": return "IMP-TS"
  elif alg == "KL-UCB(basic)": return "MP-KL-UCB"
  elif alg == "KL-UCB(extended)": return "IMP-KL-UCB"
  else:
    return alg

def getMarker(alg):
  if   alg == "theoretical1": return ":"
  elif alg == "Random": return "->"
  elif alg == "Exp3M": return "-s"
  elif alg == "UCB": return "-o"
  elif alg == "TS(basic)": return "-"
  elif alg == "TS(extended)": return "-d"
  elif alg == "KL-UCB(basic)": return "-v"
  elif alg == "KL-UCB(extended)": return "-"
  else:
    return "-"

def getColor(alg):
  if   alg == "theoretical1": return "0.25"
  elif alg == "Random": return "0.50"
  elif alg == "Exp3M": return "r"
  elif alg == "UCB": return "y"
  elif alg == "TS(basic)": return "k"
  elif alg == "TS(extended)": return "b"
  elif alg == "KL-UCB(basic)": return "g"
  elif alg == "KL-UCB(extended)": return "0.5"
  else:
    return "0.50"

def getLineWidth(alg):
  if   alg == "theoretical1": return 2
  elif alg == "theoretical2": return 2
  elif alg == "Random": return 1
  elif alg == "Exp3M": return 1
  elif alg == "UCB": return 1
  elif alg == "TS(basic)": return 2
  elif alg == "TS(extended)": return 1
  elif alg == "KL-UCB(basic)": return 1
  elif alg == "KL-UCB(extended)": return 1
  else:
    return 2

def markers(i):
  print "i=",i
  markers = ("--", "-",  "-.", ".", "+")
  return markers[i]

def plotRegret((algs, data), mode=0):
  #plt.figure(figsize=(4,3))
  #plt.subplots_adjust(left=0.2, right=0.9, top=0.9, bottom=0.2)
  plt.xlabel("t: round")
  plt.ylabel("R(t): regret")
  plt.xscale('log')
  count=0
  if SHOW_THEORETICAL:
    plt.plot(data[0], data[1], ":", label="LB", color=getColor("theoretical1"), linewidth=getLineWidth("theoretical1"))
  print "lendata=",len(data)
  for l in range(4, len(data), 2):
    i=l/2-2
    print i,l
    print algs[i]
    label = getLabel(algs[i])
    if label:
      plt.plot(data[0], data[l], getMarker(algs[i]), color=getColor(algs[i]), label=label, linewidth=getLineWidth(algs[i]))
      count+=1
  plt.legend(loc="upper left", fontsize=12)
  if mode==0:
    plt.savefig('regret.pdf')
  else:
    plt.savefig('regret'+str(mode)+'.pdf')
  plt.show()

if __name__ == '__main__':
  if len(sys.argv)>=2:
    afile = sys.argv[1]
    result = readFile(afile)
    plotRegret(result)
  else:
    print "usage: this (filename)"


