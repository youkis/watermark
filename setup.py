from distutils.core import setup, Extension
import numpy as np

setup(
    name= 'watermark',
    version= '1.0',
    author= 'youki sada',
    author_email= 'youki@kaede-tokyo.net',
    description= 'This is a package involves my functions',
    package_dir = {'watermark':''},
    packages = ['watermark'],
    py_modules = ['watermark.src.myfunction'],
    ext_modules= [
        Extension('watermark.sequence',['src/sequence.c'],extra_objects = [],include_dirs = [np.get_include()])
    ]
)
