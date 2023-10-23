REDISTRIBUTION
--------------

This binary distribution contains the below components. Components listed under
the "required" section must be redistributed with all applications using Honeycomb.
Components listed under the "optional" section may be excluded if the related
features will not be used.

Applications using Honeycomb on OS X must follow a specific app bundle structure.
Replace "honeyclient" in the below example with your application name.

honeyclient.app/
  Contents/
    Frameworks/
      Honeycomb.framework/
        Honeycomb <= main application library
        Libraries/
          libEGL.dylib <= ANGLE support libraries
          libGLESv2.dylib <=^
          libvk_swiftshader.dylib <= SwANGLE support libraries
          vk_swiftshader_icd.json <=^
        Resources/
          chrome_100_percent.pak <= non-localized resources and strings
          chrome_200_percent.pak <=^
          resources.pak          <=^
          gpu_shader_cache.bin <= ANGLE-Metal shader cache
          icudtl.dat <= unicode support
          snapshot_blob.bin, v8_context_snapshot.[x86_64|arm64].bin <= V8 initial snapshot
          en.lproj/, ... <= locale-specific resources and strings
          Info.plist
      honeyclient Helper.app/
        Contents/
          Info.plist
          MacOS/
            honeyclient Helper <= helper executable
          Pkginfo
      Info.plist
    MacOS/
      honeyclient <= honeyclient application executable
    Pkginfo
    Resources/
      binding.html, ... <= honeyclient application resources

The "Honeycomb.framework" is an unversioned framework that
contains Honeycomb binaries and resources. Executables (honeyclient, honeyclient Helper,
etc) must load this framework dynamically at runtime instead of linking it
directly. See the documentation in include/wrapper/honey_library_loader.h for
more information.

The "honeyclient Helper" app is used for executing separate processes (renderer,
plugin, etc) with different characteristics. It needs to have a separate app
bundle and Info.plist file so that, among other things, it doesn't show dock
icons.

Required components:

The following components are required. Honeycomb will not function without them.

* Honeycomb core library.
  * Honeycomb.framework/Honeycomb

* Unicode support data.
  * Honeycomb.framework/Resources/icudtl.dat

* V8 snapshot data.
  * Honeycomb.framework/Resources/snapshot_blob.bin
  * Honeycomb.framework/Resources/v8_context_snapshot.bin

Optional components:

The following components are optional. If they are missing Honeycomb will continue to
run but any related functionality may become broken or disabled.

* Localized resources.
  Locale file loading can be disabled completely using
  HoneycombSettings.pack_loading_disabled.

  * Honeycomb.framework/Resources/*.lproj/
    Directory containing localized resources used by Honeycomb, Chromium and Blink. A
    .pak file is loaded from this directory based on the HoneycombSettings.locale
    value. Only configured locales need to be distributed. If no locale is
    configured the default locale of "en" will be used. Without these files
    arbitrary Web components may display incorrectly.

* Other resources.
  Pack file loading can be disabled completely using
  HoneycombSettings.pack_loading_disabled.

  * Honeycomb.framework/Resources/chrome_100_percent.pak
  * Honeycomb.framework/Resources/chrome_200_percent.pak
  * Honeycomb.framework/Resources/resources.pak
    These files contain non-localized resources used by Honeycomb, Chromium and Blink.
    Without these files arbitrary Web components may display incorrectly.

* ANGLE support.
  * Honeycomb.framework/Libraries/libEGL.dylib
  * Honeycomb.framework/Libraries/libGLESv2.dylib
  * Honeycomb.framework/Resources/gpu_shader_cache.bin
  Support for rendering of HTML5 content like 2D canvas, 3D CSS and WebGL.
  Without these files the aforementioned capabilities may fail.

* SwANGLE support.
  * Honeycomb.framework/Libraries/libvk_swiftshader.dylib
  * Honeycomb.framework/Libraries/vk_swiftshader_icd.json
  Support for software rendering of HTML5 content like 2D canvas, 3D CSS and
  WebGL using SwiftShader's Vulkan library as ANGLE's Vulkan backend. Without
  these files the aforementioned capabilities may fail when GPU acceleration is
  disabled or unavailable.
