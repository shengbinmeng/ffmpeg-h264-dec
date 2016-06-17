1. Compare performance with Broadway's that is based on Android's sofeware H.264 decoder. No big difference maybe, but the latter only supports baseline profile.
2. Optimize JavaScript, e.g., investigate Emscripten flags, use SIMD.
3. Remove unused code. Current extraction is file based. A lot of code in some files are not used by h264 decoding. However, changing file content will make it harder to sync with FFmpeg version update.
4. Abstract a better interface and implement a better demo player.

