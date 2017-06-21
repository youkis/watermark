import sys,os
sys.path.append(os.pardir)
from sequence import *
from src.myfunction import *
#from watermark import *
import numpy as np
def test():
    for i in range(2,6):
        ccc(2**i)
    print('ccc genarate OK')
    N=4
    shift=2 # shift size[tap]. shift>=1
    d=[1,0,1,1,0,1,1,0]
    datasize=len(d)
    s=getBaseSequence(ccc(N),datasize,shift)
    y=getEmbedSequence(s,d,N,shift)
    
    tmp=np.correlate(y,s,'full')[len(s)-1:]
    impulses=tmp[:datasize*shift:shift]
    recieve=(impulses>0).astype(int)
    
    if((recieve!=d).sum()==0): print('ccc OK')
    
    
    
    for i in range(2,10):
        mls(i,'one')
        mls(i,'full')
        mls(i,'preferd')
    print('mls genarate OK')
    n=6
    exmls=np.append(mls(n),-1)
    d=[1,1,0,1]
    shift=int(''.join(map(str,d)),2)
    y=np.roll(exmls,shift)
    
    extract=np.argmax(correlate(y,exmls))
    recieve=np.frombuffer(format(extract,'b').encode('utf-8'),'b')-ord('0')
    
    if((recieve!=d).sum()==0): print('mls 1st OK')
    
    
    mseq=mls(n)
    M=np.array([np.roll(mseq,i) for i in range(len(mseq))])
    b=np.random.randint(0,2,2**n-1) # 01 data
    d=b*2-1                         # 1 or -1 data
    y=d.dot(M)+d.sum()
    
    recieve=(correlate(y,mseq)>0).astype(int)
    if((recieve!=b).sum()==0): print('mls 2st OK')
    
    
