# ffmpeg-h264-dec

H.264 decoder extracted from FFmpeg

## Build the decoder

Just run `make`.

## Compile into JavaScript

With the tool [Emscripten](http://kripken.github.io/emscripten-site/), the C code can also be compiled into JavaScript:

`emmake make js`

(`emmake` comes with Emscripten)

## TS file player

In the folder ./js, run `emmake make` will build Decoder.js, which is used in a JavaScript TS file player demo: <https://github.com/shengbinmeng/javascript-ts-player>.

## Future work

1. Compare performance with [Broadway's](https://github.com/mbebenita/Broadway) that is based on Android's software H.264 decoder. No big difference maybe, but the latter only supports baseline profile.

2. Optimize JavaScript code generation, e.g., investigate Emscripten flags, use SIMD.

3. Remove unused code. Current extraction is file based. A lot of code in some files are not used by H.264 decoding. However, changing file content will make it harder to sync with FFmpeg version update.

4. Abstract a better interface and implement a better demo player.
