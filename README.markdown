cportage - damn fast Gentoo Linux package manager
=================================================

http://github.com/slonopotamus/cportage

Dependencies
------------
Build time only:

 -  CMake >= 2.8 (tested with 2.8.1)
 -  bison (tested with 2.4.3)
 -  flex (tested with 2.5.35)
 -  GCC (tested with 4.4.3)

Both build and runtime:

 -  GLib >= 2.20 (tested with 2.22.5)

Compiling
---------

1. Create an empty directory and `cd` into it.
2. Run `cmake /path/to/cportage/source/dir/`.
That'll generate a makefile and various cmake helper things.
3. Run `ccmake .` and configure build options.
4. Now, run `make` to compile cportage sources and `make install` to install.

**Attention**, CMake doesn't support `make uninstall`.

Documentation
-------------
See 'man' dir

License
-------
cportage is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Authors
-------
 -  Marat Radchenko <marat@slonopotamus.org>

Contact
-------
If you have some ideas about cportage improvements, please feel
free to send a message to <marat@slonopotamus.org>
