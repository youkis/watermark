import sys
if sys.version_info.major==2:
    from src.myfunction import *
    from sequence import *
if sys.version_info.major==3:
    from watermark.src.myfunction import *
    from watermark.sequence import *
