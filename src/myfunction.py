#coding:utf-8
import numpy as np
from watermark.sequence import *

# 周期相関関数# {{{
def correlate(x,y): return np.correlate(x,np.append(y,y),'same')[len(x)/2-1:-len(x)/2+1]
# }}}
# 無音部分の消去# {{{
def delsilent(x,fs):
    y=abs(x)
    ave=4410/2
    count=0
    for i in range(0,x.size-ave,ave):
        if y[i:i+ave].mean()>0.005:
            count+=1
        else:
            count=0
        if float(count)*ave/fs>0.7:
            return i-(count-1)*ave
# }}}

