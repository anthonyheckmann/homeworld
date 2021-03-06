
#ifndef _HW_GLES2_COLOR_H_
#define _HW_GLES2_COLOR_H_

#include "include.h"
#include "../../GLES1/emul/color.h"

class ColorSetup : public gles1::ColorSetup
{
public:
	ColorSetup();
	void SetPointer(
		GLint          size,
		GLenum         type,
		GLsizei        stride,
		const GLvoid*  pointer
	);
private:
	friend class RENDER_PROCESSOR;
	GLint size;
	GLenum type;
	GLsizei stride;
	const GLvoid* pointer;
};

#endif //_HW_GLES2_COLOR_H_

