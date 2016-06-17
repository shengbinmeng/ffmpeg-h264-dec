#ifndef DECODER_H
#define DECODER_H

typedef unsigned char   u8;
typedef signed char     i8;
typedef unsigned short  u16;
typedef signed short    i16;
typedef unsigned int    u32;
typedef signed int      i32;

u32 broadwayInit();
u8* broadwayCreateStream(u32 length);
// When the stream is an already-parsed access unit
void broadwayPlayStream(u32 length);
// When the stream needs parsing
void broadwayParsePlayStream(u32 length);
void broadwayExit();

// Callbacks that user code should provide
extern void broadwayOnHeadersDecoded(); // not use currently
extern void broadwayOnPictureDecoded(u8 *buffer, u32 width, u32 height);

#endif