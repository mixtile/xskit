XSKit
======================================

Introduction
--------------------------------------

Xskit is a cross platform javascript gui app developing kit, focused on linux, windows, mac, even embedded platform based on mcu.

Pack
--------------------------------------
Pack you javascript project to create a **app.xpk** file which is required for application loading.

Required:

- QT(Version 5.0 and above)

Pack Steps:

- First, open **XSIDE** project with qtcreator(means open "ide/XSIDE/XSEditor.pro" width qtcreator) and run it(please copy novile directory to shadow build directory if using shadow build).
- Then, create a new project in XSIDE and write your own code in **main.js** of the new project.  
- Last, click the pack button and **app.xpk** will be created in your project. 

Build
--------------------------------------
To create a executable program, you should follow the steps below.

Windows
''''''''''''''''''''''''''''''''''''''

Required:

- Python 2.7 https://www.python.org

**NOTE**
- Before compiling **xskit**, Please change the **xpk** source path of our demo.
  Just open **pal/win32/src/main.cpp** and change the second parameter of **xsStrCpyN(g_szAppId, "file:///C:\\Users\\Lewis\\workspace\\xskit\\app.xpk", sizeof(g_szAppId));**(probably line 163) to absolute path of ***.xpk**(maybe "app.xpk" in the root directory of the project) on your computer.

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

**NOTE**
- Before compiling **xskit**, Please change the **xpk** source path of our demo.
  Just open **pal/linux/src/gtkmain.cpp** and change the second parameter of **xsStrCpy(appId, "file:////home/lewis/git/xs-new/app.xpk");**(probably line 108) to absolute path of ***.xpk**(maybe "app.xpk" in the root directory of the project) on your computer.

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
