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
 */

#ifndef _VRTObject_h
#define _VRTObject_h

#include <cctype>
#include <climits>
#include <exception>
#include <ostream>
#include <string>
#include <stdarg.h>
#include <typeinfo>
#include <vector>
#include <iostream>  // required for std::cerr
#include <limits>    // required for numeric_limits(..) on GCC4.4/libc6 2.11.1
#include <stdio.h>   // required for printf(..) on GCC4.4/libc6 2.11.1


#define __STDC_LIMIT_MACROS  1 /* Required to include the __INT64_C(..) macro in stdint.h */
#define __STDC_FORMAT_MACROS 1 /* Required to include the PRI?64 constants in inttypes.h */
#include <stdint.h>
#include <inttypes.h>

/* This next section copied from stdint.h */
# if (defined(__APPLE__) && defined(__MACH__))
// For some reason (probably historical) GCC 4.2 on OS X (APPLE+MACH) uses "LL" as the suffix
// regardless of architecture. To be honest, this makes more sense than the "normal" usage.
#  define __INT64_C(c)  c ## LL
#  define __UINT64_C(c) c ## ULL
# elif __WORDSIZE == 64
#  define __INT64_C(c)  c ## L
#  define __UINT64_C(c) c ## UL
# else
#  define __INT64_C(c)  c ## LL
#  define __UINT64_C(c) c ## ULL
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

/** This will enable/disable the automatic printing of all (non-null) exceptions when they
 *  are instantiated. This can be useful for debugging when an error is suspected within
 *  the exception handling code of either this library or the caller.
 */
#ifndef PRINT_ALL_EXCEPTIONS
# define PRINT_ALL_EXCEPTIONS 0
#endif

/** This will enable/disable the automatic inclusion of stack traces with exceptions
 *  when they are instantiated. Note that this is disabled by default since it is
 *  compiler-specific and requires additional build flags (e.g. -rdynamic with GCC).
 */
#ifndef INCLUDE_STACK_TRACE
# define INCLUDE_STACK_TRACE 1
#endif

/** Enable GCC and Intel attributes/pragmas as appropriate. */
#if defined(DOXYGEN)
  // doxygen gets confused by _Pragma(..) (see doxygen bug 643056) so
  // we set NO_PRAGMAS=1 when using doxygen to avoid issues. This is
  // not intended as a genearl-purpose flag, only one we use to work
  // around this issue.
# define _GCC_Pragma(x)   // ignore
# define _Intel_Pragma(x) // ignore
# define __attribute__(x)
# define __intelattr__(x)
#elif defined(__INTEL_COMPILER) && defined(__GNUC__)
  // Although the Intel compilers support GCC's __attribute__(..) on
  // Linux (and similar) platforms, their support for it is limited.
  // As a result we use __intelattr__(..) as a stand-in where it may
  // be appropriate. Blanket use of all of GCC's __attribute__(..)
  // flags simply gets us a lot of unnecessary warnings.
# define _GCC_Pragma(x)
# define _Intel_Pragma(x) _Pragma(x)
# define __attribute__(x)
# define __intelattr__(x) __attribute__(x)
#elif defined(__INTEL_COMPILER)
# define _GCC_Pragma(x)
# define _Intel_Pragma(x) _Pragma(x)
# define __attribute__(x)
# define __intelattr__(x)
#elif defined(__GNUC__)
# define _GCC_Pragma(x)   _Pragma(x)
# define _Intel_Pragma(x)
# define __attribute__(x) __attribute__(x)
# define __intelattr__(x)
#else
  // Disable attributes/pragmas where we don't know compiler
# define _GCC_Pragma(x)
# define _Intel_Pragma(x)
# define __attribute__(x)
# define __intelattr__(x)
#endif

using namespace std;

namespace vrt {
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // typedefs and constants
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** A type representing a boolean value that could be _TRUE or _FALSE if set and _NULL if not
   *  set. The parallel in Java is the Boolean class which supports TRUE, FALSE and null.
   */
  enum boolNull { _FALSE=-1, _NULL=0, _TRUE=+1 };

  /** A pseudo-null value for an 8-bit integer. This value is equal to <tt>-128</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Byte</tt> class
   *  would be used which can hold any 8-bit integer or could be null.
   */
  const int8_t  INT8_NULL  = numeric_limits<char>::min();
  /** A pseudo-null value for a 16-bit integer. This value is equal to <tt>-32768</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Short</tt> class
   *  would be used which can hold any 16-bit integer or could be null.
   */
  const int16_t INT16_NULL = numeric_limits<int16_t>::min();
  /** A pseudo-null value for a 32-bit integer. This value is equal to <tt>-2147483648</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Integer</tt> class
   *  would be used which can hold any 32-bit integer or could be null.
   */
  const int32_t INT32_NULL = numeric_limits<int32_t>::min();
  /** A pseudo-null value for a 64-bit integer. This value is equal to <tt>-9223372036854775808</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Long</tt> class
   *  would be used which can hold any 64-bit integer or could be null.
   */
  const int64_t INT64_NULL = numeric_limits<int64_t>::min();
  /** A pseudo-null value for a 32-bit floating-point value. This value is equal to <tt>NaN</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Float</tt> class
   *  would be used which can hold any 32-bit floating-point (including NaN) or could be null. <br>
   *  <br>
   *  <i>Implementation Note: Within this header <tt>FLOAT_NAN</tt> is unused since any comparison
   *  with NaN will fail (per the IEEE 754). As a result the applicable <tt>isNull(..)</tt> function
   *  is implemented without reference to this constant.</i>
   */
  const float __attribute__((unused)) __intelattr__((unused)) FLOAT_NAN  = numeric_limits<float>::quiet_NaN();
  /** A pseudo-null value for a 64-bit floating-point value. This value is equal to <tt>NaN</tt>
   *  which is unlikely to be used in the normal course of events. In Java the <tt>Double</tt> class
   *  would be used which can hold any 64-bit floating-point (including NaN) or could be null. <br>
   *  <br>
   *  <i>Implementation Note: Within this header <tt>DOUBLE_NAN</tt> is unused since any comparison
   *  with NaN will fail (per the IEEE 754). As a result the applicable <tt>isNull(..)</tt> function
   *  is implemented without reference to this constant.</i>
   */
  const double __attribute__((unused)) __intelattr__((unused)) DOUBLE_NAN = numeric_limits<float>::quiet_NaN();

  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // VRTObject
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  namespace VRTObject_private {
    /** <b>Internal Use Only:</b> Remove name mangling from a VRTObject class name.
     *  Example:
     *  <pre>
     *    getClassName("N3vrt18BasicContextPacketE") = "vrt::BasicContextPacket"
     *  </pre>
     *  @param name The mangled name.
     *  @return The unmangled name.
     */
    string getClassName (const char *name);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // VRTObject
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** A basic root-level object definition. This definition is extremely similar to the Java
   *  <tt>Object</tt> class and allows subclasses to more-closely follow their Java counterparts.
   *  @author         
   */
  class VRTObject {
    /** Basic copy constructor. */
    public: VRTObject(const VRTObject &o) { }
    /** Basic no-argument constructor. */
    public: VRTObject() { }
    /** Basic destructor. */
    public: virtual ~VRTObject() { }

    /** Converts this class its string form. */
    public: virtual string toString () const;

    /** Tests this object for equality with another one. */
    public: virtual bool equals (const VRTObject &o) const;

    /** Gets the name of the class. */
    public: string getClassName () const;

    /** Gets the type_info for the class. */
    public: inline const type_info &getClass () const {
      return typeid(*this);
    }

    /** The == operator is the same as calling equals(..) */
    public: inline bool operator== (const VRTObject &o) const {
      return equals(o);
    }

    /** The == operator is the same as calling equals(..) */
    public: inline bool operator!= (const VRTObject &o) const {
      return !equals(o);
    }

    /** Is this object equal to null. */
    public: inline virtual bool isNullValue () const {
      return false;
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // VRTException
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** Allows a formatted exception to be thrown. This usage is deprecated and any use of
   *  <tt>VRTEXCEPTION((..))</tt> should be replaced with <tt>VRTException(..)</tt>.
   */
  #define VRTEXCEPTION(args) \
    _GCC_Pragma("GCC warning \"Use of VRTEXCEPTION((..)) is deprecated\"") \
    _Intel_Pragma("message \"Use of VRTEXCEPTION((..)) is deprecated\"") \
    VRTException args

  /** <b>Internal Use Only:</b> The value of <tt>errno</tt> converted to a
   *  <tt>char*</tt> string for use with the <tt>VRTException(..)</tt>
   *  constructors. Use of this may require inclusion of <tt>errno.h</tt>.

   */
  #define ERRNO_STR vrt::VRTException::getErrorMsgFor(errno).c_str()

  /** A basic exception definition. All exceptions thrown within this project will be an instance
   *  of this class. This differs from the Java version which allows much more differentiation among
   *  the exception types (e.g. <tt>NullPointerException</tt>, <tt>IllegalArgumentException</tt>, etc.).
   *  @author 
   */
  class VRTException : public virtual VRTObject, public virtual exception {
    private: string         message;     // The error message
    private: string         description; // Description of error with class name and message
    private: vector<string> backtrace;   // The back-trace (if available)

    /** Constructs an exception with the specified message. */
    public: VRTException (string msg) throw();

    /** Constructs a null exception. */
    public: VRTException () throw();

    /** Constructs an exception with the specified message that follows <tt>printf</tt> formatting rules. */
    _Intel_Pragma("__printf_args")
    public: VRTException (const char *fmt, ...) throw();
    // Tried adding __attribute__((format(printf,2,3))) to the above line to enable
    // additional complier checks, but GCC 4.4.5 didn't appear to handle it properly.
    // Suspect this may be related to GCC bug 47634 ("Incorrect checking of attribute
    // format printf on constructor of derived class with virtual base") which was
    // still open against GCC 4.4.5 as of July 2013 (and also affects GCC 4.4.6).
    // Also could be an issue with the use of "throw()". Suggest revisiting in a
    // later GCC with possible #if block to disable check on buggy versions.

    /** Constructs an exception with a message appropriate for the given errno. */
    public: VRTException (int errnum) throw();
    
    /** Basic destructor for the class. */
    public: ~VRTException () throw() { }
    
    /** <b>Internal Use Only:</b> Gets the error message that corresponds to the
     *  given <tt>errno</tt> value.
     */
    public: static string getErrorMsgFor (int errnum) throw();
    
    /** A variant of getClassName() that returns "UnknownExceptionType" rather
     *  than throwing an exception.
     */
    private: string _getClassName () const throw();

    /** Gets the user-defined error message. */
    public: string getMessage () const;

    /** Version of toString() without 'const' qualifier. */
    private: virtual string _toString ();

    public: virtual string toString () const;

    /** Is this object equal to null. */
    public: virtual bool isNullValue () const;

    /** This is identical to calling <tt>toString().c_str()</tt>. */
    public: virtual const char* what () const throw();

    /** Prints the "back trace" (also called the "stack trace") to the given
     *  output stream. Generally this will write to <tt>cerr</tt>.
     *  @param out The output stream to write to.
     */
    public: void printStackTrace (ostream &out=std::cerr) const;

    /** Gets the "back trace" (also called the "stack trace").
     *  @return A copy of the entries in the back-trace with the top entry in
     *          the zero'th slot.
     */
    public: vector<string> getStackTrace () const {
      return backtrace;
    }
  };

  /** The exception thrown when a <tt>checked_dynamic_cast</tt> fails.
   *  @author 
   */
  class ClassCastException : public virtual VRTException, public virtual bad_cast {
    /** Constructs an exception with the specified message. */
    public: ClassCastException (string msg) throw();

    /** Constructs a null exception. */
    public: ClassCastException () throw();
    
    /** Basic destructor for the class. */
    public: ~ClassCastException () throw() { }
    
    /** This is identical to calling <tt>toString().c_str()</tt>. */
    public: inline virtual const char* what () const throw() {
      // This function is here to avoid issues with ambiguous definitions from
      // both bad_cast and exception (parent to VRTException) defining what()
      return VRTException::what();
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Type casting methods
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** A variant of <tt>dynamic_cast&lt;T&gt;(..)</tt> that will throw a <tt>ClassCastException</tt>
   *  any time the cast fails thereby eliminating the need to check for both <tt>bad_cast</tt> and
   *  a return value of <tt>NULL</tt>.
   *  @param obj The object to cast.
   *  @return The cast version or NULL if the input is NULL.
   */
  template <typename T,typename C>
  T checked_dynamic_cast (C ptr) {
    if (ptr == NULL) return (T)NULL;
    T p = NULL;
    try {
      p = dynamic_cast<T>(ptr);
    }
    catch (bad_cast e) {
      p = NULL;
    }
    
    if (p == NULL) {
      const char* srcName  = typeid(C).name();
      const char* destName = typeid(T).name();
      string msg  = VRTObject_private::getClassName(srcName)+" can not be cast to "
                  + VRTObject_private::getClassName(destName);
      
      throw ClassCastException(msg);
    }
    return p;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // The isNull(..) Methods
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** Checks to see if a <tt>boolNull</tt> value matches <tt>_NULL</tt>. */
  inline bool isNull (boolNull         val) { return (val == _NULL     ); }
  /** Checks to see if an 8-bit integer value matches <tt>INT8_NULL</tt>. */
  inline bool isNull (int8_t           val) { return (val == INT8_NULL ); }
  /** Checks to see if a 16-bit integer value matches <tt>INT16_NULL</tt>. */
  inline bool isNull (int16_t          val) { return (val == INT16_NULL); }
  /** Checks to see if a 32-bit integer value matches <tt>INT32_NULL</tt>. */
  inline bool isNull (int32_t          val) { return (val == INT32_NULL); }
  /** Checks to see if a 64-bit integer value matches <tt>INT64_NULL</tt>. */
  inline bool isNull (int64_t          val) { return (val == INT64_NULL); }
  /** Checks to see if a 32-bit floating-point value matches <tt>FLOAT_NAN</tt>. Note that a
   *  simple <tt>if (val == FLOAT_NAN)</tt> check is invalid since an equality check will always
   *  return false if either value is NaN.
   */
  inline bool isNull (float            val) { return !(val == val); }
  /** Checks to see if a 64-bit floating-point value matches <tt>DOUBLE_NAN</tt>. Note that a
   *  simple <tt>if (val == FLOAT_NAN)</tt> check is invalid since an equality check will always
   *  return false if either value is NaN.
   */
  inline bool isNull (double           val) { return !(val == val); }
  /** Checks to see if a {@link VRTObject} value is null.
   *  This is the same as calling <tt>val.isNullValue()</tt>.
   */
  inline bool isNull (const VRTObject &val) { return val.isNullValue(); }
  /** Checks to see if a <tt>string</tt> value is null.
   *  This is the same as calling <tt>val.empty()</tt>.
   */
  inline bool isNull (const string    &val) { return val.empty(); }
  /** Checks to see if a <tt>wstring</tt> value is null.
   *  This is the same as calling <tt>val.empty()</tt>.
   */
  inline bool isNull (const wstring   &val) { return val.empty(); }
  
  /** Checks to see if an unsigned 8-bit integer value is null (it's not). */
  inline bool isNull (uint8_t          val) { return false; }
  /** Checks to see if a unsigned 16-bit integer value is null (it's not). */
  inline bool isNull (uint16_t         val) { return false; }
  /** Checks to see if a unsigned 32-bit integer value is null (it's not). */
  inline bool isNull (uint32_t         val) { return false; }
  /** Checks to see if a unsigned 64-bit integer value is null (it's not). */
  inline bool isNull (uint64_t         val) { return false; }

#if (defined(__APPLE__) && defined(__MACH__))
  // Under OS X the definition of 'size_t' differs slightly from 'uint64_t' so
  // we need to include this to avoid issues when an isNull(..) check is done
  // on a 'size_t', as may occur when a template is used (this is particularly
  // true for the test cases).
  
  /** Checks to see if a <tt>size_t</tt> value is null (it's not). */
  inline bool isNull (size_t           val) { return false; }
#endif

  /** Checks to see if a pointer value matches <tt>NULL</tt>. */
  inline bool isNull (void            *val) { return (val == NULL); }
  /** Checks to see if a <tt>string</tt> pointer is null. This is the same as
   *  calling <tt>val.empty()</tt> following a check for a null pointer.
   */
  inline bool isNull (const string    *val) { return (val == NULL) || val->empty(); }
  /** Checks to see if a <tt>wstring</tt> pointer is null. This is the same as
   *  calling <tt>val.empty()</tt> following a check for a null pointer.
   */
  inline bool isNull (const wstring   *val) { return (val == NULL) || val->empty(); }
  /** Checks to see if a {@link VRTObject} pointer is null. This is the same as 
   *  calling <tt>val->isNullValue()</tt> following a check for a null pointer.
   */
  inline bool isNull (const VRTObject *val) { return (val == NULL) || val->isNullValue(); }
};


/** Supports appending a {@link VRTObject} value to a string. */
inline ostream& operator<<(ostream &s, const vrt::VRTObject &o) { return s << o.toString(); }

/** Supports appending a {@link VRTObject} value to an output stream. */
inline string   operator+ (string  &s, const vrt::VRTObject &o) { return s +  o.toString(); }

/** Supports appending a {@link boolNull} value to a string. */
inline string   operator+ (string  &s, vrt::boolNull val) {
  if (val == 0) return s + "_NULL";
  if (val >  0) return s + "_TRUE";
  else          return s + "_FALSE";
}

/** Supports appending a {@link boolNull} value to an output stream. */
inline ostream& operator<<(ostream &s, vrt::boolNull val) {
  if (val == 0) return s << "_NULL";
  if (val >  0) return s << "_TRUE";
  else          return s << "_FALSE";
}

/** Supports appending a <tt>wstring</tt> value to an output stream any non-ASCII characters
 *  will be replaced with a '?'.
 */
inline ostream& operator<<(ostream &s, const wstring &val) {
  string questionMark = "?";
  string asciiChar    = "_";

  for (size_t i = 0; i < val.length(); i++) {
    if ((val[i] >= 0x00) && (val[i] <= 0x7F)) {
      asciiChar[0] = (char)val[i];
      s << asciiChar;
    }
    else {
      s << questionMark;
    }
  }
  return s;
}
#endif /* _VRTObject_h */
