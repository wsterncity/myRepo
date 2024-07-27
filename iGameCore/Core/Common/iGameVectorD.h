//#ifndef iGameVectorD_h
//#define iGameVectorD_h
//
//#include <iostream>
//#include "iGameMacro.h"
//#include "iGameType.h"
//
//IGAME_NAMESPACE_BEGIN
//
//template <class ScalarT, int dimension_t>
//class VectorD {
//public:
//	static constexpr int dimension = dimension_t;
//
//	typedef ScalarT Scalar;
//	typedef VectorD<Scalar, dimension> Self;
//
//	VectorD() {}
//	VectorD(const Scalar& c0, const Scalar& c1);
//	VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2);
//	VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//		const Scalar& c3);
//	VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//		const Scalar& c3, const Scalar& c4);
//	VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//		const Scalar& c3, const Scalar& c4, const Scalar& c5);
//	VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//		const Scalar& c3, const Scalar& c4, const Scalar& c5,
//		const Scalar& c6);
//	VectorD(const Self& o);
//	VectorD(const ScalarT p[dimension_t]) {
//		for (int i = 0; i < dimension; ++i) {
//			v_[i] = p[i];
//		}
//	}
//
//	template <class OtherScalarT, int other_dimension_t>
//	VectorD(const VectorD<OtherScalarT, other_dimension_t>& src_vector) {
//		for (int i = 0; i < dimension; ++i) {
//			if (i < other_dimension_t) v_[i] = Scalar(src_vector[i]);
//			else v_[i] = Scalar(0);
//		}
//	}
//
//	inline Scalar& operator[](int i) { assert(0 <= i && i < dimension); return v_[i]; }
//	inline const Scalar& operator[](int i) const { assert(0 <= i && i < dimension); return v_[i]; }
//
//	Self operator-() const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = -(*this)[i];
//		}
//		return ret;
//	}
//
//	Self operator+(const Self& o) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = (*this)[i] + o[i];
//		}
//		return ret;
//	}
//
//	Self operator-(const Self& o) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = (*this)[i] - o[i];
//		}
//		return ret;
//	}
//
//	ScalarT operator*(const Self& o) const {
//		ScalarT ret = 0.0;
//		for (int i = 0; i < dimension; ++i) {
//			ret += (*this)[i] * o[i];
//		}
//		return ret;
//	}
//
//	Self& operator+=(const Self& o) {
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] += o[i];
//		}
//		return *this;
//	}
//
//	Self& operator+=(const double& k) {
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] += k;
//		}
//		return *this;
//	}
//
//	Self& operator-=(const Self& o) {
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] -= o[i];
//		}
//		return *this;
//	}
//
//	Self& operator-=(const double& k) {
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] -= k;
//		}
//		return *this;
//	}
//
//	Self& operator*=(const ScalarT& o) {
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] *= o;
//		}
//		return *this;
//	}
//
//	Self operator*(const ScalarT& k) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = v_[i] * k;
//		}
//		return ret;
//	}
//
//	Self operator*=(const double& k) const {
//		for (int i = 0; i < dimension; ++i) {
//			v_[i] *= k;
//		}
//		return *this;
//	}
//
//	Self operator/(const double& k) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = v_[i] / k;
//		}
//		return ret;
//	}
//
//	Self operator/=(const double& k) {
//		for (int i = 0; i < dimension; ++i) {
//			v_[i] /= k;
//		}
//		return *this;
//	}
//
//	Self operator+(const Scalar& o) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = (*this)[i] + o;
//		}
//		return ret;
//	}
//
//	Self operator-(const Scalar& o) const {
//		Self ret;
//		for (int i = 0; i < dimension; ++i) {
//			ret[i] = (*this)[i] - o;
//		}
//		return ret;
//	}
//
//	bool operator==(const Self& o) const {
//		for (int i = 0; i < dimension; ++i) {
//			if ((*this)[i] != o[i]) {
//				return false;
//			}
//		}
//		return true;
//	}
//
//	bool operator!=(const Self& x) const { return !((*this) == x); }
//
//	bool operator<(const Self& x) const {
//		for (int i = 0; i < dimension - 1; ++i) {
//			if (v_[i] < x.v_[i]) {
//				return true;
//			}
//			if (v_[i] > x.v_[i]) {
//				return false;
//			}
//		}
//		if (v_[dimension - 1] < x.v_[dimension - 1]) {
//			return true;
//		}
//		return false;
//	}
//
//	inline void SetZero() {
//		for (int i = 0; i < dimension; ++i) {
//			v_[i] = 0;
//		}
//	}
//
//	inline double Norm() const { return sqrt(this->SquaredNorm()); }
//	inline double Length() const { return sqrt(this->SquaredLength()); }
//
//	inline double SquaredNorm() const {
//		double ret = 0;
//		for (int i = 0; i < dimension; ++i) {
//			ret += v_[i] * v_[i];
//		}
//		return ret;
//	}
//
//	inline double SquaredLength() const {
//		double ret = 0;
//		for (int i = 0; i < dimension; ++i) {
//			ret += v_[i] * v_[i];
//		}
//		return ret;
//	}
//
//	inline void Normalize()
//	{
//		double magnitude = std::sqrt(this->SquaredNorm());
//		if (magnitude == 0) {
//			return;
//		}
//		for (int i = 0; i < dimension; ++i) {
//			(*this)[i] /= magnitude;
//		}
//	}
//	inline ScalarT Dot(const Self& o) const {
//		ScalarT ret = 0.0;
//		for (int i = 0; i < dimension; ++i) {
//			ret += (*this)[i] * o[i];
//		}
//		return ret;
//	}
//	inline Self Cross(const Self& v) const {
//
//		if (dimension != 3)return *this;
//		Self r;
//		r[0] = (v_[1] * v[2]) - (v_[2] * v[1]);
//		r[1] = (v_[2] * v[0]) - (v_[0] * v[2]);
//		r[2] = (v_[0] * v[1]) - (v_[1] * v[0]);
//		return r;
//	}
//
//	inline Self Normalized() const
//	{
//		Self ret(*this);
//		ret.Normalize();
//		return ret;
//	}
//
//	const Scalar& MaxCoeff() const {
//		return *std::max_element(v_, v_ + dimension);
//	}
//
//	const Scalar& MinCoeff() const {
//		return *std::min_element(v_, v_ + dimension);
//	}
//
//	Scalar* Pointer() { return &(v_[0]); }
//	const Scalar* Pointer() const { return &(v_[0]); }
//	size_t Dim() { return dimension; }
//
//private:
//	Scalar v_[dimension];
//};
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1)
//{
//	assert(dimension == 2);
//	v_[0] = c0;
//	v_[1] = c1;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2)
//{
//	assert(dimension == 3);
//	v_[0] = c0;
//	v_[1] = c1;
//	v_[2] = c2;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//	const Scalar& c3)
//{
//	assert(dimension == 4);
//	v_[0] = c0;
//	v_[1] = c1;
//	v_[2] = c2;
//	v_[3] = c3;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//	const Scalar& c3, const Scalar& c4)
//{
//	assert(dimension == 5);
//	v_[0] = c0;
//	v_[1] = c1;
//	v_[2] = c2;
//	v_[3] = c3;
//	v_[4] = c4;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//	const Scalar& c3, const Scalar& c4, const Scalar& c5)
//{
//	assert(dimension == 6);
//	v_[0] = c0;
//	v_[1] = c1;
//	v_[2] = c2;
//	v_[3] = c3;
//	v_[4] = c4;
//	v_[5] = c5;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Scalar& c0, const Scalar& c1, const Scalar& c2,
//	const Scalar& c3, const Scalar& c4, const Scalar& c5, const Scalar& c6)
//{
//	assert(dimension == 7);
//	v_[0] = c0;
//	v_[1] = c1;
//	v_[2] = c2;
//	v_[3] = c3;
//	v_[4] = c4;
//	v_[5] = c5;
//	v_[6] = c6;
//}
//
//template<class ScalarT, int dimension_t>
//inline VectorD<ScalarT, dimension_t>::VectorD(const Self& o)
//{
//	for (int i = 0; i < dimension; ++i) {
//		(*this)[i] = o[i];
//	}
//}
//
//template <class ScalarT, int dimension_t>
//VectorD<ScalarT, dimension_t> operator*(
//	const ScalarT& o, const VectorD<ScalarT, dimension_t>& v) {
//	return v * o;
//}
//
//template<class ScalarT, int dimension_t>
//inline ScalarT DotProduct(const VectorD<ScalarT, dimension_t>& o1,
//	const VectorD<ScalarT, dimension_t>& o2)
//{
//	ScalarT ret(0);
//	for (int i = 0; i < dimension_t; ++i) {
//		ret += o1[i] * o2[i];
//	}
//	return ret;
//}
//
//template <class ScalarT>
//inline VectorD<ScalarT, 3> CrossProduct(const VectorD<ScalarT, 3>& u,
//	const VectorD<ScalarT, 3>& v)
//{
//	//static_assert(std::is_signed<ScalarT>::Scalar, "ScalarT must be a signed type. ");
//	VectorD<ScalarT, 3> r;
//	r[0] = (u[1] * v[2]) - (u[2] * v[1]);
//	r[1] = (u[2] * v[0]) - (u[0] * v[2]);
//	r[2] = (u[0] * v[1]) - (u[1] * v[0]);
//	return r;
//}
//
//template <class ScalarT, int dimension_t>
//inline std::ostream& operator<<(
//	std::ostream& out, const VectorD<ScalarT, dimension_t>& vec) {
//	for (int i = 0; i < dimension_t - 1; ++i) {
//		out << vec[i] << " ";
//	}
//	out << vec[dimension_t - 1];
//	return out;
//}
//
//typedef VectorD<float, 2> Vector2f;
//typedef VectorD<float, 3> Vector3f;
//typedef VectorD<float, 4> Vector4f;
//typedef VectorD<float, 5> Vector5f;
//typedef VectorD<float, 6> Vector6f;
//typedef VectorD<float, 7> Vector7f;
//
//typedef VectorD<double, 2> Vector2d;
//typedef VectorD<double, 3> Vector3d;
//typedef VectorD<double, 4> Vector4d;
//typedef VectorD<double, 5> Vector5d;
//typedef VectorD<double, 6> Vector6d;
//typedef VectorD<double, 7> Vector7d;
//
//typedef VectorD<int, 2> Vector2i;
//typedef VectorD<int, 3> Vector3i;
//typedef VectorD<int, 4> Vector4i;
//typedef VectorD<int, 5> Vector5i;
//typedef VectorD<int, 6> Vector6i;
//typedef VectorD<int, 7> Vector7i;
//
//typedef VectorD<uint32_t, 2> Vector2ui;
//typedef VectorD<uint32_t, 3> Vector3ui;
//typedef VectorD<uint32_t, 4> Vector4ui;
//typedef VectorD<uint32_t, 5> Vector5ui;
//typedef VectorD<uint32_t, 6> Vector6ui;
//typedef VectorD<uint32_t, 7> Vector7ui;
//
//IGAME_NAMESPACE_END
//#endif
//
