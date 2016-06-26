[![Build Status](https://travis-ci.org/zz-systems/solowej.svg?branch=dev)](https://travis-ci.org/zz-systems/solowej)
[![Build status](https://ci.appveyor.com/api/projects/status/7qs0uu3nbjwprt19?svg=true)](https://ci.appveyor.com/project/zz-systems/solowej)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](http://www.gnu.org/licenses/lgpl-3.0)

# LibSolowej
Parallelized and vectorized port of LibNoise using the [Gorynych SIMD toolkit](https://github.com/zz-systems/gorynych)


## License

LibSolowej is released under the
[LGPL license](https://www.gnu.org/licenses/lgpl.html). See COPYING.txt and
COPYING.LESSER.txt for details.

## About

[LibNoise](http://libnoise.sourceforge.net/) was originally created by
Jason Bevins

LibSolowej is a config-driven, parallelized, vectorized, partially extended, partially incomplete (still in development!) port of LibNoise.

If you develop with Unity, please take a look at [LibSolowej.Unity](https://github.com/zz-systems/LibSolowej.Unity) as it is basically an efficient .NET wrapper around libsolowej.

Please keep in mind, that this release is a **Preview/Alpha Release** and feel free to report issues and bugs to the [Issue tracker on GitHub](https://github.com/zz-systems/solowej/issues)

## Features

* Config driven (*.json file)
* Vectorized    (configurable @ runtime)
* Multithreaded (configurable)
* Dispatched    (the appropriate code is executed. If your CPU only supports SSE2, then the SSE2 branch will be selected)
* Theoretically platform-independent. The MSVC builds seem not to be that good.
* Extensible

## Current state

* In development! The API should not break that much anymore, but there's still a lot to do! 
* Cache module not exposed
* Ridged multifractal unit test fails, FPU and FMA ( + AVX) value difference is larger than Epsilon (But still a good value to be visually indistinguishable)

### Compiles & runs on:
* Linux 64 bit, CLANG 3.8 (preferred, the built binary is faster than with GCC!)
* Linux 64 bit, GCC 6.1.1
* Windows 10 64bit, MSVC 2015 Update 2, x64 build

### Compiles, but does not work:
* Windows 10, 64bit, MSVC 2015 Update 2, x86 build

### Does not compile:
* Mac OS X @ Travis CI, CLANG
  * Missing intrinsic headers...
  
### Tested CPU's:
  * AMD FX-8350,         Max. Feature: AVX1
  * Intel Core i7 6500U, Max. Feature: AVX2
  
### Supported CPU Features
  * Usually the same as those of [Gorynych SIMD toolkit](https://github.com/zz-systems/gorynych)
  * SSE2
  * SSE3
  * SSE3 + SSSE3
  * SSE4
  * SSE4, FMA(3/4) (more tests necessary!)
  * AVX1 is disabled. The emulated int8 vector is still not correct.
  * AVX2
  

## External API
```C++
// returns the last error
const char* solowej_get_error         ();
// creates and / or returns the corresponding engine instance
engine*     solowej_get_engine        (const std::string& instance_key);
// compiles an immediate string
int         solowej_compile_immediate (const char* instance_key, const char* content);
// compiles the contents of a file
int         solowej_compile_file      (const char* instance_key, const char* path);
// invokes the scheduler with the given origin coordinates, fills a float array
int         solowej_run		          (const char*instance_key, float* target, float origin_x, float origin_y, float origin_z);
// invokes the scheduler with the given origin coordinates, fills an int array (same principle as solowej_run, but with round + cast to int)
int         solowej_run_cvti	      (const char*instance_key, int* target,   float origin_x, float origin_y, float origin_z);
```
That's it! (Of course you can use the raw C++ implementation instead of configuring with json and the API, I'll provide the documentation a bit later.)
___
## Configuration (v0.9)
```javascript
{
  "version": "0.9",
  "id": "597C0CF5-D13D-4379-87DF-665C1ADE7B77", // <- unused
  "environment": {
    "max_feature_set": -1, // <- current capabilities are and'ed with this value. -1 = 0xFFFF'FFFF => All bits to 1 => All features active.
    "entryPoint": "output", // <- the module that will serve as the tree root, the output.
    "scheduler": {
      "dimensions": [ 1257, 1, 1257 ], // <- x, y, z.
      "make_seam": true,               // <- makes tiles overlap a bit to hide the seam
      "scale": [ 1, 5, 1 ],            // <- scale the scheduler input
      "offset": [ 0, 0, 0 ],           // <- scheduler input offset
      "use_threads": false             // <- multithreading
    }
  },
  "modules": [
    {
      "name": "generator", // <- UNIQUE identifier
      "type": "generator", // <- module category
      "module": "perlin",  // <- module type
      "settings": { // <- module-specific parameters
        "frequency": 1.0,
        "lacunarity": 2.208984375,
        "persistence": 0.5,
        "octaves": 14,
        "seed": 0
      }
  },
  {
    "name": "output",   // <- UNIQUE identifier
    "type": "modifier", // <- module category
    "module": "curve",  // <- module type
    "source": [ "generator" ], // <- source list. uses the "generator" node as input
    "settings": {  // <- module-specific parameters
      "points": [ 
        {
          "in": -2,
          "out": -1.625
        },
        .
        .
        .
      ]
    }
  }]
}
```
The config compiler can and will emit the following errors for malconfigured files:
* Aggregate error              (You can configure the compiler not to stop on the first error. He tries to continue and collects the errors)
* Invalid module configuration (When the module conditions are not met, e.g a modifier has no source node)
* Unresolved module reference. (When you have a node which references an invalid module (wrong type, name, etc...))
* Unresolved node reference    (A node has an invalid name as a source)
* Duplicate node               (The node names MUST be unique)
* Invalid entry point          (When no entry point is set, otherwise 'Unresolved node reference')
* Cyclic module reference      (Having infinite cycles in a compute tree... have fun)

___

## Available modules 
Type, name are the config fields for a module

### Generators (type = generator)

* Perlin
* Billow
* Voronoi
* Ridged multifractal
* Spheres
* Cylinders
* Checkerboard
* Constant

### modifiers (type = modifier)
* Curve 
* Rotate
* Select
* Terrace 
* Turbulence
* Displace
* Add 
* Sub 
* Mul 
* Div 
* Abs 
* Min 
* Max 
* Clamp
* Blend
* Translate input
* Scale input 
* Scale output
* Scale output biased
