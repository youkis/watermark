from distutils.core import setup, Extension
#from numpy.distutils.misc_util import get_numpy_include_dirs

setup(
    name= 'watermark',
    version= '1.0',
    author= 'youki sada',
    author_email= 'youki@kaede-tokyo.net',
    description= 'This is a package involves my functions',
    package_dir = {'watermark':''},
    ext_modules= [
        Extension('watermark.sequence',sources=['src/sequence.c'])#,include_dirs=[]+get_numpy_include_dirs())
    ]
)
