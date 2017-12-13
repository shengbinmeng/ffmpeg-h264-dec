# ffmpeg-h264-dec

[![Build Status](https://travis-ci.org/shengbinmeng/ffmpeg-h264-dec.svg?branch=master)](https://travis-ci.org/shengbinmeng/ffmpeg-h264-dec)

H.264 decoder extracted from FFmpeg

## Extraction

Files related to H.264 decoding are extracted from [FFmpeg](https://github.com/FFmpeg/FFmpeg) and put in the folder ./ffmpeg-src.

The checkout commit is [2f76157eb05bf63725f96167feda6b2e07501c7e](https://github.com/FFmpeg/FFmpeg/commit/2f76157eb05bf63725f96167feda6b2e07501c7e).

Some files may be modified (a little).

## Build the decoder

Run `make`, and you will get the executable `h264dec`.

## Compile into JavaScript

With the tool [Emscripten](http://kripken.github.io/emscripten-site/), the C code can also be compiled into JavaScript:

- Run `emmake make h264dec.html` to get h264dec.html (and its supporting files), which can be open in a browser.
For Chrome and IE, you will [need to](http://kripken.github.io/emscripten-site/docs/getting_started/Tutorial.html) serve the files using a web server. For Safari, it just works.

- Run `emmake make h264dec.node.js` to get h264dec.node.js, which can be executed by Node.js: `node h264dec.node.js`.

(`emmake` comes with Emscripten; `make clean` is usually needed before building for another platform.)

## Use in JavaScript player

In the folder ./javascript, run `emmake make` to build Decoder.js, which is used in a JavaScript TS file player demo: <https://github.com/shengbinmeng/javascript-ts-player>.

## Future work

1. Compare performance with [Broadway's](https://github.com/mbebenita/Broadway) that is based on Android's software H.264 decoder. No big difference maybe, but the latter only supports baseline profile.

2. Optimize JavaScript code generation, e.g., investigate Emscripten flags, use SIMD.

3. Remove unused code. Current extraction is file based. A lot of code in some files are not used by H.264 decoding. However, changing file content will make it harder to sync with FFmpeg version update.

4. Abstract a better interface and implement a better demo player.
