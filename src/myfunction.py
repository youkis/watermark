#coding:utf-8
import numpy as np
import sys

# 周期相関関数# {{{
if sys.version_info.major==2:
    def correlate(x,y): return np.correlate(x,np.append(y,y),'same')[(len(x)-1)/2:-(len(x)+1)/2]
if sys.version_info.major==3:
    def correlate(x,y): return np.correlate(x,np.append(y,y),'same')[(len(x)-1)//2:-(len(x)+1)//2]
# }}}

