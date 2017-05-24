#coding:utf-8
import numpy as np

# 周期相関関数# {{{
def correlate(x,y): return np.correlate(x,np.append(y,y),'same')[(len(x)-1)/2:-(len(x)+1)/2]
# }}}

