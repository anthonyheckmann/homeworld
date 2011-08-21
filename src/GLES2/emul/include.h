#ifndef _HW_GLES2_INCLUDE_H_
#define _HW_GLES2_INCLUDE_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>

#include "defines.h"
#include "declaration.h"

#if defined __cplusplus
#ifndef nullptr
const                        // this is a const object...
class {
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};
#endif //nullptr

#include "GLPart.h"

template<typename T>
static T clamp(T Value, T Min, T Max)
{
	return (Value < Min)? Min : (Value > Max)? Max : Value;
}

template<typename T>
static T& Get();
#endif //__cplusplus
#endif //_HW_GLES2_INCLUDE_H_

