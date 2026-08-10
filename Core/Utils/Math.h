#define m_pow(n) (n)*(n)


class Vector3
{
public:
	float x, y, z;

public:


	inline float Magnitude()
	{
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	inline float dot_product(Vector3 input) const {
		return (x * input.x) + (y * input.y) + (z * input.z);
	}
	inline Vector3 Normalized()
	{
		float num = this->Magnitude();
		if (num > 9.99999974737875E-06)
		{
			return { this->x / num, this->y / num, this->z / num };
		}
		return Vector3();
	}

	inline Vector3 NormalizeVector()
	{
		float length = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
		if (length > 0.0f)
		{
			return { this->x /= length,
			this->y /= length,
			this->z /= length,
			};
		}
	}
	inline Vector3() {
		x = y = z = 0.0f;
	}

	inline Vector3(float X, float Y, float Z) {
		x = X; y = Y; z = Z;
	}

	inline float operator[](int i) const {
		return ((float*)this)[i];
	}

	inline Vector3& operator-=(float v) {
		x -= v; y -= v; z -= v; return *this;
	}

	inline Vector3 operator*(float v) const {
		return Vector3(x * v, y * v, z * v);
	}

	inline Vector3 operator/(float v) const
	{
		return Vector3(x / v, y / v, z / v);
	}

	inline Vector3& operator+=(const Vector3& v) {
		x += v.x; y += v.y; z += v.z; return *this;
	}
	inline Vector3& operator-=(const Vector3& v) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	inline Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	inline bool operator==(const Vector3& other) const {
		return (x == other.x) && (y == other.y) && (z == other.z);
	}
	inline float length_sqr() const {
		return (x * x) + (y * y) + (z * z);
	}

	inline float length() const {
		return sqrt(length_sqr());
	}
	inline float Length() {
		return sqrtf(x * x + y * y + z * z);
	}
	inline bool empty() { return x == 0.000000 && y == 0.000000 && z == 0.000000; }

	__forceinline bool Zero() const {
		return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f && z > -0.1f && z < 0.1f);
	}
	inline float distance(Vector3 vec)
	{
		return sqrtf(m_pow(vec.x - x) + m_pow(vec.y - y) + m_pow(vec.z - z));
	}
};

class Vector2 {
public:
	float x, y;

	inline Vector2() {
		x = y = 0.0f;
	}

	inline Vector2(float X, float Y) {
		x = X; y = Y;
	}

	inline Vector2 operator*(float v) const {
		return Vector2(x * v, y * v);
	}

	inline Vector2 operator/(float v) const {
		return Vector2(x / v, y / v);
	}

	inline Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}

	inline Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}

	inline Vector2& operator+=(const Vector2& v) {
		x += v.x; y += v.y; return *this;
	}

	bool Zero() const {
		return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f);
	}

	float Distance(Vector2 input) {
		return sqrt(powf(x - input.x, 2) + powf(y - input.y, 2));
	}

	bool empty() const {
		return x == 0.f && y == 0.f;
	}

	bool Normalize() {
		if (y < -89)
			return false;

		else if (y > 89)
			return false;

		if (x < -360)
			return false;

		else if (x > 360)
			return false;

		return true;
	}
};


class Matrix
{
public:
	inline float* operator[](int i) {
		return m[i];
	}

	inline const float* operator[](int i) const {
		return m[i];
	}

	inline float* Base() {
		return &m[0][0];
	}

	inline const float* Base() const {
		return &m[0][0];
	}
public:
	static Matrix identityMatrix()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}
	Vector3 MultiplyPoint3x4(Vector3 point)
	{
		Vector3 result;
		result.x = m[0][0] * point.x + m[0][1] * point.y + m[0][2] * point.z + m[0][3];
		result.y = m[1][0] * point.x + m[1][1] * point.y + m[1][2] * point.z + m[1][3];
		result.z = m[2][0] * point.x + m[2][1] * point.y + m[2][2] * point.z + m[2][3];
		return result;
	}
	inline Matrix() {
		Init(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f
		);
	}

	inline Matrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) {
		Init(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33
		);
	}

	inline void Init(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	) {
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;

		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;

		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;

		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	Matrix transpose() const {
		return Matrix(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
};