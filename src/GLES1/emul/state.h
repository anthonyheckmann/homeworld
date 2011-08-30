#ifndef _HW_GLES1_STATE_H_
#define _HW_GLES1_STATE_H_

#include "include.h"

namespace gles1
{
	class StateSetup : public GLPart<
		GL_POINTS      , GL_LINES    , GL_LINE_STRIP,
		GL_LINE_LOOP   , GL_TRIANGLES, GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN, GL_QUADS    , GL_QUAD_STRIP,
		GL_POLYGON
	>
	{
	public:
		StateSetup();
/*
		template<GLEnum T>
		void Start();
*/
		void Start(GlEnum enum);
		void End();
	private:
		bool immediate;
		GLenum mode;
	};
};


#endif //_HW_GLES1_STATE_H_