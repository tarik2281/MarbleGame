#ifndef _MATRIX_STACK_H_
#define _MATRIX_STACK_H_

#include "Maths/MatrixMath.h"

class MatrixStack
{
public:
	void Push();
	void Pop();
	Matrix4& GetCurrentTransform();


	MatrixStack(int size = 6);
	~MatrixStack();
private:
	void* m_pHeapMemory;
	unsigned int m_maxSize;
	unsigned int m_current;
};

#endif // _MATRIX_STACK_H_