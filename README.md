# ffmpeg-h264-dec

H.264 decoder extracted from FFmpeg

## For JavaScript

With the tool [Emscripten](http://kripken.github.io/emscripten-site/), the C code can also be compiled into JavaScript:

`emmake make js`

(`emmake` comes with Emscripten)

## TS file player

In the folder ./js, run `cmmake make` will build Decoder.js, which is used in a JavaScript TS file player demo: <https://github.com/shengbinmeng/javascript-ts-player>.