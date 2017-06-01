watermark
====
PythonのC拡張モジュールAPIで書かれた、電子透かしに用いる関数ライブラリ

## Requirement
linux, MacOS  
Windowsは動くだろうけど知らない。だめだったら仮想OS立ち上げてください。  
numpy必要 correlate関数だけはnumpyの関数に渡してるだけなので...  
環境によってはPython.hとかのためにpython-devが必要な可能性あり

## Install

```bash
$ git clone https://github.com/youkis/watermark.git
$ cd watermark
```

インストールしたい場合は
```bash
$ python setup.py build
$ sudo python setup.py install
```

インストールせず、watermarkフォルダをおいたディレクトリのみで利用する場合、
```bash
$ vim setup.cfg # ファイル内のコメントを外す
$ python setup.py build
```

## Usage
今のところ、下記のようにimportすれば、以下の関数が実装してあるので使えます
```Python
>>> from watermark import *
```

### mls(N,create='one') ###
+   `N` :
    dimension of monic irreducible polynomial. mls length correspond to 2**N-1
+   `create` :
    generate 'one' or 'full' or 'preferd' pair MLS ([full number of mls][link])
+   `Returns one or full or preferd pair of Maximum Length Sequences`

  [link]: https://ja.wikipedia.org/wiki/%E6%97%A2%E7%B4%84%E5%A4%9A%E9%A0%85%E5%BC%8F#.E4.BD.93.E4.B8.8A.E3.81.AE.E6.97.A2.E7.B4.84.E5.A4.9A.E9.A0.85.E5.BC.8F

### correlate(x,y) ###
+   `x,y`: 1 dimensional list
+   `Returns  cyclic cross-correlation of x and y`

### ccc(N,seed=123456) ###
+   `N` :
    CCC(N,N,N**2)
+   `seed` :
    random seed number
+   `Returns CCC, which is three dimensional list`

### getBaseSequence(CCC,datasize,shift,ch=1) ###
+   `CCC` :
    CCC(N,N,N**2)
+   `datasize` :
    data size [bit]
+   `shift` :
    shift size when you're gonna make embed sequence [tip]
+   `ch` :
    A CCC chanel to use (1<=ch<=N)
+   `Returns base sequence`

### getEmbedSequence(base,data,N,shift) ###
+   `base` :
    base sequence
+   `data` :
    bit list data such as [1,0,0,0,1,1].
+   `N` :
    CCC size.
+   `shift` :
    the shift value when the base sequence is convoluted. [tip]
+   `Returns embed sequence.`


## Demo (Memo?)
### M系列による埋め込み、抽出 ずらし編 ###
```Python
>>> from watermark import *
>>> import numpy as np
>>> n=4
>>> exmls=np.append(mls(n),-1) #拡張M系列
>>> exmls
array([ 1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1, -1])
>>> exmls.sum()
0
>>> d=[1,1,0,1]
>>> ''.join(map(str,d))
'1101'
>>> shift=int(''.join(map(str,d)),2)
>>> shift
13
>>> y=np.roll(exmls,shift)
>>> 
>>> # extract
>>> correlate(exmls,y)
array([ 0,  0, -4, 16, -4,  0,  0,  4, -8,  4, -4,  0, -4,  4, -8,  4])
>>> extract=np.argmax(correlate(y,exmls))
>>> extract
13
>>> np.frombuffer(format(extract,'b'),'b')-ord('0')
array([1, 1, 0, 1], dtype=int8)
>>> d
[1, 1, 0, 1]
```

### M系列による埋め込み、抽出 畳み込み編 ###
```Python
>>> from watermark import *
>>> import numpy as np
>>> mls(4)
[1, 1, 1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, 1]
>>> n=4
>>> mseq=mls(n)
>>> M=np.array([np.roll(mseq,i) for i in range(2**n-1)])
>>> b=np.random.randint(0,2,2**n-1) # 01 data
>>> d=b*2-1                         # 1 or -1 data
>>> d
array([ 1,  1, -1, -1, -1,  1, -1,  1, -1,  1, -1, -1, -1, -1,  1])
>>> M
array([[ 1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1],
       [ 1,  1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1],
       [-1,  1,  1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1],
       [-1, -1,  1,  1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1],
       [-1, -1, -1,  1,  1,  1,  1, -1,  1, -1,  1,  1, -1, -1,  1],
       [ 1, -1, -1, -1,  1,  1,  1,  1, -1,  1, -1,  1,  1, -1, -1],
       [-1,  1, -1, -1, -1,  1,  1,  1,  1, -1,  1, -1,  1,  1, -1],
       [-1, -1,  1, -1, -1, -1,  1,  1,  1,  1, -1,  1, -1,  1,  1],
       [ 1, -1, -1,  1, -1, -1, -1,  1,  1,  1,  1, -1,  1, -1,  1],
       [ 1,  1, -1, -1,  1, -1, -1, -1,  1,  1,  1,  1, -1,  1, -1],
       [-1,  1,  1, -1, -1,  1, -1, -1, -1,  1,  1,  1,  1, -1,  1],
       [ 1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1,  1,  1,  1, -1],
       [-1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1,  1,  1,  1],
       [ 1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1,  1,  1],
       [ 1,  1, -1,  1, -1,  1,  1, -1, -1,  1, -1, -1, -1,  1,  1]])
>>> M.shape
(15, 15)
>>> y=d.dot(M)+d.sum()
>>> y
array([  4,   0,  -4,  -8,  -4,  -4,   0,   0,  -4,   0,  -8,   0, -12,
        -4,  -4])
>>> # extract
>>> correlate(y,mseq)
array([ 16,  16, -16, -16, -16,  16, -16,  16, -16,  16, -16, -16, -16,
       -16,  16])
>>> (correlate(y,mseq)>0).astype(int)
array([1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1])
>>> b
array([1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1])
```

### CCCによる埋め込み、抽出 ###
```Python
>>> from watermark import *
>>> import numpy as np
>>> N=4
>>> shift=2 # shift size[tap]. shift>=1
>>> d=[1,0,1,1,0,1,1,0]
>>> datasize=len(d)
>>> s=getBaseSequence(ccc(N),datasize,shift)
>>> y=getEmbedSequence(s,d,N,shift)
>>> 
>>> #extract
>>> tmp=np.correlate(y,s,'full')[len(s)-1:]
>>> impulses=np.array([tmp[i] for i in range(0,datasize*shift,shift)])
>>> impulses
array([ 64, -64,  64,  64, -64,  64,  64, -64])
>>> (impulses>0).astype(int)
array([1, 0, 1, 1, 0, 1, 1, 0])
```

## Licence
[MIT License](https://github.com/youkis/watermark/blob/master/LICENSE.txt) © Youki Sada

