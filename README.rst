Xskit
======================================

Introduction
--------------------------------------

Xskit is a cross platform javascript gui app developing kit, focused on linux, windows, mac, even embedded platform based on mcu.


Build
--------------------------------------

**NOTE**:

- Before compiling **xskit**, we need to change the javascript source path of our demo.
  Just  open **demo/xsdemo/src/demoapp.cpp** and  change the parameter of **invokeJavascript("/home/lewis/git/xs-new/prime.js")** (probably line 41) to absolute path of **prime.js** on your computer. The file **prime.js** is in the root directory of the project.

Windows
''''''''''''''''''''''''''''''''''''''

Required:

- Python 2.7 https://www.python.org

For windows platform, we can build with *waf*, commands as below:

.. code-block:: 

  waf configure
  waf build
   
When it's completed, it will generate **build/demos/xsdemo/xsdemo** executable binary. 

We can also compile **xskit** with visual C++ 2010 and the command below will create a solution:

.. code-block::

  waf msdev

Linux
''''''''''''''''''''''''''''''''''''''

Required:

- Python 2.7 https://www.python.org
- gtk-2.0 http://www.gtk.org

For linux platform, we can build it with *waf*, commands as below:

.. code-block:: sh

  ./waf configure
  ./waf build

When it's completed, it will generate **build/demos/xsdemo/xsdemo** executable binary.

Mac
''''''''''''''''''''''''''''''''''''''

Working on.

MTK 6260a
''''''''''''''''''''''''''''''''''''''

Working on

License
-----------

Apache License, Version 2.0

http://www.apache.org/licenses/LICENSE-2.0
