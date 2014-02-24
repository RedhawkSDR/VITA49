/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK.
 *
 * REDHAWK is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Very basic support for working with VRL frames and VRT packets. This is a pure ANSI C
 *  include file that supports the minimum VRL and VRT functionality. There is no error checking
 *  and no user-friendly features here, nor is there specific support for the various VRT packet
 *  types. If any of those features are desired, the C++ (or Java) VRT libraries are full-featured
 *  and support all of that. <br>
 *  <br>
 *   This include file assumes the user if relatively familiar with the VITA 49.0 (VRT) and
 *  VITA 49.1 (VRL) specifications. Unlike the C++ and Java VRT libraries which to work on the
 *  fields in the packet in-place, this will pack and unpack all of the header/trailer values into
 *  C structures. It is extremely important to note that the the structures defined here are NOT
 *  binary compatible with the VRT packet or VRL frame (i.e. it is not possible to use memcpy to
 *  read/write them to/from a buffer). The inline methods provided here handle the necessary
 *  read/write functions and applicable conversions (including endian'ness conversions required
 *  on little-endian systems). <br>
 *  <br>
 *   Note that the names used in this include file *should* be distinct from any of the names
 *  used within the include files of the C++ libraries. This allows for an easy transition path
 *  from VRT Lite for C to VRT for C++. <br>
 *  <br>
 *   There are a few frequently-used utility methods present here. They are defined as inline
 *  functions rather than preprocessor macros since most compilers (including GCC) will produce
 *  code that is just as fast, but will be able to provide enhanced compile-time error checking. <br>
 *  <br>
 *   This file can be included with and built under any of the following:
 *  <pre>
 *    1. GCC for ANSI C:        gcc -Wall -ansi -pedantic [file.c]
 *    2. Intel C for ANSI C:    icc -Wall -ansi [file.c]
 *    3. GCC for C++:           g++ -Wall [file.cc]
 *    4. Intel C for C++:       icc -Wall [file.cc]
 *  </pre>
 *  @author         
 */
#ifndef VRT_LITE_H
#define VRT_LITE_H


/*------------------------------------------------------------------------------------------------*/
/* INCLUDES AND MISC. DEFINITIONS TO SUPPORT VARIOUS COMPILERS                                    */
/*------------------------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <string.h>
#include <endian.h>
#include <stdlib.h>


#ifndef BYTE_ORDER /* Intel compiler omits these when -ansi is specified */
# define LITTLE_ENDIAN  __LITTLE_ENDIAN
# define BIG_ENDIAN     __BIG_ENDIAN
# define BYTE_ORDER     __BYTE_ORDER
#endif


#ifndef NULL
# define NULL ((void*)0)
#endif

#define __STDC_LIMIT_MACROS  1 /* Required to include the __INT64_C(..) macro in stdint.h */
#define __STDC_FORMAT_MACROS 1 /* Required to include the PRI?64 constants in inttypes.h */
#include <stdint.h>
#include <inttypes.h>

/* This next section copied from stdint.h */
# if __WORDSIZE == 64
#  define __INT64_C(c)	c ## L
#  define __UINT64_C(c)	c ## UL
# else
#  define __INT64_C(c)	c ## LL
#  define __UINT64_C(c)	c ## ULL
# endif

/** __INTEL_COMPILER specifies the icc version in the form "xxyz" (I think) where xx is the major
 *  version, y is the minor version, and z is the patch version or zero. For version 10.1 the value
 *  is 1010. Here we define a similar one for GCC using the form xxyyzz form suggested in section
 *  3.7.2 "Common Predefined Macros" of the GCC documentation. Note that icc will usually define
 *  __GNUC__ for compatibility with gcc, here we specifically guard against that in the second
 *  definition (which is used to work around GCC-specific issues).
 */
#ifdef __GNUC__
# define __GCC_VERSION ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
# ifndef __INTEL_COMPILER
#  define __GNU_COMPILER __GCC_VERSION
# endif
#endif

/*  Define a few macros that will allow us to support ANSI C (strict, pre C 99), more modern C,
 * and also C++ without any errors or warnings. __STRICT_ANSI__ and __NO_INLINE__ are standard GCC
 * definitions indicating strict ANSI C 89 (inline not supported) and C 99 with optimizations
 * turned off, respectively.
 */
#if defined(__cplusplus)
# define ____inline                     inline
# define ____extern_c                   extern "C"
# define ____reinterpret_cast(type,val) reinterpret_cast<type>(val)
#elif defined(__STRICT_ANSI__) ||  defined(__NO_INLINE__)
# define ____inline                     /* no inline */
# define ____extern_c                   /* no extern "C" */
# define ____reinterpret_cast(type,val) val
#else
# define ____inline                     inline
# define ____extern_c                   /* no extern "C" */
# define ____reinterpret_cast(type,val) val
#endif


/*------------------------------------------------------------------------------------------------*/
/* VRL AND VRT CONSTANTS                                                                          */
/*------------------------------------------------------------------------------------------------*/


/**  A standard data packet with no stream identifier.     */ 
#define PACKET_TYPE_UnidentifiedData    0
/**  A standard data packet (with stream identifier).      */
#define PACKET_TYPE_Data                1
/**  An extension data packet with no stream identifier.   */
#define PACKET_TYPE_UnidentifiedExtData 2
/**  An extension data packet (with stream identifier).    */
#define PACKET_TYPE_ExtData             3
/**  A standard context packet (with stream identifier).   */
#define PACKET_TYPE_Context             4
/**  An extension context packet (with stream identifier). */
#define PACKET_TYPE_ExtContext          5



/**  No integral time code. */
#define INTEGER_MODE_None  0
/**  UTC time code.         */
#define INTEGER_MODE_UTC   1
/**  GPS time code.         */
#define INTEGER_MODE_GPS   2
/**  Other time code.       */
#define INTEGER_MODE_Other 3



/**  No fractional time code.                    */
#define FRACTIONAL_MODE_None             0
/**  Sample count.                               */
#define FRACTIONAL_MODE_SampleCount      1
/**  Real-Time time stamp (picosecond accuracy). */
#define FRACTIONAL_MODE_RealTime         2
/**  Free-running count.                         */
#define FRACTIONAL_MODE_FreeRunningCount 3



/**  This is the 32-bit FWA (frame alignment word) marking the start of a VRLP frame. */
#define _VRL_FAW = 0x56524C50;

/**  This is the special code that can be used in place of the CRC32 in cases where
 *  a CRC is not considered unnecessary, and not included.
 */
#define _NO_CRC = 0x56454E44;

/**  Mask and check value used for determining if a given class ID matches one used for a
    {@link StandardDataPacket}. This is intended to be used as follows:
    <pre>
      if ((classID & CLASS_ID_MASK) == CLASS_ID_CHECK) {
        // Found a StandardDataPacket
        ...
      }
    </pre>
     Note that this is not a guarentee that the class ID is valid, but it says that the class
    ID is either one for a {@link StandardDataPacket} or it is invalid another defined packet
    class. <br>
    <br>
     Valid standard entries are in the form:
    <pre>
      "FF-FF-0D:0000.xxyz"    z = Data type:                               '0' =  4-bit signed int
                                                '1' =  8-bit signed int    '2' = 16-bit signed int
                                                '4' = 32-bit signed int    '8' = 64-bit signed int
                                                'F' = 32-bit IEEE float    'D' = 64-bit IEEE double

                              y = Real/Complex: '1' = Real/Scalar          '2' = Complex (cartesian)

                             xx = Frame Size:    0  = non-framed data (or frame size 1)
                                                 N  = framed data with frame size N*256
    </pre>
 */
#define STD_TYPES_MASK          __UINT64_C(0xFFFFFFFFFF0000)
#define STD_TYPES_CHECK         __UINT64_C(0xFFFF0D00000000)

/**  The check value for the set of standard types used in class identifiers for basic data packets.
 *  Usage
 *  <pre>
 *    switch (classID & STD_TYPES_TYPE_MASK) {
 *      case STD_TYPES_ScalarInt4: ...
 *      case STD_TYPES_ScalarInt8: ...
 *       :
 *    }
 *  </pre>
 */
#define STD_TYPES_TYPE_MASK     __UINT64_C(0xFFFFFFFFFF00FF)

#define STD_TYPES_ScalarInt4    __UINT64_C(0xFFFF0D00000010) /*  Scalar  4-bit signed integer. */
#define STD_TYPES_ScalarInt8    __UINT64_C(0xFFFF0D00000011) /*  Scalar  8-bit signed integer. */
#define STD_TYPES_ScalarInt16   __UINT64_C(0xFFFF0D00000012) /*  Scalar 16-bit signed integer. */
#define STD_TYPES_ScalarInt32   __UINT64_C(0xFFFF0D00000014) /*  Scalar 32-bit signed integer. */
#define STD_TYPES_ScalarInt64   __UINT64_C(0xFFFF0D00000018) /*  Scalar 64-bit signed integer. */
#define STD_TYPES_ScalarFloat   __UINT64_C(0xFFFF0D0000001F) /*  Scalar 32-bit IEEE float. */
#define STD_TYPES_ScalarDouble  __UINT64_C(0xFFFF0D0000001D) /*  Scalar 64-bit IEEE double. */
#define STD_TYPES_ComplexInt4   __UINT64_C(0xFFFF0D00000020) /*  Complex (cartesian)  4-bit signed integer. */
#define STD_TYPES_ComplexInt8   __UINT64_C(0xFFFF0D00000021) /*  Complex (cartesian)  8-bit signed integer. */
#define STD_TYPES_ComplexInt16  __UINT64_C(0xFFFF0D00000022) /*  Complex (cartesian) 16-bit signed integer. */
#define STD_TYPES_ComplexInt32  __UINT64_C(0xFFFF0D00000024) /*  Complex (cartesian) 32-bit signed integer. */
#define STD_TYPES_ComplexInt64  __UINT64_C(0xFFFF0D00000028) /*  Complex (cartesian) 64-bit signed integer. */
#define STD_TYPES_ComplexFloat  __UINT64_C(0xFFFF0D0000002F) /*  Complex (cartesian) 32-bit IEEE float. */
#define STD_TYPES_ComplexDouble __UINT64_C(0xFFFF0D0000002D) /*  Complex (cartesian) 64-bit IEEE double. */


/*------------------------------------------------------------------------------------------------*/
/* VRL AND VRT STRUCTURE DEFINITIONS                                                              */
/*------------------------------------------------------------------------------------------------*/


/**  Describes the fields that are included in the VRL header, excluding the
 *  frame alignment word which is a fixed value equal to _VRL_FAW.
 */
typedef struct {
  /**  The frame count mod 4096. */
  unsigned int frame_count : 12;
  
  /**  The frame size in 32-bit words. */
  unsigned int frame_size  : 20;
} vrl_header_t;



/**  Describes the fields that are included in the VRL trailer. This
 *  structure is binary-compatible with the trailer and can be read/written
 *  from/to a packet using a standard memcpy. Note that when creating a new
 *  instance, the crc must be initialized to NO_CRC unless a CRC is being
 *  computed.
 */
typedef struct {
  /**  The CRC32 value if a CRC is specified or NO_CRC if it is not specified. */
  __int32_t crc;
} vrl_trailer_t;


/**  Describes the fields that *can* be included in the VRT header. The
 *  actual header will drop any unused fields. Use the write_vrt_header(..)
 *  to write this information to a packet and use read_vrt_header(..) to read
 *  this information from a packet. If creating a new struct, always initialize
 *  it with the following to ensure that any reserved bits are set to 0:
 *  <pre>
 *    vrt_header_info_t h;
 *    memset(&h, 0, sizeof(vrt_header_info_t));
 *  </pre>
 */
typedef struct {
  /**  The packet type (one of PACKET_TYPE_UnidentifiedData, PACKET_TYPE_Data,
   *  PACKET_TYPE_UnidentifiedExtData, PACKET_TYPE_ExtData, PACKET_TYPE_Context,
   *  or PACKET_TYPE_ExtContext)
   */
  unsigned int packet_type                : 4;
  
  /**  Is the class identifier present (1) or not (0)? */
  unsigned int class_identifier_present   : 1;
  
  /**  Is the trailer present (1) or not (0)? Must be 0 for non-data packets. */
  unsigned int trailer_present            : 1;
  
  /**  Reserved bit, must be 0 for all packet types. */
  unsigned int __reserved_bit             : 1;
  
  /**  Reserved bit, must be 0 for all data packets. */
  unsigned int time_stamp_mode            : 1;
  
  /**  Time stamp integer mode (one of INTEGER_MODE_None, INTEGER_MODE_UTC,
   *      INTEGER_MODE_GPS, or INTEGER_MODE_Other).
   */
  unsigned int time_stamp_integer_mode    : 2;
  
  /**  Time stamp fractional mode (one of FRACTIONAL_MODE_None,
   *      FRACTIONAL_MODE_SampleCount, FRACTIONAL_MODE_RealTime,
   *      or FRACTIONAL_MODE_FreeRunningCount).
   */
  unsigned int time_stamp_fractional_mode : 2;

  /**  The packet count (mod 16). */
  unsigned int packet_count               : 4;

  /**  The packet size in 32-bit words. */
  __uint16_t packet_size;

  /**  The stream identifier. This is ignored if packet_type is set to
   *  PACKET_TYPE_UnidentifiedData or PACKET_TYPE_UnidentifiedExtData.
   */
  __uint32_t stream_identifier;

  /**  The class identifier. This is ignored if class_identifier_present
   *  is zero. Note that the first 8 bits of the class identifier are
   *  reserved and must be set to 0.
   */
  __uint64_t class_identifier;

  /**  The integer time stamp. The meaning of this depends on the value
   *  of time_stamp_integer_mode.
   */
  __uint32_t time_stamp_integer;

  /**  The fractional time stamp. The meaning of this depends on the value
   *  of time_stamp_fractional_mode.
   */
  __uint64_t time_stamp_fractional;
} vrt_header_t;



/**  Describes the fields that are included in the VRT trailer. This
 *  structure is binary-compatible with the trailer and can be read/written
 *  from/to a packet using a standard memcpy. Within this structure, each
 *  of the "_indicator" values is only relevent if the cooresponding "_enable"
 *  is set to 1. This results in three distinct states for each flag:
 *  <pre>
 *    _enable=0, _indicator=?    =>  n/a
 *    _enable=1, _indicator=0    =>  FALSE
 *    _enable=1, _indicator=1    =>  TRUE
 *  </pre>
 *   Similarly the associated_packet_count is only valid if the
 *  associated_packet_count_enable is set to 1.
 */
typedef struct {
  /**  Calibrated time stamp enable. */
  unsigned int calibrated_time_stamp_enable    : 1;
  /**  Valid data enable. */
  unsigned int valid_data_enable               : 1;
  /**  Reference lock enable. */
  unsigned int reference_lock_enable           : 1;
  /**  AGC/MGC enable. */
  unsigned int agc_mgc_enable                  : 1;
  /**  Signal detected enable. */
  unsigned int signal_detected_enable          : 1;
  /**  Inverted spectrum enable. */
  unsigned int inverted_spectrum_enable        : 1;
  /**  Over range enable. */
  unsigned int over_range_enable               : 1;
  /**  Sample loss enable. */
  unsigned int sample_loss_enable              : 1;
  /**  User-defined bit 11 enable. */
  unsigned int bit11_enable                    : 1;
  /**  User-defined bit 10 enable. */
  unsigned int bit10_enable                    : 1;
  /**  User-defined bit 9 enable. */
  unsigned int bit9_enable                     : 1;
  /**  User-defined bit 8 enable. */
  unsigned int bit8_enable                     : 1;
  
  /**  Calibrated time stamp indicator. */
  unsigned int calibrated_time_stamp_indicator : 1;
  /**  Valid data indicator. */
  unsigned int valid_data_indicator            : 1;
  /**  Reference lock indicator. */
  unsigned int reference_lock_indicator        : 1;
  /**  AGC/MGC indicator. */
  unsigned int agc_mgc_indicator               : 1;
  /**  Signal detected indicator. */
  unsigned int signal_detected_indicator       : 1;
  /**  Inverted spectrum indicator. */
  unsigned int inverted_spectrum_indicator     : 1;
  /**  Over range indicator. */
  unsigned int over_range_indicator            : 1;
  /**  Sample loss indicator. */
  unsigned int sample_loss_indicator           : 1;
  /**  User-defined bit 11 indicator. */
  unsigned int bit11_indicator                 : 1;
  /**  User-defined bit 10 indicator. */
  unsigned int bit10_indicator                 : 1;
  /**  User-defined bit 9 indicator. */
  unsigned int bit9_indicator                  : 1;
  /**  User-defined bit 8 indicator. */
  unsigned int bit8_indicator                  : 1;

  /**   Associated packet count enable. */
  unsigned int associated_packet_count_enable  : 1;
  /**   Associated packet count. */
  unsigned int associated_packet_count         : 7;
} vrt_trailer_t;


/*------------------------------------------------------------------------------------------------*/
/* VRL FUNCTION PROTOTYPES                                                                        */
/*------------------------------------------------------------------------------------------------*/


/**  Writes the trailer information to the packet. No error checking is
 *  performed on the header values being written.
 *  @param packet Pointer to the point in the packet where the trailer belongs.
 *  @param header Pointer to the trailer struct.
 */
____extern_c void write_vrl_trailer (void *dest, const vrl_trailer_t *src);

/**  Read the trailer information from the packet.
 *  @param packet Pointer to the point in the packet where the trailer belongs.
 *  @param header Pointer to the header struct.
 */
____extern_c void read_vrl_trailer (const void *src, vrl_trailer_t *dest);
/**  Writes the header information to the packet. No error checking is
 *  performed on the header values being written.
 *  @param packet Pointer to the packet (usually a byte array).
 *  @param header Pointer to the header struct.
 */
____extern_c void write_vrl_header (void *dest, const vrl_header_t *src);

/**  Read the header information from the packet.
 *  @param packet Pointer in the to the location packet (usually a byte array).
 *  @param header Pointer to the header struct.
 */
____extern_c void read_vrl_header (const void *src, vrl_header_t *dest);

/**  Gets the length of the VRL frame in bytes. This is a special method that will work on an
 *  incompletely-read frame since it only looks at the first 8 bytes.
 */
____extern_c size_t get_vrl_frame_length (const void *header);

/**  Gets the length of the VRL frame in bytes. */
____extern_c size_t get_vrl_frame_len (const vrl_header_t *header);

/**  Gets a pointer to the start of the next VRT packet in a VRL frame. This is intended to be
 *  used for iterating over the packets in a VRL frame. Example usage:
 *  <pre>
 *    void *frame;
 *    void *packet;
 *
 *    ...
 *
 *    packet = NULL;
 *    while ((packet = next_vrt_packet(frame, packet)) != NULL) {
 *      ...
 *    }
 *  </pre>
 *  @param frame    The start of the VRL frame.
 *  @param previous The previous packet read from the frame (NULL if n/a)
 *  @return The next packet or NULL if there are no more.
 */
____extern_c void *next_vrt_packet (const void *frame, const void *previous);

/**  Gets a pointer to the start of the next VRT packet in a VRL frame. This is intended to be
 *  used for iterating over the packets in a VRL frame.
 *  Example usage:
 *  <pre>
 *    void         *frame          = ...;
 *    void         *packet         = NULL;
 *    vrl_header_t  frame_header;   // will be initialized on first call
 *    vrt_header_t  packet_header;  // will be initialized on first call
 *    ...
 *
 *    while ((next_vrt_packet(frame, &frame_header, &packet, &packet_header) != NULL) {
 *      // packet and packet_header now set for the packet
 *      ...
 *    }
 *  </pre>
 *  @param frame         (IN)  The start of the VRL frame.
 *  @param frame_header  (I/O) The header for the frame (will be initialized if *packet==NULL).
 *  @param packet        (I/O) Pointer to the location of the previous packet read from the frame
 *                             (*packet=NULL on first call).
 *  @param packet_header (I/O) Pointer to the packet header (write only during first call).
 *  @return The next packet or NULL if there are no more.
 */
____extern_c void *next_vrt_packet2 (const void *frame,   vrl_header_t *frame_header,
                                     void **packet, vrt_header_t *packet_header);

/**  Checks to see if the given buffer contains a VRL frame. Note that this only checks to see
 *  that the size is at least four and that the first four bytes match the VRL's frame alignment
 *  word. This does not check to see if the frame is valid.
 *  @param unknown The buffer of an unknown type.
 *  @param len     The length of the buffer. For a valid VRL frame this must
 *                 be 12+.
 *  @return TRUE if it is a VRL frame, FALSE otherwise.
 */
____extern_c int is_vrl_frame (const void *unknown, size_t len);


/*------------------------------------------------------------------------------------------------*/
/* VRT FUNCTION PROTOTYPES                                                                        */
/*------------------------------------------------------------------------------------------------*/


/**  Writes the header information to the packet. No error checking is
 *  performed on the header values being written.
 *  @param packet Pointer to the packet (usually a byte array).
 *  @param header Pointer to the header struct.
 */
____extern_c void write_vrt_header (void *dest, const vrt_header_t *src);

/**  Read the header information from the packet. Any unused fields will be
 *  set to zero.
 *  @param packet Pointer in the to the location packet (usually a byte array).
 *  @param header Pointer to the header struct.
 */
____extern_c void read_vrt_header (const void *src, vrt_header_t *dest);

/**  Writes the trailer information to the packet. No error checking is
 *  performed on the header values being written.
 *  @param packet Pointer to the point in the packet where the trailer belongs.
 *  @param header Pointer to the trailer struct.
 */
____extern_c void write_vrt_trailer (void *dest, const vrt_trailer_t *src);

/**  Read the trailer information from the packet.
 *  @param packet Pointer to the point in the packet where the trailer belongs.
 *  @param header Pointer to the header struct.
 */
____extern_c void read_vrt_trailer (const void *src, vrt_trailer_t *dest);

/**  Gets the length of the VRT packet in bytes. This is a special method that will work on an
 *  incompletely-read packet since it only looks at the first 4 bytes.
 */
____extern_c size_t get_vrt_packet_length (const void *packet);

/**  Gets the length of the VRT packet in bytes. */
____extern_c size_t get_vrt_packet_len (const vrt_header_t *header);

/**  Gets the length of the VRT header in bytes. */
____extern_c size_t get_vrt_header_len (const vrt_header_t *header);

/**  Gets the length of the VRT trailer in bytes. */
____extern_c size_t get_vrt_trailer_len (const vrt_header_t *header);

/**  Gets the length of the VRT payload in bytes. */
____extern_c size_t get_vrt_payload_len (const vrt_header_t *header);

/**  Sets the length of the VRT payload in bytes.
 *  @param header The applicable VRT header.
 *  @param len    The payload length.
 *  @return true if the length is ok, and false if it is not.
 */
____extern_c int set_vrt_payload_len (vrt_header_t *header, size_t len);

/**  Gets a pointer to the start of the payload in a VRT packet. */
____extern_c void *get_vrt_payload_ptr (void *packet, const vrt_header_t *header);

/**  Gets a pointer to the start of the trailer in a VRT packet. Returns NULL if there is
 *  no trailer.
 */
____extern_c void *get_vrt_trailer_ptr (void *packet, const vrt_header_t *header);


/*------------------------------------------------------------------------------------------------*/
/* IMPLEMENTATION CODE                                                                            */
/*------------------------------------------------------------------------------------------------*/


/**  Internal use only: Prevents errors with C++ which restricts math on void pointers. */
static ____inline void *____ptr_add (const void *ptr, int to_add) {
  const char *p = ____reinterpret_cast(const char*, ptr);
  return (void*)(p + to_add);
}
/**  Internal use only: Byte-swap and copy for 2-byte value (src!=dest). */
static ____inline void ____copy2 (const void *src, void *dest) {
  const char *s = ____reinterpret_cast(const char*, src);
        char *d = ____reinterpret_cast(      char*, dest);
#if (BYTE_ORDER == BIG_ENDIAN)
  d[0]=s[0]; d[1]=s[1];
#else
  d[0]=s[1]; d[1]=s[0];
#endif
}
/**  Internal use only: Byte-swap and copy for 4-byte value (src!=dest). */
static ____inline void ____copy4 (const void *src, void *dest) {
  const char *s = ____reinterpret_cast(const char*, src);
        char *d = ____reinterpret_cast(      char*, dest);
#if (BYTE_ORDER == BIG_ENDIAN)
  d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3];
#else
  d[0]=s[3]; d[1]=s[2]; d[2]=s[1]; d[3]=s[0];
#endif
}
/**  Internal use only: Byte-swap and copy for 8-byte value (src!=dest). */
static ____inline void ____copy8 (const void *src, void *dest) {
  const char *s = ____reinterpret_cast(const char*, src);
        char *d = ____reinterpret_cast(      char*, dest);
#if (BYTE_ORDER == BIG_ENDIAN)
  d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3];
  d[4]=s[4]; d[5]=s[5]; d[6]=s[6]; d[7]=s[7];
#else
  d[0]=s[7]; d[1]=s[6]; d[2]=s[5]; d[3]=s[4];
  d[4]=s[3]; d[5]=s[2]; d[6]=s[1]; d[7]=s[0];
#endif
}

/**  Internal use only. */
static ____inline void copy_vrt_header (const vrt_header_t *header, void *dest, const void *src) {
  const char *s = ____reinterpret_cast(const char*, src);
        char *d = ____reinterpret_cast(      char*, dest);
  d[0] = s[0]; /* bits */
  d[1] = s[1]; /* bits */
  src  = ____ptr_add(src,  2);
  dest = ____ptr_add(dest, 2);
  ____copy2(src,dest); src=____ptr_add(src,2); dest=____ptr_add(dest,2);

  if ((header->packet_type != PACKET_TYPE_UnidentifiedData) &&
      (header->packet_type != PACKET_TYPE_UnidentifiedExtData)) {
    ____copy4(src,dest); src=____ptr_add(src,4); dest=____ptr_add(dest,4);
  }
  if (header->class_identifier_present  ) { ____copy8(src,dest); src=____ptr_add(src,8); dest=____ptr_add(dest,8); }
  if (header->time_stamp_integer_mode   ) { ____copy4(src,dest); src=____ptr_add(src,4); dest=____ptr_add(dest,4); }
  if (header->time_stamp_fractional_mode) { ____copy8(src,dest); src=____ptr_add(src,8); dest=____ptr_add(dest,8); }
}


____inline void write_vrl_trailer (void *dest, const vrl_trailer_t *src) {
  ____copy4(src, dest);
}
____inline void read_vrl_trailer (const void *src, vrl_trailer_t *dest) {
  ____copy4(src, dest);
}
____inline void write_vrt_header (void *dest, const vrt_header_t *src) {
  copy_vrt_header(src, dest, src);
}
____inline void read_vrt_header (const void *src, vrt_header_t *dest) {
  memset(dest, 0, sizeof(vrt_header_t));
  copy_vrt_header(dest, dest, src);
}
____inline void write_vrt_trailer (void *dest, const vrt_trailer_t *src) {
  memcpy(dest, src, 4);
}
____inline void read_vrt_trailer (const void *src, vrt_trailer_t *dest) {
  memcpy(dest, src, 4);
}
____inline size_t get_vrt_packet_len (const vrt_header_t *header) {
  return ((size_t)header->packet_size) << 2;
}
____inline size_t get_vrt_packet_length (const void *packet) {
  const char *buf = ____reinterpret_cast(const char*, packet);
  __uint32_t buf2 = buf[2];
  __uint32_t buf3 = buf[3];
  return (buf2 << 10) | (buf3 << 2);
}
____inline size_t get_vrt_header_len (const vrt_header_t *header) {
  size_t len = 4;

  if ((header->packet_type != PACKET_TYPE_UnidentifiedData) &&
      (header->packet_type != PACKET_TYPE_UnidentifiedExtData)) {
    len += 4;
  }
  if (header->class_identifier_present  ) len += 8;
  if (header->time_stamp_integer_mode   ) len += 4;
  if (header->time_stamp_fractional_mode) len += 8;
  return len;
}
____inline size_t get_vrt_trailer_len (const vrt_header_t *header) {
  return (header->trailer_present)? 4 : 0;
}
____inline size_t get_vrt_payload_len (const vrt_header_t *header) {
  size_t packet_len  = get_vrt_packet_len(header);
  size_t header_len  = get_vrt_header_len(header);
  size_t trailer_len = get_vrt_trailer_len(header);
  
  return packet_len - header_len - trailer_len;
}
____inline int set_vrt_payload_len (vrt_header_t *header, size_t len) {
  size_t header_len  = get_vrt_header_len(header);
  size_t trailer_len = get_vrt_trailer_len(header);
  size_t packet_len  = len + header_len + trailer_len;
  size_t word_len    = packet_len >> 2; /* length in 32-bit words */
  int    ok         = ((packet_len & 0x3) == 0) && (word_len <= 0xFFFF);
  if (ok) header->packet_size = word_len;
  return ok;
}
____inline void *get_vrt_payload_ptr (void *packet, const vrt_header_t *header) {
  vrt_header_t hdr;

  if (header == NULL) {
    read_vrt_header(packet, &hdr);
    header = &hdr;
  }
  return ____ptr_add(packet, get_vrt_header_len(header));
}
____inline void *get_vrt_trailer_ptr (void *packet, const vrt_header_t *header) {
  vrt_header_t hdr;

  if (header == NULL) {
    read_vrt_header(packet, &hdr);
    header = &hdr;
  }
  return (header->trailer_present)? ____ptr_add(packet, get_vrt_packet_len(header)-4)
                                  : NULL;
}
____inline void write_vrl_header (void *dest, const vrl_header_t *src) {
  char *buf = ____reinterpret_cast(char*, dest);
  __uint32_t count = src->frame_count;
  __uint32_t size  = src->frame_size;

  buf[0] = (char)  (count >> 4);
  buf[1] = (char)(((count << 4) & 0xF0) | ((size >> 16) & 0x0F));
  buf[2] = (char)  (size >> 8);
  buf[3] = (char)  (size);
}
____inline void read_vrl_header (const void *src, vrl_header_t *dest) {
  const char *buf = ____reinterpret_cast(const char*, src);
  __uint32_t buf4 = buf[4];
  __uint32_t buf5 = buf[5];
  __uint32_t buf6 = buf[6];
  __uint32_t buf7 = buf[7];
  
  dest->frame_count = ((buf4 << 4) & 0xFF0)
                    | ((buf5 >> 4) & 0x00F);
  dest->frame_size  = ((buf5 << 4) & 0xF0000)
                    | ((buf6 << 8) & 0x0FF00)
                    | ((buf7     ) & 0x000FF);
}
____inline size_t get_vrl_frame_len (const vrl_header_t *header) {
  return ((size_t)header->frame_size) << 2;
}
____inline size_t get_vrl_frame_length (const void *frame) {
  const char *buf = ____reinterpret_cast(const char*, frame);
  __uint32_t buf5 = buf[5];
  __uint32_t buf6 = buf[6];
  __uint32_t buf7 = buf[7];

  return ((buf5 << 4) & 0xF0000)
       | ((buf6 << 8) & 0x0FF00)
       | ((buf7     ) & 0x000FF);
}
____inline void *next_vrt_packet2 (const void *frame,   vrl_header_t *frame_header,
                                     void **packet, vrt_header_t *packet_header) {
  if (*packet == NULL) {
    read_vrl_header(frame, frame_header);
    *packet = ____ptr_add(frame, 8);
  }
  else {
    *packet = ____ptr_add(frame, get_vrt_packet_len(packet_header)<<2);
  }
  
  if (*packet >= ____ptr_add(frame, get_vrl_frame_len(frame_header)-4)) {
    *packet = NULL; /* no more packets */
    memset(packet_header, 0, sizeof(vrt_header_t));
  }
  else {
    read_vrt_header(*packet, packet_header);
  }

  return *packet;
}
____inline void *next_vrt_packet (const void *frame, const void *previous) {
  vrl_header_t  frame_header;
  vrt_header_t  packet_header;
  void         *packet         = (void*)previous;

  read_vrl_header(frame, &frame_header);
  if (previous != NULL) read_vrt_header(previous, &packet_header);

  return next_vrt_packet2(frame, &frame_header, &packet, &packet_header);
}
____inline int is_vrl_frame (const void *unknown, size_t len) {
  const char *buf = ____reinterpret_cast(const char*, unknown);
  return (len >= 4) && (buf[0] == 'V') && (buf[1] == 'R') && (buf[2] == 'L') && (buf[3] == 'P');
}

#endif /* VRT_LITE_H */
