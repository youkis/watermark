from watermark import *
n=10
m=np.array(mls(n,'full'))
c=m.shape[0]
print c
cor=np.array([correlate(m[i],m[j]) for i in range(c) for j in range(i+1,c)])
condition=(cor==-1)|(cor==-1-2**((n+2)/2))|(cor==-1+2**((n+2)/2))
print condition
indx=np.where(condition.sum(axis=1)==2**n-1)[0]
z=np.zeros(c**2).reshape((c,c))
z[indx/c,indx%c]=1
z=z+z.T
print z
