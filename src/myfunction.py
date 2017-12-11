#coding:utf-8
import numpy as np

# 周期相関関数
def correlate(x,y):
    return np.correlate(np.append(x,x)[:-1],y,'valid')

def getBaseSequence(ccc,datasize,shift=1,ch=0):
    z=shift*(datasize-1)
    return np.append(ccc[ch],np.zeros((ccc.shape[0],z),dtype=int),axis=1).reshape(-1)[:-z]

def getEmbedSequence(base,data,shift):
    base=np.append(base,np.zeros(shift*(len(data)-1),dtype=int))
    data=np.array(data)
    return (data*2-1).dot(np.array([np.roll(base,i*shift) for i in range(data.size)]))

