#pragma once

#include <cmath>

namespace gfx {

class mat2d
{
public:
	mat2d() : matrix_()
	{
		matrix_[0] = 1.0f;
		matrix_[3] = 1.0f;
	}

	mat2d(float (&matrix)[6])
	{
		matrix_[0] = matrix[0];
		matrix_[1] = matrix[1];
		matrix_[2] = matrix[2];
		matrix_[3] = matrix[3];
		matrix_[4] = matrix[4];
		matrix_[5] = matrix[5];
	}

	mat2d(float a, float b, float c, float d, float e, float f)
	{
		matrix_[0] = a;
		matrix_[1] = b;
		matrix_[2] = c;
		matrix_[3] = d;
		matrix_[4] = e;
		matrix_[5] = f;
	}

	const float &operator[](int i) const
	{
		return matrix_[i];
	}

	float &operator[](int i)
	{
		return matrix_[i];
	}

	const mat2d &operator*=(const mat2d &m)
	{
		*this = *this * m;
		return *this;
	}

	mat2d operator*(const mat2d &m) const
	{
		const float (&a)[6] = matrix_;
		const float (&b)[6] = m.matrix_;

		return mat2d(
			a[0] * b[0] + a[2] * b[1],
			a[1] * b[0] + a[3] * b[1],
			a[0] * b[2] + a[2] * b[3],
			a[1] * b[2] + a[3] * b[3],
			a[0] * b[4] + a[2] * b[5] + a[4],
			a[1] * b[4] + a[3] * b[5] + a[5]
		);
	}

	template<typename vec2d_t> vec2d_t operator*(const vec2d_t &v) const
	{
		const float (&a)[6] = matrix_;

		vec2d_t result = v;

		result.x = a[0] * v.x + a[2] * v.y + a[4];
		result.y = a[1] * v.x + a[3] * v.y + a[5];

		return result;
	}

	static mat2d translate(float x, float y)
	{
		return mat2d(1.0f, 0.0f, 0.0f, 1.0f, x, y);
	}

	static mat2d scale(float x, float y)
	{
		return mat2d(x, 0.0f, 0.0f, y, 0.0f, 0.0f);
	}

	static mat2d skew(float x, float y)
	{
		return mat2d(1.0f, x, y, 1.0f, 0.0f, 0.0f);
	}

	static mat2d rotate(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		return mat2d(c, s, -s, c, 0.0f, 0.0f);
	}

	static mat2d ortho(float left, float right, float bottom, float top)
	{
		float w = right - left;
		float h = top - bottom;

		return mat2d(2.0f / w, 0.0f, 0.0f, 2.0f / h, -(right + left) / w, -(top + bottom) / h);
	}

	static mat2d transform(float x, float y, float radians, float sx, float sy, float ox, float oy,
		float kx, float ky)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		// translate   rotate      scale       skew        origin
		//  |1 0 x|   |c -s 0|   |sx  0 0|   | 1 ky 0|   |1  0 -ox|
		//  |0 1 y| . |s  c 0| . | 0 sy 0| . |kx  1 0| . |0  1 -oy|
		//  |0 0 1|   |0  0 1|   | 0  0 1|   | 0  0 1|   |0  0   1|

		mat2d result(
			c * sx - kx * s * sy,
			s * sx + c * kx * sy,
			c * ky * sx - s * sy,
			ky * s * sx + c * sy,
			0.0f, 0.0f
		);

		result[4] = x - ox * result[0] - oy * result[2];
		result[5] = y - ox * result[1] - oy * result[3];

		return result;
	}

private:
	float matrix_[6];
};

} // gfx
