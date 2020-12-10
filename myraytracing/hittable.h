#ifndef HITTABLE_H
#define HITTABLE_H

#include "utility.h"

class Material;

struct hit_record
{
	point3 p;
	vec3 normal;
	shared_ptr<Material> material_ptr;
	double t;
	bool front_face;

	inline void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
		normal = normalize(normal);
	}
};

class hittable
{
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif // !HITTABLE_H
