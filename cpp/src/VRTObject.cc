/* ===================== COPYRIGHT NOTICE =====================
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
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#include "VRTObject.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>     // required for free(..)
#include <stdio.h>      // required for perror(..)

using namespace std;
using namespace vrt;


#if (defined(__GNU_COMPILER) || defined(__CLANG_COMPILER)) && INCLUDE_STACK_TRACE
#include <execinfo.h>  // required for backtrace(..) and backtrace_symbols(..)
#include <cxxabi.h>    // required for abi::__cxa_demangle(..)
/** Uses compiler-specific functions to get the back-trace.
 *  @return The back-trace or an empty vector if unable to obtain back-trace.
 */
static vector<string> getBacktrace () {
  try {
    void  *buffer[128];
    int    nptrs    = backtrace(buffer, 128);
    char **strings  = backtrace_symbols(buffer, nptrs);

    if (strings == NULL) {
      cerr << "WARNING: Unable to lookup symbols for back-trace." << endl;
      return vector<string>();
    }

    // In the loop below we start at i=2 to omit getBacktrace() and the VRTException(..)
    // constructor. Where possible, the function names will be demangled. For example,
    // the demangling will convert (GCC on Linux):
    //    libvrt.so(_ZN3vrt12VRTExceptionC1EPKcz+0x3a4) [0x2b1766564234]
    // to:
    //    libvrt.so(vrt::VRTException::VRTException(char const*, ...)+0x3a4) [0x2b1766564234]
    // Or (Clang on OS X):
    //    3  vrttest  0x000000010276ea04 _ZN7vrttest16NetUtilitiesTest24testGetNetworkDeviceNameEv + 564
    // to:
    //    3  vrttest  0x000000010276ea04 vrttest::NetUtilitiesTest::testGetNetworkDeviceName() + 564
    vector<string> trace;
    for (int n = 2; n < nptrs; n++) {
      string s = strings[n];

#if defined(__CLANG_COMPILER)
      size_t i = string::npos;
      size_t j = s.rfind("+");

      if ((j != string::npos) && (j > 2)) {
        j = j - 1;
        i = s.rfind(" ", j-1);
      }
#else
      size_t i = s.rfind("(");
      size_t j = s.rfind("+");
#endif

      if ((i != string::npos) && (j != string::npos) && (i < j-1)) {
        string  mangled = s.substr(i+1,(j-i-1));
        int     status  = -999;
        char   *fixed   = abi::__cxa_demangle(mangled.c_str(), NULL, NULL, &status);

        if ((status == 0) && (fixed != NULL)) {
          s = s.substr(0,i+1) + string(fixed) + s.substr(j);
          free(fixed);
        }
      }
      trace.push_back(s);
    }
    free(strings);

    return trace;
  }
  catch (exception e) {
    return vector<string>();
  }
}
#else
/** Uses compiler-specific functions to get the back-trace.
 *  @return The back-trace or an empty vector if unable to obtain back-trace.
 */
static vector<string> getBacktrace () {
  return vector<string>();
}
#endif

string VRTObject_private::getClassName (const char *name) {
  string className;
  string suffix("");

  while (name[0] == 'P') {
    name++;
    suffix += "*";
  }

  while (name[0] == 'N') {
    size_t len = 0;
    name++;
    while ((name[0] >= '0') && (name[0] <= '9')) {
      len = (len * 10) + (name[0]-'0');
      name++;
    }
    className = className + string(name, len) + "::";
    name += len;
  }

  size_t len = 0;
  while ((name[0] >= '0') && (name[0] <= '9')) {
    len = (len * 10) + (name[0]-'0');
    name++;
  }
  return className + string(name, len) + suffix;
}


MutexObj::MutexObj () {
  isLocked = false;
  if (pthread_mutex_init(&mutexLock, NULL) != 0) {
    // Don't use VRTException since this can cause an infinite loop
    perror("ERROR: Unable to initialize mutex lock for VRTObject");
    throw exception();
  }
}

MutexObj::~MutexObj () {
  if (isLocked) {
    unlock();
    cerr << "ERROR: Object destruction before mutex unlocked!!!" << endl;
  }
  if (pthread_mutex_destroy(&mutexLock) != 0) {
    // Don't use VRTException since this can cause an infinite loop
    perror("ERROR: Unable to destroy mutex lock for VRTObject");
    throw exception();
  }
}

MutexObj* MutexObj::lock () {
  if (isLocked && (mutexOwner == pthread_self())) {
    return NULL; // already locked
  }

  int err = pthread_mutex_lock(&mutexLock);
  if (err != 0) throw VRTException(err);
  isLocked   = true;
  mutexOwner = pthread_self();
  return this;
}

void MutexObj::unlock () {
  int err = pthread_mutex_unlock(&mutexLock);
  if (err != 0) throw VRTException(err);
  isLocked = false;
}



MutexLock::MutexLock (const VRTObject &obj) {
  VRTObject *o = const_cast<VRTObject*>(&obj);
  mutexObj = o->mutexObj.lock();
}

MutexLock::MutexLock (const VRTObject *obj) {
  if (obj == NULL) throw VRTException("START_SYNCHRONIZED(..) called with NULL object");
  VRTObject *o = const_cast<VRTObject*>(obj);
  mutexObj = o->mutexObj.lock();
}

void MutexLock::unlock () {
  if (mutexObj != NULL) {
    mutexObj->unlock();
    mutexObj = NULL;
  }
}

string VRTObject::toString () const {
  char address[32];
  snprintf(address, 32, "@%p", this);
  return getClassName() + address;
}

string VRTObject::getClassName () const {
  return VRTObject_private::getClassName(const_cast<char*>(getClass().name()));
}

bool VRTObject::equals (const VRTObject &o) const {
  return (this == &o);
}

bool VRTObject::equals (const VRTObject *o) const {
  return (o != NULL) && equals(*o);
}

void VRTObject::_delete () {
  delete this;
}

VRTException::VRTException () throw() :
  _Exception(),
  VRTObject(),
  message(""),
  description("")
{
  // done
}

VRTException::VRTException (const VRTException &e) throw() :
  _Exception(),
  VRTObject(),
  message(e.message),
  description(e.description),
  backtrace(e.backtrace)
{
  // done
}

VRTException::VRTException (string msg) throw() :
  _Exception(),
  VRTObject(),
  message("???"),
  description("???")
{
  message     = msg;
  description = getClassName() + ": " + msg;
  backtrace   = getBacktrace();
  if (PRINT_ALL_EXCEPTIONS) {
    printf("DEBUG: creating %s\n", what());
  }
}

VRTException::VRTException (const char *fmt, ...) throw() :
  _Exception(),
  VRTObject(),
  message("???"),
  description("???")
{
  string msg;
  // The code below is extremely similar to vrt::Utilities::format(..) except that it
  // should never cause an exception.
  va_list ap;
  for (size_t len = 32; len < 4096; len*=2) {
    char str[len];
    va_start(ap, fmt);
    size_t n = vsnprintf(str, len, fmt, ap);
    va_end(ap);
    if (n < len) {
      msg = string(str);
      break;
    }
  }
  message     = msg;
  description = getClassName() + ": " + msg;
  backtrace   = getBacktrace();
  if (PRINT_ALL_EXCEPTIONS) {
    printf("DEBUG: creating %s\n", what());
  }
}

VRTException::VRTException (int errnum) throw() :
  _Exception(),
  VRTObject(),
  message("???"),
  description("???")
{
  string msg = getErrorMsgFor(errnum);
  if (isNull(msg)) {
    // This should never happen, but just in case
    msg = "Unknown error code ###";
  }
  message     = msg;
  description = getClassName() + ": " + msg;
  backtrace   = getBacktrace();
  if (PRINT_ALL_EXCEPTIONS) {
    printf("DEBUG: creating %s\n", what());
  }
  backtrace = getBacktrace();
}

string VRTException::getErrorMsgFor (int errnum) throw() {
// Formerly this checked only "(_XOPEN_SOURCE == 600)" but was revised on
// 20 May 2013 to match the GCC 4.4.5 docs (see "man strerror_r"). Revised
// again on 3 July 2013 to account for the fact that the GNU-specific version
// is not present in OS X 10.6.8 (GCC 4.2.1) or non-GCC-compatible compilers.
// It is highly likely that the GCC-specific version isn't used under BSD either,
// but this is untested.
#if (defined(__APPLE__) && defined(__MACH__)) || !defined(__GNUC__) \
 || (((_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)) && (!_GNU_SOURCE))
  // XSI-compliant strerror_r(..) [see man page for details]
  char err[128];
  // The next call returns EINVAL as *warning* if error is unknown and ERANGE as a *warning* if
  // the error message is truncated.
  int status = strerror_r(errnum, err, 128);
  if ((status != 0) && (status != EINVAL) && (status != ERANGE)) {
    // Note we use "Unknown error code nnn" rather than the normal "Unknown error nnn"
    // so we have the opportunity to differentiate this situation from the one where
    // strerror_r(..) worked properly.
    snprintf(err, 128, "Unknown error code %d (%d)", errnum, errno);
  }
  // For the rare case where the string is too long -- this should be impossible, but
  // since we are in exception handling code, we want to be extra-careful.
  err[127] = '\0';
  return string(err);
#else
  // GNU-specific strerror_r() [see man page for details]
  char err[128];
  char* _err = strerror_r(errnum, err, 128);
  if (((void*)_err) != ((void*)err)) {
    // Returned pointer to fixed error string, and (usually) set err="".
    return string(_err);
  }
  else {
    // For the rare case where the string is too long -- this should be impossible, but
    // since we are in exception handling code, we want to be extra-careful.
    err[127] = '\0';
    return string(err);
  }
#endif
}

bool VRTException::equals (const VRTException &o) const {
  return (getClassName() == o.getClassName())
      && (message        == o.message)
      && (backtrace      == o.backtrace);
}

bool VRTException::equals (const VRTObject &o) const {
  // Can't inline this since the "checked_dynamic_cast" isn't defined until
  // after VRTException and ClassCastException are defined.
  try {
    return equals(*checked_dynamic_cast<const VRTException*>(&o));
  }
  catch (bad_cast &e) {
    UNUSED_VARIABLE(e);
    return false;
  }
}

void VRTException::printStackTrace (ostream &out) const {
  out << toString() << endl;
  for (size_t i = 0; i < backtrace.size(); i++) {
    out << "        " << backtrace[i] << endl;
  }
}

ClassCastException::ClassCastException () throw() :
  _BadCast(),
  VRTException()
{
  // done
}

ClassCastException::ClassCastException (const ClassCastException &e) throw() :
  _BadCast(),
  VRTException(e)
{
  // done
}

ClassCastException::ClassCastException (string msg) throw() :
  _BadCast(),
  VRTException(msg)
{
  message     = msg;
  description = getClassName() + ": " + msg;
  backtrace   = getBacktrace();
}

