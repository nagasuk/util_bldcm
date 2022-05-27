util\_bldcm (Utilities to control mBldcm)
=========================================

Overview
--------
This is utility set to control mBldcm.

How to build
------------
```sh
$ cmake -S . -B build
$ cmake --build build
```

How to install
--------------
```sh
$ cmake --install build --prefix <Path to install>
```

List of utilities
-----------------
* bldcmmon:  
  Monitor for mBldcm.

How to use the tools
--------------------
### bldcmmon
Please make configuration file named "bldcmmon.cfg" in "/etc/".
You can use a sample file in "\<install root path\>/share/doc/bldcm/".

Requirement
-----------
* cmake (only at build)
* boost program\_options (only at build)
* drvfpgasoc
* libfpgasoc
* libbldcm

License
-------
Please see `LICENSE.md` file for details.

