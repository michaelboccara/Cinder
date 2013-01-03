Cinder on Android
=================

This is a work-in-progress port of Cinder to Android.

When linked as an activity it uses the NDK Native Activity code, which
restricts it to running on Android versions 2.3 (Gingerbread) and upwards. This
restriction does not apply when linked as a static library called through JNI.

Tested to build on Windows, OS X and Linux.


Quickstart
----------

* Install prerequisites:

  - Android SDK and Android NDK R8D
  - Boost v1.50 is required in the top Cinder directory.  The setup-android
    script will automatically install this if there is no "boost" directory
    under Cinder.
  - Cygwin on Windows
  - bash, curl, unzip, tar and bzip2 command-line tools (required by
    setup-android)

* Run setup-android

```
    % cd cinder/android
    % . ./setup-android
```

* Configure build settings by editing jni/cinder/Configure.mk (optional)
  
    USE_FREEIMAGE - use FreeImage image library (wide format compatibility but large)

    USE_STBIMAGE  - use the stb_image image library (small JPEG/PNG/GIF reader)

    USE_GLES2     - select OpenGL ES2 - if disabled then OpenGL ES1.5 is used instead

* Build the library from within the android folder.  Tip: use -j <num cores> to
  enable parallel build.

```
    % ndk-build -j 4
```

* Build one of the sample programs (EaseGallery, FBOBasic, iPhoneAccelerometer,
  MultiTouchBasic or TextureFont)

```
    % cd cinder/samples/<SAMPLE>/android
    % . ./setup-android
    % ndk-build && ant debug && adb install -r bin/<SAMPLE-APK>
```


Linking against Cinder
----------------------

The android/setup-android script will suggest settings for linking against
prebuilt static libraries.

Each sample has a setup-android script fragment that will automatically set
NDK_MODULE_PATH if it is not already defined.

See $NDK/docs/IMPORT-MODULE.html for more details of how to use
NDK_MODULE_PATH.


C++11
-----

Cinder can be built with C++11 support by enabling "USE_CPP_11" in Configure.mk.
A linked application will require the following changes to Application.mk:

APP_CPPFLAGS += -std=c++11 -D_LIBCPP_VERSION

This will enable C++11 language support and switches the appropriate imports in
the Cinder header files.


Orientation
-----------

Always add "orientation|screenSize" to the android:configChanges tag in
AndroidManifest.xml, regardless of whether you use fixed or switchable
orientation.

A change in orientation will trigger onResize() with the new viewport
dimensions.  The actual orientation can be checked by calling
app->getOrientation().

To use fixed orientation, add android:orientation="landscape" or "portrait" to
the activity tag.

Note that a fixed orientation app may be paused/restarted multiple times on
resumption.  It is important that you handle resume() correctly and quickly.
It's a good idea to defer any heavy resource loading until the app starts its
update/draw cycle.


Activity lifecycle
------------------

onStart  -> AppAndroid::setup()
onResume -> AppAndroid::resume(bool renewContext)
onPause  -> AppAndroid::pause()
onSaveInstanceState -> AppAndroid::setSavedState(void** state, size_t size)

If renewContext is true then the GL context has been renewed and all GL
resources (textures, shaders) will need to be released and recreated.

It is not sufficient to assign a new value to a stale shared pointer holder -
you must first reset the holder and then assign the new value. e.g.

```
   Texture mTexture;

   void resume(bool renewContext) {
       if (renewContext) {
           // Context recreated, must reload textures

           // WON'T WORK
           mTexture = loadTexture(); 

           // WORKS
           mTexture.reset();
           mTexture = loadTexture();
       }
   }
```

The default implementation of resume() just calls setup() if renewContext
is true.  This is unsuitable for non-trivial apps.

AppAndroid::getSavedState() can be called from inside setup() to retrieve
state saved by AppAndroid::setSavedState().

Note that, unlike the Android activity lifecycle, AppAndroid::resume() 
will not be automatically called after AppAndroid::setup() completes.


Status
------

* console() output goes to the Android debug log
* loadResource opens files embedded from the project "assets" folder
* FreeImage image source, tested to work with PNG/JPEG/BMP files so far
* stb_image image source, a smaller image backend compatible with PNG/JPEG/GIF
* OpenGL ES2 supported at compile time with a subset of the gl namespace
* TextureFont implementation using Freetype (ES1 and ES2 via propipe)
* Static prebuilt Boost libraries for ARM included (date_time, filesystem,
  regex, system and thread)
* Android save/restore state callbacks
* Propipe block (programmable pipeline), supporting most gl namespace draw
  methods in ES2
* CelPD block (libpd audio interface using OpenSL ES)


TODO
----

* Cinder Audio support (using OpenSL ES)
* URL/Http implementation (currently stubbed out)
* Camera capture 
* Video playback


Credits
-------

Portions of this software are copyright © 2011 The FreeType Project (www.freetype.org).  All rights reserved.

stb_image (nothings.org) - public domain image library by Sean Barrett et al

utf8-cpp (utfcpp.sourceforge.net) - Copyright 2006 Nemanja Trifunovic

This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details.
FreeImage is used under the FIPL, version 1.0.

Parts of the font rendering code are based on freetype-gl (http://code.google.com/p/freetype-gl/)
Copyright 2011 Nicolas P. Rougier. All rights reserved


safetydank 20130103

