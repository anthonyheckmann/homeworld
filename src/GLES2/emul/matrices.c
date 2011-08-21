#include <Matrix.h>

GLfloat IDENTITY[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
#define MATRIX_SIZE (sizeof(IDENTITY) / sizeof(IDENTITY[0]))

typedef struct GLstack_t GLstack;

GLstack* glMatrices[GL_COLOR + 1];

GLenum glActiveMatrixIndex = GL_MODELVIEW;

struct GLstack_t
{
	GLstack* previous;
	GLfloat matrix[MATRIX_SIZE];
};

ubyte getMatrixIndex(GLenum mode)
{
	switch( mode )
	{
	case GL_TEXTURE:
		return 2;
	case GL_MODELVIEW:
	case GL_PROJECTION:
	case GL_COLOR:
		return mode;
	default:
		glSetError(GL_INVALID_ENUM);
		return 0;
	}
}

GLenum getMatrixMode(ubyte index)
{
	switch( index )
	{
	case 0:
	case 1:
	case 3:
		return index;
	case 2:
		return GL_TEXTURE;
	default:
		glSetError(GL_INVALID_ENUM);
		return 0;
	}
}

void glMatrixMode(GLenum mode)
{
	glActiveMatrixIndex = getMatrixIndex(mode);
}

GLstack* getMatrixStack(GLenum matrix)
{
	GLstack* pStack = glMatrices[matrix];

	if( !pStack )
	{
		pStack = malloc(sizeof(GLstack));
		Copy(IDENTITY, pStack->matrix, MATRIX_SIZE);
	}

	return pStack;
}

GLstack* getActiveMatrixStack(GLenum matrix)
{
	return getMatrixStack(glActiveMatrixIndex);
}

GLfloat* getMatrix(GLenum matrix)
{
	return getMatrixStack(matrix)->matrix;
}

GLfloat* getActiveMatrix()
{
	return getMatrix(glActiveMatrixIndex);
}

void glGetIntegerv(GLenum pname, GLint* params)
{
	switch( pname )
	{
	case GL_MATRIX_MODE:
		*params = getMatrixMode(glActiveMatrixIndex);
		break;
	default:
		glSetError(GL_INVALID_ENUM);
		break;
	}
}

void glLoadMatrixf(const GLfloat* m)
{
	GLfloat* pMatrix = getActiveMatrix();
	Copy(m, pMatrix, MATRIX_SIZE);
}

void glLoadIdentity()
{
	glLoadMatrixf(IDENTITY);
}

void glGetFloatv(GLenum pname, GLfloat* params)
{
	switch( pname )
	{
	case GL_PROJECTION_MATRIX:
	{
		GLfloat* pMatrix = getMatrix(GL_PROJECTION);
		Copy(pMatrix, params, MATRIX_SIZE);
		break;
	}
	default:
		glSetError(GL_INVALID_ENUM);
		break;
	}
}

void glPushMatrix()
{
	GLstack* pNewStackHead = malloc(sizeof(GLstack));

	// Save reference to previous stack entry
	pNewStackHead->previous = getMatrixStack(glActiveMatrixIndex);

	// Duplicate matrix values
	copyMatrix(pNewStackHead->previous->matrix, pNewStackHead->matrix, MATRIX_SIZE);

	glMatrices[glActiveMatrixIndex] = pNewStackHead;
}

void glPopMatrix()
{
	GLstack* pOldStackHead = getMatrixStack(glActiveMatrixIndex);
	glMatrices[glActiveMatrixIndex] = pOldStackHead->previous;
	free(pOldStackHead);
}

void glMultMatrixf(const GLfloat* m)
{
	GLstack* pStack = getActiveMatrixStack();

	GLfloat buffer[MATRIX_SIZE];
	copyMatrix(pStack->matrix, buffer);

	 hmatMultiplyHMatByHMat(pStack->matrix, m, buffer);
}

void glScalef(
	GLfloat x,
	GLfloat y,
	GLfloat z)
{
	GLfloat matrix[] =
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
	glMultMatrix(matrix);
}

void glTranslatef(
	GLfloat  x,
	GLfloat  y,
	GLfloat  z)
{
	GLfloat matrix[] =
	{
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	};
	glMultMatrix(matrix);
}

void glRotatef(
	GLfloat  angle,
	GLfloat  x,
	GLfloat  y,
	GLfloat  z)
{
	GLfloat matrix[16];

  /* This function contributed by Erich Boleyn (erich@uruk.org) */
   GLfloat mag, s, c;
   GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

   s = sin( angle * (M_PI / 180.0) );
   c = cos( angle * (M_PI / 180.0) );

   mag = sqrt( x*x + y*y + z*z );

   if (mag == 0.0) {
      /* generate an identity matrix and return */
      MEMCPY(m, Identity, sizeof(GLfloat)*16);
      return;
   }

   x /= mag;
   y /= mag;
   z /= mag;

#define M(row,col)  m[col*4+row]

   /*
    *     Arbitrary axis rotation matrix.
    *
    *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
    *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
    *  (which is about the X-axis), and the two composite transforms
    *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
    *  from the arbitrary axis to the X-axis then back.  They are
    *  all elementary rotations.
    *
    *  Rz' is a rotation about the Z-axis, to bring the axis vector
    *  into the x-z plane.  Then Ry' is applied, rotating about the
    *  Y-axis to bring the axis vector parallel with the X-axis.  The
    *  rotation about the X-axis is then performed.  Ry and Rz are
    *  simply the respective inverse transforms to bring the arbitrary
    *  axis back to it's original orientation.  The first transforms
    *  Rz' and Ry' are considered inverses, since the data from the
    *  arbitrary axis gives you info on how to get to it, not how
    *  to get away from it, and an inverse must be applied.
    *
    *  The basic calculation used is to recognize that the arbitrary
    *  axis vector (x, y, z), since it is of unit length, actually
    *  represents the sines and cosines of the angles to rotate the
    *  X-axis to the same orientation, with theta being the angle about
    *  Z and phi the angle about Y (in the order described above)
    *  as follows:
    *
    *  cos ( theta ) = x / sqrt ( 1 - z^2 )
    *  sin ( theta ) = y / sqrt ( 1 - z^2 )
    *
    *  cos ( phi ) = sqrt ( 1 - z^2 )
    *  sin ( phi ) = z
    *
    *  Note that cos ( phi ) can further be inserted to the above
    *  formulas:
    *
    *  cos ( theta ) = x / cos ( phi )
    *  sin ( theta ) = y / sin ( phi )
    *
    *  ...etc.  Because of those relations and the standard trigonometric
    *  relations, it is pssible to reduce the transforms down to what
    *  is used below.  It may be that any primary axis chosen will give the
    *  same results (modulo a sign convention) using thie method.
    *
    *  Particularly nice is to notice that all divisions that might
    *  have caused trouble when parallel to certain planes or
    *  axis go away with care paid to reducing the expressions.
    *  After checking, it does perform correctly under all cases, since
    *  in all the cases of division where the denominator would have
    *  been zero, the numerator would have been zero as well, giving
    *  the expected result.
    */

   xx = x * x;
   yy = y * y;
   zz = z * z;
   xy = x * y;
   yz = y * z;
   zx = z * x;
   xs = x * s;
   ys = y * s;
   zs = z * s;
   one_c = 1.0F - c;

   M(0,0) = (one_c * xx) + c;
   M(0,1) = (one_c * xy) - zs;
   M(0,2) = (one_c * zx) + ys;
   M(0,3) = 0.0F;

   M(1,0) = (one_c * xy) + zs;
   M(1,1) = (one_c * yy) + c;
   M(1,2) = (one_c * yz) - xs;
   M(1,3) = 0.0F;

   M(2,0) = (one_c * zx) - ys;
   M(2,1) = (one_c * yz) + xs;
   M(2,2) = (one_c * zz) + c;
   M(2,3) = 0.0F;

   M(3,0) = 0.0F;
   M(3,1) = 0.0F;
   M(3,2) = 0.0F;
   M(3,3) = 1.0F;

#undef M

	glMultMatrix(m);
}

