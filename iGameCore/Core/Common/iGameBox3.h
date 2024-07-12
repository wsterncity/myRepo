#ifndef iGameBox3_h
#define iGameBox3_h

#include "iGameVectorD.h"

IGAME_NAMESPACE_BEGIN
template <class ScalarT>
class Box3{
public:
	typedef ScalarT Scalar;
	typedef VectorD<ScalarT, 3> Point;
	Point min;
	Point max;

	inline Box3() { SetNull(); }
	inline Box3(const Point& min, const Point& max) : min(min), max(max) {}
	inline Box3(const Scalar min[3], const Scalar max[3]) : min(min), max(max) {}
	inline Box3(const Point& center, const ScalarT& radius) {
		min = center - Point(radius, radius, radius);
		max = center + Point(radius, radius, radius);
	}

	inline bool operator==(const Box3<ScalarT>& p) const
	{
		return min == p.min && max == p.max;
	}
	inline bool operator != (const Box3<ScalarT>& p) const
	{
		return min != p.min || max != p.max;
	}

	void Reset()
	{
		this->SetNull();
	}

	void Set(const Scalar p[3])
	{
		min[0] = max[0] = p[0];
		min[1] = max[1] = p[1];
		min[2] = max[2] = p[2];
	}

	void Set(const Point& p)
	{
		min = max = p;
	}

	void SetNull()
	{
		min[0] = 1; max[0] = -1;
		min[1] = 1; max[1] = -1;
		min[2] = 1; max[2] = -1;
	}

	void Add(const Box3<ScalarT>& b)
	{
		if (b.IsNull()) return;
		if (IsNull()) *this = b;
		else
		{
			if (min[0] > b.min[0]) min[0] = b.min[0];
			if (min[1] > b.min[1]) min[1] = b.min[1];
			if (min[2] > b.min[2]) min[2] = b.min[2];

			if (max[0] < b.max[0]) max[0] = b.max[0];
			if (max[1] < b.max[1]) max[1] = b.max[1];
			if (max[2] < b.max[2]) max[2] = b.max[2];
		}
	}

	void Add(const Point& p)
	{
		if (IsNull()) Set(p);
		else
		{
			if (min[0] > p[0]) min[0] = p[0];
			if (min[1] > p[1]) min[1] = p[1];
			if (min[2] > p[2]) min[2] = p[2];

			if (max[0] < p[0]) max[0] = p[0];
			if (max[1] < p[1]) max[1] = p[1];
			if (max[2] < p[2]) max[2] = p[2];
		}
	}

	void Add(const Scalar p[3])
	{
		if (IsNull()) Set(p);
		else
		{
			if (min[0] > p[0]) min[0] = p[0];
			if (min[1] > p[1]) min[1] = p[1];
			if (min[2] > p[2]) min[2] = p[2];

			if (max[0] < p[0]) max[0] = p[0];
			if (max[1] < p[1]) max[1] = p[1];
			if (max[2] < p[2]) max[2] = p[2];
		}
	}

	template<typename OtherType>
	void Add(const OtherType& p)
	{
		if (IsNull()) Set(Point(p[0], p[1], p[2]));
		else
		{
			if (min[0] > p[0]) min[0] = p[0];
			if (min[1] > p[1]) min[1] = p[1];
			if (min[2] > p[2]) min[2] = p[2];

			if (max[0] < p[0]) max[0] = p[0];
			if (max[1] < p[1]) max[1] = p[1];
			if (max[2] < p[2]) max[2] = p[2];
		}
	}

	void Add(const Point& p, const ScalarT& radius)
	{
		if (IsNull()) Set(p);
		else
		{
			min[0] = std::min(min[0], p[0] - radius);
			min[1] = std::min(min[1], p[1] - radius);
			min[2] = std::min(min[2], p[2] - radius);

			max[0] = std::max(max[0], p[0] + radius);
			max[1] = std::max(max[1], p[1] + radius);
			max[2] = std::max(max[2], p[2] + radius);
		}
	}

	void Intersect(const Box3<ScalarT>& b)
	{
		if (min[0] < b.min[0]) min[0] = b.min[0];
		if (min[1] < b.min[1]) min[1] = b.min[1];
		if (min[2] < b.min[2]) min[2] = b.min[2];

		if (max[0] > b.max[0]) max[0] = b.max[0];
		if (max[1] > b.max[1]) max[1] = b.max[1];
		if (max[2] > b.max[2]) max[2] = b.max[2];

		if (min[0] > max[0] || min[1] > max[1] || min[2] > max[2]) SetNull();
	}

	void Translate(const Point& p)
	{
		min += p;
		max += p;
	}

	bool IsIn(const Point& p) const
	{
		return (
			min[0] <= p[0] && p[0] <= max[0] &&
			min[1] <= p[1] && p[1] <= max[1] &&
			min[2] <= p[2] && p[2] <= max[2]
			);
	}

	bool IsInEx(const Point& p) const
	{
		return (
			min[0] <= p[0] && p[0] < max[0] &&
			min[1] <= p[1] && p[1] < max[1] &&
			min[2] <= p[2] && p[2] < max[2]
			);
	}

	bool Collide(const Box3<ScalarT>& b) const
	{
		return b.min[0] < max[0] && b.max[0] > min[0] &&
			b.min[1] < max[1] && b.max[1] > min[1] &&
			b.min[2] < max[2] && b.max[2] > min[2];
	}

	bool IsNull() const { return min[0] > max[0] || min[1] > max[1] || min[2] > max[2]; }

	bool IsEmpty() const { return min == max; }

	ScalarT Diag() const
	{
		return (max - min).Norm();
	}

	ScalarT SquaredDiag() const
	{
		return (max - min).SquaredNorm();
	}

	Point Center() const
	{
		return (min + max) / 2;
	}

	Point Dim() const
	{
		return (max - min);
	}
};


using Box3f = Box3<float>;
using Box3d = Box3<double>;
using Box3i = Box3<int>;
IGAME_NAMESPACE_END
#endif