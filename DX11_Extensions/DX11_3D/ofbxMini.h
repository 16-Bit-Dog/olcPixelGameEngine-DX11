#pragma once

//This is a single header file compilation of https://github.com/nem0/OpenFBX /src 

/* miniz.c v1.16 beta r1 - public domain deflate/inflate, zlib-subset, ZIP reading/writing/appending, PNG writing
   See "unlicense" statement at the end of this file.
   Rich Geldreich <richgel99@gmail.com>, last updated Oct. 13, 2013
   Implements RFC 1950: http://www.ietf.org/rfc/rfc1950.txt and RFC 1951: http://www.ietf.org/rfc/rfc1951.txt

   Most API's defined in miniz.c are optional. For example, to disable the archive related functions just define
   MINIZ_NO_ARCHIVE_APIS, or to get rid of all stdio usage define MINIZ_NO_STDIO (see the list below for more macros).

   * Low-level Deflate/Inflate implementation notes:

	 Compression: Use the "tdefl" API's. The compressor supports raw, static, and dynamic blocks, lazy or
	 greedy parsing, match length filtering, RLE-only, and Huffman-only streams. It performs and compresses
	 approximately as well as zlib.

	 Decompression: Use the "tinfl" API's. The entire decompressor is implemented as a single function
	 coroutine: see tinfl_decompress(). It supports decompression into a 32KB (or larger power of 2) wrapping buffer, or into a memory
	 block large enough to hold the entire file.

	 The low-level tdefl/tinfl API's do not make any use of dynamic memory allocation.

   * zlib-style API notes:

	 miniz.c implements a fairly large subset of zlib. There's enough functionality present for it to be a drop-in
	 zlib replacement in many apps:
		The z_stream struct, optional memory allocation callbacks
		deflateInit/deflateInit2/deflate/deflateReset/deflateEnd/deflateBound
		inflateInit/inflateInit2/inflate/inflateEnd
		compress, compress2, compressBound, uncompress
		CRC-32, Adler-32 - Using modern, minimal code size, CPU cache friendly routines.
		Supports raw deflate streams or standard zlib streams with adler-32 checking.

	 Limitations:
	  The callback API's are not implemented yet. No support for gzip headers or zlib static dictionaries.
	  I've tried to closely emulate zlib's various flavors of stream flushing and return status codes, but
	  there are no guarantees that miniz.c pulls this off perfectly.

   * PNG writing: See the tdefl_write_image_to_png_file_in_memory() function, originally written by
	 Alex Evans. Supports 1-4 bytes/pixel images.

   * ZIP archive API notes:

	 The ZIP archive API's where designed with simplicity and efficiency in mind, with just enough abstraction to
	 get the job done with minimal fuss. There are simple API's to retrieve file information, read files from
	 existing archives, create new archives, append new files to existing archives, or clone archive data from
	 one archive to another. It supports archives located in memory or the heap, on disk (using stdio.h),
	 or you can specify custom file read/write callbacks.

	 - Archive reading: Just call this function to read a single file from a disk archive:

	  void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name,
		size_t *pSize, mz_uint zip_flags);

	 For more complex cases, use the "mz_zip_reader" functions. Upon opening an archive, the entire central
	 directory is located and read as-is into memory, and subsequent file access only occurs when reading individual files.

	 - Archives file scanning: The simple way is to use this function to scan a loaded archive for a specific file:

	 int mz_zip_reader_locate_file(mz_zip_archive *pZip, const char *pName, const char *pComment, mz_uint flags);

	 The locate operation can optionally check file comments too, which (as one example) can be used to identify
	 multiple versions of the same file in an archive. This function uses a simple linear search through the central
	 directory, so it's not very fast.

	 Alternately, you can iterate through all the files in an archive (using mz_zip_reader_get_num_files()) and
	 retrieve detailed info on each file by calling mz_zip_reader_file_stat().

	 - Archive creation: Use the "mz_zip_writer" functions. The ZIP writer immediately writes compressed file data
	 to disk and builds an exact image of the central directory in memory. The central directory image is written
	 all at once at the end of the archive file when the archive is finalized.

	 The archive writer can optionally align each file's local header and file data to any power of 2 alignment,
	 which can be useful when the archive will be read from optical media. Also, the writer supports placing
	 arbitrary data blobs at the very beginning of ZIP archives. Archives written using either feature are still
	 readable by any ZIP tool.

	 - Archive appending: The simple way to add a single file to an archive is to call this function:

	  mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name,
		const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags);

	 The archive will be created if it doesn't already exist, otherwise it'll be appended to.
	 Note the appending is done in-place and is not an atomic operation, so if something goes wrong
	 during the operation it's possible the archive could be left without a central directory (although the local
	 file headers and file data will be fine, so the archive will be recoverable).

	 For more complex archive modification scenarios:
	 1. The safest way is to use a mz_zip_reader to read the existing archive, cloning only those bits you want to
	 preserve into a new archive using using the mz_zip_writer_add_from_zip_reader() function (which compiles the
	 compressed file data as-is). When you're done, delete the old archive and rename the newly written archive, and
	 you're done. This is safe but requires a bunch of temporary disk space or heap memory.

	 2. Or, you can convert an mz_zip_reader in-place to an mz_zip_writer using mz_zip_writer_init_from_reader(),
	 append new files as needed, then finalize the archive which will write an updated central directory to the
	 original archive. (This is basically what mz_zip_add_mem_to_archive_file_in_place() does.) There's a
	 possibility that the archive's central directory could be lost with this method if anything goes wrong, though.

	 - ZIP archive support limitations:
	 No zip64 or spanning support. Extraction functions can only handle unencrypted, stored or deflated files.
	 Requires streams capable of seeking.

   * This is a header file library, like stb_image.c. To get only a header file, either cut and paste the
	 below header, or create miniz.h, #define MINIZ_HEADER_FILE_ONLY, and then include miniz.c from it.

   * Important: For best perf. be sure to customize the below macros for your target platform:
	 #define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
	 #define MINIZ_LITTLE_ENDIAN 1
	 #define MINIZ_HAS_64BIT_REGISTERS 1

   * On platforms using glibc, Be sure to "#define _LARGEFILE64_SOURCE 1" before including miniz.c to ensure miniz
	 uses the 64-bit variants: fopen64(), stat64(), etc. Otherwise you won't be able to process large files
	 (i.e. 32-bit stat() fails for me on files > 0x7FFFFFFF bytes).
*/
#pragma once





/* Defines to completely disable specific portions of miniz.c:
   If all macros here are defined the only functionality remaining will be CRC-32, adler-32, tinfl, and tdefl. */

   /* Define MINIZ_NO_STDIO to disable all usage and any functions which rely on stdio for file I/O. */
   /*#define MINIZ_NO_STDIO */

   /* If MINIZ_NO_TIME is specified then the ZIP archive functions will not be able to get the current time, or */
   /* get/set file times, and the C run-time funcs that get/set times won't be called. */
   /* The current downside is the times written to your archives will be from 1979. */
   /*#define MINIZ_NO_TIME */

   /* Define MINIZ_NO_ARCHIVE_APIS to disable all ZIP archive API's. */
   /*#define MINIZ_NO_ARCHIVE_APIS */

   /* Define MINIZ_NO_ARCHIVE_WRITING_APIS to disable all writing related ZIP archive API's. */
   /*#define MINIZ_NO_ARCHIVE_WRITING_APIS */

   /* Define MINIZ_NO_ZLIB_APIS to remove all ZLIB-style compression/decompression API's. */
   /*#define MINIZ_NO_ZLIB_APIS */

   /* Define MINIZ_NO_ZLIB_COMPATIBLE_NAME to disable zlib names, to prevent conflicts against stock zlib. */
   /*#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES */

   /* Define MINIZ_NO_MALLOC to disable all calls to malloc, free, and realloc.
	  Note if MINIZ_NO_MALLOC is defined then the user must always provide custom user alloc/free/realloc
	  callbacks to the zlib and archive API's, and a few stand-alone helper API's which don't provide custom user
	  functions (such as tdefl_compress_mem_to_heap() and tinfl_decompress_mem_to_heap()) won't work. */
	  /*#define MINIZ_NO_MALLOC */

#if defined(__TINYC__) && (defined(__linux) || defined(__linux__))
/* TODO: Work around "error: include file 'sys\utime.h' when compiling with tcc on Linux */
#define MINIZ_NO_TIME
#endif

#include <stddef.h>

#if !defined(MINIZ_NO_TIME) && !defined(MINIZ_NO_ARCHIVE_APIS)
#include <time.h>
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i486) || defined(i386) || defined(__ia64__) || defined(__x86_64__)
/* MINIZ_X86_OR_X64_CPU is only used to help set the below macros. */
#define MINIZ_X86_OR_X64_CPU 1
#endif

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || MINIZ_X86_OR_X64_CPU
/* Set MINIZ_LITTLE_ENDIAN to 1 if the processor is little endian. */
#define MINIZ_LITTLE_ENDIAN 1
#endif

#if MINIZ_X86_OR_X64_CPU
/* Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES to 1 on CPU's that permit efficient integer loads and stores from unaligned addresses. */
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
#endif

#if defined(_M_X64) || defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__) || defined(__ia64__) || defined(__x86_64__)
/* Set MINIZ_HAS_64BIT_REGISTERS to 1 if operations on 64-bit integers are reasonably fast (and don't involve compiler generated calls to helper functions). */
#define MINIZ_HAS_64BIT_REGISTERS 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/* ------------------- zlib-style API Definitions. */

	/* For more compatibility with zlib, miniz.c uses unsigned long for some parameters/struct members. Beware: mz_ulong can be either 32 or 64-bits! */
	typedef unsigned long mz_ulong;

	/* mz_free() internally uses the MZ_FREE() macro (which by default calls free() unless you've modified the MZ_MALLOC macro) to release a block allocated from the heap. */
	void mz_free(void* p);

#define MZ_ADLER32_INIT (1)
	/* mz_adler32() returns the initial adler-32 value to use when called with ptr==NULL. */
	mz_ulong mz_adler32(mz_ulong adler, const unsigned char* ptr, size_t buf_len);

#define MZ_CRC32_INIT (0)
	/* mz_crc32() returns the initial CRC-32 value to use when called with ptr==NULL. */
	mz_ulong mz_crc32(mz_ulong crc, const unsigned char* ptr, size_t buf_len);

	/* Compression strategies. */
	enum
	{
		MZ_DEFAULT_STRATEGY = 0,
		MZ_FILTERED = 1,
		MZ_HUFFMAN_ONLY = 2,
		MZ_RLE = 3,
		MZ_FIXED = 4
	};

	/* Method */
#define MZ_DEFLATED 8

/* Heap allocation callbacks.
Note that mz_alloc_func parameter types purpsosely differ from zlib's: items/size is size_t, not unsigned long. */
	typedef void* (*mz_alloc_func)(void* opaque, size_t items, size_t size);
	typedef void(*mz_free_func)(void* opaque, void* address);
	typedef void* (*mz_realloc_func)(void* opaque, void* address, size_t items, size_t size);

	/* Compression levels: 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow), MZ_DEFAULT_COMPRESSION=MZ_DEFAULT_LEVEL. */
	enum
	{
		MZ_NO_COMPRESSION = 0,
		MZ_BEST_SPEED = 1,
		MZ_BEST_COMPRESSION = 9,
		MZ_UBER_COMPRESSION = 10,
		MZ_DEFAULT_LEVEL = 6,
		MZ_DEFAULT_COMPRESSION = -1
	};

#define MZ_VERSION "10.0.0"
#define MZ_VERNUM 0xA000
#define MZ_VER_MAJOR 10
#define MZ_VER_MINOR 0
#define MZ_VER_REVISION 0
#define MZ_VER_SUBREVISION 0

#ifndef MINIZ_NO_ZLIB_APIS

	/* Flush values. For typical usage you only need MZ_NO_FLUSH and MZ_FINISH. The other values are for advanced use (refer to the zlib docs). */
	enum
	{
		MZ_NO_FLUSH = 0,
		MZ_PARTIAL_FLUSH = 1,
		MZ_SYNC_FLUSH = 2,
		MZ_FULL_FLUSH = 3,
		MZ_FINISH = 4,
		MZ_BLOCK = 5
	};

	/* Return status codes. MZ_PARAM_ERROR is non-standard. */
	enum
	{
		MZ_OK = 0,
		MZ_STREAM_END = 1,
		MZ_NEED_DICT = 2,
		MZ_ERRNO = -1,
		MZ_STREAM_ERROR = -2,
		MZ_DATA_ERROR = -3,
		MZ_MEM_ERROR = -4,
		MZ_BUF_ERROR = -5,
		MZ_VERSION_ERROR = -6,
		MZ_PARAM_ERROR = -10000
	};

	/* Window bits */
#define MZ_DEFAULT_WINDOW_BITS 15

	struct mz_internal_state;

	/* Compression/decompression stream struct. */
	typedef struct mz_stream_s
	{
		const unsigned char* next_in; /* pointer to next byte to read */
		unsigned int avail_in;        /* number of bytes available at next_in */
		mz_ulong total_in;            /* total number of bytes consumed so far */

		unsigned char* next_out; /* pointer to next byte to write */
		unsigned int avail_out;  /* number of bytes that can be written to next_out */
		mz_ulong total_out;      /* total number of bytes produced so far */

		char* msg;                       /* error msg (unused) */
		struct mz_internal_state* state; /* internal state, allocated by zalloc/zfree */

		mz_alloc_func zalloc; /* optional heap allocation function (defaults to malloc) */
		mz_free_func zfree;   /* optional heap free function (defaults to free) */
		void* opaque;         /* heap alloc function user pointer */

		int data_type;     /* data_type (unused) */
		mz_ulong adler;    /* adler32 of the source or uncompressed data */
		mz_ulong reserved; /* not used */
	} mz_stream;

	typedef mz_stream* mz_streamp;

	/* Returns the version string of miniz.c. */
	const char* mz_version(void);

	/* mz_deflateInit() initializes a compressor with default options: */
	/* Parameters: */
	/*  pStream must point to an initialized mz_stream struct. */
	/*  level must be between [MZ_NO_COMPRESSION, MZ_BEST_COMPRESSION]. */
	/*  level 1 enables a specially optimized compression function that's been optimized purely for performance, not ratio. */
	/*  (This special func. is currently only enabled when MINIZ_USE_UNALIGNED_LOADS_AND_STORES and MINIZ_LITTLE_ENDIAN are defined.) */
	/* Return values: */
	/*  MZ_OK on success. */
	/*  MZ_STREAM_ERROR if the stream is bogus. */
	/*  MZ_PARAM_ERROR if the input parameters are bogus. */
	/*  MZ_MEM_ERROR on out of memory. */
	int mz_deflateInit(mz_streamp pStream, int level);

	/* mz_deflateInit2() is like mz_deflate(), except with more control: */
	/* Additional parameters: */
	/*   method must be MZ_DEFLATED */
	/*   window_bits must be MZ_DEFAULT_WINDOW_BITS (to wrap the deflate stream with zlib header/adler-32 footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate/no header or footer) */
	/*   mem_level must be between [1, 9] (it's checked but ignored by miniz.c) */
	int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy);

	/* Quickly resets a compressor without having to reallocate anything. Same as calling mz_deflateEnd() followed by mz_deflateInit()/mz_deflateInit2(). */
	int mz_deflateReset(mz_streamp pStream);

	/* mz_deflate() compresses the input to output, consuming as much of the input and producing as much output as possible. */
	/* Parameters: */
	/*   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members. */
	/*   flush may be MZ_NO_FLUSH, MZ_PARTIAL_FLUSH/MZ_SYNC_FLUSH, MZ_FULL_FLUSH, or MZ_FINISH. */
	/* Return values: */
	/*   MZ_OK on success (when flushing, or if more input is needed but not available, and/or there's more output to be written but the output buffer is full). */
	/*   MZ_STREAM_END if all input has been consumed and all output bytes have been written. Don't call mz_deflate() on the stream anymore. */
	/*   MZ_STREAM_ERROR if the stream is bogus. */
	/*   MZ_PARAM_ERROR if one of the parameters is invalid. */
	/*   MZ_BUF_ERROR if no forward progress is possible because the input and/or output buffers are empty. (Fill up the input buffer or free up some output space and try again.) */
	int mz_deflate(mz_streamp pStream, int flush);

	/* mz_deflateEnd() deinitializes a compressor: */
	/* Return values: */
	/*  MZ_OK on success. */
	/*  MZ_STREAM_ERROR if the stream is bogus. */
	int mz_deflateEnd(mz_streamp pStream);

	/* mz_deflateBound() returns a (very) conservative upper bound on the amount of data that could be generated by deflate(), assuming flush is set to only MZ_NO_FLUSH or MZ_FINISH. */
	mz_ulong mz_deflateBound(mz_streamp pStream, mz_ulong source_len);

	/* Single-call compression functions mz_compress() and mz_compress2(): */
	/* Returns MZ_OK on success, or one of the error codes from mz_deflate() on failure. */
	int mz_compress(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len);
	int mz_compress2(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len, int level);

	/* mz_compressBound() returns a (very) conservative upper bound on the amount of data that could be generated by calling mz_compress(). */
	mz_ulong mz_compressBound(mz_ulong source_len);

	/* Initializes a decompressor. */
	int mz_inflateInit(mz_streamp pStream);

	/* mz_inflateInit2() is like mz_inflateInit() with an additional option that controls the window size and whether or not the stream has been wrapped with a zlib header/footer: */
	/* window_bits must be MZ_DEFAULT_WINDOW_BITS (to parse zlib header/footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate). */
	int mz_inflateInit2(mz_streamp pStream, int window_bits);

	/* Decompresses the input stream to the output, consuming only as much of the input as needed, and writing as much to the output as possible. */
	/* Parameters: */
	/*   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members. */
	/*   flush may be MZ_NO_FLUSH, MZ_SYNC_FLUSH, or MZ_FINISH. */
	/*   On the first call, if flush is MZ_FINISH it's assumed the input and output buffers are both sized large enough to decompress the entire stream in a single call (this is slightly faster). */
	/*   MZ_FINISH implies that there are no more source bytes available beside what's already in the input buffer, and that the output buffer is large enough to hold the rest of the decompressed data. */
	/* Return values: */
	/*   MZ_OK on success. Either more input is needed but not available, and/or there's more output to be written but the output buffer is full. */
	/*   MZ_STREAM_END if all needed input has been consumed and all output bytes have been written. For zlib streams, the adler-32 of the decompressed data has also been verified. */
	/*   MZ_STREAM_ERROR if the stream is bogus. */
	/*   MZ_DATA_ERROR if the deflate stream is invalid. */
	/*   MZ_PARAM_ERROR if one of the parameters is invalid. */
	/*   MZ_BUF_ERROR if no forward progress is possible because the input buffer is empty but the inflater needs more input to continue, or if the output buffer is not large enough. Call mz_inflate() again */
	/*   with more input data, or with more room in the output buffer (except when using single call decompression, described above). */
	int mz_inflate(mz_streamp pStream, int flush);

	/* Deinitializes a decompressor. */
	int mz_inflateEnd(mz_streamp pStream);

	/* Single-call decompression. */
	/* Returns MZ_OK on success, or one of the error codes from mz_inflate() on failure. */
	int mz_uncompress(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len);

	/* Returns a string description of the specified error code, or NULL if the error code is invalid. */
	const char* mz_error(int err);

	/* Redefine zlib-compatible names to miniz equivalents, so miniz.c can be used as a drop-in replacement for the subset of zlib that miniz.c supports. */
	/* Define MINIZ_NO_ZLIB_COMPATIBLE_NAMES to disable zlib-compatibility if you use zlib in the same project. */
#ifndef MINIZ_NO_ZLIB_COMPATIBLE_NAMES
	typedef unsigned char Byte;
	typedef unsigned int uInt;
	typedef mz_ulong uLong;
	typedef Byte Bytef;
	typedef uInt uIntf;
	typedef char charf;
	typedef int intf;
	typedef void* voidpf;
	typedef uLong uLongf;
	typedef void* voidp;
	typedef void* const voidpc;
#define Z_NULL 0
#define Z_NO_FLUSH MZ_NO_FLUSH
#define Z_PARTIAL_FLUSH MZ_PARTIAL_FLUSH
#define Z_SYNC_FLUSH MZ_SYNC_FLUSH
#define Z_FULL_FLUSH MZ_FULL_FLUSH
#define Z_FINISH MZ_FINISH
#define Z_BLOCK MZ_BLOCK
#define Z_OK MZ_OK
#define Z_STREAM_END MZ_STREAM_END
#define Z_NEED_DICT MZ_NEED_DICT
#define Z_ERRNO MZ_ERRNO
#define Z_STREAM_ERROR MZ_STREAM_ERROR
#define Z_DATA_ERROR MZ_DATA_ERROR
#define Z_MEM_ERROR MZ_MEM_ERROR
#define Z_BUF_ERROR MZ_BUF_ERROR
#define Z_VERSION_ERROR MZ_VERSION_ERROR
#define Z_PARAM_ERROR MZ_PARAM_ERROR
#define Z_NO_COMPRESSION MZ_NO_COMPRESSION
#define Z_BEST_SPEED MZ_BEST_SPEED
#define Z_BEST_COMPRESSION MZ_BEST_COMPRESSION
#define Z_DEFAULT_COMPRESSION MZ_DEFAULT_COMPRESSION
#define Z_DEFAULT_STRATEGY MZ_DEFAULT_STRATEGY
#define Z_FILTERED MZ_FILTERED
#define Z_HUFFMAN_ONLY MZ_HUFFMAN_ONLY
#define Z_RLE MZ_RLE
#define Z_FIXED MZ_FIXED
#define Z_DEFLATED MZ_DEFLATED
#define Z_DEFAULT_WINDOW_BITS MZ_DEFAULT_WINDOW_BITS
#define alloc_func mz_alloc_func
#define free_func mz_free_func
#define internal_state mz_internal_state
#define z_stream mz_stream
#define deflateInit mz_deflateInit
#define deflateInit2 mz_deflateInit2
#define deflateReset mz_deflateReset
#define deflate mz_deflate
#define deflateEnd mz_deflateEnd
#define deflateBound mz_deflateBound
#define compress mz_compress
#define compress2 mz_compress2
#define compressBound mz_compressBound
//#define inflateInit mz_inflateInit
#define inflateInit2 mz_inflateInit2
//#define inflate mz_inflate
//#define inflateEnd mz_inflateEnd
#define uncompress mz_uncompress
#define crc32 mz_crc32
#define adler32 mz_adler32
#define MAX_WBITS 15
#define MAX_MEM_LEVEL 9
#define zError mz_error
#define ZLIB_VERSION MZ_VERSION
#define ZLIB_VERNUM MZ_VERNUM
#define ZLIB_VER_MAJOR MZ_VER_MAJOR
#define ZLIB_VER_MINOR MZ_VER_MINOR
#define ZLIB_VER_REVISION MZ_VER_REVISION
#define ZLIB_VER_SUBREVISION MZ_VER_SUBREVISION
#define zlibVersion mz_version
#define zlib_version mz_version()
#endif /* #ifndef MINIZ_NO_ZLIB_COMPATIBLE_NAMES */

#endif /* MINIZ_NO_ZLIB_APIS */

#ifdef __cplusplus
}
#endif
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ------------------- Types and macros */
typedef unsigned char mz_uint8;
typedef signed short mz_int16;
typedef unsigned short mz_uint16;
typedef unsigned int mz_uint32;
typedef unsigned int mz_uint;
typedef int64_t mz_int64;
typedef uint64_t mz_uint64;
typedef int mz_bool;

#define MZ_FALSE (0)
#define MZ_TRUE (1)

/* Works around MSVC's spammy "warning C4127: conditional expression is constant" message. */
#ifdef _MSC_VER
#define MZ_MACRO_END while (0, 0)
#else
#define MZ_MACRO_END while (0)
#endif

#ifdef MINIZ_NO_STDIO
#define MZ_FILE void *
#else
#include <stdio.h>
#define MZ_FILE FILE
#endif /* #ifdef MINIZ_NO_STDIO */

#ifdef MINIZ_NO_TIME
typedef struct mz_dummy_time_t_tag
{
	int m_dummy;
} mz_dummy_time_t;
#define MZ_TIME_T mz_dummy_time_t
#else
#define MZ_TIME_T time_t
#endif

#define MZ_ASSERT(x) assert(x)

#ifdef MINIZ_NO_MALLOC
#define MZ_MALLOC(x) NULL
#define MZ_FREE(x) (void) x, ((void)0)
#define MZ_REALLOC(p, x) NULL
#else
#define MZ_MALLOC(x) malloc(x)
#define MZ_FREE(x) free(x)
#define MZ_REALLOC(p, x) realloc(p, x)
#endif

#define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MZ_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MZ_CLEAR_OBJ(obj) memset(&(obj), 0, sizeof(obj))

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES &&MINIZ_LITTLE_ENDIAN
#define MZ_READ_LE16(p) *((const mz_uint16 *)(p))
#define MZ_READ_LE32(p) *((const mz_uint32 *)(p))
#else
#define MZ_READ_LE16(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U))
#define MZ_READ_LE32(p) ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U) | ((mz_uint32)(((const mz_uint8 *)(p))[2]) << 16U) | ((mz_uint32)(((const mz_uint8 *)(p))[3]) << 24U))
#endif

#define MZ_READ_LE64(p) (((mz_uint64)MZ_READ_LE32(p)) | (((mz_uint64)MZ_READ_LE32((const mz_uint8 *)(p) + sizeof(mz_uint32))) << 32U))

#ifdef _MSC_VER
#define MZ_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define MZ_FORCEINLINE __inline__ __attribute__((__always_inline__))
#else
#define MZ_FORCEINLINE inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

	extern void* miniz_def_alloc_func(void* opaque, size_t items, size_t size);
	extern void miniz_def_free_func(void* opaque, void* address);
	extern void* miniz_def_realloc_func(void* opaque, void* address, size_t items, size_t size);

#define MZ_UINT16_MAX (0xFFFFU)
#define MZ_UINT32_MAX (0xFFFFFFFFU)

#ifdef __cplusplus
}
#endif
#pragma once


#ifdef __cplusplus
extern "C" {
#endif
	/* ------------------- Low-level Compression API Definitions */

	/* Set TDEFL_LESS_MEMORY to 1 to use less memory (compression will be slightly slower, and raw/dynamic blocks will be output more frequently). */
#define TDEFL_LESS_MEMORY 0

/* tdefl_init() compression flags logically OR'd together (low 12 bits contain the max. number of probes per dictionary search): */
/* TDEFL_DEFAULT_MAX_PROBES: The compressor defaults to 128 dictionary probes per dictionary search. 0=Huffman only, 1=Huffman+LZ (fastest/crap compression), 4095=Huffman+LZ (slowest/best compression). */
	enum
	{
		TDEFL_HUFFMAN_ONLY = 0,
		TDEFL_DEFAULT_MAX_PROBES = 128,
		TDEFL_MAX_PROBES_MASK = 0xFFF
	};

	/* TDEFL_WRITE_ZLIB_HEADER: If set, the compressor outputs a zlib header before the deflate data, and the Adler-32 of the source data at the end. Otherwise, you'll get raw deflate data. */
	/* TDEFL_COMPUTE_ADLER32: Always compute the adler-32 of the input data (even when not writing zlib headers). */
	/* TDEFL_GREEDY_PARSING_FLAG: Set to use faster greedy parsing, instead of more efficient lazy parsing. */
	/* TDEFL_NONDETERMINISTIC_PARSING_FLAG: Enable to decrease the compressor's initialization time to the minimum, but the output may vary from run to run given the same input (depending on the contents of memory). */
	/* TDEFL_RLE_MATCHES: Only look for RLE matches (matches with a distance of 1) */
	/* TDEFL_FILTER_MATCHES: Discards matches <= 5 chars if enabled. */
	/* TDEFL_FORCE_ALL_STATIC_BLOCKS: Disable usage of optimized Huffman tables. */
	/* TDEFL_FORCE_ALL_RAW_BLOCKS: Only use raw (uncompressed) deflate blocks. */
	/* The low 12 bits are reserved to control the max # of hash probes per dictionary lookup (see TDEFL_MAX_PROBES_MASK). */
	enum
	{
		TDEFL_WRITE_ZLIB_HEADER = 0x01000,
		TDEFL_COMPUTE_ADLER32 = 0x02000,
		TDEFL_GREEDY_PARSING_FLAG = 0x04000,
		TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000,
		TDEFL_RLE_MATCHES = 0x10000,
		TDEFL_FILTER_MATCHES = 0x20000,
		TDEFL_FORCE_ALL_STATIC_BLOCKS = 0x40000,
		TDEFL_FORCE_ALL_RAW_BLOCKS = 0x80000
	};

	/* High level compression functions: */
	/* tdefl_compress_mem_to_heap() compresses a block in memory to a heap block allocated via malloc(). */
	/* On entry: */
	/*  pSrc_buf, src_buf_len: Pointer and size of source block to compress. */
	/*  flags: The max match finder probes (default is 128) logically OR'd against the above flags. Higher probes are slower but improve compression. */
	/* On return: */
	/*  Function returns a pointer to the compressed data, or NULL on failure. */
	/*  *pOut_len will be set to the compressed data's size, which could be larger than src_buf_len on uncompressible data. */
	/*  The caller must free() the returned block when it's no longer needed. */
	void* tdefl_compress_mem_to_heap(const void* pSrc_buf, size_t src_buf_len, size_t* pOut_len, int flags);

	/* tdefl_compress_mem_to_mem() compresses a block in memory to another block in memory. */
	/* Returns 0 on failure. */
	size_t tdefl_compress_mem_to_mem(void* pOut_buf, size_t out_buf_len, const void* pSrc_buf, size_t src_buf_len, int flags);

	/* Compresses an image to a compressed PNG file in memory. */
	/* On entry: */
	/*  pImage, w, h, and num_chans describe the image to compress. num_chans may be 1, 2, 3, or 4. */
	/*  The image pitch in bytes per scanline will be w*num_chans. The leftmost pixel on the top scanline is stored first in memory. */
	/*  level may range from [0,10], use MZ_NO_COMPRESSION, MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc. or a decent default is MZ_DEFAULT_LEVEL */
	/*  If flip is true, the image will be flipped on the Y axis (useful for OpenGL apps). */
	/* On return: */
	/*  Function returns a pointer to the compressed data, or NULL on failure. */
	/*  *pLen_out will be set to the size of the PNG image file. */
	/*  The caller must mz_free() the returned heap block (which will typically be larger than *pLen_out) when it's no longer needed. */
	void* tdefl_write_image_to_png_file_in_memory_ex(const void* pImage, int w, int h, int num_chans, size_t* pLen_out, mz_uint level, mz_bool flip);
	void* tdefl_write_image_to_png_file_in_memory(const void* pImage, int w, int h, int num_chans, size_t* pLen_out);

	/* Output stream interface. The compressor uses this interface to write compressed data. It'll typically be called TDEFL_OUT_BUF_SIZE at a time. */
	typedef mz_bool(*tdefl_put_buf_func_ptr)(const void* pBuf, int len, void* pUser);

	/* tdefl_compress_mem_to_output() compresses a block to an output stream. The above helpers use this function internally. */
	mz_bool tdefl_compress_mem_to_output(const void* pBuf, size_t buf_len, tdefl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags);

	enum
	{
		TDEFL_MAX_HUFF_TABLES = 3,
		TDEFL_MAX_HUFF_SYMBOLS_0 = 288,
		TDEFL_MAX_HUFF_SYMBOLS_1 = 32,
		TDEFL_MAX_HUFF_SYMBOLS_2 = 19,
		TDEFL_LZ_DICT_SIZE = 32768,
		TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE - 1,
		TDEFL_MIN_MATCH_LEN = 3,
		TDEFL_MAX_MATCH_LEN = 258
	};

	/* TDEFL_OUT_BUF_SIZE MUST be large enough to hold a single entire compressed output block (using static/fixed Huffman codes). */
#if TDEFL_LESS_MEMORY
	enum
	{
		TDEFL_LZ_CODE_BUF_SIZE = 24 * 1024,
		TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10,
		TDEFL_MAX_HUFF_SYMBOLS = 288,
		TDEFL_LZ_HASH_BITS = 12,
		TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
		TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3,
		TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
	};
#else
	enum
	{
		TDEFL_LZ_CODE_BUF_SIZE = 64 * 1024,
		TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10,
		TDEFL_MAX_HUFF_SYMBOLS = 288,
		TDEFL_LZ_HASH_BITS = 15,
		TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
		TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3,
		TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
	};
#endif

	/* The low-level tdefl functions below may be used directly if the above helper functions aren't flexible enough. The low-level functions don't make any heap allocations, unlike the above helper functions. */
	typedef enum
	{
		TDEFL_STATUS_BAD_PARAM = -2,
		TDEFL_STATUS_PUT_BUF_FAILED = -1,
		TDEFL_STATUS_OKAY = 0,
		TDEFL_STATUS_DONE = 1
	} tdefl_status;

	/* Must map to MZ_NO_FLUSH, MZ_SYNC_FLUSH, etc. enums */
	typedef enum
	{
		TDEFL_NO_FLUSH = 0,
		TDEFL_SYNC_FLUSH = 2,
		TDEFL_FULL_FLUSH = 3,
		TDEFL_FINISH = 4
	} tdefl_flush;

	/* tdefl's compression state structure. */
	typedef struct
	{
		tdefl_put_buf_func_ptr m_pPut_buf_func;
		void* m_pPut_buf_user;
		mz_uint m_flags, m_max_probes[2];
		int m_greedy_parsing;
		mz_uint m_adler32, m_lookahead_pos, m_lookahead_size, m_dict_size;
		mz_uint8* m_pLZ_code_buf, * m_pLZ_flags, * m_pOutput_buf, * m_pOutput_buf_end;
		mz_uint m_num_flags_left, m_total_lz_bytes, m_lz_code_buf_dict_pos, m_bits_in, m_bit_buffer;
		mz_uint m_saved_match_dist, m_saved_match_len, m_saved_lit, m_output_flush_ofs, m_output_flush_remaining, m_finished, m_block_index, m_wants_to_finish;
		tdefl_status m_prev_return_status;
		const void* m_pIn_buf;
		void* m_pOut_buf;
		size_t* m_pIn_buf_size, * m_pOut_buf_size;
		tdefl_flush m_flush;
		const mz_uint8* m_pSrc;
		size_t m_src_buf_left, m_out_buf_ofs;
		mz_uint8 m_dict[TDEFL_LZ_DICT_SIZE + TDEFL_MAX_MATCH_LEN - 1];
		mz_uint16 m_huff_count[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
		mz_uint16 m_huff_codes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
		mz_uint8 m_huff_code_sizes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
		mz_uint8 m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE];
		mz_uint16 m_next[TDEFL_LZ_DICT_SIZE];
		mz_uint16 m_hash[TDEFL_LZ_HASH_SIZE];
		mz_uint8 m_output_buf[TDEFL_OUT_BUF_SIZE];
	} tdefl_compressor;

	/* Initializes the compressor. */
	/* There is no corresponding deinit() function because the tdefl API's do not dynamically allocate memory. */
	/* pBut_buf_func: If NULL, output data will be supplied to the specified callback. In this case, the user should call the tdefl_compress_buffer() API for compression. */
	/* If pBut_buf_func is NULL the user should always call the tdefl_compress() API. */
	/* flags: See the above enums (TDEFL_HUFFMAN_ONLY, TDEFL_WRITE_ZLIB_HEADER, etc.) */
	tdefl_status tdefl_init(tdefl_compressor* d, tdefl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags);

	/* Compresses a block of data, consuming as much of the specified input buffer as possible, and writing as much compressed data to the specified output buffer as possible. */
	tdefl_status tdefl_compress(tdefl_compressor* d, const void* pIn_buf, size_t* pIn_buf_size, void* pOut_buf, size_t* pOut_buf_size, tdefl_flush flush);

	/* tdefl_compress_buffer() is only usable when the tdefl_init() is called with a non-NULL tdefl_put_buf_func_ptr. */
	/* tdefl_compress_buffer() always consumes the entire input buffer. */
	tdefl_status tdefl_compress_buffer(tdefl_compressor* d, const void* pIn_buf, size_t in_buf_size, tdefl_flush flush);

	tdefl_status tdefl_get_prev_return_status(tdefl_compressor* d);
	mz_uint32 tdefl_get_adler32(tdefl_compressor* d);

	/* Create tdefl_compress() flags given zlib-style compression parameters. */
	/* level may range from [0,10] (where 10 is absolute max compression, but may be much slower on some files) */
	/* window_bits may be -15 (raw deflate) or 15 (zlib) */
	/* strategy may be either MZ_DEFAULT_STRATEGY, MZ_FILTERED, MZ_HUFFMAN_ONLY, MZ_RLE, or MZ_FIXED */
	mz_uint tdefl_create_comp_flags_from_zip_params(int level, int window_bits, int strategy);

	/* Allocate the tdefl_compressor structure in C so that */
	/* non-C language bindings to tdefl_ API don't need to worry about */
	/* structure size and allocation mechanism. */
	tdefl_compressor* tdefl_compressor_alloc();
	void tdefl_compressor_free(tdefl_compressor* pComp);

#ifdef __cplusplus
}
#endif
#pragma once

/* ------------------- Low-level Decompression API Definitions */

#ifdef __cplusplus
extern "C" {
#endif
	/* Decompression flags used by tinfl_decompress(). */
	/* TINFL_FLAG_PARSE_ZLIB_HEADER: If set, the input has a valid zlib header and ends with an adler32 checksum (it's a valid zlib stream). Otherwise, the input is a raw deflate stream. */
	/* TINFL_FLAG_HAS_MORE_INPUT: If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input. */
	/* TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF: If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB). */
	/* TINFL_FLAG_COMPUTE_ADLER32: Force adler-32 checksum computation of the decompressed bytes. */
	enum
	{
		TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
		TINFL_FLAG_HAS_MORE_INPUT = 2,
		TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
		TINFL_FLAG_COMPUTE_ADLER32 = 8
	};

	/* High level decompression functions: */
	/* tinfl_decompress_mem_to_heap() decompresses a block in memory to a heap block allocated via malloc(). */
	/* On entry: */
	/*  pSrc_buf, src_buf_len: Pointer and size of the Deflate or zlib source data to decompress. */
	/* On return: */
	/*  Function returns a pointer to the decompressed data, or NULL on failure. */
	/*  *pOut_len will be set to the decompressed data's size, which could be larger than src_buf_len on uncompressible data. */
	/*  The caller must call mz_free() on the returned block when it's no longer needed. */
	void* tinfl_decompress_mem_to_heap(const void* pSrc_buf, size_t src_buf_len, size_t* pOut_len, int flags);

	/* tinfl_decompress_mem_to_mem() decompresses a block in memory to another block in memory. */
	/* Returns TINFL_DECOMPRESS_MEM_TO_MEM_FAILED on failure, or the number of bytes written on success. */
#define TINFL_DECOMPRESS_MEM_TO_MEM_FAILED ((size_t)(-1))
	size_t tinfl_decompress_mem_to_mem(void* pOut_buf, size_t out_buf_len, const void* pSrc_buf, size_t src_buf_len, int flags);

	/* tinfl_decompress_mem_to_callback() decompresses a block in memory to an internal 32KB buffer, and a user provided callback function will be called to flush the buffer. */
	/* Returns 1 on success or 0 on failure. */
	typedef int (*tinfl_put_buf_func_ptr)(const void* pBuf, int len, void* pUser);
	int tinfl_decompress_mem_to_callback(const void* pIn_buf, size_t* pIn_buf_size, tinfl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags);

	struct tinfl_decompressor_tag;
	typedef struct tinfl_decompressor_tag tinfl_decompressor;

	/* Allocate the tinfl_decompressor structure in C so that */
	/* non-C language bindings to tinfl_ API don't need to worry about */
	/* structure size and allocation mechanism. */

	tinfl_decompressor* tinfl_decompressor_alloc();
	void tinfl_decompressor_free(tinfl_decompressor* pDecomp);

	/* Max size of LZ dictionary. */
#define TINFL_LZ_DICT_SIZE 32768

/* Return status. */
	typedef enum
	{
		/* This flags indicates the inflator needs 1 or more input bytes to make forward progress, but the caller is indicating that no more are available. The compressed data */
		/* is probably corrupted. If you call the inflator again with more bytes it'll try to continue processing the input but this is a BAD sign (either the data is corrupted or you called it incorrectly). */
		/* If you call it again with no input you'll just get TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS again. */
		TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS = -4,

		/* This flag indicates that one or more of the input parameters was obviously bogus. (You can try calling it again, but if you get this error the calling code is wrong.) */
		TINFL_STATUS_BAD_PARAM = -3,

		/* This flags indicate the inflator is finished but the adler32 check of the uncompressed data didn't match. If you call it again it'll return TINFL_STATUS_DONE. */
		TINFL_STATUS_ADLER32_MISMATCH = -2,

		/* This flags indicate the inflator has somehow failed (bad code, corrupted input, etc.). If you call it again without resetting via tinfl_init() it it'll just keep on returning the same status failure code. */
		TINFL_STATUS_FAILED = -1,

		/* Any status code less than TINFL_STATUS_DONE must indicate a failure. */

		/* This flag indicates the inflator has returned every byte of uncompressed data that it can, has consumed every byte that it needed, has successfully reached the end of the deflate stream, and */
		/* if zlib headers and adler32 checking enabled that it has successfully checked the uncompressed data's adler32. If you call it again you'll just get TINFL_STATUS_DONE over and over again. */
		TINFL_STATUS_DONE = 0,

		/* This flag indicates the inflator MUST have more input data (even 1 byte) before it can make any more forward progress, or you need to clear the TINFL_FLAG_HAS_MORE_INPUT */
		/* flag on the next call if you don't have any more source data. If the source data was somehow corrupted it's also possible (but unlikely) for the inflator to keep on demanding input to */
		/* proceed, so be sure to properly set the TINFL_FLAG_HAS_MORE_INPUT flag. */
		TINFL_STATUS_NEEDS_MORE_INPUT = 1,

		/* This flag indicates the inflator definitely has 1 or more bytes of uncompressed data available, but it cannot write this data into the output buffer. */
		/* Note if the source compressed data was corrupted it's possible for the inflator to return a lot of uncompressed data to the caller. I've been assuming you know how much uncompressed data to expect */
		/* (either exact or worst case) and will stop calling the inflator and fail after receiving too much. In pure streaming scenarios where you have no idea how many bytes to expect this may not be possible */
		/* so I may need to add some code to address this. */
		TINFL_STATUS_HAS_MORE_OUTPUT = 2
	} tinfl_status;

	/* Initializes the decompressor to its initial state. */
#define tinfl_init(r)     \
    do                    \
    {                     \
        (r)->m_state = 0; \
    }                     \
    MZ_MACRO_END
#define tinfl_get_adler32(r) (r)->m_check_adler32

/* Main low-level decompressor coroutine function. This is the only function actually needed for decompression. All the other functions are just high-level helpers for improved usability. */
/* This is a universal API, i.e. it can be used as a building block to build any desired higher level decompression API. In the limit case, it can be called once per every byte input or output. */
	tinfl_status tinfl_decompress(tinfl_decompressor* r, const mz_uint8* pIn_buf_next, size_t* pIn_buf_size, mz_uint8* pOut_buf_start, mz_uint8* pOut_buf_next, size_t* pOut_buf_size, const mz_uint32 decomp_flags);

	/* Internal/private bits follow. */
	enum
	{
		TINFL_MAX_HUFF_TABLES = 3,
		TINFL_MAX_HUFF_SYMBOLS_0 = 288,
		TINFL_MAX_HUFF_SYMBOLS_1 = 32,
		TINFL_MAX_HUFF_SYMBOLS_2 = 19,
		TINFL_FAST_LOOKUP_BITS = 10,
		TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
	};

	typedef struct
	{
		mz_uint8 m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
		mz_int16 m_look_up[TINFL_FAST_LOOKUP_SIZE], m_tree[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
	} tinfl_huff_table;

#if MINIZ_HAS_64BIT_REGISTERS
#define TINFL_USE_64BIT_BITBUF 1
#endif

#if TINFL_USE_64BIT_BITBUF
	typedef mz_uint64 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (64)
#else
	typedef mz_uint32 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (32)
#endif

	struct tinfl_decompressor_tag
	{
		mz_uint32 m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
		tinfl_bit_buf_t m_bit_buf;
		size_t m_dist_from_out_buf_start;
		tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
		mz_uint8 m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
	};

#ifdef __cplusplus
}
#endif

#pragma once


/* ------------------- ZIP archive reading/writing */

#ifndef MINIZ_NO_ARCHIVE_APIS

#ifdef __cplusplus
extern "C" {
#endif

	enum
	{
		/* Note: These enums can be reduced as needed to save memory or stack space - they are pretty conservative. */
		MZ_ZIP_MAX_IO_BUF_SIZE = 64 * 1024,
		MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE = 512,
		MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE = 512
	};

	typedef struct
	{
		/* Central directory file index. */
		mz_uint32 m_file_index;

		/* Byte offset of this entry in the archive's central directory. Note we currently only support up to UINT_MAX or less bytes in the central dir. */
		mz_uint64 m_central_dir_ofs;

		/* These fields are copied directly from the zip's central dir. */
		mz_uint16 m_version_made_by;
		mz_uint16 m_version_needed;
		mz_uint16 m_bit_flag;
		mz_uint16 m_method;

#ifndef MINIZ_NO_TIME
		MZ_TIME_T m_time;
#endif

		/* CRC-32 of uncompressed data. */
		mz_uint32 m_crc32;

		/* File's compressed size. */
		mz_uint64 m_comp_size;

		/* File's uncompressed size. Note, I've seen some old archives where directory entries had 512 bytes for their uncompressed sizes, but when you try to unpack them you actually get 0 bytes. */
		mz_uint64 m_uncomp_size;

		/* Zip internal and external file attributes. */
		mz_uint16 m_internal_attr;
		mz_uint32 m_external_attr;

		/* Entry's local header file offset in bytes. */
		mz_uint64 m_local_header_ofs;

		/* Size of comment in bytes. */
		mz_uint32 m_comment_size;

		/* MZ_TRUE if the entry appears to be a directory. */
		mz_bool m_is_directory;

		/* MZ_TRUE if the entry uses encryption/strong encryption (which miniz_zip doesn't support) */
		mz_bool m_is_encrypted;

		/* MZ_TRUE if the file is not encrypted, a patch file, and if it uses a compression method we support. */
		mz_bool m_is_supported;

		/* Filename. If string ends in '/' it's a subdirectory entry. */
		/* Guaranteed to be zero terminated, may be truncated to fit. */
		char m_filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

		/* Comment field. */
		/* Guaranteed to be zero terminated, may be truncated to fit. */
		char m_comment[MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE];

	} mz_zip_archive_file_stat;

	typedef size_t(*mz_file_read_func)(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n);
	typedef size_t(*mz_file_write_func)(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n);
	typedef mz_bool(*mz_file_needs_keepalive)(void* pOpaque);

	struct mz_zip_internal_state_tag;
	typedef struct mz_zip_internal_state_tag mz_zip_internal_state;

	typedef enum
	{
		MZ_ZIP_MODE_INVALID = 0,
		MZ_ZIP_MODE_READING = 1,
		MZ_ZIP_MODE_WRITING = 2,
		MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED = 3
	} mz_zip_mode;

	typedef enum
	{
		MZ_ZIP_FLAG_CASE_SENSITIVE = 0x0100,
		MZ_ZIP_FLAG_IGNORE_PATH = 0x0200,
		MZ_ZIP_FLAG_COMPRESSED_DATA = 0x0400,
		MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY = 0x0800,
		MZ_ZIP_FLAG_VALIDATE_LOCATE_FILE_FLAG = 0x1000, /* if enabled, mz_zip_reader_locate_file() will be called on each file as its validated to ensure the func finds the file in the central dir (intended for testing) */
		MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY = 0x2000,     /* validate the local headers, but don't decompress the entire file and check the crc32 */
		MZ_ZIP_FLAG_WRITE_ZIP64 = 0x4000,               /* use the zip64 file format, instead of the original zip file format */
		MZ_ZIP_FLAG_WRITE_ALLOW_READING = 0x8000,
		MZ_ZIP_FLAG_ASCII_FILENAME = 0x10000
	} mz_zip_flags;

	typedef enum
	{
		MZ_ZIP_TYPE_INVALID = 0,
		MZ_ZIP_TYPE_USER,
		MZ_ZIP_TYPE_MEMORY,
		MZ_ZIP_TYPE_HEAP,
		MZ_ZIP_TYPE_FILE,
		MZ_ZIP_TYPE_CFILE,
		MZ_ZIP_TOTAL_TYPES
	} mz_zip_type;

	/* miniz error codes. Be sure to update mz_zip_get_error_string() if you add or modify this enum. */
	typedef enum
	{
		MZ_ZIP_NO_ERROR = 0,
		MZ_ZIP_UNDEFINED_ERROR,
		MZ_ZIP_TOO_MANY_FILES,
		MZ_ZIP_FILE_TOO_LARGE,
		MZ_ZIP_UNSUPPORTED_METHOD,
		MZ_ZIP_UNSUPPORTED_ENCRYPTION,
		MZ_ZIP_UNSUPPORTED_FEATURE,
		MZ_ZIP_FAILED_FINDING_CENTRAL_DIR,
		MZ_ZIP_NOT_AN_ARCHIVE,
		MZ_ZIP_INVALID_HEADER_OR_CORRUPTED,
		MZ_ZIP_UNSUPPORTED_MULTIDISK,
		MZ_ZIP_DECOMPRESSION_FAILED,
		MZ_ZIP_COMPRESSION_FAILED,
		MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE,
		MZ_ZIP_CRC_CHECK_FAILED,
		MZ_ZIP_UNSUPPORTED_CDIR_SIZE,
		MZ_ZIP_ALLOC_FAILED,
		MZ_ZIP_FILE_OPEN_FAILED,
		MZ_ZIP_FILE_CREATE_FAILED,
		MZ_ZIP_FILE_WRITE_FAILED,
		MZ_ZIP_FILE_READ_FAILED,
		MZ_ZIP_FILE_CLOSE_FAILED,
		MZ_ZIP_FILE_SEEK_FAILED,
		MZ_ZIP_FILE_STAT_FAILED,
		MZ_ZIP_INVALID_PARAMETER,
		MZ_ZIP_INVALID_FILENAME,
		MZ_ZIP_BUF_TOO_SMALL,
		MZ_ZIP_INTERNAL_ERROR,
		MZ_ZIP_FILE_NOT_FOUND,
		MZ_ZIP_ARCHIVE_TOO_LARGE,
		MZ_ZIP_VALIDATION_FAILED,
		MZ_ZIP_WRITE_CALLBACK_FAILED,
		MZ_ZIP_TOTAL_ERRORS
	} mz_zip_error;

	typedef struct
	{
		mz_uint64 m_archive_size;
		mz_uint64 m_central_directory_file_ofs;

		/* We only support up to UINT32_MAX files in zip64 mode. */
		mz_uint32 m_total_files;
		mz_zip_mode m_zip_mode;
		mz_zip_type m_zip_type;
		mz_zip_error m_last_error;

		mz_uint64 m_file_offset_alignment;

		mz_alloc_func m_pAlloc;
		mz_free_func m_pFree;
		mz_realloc_func m_pRealloc;
		void* m_pAlloc_opaque;

		mz_file_read_func m_pRead;
		mz_file_write_func m_pWrite;
		mz_file_needs_keepalive m_pNeeds_keepalive;
		void* m_pIO_opaque;

		mz_zip_internal_state* m_pState;

	} mz_zip_archive;

	/* -------- ZIP reading */

	/* Inits a ZIP archive reader. */
	/* These functions read and validate the archive's central directory. */
	mz_bool mz_zip_reader_init(mz_zip_archive* pZip, mz_uint64 size, mz_uint flags);

	mz_bool mz_zip_reader_init_mem(mz_zip_archive* pZip, const void* pMem, size_t size, mz_uint flags);

#ifndef MINIZ_NO_STDIO
	/* Read a archive from a disk file. */
	/* file_start_ofs is the file offset where the archive actually begins, or 0. */
	/* actual_archive_size is the true total size of the archive, which may be smaller than the file's actual size on disk. If zero the entire file is treated as the archive. */
	mz_bool mz_zip_reader_init_file(mz_zip_archive* pZip, const char* pFilename, mz_uint32 flags);
	mz_bool mz_zip_reader_init_file_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint flags, mz_uint64 file_start_ofs, mz_uint64 archive_size);

	/* Read an archive from an already opened FILE, beginning at the current file position. */
	/* The archive is assumed to be archive_size bytes long. If archive_size is < 0, then the entire rest of the file is assumed to contain the archive. */
	/* The FILE will NOT be closed when mz_zip_reader_end() is called. */
	mz_bool mz_zip_reader_init_cfile(mz_zip_archive* pZip, MZ_FILE* pFile, mz_uint64 archive_size, mz_uint flags);
#endif

	/* Ends archive reading, freeing all allocations, and closing the input archive file if mz_zip_reader_init_file() was used. */
	mz_bool mz_zip_reader_end(mz_zip_archive* pZip);

	/* -------- ZIP reading or writing */

	/* Clears a mz_zip_archive struct to all zeros. */
	/* Important: This must be done before passing the struct to any mz_zip functions. */
	void mz_zip_zero_struct(mz_zip_archive* pZip);

	mz_zip_mode mz_zip_get_mode(mz_zip_archive* pZip);
	mz_zip_type mz_zip_get_type(mz_zip_archive* pZip);

	/* Returns the total number of files in the archive. */
	mz_uint mz_zip_reader_get_num_files(mz_zip_archive* pZip);

	mz_uint64 mz_zip_get_archive_size(mz_zip_archive* pZip);
	mz_uint64 mz_zip_get_archive_file_start_offset(mz_zip_archive* pZip);
	MZ_FILE* mz_zip_get_cfile(mz_zip_archive* pZip);

	/* Reads n bytes of raw archive data, starting at file offset file_ofs, to pBuf. */
	size_t mz_zip_read_archive_data(mz_zip_archive* pZip, mz_uint64 file_ofs, void* pBuf, size_t n);

	/* Attempts to locates a file in the archive's central directory. */
	/* Valid flags: MZ_ZIP_FLAG_CASE_SENSITIVE, MZ_ZIP_FLAG_IGNORE_PATH */
	/* Returns -1 if the file cannot be found. */
	int mz_zip_locate_file(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags);
	/* Returns MZ_FALSE if the file cannot be found. */
	mz_bool mz_zip_locate_file_v2(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags, mz_uint32* pIndex);

	/* All mz_zip funcs set the m_last_error field in the mz_zip_archive struct. These functions retrieve/manipulate this field. */
	/* Note that the m_last_error functionality is not thread safe. */
	mz_zip_error mz_zip_set_last_error(mz_zip_archive* pZip, mz_zip_error err_num);
	mz_zip_error mz_zip_peek_last_error(mz_zip_archive* pZip);
	mz_zip_error mz_zip_clear_last_error(mz_zip_archive* pZip);
	mz_zip_error mz_zip_get_last_error(mz_zip_archive* pZip);
	const char* mz_zip_get_error_string(mz_zip_error mz_err);

	/* MZ_TRUE if the archive file entry is a directory entry. */
	mz_bool mz_zip_reader_is_file_a_directory(mz_zip_archive* pZip, mz_uint file_index);

	/* MZ_TRUE if the file is encrypted/strong encrypted. */
	mz_bool mz_zip_reader_is_file_encrypted(mz_zip_archive* pZip, mz_uint file_index);

	/* MZ_TRUE if the compression method is supported, and the file is not encrypted, and the file is not a compressed patch file. */
	mz_bool mz_zip_reader_is_file_supported(mz_zip_archive* pZip, mz_uint file_index);

	/* Retrieves the filename of an archive file entry. */
	/* Returns the number of bytes written to pFilename, or if filename_buf_size is 0 this function returns the number of bytes needed to fully store the filename. */
	mz_uint mz_zip_reader_get_filename(mz_zip_archive* pZip, mz_uint file_index, char* pFilename, mz_uint filename_buf_size);

	/* Attempts to locates a file in the archive's central directory. */
	/* Valid flags: MZ_ZIP_FLAG_CASE_SENSITIVE, MZ_ZIP_FLAG_IGNORE_PATH */
	/* Returns -1 if the file cannot be found. */
	int mz_zip_reader_locate_file(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags);
	int mz_zip_reader_locate_file_v2(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags, mz_uint32* file_index);

	/* Returns detailed information about an archive file entry. */
	mz_bool mz_zip_reader_file_stat(mz_zip_archive* pZip, mz_uint file_index, mz_zip_archive_file_stat* pStat);

	/* MZ_TRUE if the file is in zip64 format. */
	/* A file is considered zip64 if it contained a zip64 end of central directory marker, or if it contained any zip64 extended file information fields in the central directory. */
	mz_bool mz_zip_is_zip64(mz_zip_archive* pZip);

	/* Returns the total central directory size in bytes. */
	/* The current max supported size is <= MZ_UINT32_MAX. */
	size_t mz_zip_get_central_dir_size(mz_zip_archive* pZip);

	/* Extracts a archive file to a memory buffer using no memory allocation. */
	/* There must be at least enough room on the stack to store the inflator's state (~34KB or so). */
	mz_bool mz_zip_reader_extract_to_mem_no_alloc(mz_zip_archive* pZip, mz_uint file_index, void* pBuf, size_t buf_size, mz_uint flags, void* pUser_read_buf, size_t user_read_buf_size);
	mz_bool mz_zip_reader_extract_file_to_mem_no_alloc(mz_zip_archive* pZip, const char* pFilename, void* pBuf, size_t buf_size, mz_uint flags, void* pUser_read_buf, size_t user_read_buf_size);

	/* Extracts a archive file to a memory buffer. */
	mz_bool mz_zip_reader_extract_to_mem(mz_zip_archive* pZip, mz_uint file_index, void* pBuf, size_t buf_size, mz_uint flags);
	mz_bool mz_zip_reader_extract_file_to_mem(mz_zip_archive* pZip, const char* pFilename, void* pBuf, size_t buf_size, mz_uint flags);

	/* Extracts a archive file to a dynamically allocated heap buffer. */
	/* The memory will be allocated via the mz_zip_archive's alloc/realloc functions. */
	/* Returns NULL and sets the last error on failure. */
	void* mz_zip_reader_extract_to_heap(mz_zip_archive* pZip, mz_uint file_index, size_t* pSize, mz_uint flags);
	void* mz_zip_reader_extract_file_to_heap(mz_zip_archive* pZip, const char* pFilename, size_t* pSize, mz_uint flags);

	/* Extracts a archive file using a callback function to output the file's data. */
	mz_bool mz_zip_reader_extract_to_callback(mz_zip_archive* pZip, mz_uint file_index, mz_file_write_func pCallback, void* pOpaque, mz_uint flags);
	mz_bool mz_zip_reader_extract_file_to_callback(mz_zip_archive* pZip, const char* pFilename, mz_file_write_func pCallback, void* pOpaque, mz_uint flags);

#ifndef MINIZ_NO_STDIO
	/* Extracts a archive file to a disk file and sets its last accessed and modified times. */
	/* This function only extracts files, not archive directory records. */
	mz_bool mz_zip_reader_extract_to_file(mz_zip_archive* pZip, mz_uint file_index, const char* pDst_filename, mz_uint flags);
	mz_bool mz_zip_reader_extract_file_to_file(mz_zip_archive* pZip, const char* pArchive_filename, const char* pDst_filename, mz_uint flags);

	/* Extracts a archive file starting at the current position in the destination FILE stream. */
	mz_bool mz_zip_reader_extract_to_cfile(mz_zip_archive* pZip, mz_uint file_index, MZ_FILE* File, mz_uint flags);
	mz_bool mz_zip_reader_extract_file_to_cfile(mz_zip_archive* pZip, const char* pArchive_filename, MZ_FILE* pFile, mz_uint flags);
#endif

#if 0
	/* TODO */
	typedef void* mz_zip_streaming_extract_state_ptr;
	mz_zip_streaming_extract_state_ptr mz_zip_streaming_extract_begin(mz_zip_archive* pZip, mz_uint file_index, mz_uint flags);
	uint64_t mz_zip_streaming_extract_get_size(mz_zip_archive* pZip, mz_zip_streaming_extract_state_ptr pState);
	uint64_t mz_zip_streaming_extract_get_cur_ofs(mz_zip_archive* pZip, mz_zip_streaming_extract_state_ptr pState);
	mz_bool mz_zip_streaming_extract_seek(mz_zip_archive* pZip, mz_zip_streaming_extract_state_ptr pState, uint64_t new_ofs);
	size_t mz_zip_streaming_extract_read(mz_zip_archive* pZip, mz_zip_streaming_extract_state_ptr pState, void* pBuf, size_t buf_size);
	mz_bool mz_zip_streaming_extract_end(mz_zip_archive* pZip, mz_zip_streaming_extract_state_ptr pState);
#endif

	/* This function compares the archive's local headers, the optional local zip64 extended information block, and the optional descriptor following the compressed data vs. the data in the central directory. */
	/* It also validates that each file can be successfully uncompressed unless the MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY is specified. */
	mz_bool mz_zip_validate_file(mz_zip_archive* pZip, mz_uint file_index, mz_uint flags);

	/* Validates an entire archive by calling mz_zip_validate_file() on each file. */
	mz_bool mz_zip_validate_archive(mz_zip_archive* pZip, mz_uint flags);

	/* Misc utils/helpers, valid for ZIP reading or writing */
	mz_bool mz_zip_validate_mem_archive(const void* pMem, size_t size, mz_uint flags, mz_zip_error* pErr);
	mz_bool mz_zip_validate_file_archive(const char* pFilename, mz_uint flags, mz_zip_error* pErr);

	/* Universal end function - calls either mz_zip_reader_end() or mz_zip_writer_end(). */
	mz_bool mz_zip_end(mz_zip_archive* pZip);

	/* -------- ZIP writing */

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

/* Inits a ZIP archive writer. */
	mz_bool mz_zip_writer_init(mz_zip_archive* pZip, mz_uint64 existing_size);
	mz_bool mz_zip_writer_init_v2(mz_zip_archive* pZip, mz_uint64 existing_size, mz_uint flags);
	mz_bool mz_zip_writer_init_heap(mz_zip_archive* pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size);
	mz_bool mz_zip_writer_init_heap_v2(mz_zip_archive* pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size, mz_uint flags);

#ifndef MINIZ_NO_STDIO
	mz_bool mz_zip_writer_init_file(mz_zip_archive* pZip, const char* pFilename, mz_uint64 size_to_reserve_at_beginning);
	mz_bool mz_zip_writer_init_file_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint64 size_to_reserve_at_beginning, mz_uint flags);
	mz_bool mz_zip_writer_init_cfile(mz_zip_archive* pZip, MZ_FILE* pFile, mz_uint flags);
#endif

	/* Converts a ZIP archive reader object into a writer object, to allow efficient in-place file appends to occur on an existing archive. */
	/* For archives opened using mz_zip_reader_init_file, pFilename must be the archive's filename so it can be reopened for writing. If the file can't be reopened, mz_zip_reader_end() will be called. */
	/* For archives opened using mz_zip_reader_init_mem, the memory block must be growable using the realloc callback (which defaults to realloc unless you've overridden it). */
	/* Finally, for archives opened using mz_zip_reader_init, the mz_zip_archive's user provided m_pWrite function cannot be NULL. */
	/* Note: In-place archive modification is not recommended unless you know what you're doing, because if execution stops or something goes wrong before */
	/* the archive is finalized the file's central directory will be hosed. */
	mz_bool mz_zip_writer_init_from_reader(mz_zip_archive* pZip, const char* pFilename);
	mz_bool mz_zip_writer_init_from_reader_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint flags);

	/* Adds the contents of a memory buffer to an archive. These functions record the current local time into the archive. */
	/* To add a directory entry, call this method with an archive name ending in a forwardslash with an empty buffer. */
	/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
	mz_bool mz_zip_writer_add_mem(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, mz_uint level_and_flags);

	/* Like mz_zip_writer_add_mem(), except you can specify a file comment field, and optionally supply the function with already compressed data. */
	/* uncomp_size/uncomp_crc32 are only used if the MZ_ZIP_FLAG_COMPRESSED_DATA flag is specified. */
	mz_bool mz_zip_writer_add_mem_ex(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags,
		mz_uint64 uncomp_size, mz_uint32 uncomp_crc32);

	mz_bool mz_zip_writer_add_mem_ex_v2(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags,
		mz_uint64 uncomp_size, mz_uint32 uncomp_crc32, MZ_TIME_T* last_modified, const char* user_extra_data_local, mz_uint user_extra_data_local_len,
		const char* user_extra_data_central, mz_uint user_extra_data_central_len);

#ifndef MINIZ_NO_STDIO
	/* Adds the contents of a disk file to an archive. This function also records the disk file's modified time into the archive. */
	/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
	mz_bool mz_zip_writer_add_file(mz_zip_archive* pZip, const char* pArchive_name, const char* pSrc_filename, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags);

	/* Like mz_zip_writer_add_file(), except the file data is read from the specified FILE stream. */
	mz_bool mz_zip_writer_add_cfile(mz_zip_archive* pZip, const char* pArchive_name, MZ_FILE* pSrc_file, mz_uint64 size_to_add,
		const MZ_TIME_T* pFile_time, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags, const char* user_extra_data_local, mz_uint user_extra_data_local_len,
		const char* user_extra_data_central, mz_uint user_extra_data_central_len);
#endif

	/* Adds a file to an archive by fully cloning the data from another archive. */
	/* This function fully clones the source file's compressed data (no recompression), along with its full filename, extra data (it may add or modify the zip64 local header extra data field), and the optional descriptor following the compressed data. */
	mz_bool mz_zip_writer_add_from_zip_reader(mz_zip_archive* pZip, mz_zip_archive* pSource_zip, mz_uint src_file_index);

	/* Finalizes the archive by writing the central directory records followed by the end of central directory record. */
	/* After an archive is finalized, the only valid call on the mz_zip_archive struct is mz_zip_writer_end(). */
	/* An archive must be manually finalized by calling this function for it to be valid. */
	mz_bool mz_zip_writer_finalize_archive(mz_zip_archive* pZip);

	/* Finalizes a heap archive, returning a poiner to the heap block and its size. */
	/* The heap block will be allocated using the mz_zip_archive's alloc/realloc callbacks. */
	mz_bool mz_zip_writer_finalize_heap_archive(mz_zip_archive* pZip, void** ppBuf, size_t* pSize);

	/* Ends archive writing, freeing all allocations, and closing the output file if mz_zip_writer_init_file() was used. */
	/* Note for the archive to be valid, it *must* have been finalized before ending (this function will not do it for you). */
	mz_bool mz_zip_writer_end(mz_zip_archive* pZip);

	/* -------- Misc. high-level helper functions: */

	/* mz_zip_add_mem_to_archive_file_in_place() efficiently (but not atomically) appends a memory blob to a ZIP archive. */
	/* Note this is NOT a fully safe operation. If it crashes or dies in some way your archive can be left in a screwed up state (without a central directory). */
	/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
	/* TODO: Perhaps add an option to leave the existing central dir in place in case the add dies? We could then truncate the file (so the old central dir would be at the end) if something goes wrong. */
	mz_bool mz_zip_add_mem_to_archive_file_in_place(const char* pZip_filename, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags);
	mz_bool mz_zip_add_mem_to_archive_file_in_place_v2(const char* pZip_filename, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags, mz_zip_error* pErr);

	/* Reads a single file from an archive into a heap block. */
	/* If pComment is not NULL, only the file with the specified comment will be extracted. */
	/* Returns NULL on failure. */
	void* mz_zip_extract_archive_file_to_heap(const char* pZip_filename, const char* pArchive_name, size_t* pSize, mz_uint flags);
	void* mz_zip_extract_archive_file_to_heap_v2(const char* pZip_filename, const char* pArchive_name, const char* pComment, size_t* pSize, mz_uint flags, mz_zip_error* pErr);

#endif /* #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS */

#ifdef __cplusplus
}
#endif

#endif /* MINIZ_NO_ARCHIVE_APIS */

/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


typedef unsigned char mz_validate_uint16[sizeof(mz_uint16) == 2 ? 1 : -1];
typedef unsigned char mz_validate_uint32[sizeof(mz_uint32) == 4 ? 1 : -1];
typedef unsigned char mz_validate_uint64[sizeof(mz_uint64) == 8 ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif

    /* ------------------- zlib-style API's */

    mz_ulong mz_adler32(mz_ulong adler, const unsigned char* ptr, size_t buf_len)
    {
        mz_uint32 i, s1 = (mz_uint32)(adler & 0xffff), s2 = (mz_uint32)(adler >> 16);
        size_t block_len = buf_len % 5552;
        if (!ptr)
            return MZ_ADLER32_INIT;
        while (buf_len)
        {
            for (i = 0; i + 7 < block_len; i += 8, ptr += 8)
            {
                s1 += ptr[0], s2 += s1;
                s1 += ptr[1], s2 += s1;
                s1 += ptr[2], s2 += s1;
                s1 += ptr[3], s2 += s1;
                s1 += ptr[4], s2 += s1;
                s1 += ptr[5], s2 += s1;
                s1 += ptr[6], s2 += s1;
                s1 += ptr[7], s2 += s1;
            }
            for (; i < block_len; ++i)
                s1 += *ptr++, s2 += s1;
            s1 %= 65521U, s2 %= 65521U;
            buf_len -= block_len;
            block_len = 5552;
        }
        return (s2 << 16) + s1;
    }

    /* Karl Malbrain's compact CRC-32. See "A compact CCITT crc16 and crc32 C implementation that balances processor cache usage against speed": http://www.geocities.com/malbrain/ */
#if 0
    mz_ulong mz_crc32(mz_ulong crc, const mz_uint8* ptr, size_t buf_len)
    {
        static const mz_uint32 s_crc32[16] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
                                               0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
        mz_uint32 crcu32 = (mz_uint32)crc;
        if (!ptr)
            return MZ_CRC32_INIT;
        crcu32 = ~crcu32;
        while (buf_len--)
        {
            mz_uint8 b = *ptr++;
            crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)];
            crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)];
        }
        return ~crcu32;
    }
#else
/* Faster, but larger CPU cache footprint.
 */
    mz_ulong mz_crc32(mz_ulong crc, const mz_uint8* ptr, size_t buf_len)
    {
        static const mz_uint32 s_crc_table[256] =
        {
            0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535,
            0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD,
            0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D,
            0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
            0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4,
            0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
            0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC,
            0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
            0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB,
            0xB6662D3D, 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F,
            0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB,
            0x086D3D2D, 0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
            0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA,
            0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 0x4DB26158, 0x3AB551CE,
            0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A,
            0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
            0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409,
            0xCE61E49F, 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
            0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739,
            0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
            0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344, 0x8708A3D2, 0x1E01F268,
            0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0,
            0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8,
            0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
            0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF,
            0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703,
            0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7,
            0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
            0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE,
            0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
            0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6,
            0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
            0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D,
            0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5,
            0x47B2CF7F, 0x30B5FFE9, 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605,
            0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
            0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
        };

        mz_uint32 crc32 = (mz_uint32)crc ^ 0xFFFFFFFF;
        const mz_uint8* pByte_buf = (const mz_uint8*)ptr;

        while (buf_len >= 4)
        {
            crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[0]) & 0xFF];
            crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[1]) & 0xFF];
            crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[2]) & 0xFF];
            crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[3]) & 0xFF];
            pByte_buf += 4;
            buf_len -= 4;
        }

        while (buf_len)
        {
            crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[0]) & 0xFF];
            ++pByte_buf;
            --buf_len;
        }

        return ~crc32;
    }
#endif

    void mz_free(void* p)
    {
        MZ_FREE(p);
    }

    void* miniz_def_alloc_func(void* opaque, size_t items, size_t size)
    {
        (void)opaque, (void)items, (void)size;
        return MZ_MALLOC(items * size);
    }
    void miniz_def_free_func(void* opaque, void* address)
    {
        (void)opaque, (void)address;
        MZ_FREE(address);
    }
    void* miniz_def_realloc_func(void* opaque, void* address, size_t items, size_t size)
    {
        (void)opaque, (void)address, (void)items, (void)size;
        return MZ_REALLOC(address, items * size);
    }

    const char* mz_version(void)
    {
        return MZ_VERSION;
    }

#ifndef MINIZ_NO_ZLIB_APIS

    int mz_deflateInit(mz_streamp pStream, int level)
    {
        return mz_deflateInit2(pStream, level, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
    }

    int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy)
    {
        tdefl_compressor* pComp;
        mz_uint comp_flags = TDEFL_COMPUTE_ADLER32 | tdefl_create_comp_flags_from_zip_params(level, window_bits, strategy);

        if (!pStream)
            return MZ_STREAM_ERROR;
        if ((method != MZ_DEFLATED) || ((mem_level < 1) || (mem_level > 9)) || ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS)))
            return MZ_PARAM_ERROR;

        pStream->data_type = 0;
        pStream->adler = MZ_ADLER32_INIT;
        pStream->msg = NULL;
        pStream->reserved = 0;
        pStream->total_in = 0;
        pStream->total_out = 0;
        if (!pStream->zalloc)
            pStream->zalloc = miniz_def_alloc_func;
        if (!pStream->zfree)
            pStream->zfree = miniz_def_free_func;

        pComp = (tdefl_compressor*)pStream->zalloc(pStream->opaque, 1, sizeof(tdefl_compressor));
        if (!pComp)
            return MZ_MEM_ERROR;

        pStream->state = (struct mz_internal_state*)pComp;

        if (tdefl_init(pComp, NULL, NULL, comp_flags) != TDEFL_STATUS_OKAY)
        {
            mz_deflateEnd(pStream);
            return MZ_PARAM_ERROR;
        }

        return MZ_OK;
    }

    int mz_deflateReset(mz_streamp pStream)
    {
        if ((!pStream) || (!pStream->state) || (!pStream->zalloc) || (!pStream->zfree))
            return MZ_STREAM_ERROR;
        pStream->total_in = pStream->total_out = 0;
        tdefl_init((tdefl_compressor*)pStream->state, NULL, NULL, ((tdefl_compressor*)pStream->state)->m_flags);
        return MZ_OK;
    }

    int mz_deflate(mz_streamp pStream, int flush)
    {
        size_t in_bytes, out_bytes;
        mz_ulong orig_total_in, orig_total_out;
        int mz_status = MZ_OK;

        if ((!pStream) || (!pStream->state) || (flush < 0) || (flush > MZ_FINISH) || (!pStream->next_out))
            return MZ_STREAM_ERROR;
        if (!pStream->avail_out)
            return MZ_BUF_ERROR;

        if (flush == MZ_PARTIAL_FLUSH)
            flush = MZ_SYNC_FLUSH;

        if (((tdefl_compressor*)pStream->state)->m_prev_return_status == TDEFL_STATUS_DONE)
            return (flush == MZ_FINISH) ? MZ_STREAM_END : MZ_BUF_ERROR;

        orig_total_in = pStream->total_in;
        orig_total_out = pStream->total_out;
        for (;;)
        {
            tdefl_status defl_status;
            in_bytes = pStream->avail_in;
            out_bytes = pStream->avail_out;

            defl_status = tdefl_compress((tdefl_compressor*)pStream->state, pStream->next_in, &in_bytes, pStream->next_out, &out_bytes, (tdefl_flush)flush);
            pStream->next_in += (mz_uint)in_bytes;
            pStream->avail_in -= (mz_uint)in_bytes;
            pStream->total_in += (mz_uint)in_bytes;
            pStream->adler = tdefl_get_adler32((tdefl_compressor*)pStream->state);

            pStream->next_out += (mz_uint)out_bytes;
            pStream->avail_out -= (mz_uint)out_bytes;
            pStream->total_out += (mz_uint)out_bytes;

            if (defl_status < 0)
            {
                mz_status = MZ_STREAM_ERROR;
                break;
            }
            else if (defl_status == TDEFL_STATUS_DONE)
            {
                mz_status = MZ_STREAM_END;
                break;
            }
            else if (!pStream->avail_out)
                break;
            else if ((!pStream->avail_in) && (flush != MZ_FINISH))
            {
                if ((flush) || (pStream->total_in != orig_total_in) || (pStream->total_out != orig_total_out))
                    break;
                return MZ_BUF_ERROR; /* Can't make forward progress without some input.
     */
            }
        }
        return mz_status;
    }

    int mz_deflateEnd(mz_streamp pStream)
    {
        if (!pStream)
            return MZ_STREAM_ERROR;
        if (pStream->state)
        {
            pStream->zfree(pStream->opaque, pStream->state);
            pStream->state = NULL;
        }
        return MZ_OK;
    }

    mz_ulong mz_deflateBound(mz_streamp pStream, mz_ulong source_len)
    {
        (void)pStream;
        /* This is really over conservative. (And lame, but it's actually pretty tricky to compute a true upper bound given the way tdefl's blocking works.) */
        return MZ_MAX(128 + (source_len * 110) / 100, 128 + source_len + ((source_len / (31 * 1024)) + 1) * 5);
    }

    int mz_compress2(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len, int level)
    {
        int status;
        mz_stream stream;
        memset(&stream, 0, sizeof(stream));

        /* In case mz_ulong is 64-bits (argh I hate longs). */
        if ((source_len | *pDest_len) > 0xFFFFFFFFU)
            return MZ_PARAM_ERROR;

        stream.next_in = pSource;
        stream.avail_in = (mz_uint32)source_len;
        stream.next_out = pDest;
        stream.avail_out = (mz_uint32)*pDest_len;

        status = mz_deflateInit(&stream, level);
        if (status != MZ_OK)
            return status;

        status = mz_deflate(&stream, MZ_FINISH);
        if (status != MZ_STREAM_END)
        {
            mz_deflateEnd(&stream);
            return (status == MZ_OK) ? MZ_BUF_ERROR : status;
        }

        *pDest_len = stream.total_out;
        return mz_deflateEnd(&stream);
    }

    int mz_compress(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len)
    {
        return mz_compress2(pDest, pDest_len, pSource, source_len, MZ_DEFAULT_COMPRESSION);
    }

    mz_ulong mz_compressBound(mz_ulong source_len)
    {
        return mz_deflateBound(NULL, source_len);
    }

    typedef struct
    {
        tinfl_decompressor m_decomp;
        mz_uint m_dict_ofs, m_dict_avail, m_first_call, m_has_flushed;
        int m_window_bits;
        mz_uint8 m_dict[TINFL_LZ_DICT_SIZE];
        tinfl_status m_last_status;
    } inflate_state;

    int mz_inflateInit2(mz_streamp pStream, int window_bits)
    {
        inflate_state* pDecomp;
        if (!pStream)
            return MZ_STREAM_ERROR;
        if ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS))
            return MZ_PARAM_ERROR;

        pStream->data_type = 0;
        pStream->adler = 0;
        pStream->msg = NULL;
        pStream->total_in = 0;
        pStream->total_out = 0;
        pStream->reserved = 0;
        if (!pStream->zalloc)
            pStream->zalloc = miniz_def_alloc_func;
        if (!pStream->zfree)
            pStream->zfree = miniz_def_free_func;

        pDecomp = (inflate_state*)pStream->zalloc(pStream->opaque, 1, sizeof(inflate_state));
        if (!pDecomp)
            return MZ_MEM_ERROR;

        pStream->state = (struct mz_internal_state*)pDecomp;

        tinfl_init(&pDecomp->m_decomp);
        pDecomp->m_dict_ofs = 0;
        pDecomp->m_dict_avail = 0;
        pDecomp->m_last_status = TINFL_STATUS_NEEDS_MORE_INPUT;
        pDecomp->m_first_call = 1;
        pDecomp->m_has_flushed = 0;
        pDecomp->m_window_bits = window_bits;

        return MZ_OK;
    }

    int mz_inflateInit(mz_streamp pStream)
    {
        return mz_inflateInit2(pStream, MZ_DEFAULT_WINDOW_BITS);
    }

    int mz_inflate(mz_streamp pStream, int flush)
    {
        inflate_state* pState;
        mz_uint n, first_call, decomp_flags = TINFL_FLAG_COMPUTE_ADLER32;
        size_t in_bytes, out_bytes, orig_avail_in;
        tinfl_status status;

        if ((!pStream) || (!pStream->state))
            return MZ_STREAM_ERROR;
        if (flush == MZ_PARTIAL_FLUSH)
            flush = MZ_SYNC_FLUSH;
        if ((flush) && (flush != MZ_SYNC_FLUSH) && (flush != MZ_FINISH))
            return MZ_STREAM_ERROR;

        pState = (inflate_state*)pStream->state;
        if (pState->m_window_bits > 0)
            decomp_flags |= TINFL_FLAG_PARSE_ZLIB_HEADER;
        orig_avail_in = pStream->avail_in;

        first_call = pState->m_first_call;
        pState->m_first_call = 0;
        if (pState->m_last_status < 0)
            return MZ_DATA_ERROR;

        if (pState->m_has_flushed && (flush != MZ_FINISH))
            return MZ_STREAM_ERROR;
        pState->m_has_flushed |= (flush == MZ_FINISH);

        if ((flush == MZ_FINISH) && (first_call))
        {
            /* MZ_FINISH on the first call implies that the input and output buffers are large enough to hold the entire compressed/decompressed file. */
            decomp_flags |= TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
            in_bytes = pStream->avail_in;
            out_bytes = pStream->avail_out;
            status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pStream->next_out, pStream->next_out, &out_bytes, decomp_flags);
            pState->m_last_status = status;
            pStream->next_in += (mz_uint)in_bytes;
            pStream->avail_in -= (mz_uint)in_bytes;
            pStream->total_in += (mz_uint)in_bytes;
            pStream->adler = tinfl_get_adler32(&pState->m_decomp);
            pStream->next_out += (mz_uint)out_bytes;
            pStream->avail_out -= (mz_uint)out_bytes;
            pStream->total_out += (mz_uint)out_bytes;

            if (status < 0)
                return MZ_DATA_ERROR;
            else if (status != TINFL_STATUS_DONE)
            {
                pState->m_last_status = TINFL_STATUS_FAILED;
                return MZ_BUF_ERROR;
            }
            return MZ_STREAM_END;
        }
        /* flush != MZ_FINISH then we must assume there's more input. */
        if (flush != MZ_FINISH)
            decomp_flags |= TINFL_FLAG_HAS_MORE_INPUT;

        if (pState->m_dict_avail)
        {
            n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
            memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
            pStream->next_out += n;
            pStream->avail_out -= n;
            pStream->total_out += n;
            pState->m_dict_avail -= n;
            pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);
            return ((pState->m_last_status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
        }

        for (;;)
        {
            in_bytes = pStream->avail_in;
            out_bytes = TINFL_LZ_DICT_SIZE - pState->m_dict_ofs;

            status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
            pState->m_last_status = status;

            pStream->next_in += (mz_uint)in_bytes;
            pStream->avail_in -= (mz_uint)in_bytes;
            pStream->total_in += (mz_uint)in_bytes;
            pStream->adler = tinfl_get_adler32(&pState->m_decomp);

            pState->m_dict_avail = (mz_uint)out_bytes;

            n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
            memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
            pStream->next_out += n;
            pStream->avail_out -= n;
            pStream->total_out += n;
            pState->m_dict_avail -= n;
            pState->m_dict_ofs = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);

            if (status < 0)
                return MZ_DATA_ERROR; /* Stream is corrupted (there could be some uncompressed data left in the output dictionary - oh well). */
            else if ((status == TINFL_STATUS_NEEDS_MORE_INPUT) && (!orig_avail_in))
                return MZ_BUF_ERROR; /* Signal caller that we can't make forward progress without supplying more input or by setting flush to MZ_FINISH. */
            else if (flush == MZ_FINISH)
            {
                /* The output buffer MUST be large to hold the remaining uncompressed data when flush==MZ_FINISH. */
                if (status == TINFL_STATUS_DONE)
                    return pState->m_dict_avail ? MZ_BUF_ERROR : MZ_STREAM_END;
                /* status here must be TINFL_STATUS_HAS_MORE_OUTPUT, which means there's at least 1 more byte on the way. If there's no more room left in the output buffer then something is wrong. */
                else if (!pStream->avail_out)
                    return MZ_BUF_ERROR;
            }
            else if ((status == TINFL_STATUS_DONE) || (!pStream->avail_in) || (!pStream->avail_out) || (pState->m_dict_avail))
                break;
        }

        return ((status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
    }

    int mz_inflateEnd(mz_streamp pStream)
    {
        if (!pStream)
            return MZ_STREAM_ERROR;
        if (pStream->state)
        {
            pStream->zfree(pStream->opaque, pStream->state);
            pStream->state = NULL;
        }
        return MZ_OK;
    }

    int mz_uncompress(unsigned char* pDest, mz_ulong* pDest_len, const unsigned char* pSource, mz_ulong source_len)
    {
        mz_stream stream;
        int status;
        memset(&stream, 0, sizeof(stream));

        /* In case mz_ulong is 64-bits (argh I hate longs). */
        if ((source_len | *pDest_len) > 0xFFFFFFFFU)
            return MZ_PARAM_ERROR;

        stream.next_in = pSource;
        stream.avail_in = (mz_uint32)source_len;
        stream.next_out = pDest;
        stream.avail_out = (mz_uint32)*pDest_len;

        status = mz_inflateInit(&stream);
        if (status != MZ_OK)
            return status;

        status = mz_inflate(&stream, MZ_FINISH);
        if (status != MZ_STREAM_END)
        {
            mz_inflateEnd(&stream);
            return ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
        }
        *pDest_len = stream.total_out;

        return mz_inflateEnd(&stream);
    }

    const char* mz_error(int err)
    {
        static struct
        {
            int m_err;
            const char* m_pDesc;
        } s_error_descs[] =
        {
            { MZ_OK, "" }, { MZ_STREAM_END, "stream end" }, { MZ_NEED_DICT, "need dictionary" }, { MZ_ERRNO, "file error" }, { MZ_STREAM_ERROR, "stream error" },
            { MZ_DATA_ERROR, "data error" }, { MZ_MEM_ERROR, "out of memory" }, { MZ_BUF_ERROR, "buf error" }, { MZ_VERSION_ERROR, "version error" }, { MZ_PARAM_ERROR, "parameter error" }
        };
        mz_uint i;
        for (i = 0; i < sizeof(s_error_descs) / sizeof(s_error_descs[0]); ++i)
            if (s_error_descs[i].m_err == err)
                return s_error_descs[i].m_pDesc;
        return NULL;
    }

#endif /*MINIZ_NO_ZLIB_APIS */

#ifdef __cplusplus
}
#endif

/*
  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org/>
*/
/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/




#ifdef __cplusplus
extern "C" {
#endif

    /* ------------------- Low-level Compression (independent from all decompression API's) */

    /* Purposely making these tables static for faster init and thread safety. */
    static const mz_uint16 s_tdefl_len_sym[256] =
    {
        257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
        273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
        277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
        279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
        281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
        282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
        283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
        284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
    };

    static const mz_uint8 s_tdefl_len_extra[256] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0
    };

    static const mz_uint8 s_tdefl_small_dist_sym[512] =
    {
        0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
    };

    static const mz_uint8 s_tdefl_small_dist_extra[512] =
    {
        0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7
    };

    static const mz_uint8 s_tdefl_large_dist_sym[128] =
    {
        0, 0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
        26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
    };

    static const mz_uint8 s_tdefl_large_dist_extra[128] =
    {
        0, 0, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
    };

    /* Radix sorts tdefl_sym_freq[] array by 16-bit key m_key. Returns ptr to sorted values. */
    typedef struct
    {
        mz_uint16 m_key, m_sym_index;
    } tdefl_sym_freq;
    static tdefl_sym_freq* tdefl_radix_sort_syms(mz_uint num_syms, tdefl_sym_freq* pSyms0, tdefl_sym_freq* pSyms1)
    {
        mz_uint32 total_passes = 2, pass_shift, pass, i, hist[256 * 2];
        tdefl_sym_freq* pCur_syms = pSyms0, * pNew_syms = pSyms1;
        MZ_CLEAR_OBJ(hist);
        for (i = 0; i < num_syms; i++)
        {
            mz_uint freq = pSyms0[i].m_key;
            hist[freq & 0xFF]++;
            hist[256 + ((freq >> 8) & 0xFF)]++;
        }
        while ((total_passes > 1) && (num_syms == hist[(total_passes - 1) * 256]))
            total_passes--;
        for (pass_shift = 0, pass = 0; pass < total_passes; pass++, pass_shift += 8)
        {
            const mz_uint32* pHist = &hist[pass << 8];
            mz_uint offsets[256], cur_ofs = 0;
            for (i = 0; i < 256; i++)
            {
                offsets[i] = cur_ofs;
                cur_ofs += pHist[i];
            }
            for (i = 0; i < num_syms; i++)
                pNew_syms[offsets[(pCur_syms[i].m_key >> pass_shift) & 0xFF]++] = pCur_syms[i];
            {
                tdefl_sym_freq* t = pCur_syms;
                pCur_syms = pNew_syms;
                pNew_syms = t;
            }
        }
        return pCur_syms;
    }

    /* tdefl_calculate_minimum_redundancy() originally written by: Alistair Moffat, alistair@cs.mu.oz.au, Jyrki Katajainen, jyrki@diku.dk, November 1996. */
    static void tdefl_calculate_minimum_redundancy(tdefl_sym_freq* A, int n)
    {
        int root, leaf, next, avbl, used, dpth;
        if (n == 0)
            return;
        else if (n == 1)
        {
            A[0].m_key = 1;
            return;
        }
        A[0].m_key += A[1].m_key;
        root = 0;
        leaf = 2;
        for (next = 1; next < n - 1; next++)
        {
            if (leaf >= n || A[root].m_key < A[leaf].m_key)
            {
                A[next].m_key = A[root].m_key;
                A[root++].m_key = (mz_uint16)next;
            }
            else
                A[next].m_key = A[leaf++].m_key;
            if (leaf >= n || (root < next && A[root].m_key < A[leaf].m_key))
            {
                A[next].m_key = (mz_uint16)(A[next].m_key + A[root].m_key);
                A[root++].m_key = (mz_uint16)next;
            }
            else
                A[next].m_key = (mz_uint16)(A[next].m_key + A[leaf++].m_key);
        }
        A[n - 2].m_key = 0;
        for (next = n - 3; next >= 0; next--)
            A[next].m_key = A[A[next].m_key].m_key + 1;
        avbl = 1;
        used = dpth = 0;
        root = n - 2;
        next = n - 1;
        while (avbl > 0)
        {
            while (root >= 0 && (int)A[root].m_key == dpth)
            {
                used++;
                root--;
            }
            while (avbl > used)
            {
                A[next--].m_key = (mz_uint16)(dpth);
                avbl--;
            }
            avbl = 2 * used;
            dpth++;
            used = 0;
        }
    }

    /* Limits canonical Huffman code table's max code size. */
    enum
    {
        TDEFL_MAX_SUPPORTED_HUFF_CODESIZE = 32
    };
    static void tdefl_huffman_enforce_max_code_size(int* pNum_codes, int code_list_len, int max_code_size)
    {
        int i;
        mz_uint32 total = 0;
        if (code_list_len <= 1)
            return;
        for (i = max_code_size + 1; i <= TDEFL_MAX_SUPPORTED_HUFF_CODESIZE; i++)
            pNum_codes[max_code_size] += pNum_codes[i];
        for (i = max_code_size; i > 0; i--)
            total += (((mz_uint32)pNum_codes[i]) << (max_code_size - i));
        while (total != (1UL << max_code_size))
        {
            pNum_codes[max_code_size]--;
            for (i = max_code_size - 1; i > 0; i--)
                if (pNum_codes[i])
                {
                    pNum_codes[i]--;
                    pNum_codes[i + 1] += 2;
                    break;
                }
            total--;
        }
    }

    static void tdefl_optimize_huffman_table(tdefl_compressor* d, int table_num, int table_len, int code_size_limit, int static_table)
    {
        int i, j, l, num_codes[1 + TDEFL_MAX_SUPPORTED_HUFF_CODESIZE];
        mz_uint next_code[TDEFL_MAX_SUPPORTED_HUFF_CODESIZE + 1];
        MZ_CLEAR_OBJ(num_codes);
        if (static_table)
        {
            for (i = 0; i < table_len; i++)
                num_codes[d->m_huff_code_sizes[table_num][i]]++;
        }
        else
        {
            tdefl_sym_freq syms0[TDEFL_MAX_HUFF_SYMBOLS], syms1[TDEFL_MAX_HUFF_SYMBOLS], * pSyms;
            int num_used_syms = 0;
            const mz_uint16* pSym_count = &d->m_huff_count[table_num][0];
            for (i = 0; i < table_len; i++)
                if (pSym_count[i])
                {
                    syms0[num_used_syms].m_key = (mz_uint16)pSym_count[i];
                    syms0[num_used_syms++].m_sym_index = (mz_uint16)i;
                }

            pSyms = tdefl_radix_sort_syms(num_used_syms, syms0, syms1);
            tdefl_calculate_minimum_redundancy(pSyms, num_used_syms);

            for (i = 0; i < num_used_syms; i++)
                num_codes[pSyms[i].m_key]++;

            tdefl_huffman_enforce_max_code_size(num_codes, num_used_syms, code_size_limit);

            MZ_CLEAR_OBJ(d->m_huff_code_sizes[table_num]);
            MZ_CLEAR_OBJ(d->m_huff_codes[table_num]);
            for (i = 1, j = num_used_syms; i <= code_size_limit; i++)
                for (l = num_codes[i]; l > 0; l--)
                    d->m_huff_code_sizes[table_num][pSyms[--j].m_sym_index] = (mz_uint8)(i);
        }

        next_code[1] = 0;
        for (j = 0, i = 2; i <= code_size_limit; i++)
            next_code[i] = j = ((j + num_codes[i - 1]) << 1);

        for (i = 0; i < table_len; i++)
        {
            mz_uint rev_code = 0, code, code_size;
            if ((code_size = d->m_huff_code_sizes[table_num][i]) == 0)
                continue;
            code = next_code[code_size]++;
            for (l = code_size; l > 0; l--, code >>= 1)
                rev_code = (rev_code << 1) | (code & 1);
            d->m_huff_codes[table_num][i] = (mz_uint16)rev_code;
        }
    }

#define TDEFL_PUT_BITS(b, l)                                       \
    do                                                             \
    {                                                              \
        mz_uint bits = b;                                          \
        mz_uint len = l;                                           \
        MZ_ASSERT(bits <= ((1U << len) - 1U));                     \
        d->m_bit_buffer |= (bits << d->m_bits_in);                 \
        d->m_bits_in += len;                                       \
        while (d->m_bits_in >= 8)                                  \
        {                                                          \
            if (d->m_pOutput_buf < d->m_pOutput_buf_end)           \
                *d->m_pOutput_buf++ = (mz_uint8)(d->m_bit_buffer); \
            d->m_bit_buffer >>= 8;                                 \
            d->m_bits_in -= 8;                                     \
        }                                                          \
    }                                                              \
    MZ_MACRO_END

#define TDEFL_RLE_PREV_CODE_SIZE()                                                                                       \
    {                                                                                                                    \
        if (rle_repeat_count)                                                                                            \
        {                                                                                                                \
            if (rle_repeat_count < 3)                                                                                    \
            {                                                                                                            \
                d->m_huff_count[2][prev_code_size] = (mz_uint16)(d->m_huff_count[2][prev_code_size] + rle_repeat_count); \
                while (rle_repeat_count--)                                                                               \
                    packed_code_sizes[num_packed_code_sizes++] = prev_code_size;                                         \
            }                                                                                                            \
            else                                                                                                         \
            {                                                                                                            \
                d->m_huff_count[2][16] = (mz_uint16)(d->m_huff_count[2][16] + 1);                                        \
                packed_code_sizes[num_packed_code_sizes++] = 16;                                                         \
                packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_repeat_count - 3);                           \
            }                                                                                                            \
            rle_repeat_count = 0;                                                                                        \
        }                                                                                                                \
    }

#define TDEFL_RLE_ZERO_CODE_SIZE()                                                         \
    {                                                                                      \
        if (rle_z_count)                                                                   \
        {                                                                                  \
            if (rle_z_count < 3)                                                           \
            {                                                                              \
                d->m_huff_count[2][0] = (mz_uint16)(d->m_huff_count[2][0] + rle_z_count);  \
                while (rle_z_count--)                                                      \
                    packed_code_sizes[num_packed_code_sizes++] = 0;                        \
            }                                                                              \
            else if (rle_z_count <= 10)                                                    \
            {                                                                              \
                d->m_huff_count[2][17] = (mz_uint16)(d->m_huff_count[2][17] + 1);          \
                packed_code_sizes[num_packed_code_sizes++] = 17;                           \
                packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_z_count - 3);  \
            }                                                                              \
            else                                                                           \
            {                                                                              \
                d->m_huff_count[2][18] = (mz_uint16)(d->m_huff_count[2][18] + 1);          \
                packed_code_sizes[num_packed_code_sizes++] = 18;                           \
                packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_z_count - 11); \
            }                                                                              \
            rle_z_count = 0;                                                               \
        }                                                                                  \
    }

    static mz_uint8 s_tdefl_packed_code_size_syms_swizzle[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

    static void tdefl_start_dynamic_block(tdefl_compressor* d)
    {
        int num_lit_codes, num_dist_codes, num_bit_lengths;
        mz_uint i, total_code_sizes_to_pack, num_packed_code_sizes, rle_z_count, rle_repeat_count, packed_code_sizes_index;
        mz_uint8 code_sizes_to_pack[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], packed_code_sizes[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], prev_code_size = 0xFF;

        d->m_huff_count[0][256] = 1;

        tdefl_optimize_huffman_table(d, 0, TDEFL_MAX_HUFF_SYMBOLS_0, 15, MZ_FALSE);
        tdefl_optimize_huffman_table(d, 1, TDEFL_MAX_HUFF_SYMBOLS_1, 15, MZ_FALSE);

        for (num_lit_codes = 286; num_lit_codes > 257; num_lit_codes--)
            if (d->m_huff_code_sizes[0][num_lit_codes - 1])
                break;
        for (num_dist_codes = 30; num_dist_codes > 1; num_dist_codes--)
            if (d->m_huff_code_sizes[1][num_dist_codes - 1])
                break;

        memcpy(code_sizes_to_pack, &d->m_huff_code_sizes[0][0], num_lit_codes);
        memcpy(code_sizes_to_pack + num_lit_codes, &d->m_huff_code_sizes[1][0], num_dist_codes);
        total_code_sizes_to_pack = num_lit_codes + num_dist_codes;
        num_packed_code_sizes = 0;
        rle_z_count = 0;
        rle_repeat_count = 0;

        memset(&d->m_huff_count[2][0], 0, sizeof(d->m_huff_count[2][0]) * TDEFL_MAX_HUFF_SYMBOLS_2);
        for (i = 0; i < total_code_sizes_to_pack; i++)
        {
            mz_uint8 code_size = code_sizes_to_pack[i];
            if (!code_size)
            {
                TDEFL_RLE_PREV_CODE_SIZE();
                if (++rle_z_count == 138)
                {
                    TDEFL_RLE_ZERO_CODE_SIZE();
                }
            }
            else
            {
                TDEFL_RLE_ZERO_CODE_SIZE();
                if (code_size != prev_code_size)
                {
                    TDEFL_RLE_PREV_CODE_SIZE();
                    d->m_huff_count[2][code_size] = (mz_uint16)(d->m_huff_count[2][code_size] + 1);
                    packed_code_sizes[num_packed_code_sizes++] = code_size;
                }
                else if (++rle_repeat_count == 6)
                {
                    TDEFL_RLE_PREV_CODE_SIZE();
                }
            }
            prev_code_size = code_size;
        }
        if (rle_repeat_count)
        {
            TDEFL_RLE_PREV_CODE_SIZE();
        }
        else
        {
            TDEFL_RLE_ZERO_CODE_SIZE();
        }

        tdefl_optimize_huffman_table(d, 2, TDEFL_MAX_HUFF_SYMBOLS_2, 7, MZ_FALSE);

        TDEFL_PUT_BITS(2, 2);

        TDEFL_PUT_BITS(num_lit_codes - 257, 5);
        TDEFL_PUT_BITS(num_dist_codes - 1, 5);

        for (num_bit_lengths = 18; num_bit_lengths >= 0; num_bit_lengths--)
            if (d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[num_bit_lengths]])
                break;
        num_bit_lengths = MZ_MAX(4, (num_bit_lengths + 1));
        TDEFL_PUT_BITS(num_bit_lengths - 4, 4);
        for (i = 0; (int)i < num_bit_lengths; i++)
            TDEFL_PUT_BITS(d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[i]], 3);

        for (packed_code_sizes_index = 0; packed_code_sizes_index < num_packed_code_sizes;)
        {
            mz_uint code = packed_code_sizes[packed_code_sizes_index++];
            MZ_ASSERT(code < TDEFL_MAX_HUFF_SYMBOLS_2);
            TDEFL_PUT_BITS(d->m_huff_codes[2][code], d->m_huff_code_sizes[2][code]);
            if (code >= 16)
                TDEFL_PUT_BITS(packed_code_sizes[packed_code_sizes_index++], "\02\03\07"[code - 16]);
        }
    }

    static void tdefl_start_static_block(tdefl_compressor* d)
    {
        mz_uint i;
        mz_uint8* p = &d->m_huff_code_sizes[0][0];

        for (i = 0; i <= 143; ++i)
            *p++ = 8;
        for (; i <= 255; ++i)
            *p++ = 9;
        for (; i <= 279; ++i)
            *p++ = 7;
        for (; i <= 287; ++i)
            *p++ = 8;

        memset(d->m_huff_code_sizes[1], 5, 32);

        tdefl_optimize_huffman_table(d, 0, 288, 15, MZ_TRUE);
        tdefl_optimize_huffman_table(d, 1, 32, 15, MZ_TRUE);

        TDEFL_PUT_BITS(1, 2);
    }

    static const mz_uint mz_bitmasks[17] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES &&MINIZ_LITTLE_ENDIAN &&MINIZ_HAS_64BIT_REGISTERS
    static mz_bool tdefl_compress_lz_codes(tdefl_compressor* d)
    {
        mz_uint flags;
        mz_uint8* pLZ_codes;
        mz_uint8* pOutput_buf = d->m_pOutput_buf;
        mz_uint8* pLZ_code_buf_end = d->m_pLZ_code_buf;
        mz_uint64 bit_buffer = d->m_bit_buffer;
        mz_uint bits_in = d->m_bits_in;

#define TDEFL_PUT_BITS_FAST(b, l)                    \
    {                                                \
        bit_buffer |= (((mz_uint64)(b)) << bits_in); \
        bits_in += (l);                              \
    }

        flags = 1;
        for (pLZ_codes = d->m_lz_code_buf; pLZ_codes < pLZ_code_buf_end; flags >>= 1)
        {
            if (flags == 1)
                flags = *pLZ_codes++ | 0x100;

            if (flags & 1)
            {
                mz_uint s0, s1, n0, n1, sym, num_extra_bits;
                mz_uint match_len = pLZ_codes[0], match_dist = *(const mz_uint16*)(pLZ_codes + 1);
                pLZ_codes += 3;

                MZ_ASSERT(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
                TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][s_tdefl_len_sym[match_len]], d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
                TDEFL_PUT_BITS_FAST(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], s_tdefl_len_extra[match_len]);

                /* This sequence coaxes MSVC into using cmov's vs. jmp's. */
                s0 = s_tdefl_small_dist_sym[match_dist & 511];
                n0 = s_tdefl_small_dist_extra[match_dist & 511];
                s1 = s_tdefl_large_dist_sym[match_dist >> 8];
                n1 = s_tdefl_large_dist_extra[match_dist >> 8];
                sym = (match_dist < 512) ? s0 : s1;
                num_extra_bits = (match_dist < 512) ? n0 : n1;

                MZ_ASSERT(d->m_huff_code_sizes[1][sym]);
                TDEFL_PUT_BITS_FAST(d->m_huff_codes[1][sym], d->m_huff_code_sizes[1][sym]);
                TDEFL_PUT_BITS_FAST(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits);
            }
            else
            {
                mz_uint lit = *pLZ_codes++;
                MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

                if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end))
                {
                    flags >>= 1;
                    lit = *pLZ_codes++;
                    MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                    TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

                    if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end))
                    {
                        flags >>= 1;
                        lit = *pLZ_codes++;
                        MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                        TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);
                    }
                }
            }

            if (pOutput_buf >= d->m_pOutput_buf_end)
                return MZ_FALSE;

            *(mz_uint64*)pOutput_buf = bit_buffer;
            pOutput_buf += (bits_in >> 3);
            bit_buffer >>= (bits_in & ~7);
            bits_in &= 7;
        }

#undef TDEFL_PUT_BITS_FAST

        d->m_pOutput_buf = pOutput_buf;
        d->m_bits_in = 0;
        d->m_bit_buffer = 0;

        while (bits_in)
        {
            mz_uint32 n = MZ_MIN(bits_in, 16);
            TDEFL_PUT_BITS((mz_uint)bit_buffer & mz_bitmasks[n], n);
            bit_buffer >>= n;
            bits_in -= n;
        }

        TDEFL_PUT_BITS(d->m_huff_codes[0][256], d->m_huff_code_sizes[0][256]);

        return (d->m_pOutput_buf < d->m_pOutput_buf_end);
    }
#else
    static mz_bool tdefl_compress_lz_codes(tdefl_compressor* d)
    {
        mz_uint flags;
        mz_uint8* pLZ_codes;

        flags = 1;
        for (pLZ_codes = d->m_lz_code_buf; pLZ_codes < d->m_pLZ_code_buf; flags >>= 1)
        {
            if (flags == 1)
                flags = *pLZ_codes++ | 0x100;
            if (flags & 1)
            {
                mz_uint sym, num_extra_bits;
                mz_uint match_len = pLZ_codes[0], match_dist = (pLZ_codes[1] | (pLZ_codes[2] << 8));
                pLZ_codes += 3;

                MZ_ASSERT(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
                TDEFL_PUT_BITS(d->m_huff_codes[0][s_tdefl_len_sym[match_len]], d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
                TDEFL_PUT_BITS(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], s_tdefl_len_extra[match_len]);

                if (match_dist < 512)
                {
                    sym = s_tdefl_small_dist_sym[match_dist];
                    num_extra_bits = s_tdefl_small_dist_extra[match_dist];
                }
                else
                {
                    sym = s_tdefl_large_dist_sym[match_dist >> 8];
                    num_extra_bits = s_tdefl_large_dist_extra[match_dist >> 8];
                }
                MZ_ASSERT(d->m_huff_code_sizes[1][sym]);
                TDEFL_PUT_BITS(d->m_huff_codes[1][sym], d->m_huff_code_sizes[1][sym]);
                TDEFL_PUT_BITS(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits);
            }
            else
            {
                mz_uint lit = *pLZ_codes++;
                MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                TDEFL_PUT_BITS(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);
            }
        }

        TDEFL_PUT_BITS(d->m_huff_codes[0][256], d->m_huff_code_sizes[0][256]);

        return (d->m_pOutput_buf < d->m_pOutput_buf_end);
    }
#endif /* MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN && MINIZ_HAS_64BIT_REGISTERS */

    static mz_bool tdefl_compress_block(tdefl_compressor* d, mz_bool static_block)
    {
        if (static_block)
            tdefl_start_static_block(d);
        else
            tdefl_start_dynamic_block(d);
        return tdefl_compress_lz_codes(d);
    }

    static int tdefl_flush_block(tdefl_compressor* d, int flush)
    {
        mz_uint saved_bit_buf, saved_bits_in;
        mz_uint8* pSaved_output_buf;
        mz_bool comp_block_succeeded = MZ_FALSE;
        int n, use_raw_block = ((d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS) != 0) && (d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size;
        mz_uint8* pOutput_buf_start = ((d->m_pPut_buf_func == NULL) && ((*d->m_pOut_buf_size - d->m_out_buf_ofs) >= TDEFL_OUT_BUF_SIZE)) ? ((mz_uint8*)d->m_pOut_buf + d->m_out_buf_ofs) : d->m_output_buf;

        d->m_pOutput_buf = pOutput_buf_start;
        d->m_pOutput_buf_end = d->m_pOutput_buf + TDEFL_OUT_BUF_SIZE - 16;

        MZ_ASSERT(!d->m_output_flush_remaining);
        d->m_output_flush_ofs = 0;
        d->m_output_flush_remaining = 0;

        *d->m_pLZ_flags = (mz_uint8)(*d->m_pLZ_flags >> d->m_num_flags_left);
        d->m_pLZ_code_buf -= (d->m_num_flags_left == 8);

        if ((d->m_flags & TDEFL_WRITE_ZLIB_HEADER) && (!d->m_block_index))
        {
            TDEFL_PUT_BITS(0x78, 8);
            TDEFL_PUT_BITS(0x01, 8);
        }

        TDEFL_PUT_BITS(flush == TDEFL_FINISH, 1);

        pSaved_output_buf = d->m_pOutput_buf;
        saved_bit_buf = d->m_bit_buffer;
        saved_bits_in = d->m_bits_in;

        if (!use_raw_block)
            comp_block_succeeded = tdefl_compress_block(d, (d->m_flags & TDEFL_FORCE_ALL_STATIC_BLOCKS) || (d->m_total_lz_bytes < 48));

        /* If the block gets expanded, forget the current contents of the output buffer and send a raw block instead. */
        if (((use_raw_block) || ((d->m_total_lz_bytes) && ((d->m_pOutput_buf - pSaved_output_buf + 1U) >= d->m_total_lz_bytes))) &&
            ((d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size))
        {
            mz_uint i;
            d->m_pOutput_buf = pSaved_output_buf;
            d->m_bit_buffer = saved_bit_buf, d->m_bits_in = saved_bits_in;
            TDEFL_PUT_BITS(0, 2);
            if (d->m_bits_in)
            {
                TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
            }
            for (i = 2; i; --i, d->m_total_lz_bytes ^= 0xFFFF)
            {
                TDEFL_PUT_BITS(d->m_total_lz_bytes & 0xFFFF, 16);
            }
            for (i = 0; i < d->m_total_lz_bytes; ++i)
            {
                TDEFL_PUT_BITS(d->m_dict[(d->m_lz_code_buf_dict_pos + i) & TDEFL_LZ_DICT_SIZE_MASK], 8);
            }
        }
        /* Check for the extremely unlikely (if not impossible) case of the compressed block not fitting into the output buffer when using dynamic codes. */
        else if (!comp_block_succeeded)
        {
            d->m_pOutput_buf = pSaved_output_buf;
            d->m_bit_buffer = saved_bit_buf, d->m_bits_in = saved_bits_in;
            tdefl_compress_block(d, MZ_TRUE);
        }

        if (flush)
        {
            if (flush == TDEFL_FINISH)
            {
                if (d->m_bits_in)
                {
                    TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
                }
                if (d->m_flags & TDEFL_WRITE_ZLIB_HEADER)
                {
                    mz_uint i, a = d->m_adler32;
                    for (i = 0; i < 4; i++)
                    {
                        TDEFL_PUT_BITS((a >> 24) & 0xFF, 8);
                        a <<= 8;
                    }
                }
            }
            else
            {
                mz_uint i, z = 0;
                TDEFL_PUT_BITS(0, 3);
                if (d->m_bits_in)
                {
                    TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
                }
                for (i = 2; i; --i, z ^= 0xFFFF)
                {
                    TDEFL_PUT_BITS(z & 0xFFFF, 16);
                }
            }
        }

        MZ_ASSERT(d->m_pOutput_buf < d->m_pOutput_buf_end);

        memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
        memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);

        d->m_pLZ_code_buf = d->m_lz_code_buf + 1;
        d->m_pLZ_flags = d->m_lz_code_buf;
        d->m_num_flags_left = 8;
        d->m_lz_code_buf_dict_pos += d->m_total_lz_bytes;
        d->m_total_lz_bytes = 0;
        d->m_block_index++;

        if ((n = (int)(d->m_pOutput_buf - pOutput_buf_start)) != 0)
        {
            if (d->m_pPut_buf_func)
            {
                *d->m_pIn_buf_size = d->m_pSrc - (const mz_uint8*)d->m_pIn_buf;
                if (!(*d->m_pPut_buf_func)(d->m_output_buf, n, d->m_pPut_buf_user))
                    return (d->m_prev_return_status = TDEFL_STATUS_PUT_BUF_FAILED);
            }
            else if (pOutput_buf_start == d->m_output_buf)
            {
                int bytes_to_copy = (int)MZ_MIN((size_t)n, (size_t)(*d->m_pOut_buf_size - d->m_out_buf_ofs));
                memcpy((mz_uint8*)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf, bytes_to_copy);
                d->m_out_buf_ofs += bytes_to_copy;
                if ((n -= bytes_to_copy) != 0)
                {
                    d->m_output_flush_ofs = bytes_to_copy;
                    d->m_output_flush_remaining = n;
                }
            }
            else
            {
                d->m_out_buf_ofs += n;
            }
        }

        return d->m_output_flush_remaining;
    }

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
#define TDEFL_READ_UNALIGNED_WORD(p) *(const mz_uint16 *)(p)
    static MZ_FORCEINLINE void tdefl_find_match(tdefl_compressor* d, mz_uint lookahead_pos, mz_uint max_dist, mz_uint max_match_len, mz_uint* pMatch_dist, mz_uint* pMatch_len)
    {
        mz_uint dist, pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK, match_len = *pMatch_len, probe_pos = pos, next_probe_pos, probe_len;
        mz_uint num_probes_left = d->m_max_probes[match_len >= 32];
        const mz_uint16* s = (const mz_uint16*)(d->m_dict + pos), * p, * q;
        mz_uint16 c01 = TDEFL_READ_UNALIGNED_WORD(&d->m_dict[pos + match_len - 1]), s01 = TDEFL_READ_UNALIGNED_WORD(s);
        MZ_ASSERT(max_match_len <= TDEFL_MAX_MATCH_LEN);
        if (max_match_len <= match_len)
            return;
        for (;;)
        {
            for (;;)
            {
                if (--num_probes_left == 0)
                    return;
#define TDEFL_PROBE                                                                             \
    next_probe_pos = d->m_next[probe_pos];                                                      \
    if ((!next_probe_pos) || ((dist = (mz_uint16)(lookahead_pos - next_probe_pos)) > max_dist)) \
        return;                                                                                 \
    probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK;                                       \
    if (TDEFL_READ_UNALIGNED_WORD(&d->m_dict[probe_pos + match_len - 1]) == c01)                \
        break;
                TDEFL_PROBE;
                TDEFL_PROBE;
                TDEFL_PROBE;
            }
            if (!dist)
                break;
            q = (const mz_uint16*)(d->m_dict + probe_pos);
            if (TDEFL_READ_UNALIGNED_WORD(q) != s01)
                continue;
            p = s;
            probe_len = 32;
            do
            {
            } while ((TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) &&
                (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (--probe_len > 0));
            if (!probe_len)
            {
                *pMatch_dist = dist;
                *pMatch_len = MZ_MIN(max_match_len, (mz_uint)TDEFL_MAX_MATCH_LEN);
                break;
            }
            else if ((probe_len = ((mz_uint)(p - s) * 2) + (mz_uint)(*(const mz_uint8*)p == *(const mz_uint8*)q)) > match_len)
            {
                *pMatch_dist = dist;
                if ((*pMatch_len = match_len = MZ_MIN(max_match_len, probe_len)) == max_match_len)
                    break;
                c01 = TDEFL_READ_UNALIGNED_WORD(&d->m_dict[pos + match_len - 1]);
            }
        }
    }
#else
    static MZ_FORCEINLINE void tdefl_find_match(tdefl_compressor* d, mz_uint lookahead_pos, mz_uint max_dist, mz_uint max_match_len, mz_uint* pMatch_dist, mz_uint* pMatch_len)
    {
        mz_uint dist, pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK, match_len = *pMatch_len, probe_pos = pos, next_probe_pos, probe_len;
        mz_uint num_probes_left = d->m_max_probes[match_len >= 32];
        const mz_uint8* s = d->m_dict + pos, * p, * q;
        mz_uint8 c0 = d->m_dict[pos + match_len], c1 = d->m_dict[pos + match_len - 1];
        MZ_ASSERT(max_match_len <= TDEFL_MAX_MATCH_LEN);
        if (max_match_len <= match_len)
            return;
        for (;;)
        {
            for (;;)
            {
                if (--num_probes_left == 0)
                    return;
#define TDEFL_PROBE                                                                               \
    next_probe_pos = d->m_next[probe_pos];                                                        \
    if ((!next_probe_pos) || ((dist = (mz_uint16)(lookahead_pos - next_probe_pos)) > max_dist))   \
        return;                                                                                   \
    probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK;                                         \
    if ((d->m_dict[probe_pos + match_len] == c0) && (d->m_dict[probe_pos + match_len - 1] == c1)) \
        break;
                TDEFL_PROBE;
                TDEFL_PROBE;
                TDEFL_PROBE;
            }
            if (!dist)
                break;
            p = s;
            q = d->m_dict + probe_pos;
            for (probe_len = 0; probe_len < max_match_len; probe_len++)
                if (*p++ != *q++)
                    break;
            if (probe_len > match_len)
            {
                *pMatch_dist = dist;
                if ((*pMatch_len = match_len = probe_len) == max_match_len)
                    return;
                c0 = d->m_dict[pos + match_len];
                c1 = d->m_dict[pos + match_len - 1];
            }
        }
    }
#endif /* #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES */

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES &&MINIZ_LITTLE_ENDIAN
    static mz_bool tdefl_compress_fast(tdefl_compressor* d)
    {
        /* Faster, minimally featured LZRW1-style match+parse loop with better register utilization. Intended for applications where raw throughput is valued more highly than ratio. */
        mz_uint lookahead_pos = d->m_lookahead_pos, lookahead_size = d->m_lookahead_size, dict_size = d->m_dict_size, total_lz_bytes = d->m_total_lz_bytes, num_flags_left = d->m_num_flags_left;
        mz_uint8* pLZ_code_buf = d->m_pLZ_code_buf, * pLZ_flags = d->m_pLZ_flags;
        mz_uint cur_pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK;

        while ((d->m_src_buf_left) || ((d->m_flush) && (lookahead_size)))
        {
            const mz_uint TDEFL_COMP_FAST_LOOKAHEAD_SIZE = 4096;
            mz_uint dst_pos = (lookahead_pos + lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK;
            mz_uint num_bytes_to_process = (mz_uint)MZ_MIN(d->m_src_buf_left, TDEFL_COMP_FAST_LOOKAHEAD_SIZE - lookahead_size);
            d->m_src_buf_left -= num_bytes_to_process;
            lookahead_size += num_bytes_to_process;

            while (num_bytes_to_process)
            {
                mz_uint32 n = MZ_MIN(TDEFL_LZ_DICT_SIZE - dst_pos, num_bytes_to_process);
                memcpy(d->m_dict + dst_pos, d->m_pSrc, n);
                if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
                    memcpy(d->m_dict + TDEFL_LZ_DICT_SIZE + dst_pos, d->m_pSrc, MZ_MIN(n, (TDEFL_MAX_MATCH_LEN - 1) - dst_pos));
                d->m_pSrc += n;
                dst_pos = (dst_pos + n) & TDEFL_LZ_DICT_SIZE_MASK;
                num_bytes_to_process -= n;
            }

            dict_size = MZ_MIN(TDEFL_LZ_DICT_SIZE - lookahead_size, dict_size);
            if ((!d->m_flush) && (lookahead_size < TDEFL_COMP_FAST_LOOKAHEAD_SIZE))
                break;

            while (lookahead_size >= 4)
            {
                mz_uint cur_match_dist, cur_match_len = 1;
                mz_uint8* pCur_dict = d->m_dict + cur_pos;
                mz_uint first_trigram = (*(const mz_uint32*)pCur_dict) & 0xFFFFFF;
                mz_uint hash = (first_trigram ^ (first_trigram >> (24 - (TDEFL_LZ_HASH_BITS - 8)))) & TDEFL_LEVEL1_HASH_SIZE_MASK;
                mz_uint probe_pos = d->m_hash[hash];
                d->m_hash[hash] = (mz_uint16)lookahead_pos;

                if (((cur_match_dist = (mz_uint16)(lookahead_pos - probe_pos)) <= dict_size) && ((*(const mz_uint32*)(d->m_dict + (probe_pos &= TDEFL_LZ_DICT_SIZE_MASK)) & 0xFFFFFF) == first_trigram))
                {
                    const mz_uint16* p = (const mz_uint16*)pCur_dict;
                    const mz_uint16* q = (const mz_uint16*)(d->m_dict + probe_pos);
                    mz_uint32 probe_len = 32;
                    do
                    {
                    } while ((TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) &&
                        (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (--probe_len > 0));
                    cur_match_len = ((mz_uint)(p - (const mz_uint16*)pCur_dict) * 2) + (mz_uint)(*(const mz_uint8*)p == *(const mz_uint8*)q);
                    if (!probe_len)
                        cur_match_len = cur_match_dist ? TDEFL_MAX_MATCH_LEN : 0;

                    if ((cur_match_len < TDEFL_MIN_MATCH_LEN) || ((cur_match_len == TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 8U * 1024U)))
                    {
                        cur_match_len = 1;
                        *pLZ_code_buf++ = (mz_uint8)first_trigram;
                        *pLZ_flags = (mz_uint8)(*pLZ_flags >> 1);
                        d->m_huff_count[0][(mz_uint8)first_trigram]++;
                    }
                    else
                    {
                        mz_uint32 s0, s1;
                        cur_match_len = MZ_MIN(cur_match_len, lookahead_size);

                        MZ_ASSERT((cur_match_len >= TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 1) && (cur_match_dist <= TDEFL_LZ_DICT_SIZE));

                        cur_match_dist--;

                        pLZ_code_buf[0] = (mz_uint8)(cur_match_len - TDEFL_MIN_MATCH_LEN);
                        *(mz_uint16*)(&pLZ_code_buf[1]) = (mz_uint16)cur_match_dist;
                        pLZ_code_buf += 3;
                        *pLZ_flags = (mz_uint8)((*pLZ_flags >> 1) | 0x80);

                        s0 = s_tdefl_small_dist_sym[cur_match_dist & 511];
                        s1 = s_tdefl_large_dist_sym[cur_match_dist >> 8];
                        d->m_huff_count[1][(cur_match_dist < 512) ? s0 : s1]++;

                        d->m_huff_count[0][s_tdefl_len_sym[cur_match_len - TDEFL_MIN_MATCH_LEN]]++;
                    }
                }
                else
                {
                    *pLZ_code_buf++ = (mz_uint8)first_trigram;
                    *pLZ_flags = (mz_uint8)(*pLZ_flags >> 1);
                    d->m_huff_count[0][(mz_uint8)first_trigram]++;
                }

                if (--num_flags_left == 0)
                {
                    num_flags_left = 8;
                    pLZ_flags = pLZ_code_buf++;
                }

                total_lz_bytes += cur_match_len;
                lookahead_pos += cur_match_len;
                dict_size = MZ_MIN(dict_size + cur_match_len, (mz_uint)TDEFL_LZ_DICT_SIZE);
                cur_pos = (cur_pos + cur_match_len) & TDEFL_LZ_DICT_SIZE_MASK;
                MZ_ASSERT(lookahead_size >= cur_match_len);
                lookahead_size -= cur_match_len;

                if (pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8])
                {
                    int n;
                    d->m_lookahead_pos = lookahead_pos;
                    d->m_lookahead_size = lookahead_size;
                    d->m_dict_size = dict_size;
                    d->m_total_lz_bytes = total_lz_bytes;
                    d->m_pLZ_code_buf = pLZ_code_buf;
                    d->m_pLZ_flags = pLZ_flags;
                    d->m_num_flags_left = num_flags_left;
                    if ((n = tdefl_flush_block(d, 0)) != 0)
                        return (n < 0) ? MZ_FALSE : MZ_TRUE;
                    total_lz_bytes = d->m_total_lz_bytes;
                    pLZ_code_buf = d->m_pLZ_code_buf;
                    pLZ_flags = d->m_pLZ_flags;
                    num_flags_left = d->m_num_flags_left;
                }
            }

            while (lookahead_size)
            {
                mz_uint8 lit = d->m_dict[cur_pos];

                total_lz_bytes++;
                *pLZ_code_buf++ = lit;
                *pLZ_flags = (mz_uint8)(*pLZ_flags >> 1);
                if (--num_flags_left == 0)
                {
                    num_flags_left = 8;
                    pLZ_flags = pLZ_code_buf++;
                }

                d->m_huff_count[0][lit]++;

                lookahead_pos++;
                dict_size = MZ_MIN(dict_size + 1, (mz_uint)TDEFL_LZ_DICT_SIZE);
                cur_pos = (cur_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK;
                lookahead_size--;

                if (pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8])
                {
                    int n;
                    d->m_lookahead_pos = lookahead_pos;
                    d->m_lookahead_size = lookahead_size;
                    d->m_dict_size = dict_size;
                    d->m_total_lz_bytes = total_lz_bytes;
                    d->m_pLZ_code_buf = pLZ_code_buf;
                    d->m_pLZ_flags = pLZ_flags;
                    d->m_num_flags_left = num_flags_left;
                    if ((n = tdefl_flush_block(d, 0)) != 0)
                        return (n < 0) ? MZ_FALSE : MZ_TRUE;
                    total_lz_bytes = d->m_total_lz_bytes;
                    pLZ_code_buf = d->m_pLZ_code_buf;
                    pLZ_flags = d->m_pLZ_flags;
                    num_flags_left = d->m_num_flags_left;
                }
            }
        }

        d->m_lookahead_pos = lookahead_pos;
        d->m_lookahead_size = lookahead_size;
        d->m_dict_size = dict_size;
        d->m_total_lz_bytes = total_lz_bytes;
        d->m_pLZ_code_buf = pLZ_code_buf;
        d->m_pLZ_flags = pLZ_flags;
        d->m_num_flags_left = num_flags_left;
        return MZ_TRUE;
    }
#endif /* MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN */

    static MZ_FORCEINLINE void tdefl_record_literal(tdefl_compressor* d, mz_uint8 lit)
    {
        d->m_total_lz_bytes++;
        *d->m_pLZ_code_buf++ = lit;
        *d->m_pLZ_flags = (mz_uint8)(*d->m_pLZ_flags >> 1);
        if (--d->m_num_flags_left == 0)
        {
            d->m_num_flags_left = 8;
            d->m_pLZ_flags = d->m_pLZ_code_buf++;
        }
        d->m_huff_count[0][lit]++;
    }

    static MZ_FORCEINLINE void tdefl_record_match(tdefl_compressor* d, mz_uint match_len, mz_uint match_dist)
    {
        mz_uint32 s0, s1;

        MZ_ASSERT((match_len >= TDEFL_MIN_MATCH_LEN) && (match_dist >= 1) && (match_dist <= TDEFL_LZ_DICT_SIZE));

        d->m_total_lz_bytes += match_len;

        d->m_pLZ_code_buf[0] = (mz_uint8)(match_len - TDEFL_MIN_MATCH_LEN);

        match_dist -= 1;
        d->m_pLZ_code_buf[1] = (mz_uint8)(match_dist & 0xFF);
        d->m_pLZ_code_buf[2] = (mz_uint8)(match_dist >> 8);
        d->m_pLZ_code_buf += 3;

        *d->m_pLZ_flags = (mz_uint8)((*d->m_pLZ_flags >> 1) | 0x80);
        if (--d->m_num_flags_left == 0)
        {
            d->m_num_flags_left = 8;
            d->m_pLZ_flags = d->m_pLZ_code_buf++;
        }

        s0 = s_tdefl_small_dist_sym[match_dist & 511];
        s1 = s_tdefl_large_dist_sym[(match_dist >> 8) & 127];
        d->m_huff_count[1][(match_dist < 512) ? s0 : s1]++;

        if (match_len >= TDEFL_MIN_MATCH_LEN)
            d->m_huff_count[0][s_tdefl_len_sym[match_len - TDEFL_MIN_MATCH_LEN]]++;
    }

    static mz_bool tdefl_compress_normal(tdefl_compressor* d)
    {
        const mz_uint8* pSrc = d->m_pSrc;
        size_t src_buf_left = d->m_src_buf_left;
        tdefl_flush flush = d->m_flush;

        while ((src_buf_left) || ((flush) && (d->m_lookahead_size)))
        {
            mz_uint len_to_move, cur_match_dist, cur_match_len, cur_pos;
            /* Update dictionary and hash chains. Keeps the lookahead size equal to TDEFL_MAX_MATCH_LEN. */
            if ((d->m_lookahead_size + d->m_dict_size) >= (TDEFL_MIN_MATCH_LEN - 1))
            {
                mz_uint dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK, ins_pos = d->m_lookahead_pos + d->m_lookahead_size - 2;
                mz_uint hash = (d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK];
                mz_uint num_bytes_to_process = (mz_uint)MZ_MIN(src_buf_left, TDEFL_MAX_MATCH_LEN - d->m_lookahead_size);
                const mz_uint8* pSrc_end = pSrc + num_bytes_to_process;
                src_buf_left -= num_bytes_to_process;
                d->m_lookahead_size += num_bytes_to_process;
                while (pSrc != pSrc_end)
                {
                    mz_uint8 c = *pSrc++;
                    d->m_dict[dst_pos] = c;
                    if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
                        d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
                    hash = ((hash << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1);
                    d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
                    d->m_hash[hash] = (mz_uint16)(ins_pos);
                    dst_pos = (dst_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK;
                    ins_pos++;
                }
            }
            else
            {
                while ((src_buf_left) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN))
                {
                    mz_uint8 c = *pSrc++;
                    mz_uint dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK;
                    src_buf_left--;
                    d->m_dict[dst_pos] = c;
                    if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
                        d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
                    if ((++d->m_lookahead_size + d->m_dict_size) >= TDEFL_MIN_MATCH_LEN)
                    {
                        mz_uint ins_pos = d->m_lookahead_pos + (d->m_lookahead_size - 1) - 2;
                        mz_uint hash = ((d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << (TDEFL_LZ_HASH_SHIFT * 2)) ^ (d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1);
                        d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
                        d->m_hash[hash] = (mz_uint16)(ins_pos);
                    }
                }
            }
            d->m_dict_size = MZ_MIN(TDEFL_LZ_DICT_SIZE - d->m_lookahead_size, d->m_dict_size);
            if ((!flush) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN))
                break;

            /* Simple lazy/greedy parsing state machine. */
            len_to_move = 1;
            cur_match_dist = 0;
            cur_match_len = d->m_saved_match_len ? d->m_saved_match_len : (TDEFL_MIN_MATCH_LEN - 1);
            cur_pos = d->m_lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK;
            if (d->m_flags & (TDEFL_RLE_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS))
            {
                if ((d->m_dict_size) && (!(d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS)))
                {
                    mz_uint8 c = d->m_dict[(cur_pos - 1) & TDEFL_LZ_DICT_SIZE_MASK];
                    cur_match_len = 0;
                    while (cur_match_len < d->m_lookahead_size)
                    {
                        if (d->m_dict[cur_pos + cur_match_len] != c)
                            break;
                        cur_match_len++;
                    }
                    if (cur_match_len < TDEFL_MIN_MATCH_LEN)
                        cur_match_len = 0;
                    else
                        cur_match_dist = 1;
                }
            }
            else
            {
                tdefl_find_match(d, d->m_lookahead_pos, d->m_dict_size, d->m_lookahead_size, &cur_match_dist, &cur_match_len);
            }
            if (((cur_match_len == TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 8U * 1024U)) || (cur_pos == cur_match_dist) || ((d->m_flags & TDEFL_FILTER_MATCHES) && (cur_match_len <= 5)))
            {
                cur_match_dist = cur_match_len = 0;
            }
            if (d->m_saved_match_len)
            {
                if (cur_match_len > d->m_saved_match_len)
                {
                    tdefl_record_literal(d, (mz_uint8)d->m_saved_lit);
                    if (cur_match_len >= 128)
                    {
                        tdefl_record_match(d, cur_match_len, cur_match_dist);
                        d->m_saved_match_len = 0;
                        len_to_move = cur_match_len;
                    }
                    else
                    {
                        d->m_saved_lit = d->m_dict[cur_pos];
                        d->m_saved_match_dist = cur_match_dist;
                        d->m_saved_match_len = cur_match_len;
                    }
                }
                else
                {
                    tdefl_record_match(d, d->m_saved_match_len, d->m_saved_match_dist);
                    len_to_move = d->m_saved_match_len - 1;
                    d->m_saved_match_len = 0;
                }
            }
            else if (!cur_match_dist)
                tdefl_record_literal(d, d->m_dict[MZ_MIN(cur_pos, sizeof(d->m_dict) - 1)]);
            else if ((d->m_greedy_parsing) || (d->m_flags & TDEFL_RLE_MATCHES) || (cur_match_len >= 128))
            {
                tdefl_record_match(d, cur_match_len, cur_match_dist);
                len_to_move = cur_match_len;
            }
            else
            {
                d->m_saved_lit = d->m_dict[MZ_MIN(cur_pos, sizeof(d->m_dict) - 1)];
                d->m_saved_match_dist = cur_match_dist;
                d->m_saved_match_len = cur_match_len;
            }
            /* Move the lookahead forward by len_to_move bytes. */
            d->m_lookahead_pos += len_to_move;
            MZ_ASSERT(d->m_lookahead_size >= len_to_move);
            d->m_lookahead_size -= len_to_move;
            d->m_dict_size = MZ_MIN(d->m_dict_size + len_to_move, (mz_uint)TDEFL_LZ_DICT_SIZE);
            /* Check if it's time to flush the current LZ codes to the internal output buffer. */
            if ((d->m_pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8]) ||
                ((d->m_total_lz_bytes > 31 * 1024) && (((((mz_uint)(d->m_pLZ_code_buf - d->m_lz_code_buf) * 115) >> 7) >= d->m_total_lz_bytes) || (d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS))))
            {
                int n;
                d->m_pSrc = pSrc;
                d->m_src_buf_left = src_buf_left;
                if ((n = tdefl_flush_block(d, 0)) != 0)
                    return (n < 0) ? MZ_FALSE : MZ_TRUE;
            }
        }

        d->m_pSrc = pSrc;
        d->m_src_buf_left = src_buf_left;
        return MZ_TRUE;
    }

    static tdefl_status tdefl_flush_output_buffer(tdefl_compressor* d)
    {
        if (d->m_pIn_buf_size)
        {
            *d->m_pIn_buf_size = d->m_pSrc - (const mz_uint8*)d->m_pIn_buf;
        }

        if (d->m_pOut_buf_size)
        {
            size_t n = MZ_MIN(*d->m_pOut_buf_size - d->m_out_buf_ofs, d->m_output_flush_remaining);
            memcpy((mz_uint8*)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf + d->m_output_flush_ofs, n);
            d->m_output_flush_ofs += (mz_uint)n;
            d->m_output_flush_remaining -= (mz_uint)n;
            d->m_out_buf_ofs += n;

            *d->m_pOut_buf_size = d->m_out_buf_ofs;
        }

        return (d->m_finished && !d->m_output_flush_remaining) ? TDEFL_STATUS_DONE : TDEFL_STATUS_OKAY;
    }

    tdefl_status tdefl_compress(tdefl_compressor* d, const void* pIn_buf, size_t* pIn_buf_size, void* pOut_buf, size_t* pOut_buf_size, tdefl_flush flush)
    {
        if (!d)
        {
            if (pIn_buf_size)
                *pIn_buf_size = 0;
            if (pOut_buf_size)
                *pOut_buf_size = 0;
            return TDEFL_STATUS_BAD_PARAM;
        }

        d->m_pIn_buf = pIn_buf;
        d->m_pIn_buf_size = pIn_buf_size;
        d->m_pOut_buf = pOut_buf;
        d->m_pOut_buf_size = pOut_buf_size;
        d->m_pSrc = (const mz_uint8*)(pIn_buf);
        d->m_src_buf_left = pIn_buf_size ? *pIn_buf_size : 0;
        d->m_out_buf_ofs = 0;
        d->m_flush = flush;

        if (((d->m_pPut_buf_func != NULL) == ((pOut_buf != NULL) || (pOut_buf_size != NULL))) || (d->m_prev_return_status != TDEFL_STATUS_OKAY) ||
            (d->m_wants_to_finish && (flush != TDEFL_FINISH)) || (pIn_buf_size && *pIn_buf_size && !pIn_buf) || (pOut_buf_size && *pOut_buf_size && !pOut_buf))
        {
            if (pIn_buf_size)
                *pIn_buf_size = 0;
            if (pOut_buf_size)
                *pOut_buf_size = 0;
            return (d->m_prev_return_status = TDEFL_STATUS_BAD_PARAM);
        }
        d->m_wants_to_finish |= (flush == TDEFL_FINISH);

        if ((d->m_output_flush_remaining) || (d->m_finished))
            return (d->m_prev_return_status = tdefl_flush_output_buffer(d));

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES &&MINIZ_LITTLE_ENDIAN
        if (((d->m_flags & TDEFL_MAX_PROBES_MASK) == 1) &&
            ((d->m_flags & TDEFL_GREEDY_PARSING_FLAG) != 0) &&
            ((d->m_flags & (TDEFL_FILTER_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS | TDEFL_RLE_MATCHES)) == 0))
        {
            if (!tdefl_compress_fast(d))
                return d->m_prev_return_status;
        }
        else
#endif /* #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN */
        {
            if (!tdefl_compress_normal(d))
                return d->m_prev_return_status;
        }

        if ((d->m_flags & (TDEFL_WRITE_ZLIB_HEADER | TDEFL_COMPUTE_ADLER32)) && (pIn_buf))
            d->m_adler32 = (mz_uint32)mz_adler32(d->m_adler32, (const mz_uint8*)pIn_buf, d->m_pSrc - (const mz_uint8*)pIn_buf);

        if ((flush) && (!d->m_lookahead_size) && (!d->m_src_buf_left) && (!d->m_output_flush_remaining))
        {
            if (tdefl_flush_block(d, flush) < 0)
                return d->m_prev_return_status;
            d->m_finished = (flush == TDEFL_FINISH);
            if (flush == TDEFL_FULL_FLUSH)
            {
                MZ_CLEAR_OBJ(d->m_hash);
                MZ_CLEAR_OBJ(d->m_next);
                d->m_dict_size = 0;
            }
        }

        return (d->m_prev_return_status = tdefl_flush_output_buffer(d));
    }

    tdefl_status tdefl_compress_buffer(tdefl_compressor* d, const void* pIn_buf, size_t in_buf_size, tdefl_flush flush)
    {
        MZ_ASSERT(d->m_pPut_buf_func);
        return tdefl_compress(d, pIn_buf, &in_buf_size, NULL, NULL, flush);
    }

    tdefl_status tdefl_init(tdefl_compressor* d, tdefl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags)
    {
        d->m_pPut_buf_func = pPut_buf_func;
        d->m_pPut_buf_user = pPut_buf_user;
        d->m_flags = (mz_uint)(flags);
        d->m_max_probes[0] = 1 + ((flags & 0xFFF) + 2) / 3;
        d->m_greedy_parsing = (flags & TDEFL_GREEDY_PARSING_FLAG) != 0;
        d->m_max_probes[1] = 1 + (((flags & 0xFFF) >> 2) + 2) / 3;
        if (!(flags & TDEFL_NONDETERMINISTIC_PARSING_FLAG))
            MZ_CLEAR_OBJ(d->m_hash);
        d->m_lookahead_pos = d->m_lookahead_size = d->m_dict_size = d->m_total_lz_bytes = d->m_lz_code_buf_dict_pos = d->m_bits_in = 0;
        d->m_output_flush_ofs = d->m_output_flush_remaining = d->m_finished = d->m_block_index = d->m_bit_buffer = d->m_wants_to_finish = 0;
        d->m_pLZ_code_buf = d->m_lz_code_buf + 1;
        d->m_pLZ_flags = d->m_lz_code_buf;
        d->m_num_flags_left = 8;
        d->m_pOutput_buf = d->m_output_buf;
        d->m_pOutput_buf_end = d->m_output_buf;
        d->m_prev_return_status = TDEFL_STATUS_OKAY;
        d->m_saved_match_dist = d->m_saved_match_len = d->m_saved_lit = 0;
        d->m_adler32 = 1;
        d->m_pIn_buf = NULL;
        d->m_pOut_buf = NULL;
        d->m_pIn_buf_size = NULL;
        d->m_pOut_buf_size = NULL;
        d->m_flush = TDEFL_NO_FLUSH;
        d->m_pSrc = NULL;
        d->m_src_buf_left = 0;
        d->m_out_buf_ofs = 0;
        memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
        memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);
        return TDEFL_STATUS_OKAY;
    }

    tdefl_status tdefl_get_prev_return_status(tdefl_compressor* d)
    {
        return d->m_prev_return_status;
    }

    mz_uint32 tdefl_get_adler32(tdefl_compressor* d)
    {
        return d->m_adler32;
    }

    mz_bool tdefl_compress_mem_to_output(const void* pBuf, size_t buf_len, tdefl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags)
    {
        tdefl_compressor* pComp;
        mz_bool succeeded;
        if (((buf_len) && (!pBuf)) || (!pPut_buf_func))
            return MZ_FALSE;
        pComp = (tdefl_compressor*)MZ_MALLOC(sizeof(tdefl_compressor));
        if (!pComp)
            return MZ_FALSE;
        succeeded = (tdefl_init(pComp, pPut_buf_func, pPut_buf_user, flags) == TDEFL_STATUS_OKAY);
        succeeded = succeeded && (tdefl_compress_buffer(pComp, pBuf, buf_len, TDEFL_FINISH) == TDEFL_STATUS_DONE);
        MZ_FREE(pComp);
        return succeeded;
    }

    typedef struct
    {
        size_t m_size, m_capacity;
        mz_uint8* m_pBuf;
        mz_bool m_expandable;
    } tdefl_output_buffer;

    static mz_bool tdefl_output_buffer_putter(const void* pBuf, int len, void* pUser)
    {
        tdefl_output_buffer* p = (tdefl_output_buffer*)pUser;
        size_t new_size = p->m_size + len;
        if (new_size > p->m_capacity)
        {
            size_t new_capacity = p->m_capacity;
            mz_uint8* pNew_buf;
            if (!p->m_expandable)
                return MZ_FALSE;
            do
            {
                new_capacity = MZ_MAX(128U, new_capacity << 1U);
            } while (new_size > new_capacity);
            pNew_buf = (mz_uint8*)MZ_REALLOC(p->m_pBuf, new_capacity);
            if (!pNew_buf)
                return MZ_FALSE;
            p->m_pBuf = pNew_buf;
            p->m_capacity = new_capacity;
        }
        memcpy((mz_uint8*)p->m_pBuf + p->m_size, pBuf, len);
        p->m_size = new_size;
        return MZ_TRUE;
    }

    void* tdefl_compress_mem_to_heap(const void* pSrc_buf, size_t src_buf_len, size_t* pOut_len, int flags)
    {
        tdefl_output_buffer out_buf;
        MZ_CLEAR_OBJ(out_buf);
        if (!pOut_len)
            return MZ_FALSE;
        else
            *pOut_len = 0;
        out_buf.m_expandable = MZ_TRUE;
        if (!tdefl_compress_mem_to_output(pSrc_buf, src_buf_len, tdefl_output_buffer_putter, &out_buf, flags))
            return NULL;
        *pOut_len = out_buf.m_size;
        return out_buf.m_pBuf;
    }

    size_t tdefl_compress_mem_to_mem(void* pOut_buf, size_t out_buf_len, const void* pSrc_buf, size_t src_buf_len, int flags)
    {
        tdefl_output_buffer out_buf;
        MZ_CLEAR_OBJ(out_buf);
        if (!pOut_buf)
            return 0;
        out_buf.m_pBuf = (mz_uint8*)pOut_buf;
        out_buf.m_capacity = out_buf_len;
        if (!tdefl_compress_mem_to_output(pSrc_buf, src_buf_len, tdefl_output_buffer_putter, &out_buf, flags))
            return 0;
        return out_buf.m_size;
    }

    static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };

    /* level may actually range from [0,10] (10 is a "hidden" max level, where we want a bit more compression and it's fine if throughput to fall off a cliff on some files). */
    mz_uint tdefl_create_comp_flags_from_zip_params(int level, int window_bits, int strategy)
    {
        mz_uint comp_flags = s_tdefl_num_probes[(level >= 0) ? MZ_MIN(10, level) : MZ_DEFAULT_LEVEL] | ((level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
        if (window_bits > 0)
            comp_flags |= TDEFL_WRITE_ZLIB_HEADER;

        if (!level)
            comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
        else if (strategy == MZ_FILTERED)
            comp_flags |= TDEFL_FILTER_MATCHES;
        else if (strategy == MZ_HUFFMAN_ONLY)
            comp_flags &= ~TDEFL_MAX_PROBES_MASK;
        else if (strategy == MZ_FIXED)
            comp_flags |= TDEFL_FORCE_ALL_STATIC_BLOCKS;
        else if (strategy == MZ_RLE)
            comp_flags |= TDEFL_RLE_MATCHES;

        return comp_flags;
    }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4204) /* nonstandard extension used : non-constant aggregate initializer (also supported by GNU C and C99, so no big deal) */
#endif

    /* Simple PNG writer function by Alex Evans, 2011. Released into the public domain: https://gist.github.com/908299, more context at
     http://altdevblogaday.org/2011/04/06/a-smaller-jpg-encoder/.
     This is actually a modification of Alex's original code so PNG files generated by this function pass pngcheck. */
    void* tdefl_write_image_to_png_file_in_memory_ex(const void* pImage, int w, int h, int num_chans, size_t* pLen_out, mz_uint level, mz_bool flip)
    {
        /* Using a local copy of this array here in case MINIZ_NO_ZLIB_APIS was defined. */
        static const mz_uint s_tdefl_png_num_probes[11] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };
        tdefl_compressor* pComp = (tdefl_compressor*)MZ_MALLOC(sizeof(tdefl_compressor));
        tdefl_output_buffer out_buf;
        int i, bpl = w * num_chans, y, z;
        mz_uint32 c;
        *pLen_out = 0;
        if (!pComp)
            return NULL;
        MZ_CLEAR_OBJ(out_buf);
        out_buf.m_expandable = MZ_TRUE;
        out_buf.m_capacity = 57 + MZ_MAX(64, (1 + bpl) * h);
        if (NULL == (out_buf.m_pBuf = (mz_uint8*)MZ_MALLOC(out_buf.m_capacity)))
        {
            MZ_FREE(pComp);
            return NULL;
        }
        /* write dummy header */
        for (z = 41; z; --z)
            tdefl_output_buffer_putter(&z, 1, &out_buf);
        /* compress image data */
        tdefl_init(pComp, tdefl_output_buffer_putter, &out_buf, s_tdefl_png_num_probes[MZ_MIN(10, level)] | TDEFL_WRITE_ZLIB_HEADER);
        for (y = 0; y < h; ++y)
        {
            tdefl_compress_buffer(pComp, &z, 1, TDEFL_NO_FLUSH);
            tdefl_compress_buffer(pComp, (mz_uint8*)pImage + (flip ? (h - 1 - y) : y) * bpl, bpl, TDEFL_NO_FLUSH);
        }
        if (tdefl_compress_buffer(pComp, NULL, 0, TDEFL_FINISH) != TDEFL_STATUS_DONE)
        {
            MZ_FREE(pComp);
            MZ_FREE(out_buf.m_pBuf);
            return NULL;
        }
        /* write real header */
        *pLen_out = out_buf.m_size - 41;
        {
            static const mz_uint8 chans[] = { 0x00, 0x00, 0x04, 0x02, 0x06 };
            mz_uint8 pnghdr[41] = { 0x89, 0x50, 0x4e, 0x47, 0x0d,
                                    0x0a, 0x1a, 0x0a, 0x00, 0x00,
                                    0x00, 0x0d, 0x49, 0x48, 0x44,
                                    0x52, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x08,
                                    0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x49, 0x44, 0x41,
                                    0x54 };
            pnghdr[18] = (mz_uint8)(w >> 8);
            pnghdr[19] = (mz_uint8)w;
            pnghdr[22] = (mz_uint8)(h >> 8);
            pnghdr[22] = (mz_uint8)h;
            pnghdr[25] = chans[num_chans];
            pnghdr[33] = (mz_uint8)(*pLen_out >> 24);
            pnghdr[34] = (mz_uint8)(*pLen_out >> 16);
            pnghdr[35] = (mz_uint8)(*pLen_out >> 8);
            pnghdr[36] = (mz_uint8)*pLen_out;
            c = (mz_uint32)mz_crc32(MZ_CRC32_INIT, pnghdr + 12, 17);
            for (i = 0; i < 4; ++i, c <<= 8)
                ((mz_uint8*)(pnghdr + 29))[i] = (mz_uint8)(c >> 24);
            memcpy(out_buf.m_pBuf, pnghdr, 41);
        }
        /* write footer (IDAT CRC-32, followed by IEND chunk) */
        if (!tdefl_output_buffer_putter("\0\0\0\0\0\0\0\0\x49\x45\x4e\x44\xae\x42\x60\x82", 16, &out_buf))
        {
            *pLen_out = 0;
            MZ_FREE(pComp);
            MZ_FREE(out_buf.m_pBuf);
            return NULL;
        }
        c = (mz_uint32)mz_crc32(MZ_CRC32_INIT, out_buf.m_pBuf + 41 - 4, *pLen_out + 4);
        for (i = 0; i < 4; ++i, c <<= 8)
            (out_buf.m_pBuf + out_buf.m_size - 16)[i] = (mz_uint8)(c >> 24);
        /* compute final size of file, grab compressed data buffer and return */
        *pLen_out += 57;
        MZ_FREE(pComp);
        return out_buf.m_pBuf;
    }
    void* tdefl_write_image_to_png_file_in_memory(const void* pImage, int w, int h, int num_chans, size_t* pLen_out)
    {
        /* Level 6 corresponds to TDEFL_DEFAULT_MAX_PROBES or MZ_DEFAULT_LEVEL (but we can't depend on MZ_DEFAULT_LEVEL being available in case the zlib API's where #defined out) */
        return tdefl_write_image_to_png_file_in_memory_ex(pImage, w, h, num_chans, pLen_out, 6, MZ_FALSE);
    }

    /* Allocate the tdefl_compressor and tinfl_decompressor structures in C so that */
    /* non-C language bindings to tdefL_ and tinfl_ API don't need to worry about */
    /* structure size and allocation mechanism. */
    tdefl_compressor* tdefl_compressor_alloc()
    {
        return (tdefl_compressor*)MZ_MALLOC(sizeof(tdefl_compressor));
    }

    void tdefl_compressor_free(tdefl_compressor* pComp)
    {
        MZ_FREE(pComp);
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __cplusplus
}
#endif
/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/



#ifdef __cplusplus
extern "C" {
#endif

    /* ------------------- Low-level Decompression (completely independent from all compression API's) */

#define TINFL_MEMCPY(d, s, l) memcpy(d, s, l)
#define TINFL_MEMSET(p, c, l) memset(p, c, l)

#define TINFL_CR_BEGIN  \
    switch (r->m_state) \
    {                   \
        case 0:
#define TINFL_CR_RETURN(state_index, result) \
    do                                       \
    {                                        \
        status = result;                     \
        r->m_state = state_index;            \
        goto common_exit;                    \
        case state_index:                    \
            ;                                \
    }                                        \
    MZ_MACRO_END
#define TINFL_CR_RETURN_FOREVER(state_index, result) \
    do                                               \
    {                                                \
        for (;;)                                     \
        {                                            \
            TINFL_CR_RETURN(state_index, result);    \
        }                                            \
    }                                                \
    MZ_MACRO_END
#define TINFL_CR_FINISH }

#define TINFL_GET_BYTE(state_index, c)                                                                                                                          \
    do                                                                                                                                                          \
    {                                                                                                                                                           \
        while (pIn_buf_cur >= pIn_buf_end)                                                                                                                      \
        {                                                                                                                                                       \
            TINFL_CR_RETURN(state_index, (decomp_flags &TINFL_FLAG_HAS_MORE_INPUT) ? TINFL_STATUS_NEEDS_MORE_INPUT : TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS); \
        }                                                                                                                                                       \
        c = *pIn_buf_cur++;                                                                                                                                     \
    }                                                                                                                                                           \
    MZ_MACRO_END

#define TINFL_NEED_BITS(state_index, n)                \
    do                                                 \
    {                                                  \
        mz_uint c;                                     \
        TINFL_GET_BYTE(state_index, c);                \
        bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); \
        num_bits += 8;                                 \
    } while (num_bits < (mz_uint)(n))
#define TINFL_SKIP_BITS(state_index, n)      \
    do                                       \
    {                                        \
        if (num_bits < (mz_uint)(n))         \
        {                                    \
            TINFL_NEED_BITS(state_index, n); \
        }                                    \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }                                        \
    MZ_MACRO_END
#define TINFL_GET_BITS(state_index, b, n)    \
    do                                       \
    {                                        \
        if (num_bits < (mz_uint)(n))         \
        {                                    \
            TINFL_NEED_BITS(state_index, n); \
        }                                    \
        b = bit_buf & ((1 << (n)) - 1);      \
        bit_buf >>= (n);                     \
        num_bits -= (n);                     \
    }                                        \
    MZ_MACRO_END

/* TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2. */
/* It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a */
/* Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the */
/* bit buffer contains >=15 bits (deflate's max. Huffman code size). */
#define TINFL_HUFF_BITBUF_FILL(state_index, pHuff)                             \
    do                                                                         \
    {                                                                          \
        temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)];     \
        if (temp >= 0)                                                         \
        {                                                                      \
            code_len = temp >> 9;                                              \
            if ((code_len) && (num_bits >= code_len))                          \
                break;                                                         \
        }                                                                      \
        else if (num_bits > TINFL_FAST_LOOKUP_BITS)                            \
        {                                                                      \
            code_len = TINFL_FAST_LOOKUP_BITS;                                 \
            do                                                                 \
            {                                                                  \
                temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
            } while ((temp < 0) && (num_bits >= (code_len + 1)));              \
            if (temp >= 0)                                                     \
                break;                                                         \
        }                                                                      \
        TINFL_GET_BYTE(state_index, c);                                        \
        bit_buf |= (((tinfl_bit_buf_t)c) << num_bits);                         \
        num_bits += 8;                                                         \
    } while (num_bits < 15);

/* TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read */
/* beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully */
/* decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32. */
/* The slow path is only executed at the very end of the input buffer. */
/* v1.16: The original macro handled the case at the very end of the passed-in input buffer, but we also need to handle the case where the user passes in 1+zillion bytes */
/* following the deflate data and our non-conservative read-ahead path won't kick in here on this code. This is much trickier. */
#define TINFL_HUFF_DECODE(state_index, sym, pHuff)                                                                                  \
    do                                                                                                                              \
    {                                                                                                                               \
        int temp;                                                                                                                   \
        mz_uint code_len, c;                                                                                                        \
        if (num_bits < 15)                                                                                                          \
        {                                                                                                                           \
            if ((pIn_buf_end - pIn_buf_cur) < 2)                                                                                    \
            {                                                                                                                       \
                TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                         \
            }                                                                                                                       \
            else                                                                                                                    \
            {                                                                                                                       \
                bit_buf |= (((tinfl_bit_buf_t)pIn_buf_cur[0]) << num_bits) | (((tinfl_bit_buf_t)pIn_buf_cur[1]) << (num_bits + 8)); \
                pIn_buf_cur += 2;                                                                                                   \
                num_bits += 16;                                                                                                     \
            }                                                                                                                       \
        }                                                                                                                           \
        if ((temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)                                               \
            code_len = temp >> 9, temp &= 511;                                                                                      \
        else                                                                                                                        \
        {                                                                                                                           \
            code_len = TINFL_FAST_LOOKUP_BITS;                                                                                      \
            do                                                                                                                      \
            {                                                                                                                       \
                temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                                                      \
            } while (temp < 0);                                                                                                     \
        }                                                                                                                           \
        sym = temp;                                                                                                                 \
        bit_buf >>= code_len;                                                                                                       \
        num_bits -= code_len;                                                                                                       \
    }                                                                                                                               \
    MZ_MACRO_END

    tinfl_status tinfl_decompress(tinfl_decompressor* r, const mz_uint8* pIn_buf_next, size_t* pIn_buf_size, mz_uint8* pOut_buf_start, mz_uint8* pOut_buf_next, size_t* pOut_buf_size, const mz_uint32 decomp_flags)
    {
        static const int s_length_base[31] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };
        static const int s_length_extra[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
        static const int s_dist_base[32] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
        static const int s_dist_extra[32] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
        static const mz_uint8 s_length_dezigzag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
        static const int s_min_table_sizes[3] = { 257, 1, 4 };

        tinfl_status status = TINFL_STATUS_FAILED;
        mz_uint32 num_bits, dist, counter, num_extra;
        tinfl_bit_buf_t bit_buf;
        const mz_uint8* pIn_buf_cur = pIn_buf_next, * const pIn_buf_end = pIn_buf_next + *pIn_buf_size;
        mz_uint8* pOut_buf_cur = pOut_buf_next, * const pOut_buf_end = pOut_buf_next + *pOut_buf_size;
        size_t out_buf_size_mask = (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOut_buf_next - pOut_buf_start) + *pOut_buf_size) - 1, dist_from_out_buf_start;

        /* Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter). */
        if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start))
        {
            *pIn_buf_size = *pOut_buf_size = 0;
            return TINFL_STATUS_BAD_PARAM;
        }

        num_bits = r->m_num_bits;
        bit_buf = r->m_bit_buf;
        dist = r->m_dist;
        counter = r->m_counter;
        num_extra = r->m_num_extra;
        dist_from_out_buf_start = r->m_dist_from_out_buf_start;
        TINFL_CR_BEGIN

            bit_buf = num_bits = dist = counter = num_extra = r->m_zhdr0 = r->m_zhdr1 = 0;
        r->m_z_adler32 = r->m_check_adler32 = 1;
        if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
        {
            TINFL_GET_BYTE(1, r->m_zhdr0);
            TINFL_GET_BYTE(2, r->m_zhdr1);
            counter = (((r->m_zhdr0 * 256 + r->m_zhdr1) % 31 != 0) || (r->m_zhdr1 & 32) || ((r->m_zhdr0 & 15) != 8));
            if (!(decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
                counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (size_t)(1U << (8U + (r->m_zhdr0 >> 4)))));
            if (counter)
            {
                TINFL_CR_RETURN_FOREVER(36, TINFL_STATUS_FAILED);
            }
        }

        do
        {
            TINFL_GET_BITS(3, r->m_final, 3);
            r->m_type = r->m_final >> 1;
            if (r->m_type == 0)
            {
                TINFL_SKIP_BITS(5, num_bits & 7);
                for (counter = 0; counter < 4; ++counter)
                {
                    if (num_bits)
                        TINFL_GET_BITS(6, r->m_raw_header[counter], 8);
                    else
                        TINFL_GET_BYTE(7, r->m_raw_header[counter]);
                }
                if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (mz_uint)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8))))
                {
                    TINFL_CR_RETURN_FOREVER(39, TINFL_STATUS_FAILED);
                }
                while ((counter) && (num_bits))
                {
                    TINFL_GET_BITS(51, dist, 8);
                    while (pOut_buf_cur >= pOut_buf_end)
                    {
                        TINFL_CR_RETURN(52, TINFL_STATUS_HAS_MORE_OUTPUT);
                    }
                    *pOut_buf_cur++ = (mz_uint8)dist;
                    counter--;
                }
                while (counter)
                {
                    size_t n;
                    while (pOut_buf_cur >= pOut_buf_end)
                    {
                        TINFL_CR_RETURN(9, TINFL_STATUS_HAS_MORE_OUTPUT);
                    }
                    while (pIn_buf_cur >= pIn_buf_end)
                    {
                        TINFL_CR_RETURN(38, (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) ? TINFL_STATUS_NEEDS_MORE_INPUT : TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS);
                    }
                    n = MZ_MIN(MZ_MIN((size_t)(pOut_buf_end - pOut_buf_cur), (size_t)(pIn_buf_end - pIn_buf_cur)), counter);
                    TINFL_MEMCPY(pOut_buf_cur, pIn_buf_cur, n);
                    pIn_buf_cur += n;
                    pOut_buf_cur += n;
                    counter -= (mz_uint)n;
                }
            }
            else if (r->m_type == 3)
            {
                TINFL_CR_RETURN_FOREVER(10, TINFL_STATUS_FAILED);
            }
            else
            {
                if (r->m_type == 1)
                {
                    mz_uint8* p = r->m_tables[0].m_code_size;
                    mz_uint i;
                    r->m_table_sizes[0] = 288;
                    r->m_table_sizes[1] = 32;
                    TINFL_MEMSET(r->m_tables[1].m_code_size, 5, 32);
                    for (i = 0; i <= 143; ++i)
                        *p++ = 8;
                    for (; i <= 255; ++i)
                        *p++ = 9;
                    for (; i <= 279; ++i)
                        *p++ = 7;
                    for (; i <= 287; ++i)
                        *p++ = 8;
                }
                else
                {
                    for (counter = 0; counter < 3; counter++)
                    {
                        TINFL_GET_BITS(11, r->m_table_sizes[counter], "\05\05\04"[counter]);
                        r->m_table_sizes[counter] += s_min_table_sizes[counter];
                    }
                    MZ_CLEAR_OBJ(r->m_tables[2].m_code_size);
                    for (counter = 0; counter < r->m_table_sizes[2]; counter++)
                    {
                        mz_uint s;
                        TINFL_GET_BITS(14, s, 3);
                        r->m_tables[2].m_code_size[s_length_dezigzag[counter]] = (mz_uint8)s;
                    }
                    r->m_table_sizes[2] = 19;
                }
                for (; (int)r->m_type >= 0; r->m_type--)
                {
                    int tree_next, tree_cur;
                    tinfl_huff_table* pTable;
                    mz_uint i, j, used_syms, total, sym_index, next_code[17], total_syms[16];
                    pTable = &r->m_tables[r->m_type];
                    MZ_CLEAR_OBJ(total_syms);
                    MZ_CLEAR_OBJ(pTable->m_look_up);
                    MZ_CLEAR_OBJ(pTable->m_tree);
                    for (i = 0; i < r->m_table_sizes[r->m_type]; ++i)
                        total_syms[pTable->m_code_size[i]]++;
                    used_syms = 0, total = 0;
                    next_code[0] = next_code[1] = 0;
                    for (i = 1; i <= 15; ++i)
                    {
                        used_syms += total_syms[i];
                        next_code[i + 1] = (total = ((total + total_syms[i]) << 1));
                    }
                    if ((65536 != total) && (used_syms > 1))
                    {
                        TINFL_CR_RETURN_FOREVER(35, TINFL_STATUS_FAILED);
                    }
                    for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
                    {
                        mz_uint rev_code = 0, l, cur_code, code_size = pTable->m_code_size[sym_index];
                        if (!code_size)
                            continue;
                        cur_code = next_code[code_size]++;
                        for (l = code_size; l > 0; l--, cur_code >>= 1)
                            rev_code = (rev_code << 1) | (cur_code & 1);
                        if (code_size <= TINFL_FAST_LOOKUP_BITS)
                        {
                            mz_int16 k = (mz_int16)((code_size << 9) | sym_index);
                            while (rev_code < TINFL_FAST_LOOKUP_SIZE)
                            {
                                pTable->m_look_up[rev_code] = k;
                                rev_code += (1 << code_size);
                            }
                            continue;
                        }
                        if (0 == (tree_cur = pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)]))
                        {
                            pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)] = (mz_int16)tree_next;
                            tree_cur = tree_next;
                            tree_next -= 2;
                        }
                        rev_code >>= (TINFL_FAST_LOOKUP_BITS - 1);
                        for (j = code_size; j > (TINFL_FAST_LOOKUP_BITS + 1); j--)
                        {
                            tree_cur -= ((rev_code >>= 1) & 1);
                            if (!pTable->m_tree[-tree_cur - 1])
                            {
                                pTable->m_tree[-tree_cur - 1] = (mz_int16)tree_next;
                                tree_cur = tree_next;
                                tree_next -= 2;
                            }
                            else
                                tree_cur = pTable->m_tree[-tree_cur - 1];
                        }
                        tree_cur -= ((rev_code >>= 1) & 1);
                        pTable->m_tree[-tree_cur - 1] = (mz_int16)sym_index;
                    }
                    if (r->m_type == 2)
                    {
                        for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]);)
                        {
                            mz_uint s;
                            TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                            if (dist < 16)
                            {
                                r->m_len_codes[counter++] = (mz_uint8)dist;
                                continue;
                            }
                            if ((dist == 16) && (!counter))
                            {
                                TINFL_CR_RETURN_FOREVER(17, TINFL_STATUS_FAILED);
                            }
                            num_extra = "\02\03\07"[dist - 16];
                            TINFL_GET_BITS(18, s, num_extra);
                            s += "\03\03\013"[dist - 16];
                            TINFL_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s);
                            counter += s;
                        }
                        if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
                        {
                            TINFL_CR_RETURN_FOREVER(21, TINFL_STATUS_FAILED);
                        }
                        TINFL_MEMCPY(r->m_tables[0].m_code_size, r->m_len_codes, r->m_table_sizes[0]);
                        TINFL_MEMCPY(r->m_tables[1].m_code_size, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
                    }
                }
                for (;;)
                {
                    mz_uint8* pSrc;
                    for (;;)
                    {
                        if (((pIn_buf_end - pIn_buf_cur) < 4) || ((pOut_buf_end - pOut_buf_cur) < 2))
                        {
                            TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                            if (counter >= 256)
                                break;
                            while (pOut_buf_cur >= pOut_buf_end)
                            {
                                TINFL_CR_RETURN(24, TINFL_STATUS_HAS_MORE_OUTPUT);
                            }
                            *pOut_buf_cur++ = (mz_uint8)counter;
                        }
                        else
                        {
                            int sym2;
                            mz_uint code_len;
#if TINFL_USE_64BIT_BITBUF
                            if (num_bits < 30)
                            {
                                bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE32(pIn_buf_cur)) << num_bits);
                                pIn_buf_cur += 4;
                                num_bits += 32;
                            }
#else
                            if (num_bits < 15)
                            {
                                bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
                                pIn_buf_cur += 2;
                                num_bits += 16;
                            }
#endif
                            if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
                                code_len = sym2 >> 9;
                            else
                            {
                                code_len = TINFL_FAST_LOOKUP_BITS;
                                do
                                {
                                    sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                                } while (sym2 < 0);
                            }
                            counter = sym2;
                            bit_buf >>= code_len;
                            num_bits -= code_len;
                            if (counter & 256)
                                break;

#if !TINFL_USE_64BIT_BITBUF
                            if (num_bits < 15)
                            {
                                bit_buf |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
                                pIn_buf_cur += 2;
                                num_bits += 16;
                            }
#endif
                            if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
                                code_len = sym2 >> 9;
                            else
                            {
                                code_len = TINFL_FAST_LOOKUP_BITS;
                                do
                                {
                                    sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                                } while (sym2 < 0);
                            }
                            bit_buf >>= code_len;
                            num_bits -= code_len;

                            pOut_buf_cur[0] = (mz_uint8)counter;
                            if (sym2 & 256)
                            {
                                pOut_buf_cur++;
                                counter = sym2;
                                break;
                            }
                            pOut_buf_cur[1] = (mz_uint8)sym2;
                            pOut_buf_cur += 2;
                        }
                    }
                    if ((counter &= 511) == 256)
                        break;

                    num_extra = s_length_extra[counter - 257];
                    counter = s_length_base[counter - 257];
                    if (num_extra)
                    {
                        mz_uint extra_bits;
                        TINFL_GET_BITS(25, extra_bits, num_extra);
                        counter += extra_bits;
                    }

                    TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                    num_extra = s_dist_extra[dist];
                    dist = s_dist_base[dist];
                    if (num_extra)
                    {
                        mz_uint extra_bits;
                        TINFL_GET_BITS(27, extra_bits, num_extra);
                        dist += extra_bits;
                    }

                    dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
                    if ((dist > dist_from_out_buf_start) && (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
                    {
                        TINFL_CR_RETURN_FOREVER(37, TINFL_STATUS_FAILED);
                    }

                    pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

                    if ((MZ_MAX(pOut_buf_cur, pSrc) + counter) > pOut_buf_end)
                    {
                        while (counter--)
                        {
                            while (pOut_buf_cur >= pOut_buf_end)
                            {
                                TINFL_CR_RETURN(53, TINFL_STATUS_HAS_MORE_OUTPUT);
                            }
                            *pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
                        }
                        continue;
                    }
#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
                    else if ((counter >= 9) && (counter <= dist))
                    {
                        const mz_uint8* pSrc_end = pSrc + (counter & ~7);
                        do
                        {
                            ((mz_uint32*)pOut_buf_cur)[0] = ((const mz_uint32*)pSrc)[0];
                            ((mz_uint32*)pOut_buf_cur)[1] = ((const mz_uint32*)pSrc)[1];
                            pOut_buf_cur += 8;
                        } while ((pSrc += 8) < pSrc_end);
                        if ((counter &= 7) < 3)
                        {
                            if (counter)
                            {
                                pOut_buf_cur[0] = pSrc[0];
                                if (counter > 1)
                                    pOut_buf_cur[1] = pSrc[1];
                                pOut_buf_cur += counter;
                            }
                            continue;
                        }
                    }
#endif
                    do
                    {
                        pOut_buf_cur[0] = pSrc[0];
                        pOut_buf_cur[1] = pSrc[1];
                        pOut_buf_cur[2] = pSrc[2];
                        pOut_buf_cur += 3;
                        pSrc += 3;
                    } while ((int)(counter -= 3) > 2);
                    if ((int)counter > 0)
                    {
                        pOut_buf_cur[0] = pSrc[0];
                        if ((int)counter > 1)
                            pOut_buf_cur[1] = pSrc[1];
                        pOut_buf_cur += counter;
                    }
                }
            }
        } while (!(r->m_final & 1));

        /* Ensure byte alignment and put back any bytes from the bitbuf if we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
        /* I'm being super conservative here. A number of simplifications can be made to the byte alignment part, and the Adler32 check shouldn't ever need to worry about reading from the bitbuf now. */
        TINFL_SKIP_BITS(32, num_bits & 7);
        while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
        {
            --pIn_buf_cur;
            num_bits -= 8;
        }
        bit_buf &= (tinfl_bit_buf_t)((((mz_uint64)1) << num_bits) - (mz_uint64)1);
        MZ_ASSERT(!num_bits); /* if this assert fires then we've read beyond the end of non-deflate/zlib streams with following data (such as gzip streams). */

        if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER)
        {
            for (counter = 0; counter < 4; ++counter)
            {
                mz_uint s;
                if (num_bits)
                    TINFL_GET_BITS(41, s, 8);
                else
                    TINFL_GET_BYTE(42, s);
                r->m_z_adler32 = (r->m_z_adler32 << 8) | s;
            }
        }
        TINFL_CR_RETURN_FOREVER(34, TINFL_STATUS_DONE);

        TINFL_CR_FINISH

            common_exit :
        /* As long as we aren't telling the caller that we NEED more input to make forward progress: */
        /* Put back any bytes from the bitbuf in case we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
        /* We need to be very careful here to NOT push back any bytes we definitely know we need to make forward progress, though, or we'll lock the caller up into an inf loop. */
        if ((status != TINFL_STATUS_NEEDS_MORE_INPUT) && (status != TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS))
        {
            while ((pIn_buf_cur > pIn_buf_next) && (num_bits >= 8))
            {
                --pIn_buf_cur;
                num_bits -= 8;
            }
        }
        r->m_num_bits = num_bits;
        r->m_bit_buf = bit_buf & (tinfl_bit_buf_t)((((mz_uint64)1) << num_bits) - (mz_uint64)1);
        r->m_dist = dist;
        r->m_counter = counter;
        r->m_num_extra = num_extra;
        r->m_dist_from_out_buf_start = dist_from_out_buf_start;
        *pIn_buf_size = pIn_buf_cur - pIn_buf_next;
        *pOut_buf_size = pOut_buf_cur - pOut_buf_next;
        if ((decomp_flags & (TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_COMPUTE_ADLER32)) && (status >= 0))
        {
            const mz_uint8* ptr = pOut_buf_next;
            size_t buf_len = *pOut_buf_size;
            mz_uint32 i, s1 = r->m_check_adler32 & 0xffff, s2 = r->m_check_adler32 >> 16;
            size_t block_len = buf_len % 5552;
            while (buf_len)
            {
                for (i = 0; i + 7 < block_len; i += 8, ptr += 8)
                {
                    s1 += ptr[0], s2 += s1;
                    s1 += ptr[1], s2 += s1;
                    s1 += ptr[2], s2 += s1;
                    s1 += ptr[3], s2 += s1;
                    s1 += ptr[4], s2 += s1;
                    s1 += ptr[5], s2 += s1;
                    s1 += ptr[6], s2 += s1;
                    s1 += ptr[7], s2 += s1;
                }
                for (; i < block_len; ++i)
                    s1 += *ptr++, s2 += s1;
                s1 %= 65521U, s2 %= 65521U;
                buf_len -= block_len;
                block_len = 5552;
            }
            r->m_check_adler32 = (s2 << 16) + s1;
            if ((status == TINFL_STATUS_DONE) && (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32))
                status = TINFL_STATUS_ADLER32_MISMATCH;
        }
        return status;
    }

    /* Higher level helper functions. */
    void* tinfl_decompress_mem_to_heap(const void* pSrc_buf, size_t src_buf_len, size_t* pOut_len, int flags)
    {
        tinfl_decompressor decomp;
        void* pBuf = NULL, * pNew_buf;
        size_t src_buf_ofs = 0, out_buf_capacity = 0;
        *pOut_len = 0;
        tinfl_init(&decomp);
        for (;;)
        {
            size_t src_buf_size = src_buf_len - src_buf_ofs, dst_buf_size = out_buf_capacity - *pOut_len, new_out_buf_capacity;
            tinfl_status status = tinfl_decompress(&decomp, (const mz_uint8*)pSrc_buf + src_buf_ofs, &src_buf_size, (mz_uint8*)pBuf, pBuf ? (mz_uint8*)pBuf + *pOut_len : NULL, &dst_buf_size,
                (flags & ~TINFL_FLAG_HAS_MORE_INPUT) | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
            if ((status < 0) || (status == TINFL_STATUS_NEEDS_MORE_INPUT))
            {
                MZ_FREE(pBuf);
                *pOut_len = 0;
                return NULL;
            }
            src_buf_ofs += src_buf_size;
            *pOut_len += dst_buf_size;
            if (status == TINFL_STATUS_DONE)
                break;
            new_out_buf_capacity = out_buf_capacity * 2;
            if (new_out_buf_capacity < 128)
                new_out_buf_capacity = 128;
            pNew_buf = MZ_REALLOC(pBuf, new_out_buf_capacity);
            if (!pNew_buf)
            {
                MZ_FREE(pBuf);
                *pOut_len = 0;
                return NULL;
            }
            pBuf = pNew_buf;
            out_buf_capacity = new_out_buf_capacity;
        }
        return pBuf;
    }

    size_t tinfl_decompress_mem_to_mem(void* pOut_buf, size_t out_buf_len, const void* pSrc_buf, size_t src_buf_len, int flags)
    {
        tinfl_decompressor decomp;
        tinfl_status status;
        tinfl_init(&decomp);
        status = tinfl_decompress(&decomp, (const mz_uint8*)pSrc_buf, &src_buf_len, (mz_uint8*)pOut_buf, (mz_uint8*)pOut_buf, &out_buf_len, (flags & ~TINFL_FLAG_HAS_MORE_INPUT) | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
        return (status != TINFL_STATUS_DONE) ? TINFL_DECOMPRESS_MEM_TO_MEM_FAILED : out_buf_len;
    }

    int tinfl_decompress_mem_to_callback(const void* pIn_buf, size_t* pIn_buf_size, tinfl_put_buf_func_ptr pPut_buf_func, void* pPut_buf_user, int flags)
    {
        int result = 0;
        tinfl_decompressor decomp;
        mz_uint8* pDict = (mz_uint8*)MZ_MALLOC(TINFL_LZ_DICT_SIZE);
        size_t in_buf_ofs = 0, dict_ofs = 0;
        if (!pDict)
            return TINFL_STATUS_FAILED;
        tinfl_init(&decomp);
        for (;;)
        {
            size_t in_buf_size = *pIn_buf_size - in_buf_ofs, dst_buf_size = TINFL_LZ_DICT_SIZE - dict_ofs;
            tinfl_status status = tinfl_decompress(&decomp, (const mz_uint8*)pIn_buf + in_buf_ofs, &in_buf_size, pDict, pDict + dict_ofs, &dst_buf_size,
                (flags & ~(TINFL_FLAG_HAS_MORE_INPUT | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)));
            in_buf_ofs += in_buf_size;
            if ((dst_buf_size) && (!(*pPut_buf_func)(pDict + dict_ofs, (int)dst_buf_size, pPut_buf_user)))
                break;
            if (status != TINFL_STATUS_HAS_MORE_OUTPUT)
            {
                result = (status == TINFL_STATUS_DONE);
                break;
            }
            dict_ofs = (dict_ofs + dst_buf_size) & (TINFL_LZ_DICT_SIZE - 1);
        }
        MZ_FREE(pDict);
        *pIn_buf_size = in_buf_ofs;
        return result;
    }

    tinfl_decompressor* tinfl_decompressor_alloc()
    {
        tinfl_decompressor* pDecomp = (tinfl_decompressor*)MZ_MALLOC(sizeof(tinfl_decompressor));
        if (pDecomp)
            tinfl_init(pDecomp);
        return pDecomp;
    }

    void tinfl_decompressor_free(tinfl_decompressor* pDecomp)
    {
        MZ_FREE(pDecomp);
    }

#ifdef __cplusplus
}
#endif
/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * Copyright 2016 Martin Raiber
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#ifndef MINIZ_NO_ARCHIVE_APIS

#ifdef __cplusplus
extern "C" {
#endif

    /* ------------------- .ZIP archive reading */

#ifdef MINIZ_NO_STDIO
#define MZ_FILE void *
#else
#include <sys/stat.h>

#if defined(_MSC_VER) || defined(__MINGW64__)
    static FILE* mz_fopen(const char* pFilename, const char* pMode)
    {
        FILE* pFile = NULL;
        fopen_s(&pFile, pFilename, pMode);
        return pFile;
    }
    static FILE* mz_freopen(const char* pPath, const char* pMode, FILE* pStream)
    {
        FILE* pFile = NULL;
        if (freopen_s(&pFile, pPath, pMode, pStream))
            return NULL;
        return pFile;
    }
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FOPEN mz_fopen
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 _ftelli64
#define MZ_FSEEK64 _fseeki64
#define MZ_FILE_STAT_STRUCT _stat
#define MZ_FILE_STAT _stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN mz_freopen
#define MZ_DELETE_FILE remove
#elif defined(__MINGW32__)
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello64
#define MZ_FSEEK64 fseeko64
#define MZ_FILE_STAT_STRUCT _stat
#define MZ_FILE_STAT _stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#elif defined(__TINYC__)
#ifndef MINIZ_NO_TIME
#include <sys/utime.h>
#endif
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftell
#define MZ_FSEEK64 fseek
#define MZ_FILE_STAT_STRUCT stat
#define MZ_FILE_STAT stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#elif defined(__GNUC__) && _LARGEFILE64_SOURCE
#ifndef MINIZ_NO_TIME
#include <utime.h>
#endif
#define MZ_FOPEN(f, m) fopen64(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello64
#define MZ_FSEEK64 fseeko64
#define MZ_FILE_STAT_STRUCT stat64
#define MZ_FILE_STAT stat64
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(p, m, s) freopen64(p, m, s)
#define MZ_DELETE_FILE remove
#elif defined(__APPLE__) && _LARGEFILE64_SOURCE
#ifndef MINIZ_NO_TIME
#include <utime.h>
#endif
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#define MZ_FTELL64 ftello
#define MZ_FSEEK64 fseeko
#define MZ_FILE_STAT_STRUCT stat
#define MZ_FILE_STAT stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(p, m, s) freopen(p, m, s)
#define MZ_DELETE_FILE remove

#else
#pragma message("Using fopen, ftello, fseeko, stat() etc. path for file I/O - this path may not support large files.")
#ifndef MINIZ_NO_TIME
#include <utime.h>
#endif
#define MZ_FOPEN(f, m) fopen(f, m)
#define MZ_FCLOSE fclose
#define MZ_FREAD fread
#define MZ_FWRITE fwrite
#ifdef __STRICT_ANSI__
#define MZ_FTELL64 ftell
#define MZ_FSEEK64 fseek
#else
#define MZ_FTELL64 ftello
#define MZ_FSEEK64 fseeko
#endif
#define MZ_FILE_STAT_STRUCT stat
#define MZ_FILE_STAT stat
#define MZ_FFLUSH fflush
#define MZ_FREOPEN(f, m, s) freopen(f, m, s)
#define MZ_DELETE_FILE remove
#endif /* #ifdef _MSC_VER */
#endif /* #ifdef MINIZ_NO_STDIO */

#define MZ_TOLOWER(c) ((((c) >= 'A') && ((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))

/* Various ZIP archive enums. To completely avoid cross platform compiler alignment and platform endian issues, miniz.c doesn't use structs for any of this stuff. */
    enum
    {
        /* ZIP archive identifiers and record sizes */
        MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50,
        MZ_ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50,
        MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50,
        MZ_ZIP_LOCAL_DIR_HEADER_SIZE = 30,
        MZ_ZIP_CENTRAL_DIR_HEADER_SIZE = 46,
        MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE = 22,

        /* ZIP64 archive identifier and record sizes */
        MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06064b50,
        MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIG = 0x07064b50,
        MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE = 56,
        MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE = 20,
        MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID = 0x0001,
        MZ_ZIP_DATA_DESCRIPTOR_ID = 0x08074b50,
        MZ_ZIP_DATA_DESCRIPTER_SIZE64 = 24,
        MZ_ZIP_DATA_DESCRIPTER_SIZE32 = 16,

        /* Central directory header record offsets */
        MZ_ZIP_CDH_SIG_OFS = 0,
        MZ_ZIP_CDH_VERSION_MADE_BY_OFS = 4,
        MZ_ZIP_CDH_VERSION_NEEDED_OFS = 6,
        MZ_ZIP_CDH_BIT_FLAG_OFS = 8,
        MZ_ZIP_CDH_METHOD_OFS = 10,
        MZ_ZIP_CDH_FILE_TIME_OFS = 12,
        MZ_ZIP_CDH_FILE_DATE_OFS = 14,
        MZ_ZIP_CDH_CRC32_OFS = 16,
        MZ_ZIP_CDH_COMPRESSED_SIZE_OFS = 20,
        MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS = 24,
        MZ_ZIP_CDH_FILENAME_LEN_OFS = 28,
        MZ_ZIP_CDH_EXTRA_LEN_OFS = 30,
        MZ_ZIP_CDH_COMMENT_LEN_OFS = 32,
        MZ_ZIP_CDH_DISK_START_OFS = 34,
        MZ_ZIP_CDH_INTERNAL_ATTR_OFS = 36,
        MZ_ZIP_CDH_EXTERNAL_ATTR_OFS = 38,
        MZ_ZIP_CDH_LOCAL_HEADER_OFS = 42,

        /* Local directory header offsets */
        MZ_ZIP_LDH_SIG_OFS = 0,
        MZ_ZIP_LDH_VERSION_NEEDED_OFS = 4,
        MZ_ZIP_LDH_BIT_FLAG_OFS = 6,
        MZ_ZIP_LDH_METHOD_OFS = 8,
        MZ_ZIP_LDH_FILE_TIME_OFS = 10,
        MZ_ZIP_LDH_FILE_DATE_OFS = 12,
        MZ_ZIP_LDH_CRC32_OFS = 14,
        MZ_ZIP_LDH_COMPRESSED_SIZE_OFS = 18,
        MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS = 22,
        MZ_ZIP_LDH_FILENAME_LEN_OFS = 26,
        MZ_ZIP_LDH_EXTRA_LEN_OFS = 28,
        MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR = 1 << 3,

        /* End of central directory offsets */
        MZ_ZIP_ECDH_SIG_OFS = 0,
        MZ_ZIP_ECDH_NUM_THIS_DISK_OFS = 4,
        MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS = 6,
        MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 8,
        MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS = 10,
        MZ_ZIP_ECDH_CDIR_SIZE_OFS = 12,
        MZ_ZIP_ECDH_CDIR_OFS_OFS = 16,
        MZ_ZIP_ECDH_COMMENT_SIZE_OFS = 20,

        /* ZIP64 End of central directory locator offsets */
        MZ_ZIP64_ECDL_SIG_OFS = 0,                    /* 4 bytes */
        MZ_ZIP64_ECDL_NUM_DISK_CDIR_OFS = 4,          /* 4 bytes */
        MZ_ZIP64_ECDL_REL_OFS_TO_ZIP64_ECDR_OFS = 8,  /* 8 bytes */
        MZ_ZIP64_ECDL_TOTAL_NUMBER_OF_DISKS_OFS = 16, /* 4 bytes */

        /* ZIP64 End of central directory header offsets */
        MZ_ZIP64_ECDH_SIG_OFS = 0,                       /* 4 bytes */
        MZ_ZIP64_ECDH_SIZE_OF_RECORD_OFS = 4,            /* 8 bytes */
        MZ_ZIP64_ECDH_VERSION_MADE_BY_OFS = 12,          /* 2 bytes */
        MZ_ZIP64_ECDH_VERSION_NEEDED_OFS = 14,           /* 2 bytes */
        MZ_ZIP64_ECDH_NUM_THIS_DISK_OFS = 16,            /* 4 bytes */
        MZ_ZIP64_ECDH_NUM_DISK_CDIR_OFS = 20,            /* 4 bytes */
        MZ_ZIP64_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 24, /* 8 bytes */
        MZ_ZIP64_ECDH_CDIR_TOTAL_ENTRIES_OFS = 32,       /* 8 bytes */
        MZ_ZIP64_ECDH_CDIR_SIZE_OFS = 40,                /* 8 bytes */
        MZ_ZIP64_ECDH_CDIR_OFS_OFS = 48,                 /* 8 bytes */
        MZ_ZIP_VERSION_MADE_BY_DOS_FILESYSTEM_ID = 0,
        MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG = 0x10,
        MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED = 1,
        MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG = 32,
        MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION = 64,
        MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_LOCAL_DIR_IS_MASKED = 8192,
        MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8 = 1 << 11
    };

    typedef struct
    {
        void* m_p;
        size_t m_size, m_capacity;
        mz_uint m_element_size;
    } mz_zip_array;

    struct mz_zip_internal_state_tag
    {
        mz_zip_array m_central_dir;
        mz_zip_array m_central_dir_offsets;
        mz_zip_array m_sorted_central_dir_offsets;

        /* The flags passed in when the archive is initially opened. */
        uint32_t m_init_flags;

        /* MZ_TRUE if the archive has a zip64 end of central directory headers, etc. */
        mz_bool m_zip64;

        /* MZ_TRUE if we found zip64 extended info in the central directory (m_zip64 will also be slammed to true too, even if we didn't find a zip64 end of central dir header, etc.) */
        mz_bool m_zip64_has_extended_info_fields;

        /* These fields are used by the file, FILE, memory, and memory/heap read/write helpers. */
        MZ_FILE* m_pFile;
        mz_uint64 m_file_archive_start_ofs;

        void* m_pMem;
        size_t m_mem_size;
        size_t m_mem_capacity;
    };

#define MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(array_ptr, element_size) (array_ptr)->m_element_size = element_size

#if defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG)
    static MZ_FORCEINLINE mz_uint mz_zip_array_range_check(const mz_zip_array* pArray, mz_uint index)
    {
        MZ_ASSERT(index < pArray->m_size);
        return index;
    }
#define MZ_ZIP_ARRAY_ELEMENT(array_ptr, element_type, index) ((element_type *)((array_ptr)->m_p))[mz_zip_array_range_check(array_ptr, index)]
#else
#define MZ_ZIP_ARRAY_ELEMENT(array_ptr, element_type, index) ((element_type *)((array_ptr)->m_p))[index]
#endif

    static MZ_FORCEINLINE void mz_zip_array_init(mz_zip_array* pArray, mz_uint32 element_size)
    {
        memset(pArray, 0, sizeof(mz_zip_array));
        pArray->m_element_size = element_size;
    }

    static MZ_FORCEINLINE void mz_zip_array_clear(mz_zip_archive* pZip, mz_zip_array* pArray)
    {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pArray->m_p);
        memset(pArray, 0, sizeof(mz_zip_array));
    }

    static mz_bool mz_zip_array_ensure_capacity(mz_zip_archive* pZip, mz_zip_array* pArray, size_t min_new_capacity, mz_uint growing)
    {
        void* pNew_p;
        size_t new_capacity = min_new_capacity;
        MZ_ASSERT(pArray->m_element_size);
        if (pArray->m_capacity >= min_new_capacity)
            return MZ_TRUE;
        if (growing)
        {
            new_capacity = MZ_MAX(1, pArray->m_capacity);
            while (new_capacity < min_new_capacity)
                new_capacity *= 2;
        }
        if (NULL == (pNew_p = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pArray->m_p, pArray->m_element_size, new_capacity)))
            return MZ_FALSE;
        pArray->m_p = pNew_p;
        pArray->m_capacity = new_capacity;
        return MZ_TRUE;
    }

    static MZ_FORCEINLINE mz_bool mz_zip_array_reserve(mz_zip_archive* pZip, mz_zip_array* pArray, size_t new_capacity, mz_uint growing)
    {
        if (new_capacity > pArray->m_capacity)
        {
            if (!mz_zip_array_ensure_capacity(pZip, pArray, new_capacity, growing))
                return MZ_FALSE;
        }
        return MZ_TRUE;
    }

    static MZ_FORCEINLINE mz_bool mz_zip_array_resize(mz_zip_archive* pZip, mz_zip_array* pArray, size_t new_size, mz_uint growing)
    {
        if (new_size > pArray->m_capacity)
        {
            if (!mz_zip_array_ensure_capacity(pZip, pArray, new_size, growing))
                return MZ_FALSE;
        }
        pArray->m_size = new_size;
        return MZ_TRUE;
    }

    static MZ_FORCEINLINE mz_bool mz_zip_array_ensure_room(mz_zip_archive* pZip, mz_zip_array* pArray, size_t n)
    {
        return mz_zip_array_reserve(pZip, pArray, pArray->m_size + n, MZ_TRUE);
    }

    static MZ_FORCEINLINE mz_bool mz_zip_array_push_back(mz_zip_archive* pZip, mz_zip_array* pArray, const void* pElements, size_t n)
    {
        size_t orig_size = pArray->m_size;
        if (!mz_zip_array_resize(pZip, pArray, orig_size + n, MZ_TRUE))
            return MZ_FALSE;
        memcpy((mz_uint8*)pArray->m_p + orig_size * pArray->m_element_size, pElements, n * pArray->m_element_size);
        return MZ_TRUE;
    }

#ifndef MINIZ_NO_TIME
    static MZ_TIME_T mz_zip_dos_to_time_t(int dos_time, int dos_date)
    {
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        tm.tm_isdst = -1;
        tm.tm_year = ((dos_date >> 9) & 127) + 1980 - 1900;
        tm.tm_mon = ((dos_date >> 5) & 15) - 1;
        tm.tm_mday = dos_date & 31;
        tm.tm_hour = (dos_time >> 11) & 31;
        tm.tm_min = (dos_time >> 5) & 63;
        tm.tm_sec = (dos_time << 1) & 62;
        return mktime(&tm);
    }

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
    static void mz_zip_time_t_to_dos_time(MZ_TIME_T time, mz_uint16* pDOS_time, mz_uint16* pDOS_date)
    {
#ifdef _MSC_VER
        struct tm tm_struct;
        struct tm* tm = &tm_struct;
        errno_t err = localtime_s(tm, &time);
        if (err)
        {
            *pDOS_date = 0;
            *pDOS_time = 0;
            return;
        }
#else
        struct tm* tm = localtime(&time);
#endif /* #ifdef _MSC_VER */

        * pDOS_time = (mz_uint16)(((tm->tm_hour) << 11) + ((tm->tm_min) << 5) + ((tm->tm_sec) >> 1));
        *pDOS_date = (mz_uint16)(((tm->tm_year + 1900 - 1980) << 9) + ((tm->tm_mon + 1) << 5) + tm->tm_mday);
    }
#endif /* MINIZ_NO_ARCHIVE_WRITING_APIS */

#ifndef MINIZ_NO_STDIO
#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
    static mz_bool mz_zip_get_file_modified_time(const char* pFilename, MZ_TIME_T* pTime)
    {
        struct MZ_FILE_STAT_STRUCT file_stat;

        /* On Linux with x86 glibc, this call will fail on large files (I think >= 0x80000000 bytes) unless you compiled with _LARGEFILE64_SOURCE. Argh. */
        if (MZ_FILE_STAT(pFilename, &file_stat) != 0)
            return MZ_FALSE;

        *pTime = file_stat.st_mtime;

        return MZ_TRUE;
    }
#endif /* #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS*/

    static mz_bool mz_zip_set_file_times(const char* pFilename, MZ_TIME_T access_time, MZ_TIME_T modified_time)
    {
        struct utimbuf t;

        memset(&t, 0, sizeof(t));
        t.actime = access_time;
        t.modtime = modified_time;

        return !utime(pFilename, &t);
    }
#endif /* #ifndef MINIZ_NO_STDIO */
#endif /* #ifndef MINIZ_NO_TIME */

    static MZ_FORCEINLINE mz_bool mz_zip_set_error(mz_zip_archive* pZip, mz_zip_error err_num)
    {
        if (pZip)
            pZip->m_last_error = err_num;
        return MZ_FALSE;
    }

    static mz_bool mz_zip_reader_init_internal(mz_zip_archive* pZip, mz_uint flags)
    {
        (void)flags;
        if ((!pZip) || (pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!pZip->m_pAlloc)
            pZip->m_pAlloc = miniz_def_alloc_func;
        if (!pZip->m_pFree)
            pZip->m_pFree = miniz_def_free_func;
        if (!pZip->m_pRealloc)
            pZip->m_pRealloc = miniz_def_realloc_func;

        pZip->m_archive_size = 0;
        pZip->m_central_directory_file_ofs = 0;
        pZip->m_total_files = 0;
        pZip->m_last_error = MZ_ZIP_NO_ERROR;

        if (NULL == (pZip->m_pState = (mz_zip_internal_state*)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));
        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));
        pZip->m_pState->m_init_flags = flags;
        pZip->m_pState->m_zip64 = MZ_FALSE;
        pZip->m_pState->m_zip64_has_extended_info_fields = MZ_FALSE;

        pZip->m_zip_mode = MZ_ZIP_MODE_READING;

        return MZ_TRUE;
    }

    static MZ_FORCEINLINE mz_bool mz_zip_reader_filename_less(const mz_zip_array* pCentral_dir_array, const mz_zip_array* pCentral_dir_offsets, mz_uint l_index, mz_uint r_index)
    {
        const mz_uint8* pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), * pE;
        const mz_uint8* pR = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, r_index));
        mz_uint l_len = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS), r_len = MZ_READ_LE16(pR + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        mz_uint8 l = 0, r = 0;
        pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
        pR += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
        pE = pL + MZ_MIN(l_len, r_len);
        while (pL < pE)
        {
            if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
                break;
            pL++;
            pR++;
        }
        return (pL == pE) ? (l_len < r_len) : (l < r);
    }

#define MZ_SWAP_UINT32(a, b) \
    do                       \
    {                        \
        mz_uint32 t = a;     \
        a = b;               \
        b = t;               \
    }                        \
    MZ_MACRO_END

    /* Heap sort of lowercased filenames, used to help accelerate plain central directory searches by mz_zip_reader_locate_file(). (Could also use qsort(), but it could allocate memory.) */
    static void mz_zip_reader_sort_central_dir_offsets_by_filename(mz_zip_archive* pZip)
    {
        mz_zip_internal_state* pState = pZip->m_pState;
        const mz_zip_array* pCentral_dir_offsets = &pState->m_central_dir_offsets;
        const mz_zip_array* pCentral_dir = &pState->m_central_dir;
        mz_uint32* pIndices;
        mz_uint32 start, end;
        const mz_uint32 size = pZip->m_total_files;

        if (size <= 1U)
            return;

        pIndices = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);

        start = (size - 2U) >> 1U;
        for (;;)
        {
            mz_uint64 child, root = start;
            for (;;)
            {
                if ((child = (root << 1U) + 1U) >= size)
                    break;
                child += (((child + 1U) < size) && (mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1U])));
                if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                    break;
                MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
                root = child;
            }
            if (!start)
                break;
            start--;
        }

        end = size - 1;
        while (end > 0)
        {
            mz_uint64 child, root = 0;
            MZ_SWAP_UINT32(pIndices[end], pIndices[0]);
            for (;;)
            {
                if ((child = (root << 1U) + 1U) >= end)
                    break;
                child += (((child + 1U) < end) && mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1U]));
                if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                    break;
                MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
                root = child;
            }
            end--;
        }
    }

    static mz_bool mz_zip_reader_locate_header_sig(mz_zip_archive* pZip, mz_uint32 record_sig, mz_uint32 record_size, mz_int64* pOfs)
    {
        mz_int64 cur_file_ofs;
        mz_uint32 buf_u32[4096 / sizeof(mz_uint32)];
        mz_uint8* pBuf = (mz_uint8*)buf_u32;

        /* Basic sanity checks - reject files which are too small */
        if (pZip->m_archive_size < record_size)
            return MZ_FALSE;

        /* Find the record by scanning the file from the end towards the beginning. */
        cur_file_ofs = MZ_MAX((mz_int64)pZip->m_archive_size - (mz_int64)sizeof(buf_u32), 0);
        for (;;)
        {
            int i, n = (int)MZ_MIN(sizeof(buf_u32), pZip->m_archive_size - cur_file_ofs);

            if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, n) != (mz_uint)n)
                return MZ_FALSE;

            for (i = n - 4; i >= 0; --i)
            {
                mz_uint s = MZ_READ_LE32(pBuf + i);
                if (s == record_sig)
                {
                    if ((pZip->m_archive_size - (cur_file_ofs + i)) >= record_size)
                        break;
                }
            }

            if (i >= 0)
            {
                cur_file_ofs += i;
                break;
            }

            /* Give up if we've searched the entire file, or we've gone back "too far" (~64kb) */
            if ((!cur_file_ofs) || ((pZip->m_archive_size - cur_file_ofs) >= (MZ_UINT16_MAX + record_size)))
                return MZ_FALSE;

            cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
        }

        *pOfs = cur_file_ofs;
        return MZ_TRUE;
    }

    static mz_bool mz_zip_reader_read_central_dir(mz_zip_archive* pZip, mz_uint flags)
    {
        mz_uint cdir_size = 0, cdir_entries_on_this_disk = 0, num_this_disk = 0, cdir_disk_index = 0;
        mz_uint64 cdir_ofs = 0;
        mz_int64 cur_file_ofs = 0;
        const mz_uint8* p;

        mz_uint32 buf_u32[4096 / sizeof(mz_uint32)];
        mz_uint8* pBuf = (mz_uint8*)buf_u32;
        mz_bool sort_central_dir = ((flags & MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == 0);
        mz_uint32 zip64_end_of_central_dir_locator_u32[(MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pZip64_locator = (mz_uint8*)zip64_end_of_central_dir_locator_u32;

        mz_uint32 zip64_end_of_central_dir_header_u32[(MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pZip64_end_of_central_dir = (mz_uint8*)zip64_end_of_central_dir_header_u32;

        mz_uint64 zip64_end_of_central_dir_ofs = 0;

        /* Basic sanity checks - reject files which are too small, and check the first 4 bytes of the file to make sure a local header is there. */
        if (pZip->m_archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);

        if (!mz_zip_reader_locate_header_sig(pZip, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE, &cur_file_ofs))
            return mz_zip_set_error(pZip, MZ_ZIP_FAILED_FINDING_CENTRAL_DIR);

        /* Read and verify the end of central directory record. */
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

        if (MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_SIG_OFS) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);

        if (cur_file_ofs >= (MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE + MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE))
        {
            if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs - MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE, pZip64_locator, MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE) == MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE)
            {
                if (MZ_READ_LE32(pZip64_locator + MZ_ZIP64_ECDL_SIG_OFS) == MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIG)
                {
                    zip64_end_of_central_dir_ofs = MZ_READ_LE64(pZip64_locator + MZ_ZIP64_ECDL_REL_OFS_TO_ZIP64_ECDR_OFS);
                    if (zip64_end_of_central_dir_ofs > (pZip->m_archive_size - MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE))
                        return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);

                    if (pZip->m_pRead(pZip->m_pIO_opaque, zip64_end_of_central_dir_ofs, pZip64_end_of_central_dir, MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE) == MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE)
                    {
                        if (MZ_READ_LE32(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_SIG_OFS) == MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIG)
                        {
                            pZip->m_pState->m_zip64 = MZ_TRUE;
                        }
                    }
                }
            }
        }

        pZip->m_total_files = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS);
        cdir_entries_on_this_disk = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS);
        num_this_disk = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_THIS_DISK_OFS);
        cdir_disk_index = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS);
        cdir_size = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_SIZE_OFS);
        cdir_ofs = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_OFS_OFS);

        if (pZip->m_pState->m_zip64)
        {
            mz_uint32 zip64_total_num_of_disks = MZ_READ_LE32(pZip64_locator + MZ_ZIP64_ECDL_TOTAL_NUMBER_OF_DISKS_OFS);
            mz_uint64 zip64_cdir_total_entries = MZ_READ_LE64(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_CDIR_TOTAL_ENTRIES_OFS);
            mz_uint64 zip64_cdir_total_entries_on_this_disk = MZ_READ_LE64(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS);
            mz_uint64 zip64_size_of_end_of_central_dir_record = MZ_READ_LE64(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_SIZE_OF_RECORD_OFS);
            mz_uint64 zip64_size_of_central_directory = MZ_READ_LE64(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_CDIR_SIZE_OFS);

            if (zip64_size_of_end_of_central_dir_record < (MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE - 12))
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

            if (zip64_total_num_of_disks != 1U)
                return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_MULTIDISK);

            /* Check for miniz's practical limits */
            if (zip64_cdir_total_entries > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);

            pZip->m_total_files = (mz_uint32)zip64_cdir_total_entries;

            if (zip64_cdir_total_entries_on_this_disk > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);

            cdir_entries_on_this_disk = (mz_uint32)zip64_cdir_total_entries_on_this_disk;

            /* Check for miniz's current practical limits (sorry, this should be enough for millions of files) */
            if (zip64_size_of_central_directory > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

            cdir_size = (mz_uint32)zip64_size_of_central_directory;

            num_this_disk = MZ_READ_LE32(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_NUM_THIS_DISK_OFS);

            cdir_disk_index = MZ_READ_LE32(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_NUM_DISK_CDIR_OFS);

            cdir_ofs = MZ_READ_LE64(pZip64_end_of_central_dir + MZ_ZIP64_ECDH_CDIR_OFS_OFS);
        }

        if (pZip->m_total_files != cdir_entries_on_this_disk)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_MULTIDISK);

        if (((num_this_disk | cdir_disk_index) != 0) && ((num_this_disk != 1) || (cdir_disk_index != 1)))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_MULTIDISK);

        if (cdir_size < pZip->m_total_files * MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        if ((cdir_ofs + (mz_uint64)cdir_size) > pZip->m_archive_size)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        pZip->m_central_directory_file_ofs = cdir_ofs;

        if (pZip->m_total_files)
        {
            mz_uint i, n;
            /* Read the entire central directory into a heap block, and allocate another heap block to hold the unsorted central dir file record offsets, and possibly another to hold the sorted indices. */
            if ((!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir, cdir_size, MZ_FALSE)) ||
                (!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir_offsets, pZip->m_total_files, MZ_FALSE)))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

            if (sort_central_dir)
            {
                if (!mz_zip_array_resize(pZip, &pZip->m_pState->m_sorted_central_dir_offsets, pZip->m_total_files, MZ_FALSE))
                    return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (pZip->m_pRead(pZip->m_pIO_opaque, cdir_ofs, pZip->m_pState->m_central_dir.m_p, cdir_size) != cdir_size)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

            /* Now create an index into the central directory file records, do some basic sanity checking on each record */
            p = (const mz_uint8*)pZip->m_pState->m_central_dir.m_p;
            for (n = cdir_size, i = 0; i < pZip->m_total_files; ++i)
            {
                mz_uint total_header_size, disk_index, bit_flags, filename_size, ext_data_size;
                mz_uint64 comp_size, decomp_size, local_header_ofs;

                if ((n < MZ_ZIP_CENTRAL_DIR_HEADER_SIZE) || (MZ_READ_LE32(p) != MZ_ZIP_CENTRAL_DIR_HEADER_SIG))
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, i) = (mz_uint32)(p - (const mz_uint8*)pZip->m_pState->m_central_dir.m_p);

                if (sort_central_dir)
                    MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_sorted_central_dir_offsets, mz_uint32, i) = i;

                comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
                decomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
                local_header_ofs = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);
                filename_size = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
                ext_data_size = MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS);

                if ((!pZip->m_pState->m_zip64_has_extended_info_fields) &&
                    (ext_data_size) &&
                    (MZ_MAX(MZ_MAX(comp_size, decomp_size), local_header_ofs) == MZ_UINT32_MAX))
                {
                    /* Attempt to find zip64 extended information field in the entry's extra data */
                    mz_uint32 extra_size_remaining = ext_data_size;

                    if (extra_size_remaining)
                    {
                        const mz_uint8* pExtra_data = p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_size;

                        do
                        {
                            mz_uint32 field_id;
                            mz_uint32 field_data_size;

                            if (extra_size_remaining < (sizeof(mz_uint16) * 2))
                                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                            field_id = MZ_READ_LE16(pExtra_data);
                            field_data_size = MZ_READ_LE16(pExtra_data + sizeof(mz_uint16));

                            if ((field_data_size + sizeof(mz_uint16) * 2) > extra_size_remaining)
                                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                            if (field_id == MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID)
                            {
                                /* Ok, the archive didn't have any zip64 headers but it uses a zip64 extended information field so mark it as zip64 anyway (this can occur with infozip's zip util when it reads compresses files from stdin). */
                                pZip->m_pState->m_zip64 = MZ_TRUE;
                                pZip->m_pState->m_zip64_has_extended_info_fields = MZ_TRUE;
                                break;
                            }

                            pExtra_data += sizeof(mz_uint16) * 2 + field_data_size;
                            extra_size_remaining = extra_size_remaining - sizeof(mz_uint16) * 2 - field_data_size;
                        } while (extra_size_remaining);
                    }
                }

                /* I've seen archives that aren't marked as zip64 that uses zip64 ext data, argh */
                if ((comp_size != MZ_UINT32_MAX) && (decomp_size != MZ_UINT32_MAX))
                {
                    if (((!MZ_READ_LE32(p + MZ_ZIP_CDH_METHOD_OFS)) && (decomp_size != comp_size)) || (decomp_size && !comp_size))
                        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                }

                disk_index = MZ_READ_LE16(p + MZ_ZIP_CDH_DISK_START_OFS);
                if ((disk_index == MZ_UINT16_MAX) || ((disk_index != num_this_disk) && (disk_index != 1)))
                    return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_MULTIDISK);

                if (comp_size != MZ_UINT32_MAX)
                {
                    if (((mz_uint64)MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS) + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + comp_size) > pZip->m_archive_size)
                        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                }

                bit_flags = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
                if (bit_flags & MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_LOCAL_DIR_IS_MASKED)
                    return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_ENCRYPTION);

                if ((total_header_size = MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS)) > n)
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                n -= total_header_size;
                p += total_header_size;
            }
        }

        if (sort_central_dir)
            mz_zip_reader_sort_central_dir_offsets_by_filename(pZip);

        return MZ_TRUE;
    }

    void mz_zip_zero_struct(mz_zip_archive* pZip)
    {
        if (pZip)
            MZ_CLEAR_OBJ(*pZip);
    }

    static mz_bool mz_zip_reader_end_internal(mz_zip_archive* pZip, mz_bool set_last_error)
    {
        mz_bool status = MZ_TRUE;

        if (!pZip)
            return MZ_FALSE;

        if ((!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        {
            if (set_last_error)
                pZip->m_last_error = MZ_ZIP_INVALID_PARAMETER;

            return MZ_FALSE;
        }

        if (pZip->m_pState)
        {
            mz_zip_internal_state* pState = pZip->m_pState;
            pZip->m_pState = NULL;

            mz_zip_array_clear(pZip, &pState->m_central_dir);
            mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
            mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

#ifndef MINIZ_NO_STDIO
            if (pState->m_pFile)
            {
                if (pZip->m_zip_type == MZ_ZIP_TYPE_FILE)
                {
                    if (MZ_FCLOSE(pState->m_pFile) == EOF)
                    {
                        if (set_last_error)
                            pZip->m_last_error = MZ_ZIP_FILE_CLOSE_FAILED;
                        status = MZ_FALSE;
                    }
                }
                pState->m_pFile = NULL;
            }
#endif /* #ifndef MINIZ_NO_STDIO */

            pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
        }
        pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;

        return status;
    }

    mz_bool mz_zip_reader_end(mz_zip_archive* pZip)
    {
        return mz_zip_reader_end_internal(pZip, MZ_TRUE);
    }
    mz_bool mz_zip_reader_init(mz_zip_archive* pZip, mz_uint64 size, mz_uint flags)
    {
        if ((!pZip) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_reader_init_internal(pZip, flags))
            return MZ_FALSE;

        pZip->m_zip_type = MZ_ZIP_TYPE_USER;
        pZip->m_archive_size = size;

        if (!mz_zip_reader_read_central_dir(pZip, flags))
        {
            mz_zip_reader_end_internal(pZip, MZ_FALSE);
            return MZ_FALSE;
        }

        return MZ_TRUE;
    }

    static size_t mz_zip_mem_read_func(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n)
    {
        mz_zip_archive* pZip = (mz_zip_archive*)pOpaque;
        size_t s = (file_ofs >= pZip->m_archive_size) ? 0 : (size_t)MZ_MIN(pZip->m_archive_size - file_ofs, n);
        memcpy(pBuf, (const mz_uint8*)pZip->m_pState->m_pMem + file_ofs, s);
        return s;
    }

    mz_bool mz_zip_reader_init_mem(mz_zip_archive* pZip, const void* pMem, size_t size, mz_uint flags)
    {
        if (!pMem)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);

        if (!mz_zip_reader_init_internal(pZip, flags))
            return MZ_FALSE;

        pZip->m_zip_type = MZ_ZIP_TYPE_MEMORY;
        pZip->m_archive_size = size;
        pZip->m_pRead = mz_zip_mem_read_func;
        pZip->m_pIO_opaque = pZip;
        pZip->m_pNeeds_keepalive = NULL;

#ifdef __cplusplus
        pZip->m_pState->m_pMem = const_cast<void*>(pMem);
#else
        pZip->m_pState->m_pMem = (void*)pMem;
#endif

        pZip->m_pState->m_mem_size = size;

        if (!mz_zip_reader_read_central_dir(pZip, flags))
        {
            mz_zip_reader_end_internal(pZip, MZ_FALSE);
            return MZ_FALSE;
        }

        return MZ_TRUE;
    }

#ifndef MINIZ_NO_STDIO
    static size_t mz_zip_file_read_func(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n)
    {
        mz_zip_archive* pZip = (mz_zip_archive*)pOpaque;
        mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);

        file_ofs += pZip->m_pState->m_file_archive_start_ofs;

        if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
            return 0;

        return MZ_FREAD(pBuf, 1, n, pZip->m_pState->m_pFile);
    }

    mz_bool mz_zip_reader_init_file(mz_zip_archive* pZip, const char* pFilename, mz_uint32 flags)
    {
        return mz_zip_reader_init_file_v2(pZip, pFilename, flags, 0, 0);
    }

    mz_bool mz_zip_reader_init_file_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint flags, mz_uint64 file_start_ofs, mz_uint64 archive_size)
    {
        mz_uint64 file_size;
        MZ_FILE* pFile;

        if ((!pZip) || (!pFilename) || ((archive_size) && (archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pFile = MZ_FOPEN(pFilename, "rb");
        if (!pFile)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);

        file_size = archive_size;
        if (!file_size)
        {
            if (MZ_FSEEK64(pFile, 0, SEEK_END))
            {
                MZ_FCLOSE(pFile);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_SEEK_FAILED);
            }

            file_size = MZ_FTELL64(pFile);
        }

        /* TODO: Better sanity check archive_size and the # of actual remaining bytes */

        if (file_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);

        if (!mz_zip_reader_init_internal(pZip, flags))
        {
            MZ_FCLOSE(pFile);
            return MZ_FALSE;
        }

        pZip->m_zip_type = MZ_ZIP_TYPE_FILE;
        pZip->m_pRead = mz_zip_file_read_func;
        pZip->m_pIO_opaque = pZip;
        pZip->m_pState->m_pFile = pFile;
        pZip->m_archive_size = file_size;
        pZip->m_pState->m_file_archive_start_ofs = file_start_ofs;

        if (!mz_zip_reader_read_central_dir(pZip, flags))
        {
            mz_zip_reader_end_internal(pZip, MZ_FALSE);
            return MZ_FALSE;
        }

        return MZ_TRUE;
    }

    mz_bool mz_zip_reader_init_cfile(mz_zip_archive* pZip, MZ_FILE* pFile, mz_uint64 archive_size, mz_uint flags)
    {
        mz_uint64 cur_file_ofs;

        if ((!pZip) || (!pFile))
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);

        cur_file_ofs = MZ_FTELL64(pFile);

        if (!archive_size)
        {
            if (MZ_FSEEK64(pFile, 0, SEEK_END))
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_SEEK_FAILED);

            archive_size = MZ_FTELL64(pFile) - cur_file_ofs;

            if (archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
                return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);
        }

        if (!mz_zip_reader_init_internal(pZip, flags))
            return MZ_FALSE;

        pZip->m_zip_type = MZ_ZIP_TYPE_CFILE;
        pZip->m_pRead = mz_zip_file_read_func;

        pZip->m_pIO_opaque = pZip;
        pZip->m_pState->m_pFile = pFile;
        pZip->m_archive_size = archive_size;
        pZip->m_pState->m_file_archive_start_ofs = cur_file_ofs;

        if (!mz_zip_reader_read_central_dir(pZip, flags))
        {
            mz_zip_reader_end_internal(pZip, MZ_FALSE);
            return MZ_FALSE;
        }

        return MZ_TRUE;
    }

#endif /* #ifndef MINIZ_NO_STDIO */

    static MZ_FORCEINLINE const mz_uint8* mz_zip_get_cdh(mz_zip_archive* pZip, mz_uint file_index)
    {
        if ((!pZip) || (!pZip->m_pState) || (file_index >= pZip->m_total_files))
            return NULL;
        return &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
    }

    mz_bool mz_zip_reader_is_file_encrypted(mz_zip_archive* pZip, mz_uint file_index)
    {
        mz_uint m_bit_flag;
        const mz_uint8* p = mz_zip_get_cdh(pZip, file_index);
        if (!p)
        {
            mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return MZ_FALSE;
        }

        m_bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
        return (m_bit_flag & (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION)) != 0;
    }

    mz_bool mz_zip_reader_is_file_supported(mz_zip_archive* pZip, mz_uint file_index)
    {
        mz_uint bit_flag;
        mz_uint method;

        const mz_uint8* p = mz_zip_get_cdh(pZip, file_index);
        if (!p)
        {
            mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return MZ_FALSE;
        }

        method = MZ_READ_LE16(p + MZ_ZIP_CDH_METHOD_OFS);
        bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);

        if ((method != 0) && (method != MZ_DEFLATED))
        {
            mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_METHOD);
            return MZ_FALSE;
        }

        if (bit_flag & (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION))
        {
            mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_ENCRYPTION);
            return MZ_FALSE;
        }

        if (bit_flag & MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG)
        {
            mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_FEATURE);
            return MZ_FALSE;
        }

        return MZ_TRUE;
    }

    mz_bool mz_zip_reader_is_file_a_directory(mz_zip_archive* pZip, mz_uint file_index)
    {
        mz_uint filename_len, attribute_mapping_id, external_attr;
        const mz_uint8* p = mz_zip_get_cdh(pZip, file_index);
        if (!p)
        {
            mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return MZ_FALSE;
        }

        filename_len = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        if (filename_len)
        {
            if (*(p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_len - 1) == '/')
                return MZ_TRUE;
        }

        /* Bugfix: This code was also checking if the internal attribute was non-zero, which wasn't correct. */
        /* Most/all zip writers (hopefully) set DOS file/directory attributes in the low 16-bits, so check for the DOS directory flag and ignore the source OS ID in the created by field. */
        /* FIXME: Remove this check? Is it necessary - we already check the filename. */
        attribute_mapping_id = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_MADE_BY_OFS) >> 8;
        (void)attribute_mapping_id;

        external_attr = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
        if ((external_attr & MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG) != 0)
        {
            return MZ_TRUE;
        }

        return MZ_FALSE;
    }

    static mz_bool mz_zip_file_stat_internal(mz_zip_archive* pZip, mz_uint file_index, const mz_uint8* pCentral_dir_header, mz_zip_archive_file_stat* pStat, mz_bool* pFound_zip64_extra_data)
    {
        mz_uint n;
        const mz_uint8* p = pCentral_dir_header;

        if (pFound_zip64_extra_data)
            *pFound_zip64_extra_data = MZ_FALSE;

        if ((!p) || (!pStat))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        /* Extract fields from the central directory record. */
        pStat->m_file_index = file_index;
        pStat->m_central_dir_ofs = MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index);
        pStat->m_version_made_by = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_MADE_BY_OFS);
        pStat->m_version_needed = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_NEEDED_OFS);
        pStat->m_bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
        pStat->m_method = MZ_READ_LE16(p + MZ_ZIP_CDH_METHOD_OFS);
#ifndef MINIZ_NO_TIME
        pStat->m_time = mz_zip_dos_to_time_t(MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_TIME_OFS), MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_DATE_OFS));
#endif
        pStat->m_crc32 = MZ_READ_LE32(p + MZ_ZIP_CDH_CRC32_OFS);
        pStat->m_comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
        pStat->m_uncomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
        pStat->m_internal_attr = MZ_READ_LE16(p + MZ_ZIP_CDH_INTERNAL_ATTR_OFS);
        pStat->m_external_attr = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
        pStat->m_local_header_ofs = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);

        /* Copy as much of the filename and comment as possible. */
        n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - 1);
        memcpy(pStat->m_filename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
        pStat->m_filename[n] = '\0';

        n = MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS);
        n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE - 1);
        pStat->m_comment_size = n;
        memcpy(pStat->m_comment, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS), n);
        pStat->m_comment[n] = '\0';

        /* Set some flags for convienance */
        pStat->m_is_directory = mz_zip_reader_is_file_a_directory(pZip, file_index);
        pStat->m_is_encrypted = mz_zip_reader_is_file_encrypted(pZip, file_index);
        pStat->m_is_supported = mz_zip_reader_is_file_supported(pZip, file_index);

        /* See if we need to read any zip64 extended information fields. */
        /* Confusingly, these zip64 fields can be present even on non-zip64 archives (Debian zip on a huge files from stdin piped to stdout creates them). */
        if (MZ_MAX(MZ_MAX(pStat->m_comp_size, pStat->m_uncomp_size), pStat->m_local_header_ofs) == MZ_UINT32_MAX)
        {
            /* Attempt to find zip64 extended information field in the entry's extra data */
            mz_uint32 extra_size_remaining = MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS);

            if (extra_size_remaining)
            {
                const mz_uint8* pExtra_data = p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);

                do
                {
                    mz_uint32 field_id;
                    mz_uint32 field_data_size;

                    if (extra_size_remaining < (sizeof(mz_uint16) * 2))
                        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                    field_id = MZ_READ_LE16(pExtra_data);
                    field_data_size = MZ_READ_LE16(pExtra_data + sizeof(mz_uint16));

                    if ((field_data_size + sizeof(mz_uint16) * 2) > extra_size_remaining)
                        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                    if (field_id == MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID)
                    {
                        const mz_uint8* pField_data = pExtra_data + sizeof(mz_uint16) * 2;
                        mz_uint32 field_data_remaining = field_data_size;

                        if (pFound_zip64_extra_data)
                            *pFound_zip64_extra_data = MZ_TRUE;

                        if (pStat->m_uncomp_size == MZ_UINT32_MAX)
                        {
                            if (field_data_remaining < sizeof(mz_uint64))
                                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                            pStat->m_uncomp_size = MZ_READ_LE64(pField_data);
                            pField_data += sizeof(mz_uint64);
                            field_data_remaining -= sizeof(mz_uint64);
                        }

                        if (pStat->m_comp_size == MZ_UINT32_MAX)
                        {
                            if (field_data_remaining < sizeof(mz_uint64))
                                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                            pStat->m_comp_size = MZ_READ_LE64(pField_data);
                            pField_data += sizeof(mz_uint64);
                            field_data_remaining -= sizeof(mz_uint64);
                        }

                        if (pStat->m_local_header_ofs == MZ_UINT32_MAX)
                        {
                            if (field_data_remaining < sizeof(mz_uint64))
                                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                            pStat->m_local_header_ofs = MZ_READ_LE64(pField_data);
                            pField_data += sizeof(mz_uint64);
                            field_data_remaining -= sizeof(mz_uint64);
                        }

                        break;
                    }

                    pExtra_data += sizeof(mz_uint16) * 2 + field_data_size;
                    extra_size_remaining = extra_size_remaining - sizeof(mz_uint16) * 2 - field_data_size;
                } while (extra_size_remaining);
            }
        }

        return MZ_TRUE;
    }

    static MZ_FORCEINLINE mz_bool mz_zip_string_equal(const char* pA, const char* pB, mz_uint len, mz_uint flags)
    {
        mz_uint i;
        if (flags & MZ_ZIP_FLAG_CASE_SENSITIVE)
            return 0 == memcmp(pA, pB, len);
        for (i = 0; i < len; ++i)
            if (MZ_TOLOWER(pA[i]) != MZ_TOLOWER(pB[i]))
                return MZ_FALSE;
        return MZ_TRUE;
    }

    static MZ_FORCEINLINE int mz_zip_filename_compare(const mz_zip_array* pCentral_dir_array, const mz_zip_array* pCentral_dir_offsets, mz_uint l_index, const char* pR, mz_uint r_len)
    {
        const mz_uint8* pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), * pE;
        mz_uint l_len = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        mz_uint8 l = 0, r = 0;
        pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
        pE = pL + MZ_MIN(l_len, r_len);
        while (pL < pE)
        {
            if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
                break;
            pL++;
            pR++;
        }
        return (pL == pE) ? (int)(l_len - r_len) : (l - r);
    }

    static mz_bool mz_zip_locate_file_binary_search(mz_zip_archive* pZip, const char* pFilename, mz_uint32* pIndex)
    {
        mz_zip_internal_state* pState = pZip->m_pState;
        const mz_zip_array* pCentral_dir_offsets = &pState->m_central_dir_offsets;
        const mz_zip_array* pCentral_dir = &pState->m_central_dir;
        mz_uint32* pIndices = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);
        const uint32_t size = pZip->m_total_files;
        const mz_uint filename_len = (mz_uint)strlen(pFilename);

        if (pIndex)
            *pIndex = 0;

        if (size)
        {
            /* yes I could use uint32_t's, but then we would have to add some special case checks in the loop, argh, and */
            /* honestly the major expense here on 32-bit CPU's will still be the filename compare */
            mz_int64 l = 0, h = (mz_int64)size - 1;

            while (l <= h)
            {
                mz_int64 m = l + ((h - l) >> 1);
                uint32_t file_index = pIndices[(uint32_t)m];

                int comp = mz_zip_filename_compare(pCentral_dir, pCentral_dir_offsets, file_index, pFilename, filename_len);
                if (!comp)
                {
                    if (pIndex)
                        *pIndex = file_index;
                    return MZ_TRUE;
                }
                else if (comp < 0)
                    l = m + 1;
                else
                    h = m - 1;
            }
        }

        return mz_zip_set_error(pZip, MZ_ZIP_FILE_NOT_FOUND);
    }

    int mz_zip_reader_locate_file(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags)
    {
        mz_uint32 index;
        if (!mz_zip_reader_locate_file_v2(pZip, pName, pComment, flags, &index))
            return -1;
        else
            return (int)index;
    }

    mz_bool mz_zip_reader_locate_file_v2(mz_zip_archive* pZip, const char* pName, const char* pComment, mz_uint flags, mz_uint32* pIndex)
    {
        mz_uint file_index;
        size_t name_len, comment_len;

        if (pIndex)
            *pIndex = 0;

        if ((!pZip) || (!pZip->m_pState) || (!pName))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        /* See if we can use a binary search */
        if (((pZip->m_pState->m_init_flags & MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == 0) &&
            (pZip->m_zip_mode == MZ_ZIP_MODE_READING) &&
            ((flags & (MZ_ZIP_FLAG_IGNORE_PATH | MZ_ZIP_FLAG_CASE_SENSITIVE)) == 0) && (!pComment) && (pZip->m_pState->m_sorted_central_dir_offsets.m_size))
        {
            return mz_zip_locate_file_binary_search(pZip, pName, pIndex);
        }

        /* Locate the entry by scanning the entire central directory */
        name_len = strlen(pName);
        if (name_len > MZ_UINT16_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        comment_len = pComment ? strlen(pComment) : 0;
        if (comment_len > MZ_UINT16_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        for (file_index = 0; file_index < pZip->m_total_files; file_index++)
        {
            const mz_uint8* pHeader = &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
            mz_uint filename_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS);
            const char* pFilename = (const char*)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
            if (filename_len < name_len)
                continue;
            if (comment_len)
            {
                mz_uint file_extra_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_EXTRA_LEN_OFS), file_comment_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_COMMENT_LEN_OFS);
                const char* pFile_comment = pFilename + filename_len + file_extra_len;
                if ((file_comment_len != comment_len) || (!mz_zip_string_equal(pComment, pFile_comment, file_comment_len, flags)))
                    continue;
            }
            if ((flags & MZ_ZIP_FLAG_IGNORE_PATH) && (filename_len))
            {
                int ofs = filename_len - 1;
                do
                {
                    if ((pFilename[ofs] == '/') || (pFilename[ofs] == '\\') || (pFilename[ofs] == ':'))
                        break;
                } while (--ofs >= 0);
                ofs++;
                pFilename += ofs;
                filename_len -= ofs;
            }
            if ((filename_len == name_len) && (mz_zip_string_equal(pName, pFilename, filename_len, flags)))
            {
                if (pIndex)
                    *pIndex = file_index;
                return MZ_TRUE;
            }
        }

        return mz_zip_set_error(pZip, MZ_ZIP_FILE_NOT_FOUND);
    }

    mz_bool mz_zip_reader_extract_to_mem_no_alloc(mz_zip_archive* pZip, mz_uint file_index, void* pBuf, size_t buf_size, mz_uint flags, void* pUser_read_buf, size_t user_read_buf_size)
    {
        int status = TINFL_STATUS_DONE;
        mz_uint64 needed_size, cur_file_ofs, comp_remaining, out_buf_ofs = 0, read_buf_size, read_buf_ofs = 0, read_buf_avail;
        mz_zip_archive_file_stat file_stat;
        void* pRead_buf;
        mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pLocal_header = (mz_uint8*)local_header_u32;
        tinfl_decompressor inflator;

        if ((!pZip) || (!pZip->m_pState) || ((buf_size) && (!pBuf)) || ((user_read_buf_size) && (!pUser_read_buf)) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
            return MZ_FALSE;

        /* A directory or zero length file */
        if ((file_stat.m_is_directory) || (!file_stat.m_comp_size))
            return MZ_TRUE;

        /* Encryption and patch files are not supported. */
        if (file_stat.m_bit_flag & (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_ENCRYPTION);

        /* This function only supports decompressing stored and deflate. */
        if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_METHOD);

        /* Ensure supplied output buffer is large enough. */
        needed_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? file_stat.m_comp_size : file_stat.m_uncomp_size;
        if (buf_size < needed_size)
            return mz_zip_set_error(pZip, MZ_ZIP_BUF_TOO_SMALL);

        /* Read and parse the local directory entry. */
        cur_file_ofs = file_stat.m_local_header_ofs;
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

        if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
        if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method))
        {
            /* The file is stored or the caller has requested the compressed data. */
            if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, (size_t)needed_size) != needed_size)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
            if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) == 0)
            {
                if (mz_crc32(MZ_CRC32_INIT, (const mz_uint8*)pBuf, (size_t)file_stat.m_uncomp_size) != file_stat.m_crc32)
                    return mz_zip_set_error(pZip, MZ_ZIP_CRC_CHECK_FAILED);
            }
#endif

            return MZ_TRUE;
        }

        /* Decompress the file either directly from memory or from a file input buffer. */
        tinfl_init(&inflator);

        if (pZip->m_pState->m_pMem)
        {
            /* Read directly from the archive in memory. */
            pRead_buf = (mz_uint8*)pZip->m_pState->m_pMem + cur_file_ofs;
            read_buf_size = read_buf_avail = file_stat.m_comp_size;
            comp_remaining = 0;
        }
        else if (pUser_read_buf)
        {
            /* Use a user provided read buffer. */
            if (!user_read_buf_size)
                return MZ_FALSE;
            pRead_buf = (mz_uint8*)pUser_read_buf;
            read_buf_size = user_read_buf_size;
            read_buf_avail = 0;
            comp_remaining = file_stat.m_comp_size;
        }
        else
        {
            /* Temporarily allocate a read buffer. */
            read_buf_size = MZ_MIN(file_stat.m_comp_size, (mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE);
            if (((sizeof(size_t) == sizeof(mz_uint32))) && (read_buf_size > 0x7FFFFFFF))
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

            if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

            read_buf_avail = 0;
            comp_remaining = file_stat.m_comp_size;
        }

        do
        {
            /* The size_t cast here should be OK because we've verified that the output buffer is >= file_stat.m_uncomp_size above */
            size_t in_buf_size, out_buf_size = (size_t)(file_stat.m_uncomp_size - out_buf_ofs);
            if ((!read_buf_avail) && (!pZip->m_pState->m_pMem))
            {
                read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                {
                    status = TINFL_STATUS_FAILED;
                    mz_zip_set_error(pZip, MZ_ZIP_DECOMPRESSION_FAILED);
                    break;
                }
                cur_file_ofs += read_buf_avail;
                comp_remaining -= read_buf_avail;
                read_buf_ofs = 0;
            }
            in_buf_size = (size_t)read_buf_avail;
            status = tinfl_decompress(&inflator, (mz_uint8*)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8*)pBuf, (mz_uint8*)pBuf + out_buf_ofs, &out_buf_size, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF | (comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));
            read_buf_avail -= in_buf_size;
            read_buf_ofs += in_buf_size;
            out_buf_ofs += out_buf_size;
        } while (status == TINFL_STATUS_NEEDS_MORE_INPUT);

        if (status == TINFL_STATUS_DONE)
        {
            /* Make sure the entire file was decompressed, and check its CRC. */
            if (out_buf_ofs != file_stat.m_uncomp_size)
            {
                mz_zip_set_error(pZip, MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE);
                status = TINFL_STATUS_FAILED;
            }
#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
            else if (mz_crc32(MZ_CRC32_INIT, (const mz_uint8*)pBuf, (size_t)file_stat.m_uncomp_size) != file_stat.m_crc32)
            {
                mz_zip_set_error(pZip, MZ_ZIP_CRC_CHECK_FAILED);
                status = TINFL_STATUS_FAILED;
            }
#endif
        }

        if ((!pZip->m_pState->m_pMem) && (!pUser_read_buf))
            pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);

        return status == TINFL_STATUS_DONE;
    }

    mz_bool mz_zip_reader_extract_file_to_mem_no_alloc(mz_zip_archive* pZip, const char* pFilename, void* pBuf, size_t buf_size, mz_uint flags, void* pUser_read_buf, size_t user_read_buf_size)
    {
        mz_uint32 file_index;
        if (!mz_zip_reader_locate_file_v2(pZip, pFilename, NULL, flags, &file_index))
            return MZ_FALSE;
        return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, pUser_read_buf, user_read_buf_size);
    }

    mz_bool mz_zip_reader_extract_to_mem(mz_zip_archive* pZip, mz_uint file_index, void* pBuf, size_t buf_size, mz_uint flags)
    {
        return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, NULL, 0);
    }

    mz_bool mz_zip_reader_extract_file_to_mem(mz_zip_archive* pZip, const char* pFilename, void* pBuf, size_t buf_size, mz_uint flags)
    {
        return mz_zip_reader_extract_file_to_mem_no_alloc(pZip, pFilename, pBuf, buf_size, flags, NULL, 0);
    }

    void* mz_zip_reader_extract_to_heap(mz_zip_archive* pZip, mz_uint file_index, size_t* pSize, mz_uint flags)
    {
        mz_uint64 comp_size, uncomp_size, alloc_size;
        const mz_uint8* p = mz_zip_get_cdh(pZip, file_index);
        void* pBuf;

        if (pSize)
            *pSize = 0;

        if (!p)
        {
            mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return NULL;
        }

        comp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
        uncomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);

        alloc_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? comp_size : uncomp_size;
        if (((sizeof(size_t) == sizeof(mz_uint32))) && (alloc_size > 0x7FFFFFFF))
        {
            mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);
            return NULL;
        }

        if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)alloc_size)))
        {
            mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            return NULL;
        }

        if (!mz_zip_reader_extract_to_mem(pZip, file_index, pBuf, (size_t)alloc_size, flags))
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return NULL;
        }

        if (pSize)
            *pSize = (size_t)alloc_size;
        return pBuf;
    }

    void* mz_zip_reader_extract_file_to_heap(mz_zip_archive* pZip, const char* pFilename, size_t* pSize, mz_uint flags)
    {
        mz_uint32 file_index;
        if (!mz_zip_reader_locate_file_v2(pZip, pFilename, NULL, flags, &file_index))
        {
            if (pSize)
                *pSize = 0;
            return MZ_FALSE;
        }
        return mz_zip_reader_extract_to_heap(pZip, file_index, pSize, flags);
    }

    mz_bool mz_zip_reader_extract_to_callback(mz_zip_archive* pZip, mz_uint file_index, mz_file_write_func pCallback, void* pOpaque, mz_uint flags)
    {
        int status = TINFL_STATUS_DONE;
        mz_uint file_crc32 = MZ_CRC32_INIT;
        mz_uint64 read_buf_size, read_buf_ofs = 0, read_buf_avail, comp_remaining, out_buf_ofs = 0, cur_file_ofs;
        mz_zip_archive_file_stat file_stat;
        void* pRead_buf = NULL;
        void* pWrite_buf = NULL;
        mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pLocal_header = (mz_uint8*)local_header_u32;

        if ((!pZip) || (!pZip->m_pState) || (!pCallback) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
            return MZ_FALSE;

        /* A directory or zero length file */
        if ((file_stat.m_is_directory) || (!file_stat.m_comp_size))
            return MZ_TRUE;

        /* Encryption and patch files are not supported. */
        if (file_stat.m_bit_flag & (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_ENCRYPTION);

        /* This function only supports decompressing stored and deflate. */
        if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_METHOD);

        /* Read and do some minimal validation of the local directory entry (this doesn't crack the zip64 stuff, which we already have from the central dir) */
        cur_file_ofs = file_stat.m_local_header_ofs;
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

        if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
        if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        /* Decompress the file either directly from memory or from a file input buffer. */
        if (pZip->m_pState->m_pMem)
        {
            pRead_buf = (mz_uint8*)pZip->m_pState->m_pMem + cur_file_ofs;
            read_buf_size = read_buf_avail = file_stat.m_comp_size;
            comp_remaining = 0;
        }
        else
        {
            read_buf_size = MZ_MIN(file_stat.m_comp_size, (mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE);
            if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

            read_buf_avail = 0;
            comp_remaining = file_stat.m_comp_size;
        }

        if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method))
        {
            /* The file is stored or the caller has requested the compressed data. */
            if (pZip->m_pState->m_pMem)
            {
                if (((sizeof(size_t) == sizeof(mz_uint32))) && (file_stat.m_comp_size > MZ_UINT32_MAX))
                    return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

                if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)file_stat.m_comp_size) != file_stat.m_comp_size)
                {
                    mz_zip_set_error(pZip, MZ_ZIP_WRITE_CALLBACK_FAILED);
                    status = TINFL_STATUS_FAILED;
                }
                else if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                {
#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
                    file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8*)pRead_buf, (size_t)file_stat.m_comp_size);
#endif
                }

                cur_file_ofs += file_stat.m_comp_size;
                out_buf_ofs += file_stat.m_comp_size;
                comp_remaining = 0;
            }
            else
            {
                while (comp_remaining)
                {
                    read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                    {
                        mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                        status = TINFL_STATUS_FAILED;
                        break;
                    }

#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
                    if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                    {
                        file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8*)pRead_buf, (size_t)read_buf_avail);
                    }
#endif

                    if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                    {
                        mz_zip_set_error(pZip, MZ_ZIP_WRITE_CALLBACK_FAILED);
                        status = TINFL_STATUS_FAILED;
                        break;
                    }

                    cur_file_ofs += read_buf_avail;
                    out_buf_ofs += read_buf_avail;
                    comp_remaining -= read_buf_avail;
                }
            }
        }
        else
        {
            tinfl_decompressor inflator;
            tinfl_init(&inflator);

            if (NULL == (pWrite_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, TINFL_LZ_DICT_SIZE)))
            {
                mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
                status = TINFL_STATUS_FAILED;
            }
            else
            {
                do
                {
                    mz_uint8* pWrite_buf_cur = (mz_uint8*)pWrite_buf + (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                    size_t in_buf_size, out_buf_size = TINFL_LZ_DICT_SIZE - (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                    if ((!read_buf_avail) && (!pZip->m_pState->m_pMem))
                    {
                        read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail)
                        {
                            mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                            status = TINFL_STATUS_FAILED;
                            break;
                        }
                        cur_file_ofs += read_buf_avail;
                        comp_remaining -= read_buf_avail;
                        read_buf_ofs = 0;
                    }

                    in_buf_size = (size_t)read_buf_avail;
                    status = tinfl_decompress(&inflator, (const mz_uint8*)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8*)pWrite_buf, pWrite_buf_cur, &out_buf_size, comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0);
                    read_buf_avail -= in_buf_size;
                    read_buf_ofs += in_buf_size;

                    if (out_buf_size)
                    {
                        if (pCallback(pOpaque, out_buf_ofs, pWrite_buf_cur, out_buf_size) != out_buf_size)
                        {
                            mz_zip_set_error(pZip, MZ_ZIP_WRITE_CALLBACK_FAILED);
                            status = TINFL_STATUS_FAILED;
                            break;
                        }

#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
                        file_crc32 = (mz_uint32)mz_crc32(file_crc32, pWrite_buf_cur, out_buf_size);
#endif
                        if ((out_buf_ofs += out_buf_size) > file_stat.m_uncomp_size)
                        {
                            mz_zip_set_error(pZip, MZ_ZIP_DECOMPRESSION_FAILED);
                            status = TINFL_STATUS_FAILED;
                            break;
                        }
                    }
                } while ((status == TINFL_STATUS_NEEDS_MORE_INPUT) || (status == TINFL_STATUS_HAS_MORE_OUTPUT));
            }
        }

        if ((status == TINFL_STATUS_DONE) && (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)))
        {
            /* Make sure the entire file was decompressed, and check its CRC. */
            if (out_buf_ofs != file_stat.m_uncomp_size)
            {
                mz_zip_set_error(pZip, MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE);
                status = TINFL_STATUS_FAILED;
            }
#ifndef MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS
            else if (file_crc32 != file_stat.m_crc32)
            {
                mz_zip_set_error(pZip, MZ_ZIP_DECOMPRESSION_FAILED);
                status = TINFL_STATUS_FAILED;
            }
#endif
        }

        if (!pZip->m_pState->m_pMem)
            pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);

        if (pWrite_buf)
            pZip->m_pFree(pZip->m_pAlloc_opaque, pWrite_buf);

        return status == TINFL_STATUS_DONE;
    }

    mz_bool mz_zip_reader_extract_file_to_callback(mz_zip_archive* pZip, const char* pFilename, mz_file_write_func pCallback, void* pOpaque, mz_uint flags)
    {
        mz_uint32 file_index;
        if (!mz_zip_reader_locate_file_v2(pZip, pFilename, NULL, flags, &file_index))
            return MZ_FALSE;

        return mz_zip_reader_extract_to_callback(pZip, file_index, pCallback, pOpaque, flags);
    }

#ifndef MINIZ_NO_STDIO
    static size_t mz_zip_file_write_callback(void* pOpaque, mz_uint64 ofs, const void* pBuf, size_t n)
    {
        (void)ofs;

        return MZ_FWRITE(pBuf, 1, n, (MZ_FILE*)pOpaque);
    }

    mz_bool mz_zip_reader_extract_to_file(mz_zip_archive* pZip, mz_uint file_index, const char* pDst_filename, mz_uint flags)
    {
        mz_bool status;
        mz_zip_archive_file_stat file_stat;
        MZ_FILE* pFile;

        if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
            return MZ_FALSE;

        if ((file_stat.m_is_directory) || (!file_stat.m_is_supported))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_FEATURE);

        pFile = MZ_FOPEN(pDst_filename, "wb");
        if (!pFile)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);

        status = mz_zip_reader_extract_to_callback(pZip, file_index, mz_zip_file_write_callback, pFile, flags);

        if (MZ_FCLOSE(pFile) == EOF)
        {
            if (status)
                mz_zip_set_error(pZip, MZ_ZIP_FILE_CLOSE_FAILED);

            status = MZ_FALSE;
        }

#if !defined(MINIZ_NO_TIME) && !defined(MINIZ_NO_STDIO)
        if (status)
            mz_zip_set_file_times(pDst_filename, file_stat.m_time, file_stat.m_time);
#endif

        return status;
    }

    mz_bool mz_zip_reader_extract_file_to_file(mz_zip_archive* pZip, const char* pArchive_filename, const char* pDst_filename, mz_uint flags)
    {
        mz_uint32 file_index;
        if (!mz_zip_reader_locate_file_v2(pZip, pArchive_filename, NULL, flags, &file_index))
            return MZ_FALSE;

        return mz_zip_reader_extract_to_file(pZip, file_index, pDst_filename, flags);
    }

    mz_bool mz_zip_reader_extract_to_cfile(mz_zip_archive* pZip, mz_uint file_index, MZ_FILE* pFile, mz_uint flags)
    {
        mz_zip_archive_file_stat file_stat;

        if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
            return MZ_FALSE;

        if ((file_stat.m_is_directory) || (!file_stat.m_is_supported))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_FEATURE);

        return mz_zip_reader_extract_to_callback(pZip, file_index, mz_zip_file_write_callback, pFile, flags);
    }

    mz_bool mz_zip_reader_extract_file_to_cfile(mz_zip_archive* pZip, const char* pArchive_filename, MZ_FILE* pFile, mz_uint flags)
    {
        mz_uint32 file_index;
        if (!mz_zip_reader_locate_file_v2(pZip, pArchive_filename, NULL, flags, &file_index))
            return MZ_FALSE;

        return mz_zip_reader_extract_to_cfile(pZip, file_index, pFile, flags);
    }
#endif /* #ifndef MINIZ_NO_STDIO */

    static size_t mz_zip_compute_crc32_callback(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n)
    {
        mz_uint32* p = (mz_uint32*)pOpaque;
        (void)file_ofs;
        *p = (mz_uint32)mz_crc32(*p, (const mz_uint8*)pBuf, n);
        return n;
    }

    mz_bool mz_zip_validate_file(mz_zip_archive* pZip, mz_uint file_index, mz_uint flags)
    {
        mz_zip_archive_file_stat file_stat;
        mz_zip_internal_state* pState;
        const mz_uint8* pCentral_dir_header;
        mz_bool found_zip64_ext_data_in_cdir = MZ_FALSE;
        mz_bool found_zip64_ext_data_in_ldir = MZ_FALSE;
        mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pLocal_header = (mz_uint8*)local_header_u32;
        mz_uint64 local_header_ofs = 0;
        mz_uint32 local_header_filename_len, local_header_extra_len, local_header_crc32;
        mz_uint64 local_header_comp_size, local_header_uncomp_size;
        mz_uint32 uncomp_crc32 = MZ_CRC32_INIT;
        mz_bool has_data_descriptor;
        mz_uint32 local_header_bit_flags;

        mz_zip_array file_data_array;
        mz_zip_array_init(&file_data_array, 1);

        if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (file_index > pZip->m_total_files)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        pCentral_dir_header = mz_zip_get_cdh(pZip, file_index);

        if (!mz_zip_file_stat_internal(pZip, file_index, pCentral_dir_header, &file_stat, &found_zip64_ext_data_in_cdir))
            return MZ_FALSE;

        /* A directory or zero length file */
        if ((file_stat.m_is_directory) || (!file_stat.m_uncomp_size))
            return MZ_TRUE;

        /* Encryption and patch files are not supported. */
        if (file_stat.m_is_encrypted)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_ENCRYPTION);

        /* This function only supports stored and deflate. */
        if ((file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_METHOD);

        if (!file_stat.m_is_supported)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_FEATURE);

        /* Read and parse the local directory entry. */
        local_header_ofs = file_stat.m_local_header_ofs;
        if (pZip->m_pRead(pZip->m_pIO_opaque, local_header_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

        if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        local_header_filename_len = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS);
        local_header_extra_len = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
        local_header_comp_size = MZ_READ_LE32(pLocal_header + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS);
        local_header_uncomp_size = MZ_READ_LE32(pLocal_header + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS);
        local_header_crc32 = MZ_READ_LE32(pLocal_header + MZ_ZIP_LDH_CRC32_OFS);
        local_header_bit_flags = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_BIT_FLAG_OFS);
        has_data_descriptor = (local_header_bit_flags & 8) != 0;

        if (local_header_filename_len != strlen(file_stat.m_filename))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        if ((local_header_ofs + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + local_header_filename_len + local_header_extra_len + file_stat.m_comp_size) > pZip->m_archive_size)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        if (!mz_zip_array_resize(pZip, &file_data_array, MZ_MAX(local_header_filename_len, local_header_extra_len), MZ_FALSE))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        if (local_header_filename_len)
        {
            if (pZip->m_pRead(pZip->m_pIO_opaque, local_header_ofs + MZ_ZIP_LOCAL_DIR_HEADER_SIZE, file_data_array.m_p, local_header_filename_len) != local_header_filename_len)
            {
                mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                goto handle_failure;
            }

            /* I've seen 1 archive that had the same pathname, but used backslashes in the local dir and forward slashes in the central dir. Do we care about this? For now, this case will fail validation. */
            if (memcmp(file_stat.m_filename, file_data_array.m_p, local_header_filename_len) != 0)
            {
                mz_zip_set_error(pZip, MZ_ZIP_VALIDATION_FAILED);
                goto handle_failure;
            }
        }

        if ((local_header_extra_len) && ((local_header_comp_size == MZ_UINT32_MAX) || (local_header_uncomp_size == MZ_UINT32_MAX)))
        {
            mz_uint32 extra_size_remaining = local_header_extra_len;
            const mz_uint8* pExtra_data = (const mz_uint8*)file_data_array.m_p;

            if (pZip->m_pRead(pZip->m_pIO_opaque, local_header_ofs + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + local_header_filename_len, file_data_array.m_p, local_header_extra_len) != local_header_extra_len)
            {
                mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                goto handle_failure;
            }

            do
            {
                mz_uint32 field_id, field_data_size, field_total_size;

                if (extra_size_remaining < (sizeof(mz_uint16) * 2))
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                field_id = MZ_READ_LE16(pExtra_data);
                field_data_size = MZ_READ_LE16(pExtra_data + sizeof(mz_uint16));
                field_total_size = field_data_size + sizeof(mz_uint16) * 2;

                if (field_total_size > extra_size_remaining)
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                if (field_id == MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID)
                {
                    const mz_uint8* pSrc_field_data = pExtra_data + sizeof(mz_uint32);

                    if (field_data_size < sizeof(mz_uint64) * 2)
                    {
                        mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                        goto handle_failure;
                    }

                    local_header_uncomp_size = MZ_READ_LE64(pSrc_field_data);
                    local_header_comp_size = MZ_READ_LE64(pSrc_field_data + sizeof(mz_uint64));

                    found_zip64_ext_data_in_ldir = MZ_TRUE;
                    break;
                }

                pExtra_data += field_total_size;
                extra_size_remaining -= field_total_size;
            } while (extra_size_remaining);
        }

        /* TODO: parse local header extra data when local_header_comp_size is 0xFFFFFFFF! (big_descriptor.zip) */
        /* I've seen zips in the wild with the data descriptor bit set, but proper local header values and bogus data descriptors */
        if ((has_data_descriptor) && (!local_header_comp_size) && (!local_header_crc32))
        {
            mz_uint8 descriptor_buf[32];
            mz_bool has_id;
            const mz_uint8* pSrc;
            mz_uint32 file_crc32;
            mz_uint64 comp_size = 0, uncomp_size = 0;

            mz_uint32 num_descriptor_uint32s = ((pState->m_zip64) || (found_zip64_ext_data_in_ldir)) ? 6 : 4;

            if (pZip->m_pRead(pZip->m_pIO_opaque, local_header_ofs + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + local_header_filename_len + local_header_extra_len + file_stat.m_comp_size, descriptor_buf, sizeof(mz_uint32) * num_descriptor_uint32s) != (sizeof(mz_uint32) * num_descriptor_uint32s))
            {
                mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                goto handle_failure;
            }

            has_id = (MZ_READ_LE32(descriptor_buf) == MZ_ZIP_DATA_DESCRIPTOR_ID);
            pSrc = has_id ? (descriptor_buf + sizeof(mz_uint32)) : descriptor_buf;

            file_crc32 = MZ_READ_LE32(pSrc);

            if ((pState->m_zip64) || (found_zip64_ext_data_in_ldir))
            {
                comp_size = MZ_READ_LE64(pSrc + sizeof(mz_uint32));
                uncomp_size = MZ_READ_LE64(pSrc + sizeof(mz_uint32) + sizeof(mz_uint64));
            }
            else
            {
                comp_size = MZ_READ_LE32(pSrc + sizeof(mz_uint32));
                uncomp_size = MZ_READ_LE32(pSrc + sizeof(mz_uint32) + sizeof(mz_uint32));
            }

            if ((file_crc32 != file_stat.m_crc32) || (comp_size != file_stat.m_comp_size) || (uncomp_size != file_stat.m_uncomp_size))
            {
                mz_zip_set_error(pZip, MZ_ZIP_VALIDATION_FAILED);
                goto handle_failure;
            }
        }
        else
        {
            if ((local_header_crc32 != file_stat.m_crc32) || (local_header_comp_size != file_stat.m_comp_size) || (local_header_uncomp_size != file_stat.m_uncomp_size))
            {
                mz_zip_set_error(pZip, MZ_ZIP_VALIDATION_FAILED);
                goto handle_failure;
            }
        }

        mz_zip_array_clear(pZip, &file_data_array);

        if ((flags & MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY) == 0)
        {
            if (!mz_zip_reader_extract_to_callback(pZip, file_index, mz_zip_compute_crc32_callback, &uncomp_crc32, 0))
                return MZ_FALSE;

            /* 1 more check to be sure, although the extract checks too. */
            if (uncomp_crc32 != file_stat.m_crc32)
            {
                mz_zip_set_error(pZip, MZ_ZIP_VALIDATION_FAILED);
                return MZ_FALSE;
            }
        }

        return MZ_TRUE;

    handle_failure:
        mz_zip_array_clear(pZip, &file_data_array);
        return MZ_FALSE;
    }

    mz_bool mz_zip_validate_archive(mz_zip_archive* pZip, mz_uint flags)
    {
        mz_zip_internal_state* pState;
        uint32_t i;

        if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        /* Basic sanity checks */
        if (!pState->m_zip64)
        {
            if (pZip->m_total_files > MZ_UINT16_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

            if (pZip->m_archive_size > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
        }
        else
        {
            if (pZip->m_total_files >= MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

            if (pState->m_central_dir.m_size >= MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
        }

        for (i = 0; i < pZip->m_total_files; i++)
        {
            if (MZ_ZIP_FLAG_VALIDATE_LOCATE_FILE_FLAG & flags)
            {
                mz_uint32 found_index;
                mz_zip_archive_file_stat stat;

                if (!mz_zip_reader_file_stat(pZip, i, &stat))
                    return MZ_FALSE;

                if (!mz_zip_reader_locate_file_v2(pZip, stat.m_filename, NULL, 0, &found_index))
                    return MZ_FALSE;

                /* This check can fail if there are duplicate filenames in the archive (which we don't check for when writing - that's up to the user) */
                if (found_index != i)
                    return mz_zip_set_error(pZip, MZ_ZIP_VALIDATION_FAILED);
            }

            if (!mz_zip_validate_file(pZip, i, flags))
                return MZ_FALSE;
        }

        return MZ_TRUE;
    }

    mz_bool mz_zip_validate_mem_archive(const void* pMem, size_t size, mz_uint flags, mz_zip_error* pErr)
    {
        mz_bool success = MZ_TRUE;
        mz_zip_archive zip;
        mz_zip_error actual_err = MZ_ZIP_NO_ERROR;

        if ((!pMem) || (!size))
        {
            if (pErr)
                *pErr = MZ_ZIP_INVALID_PARAMETER;
            return MZ_FALSE;
        }

        mz_zip_zero_struct(&zip);

        if (!mz_zip_reader_init_mem(&zip, pMem, size, flags))
        {
            if (pErr)
                *pErr = zip.m_last_error;
            return MZ_FALSE;
        }

        if (!mz_zip_validate_archive(&zip, flags))
        {
            actual_err = zip.m_last_error;
            success = MZ_FALSE;
        }

        if (!mz_zip_reader_end_internal(&zip, success))
        {
            if (!actual_err)
                actual_err = zip.m_last_error;
            success = MZ_FALSE;
        }

        if (pErr)
            *pErr = actual_err;

        return success;
    }

#ifndef MINIZ_NO_STDIO
    mz_bool mz_zip_validate_file_archive(const char* pFilename, mz_uint flags, mz_zip_error* pErr)
    {
        mz_bool success = MZ_TRUE;
        mz_zip_archive zip;
        mz_zip_error actual_err = MZ_ZIP_NO_ERROR;

        if (!pFilename)
        {
            if (pErr)
                *pErr = MZ_ZIP_INVALID_PARAMETER;
            return MZ_FALSE;
        }

        mz_zip_zero_struct(&zip);

        if (!mz_zip_reader_init_file_v2(&zip, pFilename, flags, 0, 0))
        {
            if (pErr)
                *pErr = zip.m_last_error;
            return MZ_FALSE;
        }

        if (!mz_zip_validate_archive(&zip, flags))
        {
            actual_err = zip.m_last_error;
            success = MZ_FALSE;
        }

        if (!mz_zip_reader_end_internal(&zip, success))
        {
            if (!actual_err)
                actual_err = zip.m_last_error;
            success = MZ_FALSE;
        }

        if (pErr)
            *pErr = actual_err;

        return success;
    }
#endif /* #ifndef MINIZ_NO_STDIO */

    /* ------------------- .ZIP archive writing */

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

    static MZ_FORCEINLINE void mz_write_le16(mz_uint8* p, mz_uint16 v)
    {
        p[0] = (mz_uint8)v;
        p[1] = (mz_uint8)(v >> 8);
    }
    static MZ_FORCEINLINE void mz_write_le32(mz_uint8* p, mz_uint32 v)
    {
        p[0] = (mz_uint8)v;
        p[1] = (mz_uint8)(v >> 8);
        p[2] = (mz_uint8)(v >> 16);
        p[3] = (mz_uint8)(v >> 24);
    }
    static MZ_FORCEINLINE void mz_write_le64(mz_uint8* p, mz_uint64 v)
    {
        mz_write_le32(p, (mz_uint32)v);
        mz_write_le32(p + sizeof(mz_uint32), (mz_uint32)(v >> 32));
    }

#define MZ_WRITE_LE16(p, v) mz_write_le16((mz_uint8 *)(p), (mz_uint16)(v))
#define MZ_WRITE_LE32(p, v) mz_write_le32((mz_uint8 *)(p), (mz_uint32)(v))
#define MZ_WRITE_LE64(p, v) mz_write_le64((mz_uint8 *)(p), (mz_uint64)(v))

    static size_t mz_zip_heap_write_func(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n)
    {
        mz_zip_archive* pZip = (mz_zip_archive*)pOpaque;
        mz_zip_internal_state* pState = pZip->m_pState;
        mz_uint64 new_size = MZ_MAX(file_ofs + n, pState->m_mem_size);

        if (!n)
            return 0;

        /* An allocation this big is likely to just fail on 32-bit systems, so don't even go there. */
        if ((sizeof(size_t) == sizeof(mz_uint32)) && (new_size > 0x7FFFFFFF))
        {
            mz_zip_set_error(pZip, MZ_ZIP_FILE_TOO_LARGE);
            return 0;
        }

        if (new_size > pState->m_mem_capacity)
        {
            void* pNew_block;
            size_t new_capacity = MZ_MAX(64, pState->m_mem_capacity);

            while (new_capacity < new_size)
                new_capacity *= 2;

            if (NULL == (pNew_block = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pState->m_pMem, 1, new_capacity)))
            {
                mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
                return 0;
            }

            pState->m_pMem = pNew_block;
            pState->m_mem_capacity = new_capacity;
        }
        memcpy((mz_uint8*)pState->m_pMem + file_ofs, pBuf, n);
        pState->m_mem_size = (size_t)new_size;
        return n;
    }

    static mz_bool mz_zip_writer_end_internal(mz_zip_archive* pZip, mz_bool set_last_error)
    {
        mz_zip_internal_state* pState;
        mz_bool status = MZ_TRUE;

        if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || ((pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) && (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED)))
        {
            if (set_last_error)
                mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return MZ_FALSE;
        }

        pState = pZip->m_pState;
        pZip->m_pState = NULL;
        mz_zip_array_clear(pZip, &pState->m_central_dir);
        mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
        mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

#ifndef MINIZ_NO_STDIO
        if (pState->m_pFile)
        {
            if (pZip->m_zip_type == MZ_ZIP_TYPE_FILE)
            {
                if (MZ_FCLOSE(pState->m_pFile) == EOF)
                {
                    if (set_last_error)
                        mz_zip_set_error(pZip, MZ_ZIP_FILE_CLOSE_FAILED);
                    status = MZ_FALSE;
                }
            }

            pState->m_pFile = NULL;
        }
#endif /* #ifndef MINIZ_NO_STDIO */

        if ((pZip->m_pWrite == mz_zip_heap_write_func) && (pState->m_pMem))
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pState->m_pMem);
            pState->m_pMem = NULL;
        }

        pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
        pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;
        return status;
    }

    mz_bool mz_zip_writer_init_v2(mz_zip_archive* pZip, mz_uint64 existing_size, mz_uint flags)
    {
        mz_bool zip64 = (flags & MZ_ZIP_FLAG_WRITE_ZIP64) != 0;

        if ((!pZip) || (pZip->m_pState) || (!pZip->m_pWrite) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING)
        {
            if (!pZip->m_pRead)
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
        }

        if (pZip->m_file_offset_alignment)
        {
            /* Ensure user specified file offset alignment is a power of 2. */
            if (pZip->m_file_offset_alignment & (pZip->m_file_offset_alignment - 1))
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
        }

        if (!pZip->m_pAlloc)
            pZip->m_pAlloc = miniz_def_alloc_func;
        if (!pZip->m_pFree)
            pZip->m_pFree = miniz_def_free_func;
        if (!pZip->m_pRealloc)
            pZip->m_pRealloc = miniz_def_realloc_func;

        pZip->m_archive_size = existing_size;
        pZip->m_central_directory_file_ofs = 0;
        pZip->m_total_files = 0;

        if (NULL == (pZip->m_pState = (mz_zip_internal_state*)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));

        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
        MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));

        pZip->m_pState->m_zip64 = zip64;
        pZip->m_pState->m_zip64_has_extended_info_fields = zip64;

        pZip->m_zip_type = MZ_ZIP_TYPE_USER;
        pZip->m_zip_mode = MZ_ZIP_MODE_WRITING;

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_init(mz_zip_archive* pZip, mz_uint64 existing_size)
    {
        return mz_zip_writer_init_v2(pZip, existing_size, 0);
    }

    mz_bool mz_zip_writer_init_heap_v2(mz_zip_archive* pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size, mz_uint flags)
    {
        pZip->m_pWrite = mz_zip_heap_write_func;
        pZip->m_pNeeds_keepalive = NULL;

        if (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING)
            pZip->m_pRead = mz_zip_mem_read_func;

        pZip->m_pIO_opaque = pZip;

        if (!mz_zip_writer_init_v2(pZip, size_to_reserve_at_beginning, flags))
            return MZ_FALSE;

        pZip->m_zip_type = MZ_ZIP_TYPE_HEAP;

        if (0 != (initial_allocation_size = MZ_MAX(initial_allocation_size, size_to_reserve_at_beginning)))
        {
            if (NULL == (pZip->m_pState->m_pMem = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, initial_allocation_size)))
            {
                mz_zip_writer_end_internal(pZip, MZ_FALSE);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }
            pZip->m_pState->m_mem_capacity = initial_allocation_size;
        }

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_init_heap(mz_zip_archive* pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size)
    {
        return mz_zip_writer_init_heap_v2(pZip, size_to_reserve_at_beginning, initial_allocation_size, 0);
    }

#ifndef MINIZ_NO_STDIO
    static size_t mz_zip_file_write_func(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n)
    {
        mz_zip_archive* pZip = (mz_zip_archive*)pOpaque;
        mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);

        file_ofs += pZip->m_pState->m_file_archive_start_ofs;

        if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        {
            mz_zip_set_error(pZip, MZ_ZIP_FILE_SEEK_FAILED);
            return 0;
        }

        return MZ_FWRITE(pBuf, 1, n, pZip->m_pState->m_pFile);
    }

    mz_bool mz_zip_writer_init_file(mz_zip_archive* pZip, const char* pFilename, mz_uint64 size_to_reserve_at_beginning)
    {
        return mz_zip_writer_init_file_v2(pZip, pFilename, size_to_reserve_at_beginning, 0);
    }

    mz_bool mz_zip_writer_init_file_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint64 size_to_reserve_at_beginning, mz_uint flags)
    {
        MZ_FILE* pFile;

        pZip->m_pWrite = mz_zip_file_write_func;
        pZip->m_pNeeds_keepalive = NULL;

        if (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING)
            pZip->m_pRead = mz_zip_file_read_func;

        pZip->m_pIO_opaque = pZip;

        if (!mz_zip_writer_init_v2(pZip, size_to_reserve_at_beginning, flags))
            return MZ_FALSE;

        if (NULL == (pFile = MZ_FOPEN(pFilename, (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING) ? "w+b" : "wb")))
        {
            mz_zip_writer_end(pZip);
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);
        }

        pZip->m_pState->m_pFile = pFile;
        pZip->m_zip_type = MZ_ZIP_TYPE_FILE;

        if (size_to_reserve_at_beginning)
        {
            mz_uint64 cur_ofs = 0;
            char buf[4096];

            MZ_CLEAR_OBJ(buf);

            do
            {
                size_t n = (size_t)MZ_MIN(sizeof(buf), size_to_reserve_at_beginning);
                if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_ofs, buf, n) != n)
                {
                    mz_zip_writer_end(pZip);
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
                }
                cur_ofs += n;
                size_to_reserve_at_beginning -= n;
            } while (size_to_reserve_at_beginning);
        }

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_init_cfile(mz_zip_archive* pZip, MZ_FILE* pFile, mz_uint flags)
    {
        pZip->m_pWrite = mz_zip_file_write_func;
        pZip->m_pNeeds_keepalive = NULL;

        if (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING)
            pZip->m_pRead = mz_zip_file_read_func;

        pZip->m_pIO_opaque = pZip;

        if (!mz_zip_writer_init_v2(pZip, 0, flags))
            return MZ_FALSE;

        pZip->m_pState->m_pFile = pFile;
        pZip->m_pState->m_file_archive_start_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);
        pZip->m_zip_type = MZ_ZIP_TYPE_CFILE;

        return MZ_TRUE;
    }
#endif /* #ifndef MINIZ_NO_STDIO */

    mz_bool mz_zip_writer_init_from_reader_v2(mz_zip_archive* pZip, const char* pFilename, mz_uint flags)
    {
        mz_zip_internal_state* pState;

        if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (flags & MZ_ZIP_FLAG_WRITE_ZIP64)
        {
            /* We don't support converting a non-zip64 file to zip64 - this seems like more trouble than it's worth. (What about the existing 32-bit data descriptors that could follow the compressed data?) */
            if (!pZip->m_pState->m_zip64)
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
        }

        /* No sense in trying to write to an archive that's already at the support max size */
        if (pZip->m_pState->m_zip64)
        {
            if (pZip->m_total_files == MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }
        else
        {
            if (pZip->m_total_files == MZ_UINT16_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);

            if ((pZip->m_archive_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_ZIP_LOCAL_DIR_HEADER_SIZE) > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_TOO_LARGE);
        }

        pState = pZip->m_pState;

        if (pState->m_pFile)
        {
#ifdef MINIZ_NO_STDIO
            (void)pFilename;
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
#else
            if (pZip->m_pIO_opaque != pZip)
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

            if (pZip->m_zip_type == MZ_ZIP_TYPE_FILE)
            {
                if (!pFilename)
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

                /* Archive is being read from stdio and was originally opened only for reading. Try to reopen as writable. */
                if (NULL == (pState->m_pFile = MZ_FREOPEN(pFilename, "r+b", pState->m_pFile)))
                {
                    /* The mz_zip_archive is now in a bogus state because pState->m_pFile is NULL, so just close it. */
                    mz_zip_reader_end_internal(pZip, MZ_FALSE);
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);
                }
            }

            pZip->m_pWrite = mz_zip_file_write_func;
            pZip->m_pNeeds_keepalive = NULL;
#endif /* #ifdef MINIZ_NO_STDIO */
        }
        else if (pState->m_pMem)
        {
            /* Archive lives in a memory block. Assume it's from the heap that we can resize using the realloc callback. */
            if (pZip->m_pIO_opaque != pZip)
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

            pState->m_mem_capacity = pState->m_mem_size;
            pZip->m_pWrite = mz_zip_heap_write_func;
            pZip->m_pNeeds_keepalive = NULL;
        }
        /* Archive is being read via a user provided read function - make sure the user has specified a write function too. */
        else if (!pZip->m_pWrite)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        /* Start writing new files at the archive's current central directory location. */
        /* TODO: We could add a flag that lets the user start writing immediately AFTER the existing central dir - this would be safer. */
        pZip->m_archive_size = pZip->m_central_directory_file_ofs;
        pZip->m_central_directory_file_ofs = 0;

        /* Clear the sorted central dir offsets, they aren't useful or maintained now. */
        /* Even though we're now in write mode, files can still be extracted and verified, but file locates will be slow. */
        /* TODO: We could easily maintain the sorted central directory offsets. */
        mz_zip_array_clear(pZip, &pZip->m_pState->m_sorted_central_dir_offsets);

        pZip->m_zip_mode = MZ_ZIP_MODE_WRITING;

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_init_from_reader(mz_zip_archive* pZip, const char* pFilename)
    {
        return mz_zip_writer_init_from_reader_v2(pZip, pFilename, 0);
    }

    /* TODO: pArchive_name is a terrible name here! */
    mz_bool mz_zip_writer_add_mem(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, mz_uint level_and_flags)
    {
        return mz_zip_writer_add_mem_ex(pZip, pArchive_name, pBuf, buf_size, NULL, 0, level_and_flags, 0, 0);
    }

    typedef struct
    {
        mz_zip_archive* m_pZip;
        mz_uint64 m_cur_archive_file_ofs;
        mz_uint64 m_comp_size;
    } mz_zip_writer_add_state;

    static mz_bool mz_zip_writer_add_put_buf_callback(const void* pBuf, int len, void* pUser)
    {
        mz_zip_writer_add_state* pState = (mz_zip_writer_add_state*)pUser;
        if ((int)pState->m_pZip->m_pWrite(pState->m_pZip->m_pIO_opaque, pState->m_cur_archive_file_ofs, pBuf, len) != len)
            return MZ_FALSE;

        pState->m_cur_archive_file_ofs += len;
        pState->m_comp_size += len;
        return MZ_TRUE;
    }

#define MZ_ZIP64_MAX_LOCAL_EXTRA_FIELD_SIZE (sizeof(mz_uint16) * 2 + sizeof(mz_uint64) * 2)
#define MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE (sizeof(mz_uint16) * 2 + sizeof(mz_uint64) * 3)
    static mz_uint32 mz_zip_writer_create_zip64_extra_data(mz_uint8* pBuf, mz_uint64* pUncomp_size, mz_uint64* pComp_size, mz_uint64* pLocal_header_ofs)
    {
        mz_uint8* pDst = pBuf;
        mz_uint32 field_size = 0;

        MZ_WRITE_LE16(pDst + 0, MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID);
        MZ_WRITE_LE16(pDst + 2, 0);
        pDst += sizeof(mz_uint16) * 2;

        if (pUncomp_size)
        {
            MZ_WRITE_LE64(pDst, *pUncomp_size);
            pDst += sizeof(mz_uint64);
            field_size += sizeof(mz_uint64);
        }

        if (pComp_size)
        {
            MZ_WRITE_LE64(pDst, *pComp_size);
            pDst += sizeof(mz_uint64);
            field_size += sizeof(mz_uint64);
        }

        if (pLocal_header_ofs)
        {
            MZ_WRITE_LE64(pDst, *pLocal_header_ofs);
            pDst += sizeof(mz_uint64);
            field_size += sizeof(mz_uint64);
        }

        MZ_WRITE_LE16(pBuf + 2, field_size);

        return (mz_uint32)(pDst - pBuf);
    }

    static mz_bool mz_zip_writer_create_local_dir_header(mz_zip_archive* pZip, mz_uint8* pDst, mz_uint16 filename_size, mz_uint16 extra_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date)
    {
        (void)pZip;
        memset(pDst, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE);
        MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_SIG_OFS, MZ_ZIP_LOCAL_DIR_HEADER_SIG);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_VERSION_NEEDED_OFS, method ? 20 : 0);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_BIT_FLAG_OFS, bit_flags);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_METHOD_OFS, method);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_TIME_OFS, dos_time);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_DATE_OFS, dos_date);
        MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_CRC32_OFS, uncomp_crc32);
        MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS, MZ_MIN(comp_size, MZ_UINT32_MAX));
        MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS, MZ_MIN(uncomp_size, MZ_UINT32_MAX));
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILENAME_LEN_OFS, filename_size);
        MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_EXTRA_LEN_OFS, extra_size);
        return MZ_TRUE;
    }

    static mz_bool mz_zip_writer_create_central_dir_header(mz_zip_archive* pZip, mz_uint8* pDst,
        mz_uint16 filename_size, mz_uint16 extra_size, mz_uint16 comment_size,
        mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32,
        mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date,
        mz_uint64 local_header_ofs, mz_uint32 ext_attributes)
    {
        (void)pZip;
        memset(pDst, 0, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_SIG_OFS, MZ_ZIP_CENTRAL_DIR_HEADER_SIG);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_VERSION_NEEDED_OFS, method ? 20 : 0);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_BIT_FLAG_OFS, bit_flags);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_METHOD_OFS, method);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_TIME_OFS, dos_time);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_DATE_OFS, dos_date);
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_CRC32_OFS, uncomp_crc32);
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS, MZ_MIN(comp_size, MZ_UINT32_MAX));
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS, MZ_MIN(uncomp_size, MZ_UINT32_MAX));
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILENAME_LEN_OFS, filename_size);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_EXTRA_LEN_OFS, extra_size);
        MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_COMMENT_LEN_OFS, comment_size);
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS, ext_attributes);
        MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_LOCAL_HEADER_OFS, MZ_MIN(local_header_ofs, MZ_UINT32_MAX));
        return MZ_TRUE;
    }

    static mz_bool mz_zip_writer_add_to_central_dir(mz_zip_archive* pZip, const char* pFilename, mz_uint16 filename_size,
        const void* pExtra, mz_uint16 extra_size, const void* pComment, mz_uint16 comment_size,
        mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32,
        mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date,
        mz_uint64 local_header_ofs, mz_uint32 ext_attributes,
        const char* user_extra_data, mz_uint user_extra_data_len)
    {
        mz_zip_internal_state* pState = pZip->m_pState;
        mz_uint32 central_dir_ofs = (mz_uint32)pState->m_central_dir.m_size;
        size_t orig_central_dir_size = pState->m_central_dir.m_size;
        mz_uint8 central_dir_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];

        if (!pZip->m_pState->m_zip64)
        {
            if (local_header_ofs > 0xFFFFFFFF)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_TOO_LARGE);
        }

        /* miniz doesn't support central dirs >= MZ_UINT32_MAX bytes yet */
        if (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_size + extra_size + user_extra_data_len + comment_size) >= MZ_UINT32_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

        if (!mz_zip_writer_create_central_dir_header(pZip, central_dir_header, filename_size, extra_size + user_extra_data_len, comment_size, uncomp_size, comp_size, uncomp_crc32, method, bit_flags, dos_time, dos_date, local_header_ofs, ext_attributes))
            return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

        if ((!mz_zip_array_push_back(pZip, &pState->m_central_dir, central_dir_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)) ||
            (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pFilename, filename_size)) ||
            (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pExtra, extra_size)) ||
            (!mz_zip_array_push_back(pZip, &pState->m_central_dir, user_extra_data, user_extra_data_len)) ||
            (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pComment, comment_size)) ||
            (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &central_dir_ofs, 1)))
        {
            /* Try to resize the central directory array back into its original state. */
            mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
        }

        return MZ_TRUE;
    }

    static mz_bool mz_zip_writer_validate_archive_name(const char* pArchive_name)
    {
        /* Basic ZIP archive filename validity checks: Valid filenames cannot start with a forward slash, cannot contain a drive letter, and cannot use DOS-style backward slashes. */
        if (*pArchive_name == '/')
            return MZ_FALSE;

        while (*pArchive_name)
        {
            if ((*pArchive_name == '\\') || (*pArchive_name == ':'))
                return MZ_FALSE;

            pArchive_name++;
        }

        return MZ_TRUE;
    }

    static mz_uint mz_zip_writer_compute_padding_needed_for_file_alignment(mz_zip_archive* pZip)
    {
        mz_uint32 n;
        if (!pZip->m_file_offset_alignment)
            return 0;
        n = (mz_uint32)(pZip->m_archive_size & (pZip->m_file_offset_alignment - 1));
        return (mz_uint)((pZip->m_file_offset_alignment - n) & (pZip->m_file_offset_alignment - 1));
    }

    static mz_bool mz_zip_writer_write_zeros(mz_zip_archive* pZip, mz_uint64 cur_file_ofs, mz_uint32 n)
    {
        char buf[4096];
        memset(buf, 0, MZ_MIN(sizeof(buf), n));
        while (n)
        {
            mz_uint32 s = MZ_MIN(sizeof(buf), n);
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_file_ofs, buf, s) != s)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_file_ofs += s;
            n -= s;
        }
        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_add_mem_ex(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags,
        mz_uint64 uncomp_size, mz_uint32 uncomp_crc32)
    {
        return mz_zip_writer_add_mem_ex_v2(pZip, pArchive_name, pBuf, buf_size, pComment, comment_size, level_and_flags, uncomp_size, uncomp_crc32, NULL, NULL, 0, NULL, 0);
    }

    mz_bool mz_zip_writer_add_mem_ex_v2(mz_zip_archive* pZip, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size,
        mz_uint level_and_flags, mz_uint64 uncomp_size, mz_uint32 uncomp_crc32, MZ_TIME_T* last_modified,
        const char* user_extra_data, mz_uint user_extra_data_len, const char* user_extra_data_central, mz_uint user_extra_data_central_len)
    {
        mz_uint16 method = 0, dos_time = 0, dos_date = 0;
        mz_uint level, ext_attributes = 0, num_alignment_padding_bytes;
        mz_uint64 local_dir_header_ofs = pZip->m_archive_size, cur_archive_file_ofs = pZip->m_archive_size, comp_size = 0;
        size_t archive_name_size;
        mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
        tdefl_compressor* pComp = NULL;
        mz_bool store_data_uncompressed;
        mz_zip_internal_state* pState;
        mz_uint8* pExtra_data = NULL;
        mz_uint32 extra_size = 0;
        mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
        mz_uint16 bit_flags = 0;

        if (uncomp_size || (buf_size && !(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)))
            bit_flags |= MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR;

        if (!(level_and_flags & MZ_ZIP_FLAG_ASCII_FILENAME))
            bit_flags |= MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8;

        if ((int)level_and_flags < 0)
            level_and_flags = MZ_DEFAULT_LEVEL;
        level = level_and_flags & 0xF;
        store_data_uncompressed = ((!level) || (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA));

        if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || ((buf_size) && (!pBuf)) || (!pArchive_name) || ((comment_size) && (!pComment)) || (level > MZ_UBER_COMPRESSION))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        if (pState->m_zip64)
        {
            if (pZip->m_total_files == MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }
        else
        {
            if (pZip->m_total_files == MZ_UINT16_MAX)
            {
                pState->m_zip64 = MZ_TRUE;
                /*return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES); */
            }
            if ((buf_size > 0xFFFFFFFF) || (uncomp_size > 0xFFFFFFFF))
            {
                pState->m_zip64 = MZ_TRUE;
                /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
            }
        }

        if ((!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (uncomp_size))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_writer_validate_archive_name(pArchive_name))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

#ifndef MINIZ_NO_TIME
        if (last_modified != NULL)
        {
            mz_zip_time_t_to_dos_time(*last_modified, &dos_time, &dos_date);
        }
        else
        {
            MZ_TIME_T cur_time;
            time(&cur_time);
            mz_zip_time_t_to_dos_time(cur_time, &dos_time, &dos_date);
        }
#endif /* #ifndef MINIZ_NO_TIME */

        archive_name_size = strlen(pArchive_name);
        if (archive_name_size > MZ_UINT16_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

        num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

        /* miniz doesn't support central dirs >= MZ_UINT32_MAX bytes yet */
        if (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE + comment_size) >= MZ_UINT32_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

        if (!pState->m_zip64)
        {
            /* Bail early if the archive would obviously become too large */
            if ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) > 0xFFFFFFFF)
            {
                pState->m_zip64 = MZ_TRUE;
                /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
            }
        }

        if ((archive_name_size) && (pArchive_name[archive_name_size - 1] == '/'))
        {
            /* Set DOS Subdirectory attribute bit. */
            ext_attributes |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;

            /* Subdirectories cannot contain data. */
            if ((buf_size) || (uncomp_size))
                return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
        }

        /* Try to do any allocations before writing to the archive, so if an allocation fails the file remains unmodified. (A good idea if we're doing an in-place modification.) */
        if ((!mz_zip_array_ensure_room(pZip, &pState->m_central_dir, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size + (pState->m_zip64 ? MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE : 0))) || (!mz_zip_array_ensure_room(pZip, &pState->m_central_dir_offsets, 1)))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        if ((!store_data_uncompressed) && (buf_size))
        {
            if (NULL == (pComp = (tdefl_compressor*)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor))))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
        }

        if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes))
        {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
            return MZ_FALSE;
        }

        local_dir_header_ofs += num_alignment_padding_bytes;
        if (pZip->m_file_offset_alignment)
        {
            MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
        }
        cur_archive_file_ofs += num_alignment_padding_bytes;

        MZ_CLEAR_OBJ(local_dir_header);

        if (!store_data_uncompressed || (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
        {
            method = MZ_DEFLATED;
        }

        if (pState->m_zip64)
        {
            if (uncomp_size >= MZ_UINT32_MAX || local_dir_header_ofs >= MZ_UINT32_MAX)
            {
                pExtra_data = extra_data;
                extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
                    (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
            }

            if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, extra_size + user_extra_data_len, 0, 0, 0, method, bit_flags, dos_time, dos_date))
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += sizeof(local_dir_header);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }
            cur_archive_file_ofs += archive_name_size;

            if (pExtra_data != NULL)
            {
                if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, extra_data, extra_size) != extra_size)
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

                cur_archive_file_ofs += extra_size;
            }
        }
        else
        {
            if ((comp_size > MZ_UINT32_MAX) || (cur_archive_file_ofs > MZ_UINT32_MAX))
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
            if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, user_extra_data_len, 0, 0, 0, method, bit_flags, dos_time, dos_date))
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += sizeof(local_dir_header);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }
            cur_archive_file_ofs += archive_name_size;
        }

        if (user_extra_data_len > 0)
        {
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, user_extra_data, user_extra_data_len) != user_extra_data_len)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += user_extra_data_len;
        }

        if (!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
        {
            uncomp_crc32 = (mz_uint32)mz_crc32(MZ_CRC32_INIT, (const mz_uint8*)pBuf, buf_size);
            uncomp_size = buf_size;
            if (uncomp_size <= 3)
            {
                level = 0;
                store_data_uncompressed = MZ_TRUE;
            }
        }

        if (store_data_uncompressed)
        {
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pBuf, buf_size) != buf_size)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }

            cur_archive_file_ofs += buf_size;
            comp_size = buf_size;
        }
        else if (buf_size)
        {
            mz_zip_writer_add_state state;

            state.m_pZip = pZip;
            state.m_cur_archive_file_ofs = cur_archive_file_ofs;
            state.m_comp_size = 0;

            if ((tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY) ||
                (tdefl_compress_buffer(pComp, pBuf, buf_size, TDEFL_FINISH) != TDEFL_STATUS_DONE))
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                return mz_zip_set_error(pZip, MZ_ZIP_COMPRESSION_FAILED);
            }

            comp_size = state.m_comp_size;
            cur_archive_file_ofs = state.m_cur_archive_file_ofs;
        }

        pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
        pComp = NULL;

        if (uncomp_size)
        {
            mz_uint8 local_dir_footer[MZ_ZIP_DATA_DESCRIPTER_SIZE64];
            mz_uint32 local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE32;

            MZ_ASSERT(bit_flags & MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR);

            MZ_WRITE_LE32(local_dir_footer + 0, MZ_ZIP_DATA_DESCRIPTOR_ID);
            MZ_WRITE_LE32(local_dir_footer + 4, uncomp_crc32);
            if (pExtra_data == NULL)
            {
                if ((comp_size > MZ_UINT32_MAX) || (cur_archive_file_ofs > MZ_UINT32_MAX))
                    return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

                MZ_WRITE_LE32(local_dir_footer + 8, comp_size);
                MZ_WRITE_LE32(local_dir_footer + 12, uncomp_size);
            }
            else
            {
                MZ_WRITE_LE64(local_dir_footer + 8, comp_size);
                MZ_WRITE_LE64(local_dir_footer + 16, uncomp_size);
                local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE64;
            }

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_footer, local_dir_footer_size) != local_dir_footer_size)
                return MZ_FALSE;

            cur_archive_file_ofs += local_dir_footer_size;
        }

        if (pExtra_data != NULL)
        {
            extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
                (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
        }

        if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, pExtra_data, extra_size, pComment,
            comment_size, uncomp_size, comp_size, uncomp_crc32, method, bit_flags, dos_time, dos_date, local_dir_header_ofs, ext_attributes,
            user_extra_data_central, user_extra_data_central_len))
            return MZ_FALSE;

        pZip->m_total_files++;
        pZip->m_archive_size = cur_archive_file_ofs;

        return MZ_TRUE;
    }

#ifndef MINIZ_NO_STDIO
    mz_bool mz_zip_writer_add_cfile(mz_zip_archive* pZip, const char* pArchive_name, MZ_FILE* pSrc_file, mz_uint64 size_to_add, const MZ_TIME_T* pFile_time, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags,
        const char* user_extra_data, mz_uint user_extra_data_len, const char* user_extra_data_central, mz_uint user_extra_data_central_len)
    {
        mz_uint16 gen_flags = MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR;
        mz_uint uncomp_crc32 = MZ_CRC32_INIT, level, num_alignment_padding_bytes;
        mz_uint16 method = 0, dos_time = 0, dos_date = 0, ext_attributes = 0;
        mz_uint64 local_dir_header_ofs, cur_archive_file_ofs = pZip->m_archive_size, uncomp_size = size_to_add, comp_size = 0;
        size_t archive_name_size;
        mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
        mz_uint8* pExtra_data = NULL;
        mz_uint32 extra_size = 0;
        mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
        mz_zip_internal_state* pState;

        if (!(level_and_flags & MZ_ZIP_FLAG_ASCII_FILENAME))
            gen_flags |= MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8;

        if ((int)level_and_flags < 0)
            level_and_flags = MZ_DEFAULT_LEVEL;
        level = level_and_flags & 0xF;

        /* Sanity checks */
        if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || (!pArchive_name) || ((comment_size) && (!pComment)) || (level > MZ_UBER_COMPRESSION))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        if ((!pState->m_zip64) && (uncomp_size > MZ_UINT32_MAX))
        {
            /* Source file is too large for non-zip64 */
            /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
            pState->m_zip64 = MZ_TRUE;
        }

        /* We could support this, but why? */
        if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_writer_validate_archive_name(pArchive_name))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

        if (pState->m_zip64)
        {
            if (pZip->m_total_files == MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }
        else
        {
            if (pZip->m_total_files == MZ_UINT16_MAX)
            {
                pState->m_zip64 = MZ_TRUE;
                /*return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES); */
            }
        }

        archive_name_size = strlen(pArchive_name);
        if (archive_name_size > MZ_UINT16_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

        num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

        /* miniz doesn't support central dirs >= MZ_UINT32_MAX bytes yet */
        if (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE + comment_size) >= MZ_UINT32_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

        if (!pState->m_zip64)
        {
            /* Bail early if the archive would obviously become too large */
            if ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE + 1024) > 0xFFFFFFFF)
            {
                pState->m_zip64 = MZ_TRUE;
                /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
            }
        }

#ifndef MINIZ_NO_TIME
        if (pFile_time)
        {
            mz_zip_time_t_to_dos_time(*pFile_time, &dos_time, &dos_date);
        }
#endif

        if (uncomp_size <= 3)
            level = 0;

        if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes))
        {
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
        }

        cur_archive_file_ofs += num_alignment_padding_bytes;
        local_dir_header_ofs = cur_archive_file_ofs;

        if (pZip->m_file_offset_alignment)
        {
            MZ_ASSERT((cur_archive_file_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
        }

        if (uncomp_size && level)
        {
            method = MZ_DEFLATED;
        }

        MZ_CLEAR_OBJ(local_dir_header);
        if (pState->m_zip64)
        {
            if (uncomp_size >= MZ_UINT32_MAX || local_dir_header_ofs >= MZ_UINT32_MAX)
            {
                pExtra_data = extra_data;
                extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
                    (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
            }

            if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, extra_size + user_extra_data_len, 0, 0, 0, method, gen_flags, dos_time, dos_date))
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += sizeof(local_dir_header);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
            {
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }

            cur_archive_file_ofs += archive_name_size;

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, extra_data, extra_size) != extra_size)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += extra_size;
        }
        else
        {
            if ((comp_size > MZ_UINT32_MAX) || (cur_archive_file_ofs > MZ_UINT32_MAX))
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
            if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, user_extra_data_len, 0, 0, 0, method, gen_flags, dos_time, dos_date))
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += sizeof(local_dir_header);

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
            {
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }

            cur_archive_file_ofs += archive_name_size;
        }

        if (user_extra_data_len > 0)
        {
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, user_extra_data, user_extra_data_len) != user_extra_data_len)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            cur_archive_file_ofs += user_extra_data_len;
        }

        if (uncomp_size)
        {
            mz_uint64 uncomp_remaining = uncomp_size;
            void* pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, MZ_ZIP_MAX_IO_BUF_SIZE);
            if (!pRead_buf)
            {
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (!level)
            {
                while (uncomp_remaining)
                {
                    mz_uint n = (mz_uint)MZ_MIN((mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE, uncomp_remaining);
                    if ((MZ_FREAD(pRead_buf, 1, n, pSrc_file) != n) || (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pRead_buf, n) != n))
                    {
                        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                        return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                    }
                    uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8*)pRead_buf, n);
                    uncomp_remaining -= n;
                    cur_archive_file_ofs += n;
                }
                comp_size = uncomp_size;
            }
            else
            {
                mz_bool result = MZ_FALSE;
                mz_zip_writer_add_state state;
                tdefl_compressor* pComp = (tdefl_compressor*)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
                if (!pComp)
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
                }

                state.m_pZip = pZip;
                state.m_cur_archive_file_ofs = cur_archive_file_ofs;
                state.m_comp_size = 0;

                if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY)
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);
                }

                for (;;)
                {
                    size_t in_buf_size = (mz_uint32)MZ_MIN(uncomp_remaining, (mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE);
                    tdefl_status status;
                    tdefl_flush flush = TDEFL_NO_FLUSH;

                    if (MZ_FREAD(pRead_buf, 1, in_buf_size, pSrc_file) != in_buf_size)
                    {
                        mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                        break;
                    }

                    uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8*)pRead_buf, in_buf_size);
                    uncomp_remaining -= in_buf_size;

                    if (pZip->m_pNeeds_keepalive != NULL && pZip->m_pNeeds_keepalive(pZip->m_pIO_opaque))
                        flush = TDEFL_FULL_FLUSH;

                    status = tdefl_compress_buffer(pComp, pRead_buf, in_buf_size, uncomp_remaining ? flush : TDEFL_FINISH);
                    if (status == TDEFL_STATUS_DONE)
                    {
                        result = MZ_TRUE;
                        break;
                    }
                    else if (status != TDEFL_STATUS_OKAY)
                    {
                        mz_zip_set_error(pZip, MZ_ZIP_COMPRESSION_FAILED);
                        break;
                    }
                }

                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

                if (!result)
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    return MZ_FALSE;
                }

                comp_size = state.m_comp_size;
                cur_archive_file_ofs = state.m_cur_archive_file_ofs;
            }

            pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
        }

        {
            mz_uint8 local_dir_footer[MZ_ZIP_DATA_DESCRIPTER_SIZE64];
            mz_uint32 local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE32;

            MZ_WRITE_LE32(local_dir_footer + 0, MZ_ZIP_DATA_DESCRIPTOR_ID);
            MZ_WRITE_LE32(local_dir_footer + 4, uncomp_crc32);
            if (pExtra_data == NULL)
            {
                if (comp_size > MZ_UINT32_MAX)
                    return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

                MZ_WRITE_LE32(local_dir_footer + 8, comp_size);
                MZ_WRITE_LE32(local_dir_footer + 12, uncomp_size);
            }
            else
            {
                MZ_WRITE_LE64(local_dir_footer + 8, comp_size);
                MZ_WRITE_LE64(local_dir_footer + 16, uncomp_size);
                local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE64;
            }

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_footer, local_dir_footer_size) != local_dir_footer_size)
                return MZ_FALSE;

            cur_archive_file_ofs += local_dir_footer_size;
        }

        if (pExtra_data != NULL)
        {
            extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
                (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
        }

        if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, pExtra_data, extra_size, pComment, comment_size,
            uncomp_size, comp_size, uncomp_crc32, method, gen_flags, dos_time, dos_date, local_dir_header_ofs, ext_attributes,
            user_extra_data_central, user_extra_data_central_len))
            return MZ_FALSE;

        pZip->m_total_files++;
        pZip->m_archive_size = cur_archive_file_ofs;

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_add_file(mz_zip_archive* pZip, const char* pArchive_name, const char* pSrc_filename, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags)
    {
        MZ_FILE* pSrc_file = NULL;
        mz_uint64 uncomp_size = 0;
        MZ_TIME_T file_modified_time;
        MZ_TIME_T* pFile_time = NULL;
        mz_bool status;

        memset(&file_modified_time, 0, sizeof(file_modified_time));

#if !defined(MINIZ_NO_TIME) && !defined(MINIZ_NO_STDIO)
        pFile_time = &file_modified_time;
        if (!mz_zip_get_file_modified_time(pSrc_filename, &file_modified_time))
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_STAT_FAILED);
#endif

        pSrc_file = MZ_FOPEN(pSrc_filename, "rb");
        if (!pSrc_file)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);

        MZ_FSEEK64(pSrc_file, 0, SEEK_END);
        uncomp_size = MZ_FTELL64(pSrc_file);
        MZ_FSEEK64(pSrc_file, 0, SEEK_SET);

        status = mz_zip_writer_add_cfile(pZip, pArchive_name, pSrc_file, uncomp_size, pFile_time, pComment, comment_size, level_and_flags, NULL, 0, NULL, 0);

        MZ_FCLOSE(pSrc_file);

        return status;
    }
#endif /* #ifndef MINIZ_NO_STDIO */

    static mz_bool mz_zip_writer_update_zip64_extension_block(mz_zip_array* pNew_ext, mz_zip_archive* pZip, const mz_uint8* pExt, uint32_t ext_len, mz_uint64* pComp_size, mz_uint64* pUncomp_size, mz_uint64* pLocal_header_ofs, mz_uint32* pDisk_start)
    {
        /* + 64 should be enough for any new zip64 data */
        if (!mz_zip_array_reserve(pZip, pNew_ext, ext_len + 64, MZ_FALSE))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        mz_zip_array_resize(pZip, pNew_ext, 0, MZ_FALSE);

        if ((pUncomp_size) || (pComp_size) || (pLocal_header_ofs) || (pDisk_start))
        {
            mz_uint8 new_ext_block[64];
            mz_uint8* pDst = new_ext_block;
            mz_write_le16(pDst, MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID);
            mz_write_le16(pDst + sizeof(mz_uint16), 0);
            pDst += sizeof(mz_uint16) * 2;

            if (pUncomp_size)
            {
                mz_write_le64(pDst, *pUncomp_size);
                pDst += sizeof(mz_uint64);
            }

            if (pComp_size)
            {
                mz_write_le64(pDst, *pComp_size);
                pDst += sizeof(mz_uint64);
            }

            if (pLocal_header_ofs)
            {
                mz_write_le64(pDst, *pLocal_header_ofs);
                pDst += sizeof(mz_uint64);
            }

            if (pDisk_start)
            {
                mz_write_le32(pDst, *pDisk_start);
                pDst += sizeof(mz_uint32);
            }

            mz_write_le16(new_ext_block + sizeof(mz_uint16), (mz_uint16)((pDst - new_ext_block) - sizeof(mz_uint16) * 2));

            if (!mz_zip_array_push_back(pZip, pNew_ext, new_ext_block, pDst - new_ext_block))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
        }

        if ((pExt) && (ext_len))
        {
            mz_uint32 extra_size_remaining = ext_len;
            const mz_uint8* pExtra_data = pExt;

            do
            {
                mz_uint32 field_id, field_data_size, field_total_size;

                if (extra_size_remaining < (sizeof(mz_uint16) * 2))
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                field_id = MZ_READ_LE16(pExtra_data);
                field_data_size = MZ_READ_LE16(pExtra_data + sizeof(mz_uint16));
                field_total_size = field_data_size + sizeof(mz_uint16) * 2;

                if (field_total_size > extra_size_remaining)
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

                if (field_id != MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID)
                {
                    if (!mz_zip_array_push_back(pZip, pNew_ext, pExtra_data, field_total_size))
                        return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
                }

                pExtra_data += field_total_size;
                extra_size_remaining -= field_total_size;
            } while (extra_size_remaining);
        }

        return MZ_TRUE;
    }

    /* TODO: This func is now pretty freakin complex due to zip64, split it up? */
    mz_bool mz_zip_writer_add_from_zip_reader(mz_zip_archive* pZip, mz_zip_archive* pSource_zip, mz_uint src_file_index)
    {
        mz_uint n, bit_flags, num_alignment_padding_bytes, src_central_dir_following_data_size;
        mz_uint64 src_archive_bytes_remaining, local_dir_header_ofs;
        mz_uint64 cur_src_file_ofs, cur_dst_file_ofs;
        mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
        mz_uint8* pLocal_header = (mz_uint8*)local_header_u32;
        mz_uint8 new_central_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];
        size_t orig_central_dir_size;
        mz_zip_internal_state* pState;
        void* pBuf;
        const mz_uint8* pSrc_central_header;
        mz_zip_archive_file_stat src_file_stat;
        mz_uint32 src_filename_len, src_comment_len, src_ext_len;
        mz_uint32 local_header_filename_size, local_header_extra_len;
        mz_uint64 local_header_comp_size, local_header_uncomp_size;
        mz_bool found_zip64_ext_data_in_ldir = MZ_FALSE;

        /* Sanity checks */
        if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || (!pSource_zip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        /* Don't support copying files from zip64 archives to non-zip64, even though in some cases this is possible */
        if ((pSource_zip->m_pState->m_zip64) && (!pZip->m_pState->m_zip64))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        /* Get pointer to the source central dir header and crack it */
        if (NULL == (pSrc_central_header = mz_zip_get_cdh(pSource_zip, src_file_index)))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (MZ_READ_LE32(pSrc_central_header + MZ_ZIP_CDH_SIG_OFS) != MZ_ZIP_CENTRAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        src_filename_len = MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        src_comment_len = MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_COMMENT_LEN_OFS);
        src_ext_len = MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_EXTRA_LEN_OFS);
        src_central_dir_following_data_size = src_filename_len + src_ext_len + src_comment_len;

        /* TODO: We don't support central dir's >= MZ_UINT32_MAX bytes right now (+32 fudge factor in case we need to add more extra data) */
        if ((pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + src_central_dir_following_data_size + 32) >= MZ_UINT32_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

        num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

        if (!pState->m_zip64)
        {
            if (pZip->m_total_files == MZ_UINT16_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }
        else
        {
            /* TODO: Our zip64 support still has some 32-bit limits that may not be worth fixing. */
            if (pZip->m_total_files == MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }

        if (!mz_zip_file_stat_internal(pSource_zip, src_file_index, pSrc_central_header, &src_file_stat, NULL))
            return MZ_FALSE;

        cur_src_file_ofs = src_file_stat.m_local_header_ofs;
        cur_dst_file_ofs = pZip->m_archive_size;

        /* Read the source archive's local dir header */
        if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);

        if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);

        cur_src_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

        /* Compute the total size we need to copy (filename+extra data+compressed data) */
        local_header_filename_size = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS);
        local_header_extra_len = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
        local_header_comp_size = MZ_READ_LE32(pLocal_header + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS);
        local_header_uncomp_size = MZ_READ_LE32(pLocal_header + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS);
        src_archive_bytes_remaining = local_header_filename_size + local_header_extra_len + src_file_stat.m_comp_size;

        /* Try to find a zip64 extended information field */
        if ((local_header_extra_len) && ((local_header_comp_size == MZ_UINT32_MAX) || (local_header_uncomp_size == MZ_UINT32_MAX)))
        {
            mz_zip_array file_data_array;
            const mz_uint8* pExtra_data;
            mz_uint32 extra_size_remaining = local_header_extra_len;

            mz_zip_array_init(&file_data_array, 1);
            if (!mz_zip_array_resize(pZip, &file_data_array, local_header_extra_len, MZ_FALSE))
            {
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, src_file_stat.m_local_header_ofs + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + local_header_filename_size, file_data_array.m_p, local_header_extra_len) != local_header_extra_len)
            {
                mz_zip_array_clear(pZip, &file_data_array);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
            }

            pExtra_data = (const mz_uint8*)file_data_array.m_p;

            do
            {
                mz_uint32 field_id, field_data_size, field_total_size;

                if (extra_size_remaining < (sizeof(mz_uint16) * 2))
                {
                    mz_zip_array_clear(pZip, &file_data_array);
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                }

                field_id = MZ_READ_LE16(pExtra_data);
                field_data_size = MZ_READ_LE16(pExtra_data + sizeof(mz_uint16));
                field_total_size = field_data_size + sizeof(mz_uint16) * 2;

                if (field_total_size > extra_size_remaining)
                {
                    mz_zip_array_clear(pZip, &file_data_array);
                    return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                }

                if (field_id == MZ_ZIP64_EXTENDED_INFORMATION_FIELD_HEADER_ID)
                {
                    const mz_uint8* pSrc_field_data = pExtra_data + sizeof(mz_uint32);

                    if (field_data_size < sizeof(mz_uint64) * 2)
                    {
                        mz_zip_array_clear(pZip, &file_data_array);
                        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_HEADER_OR_CORRUPTED);
                    }

                    local_header_uncomp_size = MZ_READ_LE64(pSrc_field_data);
                    local_header_comp_size = MZ_READ_LE64(pSrc_field_data + sizeof(mz_uint64)); /* may be 0 if there's a descriptor */

                    found_zip64_ext_data_in_ldir = MZ_TRUE;
                    break;
                }

                pExtra_data += field_total_size;
                extra_size_remaining -= field_total_size;
            } while (extra_size_remaining);

            mz_zip_array_clear(pZip, &file_data_array);
        }

        if (!pState->m_zip64)
        {
            /* Try to detect if the new archive will most likely wind up too big and bail early (+(sizeof(mz_uint32) * 4) is for the optional descriptor which could be present, +64 is a fudge factor). */
            /* We also check when the archive is finalized so this doesn't need to be perfect. */
            mz_uint64 approx_new_archive_size = cur_dst_file_ofs + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + src_archive_bytes_remaining + (sizeof(mz_uint32) * 4) +
                pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + src_central_dir_following_data_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE + 64;

            if (approx_new_archive_size >= MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
        }

        /* Write dest archive padding */
        if (!mz_zip_writer_write_zeros(pZip, cur_dst_file_ofs, num_alignment_padding_bytes))
            return MZ_FALSE;

        cur_dst_file_ofs += num_alignment_padding_bytes;

        local_dir_header_ofs = cur_dst_file_ofs;
        if (pZip->m_file_offset_alignment)
        {
            MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
        }

        /* The original zip's local header+ext block doesn't change, even with zip64, so we can just copy it over to the dest zip */
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

        cur_dst_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

        /* Copy over the source archive bytes to the dest archive, also ensure we have enough buf space to handle optional data descriptor */
        if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)MZ_MAX(32U, MZ_MIN((mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE, src_archive_bytes_remaining)))))
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

        while (src_archive_bytes_remaining)
        {
            n = (mz_uint)MZ_MIN((mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE, src_archive_bytes_remaining);
            if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, n) != n)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
            }
            cur_src_file_ofs += n;

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }
            cur_dst_file_ofs += n;

            src_archive_bytes_remaining -= n;
        }

        /* Now deal with the optional data descriptor */
        bit_flags = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_BIT_FLAG_OFS);
        if (bit_flags & 8)
        {
            /* Copy data descriptor */
            if ((pSource_zip->m_pState->m_zip64) || (found_zip64_ext_data_in_ldir))
            {
                /* src is zip64, dest must be zip64 */

                /* name			uint32_t's */
                /* id				1 (optional in zip64?) */
                /* crc			1 */
                /* comp_size	2 */
                /* uncomp_size 2 */
                if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, (sizeof(mz_uint32) * 6)) != (sizeof(mz_uint32) * 6))
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                }

                n = sizeof(mz_uint32) * ((MZ_READ_LE32(pBuf) == MZ_ZIP_DATA_DESCRIPTOR_ID) ? 6 : 5);
            }
            else
            {
                /* src is NOT zip64 */
                mz_bool has_id;

                if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, sizeof(mz_uint32) * 4) != sizeof(mz_uint32) * 4)
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                }

                has_id = (MZ_READ_LE32(pBuf) == MZ_ZIP_DATA_DESCRIPTOR_ID);

                if (pZip->m_pState->m_zip64)
                {
                    /* dest is zip64, so upgrade the data descriptor */
                    const mz_uint32* pSrc_descriptor = (const mz_uint32*)((const mz_uint8*)pBuf + (has_id ? sizeof(mz_uint32) : 0));
                    const mz_uint32 src_crc32 = pSrc_descriptor[0];
                    const mz_uint64 src_comp_size = pSrc_descriptor[1];
                    const mz_uint64 src_uncomp_size = pSrc_descriptor[2];

                    mz_write_le32((mz_uint8*)pBuf, MZ_ZIP_DATA_DESCRIPTOR_ID);
                    mz_write_le32((mz_uint8*)pBuf + sizeof(mz_uint32) * 1, src_crc32);
                    mz_write_le64((mz_uint8*)pBuf + sizeof(mz_uint32) * 2, src_comp_size);
                    mz_write_le64((mz_uint8*)pBuf + sizeof(mz_uint32) * 4, src_uncomp_size);

                    n = sizeof(mz_uint32) * 6;
                }
                else
                {
                    /* dest is NOT zip64, just copy it as-is */
                    n = sizeof(mz_uint32) * (has_id ? 4 : 3);
                }
            }

            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
            }

            cur_src_file_ofs += n;
            cur_dst_file_ofs += n;
        }
        pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);

        /* Finally, add the new central dir header */
        orig_central_dir_size = pState->m_central_dir.m_size;

        memcpy(new_central_header, pSrc_central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);

        if (pState->m_zip64)
        {
            /* This is the painful part: We need to write a new central dir header + ext block with updated zip64 fields, and ensure the old fields (if any) are not included. */
            const mz_uint8* pSrc_ext = pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + src_filename_len;
            mz_zip_array new_ext_block;

            mz_zip_array_init(&new_ext_block, sizeof(mz_uint8));

            MZ_WRITE_LE32(new_central_header + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS, MZ_UINT32_MAX);
            MZ_WRITE_LE32(new_central_header + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS, MZ_UINT32_MAX);
            MZ_WRITE_LE32(new_central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS, MZ_UINT32_MAX);

            if (!mz_zip_writer_update_zip64_extension_block(&new_ext_block, pZip, pSrc_ext, src_ext_len, &src_file_stat.m_comp_size, &src_file_stat.m_uncomp_size, &local_dir_header_ofs, NULL))
            {
                mz_zip_array_clear(pZip, &new_ext_block);
                return MZ_FALSE;
            }

            MZ_WRITE_LE16(new_central_header + MZ_ZIP_CDH_EXTRA_LEN_OFS, new_ext_block.m_size);

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, new_central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE))
            {
                mz_zip_array_clear(pZip, &new_ext_block);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, src_filename_len))
            {
                mz_zip_array_clear(pZip, &new_ext_block);
                mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, new_ext_block.m_p, new_ext_block.m_size))
            {
                mz_zip_array_clear(pZip, &new_ext_block);
                mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + src_filename_len + src_ext_len, src_comment_len))
            {
                mz_zip_array_clear(pZip, &new_ext_block);
                mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            mz_zip_array_clear(pZip, &new_ext_block);
        }
        else
        {
            /* sanity checks */
            if (cur_dst_file_ofs > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

            if (local_dir_header_ofs >= MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

            MZ_WRITE_LE32(new_central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS, local_dir_header_ofs);

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, new_central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE))
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);

            if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, src_central_dir_following_data_size))
            {
                mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }
        }

        /* This shouldn't trigger unless we screwed up during the initial sanity checks */
        if (pState->m_central_dir.m_size >= MZ_UINT32_MAX)
        {
            /* TODO: Support central dirs >= 32-bits in size */
            mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
            return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);
        }

        n = (mz_uint32)orig_central_dir_size;
        if (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &n, 1))
        {
            mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
        }

        pZip->m_total_files++;
        pZip->m_archive_size = cur_dst_file_ofs;

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_finalize_archive(mz_zip_archive* pZip)
    {
        mz_zip_internal_state* pState;
        mz_uint64 central_dir_ofs, central_dir_size;
        mz_uint8 hdr[256];

        if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        pState = pZip->m_pState;

        if (pState->m_zip64)
        {
            if ((pZip->m_total_files > MZ_UINT32_MAX) || (pState->m_central_dir.m_size >= MZ_UINT32_MAX))
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }
        else
        {
            if ((pZip->m_total_files > MZ_UINT16_MAX) || ((pZip->m_archive_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) > MZ_UINT32_MAX))
                return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
        }

        central_dir_ofs = 0;
        central_dir_size = 0;
        if (pZip->m_total_files)
        {
            /* Write central directory */
            central_dir_ofs = pZip->m_archive_size;
            central_dir_size = pState->m_central_dir.m_size;
            pZip->m_central_directory_file_ofs = central_dir_ofs;
            if (pZip->m_pWrite(pZip->m_pIO_opaque, central_dir_ofs, pState->m_central_dir.m_p, (size_t)central_dir_size) != central_dir_size)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            pZip->m_archive_size += central_dir_size;
        }

        if (pState->m_zip64)
        {
            /* Write zip64 end of central directory header */
            mz_uint64 rel_ofs_to_zip64_ecdr = pZip->m_archive_size;

            MZ_CLEAR_OBJ(hdr);
            MZ_WRITE_LE32(hdr + MZ_ZIP64_ECDH_SIG_OFS, MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIG);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDH_SIZE_OF_RECORD_OFS, MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE - sizeof(mz_uint32) - sizeof(mz_uint64));
            MZ_WRITE_LE16(hdr + MZ_ZIP64_ECDH_VERSION_MADE_BY_OFS, 0x031E); /* TODO: always Unix */
            MZ_WRITE_LE16(hdr + MZ_ZIP64_ECDH_VERSION_NEEDED_OFS, 0x002D);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS, pZip->m_total_files);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDH_CDIR_TOTAL_ENTRIES_OFS, pZip->m_total_files);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDH_CDIR_SIZE_OFS, central_dir_size);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDH_CDIR_OFS_OFS, central_dir_ofs);
            if (pZip->m_pWrite(pZip->m_pIO_opaque, pZip->m_archive_size, hdr, MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE) != MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            pZip->m_archive_size += MZ_ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE;

            /* Write zip64 end of central directory locator */
            MZ_CLEAR_OBJ(hdr);
            MZ_WRITE_LE32(hdr + MZ_ZIP64_ECDL_SIG_OFS, MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIG);
            MZ_WRITE_LE64(hdr + MZ_ZIP64_ECDL_REL_OFS_TO_ZIP64_ECDR_OFS, rel_ofs_to_zip64_ecdr);
            MZ_WRITE_LE32(hdr + MZ_ZIP64_ECDL_TOTAL_NUMBER_OF_DISKS_OFS, 1);
            if (pZip->m_pWrite(pZip->m_pIO_opaque, pZip->m_archive_size, hdr, MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE) != MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE)
                return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

            pZip->m_archive_size += MZ_ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE;
        }

        /* Write end of central directory record */
        MZ_CLEAR_OBJ(hdr);
        MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_SIG_OFS, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG);
        MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS, MZ_MIN(MZ_UINT16_MAX, pZip->m_total_files));
        MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS, MZ_MIN(MZ_UINT16_MAX, pZip->m_total_files));
        MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_SIZE_OFS, MZ_MIN(MZ_UINT32_MAX, central_dir_size));
        MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_OFS_OFS, MZ_MIN(MZ_UINT32_MAX, central_dir_ofs));

        if (pZip->m_pWrite(pZip->m_pIO_opaque, pZip->m_archive_size, hdr, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

#ifndef MINIZ_NO_STDIO
        if ((pState->m_pFile) && (MZ_FFLUSH(pState->m_pFile) == EOF))
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_CLOSE_FAILED);
#endif /* #ifndef MINIZ_NO_STDIO */

        pZip->m_archive_size += MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE;

        pZip->m_zip_mode = MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED;
        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_finalize_heap_archive(mz_zip_archive* pZip, void** ppBuf, size_t* pSize)
    {
        if ((!ppBuf) || (!pSize))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        *ppBuf = NULL;
        *pSize = 0;

        if ((!pZip) || (!pZip->m_pState))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (pZip->m_pWrite != mz_zip_heap_write_func)
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        if (!mz_zip_writer_finalize_archive(pZip))
            return MZ_FALSE;

        *ppBuf = pZip->m_pState->m_pMem;
        *pSize = pZip->m_pState->m_mem_size;
        pZip->m_pState->m_pMem = NULL;
        pZip->m_pState->m_mem_size = pZip->m_pState->m_mem_capacity = 0;

        return MZ_TRUE;
    }

    mz_bool mz_zip_writer_end(mz_zip_archive* pZip)
    {
        return mz_zip_writer_end_internal(pZip, MZ_TRUE);
    }

#ifndef MINIZ_NO_STDIO
    mz_bool mz_zip_add_mem_to_archive_file_in_place(const char* pZip_filename, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags)
    {
        return mz_zip_add_mem_to_archive_file_in_place_v2(pZip_filename, pArchive_name, pBuf, buf_size, pComment, comment_size, level_and_flags, NULL);
    }

    mz_bool mz_zip_add_mem_to_archive_file_in_place_v2(const char* pZip_filename, const char* pArchive_name, const void* pBuf, size_t buf_size, const void* pComment, mz_uint16 comment_size, mz_uint level_and_flags, mz_zip_error* pErr)
    {
        mz_bool status, created_new_archive = MZ_FALSE;
        mz_zip_archive zip_archive;
        struct MZ_FILE_STAT_STRUCT file_stat;
        mz_zip_error actual_err = MZ_ZIP_NO_ERROR;

        mz_zip_zero_struct(&zip_archive);
        if ((int)level_and_flags < 0)
            level_and_flags = MZ_DEFAULT_LEVEL;

        if ((!pZip_filename) || (!pArchive_name) || ((buf_size) && (!pBuf)) || ((comment_size) && (!pComment)) || ((level_and_flags & 0xF) > MZ_UBER_COMPRESSION))
        {
            if (pErr)
                *pErr = MZ_ZIP_INVALID_PARAMETER;
            return MZ_FALSE;
        }

        if (!mz_zip_writer_validate_archive_name(pArchive_name))
        {
            if (pErr)
                *pErr = MZ_ZIP_INVALID_FILENAME;
            return MZ_FALSE;
        }

        /* Important: The regular non-64 bit version of stat() can fail here if the file is very large, which could cause the archive to be overwritten. */
        /* So be sure to compile with _LARGEFILE64_SOURCE 1 */
        if (MZ_FILE_STAT(pZip_filename, &file_stat) != 0)
        {
            /* Create a new archive. */
            if (!mz_zip_writer_init_file_v2(&zip_archive, pZip_filename, 0, level_and_flags))
            {
                if (pErr)
                    *pErr = zip_archive.m_last_error;
                return MZ_FALSE;
            }

            created_new_archive = MZ_TRUE;
        }
        else
        {
            /* Append to an existing archive. */
            if (!mz_zip_reader_init_file_v2(&zip_archive, pZip_filename, level_and_flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0))
            {
                if (pErr)
                    *pErr = zip_archive.m_last_error;
                return MZ_FALSE;
            }

            if (!mz_zip_writer_init_from_reader_v2(&zip_archive, pZip_filename, level_and_flags))
            {
                if (pErr)
                    *pErr = zip_archive.m_last_error;

                mz_zip_reader_end_internal(&zip_archive, MZ_FALSE);

                return MZ_FALSE;
            }
        }

        status = mz_zip_writer_add_mem_ex(&zip_archive, pArchive_name, pBuf, buf_size, pComment, comment_size, level_and_flags, 0, 0);
        actual_err = zip_archive.m_last_error;

        /* Always finalize, even if adding failed for some reason, so we have a valid central directory. (This may not always succeed, but we can try.) */
        if (!mz_zip_writer_finalize_archive(&zip_archive))
        {
            if (!actual_err)
                actual_err = zip_archive.m_last_error;

            status = MZ_FALSE;
        }

        if (!mz_zip_writer_end_internal(&zip_archive, status))
        {
            if (!actual_err)
                actual_err = zip_archive.m_last_error;

            status = MZ_FALSE;
        }

        if ((!status) && (created_new_archive))
        {
            /* It's a new archive and something went wrong, so just delete it. */
            int ignoredStatus = MZ_DELETE_FILE(pZip_filename);
            (void)ignoredStatus;
        }

        if (pErr)
            *pErr = actual_err;

        return status;
    }

    void* mz_zip_extract_archive_file_to_heap_v2(const char* pZip_filename, const char* pArchive_name, const char* pComment, size_t* pSize, mz_uint flags, mz_zip_error* pErr)
    {
        mz_uint32 file_index;
        mz_zip_archive zip_archive;
        void* p = NULL;

        if (pSize)
            *pSize = 0;

        if ((!pZip_filename) || (!pArchive_name))
        {
            if (pErr)
                *pErr = MZ_ZIP_INVALID_PARAMETER;

            return NULL;
        }

        mz_zip_zero_struct(&zip_archive);
        if (!mz_zip_reader_init_file_v2(&zip_archive, pZip_filename, flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0))
        {
            if (pErr)
                *pErr = zip_archive.m_last_error;

            return NULL;
        }

        if (mz_zip_reader_locate_file_v2(&zip_archive, pArchive_name, pComment, flags, &file_index))
        {
            p = mz_zip_reader_extract_to_heap(&zip_archive, file_index, pSize, flags);
        }

        mz_zip_reader_end_internal(&zip_archive, p != NULL);

        if (pErr)
            *pErr = zip_archive.m_last_error;

        return p;
    }

    void* mz_zip_extract_archive_file_to_heap(const char* pZip_filename, const char* pArchive_name, size_t* pSize, mz_uint flags)
    {
        return mz_zip_extract_archive_file_to_heap_v2(pZip_filename, pArchive_name, NULL, pSize, flags, NULL);
    }

#endif /* #ifndef MINIZ_NO_STDIO */

#endif /* #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS */

    /* ------------------- Misc utils */

    mz_zip_mode mz_zip_get_mode(mz_zip_archive* pZip)
    {
        return pZip ? pZip->m_zip_mode : MZ_ZIP_MODE_INVALID;
    }

    mz_zip_type mz_zip_get_type(mz_zip_archive* pZip)
    {
        return pZip ? pZip->m_zip_type : MZ_ZIP_TYPE_INVALID;
    }

    mz_zip_error mz_zip_set_last_error(mz_zip_archive* pZip, mz_zip_error err_num)
    {
        mz_zip_error prev_err;

        if (!pZip)
            return MZ_ZIP_INVALID_PARAMETER;

        prev_err = pZip->m_last_error;

        pZip->m_last_error = err_num;
        return prev_err;
    }

    mz_zip_error mz_zip_peek_last_error(mz_zip_archive* pZip)
    {
        if (!pZip)
            return MZ_ZIP_INVALID_PARAMETER;

        return pZip->m_last_error;
    }

    mz_zip_error mz_zip_clear_last_error(mz_zip_archive* pZip)
    {
        return mz_zip_set_last_error(pZip, MZ_ZIP_NO_ERROR);
    }

    mz_zip_error mz_zip_get_last_error(mz_zip_archive* pZip)
    {
        mz_zip_error prev_err;

        if (!pZip)
            return MZ_ZIP_INVALID_PARAMETER;

        prev_err = pZip->m_last_error;

        pZip->m_last_error = MZ_ZIP_NO_ERROR;
        return prev_err;
    }

    const char* mz_zip_get_error_string(mz_zip_error mz_err)
    {
        switch (mz_err)
        {
        case MZ_ZIP_NO_ERROR:
            return "no error";
        case MZ_ZIP_UNDEFINED_ERROR:
            return "undefined error";
        case MZ_ZIP_TOO_MANY_FILES:
            return "too many files";
        case MZ_ZIP_FILE_TOO_LARGE:
            return "file too large";
        case MZ_ZIP_UNSUPPORTED_METHOD:
            return "unsupported method";
        case MZ_ZIP_UNSUPPORTED_ENCRYPTION:
            return "unsupported encryption";
        case MZ_ZIP_UNSUPPORTED_FEATURE:
            return "unsupported feature";
        case MZ_ZIP_FAILED_FINDING_CENTRAL_DIR:
            return "failed finding central directory";
        case MZ_ZIP_NOT_AN_ARCHIVE:
            return "not a ZIP archive";
        case MZ_ZIP_INVALID_HEADER_OR_CORRUPTED:
            return "invalid header or archive is corrupted";
        case MZ_ZIP_UNSUPPORTED_MULTIDISK:
            return "unsupported multidisk archive";
        case MZ_ZIP_DECOMPRESSION_FAILED:
            return "decompression failed or archive is corrupted";
        case MZ_ZIP_COMPRESSION_FAILED:
            return "compression failed";
        case MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE:
            return "unexpected decompressed size";
        case MZ_ZIP_CRC_CHECK_FAILED:
            return "CRC-32 check failed";
        case MZ_ZIP_UNSUPPORTED_CDIR_SIZE:
            return "unsupported central directory size";
        case MZ_ZIP_ALLOC_FAILED:
            return "allocation failed";
        case MZ_ZIP_FILE_OPEN_FAILED:
            return "file open failed";
        case MZ_ZIP_FILE_CREATE_FAILED:
            return "file create failed";
        case MZ_ZIP_FILE_WRITE_FAILED:
            return "file write failed";
        case MZ_ZIP_FILE_READ_FAILED:
            return "file read failed";
        case MZ_ZIP_FILE_CLOSE_FAILED:
            return "file close failed";
        case MZ_ZIP_FILE_SEEK_FAILED:
            return "file seek failed";
        case MZ_ZIP_FILE_STAT_FAILED:
            return "file stat failed";
        case MZ_ZIP_INVALID_PARAMETER:
            return "invalid parameter";
        case MZ_ZIP_INVALID_FILENAME:
            return "invalid filename";
        case MZ_ZIP_BUF_TOO_SMALL:
            return "buffer too small";
        case MZ_ZIP_INTERNAL_ERROR:
            return "internal error";
        case MZ_ZIP_FILE_NOT_FOUND:
            return "file not found";
        case MZ_ZIP_ARCHIVE_TOO_LARGE:
            return "archive is too large";
        case MZ_ZIP_VALIDATION_FAILED:
            return "validation failed";
        case MZ_ZIP_WRITE_CALLBACK_FAILED:
            return "write calledback failed";
        default:
            break;
        }

        return "unknown error";
    }

    /* Note: Just because the archive is not zip64 doesn't necessarily mean it doesn't have Zip64 extended information extra field, argh. */
    mz_bool mz_zip_is_zip64(mz_zip_archive* pZip)
    {
        if ((!pZip) || (!pZip->m_pState))
            return MZ_FALSE;

        return pZip->m_pState->m_zip64;
    }

    size_t mz_zip_get_central_dir_size(mz_zip_archive* pZip)
    {
        if ((!pZip) || (!pZip->m_pState))
            return 0;

        return pZip->m_pState->m_central_dir.m_size;
    }

    mz_uint mz_zip_reader_get_num_files(mz_zip_archive* pZip)
    {
        return pZip ? pZip->m_total_files : 0;
    }

    mz_uint64 mz_zip_get_archive_size(mz_zip_archive* pZip)
    {
        if (!pZip)
            return 0;
        return pZip->m_archive_size;
    }

    mz_uint64 mz_zip_get_archive_file_start_offset(mz_zip_archive* pZip)
    {
        if ((!pZip) || (!pZip->m_pState))
            return 0;
        return pZip->m_pState->m_file_archive_start_ofs;
    }

    MZ_FILE* mz_zip_get_cfile(mz_zip_archive* pZip)
    {
        if ((!pZip) || (!pZip->m_pState))
            return 0;
        return pZip->m_pState->m_pFile;
    }

    size_t mz_zip_read_archive_data(mz_zip_archive* pZip, mz_uint64 file_ofs, void* pBuf, size_t n)
    {
        if ((!pZip) || (!pZip->m_pState) || (!pBuf) || (!pZip->m_pRead))
            return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

        return pZip->m_pRead(pZip->m_pIO_opaque, file_ofs, pBuf, n);
    }

    mz_uint mz_zip_reader_get_filename(mz_zip_archive* pZip, mz_uint file_index, char* pFilename, mz_uint filename_buf_size)
    {
        mz_uint n;
        const mz_uint8* p = mz_zip_get_cdh(pZip, file_index);
        if (!p)
        {
            if (filename_buf_size)
                pFilename[0] = '\0';
            mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
            return 0;
        }
        n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        if (filename_buf_size)
        {
            n = MZ_MIN(n, filename_buf_size - 1);
            memcpy(pFilename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
            pFilename[n] = '\0';
        }
        return n + 1;
    }

    mz_bool mz_zip_reader_file_stat(mz_zip_archive* pZip, mz_uint file_index, mz_zip_archive_file_stat* pStat)
    {
        return mz_zip_file_stat_internal(pZip, file_index, mz_zip_get_cdh(pZip, file_index), pStat, NULL);
    }

    mz_bool mz_zip_end(mz_zip_archive* pZip)
    {
        if (!pZip)
            return MZ_FALSE;

        if (pZip->m_zip_mode == MZ_ZIP_MODE_READING)
            return mz_zip_reader_end(pZip);
#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
        else if ((pZip->m_zip_mode == MZ_ZIP_MODE_WRITING) || (pZip->m_zip_mode == MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED))
            return mz_zip_writer_end(pZip);
#endif

        return MZ_FALSE;
    }

#ifdef __cplusplus
}
#endif

#endif /*#ifndef MINIZ_NO_ARCHIVE_APIS*/



namespace ofbx
{


	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
#ifdef _WIN32
	typedef long long i64;
	typedef unsigned long long u64;
#else
	typedef long i64;
	typedef unsigned long u64;
#endif

	static_assert(sizeof(u8) == 1, "u8 is not 1 byte");
	static_assert(sizeof(u32) == 4, "u32 is not 4 bytes");
	static_assert(sizeof(u64) == 8, "u64 is not 8 bytes");
	static_assert(sizeof(i64) == 8, "i64 is not 8 bytes");


	using JobFunction = void (*)(void*);
	using JobProcessor = void (*)(JobFunction, void*, void*, u32, u32);

	enum class LoadFlags : u64 {
		TRIANGULATE = 1 << 0,
		IGNORE_GEOMETRY = 1 << 1,
		IGNORE_BLEND_SHAPES = 1 << 2,
	};


	struct Vec2
	{
		double x, y;
	};


	struct Vec3
	{
		double x, y, z;
	};


	struct Vec4
	{
		double x, y, z, w;
	};


	struct Matrix
	{
		double m[16]; // last 4 are translation
	};


	struct Quat
	{
		double x, y, z, w;
	};


	struct Color
	{
		float r, g, b;
	};


	struct DataView
	{
		const u8* begin = nullptr;
		const u8* end = nullptr;
		bool is_binary = true;

		bool operator!=(const char* rhs) const { return !(*this == rhs); }
		bool operator==(const char* rhs) const;

		u64 toU64() const;
		i64 toI64() const;
		int toInt() const;
		u32 toU32() const;
		double toDouble() const;
		float toFloat() const;

		template <int N>
		void toString(char(&out)[N]) const
		{
			char* cout = out;
			const u8* cin = begin;
			while (cin != end && cout - out < N - 1)
			{
				*cout = (char)*cin;
				++cin;
				++cout;
			}
			*cout = '\0';
		}
	};


	struct IElementProperty
	{
		enum Type : unsigned char
		{
			LONG = 'L',
			INTEGER = 'I',
			STRING = 'S',
			FLOAT = 'F',
			DOUBLE = 'D',
			ARRAY_DOUBLE = 'd',
			ARRAY_INT = 'i',
			ARRAY_LONG = 'l',
			ARRAY_FLOAT = 'f',
			BINARY = 'R'
		};
		virtual ~IElementProperty() {}
		virtual Type getType() const = 0;
		virtual IElementProperty* getNext() const = 0;
		virtual DataView getValue() const = 0;
		virtual int getCount() const = 0;
		virtual bool getValues(double* values, int max_size) const = 0;
		virtual bool getValues(int* values, int max_size) const = 0;
		virtual bool getValues(float* values, int max_size) const = 0;
		virtual bool getValues(u64* values, int max_size) const = 0;
		virtual bool getValues(i64* values, int max_size) const = 0;
	};


	struct IElement
	{
		virtual ~IElement() = default;
		virtual IElement* getFirstChild() const = 0;
		virtual IElement* getSibling() const = 0;
		virtual DataView getID() const = 0;
		virtual IElementProperty* getFirstProperty() const = 0;
	};


	enum class RotationOrder
	{
		EULER_XYZ,
		EULER_XZY,
		EULER_YZX,
		EULER_YXZ,
		EULER_ZXY,
		EULER_ZYX,
		SPHERIC_XYZ // Currently unsupported. Treated as EULER_XYZ.
	};


	struct AnimationCurveNode;
	struct AnimationLayer;
	struct Scene;
	struct IScene;


	struct Object
	{
		enum class Type
		{
			ROOT,
			GEOMETRY,
			SHAPE,
			MATERIAL,
			MESH,
			TEXTURE,
			LIMB_NODE,
			NULL_NODE,
			NODE_ATTRIBUTE,
			CLUSTER,
			SKIN,
			BLEND_SHAPE,
			BLEND_SHAPE_CHANNEL,
			ANIMATION_STACK,
			ANIMATION_LAYER,
			ANIMATION_CURVE,
			ANIMATION_CURVE_NODE,
			POSE
		};

		Object(const Scene& _scene, const IElement& _element);

		virtual ~Object() {}
		virtual Type getType() const = 0;

		const IScene& getScene() const;
		Object* resolveObjectLink(int idx) const;
		Object* resolveObjectLink(Type type, const char* property, int idx) const;
		Object* resolveObjectLinkReverse(Type type) const;
		Object* getParent() const;

		RotationOrder getRotationOrder() const;
		Vec3 getRotationOffset() const;
		Vec3 getRotationPivot() const;
		Vec3 getPostRotation() const;
		Vec3 getScalingOffset() const;
		Vec3 getScalingPivot() const;
		Vec3 getPreRotation() const;
		Vec3 getLocalTranslation() const;
		Vec3 getLocalRotation() const;
		Vec3 getLocalScaling() const;
		Matrix getGlobalTransform() const;
		Matrix getLocalTransform() const;
		Matrix evalLocal(const Vec3& translation, const Vec3& rotation) const;
		Matrix evalLocal(const Vec3& translation, const Vec3& rotation, const Vec3& scaling) const;
		bool isNode() const { return is_node; }


		template <typename T> T* resolveObjectLink(int idx) const
		{
			return static_cast<T*>(resolveObjectLink(T::s_type, nullptr, idx));
		}

		u64 id;
		char name[128];
		const IElement& element;
		const Object* node_attribute;

	protected:
		bool is_node;
		const Scene& scene;
	};


	struct Pose : Object {
		static const Type s_type = Type::POSE;
		Pose(const Scene& _scene, const IElement& _element);

		virtual Matrix getMatrix() const = 0;
		virtual const Object* getNode() const = 0;
	};


	struct Texture : Object
	{
		enum TextureType
		{
			DIFFUSE,
			NORMAL,
			SPECULAR,
			SHININESS,
			AMBIENT,
			EMISSIVE,
			REFLECTION,
			COUNT
		};

		static const Type s_type = Type::TEXTURE;

		Texture(const Scene& _scene, const IElement& _element);
		virtual DataView getFileName() const = 0;
		virtual DataView getRelativeFileName() const = 0;
		virtual DataView getEmbeddedData() const = 0;
	};


	struct Material : Object
	{
		static const Type s_type = Type::MATERIAL;

		Material(const Scene& _scene, const IElement& _element);

		virtual Color getDiffuseColor() const = 0;
		virtual Color getSpecularColor() const = 0;
		virtual Color getReflectionColor() const = 0;
		virtual Color getAmbientColor() const = 0;
		virtual Color getEmissiveColor() const = 0;

		virtual double getDiffuseFactor() const = 0;
		virtual double getSpecularFactor() const = 0;
		virtual double getReflectionFactor() const = 0;
		virtual double getShininess() const = 0;
		virtual double getShininessExponent() const = 0;
		virtual double getAmbientFactor() const = 0;
		virtual double getBumpFactor() const = 0;
		virtual double getEmissiveFactor() const = 0;

		virtual const Texture* getTexture(Texture::TextureType type) const = 0;
	};


	struct Cluster : Object
	{
		static const Type s_type = Type::CLUSTER;

		Cluster(const Scene& _scene, const IElement& _element);

		virtual const int* getIndices() const = 0;
		virtual int getIndicesCount() const = 0;
		virtual const double* getWeights() const = 0;
		virtual int getWeightsCount() const = 0;
		virtual Matrix getTransformMatrix() const = 0;
		virtual Matrix getTransformLinkMatrix() const = 0;
		virtual const Object* getLink() const = 0;
	};


	struct Skin : Object
	{
		static const Type s_type = Type::SKIN;

		Skin(const Scene& _scene, const IElement& _element);

		virtual int getClusterCount() const = 0;
		virtual const Cluster* getCluster(int idx) const = 0;
	};


	struct BlendShapeChannel : Object
	{
		static const Type s_type = Type::BLEND_SHAPE_CHANNEL;

		BlendShapeChannel(const Scene& _scene, const IElement& _element);

		virtual double getDeformPercent() const = 0;
		virtual int getShapeCount() const = 0;
		virtual const struct Shape* getShape(int idx) const = 0;
	};


	struct BlendShape : Object
	{
		static const Type s_type = Type::BLEND_SHAPE;

		BlendShape(const Scene& _scene, const IElement& _element);

		virtual int getBlendShapeChannelCount() const = 0;
		virtual const BlendShapeChannel* getBlendShapeChannel(int idx) const = 0;
	};


	struct NodeAttribute : Object
	{
		static const Type s_type = Type::NODE_ATTRIBUTE;

		NodeAttribute(const Scene& _scene, const IElement& _element);

		virtual DataView getAttributeType() const = 0;
	};


	struct Geometry : Object
	{
		static const Type s_type = Type::GEOMETRY;
		static const int s_uvs_max = 4;

		Geometry(const Scene& _scene, const IElement& _element);

		virtual const Vec3* getVertices() const = 0;
		virtual int getVertexCount() const = 0;

		virtual const int* getFaceIndices() const = 0;
        virtual const int* getTo_old_vertices() const = 0;
		virtual int getIndexCount() const = 0;

		virtual const Vec3* getNormals() const = 0;
		virtual const Vec2* getUVs(int index = 0) const = 0;
		virtual const Vec4* getColors() const = 0;
		virtual const Vec3* getTangents() const = 0;
		virtual const Skin* getSkin() const = 0;
		virtual const BlendShape* getBlendShape() const = 0;
		virtual const int* getMaterials() const = 0;
	};


	struct Shape : Object
	{
		static const Type s_type = Type::SHAPE;

		Shape(const Scene& _scene, const IElement& _element);

		virtual const Vec3* getVertices() const = 0;
		virtual int getVertexCount() const = 0;

		virtual const Vec3* getNormals() const = 0;
	};


	struct Mesh : Object
	{
		static const Type s_type = Type::MESH;

		Mesh(const Scene& _scene, const IElement& _element);

		virtual const Pose* getPose() const = 0;
		virtual const Geometry* getGeometry() const = 0;
		virtual Matrix getGeometricMatrix() const = 0;
		virtual const Material* getMaterial(int idx) const = 0;
		virtual int getMaterialCount() const = 0;
	};


	struct AnimationStack : Object
	{
		static const Type s_type = Type::ANIMATION_STACK;

		AnimationStack(const Scene& _scene, const IElement& _element);
		virtual const AnimationLayer* getLayer(int index) const = 0;
	};


	struct AnimationLayer : Object
	{
		static const Type s_type = Type::ANIMATION_LAYER;

		AnimationLayer(const Scene& _scene, const IElement& _element);

		virtual const AnimationCurveNode* getCurveNode(int index) const = 0;
		virtual const AnimationCurveNode* getCurveNode(const Object& bone, const char* property) const = 0;
	};


	struct AnimationCurve : Object
	{
		static const Type s_type = Type::ANIMATION_CURVE;

		AnimationCurve(const Scene& _scene, const IElement& _element);

		virtual int getKeyCount() const = 0;
		virtual const i64* getKeyTime() const = 0;
		virtual const float* getKeyValue() const = 0;
	};


	struct AnimationCurveNode : Object
	{
		static const Type s_type = Type::ANIMATION_CURVE_NODE;

		AnimationCurveNode(const Scene& _scene, const IElement& _element);

		virtual const AnimationCurve* getCurve(int idx) const = 0;
		virtual Vec3 getNodeLocalTransform(double time) const = 0;
		virtual const Object* getBone() const = 0;
	};


	struct TakeInfo
	{
		DataView name;
		DataView filename;
		double local_time_from;
		double local_time_to;
		double reference_time_from;
		double reference_time_to;
	};


	// Specifies which canonical axis represents up in the system (typically Y or Z).
	enum UpVector
	{
		UpVector_AxisX = 0,
		UpVector_AxisY = 1,
		UpVector_AxisZ = 2
	};


	// Specifies the third vector of the system.
	enum CoordSystem
	{
		CoordSystem_RightHanded = 0,
		CoordSystem_LeftHanded = 1
	};


	// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/index.html?url=cpp_ref/class_fbx_time.html,topicNumber=cpp_ref_class_fbx_time_html29087af6-8c2c-4e9d-aede-7dc5a1c2436c,hash=a837590fd5310ff5df56ffcf7c394787e
	enum FrameRate
	{
		FrameRate_DEFAULT = 0,
		FrameRate_120 = 1,
		FrameRate_100 = 2,
		FrameRate_60 = 3,
		FrameRate_50 = 4,
		FrameRate_48 = 5,
		FrameRate_30 = 6,
		FrameRate_30_DROP = 7,
		FrameRate_NTSC_DROP_FRAME = 8,
		FrameRate_NTSC_FULL_FRAME = 9,
		FrameRate_PAL = 10,
		FrameRate_CINEMA = 11,
		FrameRate_1000 = 12,
		FrameRate_CINEMA_ND = 13,
		FrameRate_CUSTOM = 14,
	};


	struct GlobalSettings
	{
		UpVector UpAxis = UpVector_AxisX;
		int UpAxisSign = 1;
		// this seems to be 1-2 in Autodesk (odd/even parity), and 0-2 in Blender (axis as in UpAxis)
		// I recommend to ignore FrontAxis and use just UpVector
		int FrontAxis = 1;
		int FrontAxisSign = 1;
		CoordSystem CoordAxis = CoordSystem_RightHanded;
		int CoordAxisSign = 1;
		int OriginalUpAxis = 0;
		int OriginalUpAxisSign = 1;
		float UnitScaleFactor = 1;
		float OriginalUnitScaleFactor = 1;
		double TimeSpanStart = 0L;
		double TimeSpanStop = 0L;
		FrameRate TimeMode = FrameRate_DEFAULT;
		float CustomFrameRate = -1.0f;
	};


	struct IScene
	{
		virtual void destroy() = 0;
		virtual const IElement* getRootElement() const = 0;
		virtual const Object* getRoot() const = 0;
		virtual const TakeInfo* getTakeInfo(const char* name) const = 0;
		virtual int getGeometryCount() const = 0;
		virtual int getMeshCount() const = 0;
		virtual float getSceneFrameRate() const = 0;
		virtual const GlobalSettings* getGlobalSettings() const = 0;
		virtual const Mesh* getMesh(int index) const = 0;
		virtual const Geometry* getGeometry(int index) const = 0;
		virtual int getAnimationStackCount() const = 0;
		virtual const AnimationStack* getAnimationStack(int index) const = 0;
		virtual const Object* const* getAllObjects() const = 0;
		virtual int getAllObjectCount() const = 0;
		virtual int getEmbeddedDataCount() const = 0;
		virtual DataView getEmbeddedData(int index) const = 0;
		virtual DataView getEmbeddedFilename(int index) const = 0;

	protected:
		virtual ~IScene() {}
	};


	IScene* load(const u8* data, int size, u64 flags, JobProcessor job_processor = nullptr, void* job_user_ptr = nullptr);
	const char* getError();
	double fbxTimeToSeconds(i64 value);
	i64 secondsToFbxTime(double value);


} // namespace ofbx

#include <cassert>
#include <math.h>
#include <ctype.h>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>


namespace ofbx
{


	struct Allocator {
		struct Page {
			struct {
				Page* next = nullptr;
				u32 offset = 0;
			} header;
			u8 data[4096 * 1024 - 12];
		};
		Page* first = nullptr;

		~Allocator() {
			Page* p = first;
			while (p) {
				Page* n = p->header.next;
				delete p;
				p = n;
			}
		}

		template <typename T, typename... Args> T* allocate(Args&&... args)
		{
			assert(sizeof(T) <= sizeof(first->data));
			if (!first) {
				first = new Page;
			}
			Page* p = first;
			if (p->header.offset % alignof(T) != 0) {
				p->header.offset += alignof(T) - p->header.offset % alignof(T);
			}

			if (p->header.offset + sizeof(T) > sizeof(p->data)) {
				p = new Page;
				p->header.next = first;
				first = p;
			}
			T* res = new (p->data + p->header.offset) T(args...);
			p->header.offset += sizeof(T);
			return res;
		}

		// store temporary data, can be reused
		std::vector<float> tmp;
		std::vector<int> int_tmp;
		std::vector<Vec3> vec3_tmp;
		std::vector<double> double_tmp;
		std::vector<Vec3> vec3_tmp2;
	};


	struct Temporaries {
		std::vector<float> f;
		std::vector<int> i;
		std::vector<Vec2> v2;
		std::vector<Vec3> v3;
		std::vector<Vec4> v4;
	};


	struct Video
	{
		DataView filename;
		DataView content;
		DataView media;
	};


	struct Error
	{
		Error() {}
		Error(const char* msg) { s_message = msg; }

		static const char* s_message;
	};


	const char* Error::s_message = "";


	template <typename T> struct OptionalError
	{
		OptionalError(Error error)
			: is_error(true)
		{
		}


		OptionalError(T _value)
			: value(_value)
			, is_error(false)
		{
		}


		T getValue() const
		{
#ifdef _DEBUG
			assert(error_checked);
#endif
			return value;
		}


		bool isError()
		{
#ifdef _DEBUG
			error_checked = true;
#endif
			return is_error;
		}


	private:
		T value;
		bool is_error;
#ifdef _DEBUG
		bool error_checked = false;
#endif
	};


#pragma pack(1)
	struct Header
	{
		u8 magic[21];
		u8 reserved[2];
		u32 version;
	};
#pragma pack()


	struct Cursor
	{
		const u8* current;
		const u8* begin;
		const u8* end;
	};


	static void setTranslation(const Vec3& t, Matrix* mtx)
	{
		mtx->m[12] = t.x;
		mtx->m[13] = t.y;
		mtx->m[14] = t.z;
	}


	static Vec3 operator-(const Vec3& v)
	{
		return { -v.x, -v.y, -v.z };
	}


	static Matrix operator*(const Matrix& lhs, const Matrix& rhs)
	{
		Matrix res;
		for (int j = 0; j < 4; ++j)
		{
			for (int i = 0; i < 4; ++i)
			{
				double tmp = 0;
				for (int k = 0; k < 4; ++k)
				{
					tmp += lhs.m[i + k * 4] * rhs.m[k + j * 4];
				}
				res.m[i + j * 4] = tmp;
			}
		}
		return res;
	}


	static Matrix makeIdentity()
	{
		return { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}


	static Matrix rotationX(double angle)
	{
		Matrix m = makeIdentity();
		double c = cos(angle);
		double s = sin(angle);

		m.m[5] = m.m[10] = c;
		m.m[9] = -s;
		m.m[6] = s;

		return m;
	}


	static Matrix rotationY(double angle)
	{
		Matrix m = makeIdentity();
		double c = cos(angle);
		double s = sin(angle);

		m.m[0] = m.m[10] = c;
		m.m[8] = s;
		m.m[2] = -s;

		return m;
	}


	static Matrix rotationZ(double angle)
	{
		Matrix m = makeIdentity();
		double c = cos(angle);
		double s = sin(angle);

		m.m[0] = m.m[5] = c;
		m.m[4] = -s;
		m.m[1] = s;

		return m;
	}


	static Matrix getRotationMatrix(const Vec3& euler, RotationOrder order)
	{
		const double TO_RAD = 3.1415926535897932384626433832795028 / 180.0;
		Matrix rx = rotationX(euler.x * TO_RAD);
		Matrix ry = rotationY(euler.y * TO_RAD);
		Matrix rz = rotationZ(euler.z * TO_RAD);
		switch (order)
		{
		default:
		case RotationOrder::EULER_XYZ: return rz * ry * rx;
		case RotationOrder::EULER_XZY: return ry * rz * rx;
		case RotationOrder::EULER_YXZ: return rz * rx * ry;
		case RotationOrder::EULER_YZX: return rx * rz * ry;
		case RotationOrder::EULER_ZXY: return ry * rx * rz;
		case RotationOrder::EULER_ZYX: return rx * ry * rz;
		case RotationOrder::SPHERIC_XYZ: assert(false); Error::s_message = "Unsupported rotation order."; return rx * ry * rz;
		}
	}


	double fbxTimeToSeconds(i64 value)
	{
		return double(value) / 46186158000L;
	}


	i64 secondsToFbxTime(double value)
	{
		return i64(value * 46186158000L);
	}


	static Vec3 operator*(const Vec3& v, float f)
	{
		return { v.x * f, v.y * f, v.z * f };
	}


	static Vec3 operator+(const Vec3& a, const Vec3& b)
	{
		return { a.x + b.x, a.y + b.y, a.z + b.z };
	}


	template <int SIZE> static bool copyString(char(&destination)[SIZE], const char* source)
	{
		const char* src = source;
		char* dest = destination;
		int length = SIZE;
		if (!src) return false;

		while (*src && length > 1)
		{
			*dest = *src;
			--length;
			++dest;
			++src;
		}
		*dest = 0;
		return *src == '\0';
	}


	u64 DataView::toU64() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(u64));
			u64 result;
			memcpy(&result, begin, sizeof(u64));
			return result;
		}
		static_assert(sizeof(unsigned long long) >= sizeof(u64), "can't use strtoull");
		return strtoull((const char*)begin, nullptr, 10);
	}


	i64 DataView::toI64() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(i64));
			i64 result;
			memcpy(&result, begin, sizeof(i64));
			return result;
		}
		static_assert(sizeof(long long) >= sizeof(i64), "can't use atoll");
		return atoll((const char*)begin);
	}


	int DataView::toInt() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(int));
			int result;
			memcpy(&result, begin, sizeof(int));
			return result;
		}
		return atoi((const char*)begin);
	}


	u32 DataView::toU32() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(u32));
			u32 result;
			memcpy(&result, begin, sizeof(u32));
			return result;
		}
		return (u32)atoll((const char*)begin);
	}


	double DataView::toDouble() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(double));
			double result;
			memcpy(&result, begin, sizeof(double));
			return result;
		}
		return atof((const char*)begin);
	}


	float DataView::toFloat() const
	{
		if (is_binary)
		{
			assert(end - begin == sizeof(float));
			float result;
			memcpy(&result, begin, sizeof(float));
			return result;
		}
		return (float)atof((const char*)begin);
	}


	bool DataView::operator==(const char* rhs) const
	{
		const char* c = rhs;
		const char* c2 = (const char*)begin;
		while (*c && c2 != (const char*)end)
		{
			if (*c != *c2) return false;
			++c;
			++c2;
		}
		return c2 == (const char*)end && *c == '\0';
	}


	struct Property;
	template <typename T> static bool parseArrayRaw(const Property& property, T* out, int max_size);
	template <typename T> static bool parseBinaryArray(const Property& property, std::vector<T>* out);
	static bool parseDouble(Property& property, double* out);


	struct Property : IElementProperty
	{
		Type getType() const override { return (Type)type; }
		IElementProperty* getNext() const override { return next; }
		DataView getValue() const override { return value; }
		int getCount() const override
		{
			assert(type == ARRAY_DOUBLE || type == ARRAY_INT || type == ARRAY_FLOAT || type == ARRAY_LONG);
			if (value.is_binary)
			{
				int i;
				memcpy(&i, value.begin, sizeof(i));
				return i;
			}
			return count;
		}

		bool getValues(double* values, int max_size) const override { return parseArrayRaw(*this, values, max_size); }

		bool getValues(float* values, int max_size) const override { return parseArrayRaw(*this, values, max_size); }

		bool getValues(u64* values, int max_size) const override { return parseArrayRaw(*this, values, max_size); }

		bool getValues(i64* values, int max_size) const override { return parseArrayRaw(*this, values, max_size); }

		bool getValues(int* values, int max_size) const override { return parseArrayRaw(*this, values, max_size); }

		int count = 0;
		u8 type = INTEGER;
		DataView value;
		Property* next = nullptr;
	};


	struct Element : IElement
	{
		IElement* getFirstChild() const override { return child; }
		IElement* getSibling() const override { return sibling; }
		DataView getID() const override { return id; }
		IElementProperty* getFirstProperty() const override { return first_property; }
		IElementProperty* getProperty(int idx) const
		{
			IElementProperty* prop = first_property;
			for (int i = 0; i < idx; ++i)
			{
				if (prop == nullptr) return nullptr;
				prop = prop->getNext();
			}
			return prop;
		}

		DataView id;
		Element* child = nullptr;
		Element* sibling = nullptr;
		Property* first_property = nullptr;
	};


	static const Element* findChild(const Element& element, const char* id)
	{
		Element* const* iter = &element.child;
		while (*iter)
		{
			if ((*iter)->id == id) return *iter;
			iter = &(*iter)->sibling;
		}
		return nullptr;
	}


	static IElement* resolveProperty(const Object& obj, const char* name)
	{
		const Element* props = findChild((const Element&)obj.element, "Properties70");
		if (!props) return nullptr;

		Element* prop = props->child;
		while (prop)
		{
			if (prop->first_property && prop->first_property->value == name)
			{
				return prop;
			}
			prop = prop->sibling;
		}
		return nullptr;
	}


	static int resolveEnumProperty(const Object& object, const char* name, int default_value)
	{
		Element* element = (Element*)resolveProperty(object, name);
		if (!element) return default_value;
		Property* x = (Property*)element->getProperty(4);
		if (!x) return default_value;

		return x->value.toInt();
	}


	static Vec3 resolveVec3Property(const Object& object, const char* name, const Vec3& default_value)
	{
		Element* element = (Element*)resolveProperty(object, name);
		if (!element) return default_value;
		Property* x = (Property*)element->getProperty(4);
		if (!x || !x->next || !x->next->next) return default_value;

		return { x->value.toDouble(), x->next->value.toDouble(), x->next->next->value.toDouble() };
	}


	Object::Object(const Scene& _scene, const IElement& _element)
		: scene(_scene)
		, element(_element)
		, is_node(false)
		, node_attribute(nullptr)
	{
		auto& e = (Element&)_element;
		if (e.first_property && e.first_property->next)
		{
			e.first_property->next->value.toString(name);
		}
		else
		{
			name[0] = '\0';
		}
	}


	static bool decompress(const u8* in, size_t in_size, u8* out, size_t out_size)
	{
		mz_stream stream = {};
		mz_inflateInit(&stream);

		stream.avail_in = (int)in_size;
		stream.next_in = in;
		stream.avail_out = (int)out_size;
		stream.next_out = out;

		int status = mz_inflate(&stream, Z_SYNC_FLUSH);

		if (status != Z_STREAM_END) return false;

		return mz_inflateEnd(&stream) == Z_OK;
	}


	template <typename T> static OptionalError<T> read(Cursor* cursor)
	{
		if (cursor->current + sizeof(T) > cursor->end) return Error("Reading past the end");
		T value = *(const T*)cursor->current;
		cursor->current += sizeof(T);
		return value;
	}


	static OptionalError<DataView> readShortString(Cursor* cursor)
	{
		DataView value;
		OptionalError<u8> length = read<u8>(cursor);
		if (length.isError()) return Error();

		if (cursor->current + length.getValue() > cursor->end) return Error("Reading past the end");
		value.begin = cursor->current;
		cursor->current += length.getValue();

		value.end = cursor->current;

		return value;
	}


	static OptionalError<DataView> readLongString(Cursor* cursor)
	{
		DataView value;
		OptionalError<u32> length = read<u32>(cursor);
		if (length.isError()) return Error();

		if (cursor->current + length.getValue() > cursor->end) return Error("Reading past the end");
		value.begin = cursor->current;
		cursor->current += length.getValue();

		value.end = cursor->current;

		return value;
	}


	static OptionalError<Property*> readProperty(Cursor* cursor, Allocator& allocator)
	{
		if (cursor->current == cursor->end) return Error("Reading past the end");

		Property* prop = allocator.allocate<Property>();
		prop->next = nullptr;
		prop->type = *cursor->current;
		++cursor->current;
		prop->value.begin = cursor->current;

		switch (prop->type)
		{
		case 'S':
		{
			OptionalError<DataView> val = readLongString(cursor);
			if (val.isError()) return Error();
			prop->value = val.getValue();
			break;
		}
		case 'Y': cursor->current += 2; break;
		case 'C': cursor->current += 1; break;
		case 'I': cursor->current += 4; break;
		case 'F': cursor->current += 4; break;
		case 'D': cursor->current += 8; break;
		case 'L': cursor->current += 8; break;
		case 'R':
		{
			OptionalError<u32> len = read<u32>(cursor);
			if (len.isError()) return Error();
			if (cursor->current + len.getValue() > cursor->end) return Error("Reading past the end");
			cursor->current += len.getValue();
			break;
		}
		case 'b':
		case 'f':
		case 'd':
		case 'l':
		case 'i':
		{
			OptionalError<u32> length = read<u32>(cursor);
			OptionalError<u32> encoding = read<u32>(cursor);
			OptionalError<u32> comp_len = read<u32>(cursor);
			if (length.isError() || encoding.isError() || comp_len.isError()) return Error();
			if (cursor->current + comp_len.getValue() > cursor->end) return Error("Reading past the end");
			cursor->current += comp_len.getValue();
			break;
		}
		default: return Error("Unknown property type");
		}
		prop->value.end = cursor->current;
		return prop;
	}


	static OptionalError<u64> readElementOffset(Cursor* cursor, u16 version)
	{
		if (version >= 7500)
		{
			OptionalError<u64> tmp = read<u64>(cursor);
			if (tmp.isError()) return Error();
			return tmp.getValue();
		}

		OptionalError<u32> tmp = read<u32>(cursor);
		if (tmp.isError()) return Error();
		return tmp.getValue();
	}


	static OptionalError<Element*> readElement(Cursor* cursor, u32 version, Allocator& allocator)
	{
		OptionalError<u64> end_offset = readElementOffset(cursor, version);
		if (end_offset.isError()) return Error();
		if (end_offset.getValue() == 0) return nullptr;

		OptionalError<u64> prop_count = readElementOffset(cursor, version);
		OptionalError<u64> prop_length = readElementOffset(cursor, version);
		if (prop_count.isError() || prop_length.isError()) return Error();

		OptionalError<DataView> id = readShortString(cursor);
		if (id.isError()) return Error();

		Element* element = allocator.allocate<Element>();
		element->first_property = nullptr;
		element->id = id.getValue();

		element->child = nullptr;
		element->sibling = nullptr;

		Property** prop_link = &element->first_property;
		for (u32 i = 0; i < prop_count.getValue(); ++i)
		{
			OptionalError<Property*> prop = readProperty(cursor, allocator);
			if (prop.isError())
			{
				return Error();
			}

			*prop_link = prop.getValue();
			prop_link = &(*prop_link)->next;
		}

		if (cursor->current - cursor->begin >= (ptrdiff_t)end_offset.getValue()) return element;

		int BLOCK_SENTINEL_LENGTH = version >= 7500 ? 25 : 13;

		Element** link = &element->child;
		while (cursor->current - cursor->begin < ((ptrdiff_t)end_offset.getValue() - BLOCK_SENTINEL_LENGTH))
		{
			OptionalError<Element*> child = readElement(cursor, version, allocator);
			if (child.isError())
			{
				return Error();
			}

			*link = child.getValue();
			if (child.getValue() == 0) break;
			link = &(*link)->sibling;
		}

		if (cursor->current + BLOCK_SENTINEL_LENGTH > cursor->end)
		{
			return Error("Reading past the end");
		}

		cursor->current += BLOCK_SENTINEL_LENGTH;
		return element;
	}


	static bool isEndLine(const Cursor& cursor)
	{
		return *cursor.current == '\n' || *cursor.current == '\r' && cursor.current + 1 < cursor.end && *(cursor.current + 1) != '\n';
	}


	static void skipInsignificantWhitespaces(Cursor* cursor)
	{
		while (cursor->current < cursor->end && isspace(*cursor->current) && !isEndLine(*cursor))
		{
			++cursor->current;
		}
	}


	static void skipLine(Cursor* cursor)
	{
		while (cursor->current < cursor->end && !isEndLine(*cursor))
		{
			++cursor->current;
		}
		if (cursor->current < cursor->end) ++cursor->current;
		skipInsignificantWhitespaces(cursor);
	}


	static void skipWhitespaces(Cursor* cursor)
	{
		while (cursor->current < cursor->end && isspace(*cursor->current))
		{
			++cursor->current;
		}
		while (cursor->current < cursor->end && *cursor->current == ';') skipLine(cursor);
	}


	static bool isTextTokenChar(char c)
	{
		return isalnum(c) || c == '_' || c == '-';
	}


	static DataView readTextToken(Cursor* cursor)
	{
		DataView ret;
		ret.begin = cursor->current;
		while (cursor->current < cursor->end && isTextTokenChar(*cursor->current))
		{
			++cursor->current;
		}
		ret.end = cursor->current;
		return ret;
	}


	static OptionalError<Property*> readTextProperty(Cursor* cursor, Allocator& allocator)
	{
		Property* prop = allocator.allocate<Property>();
		prop->value.is_binary = false;
		prop->next = nullptr;
		if (*cursor->current == '"')
		{
			prop->type = 'S';
			++cursor->current;
			prop->value.begin = cursor->current;
			while (cursor->current < cursor->end && *cursor->current != '"')
			{
				++cursor->current;
			}
			prop->value.end = cursor->current;
			if (cursor->current < cursor->end) ++cursor->current; // skip '"'
			return prop;
		}

		if (isdigit(*cursor->current) || *cursor->current == '-')
		{
			prop->type = 'L';
			prop->value.begin = cursor->current;
			if (*cursor->current == '-') ++cursor->current;
			while (cursor->current < cursor->end && isdigit(*cursor->current))
			{
				++cursor->current;
			}
			prop->value.end = cursor->current;

			if (cursor->current < cursor->end && *cursor->current == '.')
			{
				prop->type = 'D';
				++cursor->current;
				while (cursor->current < cursor->end && isdigit(*cursor->current))
				{
					++cursor->current;
				}
				if (cursor->current < cursor->end && (*cursor->current == 'e' || *cursor->current == 'E'))
				{
					// 10.5e-013
					++cursor->current;
					if (cursor->current < cursor->end && *cursor->current == '-') ++cursor->current;
					while (cursor->current < cursor->end && isdigit(*cursor->current)) ++cursor->current;
				}


				prop->value.end = cursor->current;
			}
			return prop;
		}

		if (*cursor->current == 'T' || *cursor->current == 'Y' || *cursor->current == 'W' || *cursor->current == 'C')
		{
			// WTF is this
			prop->type = *cursor->current;
			prop->value.begin = cursor->current;
			++cursor->current;
			prop->value.end = cursor->current;
			return prop;
		}

		if (*cursor->current == '*')
		{
			prop->type = 'l';
			++cursor->current;
			// Vertices: *10740 { a: 14.2760353088379,... }
			while (cursor->current < cursor->end && *cursor->current != ':')
			{
				++cursor->current;
			}
			if (cursor->current < cursor->end) ++cursor->current; // skip ':'
			skipInsignificantWhitespaces(cursor);
			prop->value.begin = cursor->current;
			prop->count = 0;
			bool is_any = false;
			while (cursor->current < cursor->end && *cursor->current != '}')
			{
				if (*cursor->current == ',')
				{
					if (is_any) ++prop->count;
					is_any = false;
				}
				else if (!isspace(*cursor->current) && !isEndLine(*cursor))
					is_any = true;
				if (*cursor->current == '.') prop->type = 'd';
				++cursor->current;
			}
			if (is_any) ++prop->count;
			prop->value.end = cursor->current;
			if (cursor->current < cursor->end) ++cursor->current; // skip '}'
			return prop;
		}

		assert(false);
		return Error("TODO");
	}


	static OptionalError<Element*> readTextElement(Cursor* cursor, Allocator& allocator)
	{
		DataView id = readTextToken(cursor);
		if (cursor->current == cursor->end) return Error("Unexpected end of file");
		if (*cursor->current != ':') return Error("Unexpected character");
		++cursor->current;

		skipInsignificantWhitespaces(cursor);
		if (cursor->current == cursor->end) return Error("Unexpected end of file");

		Element* element = allocator.allocate<Element>();
		element->id = id;

		Property** prop_link = &element->first_property;
		while (cursor->current < cursor->end && !isEndLine(*cursor) && *cursor->current != '{')
		{
			OptionalError<Property*> prop = readTextProperty(cursor, allocator);
			if (prop.isError())
			{
				return Error();
			}
			if (cursor->current < cursor->end && *cursor->current == ',')
			{
				++cursor->current;
				skipWhitespaces(cursor);
			}
			skipInsignificantWhitespaces(cursor);

			*prop_link = prop.getValue();
			prop_link = &(*prop_link)->next;
		}

		Element** link = &element->child;
		if (*cursor->current == '{')
		{
			++cursor->current;
			skipWhitespaces(cursor);
			while (cursor->current < cursor->end && *cursor->current != '}')
			{
				OptionalError<Element*> child = readTextElement(cursor, allocator);
				if (child.isError())
				{
					return Error();
				}
				skipWhitespaces(cursor);

				*link = child.getValue();
				link = &(*link)->sibling;
			}
			if (cursor->current < cursor->end) ++cursor->current; // skip '}'
		}
		return element;
	}


	static OptionalError<Element*> tokenizeText(const u8* data, size_t size, Allocator& allocator)
	{
		Cursor cursor;
		cursor.begin = data;
		cursor.current = data;
		cursor.end = data + size;

		Element* root = allocator.allocate<Element>();
		root->first_property = nullptr;
		root->id.begin = nullptr;
		root->id.end = nullptr;
		root->child = nullptr;
		root->sibling = nullptr;

		Element** element = &root->child;
		while (cursor.current < cursor.end)
		{
			if (*cursor.current == ';' || *cursor.current == '\r' || *cursor.current == '\n')
			{
				skipLine(&cursor);
			}
			else
			{
				OptionalError<Element*> child = readTextElement(&cursor, allocator);
				if (child.isError())
				{
					return Error();
				}
				*element = child.getValue();
				if (!*element) return root;
				element = &(*element)->sibling;
			}
		}

		return root;
	}


	static OptionalError<Element*> tokenize(const u8* data, size_t size, u32& version, Allocator& allocator)
	{
		Cursor cursor;
		cursor.begin = data;
		cursor.current = data;
		cursor.end = data + size;

		const Header* header = (const Header*)cursor.current;
		cursor.current += sizeof(*header);
		version = header->version;

		Element* root = allocator.allocate<Element>();
		root->first_property = nullptr;
		root->id.begin = nullptr;
		root->id.end = nullptr;
		root->child = nullptr;
		root->sibling = nullptr;

		Element** element = &root->child;
		for (;;)
		{
			OptionalError<Element*> child = readElement(&cursor, header->version, allocator);
			if (child.isError()) {
				return Error();
			}
			*element = child.getValue();
			if (!*element) return root;
			element = &(*element)->sibling;
		}
	}


	static void parseTemplates(const Element& root)
	{
		const Element* defs = findChild(root, "Definitions");
		if (!defs) return;

		std::unordered_map<std::string, Element*> templates;
		Element* def = defs->child;
		while (def)
		{
			if (def->id == "ObjectType")
			{
				Element* subdef = def->child;
				while (subdef)
				{
					if (subdef->id == "PropertyTemplate")
					{
						DataView prop1 = def->first_property->value;
						DataView prop2 = subdef->first_property->value;
						std::string key((const char*)prop1.begin, prop1.end - prop1.begin);
						key += std::string((const char*)prop1.begin, prop1.end - prop1.begin);
						templates[key] = subdef;
					}
					subdef = subdef->sibling;
				}
			}
			def = def->sibling;
		}
		// TODO
	}


	struct Scene;


	Mesh::Mesh(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct MeshImpl : Mesh
	{
		MeshImpl(const Scene& _scene, const IElement& _element)
			: Mesh(_scene, _element)
		{
			is_node = true;
		}


		Matrix getGeometricMatrix() const override
		{
			Vec3 translation = resolveVec3Property(*this, "GeometricTranslation", { 0, 0, 0 });
			Vec3 rotation = resolveVec3Property(*this, "GeometricRotation", { 0, 0, 0 });
			Vec3 scale = resolveVec3Property(*this, "GeometricScaling", { 1, 1, 1 });

			Matrix scale_mtx = makeIdentity();
			scale_mtx.m[0] = (float)scale.x;
			scale_mtx.m[5] = (float)scale.y;
			scale_mtx.m[10] = (float)scale.z;
			Matrix mtx = getRotationMatrix(rotation, RotationOrder::EULER_XYZ);
			setTranslation(translation, &mtx);

			return scale_mtx * mtx;
		}


		Type getType() const override { return Type::MESH; }


		const Pose* getPose() const override { return pose; }
		const Geometry* getGeometry() const override { return geometry; }
		const Material* getMaterial(int index) const override { return materials[index]; }
		int getMaterialCount() const override { return (int)materials.size(); }


		const Pose* pose = nullptr;
		const Geometry* geometry = nullptr;
		std::vector<const Material*> materials;
	};


	Material::Material(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct MaterialImpl : Material
	{
		MaterialImpl(const Scene& _scene, const IElement& _element)
			: Material(_scene, _element)
		{
			for (const Texture*& tex : textures) tex = nullptr;
		}

		Type getType() const override { return Type::MATERIAL; }

		const Texture* getTexture(Texture::TextureType type) const override { return textures[type]; }
		Color getDiffuseColor() const override { return diffuse_color; }
		Color getSpecularColor() const override { return specular_color; }
		Color getReflectionColor() const override { return reflection_color; };
		Color getAmbientColor() const override { return ambient_color; };
		Color getEmissiveColor() const override { return emissive_color; };

		double getDiffuseFactor() const override { return diffuse_factor; };
		double getSpecularFactor() const override { return specular_factor; };
		double getReflectionFactor() const override { return reflection_factor; };
		double getShininess() const override { return shininess; };
		double getShininessExponent() const override { return shininess_exponent; };
		double getAmbientFactor() const override { return ambient_factor; };
		double getBumpFactor() const override { return bump_factor; };
		double getEmissiveFactor() const override { return emissive_factor; };

		const Texture* textures[Texture::TextureType::COUNT];
		Color diffuse_color;
		Color specular_color;
		Color reflection_color;
		Color ambient_color;
		Color emissive_color;

		double diffuse_factor;
		double specular_factor;
		double reflection_factor;
		double shininess;
		double shininess_exponent;
		double ambient_factor;
		double bump_factor;
		double emissive_factor;
	};


	struct LimbNodeImpl : Object
	{
		LimbNodeImpl(const Scene& _scene, const IElement& _element)
			: Object(_scene, _element)
		{
			is_node = true;
		}
		Type getType() const override { return Type::LIMB_NODE; }
	};


	struct NullImpl : Object
	{
		NullImpl(const Scene& _scene, const IElement& _element)
			: Object(_scene, _element)
		{
			is_node = true;
		}
		Type getType() const override { return Type::NULL_NODE; }
	};


	NodeAttribute::NodeAttribute(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct NodeAttributeImpl : NodeAttribute
	{
		NodeAttributeImpl(const Scene& _scene, const IElement& _element)
			: NodeAttribute(_scene, _element)
		{
		}
		Type getType() const override { return Type::NODE_ATTRIBUTE; }
		DataView getAttributeType() const override { return attribute_type; }


		DataView attribute_type;
	};


	Geometry::Geometry(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct GeometryImpl : Geometry
	{
		enum VertexDataMapping
		{
			BY_POLYGON_VERTEX,
			BY_POLYGON,
			BY_VERTEX
		};

		struct NewVertex
		{
			~NewVertex() { delete next; }

			int index = -1;
			NewVertex* next = nullptr;
		};

		std::vector<Vec3> vertices;
		std::vector<Vec3> normals;
		std::vector<Vec2> uvs[s_uvs_max];
		std::vector<Vec4> colors;
		std::vector<Vec3> tangents;
		std::vector<int> materials;

		const Skin* skin = nullptr;
		const BlendShape* blendShape = nullptr;

		std::vector<int> indices;
		std::vector<int> to_old_vertices;
		std::vector<NewVertex> to_new_vertices;

		GeometryImpl(const Scene& _scene, const IElement& _element)
			: Geometry(_scene, _element)
		{
		}


		Type getType() const override { return Type::GEOMETRY; }
		int getVertexCount() const override { return (int)vertices.size(); }
		const int* getFaceIndices() const override { return indices.empty() ? nullptr : &indices[0]; }
        const int* getTo_old_vertices() const override { return to_old_vertices.empty() ? nullptr : &to_old_vertices[0]; }
		int getIndexCount() const override { return (int)indices.size(); }
		const Vec3* getVertices() const override { return &vertices[0]; }
		const Vec3* getNormals() const override { return normals.empty() ? nullptr : &normals[0]; }
		const Vec2* getUVs(int index = 0) const override { return index < 0 || index >= s_uvs_max || uvs[index].empty() ? nullptr : &uvs[index][0]; }
		const Vec4* getColors() const override { return colors.empty() ? nullptr : &colors[0]; }
		const Vec3* getTangents() const override { return tangents.empty() ? nullptr : &tangents[0]; }
		const Skin* getSkin() const override { return skin; }
		const BlendShape* getBlendShape() const override { return blendShape; }
		const int* getMaterials() const override { return materials.empty() ? nullptr : &materials[0]; }
        
	};


	Shape::Shape(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct ShapeImpl : Shape
	{
		std::vector<Vec3> vertices;
		std::vector<Vec3> normals;

		ShapeImpl(const Scene& _scene, const IElement& _element)
			: Shape(_scene, _element)
		{
		}


		bool postprocess(GeometryImpl* geom, Allocator& allocator);


		Type getType() const override { return Type::SHAPE; }
		int getVertexCount() const override { return (int)vertices.size(); }
		const Vec3* getVertices() const override { return &vertices[0]; }
		const Vec3* getNormals() const override { return normals.empty() ? nullptr : &normals[0]; }
	};


	Cluster::Cluster(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct ClusterImpl : Cluster
	{
		ClusterImpl(const Scene& _scene, const IElement& _element)
			: Cluster(_scene, _element)
		{
		}

		const int* getIndices() const override { return &indices[0]; }
		int getIndicesCount() const override { return (int)indices.size(); }
		const double* getWeights() const override { return &weights[0]; }
		int getWeightsCount() const override { return (int)weights.size(); }
		Matrix getTransformMatrix() const override { return transform_matrix; }
		Matrix getTransformLinkMatrix() const override { return transform_link_matrix; }
		Object* getLink() const override { return link; }


		bool postprocess(Allocator& allocator)
		{
			assert(skin);

			GeometryImpl* geom = (GeometryImpl*)skin->resolveObjectLinkReverse(Object::Type::GEOMETRY);
			if (!geom) return false;

			allocator.int_tmp.clear(); // old indices
			const Element* indexes = findChild((const Element&)element, "Indexes");
			if (indexes && indexes->first_property)
			{
				if (!parseBinaryArray(*indexes->first_property, &allocator.int_tmp)) return false;
			}

			allocator.double_tmp.clear(); // old weights
			const Element* weights_el = findChild((const Element&)element, "Weights");
			if (weights_el && weights_el->first_property)
			{
				if (!parseBinaryArray(*weights_el->first_property, &allocator.double_tmp)) return false;
			}

			if (allocator.int_tmp.size() != allocator.double_tmp.size()) return false;

			indices.reserve(allocator.int_tmp.size());
			weights.reserve(allocator.int_tmp.size());
			int* ir = allocator.int_tmp.empty() ? nullptr : &allocator.int_tmp[0];
			double* wr = allocator.double_tmp.empty() ? nullptr : &allocator.double_tmp[0];
			for (int i = 0, c = (int)allocator.int_tmp.size(); i < c; ++i)
			{
				int old_idx = ir[i];
				double w = wr[i];
				GeometryImpl::NewVertex* n = &geom->to_new_vertices[old_idx];
				if (n->index == -1) continue; // skip vertices which aren't indexed.
				while (n)
				{
					indices.push_back(n->index);
					weights.push_back(w);
					n = n->next;
				}
			}

			return true;
		}


		Object* link = nullptr;
		Skin* skin = nullptr;
		std::vector<int> indices;
		std::vector<double> weights;
		Matrix transform_matrix;
		Matrix transform_link_matrix;
		Type getType() const override { return Type::CLUSTER; }
	};


	AnimationStack::AnimationStack(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	AnimationLayer::AnimationLayer(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	AnimationCurve::AnimationCurve(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	AnimationCurveNode::AnimationCurveNode(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct AnimationStackImpl : AnimationStack
	{
		AnimationStackImpl(const Scene& _scene, const IElement& _element)
			: AnimationStack(_scene, _element)
		{
		}


		const AnimationLayer* getLayer(int index) const override
		{
			return resolveObjectLink<AnimationLayer>(index);
		}


		Type getType() const override { return Type::ANIMATION_STACK; }
	};


	struct AnimationCurveImpl : AnimationCurve
	{
		AnimationCurveImpl(const Scene& _scene, const IElement& _element)
			: AnimationCurve(_scene, _element)
		{
		}

		int getKeyCount() const override { return (int)times.size(); }
		const i64* getKeyTime() const override { return &times[0]; }
		const float* getKeyValue() const override { return &values[0]; }

		std::vector<i64> times;
		std::vector<float> values;
		Type getType() const override { return Type::ANIMATION_CURVE; }
	};


	Skin::Skin(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct SkinImpl : Skin
	{
		SkinImpl(const Scene& _scene, const IElement& _element)
			: Skin(_scene, _element)
		{
		}

		int getClusterCount() const override { return (int)clusters.size(); }
		const Cluster* getCluster(int idx) const override { return clusters[idx]; }

		Type getType() const override { return Type::SKIN; }

		std::vector<Cluster*> clusters;
	};


	BlendShapeChannel::BlendShapeChannel(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct BlendShapeChannelImpl : BlendShapeChannel
	{
		BlendShapeChannelImpl(const Scene& _scene, const IElement& _element)
			: BlendShapeChannel(_scene, _element)
		{
		}

		double getDeformPercent() const override { return deformPercent; }
		int getShapeCount() const override { return (int)shapes.size(); }
		const Shape* getShape(int idx) const override { return shapes[idx]; }

		Type getType() const override { return Type::BLEND_SHAPE_CHANNEL; }

		bool postprocess(Allocator& allocator)
		{
			assert(blendShape);

			GeometryImpl* geom = (GeometryImpl*)blendShape->resolveObjectLinkReverse(Object::Type::GEOMETRY);
			if (!geom) return false;

			const Element* deform_percent_el = findChild((const Element&)element, "DeformPercent");
			if (deform_percent_el && deform_percent_el->first_property)
			{
				if (!parseDouble(*deform_percent_el->first_property, &deformPercent)) return false;
			}

			const Element* full_weights_el = findChild((const Element&)element, "FullWeights");
			if (full_weights_el && full_weights_el->first_property)
			{
				if (!parseBinaryArray(*full_weights_el->first_property, &fullWeights)) return false;
			}

			for (int i = 0; i < (int)shapes.size(); i++)
			{
				auto shape = (ShapeImpl*)shapes[i];
				if (!shape->postprocess(geom, allocator)) return false;
			}

			return true;
		}


		BlendShape* blendShape = nullptr;
		double deformPercent = 0;
		std::vector<double> fullWeights;
		std::vector<Shape*> shapes;
	};


	BlendShape::BlendShape(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct BlendShapeImpl : BlendShape
	{
		BlendShapeImpl(const Scene& _scene, const IElement& _element)
			: BlendShape(_scene, _element)
		{
		}

		int getBlendShapeChannelCount() const override { return (int)blendShapeChannels.size(); }
		const BlendShapeChannel* getBlendShapeChannel(int idx) const override { return blendShapeChannels[idx]; }

		Type getType() const override { return Type::BLEND_SHAPE; }

		std::vector<BlendShapeChannel*> blendShapeChannels;
	};


	Texture::Texture(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	Pose::Pose(const Scene& _scene, const IElement& _element)
		: Object(_scene, _element)
	{
	}


	struct PoseImpl : Pose
	{
		PoseImpl(const Scene& _scene, const IElement& _element)
			: Pose(_scene, _element)
		{
		}

		bool postprocess(Scene* scene);


		Matrix getMatrix() const override { return matrix; }
		const Object* getNode() const override { return node; }

		Type getType() const override { return Type::POSE; }

		Matrix matrix;
		Object* node = nullptr;
		DataView node_id;
	};


	struct TextureImpl : Texture
	{
		TextureImpl(const Scene& _scene, const IElement& _element)
			: Texture(_scene, _element)
		{
		}

		DataView getRelativeFileName() const override { return relative_filename; }
		DataView getFileName() const override { return filename; }
		DataView getEmbeddedData() const override;

		DataView media;
		DataView filename;
		DataView relative_filename;
		Type getType() const override { return Type::TEXTURE; }
	};


	struct Root : Object
	{
		Root(const Scene& _scene, const IElement& _element)
			: Object(_scene, _element)
		{
			copyString(name, "RootNode");
			is_node = true;
		}
		Type getType() const override { return Type::ROOT; }
	};


	struct Scene : IScene
	{
		struct Connection
		{
			enum Type
			{
				OBJECT_OBJECT,
				OBJECT_PROPERTY
			};

			Type type = OBJECT_OBJECT;
			u64 from = 0;
			u64 to = 0;
			DataView property;
		};

		struct ObjectPair
		{
			const Element* element;
			Object* object;
		};


		int getAnimationStackCount() const override { return (int)m_animation_stacks.size(); }
		int getGeometryCount() const override { return (int)m_geometries.size(); }
		int getMeshCount() const override { return (int)m_meshes.size(); }
		float getSceneFrameRate() const override { return m_scene_frame_rate; }
		const GlobalSettings* getGlobalSettings() const override { return &m_settings; }

		const Object* const* getAllObjects() const override { return m_all_objects.empty() ? nullptr : &m_all_objects[0]; }


		int getAllObjectCount() const override { return (int)m_all_objects.size(); }

		int getEmbeddedDataCount() const override {
			return (int)m_videos.size();
		}

		DataView getEmbeddedData(int index) const override {
			return m_videos[index].content;
		}

		DataView getEmbeddedFilename(int index) const override {
			return m_videos[index].filename;
		}

		const AnimationStack* getAnimationStack(int index) const override
		{
			assert(index >= 0);
			assert(index < m_animation_stacks.size());
			return m_animation_stacks[index];
		}


		const Mesh* getMesh(int index) const override
		{
			assert(index >= 0);
			assert(index < m_meshes.size());
			return m_meshes[index];
		}


		const Geometry* getGeometry(int index) const override
		{
			assert(index >= 0);
			assert(index < m_geometries.size());
			return m_geometries[index];
		}


		const TakeInfo* getTakeInfo(const char* name) const override
		{
			for (const TakeInfo& info : m_take_infos)
			{
				if (info.name == name) return &info;
			}
			return nullptr;
		}


		const IElement* getRootElement() const override { return m_root_element; }
		const Object* getRoot() const override { return m_root; }


		void destroy() override { delete this; }


		~Scene() override {
			for (auto ptr : m_all_objects)
				ptr->~Object();
		}


		Element* m_root_element = nullptr;
		Root* m_root = nullptr;
		float m_scene_frame_rate = -1;
		GlobalSettings m_settings;
		std::unordered_map<u64, ObjectPair> m_object_map;
		std::vector<Object*> m_all_objects;
		std::vector<Mesh*> m_meshes;
		std::vector<Geometry*> m_geometries;
		std::vector<AnimationStack*> m_animation_stacks;
		std::vector<Connection> m_connections;
		std::vector<u8> m_data;
		std::vector<TakeInfo> m_take_infos;
		std::vector<Video> m_videos;
		Allocator m_allocator;
	};


	DataView TextureImpl::getEmbeddedData() const {
		if (!media.begin) return media;
		for (const Video& v : scene.m_videos) {
			if (v.media.end - v.media.begin != media.end - media.begin) continue;
			const size_t len = v.media.end - v.media.begin;
			if (memcmp(v.media.begin, media.begin, len) != 0) continue;

			return v.content;
		}
		return {};
	}


	bool PoseImpl::postprocess(Scene* scene)
	{
		node = scene->m_object_map[node_id.toU64()].object;
		if (node && node->getType() == Object::Type::MESH) {
			static_cast<MeshImpl*>(node)->pose = this;
		}
		return true;
	}


	struct AnimationCurveNodeImpl : AnimationCurveNode
	{
		AnimationCurveNodeImpl(const Scene& _scene, const IElement& _element)
			: AnimationCurveNode(_scene, _element)
		{
			default_values[0] = default_values[1] = default_values[2] = 0;
			Element* dx = static_cast<Element*>(resolveProperty(*this, "d|X"));
			Element* dy = static_cast<Element*>(resolveProperty(*this, "d|Y"));
			Element* dz = static_cast<Element*>(resolveProperty(*this, "d|Z"));

			if (dx) {
				Property* x = (Property*)dx->getProperty(4);
				if (x) default_values[0] = (float)x->value.toDouble();
			}
			if (dy) {
				Property* y = (Property*)dy->getProperty(4);
				if (y) default_values[1] = (float)y->value.toDouble();
			}
			if (dz) {
				Property* z = (Property*)dz->getProperty(4);
				if (z) default_values[2] = (float)z->value.toDouble();
			}
		}


		const Object* getBone() const override
		{
			return bone;
		}


		const AnimationCurve* getCurve(int idx) const override {
			assert(idx >= 0 && idx < 3);
			return curves[idx].curve;
		}


		Vec3 getNodeLocalTransform(double time) const override
		{
			i64 fbx_time = secondsToFbxTime(time);

			auto getCoord = [&](const Curve& curve, i64 fbx_time, int idx) {
				if (!curve.curve) return default_values[idx];

				const i64* times = curve.curve->getKeyTime();
				const float* values = curve.curve->getKeyValue();
				int count = curve.curve->getKeyCount();

				if (fbx_time < times[0]) fbx_time = times[0];
				if (fbx_time > times[count - 1]) fbx_time = times[count - 1];
				for (int i = 1; i < count; ++i)
				{
					if (times[i] >= fbx_time)
					{
						float t = float(double(fbx_time - times[i - 1]) / double(times[i] - times[i - 1]));
						return values[i - 1] * (1 - t) + values[i] * t;
					}
				}
				return values[0];
			};

			return { getCoord(curves[0], fbx_time, 0), getCoord(curves[1], fbx_time, 1), getCoord(curves[2], fbx_time, 2) };
		}


		struct Curve
		{
			const AnimationCurve* curve = nullptr;
			const Scene::Connection* connection = nullptr;
		};


		Curve curves[3];
		Object* bone = nullptr;
		DataView bone_link_property;
		Type getType() const override { return Type::ANIMATION_CURVE_NODE; }
		float default_values[3];
		enum Mode
		{
			TRANSLATION,
			ROTATION,
			SCALE
		} mode = TRANSLATION;
	};


	struct AnimationLayerImpl : AnimationLayer
	{
		AnimationLayerImpl(const Scene& _scene, const IElement& _element)
			: AnimationLayer(_scene, _element)
		{
		}


		Type getType() const override { return Type::ANIMATION_LAYER; }


		const AnimationCurveNode* getCurveNode(int index) const override
		{
			if (index >= (int)curve_nodes.size() || index < 0) return nullptr;
			return curve_nodes[index];
		}


		const AnimationCurveNode* getCurveNode(const Object& bone, const char* prop) const override
		{
			for (const AnimationCurveNodeImpl* node : curve_nodes)
			{
				if (node->bone_link_property == prop && node->bone == &bone) return node;
			}
			return nullptr;
		}


		std::vector<AnimationCurveNodeImpl*> curve_nodes;
	};

	void parseVideo(Scene& scene, const Element& element, Allocator& allocator)
	{
		if (!element.first_property) return;
		if (!element.first_property->next) return;
		if (element.first_property->next->getType() != IElementProperty::STRING) return;

		const Element* content_element = findChild(element, "Content");

		if (!content_element) return;
		if (!content_element->first_property) return;
		if (content_element->first_property->getType() != IElementProperty::BINARY) return;

		const Element* filename_element = findChild(element, "Filename");
		if (!filename_element) return;
		if (!filename_element->first_property) return;
		if (filename_element->first_property->getType() != IElementProperty::STRING) return;

		Video video;
		video.content = content_element->first_property->value;
		video.filename = filename_element->first_property->value;
		video.media = element.first_property->next->value;
		scene.m_videos.push_back(video);
	}

	struct OptionalError<Object*> parseTexture(const Scene& scene, const Element& element, Allocator& allocator)
	{
		TextureImpl* texture = allocator.allocate<TextureImpl>(scene, element);
		const Element* texture_filename = findChild(element, "FileName");
		if (texture_filename && texture_filename->first_property)
		{
			texture->filename = texture_filename->first_property->value;
		}

		const Element* media = findChild(element, "Media");
		if (media && media->first_property)
		{
			texture->media = media->first_property->value;
		}

		const Element* texture_relative_filename = findChild(element, "RelativeFilename");
		if (texture_relative_filename && texture_relative_filename->first_property)
		{
			texture->relative_filename = texture_relative_filename->first_property->value;
		}
		return texture;
	}


	struct OptionalError<Object*> parsePose(const Scene& scene, const Element& element, Allocator& allocator)
	{
		PoseImpl* pose = allocator.allocate<PoseImpl>(scene, element);
		const Element* pose_node = findChild(element, "PoseNode");
		if (pose_node) {
			const Element* node = findChild(*pose_node, "Node");
			const Element* matrix = findChild(*pose_node, "Matrix");

			if (matrix->first_property) {
				parseArrayRaw(*matrix->first_property, &pose->matrix, sizeof(pose->matrix));
			}
			pose->node_id = node->first_property->value;
		}
		return pose;
	}


	template <typename T>
	static OptionalError<Object*> parse(const Scene& scene, const Element& element, Allocator& allocator)
	{
		T* obj = allocator.allocate<T>(scene, element);
		return obj;
	}


	static OptionalError<Object*> parseCluster(const Scene& scene, const Element& element, Allocator& allocator)
	{
		ClusterImpl* obj = allocator.allocate<ClusterImpl>(scene, element);

		const Element* transform_link = findChild(element, "TransformLink");
		if (transform_link && transform_link->first_property)
		{
			if (!parseArrayRaw(
				*transform_link->first_property, &obj->transform_link_matrix, sizeof(obj->transform_link_matrix)))
			{
				return Error("Failed to parse TransformLink");
			}
		}
		const Element* transform = findChild(element, "Transform");
		if (transform && transform->first_property)
		{
			if (!parseArrayRaw(*transform->first_property, &obj->transform_matrix, sizeof(obj->transform_matrix)))
			{
				return Error("Failed to parse Transform");
			}
		}

		return obj;
	}


	static OptionalError<Object*> parseNodeAttribute(const Scene& scene, const Element& element, Allocator& allocator)
	{
		NodeAttributeImpl* obj = allocator.allocate<NodeAttributeImpl>(scene, element);
		const Element* type_flags = findChild(element, "TypeFlags");
		if (type_flags && type_flags->first_property)
		{
			obj->attribute_type = type_flags->first_property->value;
		}
		return obj;
	}


	static OptionalError<Object*> parseLimbNode(const Scene& scene, const Element& element, Allocator& allocator)
	{
		if (!element.first_property
			|| !element.first_property->next
			|| !element.first_property->next->next
			|| element.first_property->next->next->value != "LimbNode")
		{
			return Error("Invalid limb node");
		}

		LimbNodeImpl* obj = allocator.allocate<LimbNodeImpl>(scene, element);
		return obj;
	}


	static OptionalError<Object*> parseMesh(const Scene& scene, const Element& element, Allocator& allocator)
	{
		if (!element.first_property
			|| !element.first_property->next
			|| !element.first_property->next->next
			|| element.first_property->next->next->value != "Mesh")
		{
			return Error("Invalid mesh");
		}

		return allocator.allocate<MeshImpl>(scene, element);
	}


	static OptionalError<Object*> parseMaterial(const Scene& scene, const Element& element, Allocator& allocator)
	{
		MaterialImpl* material = allocator.allocate<MaterialImpl>(scene, element);
		const Element* prop = findChild(element, "Properties70");
		material->diffuse_color = { 1, 1, 1 };
		if (prop) prop = prop->child;
		while (prop)
		{
			if (prop->id == "P" && prop->first_property)
			{
				if (prop->first_property->value == "DiffuseColor")
				{
					material->diffuse_color.r = (float)prop->getProperty(4)->getValue().toDouble();
					material->diffuse_color.g = (float)prop->getProperty(5)->getValue().toDouble();
					material->diffuse_color.b = (float)prop->getProperty(6)->getValue().toDouble();
				}
				else if (prop->first_property->value == "SpecularColor")
				{
					material->specular_color.r = (float)prop->getProperty(4)->getValue().toDouble();
					material->specular_color.g = (float)prop->getProperty(5)->getValue().toDouble();
					material->specular_color.b = (float)prop->getProperty(6)->getValue().toDouble();
				}
				else if (prop->first_property->value == "Shininess")
				{
					material->shininess = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "ShininessExponent")
				{
					material->shininess_exponent = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "ReflectionColor")
				{
					material->reflection_color.r = (float)prop->getProperty(4)->getValue().toDouble();
					material->reflection_color.g = (float)prop->getProperty(5)->getValue().toDouble();
					material->reflection_color.b = (float)prop->getProperty(6)->getValue().toDouble();
				}
				else if (prop->first_property->value == "AmbientColor")
				{
					material->ambient_color.r = (float)prop->getProperty(4)->getValue().toDouble();
					material->ambient_color.g = (float)prop->getProperty(5)->getValue().toDouble();
					material->ambient_color.b = (float)prop->getProperty(6)->getValue().toDouble();
				}
				else if (prop->first_property->value == "EmissiveColor")
				{
					material->emissive_color.r = (float)prop->getProperty(4)->getValue().toDouble();
					material->emissive_color.g = (float)prop->getProperty(5)->getValue().toDouble();
					material->emissive_color.b = (float)prop->getProperty(6)->getValue().toDouble();
				}
				else if (prop->first_property->value == "ReflectionFactor")
				{
					material->reflection_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "BumpFactor")
				{
					material->bump_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "AmbientFactor")
				{
					material->ambient_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "DiffuseFactor")
				{
					material->diffuse_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "SpecularFactor")
				{
					material->specular_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
				else if (prop->first_property->value == "EmissiveFactor")
				{
					material->emissive_factor = (float)prop->getProperty(4)->getValue().toDouble();
				}
			}
			prop = prop->sibling;
		}
		return material;
	}


	template <typename T> static bool parseTextArrayRaw(const Property& property, T* out, int max_size);

	template <typename T> static bool parseArrayRaw(const Property& property, T* out, int max_size)
	{
		if (property.value.is_binary)
		{
			assert(out);

			int elem_size = 1;
			switch (property.type)
			{
			case 'l': elem_size = 8; break;
			case 'd': elem_size = 8; break;
			case 'f': elem_size = 4; break;
			case 'i': elem_size = 4; break;
			default: return false;
			}

			const u8* data = property.value.begin + sizeof(u32) * 3;
			if (data > property.value.end) return false;

			u32 count = property.getCount();
			u32 enc = *(const u32*)(property.value.begin + 4);
			u32 len = *(const u32*)(property.value.begin + 8);

			if (enc == 0)
			{
				if ((int)len > max_size) return false;
				if (data + len > property.value.end) return false;
				memcpy(out, data, len);
				return true;
			}
			else if (enc == 1)
			{
				if (int(elem_size * count) > max_size) return false;
				return decompress(data, len, (u8*)out, elem_size * count);
			}

			return false;
		}

		return parseTextArrayRaw(property, out, max_size);
	}


	template <typename T> const char* fromString(const char* str, const char* end, T* val);
	template <> const char* fromString<int>(const char* str, const char* end, int* val)
	{
		*val = atoi(str);
		const char* iter = str;
		while (iter < end && *iter != ',') ++iter;
		if (iter < end) ++iter; // skip ','
		return (const char*)iter;
	}


	template <> const char* fromString<u64>(const char* str, const char* end, u64* val)
	{
		*val = strtoull(str, nullptr, 10);
		const char* iter = str;
		while (iter < end && *iter != ',') ++iter;
		if (iter < end) ++iter; // skip ','
		return (const char*)iter;
	}


	template <> const char* fromString<i64>(const char* str, const char* end, i64* val)
	{
		*val = atoll(str);
		const char* iter = str;
		while (iter < end && *iter != ',') ++iter;
		if (iter < end) ++iter; // skip ','
		return (const char*)iter;
	}


	template <> const char* fromString<double>(const char* str, const char* end, double* val)
	{
		*val = atof(str);
		const char* iter = str;
		while (iter < end && *iter != ',') ++iter;
		if (iter < end) ++iter; // skip ','
		return (const char*)iter;
	}


	template <> const char* fromString<float>(const char* str, const char* end, float* val)
	{
		*val = (float)atof(str);
		const char* iter = str;
		while (iter < end && *iter != ',') ++iter;
		if (iter < end) ++iter; // skip ','
		return (const char*)iter;
	}


	const char* fromString(const char* str, const char* end, double* val, int count)
	{
		const char* iter = str;
		for (int i = 0; i < count; ++i)
		{
			*val = atof(iter);
			++val;
			while (iter < end && *iter != ',') ++iter;
			if (iter < end) ++iter; // skip ','

			if (iter == end) return iter;
		}
		return (const char*)iter;
	}


	template <> const char* fromString<Vec2>(const char* str, const char* end, Vec2* val)
	{
		return fromString(str, end, &val->x, 2);
	}


	template <> const char* fromString<Vec3>(const char* str, const char* end, Vec3* val)
	{
		return fromString(str, end, &val->x, 3);
	}


	template <> const char* fromString<Vec4>(const char* str, const char* end, Vec4* val)
	{
		return fromString(str, end, &val->x, 4);
	}


	template <> const char* fromString<Matrix>(const char* str, const char* end, Matrix* val)
	{
		return fromString(str, end, &val->m[0], 16);
	}


	template <typename T> static void parseTextArray(const Property& property, std::vector<T>* out)
	{
		const u8* iter = property.value.begin;
		while (iter < property.value.end)
		{
			T val;
			iter = (const u8*)fromString<T>((const char*)iter, (const char*)property.value.end, &val);
			out->push_back(val);
		}
	}


	template <typename T> static bool parseTextArrayRaw(const Property& property, T* out_raw, int max_size)
	{
		const u8* iter = property.value.begin;

		T* out = out_raw;
		while (iter < property.value.end)
		{
			iter = (const u8*)fromString<T>((const char*)iter, (const char*)property.value.end, out);
			++out;
			if (out - out_raw == max_size / sizeof(T)) return true;
		}
		return out - out_raw == max_size / sizeof(T);
	}


	template <typename T> static bool parseBinaryArray(const Property& property, std::vector<T>* out)
	{
		assert(out);
		if (property.value.is_binary)
		{
			u32 count = property.getCount();
			int elem_size = 1;
			switch (property.type)
			{
			case 'd': elem_size = 8; break;
			case 'f': elem_size = 4; break;
			case 'i': elem_size = 4; break;
			default: return false;
			}
			int elem_count = sizeof(T) / elem_size;
			out->resize(count / elem_count);

			if (count == 0) return true;
			return parseArrayRaw(property, &(*out)[0], int(sizeof((*out)[0]) * out->size()));
		}
		else
		{
			parseTextArray(property, out);
			return true;
		}
	}


	template <typename T> static bool parseDoubleVecData(Property& property, std::vector<T>* out_vec, std::vector<float>* tmp)
	{
		assert(out_vec);
		if (!property.value.is_binary)
		{
			parseTextArray(property, out_vec);
			return true;
		}

		if (property.type == 'd')
		{
			return parseBinaryArray(property, out_vec);
		}

		assert(property.type == 'f');
		assert(sizeof((*out_vec)[0].x) == sizeof(double));
		tmp->clear();
		if (!parseBinaryArray(property, tmp)) return false;
		int elem_count = sizeof((*out_vec)[0]) / sizeof((*out_vec)[0].x);
		out_vec->resize(tmp->size() / elem_count);
		double* out = &(*out_vec)[0].x;
		for (int i = 0, c = (int)tmp->size(); i < c; ++i)
		{
			out[i] = (*tmp)[i];
		}
		return true;
	}


	static bool parseDouble(Property& property, double* out)
	{
		assert(out);
		if (property.value.is_binary)
		{
			int elem_size = 1;
			switch (property.type)
			{
			case 'D': elem_size = 8; break;
			case 'F': elem_size = 4; break;
			default: return false;
			}
			const u8* data = property.value.begin;
			if (data > property.value.end) return false;
			memcpy(out, data, elem_size);
			return true;
		}
		else
		{
			fromString<double>((const char*)property.value.begin, (const char*)property.value.end, out);
			return true;
		}
	}


	template <typename T>
	static bool parseVertexData(const Element& element,
		const char* name,
		const char* index_name,
		std::vector<T>* out,
		std::vector<int>* out_indices,
		GeometryImpl::VertexDataMapping* mapping,
		std::vector<float>* tmp)
	{
		assert(out);
		assert(mapping);
		const Element* data_element = findChild(element, name);
		if (!data_element || !data_element->first_property) return false;

		const Element* mapping_element = findChild(element, "MappingInformationType");
		const Element* reference_element = findChild(element, "ReferenceInformationType");
		out_indices->clear();
		if (mapping_element && mapping_element->first_property)
		{
			if (mapping_element->first_property->value == "ByPolygonVertex")
			{
				*mapping = GeometryImpl::BY_POLYGON_VERTEX;
			}
			else if (mapping_element->first_property->value == "ByPolygon")
			{
				*mapping = GeometryImpl::BY_POLYGON;
			}
			else if (mapping_element->first_property->value == "ByVertice" ||
				mapping_element->first_property->value == "ByVertex")
			{
				*mapping = GeometryImpl::BY_VERTEX;
			}
			else
			{
				return false;
			}
		}
		if (reference_element && reference_element->first_property)
		{
			if (reference_element->first_property->value == "IndexToDirect")
			{
				const Element* indices_element = findChild(element, index_name);
				if (indices_element && indices_element->first_property)
				{
					if (!parseBinaryArray(*indices_element->first_property, out_indices)) return false;
				}
			}
			else if (reference_element->first_property->value != "Direct")
			{
				return false;
			}
		}
		return parseDoubleVecData(*data_element->first_property, out, tmp);
	}


	static int decodeIndex(int idx)
	{
		return (idx < 0) ? (-idx - 1) : idx;
	}


	static int codeIndex(int idx, bool last)
	{
		return last ? (-idx - 1) : idx;
	}


	template <typename T>
	static void splat(std::vector<T>* out,
		GeometryImpl::VertexDataMapping mapping,
		const std::vector<T>& data,
		const std::vector<int>& indices,
		const std::vector<int>& original_indices)
	{
		assert(out);
		assert(!data.empty());

		if (mapping == GeometryImpl::BY_POLYGON_VERTEX)
		{
			if (indices.empty())
			{
				out->resize(data.size());
				memcpy(&(*out)[0], &data[0], sizeof(data[0]) * data.size());
			}
			else
			{
				out->resize(indices.size());
				int data_size = (int)data.size();
				for (int i = 0, c = (int)indices.size(); i < c; ++i)
				{
					int index = indices[i];

					if ((index < data_size) && (index >= 0))
						(*out)[i] = data[index];
					else
						(*out)[i] = T();
				}
			}
		}
		else if (mapping == GeometryImpl::BY_VERTEX)
		{
			//  v0  v1 ...
			// uv0 uv1 ...
			assert(indices.empty());

			out->resize(original_indices.size());

			int data_size = (int)data.size();
			for (int i = 0, c = (int)original_indices.size(); i < c; ++i)
			{
				int idx = decodeIndex(original_indices[i]);
				if ((idx < data_size) && (idx >= 0)) //-V560
					(*out)[i] = data[idx];
				else
					(*out)[i] = T();
			}
		}
		else
		{
			assert(false);
		}
	}


	template <typename T> static void remap(std::vector<T>* out, const std::vector<int>& map)
	{
		if (out->empty()) return;

		std::vector<T> old;
		old.swap(*out);
		int old_size = (int)old.size();
		for (int i = 0, c = (int)map.size(); i < c; ++i)
		{
			out->push_back(map[i] < old_size ? old[map[i]] : T());
		}
	}


	static OptionalError<Object*> parseAnimationCurve(const Scene& scene, const Element& element, Allocator& allocator)
	{
		AnimationCurveImpl* curve = allocator.allocate<AnimationCurveImpl>(scene, element);

		const Element* times = findChild(element, "KeyTime");
		const Element* values = findChild(element, "KeyValueFloat");

		if (times && times->first_property)
		{
			curve->times.resize(times->first_property->getCount());
			if (!times->first_property->getValues(&curve->times[0], (int)curve->times.size() * sizeof(curve->times[0])))
			{
				return Error("Invalid animation curve");
			}
		}

		if (values && values->first_property)
		{
			curve->values.resize(values->first_property->getCount());
			if (!values->first_property->getValues(&curve->values[0], (int)curve->values.size() * sizeof(curve->values[0])))
			{
				return Error("Invalid animation curve");
			}
		}

		if (curve->times.size() != curve->values.size()) return Error("Invalid animation curve");

		return curve;
	}


	static int getTriCountFromPoly(const std::vector<int>& indices, int* idx)
	{
		int count = 1;
		while (indices[*idx + 1 + count] >= 0)
		{
			++count;
		}

		*idx = *idx + 2 + count;
		return count;
	}


	static void add(GeometryImpl::NewVertex& vtx, int index)
	{
		if (vtx.index == -1)
		{
			vtx.index = index;
		}
		else if (vtx.next)
		{
			add(*vtx.next, index);
		}
		else
		{
			vtx.next = new GeometryImpl::NewVertex;
			vtx.next->index = index;
		}
	}


	static void triangulate(
		const std::vector<int>& old_indices,
		std::vector<int>* to_old_vertices,
		std::vector<int>* to_old_indices)
	{
		assert(to_old_vertices);
		assert(to_old_indices);

		auto getIdx = [&old_indices](int i) -> int {
			int idx = old_indices[i];
			return decodeIndex(idx);
		};

		int in_polygon_idx = 0;
		for (int i = 0; i < (int)old_indices.size(); ++i)
		{
			int idx = getIdx(i);
			if (in_polygon_idx <= 2) //-V1051
			{
				to_old_vertices->push_back(idx);
				to_old_indices->push_back(i);
			}
			else
			{
				to_old_vertices->push_back(old_indices[i - in_polygon_idx]);
				to_old_indices->push_back(i - in_polygon_idx);
				to_old_vertices->push_back(old_indices[i - 1]);
				to_old_indices->push_back(i - 1);
				to_old_vertices->push_back(idx);
				to_old_indices->push_back(i);
			}
			++in_polygon_idx;
			if (old_indices[i] < 0)
			{
				in_polygon_idx = 0;
			}
		}
	}


	static void buildGeometryVertexData(
		GeometryImpl* geom,
		const std::vector<Vec3>& vertices,
		const std::vector<int>& original_indices,
		std::vector<int>& to_old_indices,
		bool triangulationEnabled)
	{
		if (triangulationEnabled) {
			triangulate(original_indices, &geom->to_old_vertices, &to_old_indices);
			geom->vertices.resize(geom->to_old_vertices.size());
			geom->indices.resize(geom->vertices.size());
			for (int i = 0, c = (int)geom->to_old_vertices.size(); i < c; ++i)
			{
				geom->vertices[i] = vertices[geom->to_old_vertices[i]];
				geom->indices[i] = codeIndex(i, i % 3 == 2);
			}
		}
		else {
			geom->vertices = vertices;
			geom->to_old_vertices.resize(original_indices.size());
			for (size_t i = 0; i < original_indices.size(); ++i) {
				geom->to_old_vertices[i] = decodeIndex(original_indices[i]);
			}
			geom->indices = original_indices;
			to_old_indices.resize(original_indices.size());
			iota(to_old_indices.begin(), to_old_indices.end(), 0);
		}

		geom->to_new_vertices.resize(vertices.size()); // some vertices can be unused, so this isn't necessarily the same size as to_old_vertices.
		const int* to_old_vertices = geom->to_old_vertices.empty() ? nullptr : &geom->to_old_vertices[0];
		for (int i = 0, c = (int)geom->to_old_vertices.size(); i < c; ++i)
		{
			int old = to_old_vertices[i];
			add(geom->to_new_vertices[old], i);
		}
	}


	static OptionalError<Object*> parseGeometryMaterials(
		GeometryImpl* geom,
		const Element& element,
		const std::vector<int>& original_indices)
	{
		const Element* layer_material_element = findChild(element, "LayerElementMaterial");
		if (layer_material_element)
		{
			const Element* mapping_element = findChild(*layer_material_element, "MappingInformationType");
			const Element* reference_element = findChild(*layer_material_element, "ReferenceInformationType");

			if (!mapping_element || !reference_element) return Error("Invalid LayerElementMaterial");

			if (mapping_element->first_property->value == "ByPolygon" &&
				reference_element->first_property->value == "IndexToDirect")
			{
				geom->materials.reserve(geom->vertices.size() / 3);
				for (int& i : geom->materials) i = -1;

				const Element* indices_element = findChild(*layer_material_element, "Materials");
				if (!indices_element || !indices_element->first_property) return Error("Invalid LayerElementMaterial");

				std::vector<int> int_tmp;
				if (!parseBinaryArray(*indices_element->first_property, &int_tmp)) return Error("Failed to parse material indices");

				int tmp_i = 0;
				for (int poly = 0, c = (int)int_tmp.size(); poly < c; ++poly)
				{
					int tri_count = getTriCountFromPoly(original_indices, &tmp_i);
					for (int i = 0; i < tri_count; ++i) {
						geom->materials.push_back(int_tmp[poly]);
					}
				}
			}
			else
			{
				if (mapping_element->first_property->value != "AllSame") return Error("Mapping not supported");
			}
		}
		return { nullptr };
	}


	static OptionalError<Object*> parseGeometryUVs(
		GeometryImpl* geom,
		const Element& element,
		const std::vector<int>& original_indices,
		const std::vector<int>& to_old_indices,
		Temporaries* tmp)
	{
		const Element* layer_uv_element = findChild(element, "LayerElementUV");
		while (layer_uv_element)
		{
			const int uv_index =
				layer_uv_element->first_property ? layer_uv_element->first_property->getValue().toInt() : 0;
			if (uv_index >= 0 && uv_index < Geometry::s_uvs_max)
			{
				std::vector<Vec2>& uvs = geom->uvs[uv_index];

				tmp->v2.clear();
				tmp->i.clear();
				GeometryImpl::VertexDataMapping mapping;
				if (!parseVertexData(*layer_uv_element, "UV", "UVIndex", &tmp->v2, &tmp->i, &mapping, &tmp->f))
					return Error("Invalid UVs");
				if (!tmp->v2.empty() && (tmp->i.empty() || tmp->i[0] != -1))
				{
					uvs.resize(tmp->i.empty() ? tmp->v2.size() : tmp->i.size());
					splat(&uvs, mapping, tmp->v2, tmp->i, original_indices);
					remap(&uvs, to_old_indices);
				}
			}

			do
			{
				layer_uv_element = layer_uv_element->sibling;
			} while (layer_uv_element && layer_uv_element->id != "LayerElementUV");
		}
		return { nullptr };
	}


	static OptionalError<Object*> parseGeometryTangents(
		GeometryImpl* geom,
		const Element& element,
		const std::vector<int>& original_indices,
		const std::vector<int>& to_old_indices,
		Temporaries* tmp)
	{
		const Element* layer_tangent_element = findChild(element, "LayerElementTangents");
		if (!layer_tangent_element) {
			layer_tangent_element = findChild(element, "LayerElementTangent");
		}
		if (layer_tangent_element)
		{
			GeometryImpl::VertexDataMapping mapping;
			if (findChild(*layer_tangent_element, "Tangents"))
			{
				if (!parseVertexData(*layer_tangent_element, "Tangents", "TangentsIndex", &tmp->v3, &tmp->i, &mapping, &tmp->f))
					return Error("Invalid tangets");
			}
			else
			{
				if (!parseVertexData(*layer_tangent_element, "Tangent", "TangentIndex", &tmp->v3, &tmp->i, &mapping, &tmp->f))
					return Error("Invalid tangets");
			}
			if (!tmp->v3.empty())
			{
				splat(&geom->tangents, mapping, tmp->v3, tmp->i, original_indices);
				remap(&geom->tangents, to_old_indices);
			}
		}
		return { nullptr };
	}


	static OptionalError<Object*> parseGeometryColors(
		GeometryImpl* geom,
		const Element& element,
		const std::vector<int>& original_indices,
		const std::vector<int>& to_old_indices,
		Temporaries* tmp)
	{
		const Element* layer_color_element = findChild(element, "LayerElementColor");
		if (layer_color_element)
		{
			GeometryImpl::VertexDataMapping mapping;
			if (!parseVertexData(*layer_color_element, "Colors", "ColorIndex", &tmp->v4, &tmp->i, &mapping, &tmp->f))
				return Error("Invalid colors");
			if (!tmp->v4.empty())
			{
				splat(&geom->colors, mapping, tmp->v4, tmp->i, original_indices);
				remap(&geom->colors, to_old_indices);
			}
		}
		return { nullptr };
	}


	static OptionalError<Object*> parseGeometryNormals(
		GeometryImpl* geom,
		const Element& element,
		const std::vector<int>& original_indices,
		const std::vector<int>& to_old_indices,
		Temporaries* tmp)
	{
		const Element* layer_normal_element = findChild(element, "LayerElementNormal");
		if (layer_normal_element)
		{
			GeometryImpl::VertexDataMapping mapping;
			if (!parseVertexData(*layer_normal_element, "Normals", "NormalsIndex", &tmp->v3, &tmp->i, &mapping, &tmp->f))
				return Error("Invalid normals");
			if (!tmp->v3.empty())
			{
				splat(&geom->normals, mapping, tmp->v3, tmp->i, original_indices);
				remap(&geom->normals, to_old_indices);
			}
		}
		return { nullptr };
	}


	static OptionalError<Object*> parseGeometry(const Element& element, bool triangulate, GeometryImpl* geom)
	{
		assert(element.first_property);

		const Element* vertices_element = findChild(element, "Vertices");
		if (!vertices_element || !vertices_element->first_property)
		{
			return geom;
		}

		const Element* polys_element = findChild(element, "PolygonVertexIndex");
		if (!polys_element || !polys_element->first_property) return Error("Indices missing");

		std::vector<Vec3> vertices;
		std::vector<int> original_indices;
		std::vector<int> to_old_indices;
		Temporaries tmp;
		if (!parseDoubleVecData(*vertices_element->first_property, &vertices, &tmp.f)) return Error("Failed to parse vertices");
		if (!parseBinaryArray(*polys_element->first_property, &original_indices)) return Error("Failed to parse indices");

		buildGeometryVertexData(geom, vertices, original_indices, to_old_indices, triangulate);

		OptionalError<Object*> materialParsingError = parseGeometryMaterials(geom, element, original_indices);
		if (materialParsingError.isError()) return materialParsingError;

		OptionalError<Object*> uvParsingError = parseGeometryUVs(geom, element, original_indices, to_old_indices, &tmp);
		if (uvParsingError.isError()) return uvParsingError;

		OptionalError<Object*> tangentsParsingError = parseGeometryTangents(geom, element, original_indices, to_old_indices, &tmp);
		if (tangentsParsingError.isError()) return tangentsParsingError;

		OptionalError<Object*> colorsParsingError = parseGeometryColors(geom, element, original_indices, to_old_indices, &tmp);
		if (colorsParsingError.isError()) return colorsParsingError;

		OptionalError<Object*> normalsParsingError = parseGeometryNormals(geom, element, original_indices, to_old_indices, &tmp);
		if (normalsParsingError.isError()) return normalsParsingError;

		return geom;
	}


	bool ShapeImpl::postprocess(GeometryImpl* geom, Allocator& allocator)
	{
		assert(geom);

		const Element* vertices_element = findChild((const Element&)element, "Vertices");
		const Element* normals_element = findChild((const Element&)element, "Normals");
		const Element* indexes_element = findChild((const Element&)element, "Indexes");
		if (!vertices_element || !vertices_element->first_property ||
			!indexes_element || !indexes_element->first_property)
		{
			return false;
		}

		allocator.vec3_tmp.clear(); // old vertices
		allocator.vec3_tmp2.clear(); // old normals
		allocator.int_tmp.clear(); // old indices
		if (!parseDoubleVecData(*vertices_element->first_property, &allocator.vec3_tmp, &allocator.tmp)) return true;
		if (!parseDoubleVecData(*normals_element->first_property, &allocator.vec3_tmp2, &allocator.tmp)) return true;
		if (!parseBinaryArray(*indexes_element->first_property, &allocator.int_tmp)) return true;

		if (allocator.vec3_tmp.size() != allocator.int_tmp.size() || allocator.vec3_tmp2.size() != allocator.int_tmp.size()) return false;

		vertices = geom->vertices;
		normals = geom->normals;

		Vec3* vr = &allocator.vec3_tmp[0];
		Vec3* nr = &allocator.vec3_tmp2[0];
		int* ir = &allocator.int_tmp[0];
		for (int i = 0, c = (int)allocator.int_tmp.size(); i < c; ++i)
		{
			int old_idx = ir[i];
			GeometryImpl::NewVertex* n = &geom->to_new_vertices[old_idx];
			if (n->index == -1) continue; // skip vertices which aren't indexed.
			while (n)
			{
				vertices[n->index] = vertices[n->index] + vr[i];
				normals[n->index] = vertices[n->index] + nr[i];
				n = n->next;
			}
		}

		return true;
	}


	static bool isString(const Property* prop)
	{
		if (!prop) return false;
		return prop->getType() == Property::STRING;
	}


	static bool isLong(const Property* prop)
	{
		if (!prop) return false;
		return prop->getType() == Property::LONG;
	}


	static bool parseConnections(const Element& root, Scene* scene)
	{
		assert(scene);

		const Element* connections = findChild(root, "Connections");
		if (!connections) return true;

		scene->m_connections.reserve(1024);
		const Element* connection = connections->child;
		while (connection)
		{
			if (!isString(connection->first_property)
				|| !isLong(connection->first_property->next)
				|| !isLong(connection->first_property->next->next))
			{
				Error::s_message = "Invalid connection";
				return false;
			}

			Scene::Connection c;
			c.from = connection->first_property->next->value.toU64();
			c.to = connection->first_property->next->next->value.toU64();
			if (connection->first_property->value == "OO")
			{
				c.type = Scene::Connection::OBJECT_OBJECT;
			}
			else if (connection->first_property->value == "OP")
			{
				c.type = Scene::Connection::OBJECT_PROPERTY;
				if (!connection->first_property->next->next->next)
				{
					Error::s_message = "Invalid connection";
					return false;
				}
				c.property = connection->first_property->next->next->next->value;
			}
			else
			{
				assert(false);
				Error::s_message = "Not supported";
				return false;
			}
			scene->m_connections.push_back(c);

			connection = connection->sibling;
		}
		return true;
	}


	static bool parseTakes(Scene* scene)
	{
		const Element* takes = findChild((const Element&)*scene->getRootElement(), "Takes");
		if (!takes) return true;

		const Element* object = takes->child;
		while (object)
		{
			if (object->id == "Take")
			{
				if (!isString(object->first_property))
				{
					Error::s_message = "Invalid name in take";
					return false;
				}

				TakeInfo take;
				take.name = object->first_property->value;
				const Element* filename = findChild(*object, "FileName");
				if (filename)
				{
					if (!isString(filename->first_property))
					{
						Error::s_message = "Invalid filename in take";
						return false;
					}
					take.filename = filename->first_property->value;
				}
				const Element* local_time = findChild(*object, "LocalTime");
				if (local_time)
				{
					if (!isLong(local_time->first_property) || !isLong(local_time->first_property->next))
					{
						Error::s_message = "Invalid local time in take";
						return false;
					}

					take.local_time_from = fbxTimeToSeconds(local_time->first_property->value.toI64());
					take.local_time_to = fbxTimeToSeconds(local_time->first_property->next->value.toI64());
				}
				const Element* reference_time = findChild(*object, "ReferenceTime");
				if (reference_time)
				{
					if (!isLong(reference_time->first_property) || !isLong(reference_time->first_property->next))
					{
						Error::s_message = "Invalid reference time in take";
						return false;
					}

					take.reference_time_from = fbxTimeToSeconds(reference_time->first_property->value.toI64());
					take.reference_time_to = fbxTimeToSeconds(reference_time->first_property->next->value.toI64());
				}

				scene->m_take_infos.push_back(take);
			}

			object = object->sibling;
		}

		return true;
	}


	static float getFramerateFromTimeMode(FrameRate time_mode, float custom_frame_rate)
	{
		switch (time_mode)
		{
		case FrameRate_DEFAULT: return 14;
		case FrameRate_120: return 120;
		case FrameRate_100: return 100;
		case FrameRate_60: return 60;
		case FrameRate_50: return 50;
		case FrameRate_48: return 48;
		case FrameRate_30: return 30;
		case FrameRate_30_DROP: return 30;
		case FrameRate_NTSC_DROP_FRAME: return 29.9700262f;
		case FrameRate_NTSC_FULL_FRAME: return 29.9700262f;
		case FrameRate_PAL: return 25;
		case FrameRate_CINEMA: return 24;
		case FrameRate_1000: return 1000;
		case FrameRate_CINEMA_ND: return 23.976f;
		case FrameRate_CUSTOM: return custom_frame_rate;
		}
		return -1;
	}


	static void parseGlobalSettings(const Element& root, Scene* scene)
	{
		const Element* settings = findChild(root, "GlobalSettings");
		if (!settings) return;

		const Element* props70 = findChild(*settings, "Properties70");
		if (!props70) return;

		for (Element* node = props70->child; node; node = node->sibling) {
			if (!node->first_property) continue;

#define get_property(name, field, type, getter) if(node->first_property->value == name) \
		{ \
			IElementProperty* prop = node->getProperty(4); \
			if (prop) \
			{ \
				DataView value = prop->getValue(); \
				scene->m_settings.field = (type)value.getter(); \
			} \
		}

#define get_time_property(name, field, type, getter) if(node->first_property->value == name) \
		{ \
			IElementProperty* prop = node->getProperty(4); \
			if (prop) \
			{ \
				DataView value = prop->getValue(); \
				scene->m_settings.field = fbxTimeToSeconds((type)value.getter()); \
			} \
		}

			get_property("UpAxis", UpAxis, UpVector, toInt);
			get_property("UpAxisSign", UpAxisSign, int, toInt);
			get_property("FrontAxis", FrontAxis, int, toInt);
			get_property("FrontAxisSign", FrontAxisSign, int, toInt);
			get_property("CoordAxis", CoordAxis, CoordSystem, toInt);
			get_property("CoordAxisSign", CoordAxisSign, int, toInt);
			get_property("OriginalUpAxis", OriginalUpAxis, int, toInt);
			get_property("OriginalUpAxisSign", OriginalUpAxisSign, int, toInt);
			get_property("UnitScaleFactor", UnitScaleFactor, float, toDouble);
			get_property("OriginalUnitScaleFactor", OriginalUnitScaleFactor, float, toDouble);
			get_time_property("TimeSpanStart", TimeSpanStart, u64, toU64);
			get_time_property("TimeSpanStop", TimeSpanStop, u64, toU64);
			get_property("TimeMode", TimeMode, FrameRate, toInt);
			get_property("CustomFrameRate", CustomFrameRate, float, toDouble);

#undef get_property
#undef get_time_property

			scene->m_scene_frame_rate = getFramerateFromTimeMode(scene->m_settings.TimeMode, scene->m_settings.CustomFrameRate);
		}
	}


	struct ParseGeometryJob {
		const Element* element;
		bool triangulate;
		GeometryImpl* geom;
		u64 id;
		bool is_error;
	};

	void sync_job_processor(JobFunction fn, void*, void* data, u32 size, u32 count) {
		u8* ptr = (u8*)data;
		for (u32 i = 0; i < count; ++i) {
			fn(ptr);
			ptr += size;
		}
	}

	static bool parseObjects(const Element& root, Scene* scene, u64 flags, Allocator& allocator, JobProcessor job_processor, void* job_user_ptr)
	{
		if (!job_processor) job_processor = &sync_job_processor;
		const bool triangulate = (flags & (u64)LoadFlags::TRIANGULATE) != 0;
		const bool ignore_geometry = (flags & (u64)LoadFlags::IGNORE_GEOMETRY) != 0;
		const bool ignore_blend_shapes = (flags & (u64)LoadFlags::IGNORE_BLEND_SHAPES) != 0;
		const Element* objs = findChild(root, "Objects");
		if (!objs) return true;

		scene->m_root = allocator.allocate<Root>(*scene, root);
		scene->m_root->id = 0;
		scene->m_object_map[0] = { &root, scene->m_root };

		const Element* object = objs->child;
		while (object)
		{
			if (!isLong(object->first_property))
			{
				Error::s_message = "Invalid";
				return false;
			}

			u64 id = object->first_property->value.toU64();
			scene->m_object_map[id] = { object, nullptr };
			object = object->sibling;
		}

		std::vector<ParseGeometryJob> parse_geom_jobs;
		for (auto iter : scene->m_object_map)
		{
			OptionalError<Object*> obj = nullptr;

			if (iter.second.object == scene->m_root) continue;

			if (iter.second.element->id == "Geometry")
			{
				Property* last_prop = iter.second.element->first_property;
				while (last_prop->next) last_prop = last_prop->next;
				if (last_prop && last_prop->value == "Mesh" && !ignore_geometry)
				{
					GeometryImpl* geom = allocator.allocate<GeometryImpl>(*scene, *iter.second.element);
					scene->m_geometries.push_back(geom);
					ParseGeometryJob job{ iter.second.element, triangulate, geom, iter.first, false };
					parse_geom_jobs.push_back(job);
					continue;
				}
				if (last_prop && last_prop->value == "Shape" && !ignore_geometry)
				{
					obj = allocator.allocate<ShapeImpl>(*scene, *iter.second.element);
				}
			}
			else if (iter.second.element->id == "Material")
			{
				obj = parseMaterial(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "AnimationStack")
			{
				obj = parse<AnimationStackImpl>(*scene, *iter.second.element, allocator);
				if (!obj.isError())
				{
					AnimationStackImpl* stack = (AnimationStackImpl*)obj.getValue();
					scene->m_animation_stacks.push_back(stack);
				}
			}
			else if (iter.second.element->id == "AnimationLayer")
			{
				obj = parse<AnimationLayerImpl>(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "AnimationCurve")
			{
				obj = parseAnimationCurve(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "AnimationCurveNode")
			{
				obj = parse<AnimationCurveNodeImpl>(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "Deformer")
			{
				IElementProperty* class_prop = iter.second.element->getProperty(2);

				if (class_prop)
				{
					if (class_prop->getValue() == "Cluster")
						obj = parseCluster(*scene, *iter.second.element, allocator);
					else if (class_prop->getValue() == "Skin")
						obj = parse<SkinImpl>(*scene, *iter.second.element, allocator);
					else if (class_prop->getValue() == "BlendShape" && !ignore_blend_shapes)
						obj = parse<BlendShapeImpl>(*scene, *iter.second.element, allocator);
					else if (class_prop->getValue() == "BlendShapeChannel" && !ignore_blend_shapes)
						obj = parse<BlendShapeChannelImpl>(*scene, *iter.second.element, allocator);
				}
			}
			else if (iter.second.element->id == "NodeAttribute")
			{
				obj = parseNodeAttribute(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "Model")
			{
				IElementProperty* class_prop = iter.second.element->getProperty(2);

				if (class_prop)
				{
					if (class_prop->getValue() == "Mesh")
					{
						obj = parseMesh(*scene, *iter.second.element, allocator);
						if (!obj.isError())
						{
							Mesh* mesh = (Mesh*)obj.getValue();
							scene->m_meshes.push_back(mesh);
							obj = mesh;
						}
					}
					else if (class_prop->getValue() == "LimbNode")
						obj = parseLimbNode(*scene, *iter.second.element, allocator);
					else
						obj = parse<NullImpl>(*scene, *iter.second.element, allocator);
				}
			}
			else if (iter.second.element->id == "Texture")
			{
				obj = parseTexture(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "Video")
			{
				parseVideo(*scene, *iter.second.element, allocator);
			}
			else if (iter.second.element->id == "Pose")
			{
				obj = parsePose(*scene, *iter.second.element, allocator);
			}

			if (obj.isError()) return false;

			scene->m_object_map[iter.first].object = obj.getValue();
			if (obj.getValue())
			{
				scene->m_all_objects.push_back(obj.getValue());
				obj.getValue()->id = iter.first;
			}
		}

		if (!parse_geom_jobs.empty()) {
			(*job_processor)([](void* ptr) {
				ParseGeometryJob* job = (ParseGeometryJob*)ptr;
				job->is_error = parseGeometry(*job->element, job->triangulate, job->geom).isError();
				}, job_user_ptr, &parse_geom_jobs[0], (u32)sizeof(parse_geom_jobs[0]), (u32)parse_geom_jobs.size());
		}

		for (const ParseGeometryJob& job : parse_geom_jobs) {
			if (job.is_error) return false;
			scene->m_object_map[job.id].object = job.geom;
			if (job.geom) {
				scene->m_all_objects.push_back(job.geom);
				job.geom->id = job.id;
			}
		}

		for (const Scene::Connection& con : scene->m_connections)
		{
			Object* parent = scene->m_object_map[con.to].object;
			Object* child = scene->m_object_map[con.from].object;
			if (!child) continue;
			if (!parent) continue;

			switch (child->getType())
			{
			case Object::Type::NODE_ATTRIBUTE:
				if (parent->node_attribute)
				{
					Error::s_message = "Invalid node attribute";
					return false;
				}
				parent->node_attribute = (NodeAttribute*)child;
				break;
			case Object::Type::ANIMATION_CURVE_NODE:
				if (parent->isNode())
				{
					AnimationCurveNodeImpl* node = (AnimationCurveNodeImpl*)child;
					node->bone = parent;
					node->bone_link_property = con.property;
				}
				break;
			}

			switch (parent->getType())
			{
			case Object::Type::MESH:
			{
				MeshImpl* mesh = (MeshImpl*)parent;
				switch (child->getType())
				{
				case Object::Type::GEOMETRY:
					if (mesh->geometry)
					{
						Error::s_message = "Invalid mesh";
						return false;
					}
					mesh->geometry = (Geometry*)child;
					break;
				case Object::Type::MATERIAL: mesh->materials.push_back((Material*)child); break;
				}
				break;
			}
			case Object::Type::SKIN:
			{
				SkinImpl* skin = (SkinImpl*)parent;
				if (child->getType() == Object::Type::CLUSTER)
				{
					ClusterImpl* cluster = (ClusterImpl*)child;
					skin->clusters.push_back(cluster);
					if (cluster->skin)
					{
						Error::s_message = "Invalid cluster";
						return false;
					}
					cluster->skin = skin;
				}
				break;
			}
			case Object::Type::BLEND_SHAPE:
			{
				BlendShapeImpl* blendShape = (BlendShapeImpl*)parent;
				if (child->getType() == Object::Type::BLEND_SHAPE_CHANNEL)
				{
					BlendShapeChannelImpl* blendShapeChannel = (BlendShapeChannelImpl*)child;
					blendShape->blendShapeChannels.push_back(blendShapeChannel);
					if (blendShapeChannel->blendShape)
					{
						Error::s_message = "Invalid blend shape";
						return false;
					}
					blendShapeChannel->blendShape = blendShape;
				}
				break;
			}
			case Object::Type::BLEND_SHAPE_CHANNEL:
			{
				BlendShapeChannelImpl* blendShapeChannel = (BlendShapeChannelImpl*)parent;
				if (child->getType() == Object::Type::SHAPE)
				{
					ShapeImpl* shape = (ShapeImpl*)child;
					blendShapeChannel->shapes.push_back(shape);
				}
				break;
			}
			case Object::Type::MATERIAL:
			{
				MaterialImpl* mat = (MaterialImpl*)parent;
				if (child->getType() == Object::Type::TEXTURE)
				{
					Texture::TextureType type = Texture::COUNT;
					if (con.property == "NormalMap")
						type = Texture::NORMAL;
					else if (con.property == "DiffuseColor")
						type = Texture::DIFFUSE;
					else if (con.property == "SpecularColor")
						type = Texture::SPECULAR;
					else if (con.property == "ShininessExponent")
						type = Texture::SHININESS;
					else if (con.property == "EmissiveColor")
						type = Texture::EMISSIVE;
					else if (con.property == "AmbientColor")
						type = Texture::AMBIENT;
					else if (con.property == "ReflectionFactor")
						type = Texture::REFLECTION;
					if (type == Texture::COUNT) break;

					if (mat->textures[type])
					{
						break; // This may happen for some models (eg. 2 normal maps in use)
					}

					mat->textures[type] = (Texture*)child;
				}
				break;
			}
			case Object::Type::GEOMETRY:
			{
				GeometryImpl* geom = (GeometryImpl*)parent;
				if (child->getType() == Object::Type::SKIN)
					geom->skin = (Skin*)child;
				else if (child->getType() == Object::Type::BLEND_SHAPE)
					geom->blendShape = (BlendShape*)child;
				break;
			}
			case Object::Type::CLUSTER:
			{
				ClusterImpl* cluster = (ClusterImpl*)parent;
				if (child->getType() == Object::Type::LIMB_NODE || child->getType() == Object::Type::MESH || child->getType() == Object::Type::NULL_NODE)
				{
					if (cluster->link && cluster->link != child)
					{
						Error::s_message = "Invalid cluster";
						return false;
					}

					cluster->link = child;
				}
				break;
			}
			case Object::Type::ANIMATION_LAYER:
			{
				if (child->getType() == Object::Type::ANIMATION_CURVE_NODE)
				{
					((AnimationLayerImpl*)parent)->curve_nodes.push_back((AnimationCurveNodeImpl*)child);
				}
			}
			break;
			case Object::Type::ANIMATION_CURVE_NODE:
			{
				AnimationCurveNodeImpl* node = (AnimationCurveNodeImpl*)parent;
				if (child->getType() == Object::Type::ANIMATION_CURVE)
				{
					char tmp[32];
					con.property.toString(tmp);
					if (strcmp(tmp, "d|X") == 0)
					{
						node->curves[0].connection = &con;
						node->curves[0].curve = (AnimationCurve*)child;
					}
					else if (strcmp(tmp, "d|Y") == 0)
					{
						node->curves[1].connection = &con;
						node->curves[1].curve = (AnimationCurve*)child;
					}
					else if (strcmp(tmp, "d|Z") == 0)
					{
						node->curves[2].connection = &con;
						node->curves[2].curve = (AnimationCurve*)child;
					}
				}
				break;
			}
			}
		}

		if (!ignore_geometry) {
			for (auto iter : scene->m_object_map)
			{
				Object* obj = iter.second.object;
				if (!obj) continue;
				switch (obj->getType()) {
				case Object::Type::CLUSTER:
					if (!((ClusterImpl*)iter.second.object)->postprocess(scene->m_allocator)) {
						Error::s_message = "Failed to postprocess cluster";
						return false;
					}
					break;
				case Object::Type::BLEND_SHAPE_CHANNEL:
					if (!((BlendShapeChannelImpl*)iter.second.object)->postprocess(scene->m_allocator)) {
						Error::s_message = "Failed to postprocess blend shape channel";
						return false;
					}
					break;
				case Object::Type::POSE:
					if (!((PoseImpl*)iter.second.object)->postprocess(scene)) {
						Error::s_message = "Failed to postprocess pose";
						return false;
					}
					break;
				}
			}
		}

		return true;
	}


	RotationOrder Object::getRotationOrder() const
	{
		// This assumes that the default rotation order is EULER_XYZ.
		return (RotationOrder)resolveEnumProperty(*this, "RotationOrder", (int)RotationOrder::EULER_XYZ);
	}


	Vec3 Object::getRotationOffset() const
	{
		return resolveVec3Property(*this, "RotationOffset", { 0, 0, 0 });
	}


	Vec3 Object::getRotationPivot() const
	{
		return resolveVec3Property(*this, "RotationPivot", { 0, 0, 0 });
	}


	Vec3 Object::getPostRotation() const
	{
		return resolveVec3Property(*this, "PostRotation", { 0, 0, 0 });
	}


	Vec3 Object::getScalingOffset() const
	{
		return resolveVec3Property(*this, "ScalingOffset", { 0, 0, 0 });
	}


	Vec3 Object::getScalingPivot() const
	{
		return resolveVec3Property(*this, "ScalingPivot", { 0, 0, 0 });
	}


	Matrix Object::evalLocal(const Vec3& translation, const Vec3& rotation) const
	{
		return evalLocal(translation, rotation, getLocalScaling());
	}


	Matrix Object::evalLocal(const Vec3& translation, const Vec3& rotation, const Vec3& scaling) const
	{
		Vec3 rotation_pivot = getRotationPivot();
		Vec3 scaling_pivot = getScalingPivot();
		RotationOrder rotation_order = getRotationOrder();

		Matrix s = makeIdentity();
		s.m[0] = scaling.x;
		s.m[5] = scaling.y;
		s.m[10] = scaling.z;

		Matrix t = makeIdentity();
		setTranslation(translation, &t);

		Matrix r = getRotationMatrix(rotation, rotation_order);
		Matrix r_pre = getRotationMatrix(getPreRotation(), RotationOrder::EULER_XYZ);
		Matrix r_post_inv = getRotationMatrix(-getPostRotation(), RotationOrder::EULER_ZYX);

		Matrix r_off = makeIdentity();
		setTranslation(getRotationOffset(), &r_off);

		Matrix r_p = makeIdentity();
		setTranslation(rotation_pivot, &r_p);

		Matrix r_p_inv = makeIdentity();
		setTranslation(-rotation_pivot, &r_p_inv);

		Matrix s_off = makeIdentity();
		setTranslation(getScalingOffset(), &s_off);

		Matrix s_p = makeIdentity();
		setTranslation(scaling_pivot, &s_p);

		Matrix s_p_inv = makeIdentity();
		setTranslation(-scaling_pivot, &s_p_inv);

		// http://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_10CDD63C_79C1_4F2D_BB28_AD2BE65A02ED_htm
		return t * r_off * r_p * r_pre * r * r_post_inv * r_p_inv * s_off * s_p * s * s_p_inv;
	}


	Vec3 Object::getLocalTranslation() const
	{
		return resolveVec3Property(*this, "Lcl Translation", { 0, 0, 0 });
	}


	Vec3 Object::getPreRotation() const
	{
		return resolveVec3Property(*this, "PreRotation", { 0, 0, 0 });
	}


	Vec3 Object::getLocalRotation() const
	{
		return resolveVec3Property(*this, "Lcl Rotation", { 0, 0, 0 });
	}


	Vec3 Object::getLocalScaling() const
	{
		return resolveVec3Property(*this, "Lcl Scaling", { 1, 1, 1 });
	}


	Matrix Object::getGlobalTransform() const
	{
		const Object* parent = getParent();
		if (!parent) return evalLocal(getLocalTranslation(), getLocalRotation());

		return parent->getGlobalTransform() * evalLocal(getLocalTranslation(), getLocalRotation());
	}


	Matrix Object::getLocalTransform() const
	{
		return evalLocal(getLocalTranslation(), getLocalRotation(), getLocalScaling());
	}


	Object* Object::resolveObjectLinkReverse(Object::Type type) const
	{
		u64 id = element.getFirstProperty() ? element.getFirstProperty()->getValue().toU64() : 0;
		for (auto& connection : scene.m_connections)
		{
			if (connection.from == id && connection.to != 0)
			{
				const Scene::ObjectPair& pair = scene.m_object_map.find(connection.to)->second;
				Object* obj = pair.object;
				if (obj && obj->getType() == type) return obj;
			}
		}
		return nullptr;
	}


	const IScene& Object::getScene() const
	{
		return scene;
	}


	Object* Object::resolveObjectLink(int idx) const
	{
		u64 id = element.getFirstProperty() ? element.getFirstProperty()->getValue().toU64() : 0;
		for (auto& connection : scene.m_connections)
		{
			if (connection.to == id && connection.from != 0)
			{
				Object* obj = scene.m_object_map.find(connection.from)->second.object;
				if (obj)
				{
					if (idx == 0) return obj;
					--idx;
				}
			}
		}
		return nullptr;
	}


	Object* Object::resolveObjectLink(Object::Type type, const char* property, int idx) const
	{
		u64 id = element.getFirstProperty() ? element.getFirstProperty()->getValue().toU64() : 0;
		for (auto& connection : scene.m_connections)
		{
			if (connection.to == id && connection.from != 0)
			{
				Object* obj = scene.m_object_map.find(connection.from)->second.object;
				if (obj && obj->getType() == type)
				{
					if (property == nullptr || connection.property == property)
					{
						if (idx == 0) return obj;
						--idx;
					}
				}
			}
		}
		return nullptr;
	}


	Object* Object::getParent() const
	{
		Object* parent = nullptr;
		for (auto& connection : scene.m_connections)
		{
			if (connection.from == id)
			{
				Object* obj = scene.m_object_map.find(connection.to)->second.object;
				if (obj && obj->is_node && obj != this)
				{
					assert(parent == nullptr);
					parent = obj;
				}
			}
		}
		return parent;
	}


	IScene* load(const u8* data, int size, u64 flags, JobProcessor job_processor, void* job_user_ptr)
	{
		std::unique_ptr<Scene> scene(new Scene());
		scene->m_data.resize(size);
		memcpy(&scene->m_data[0], data, size);
		u32 version;

		const bool is_binary = size >= 18 && strncmp((const char*)data, "Kaydara FBX Binary", 18) == 0;
		OptionalError<Element*> root(nullptr);
		if (is_binary) {
			root = tokenize(&scene->m_data[0], size, version, scene->m_allocator);
			if (version < 6200)
			{
				Error::s_message = "Unsupported FBX file format version. Minimum supported version is 6.2";
				return nullptr;
			}
			if (root.isError())
			{
				Error::s_message = "";
				if (root.isError()) return nullptr;
			}
		}
		else {
			root = tokenizeText(&scene->m_data[0], size, scene->m_allocator);
			if (root.isError()) return nullptr;
		}

		scene->m_root_element = root.getValue();
		assert(scene->m_root_element);

		// if (parseTemplates(*root.getValue()).isError()) return nullptr;
		if (!parseConnections(*root.getValue(), scene.get())) return nullptr;
		if (!parseTakes(scene.get())) return nullptr;
		if (!parseObjects(*root.getValue(), scene.get(), flags, scene->m_allocator, job_processor, job_user_ptr)) return nullptr;
		parseGlobalSettings(*root.getValue(), scene.get());

		return scene.release();
	}


	const char* getError()
	{
		return Error::s_message;
	}


} // namespace ofbx
