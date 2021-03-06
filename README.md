cportage - damn fast Gentoo Linux package manager
=================================================
Latest cportage sources & docs are available
at <http://github.com/slonopotamus/cportage>

Dependencies
------------
Build time only:

 -  CMake >= 2.8 (tested with 2.8.11.2)
 -  bison (tested with 2.7.12)
 -  flex (tested with 2.5.37)
 -  GCC >= 4.7 (tested with 4.7.3)
 -  pkg-config (tested with 0.23)
 -  gettext if ENABLE_NLS=ON (tested with 0.18.2)

Both build and runtime:

 -  GLib >= 2.30 (tested with 2.36.4)
 -  GMP (tested with 5.1.2)

Compiling
---------
1. Create an empty directory and `cd` into it
2. Run `cmake /path/to/cportage/sources/dir/`
That'll generate a makefile and various cmake helper things.
3. Run `ccmake .` and configure build options
4. Now, run `make` to compile cportage sources and `make install` to install

**Attention**, CMake doesn't support `make uninstall`

Documentation
-------------
See 'man' dir

License
-------
cportage is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version

Authors
-------
 -  Marat Radchenko <marat@slonopotamus.org>

Contact
-------
If you have ideas about cportage improvement, feel
free to send a message to <marat@slonopotamus.org>
