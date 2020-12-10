#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class Sphere : public hittable
{
public:
	Sphere() {}
	Sphere(point3 cen, double r, shared_ptr<Material> m) : center(cen), radius(r), material_ptr(m) {};

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	point3 center;
	double radius;
	shared_ptr<Material> material_ptr;
};

bool Sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	// ray: A + tb
	// t^{2}b*b+2t*b*(A−C)+(A−C)*(A−C)−r^2=0
	vec3 oc = r.origin() - center;
	double a = dot(r.direction(), r.direction());
	double b = 2.0 * dot(oc, r.direction());
	double c = dot(oc, oc) - radius * radius;
	double discriminant = b * b - 4 * a*c;
	
	if (discriminant > 0)
	{
		double root = sqrt(discriminant);
		double temp = (-b - root) / (2.0*a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.at(temp);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.material_ptr = material_ptr;
			return true;
		}
		temp = (-b + root) / (2.0*a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.at(temp);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.material_ptr = material_ptr;
			return true;
		}


	}

	return false;
}

#endif // !SPHERE_H
