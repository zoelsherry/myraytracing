#ifndef MATERIAL_H
#define MATERIAL_H

#include "utility.h"

//struct hit_record;
#include "hittable.h"

class Material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

};

class Lambertian : public Material
{
public:
	Lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();

		// Catch degenerate scatter directon
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

public:
	color albedo;
};

class Metal : public Material
{
public:
	Metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

public:
	color albedo;
	double fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(double refractive_indices_ratio) : refractive_index_ratio_(refractive_indices_ratio) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		double refraction_ratio = rec.front_face ? (1.0 / refractive_index_ratio_) : refractive_index_ratio_;

		double cos_theta = fmin(dot(-r_in.direction(), rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
		
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) // TODO: WHY great than random
		{
			direction = reflect(r_in.direction(), rec.normal);
		}
		else
		{
			direction = refract(r_in.direction(), rec.normal, refraction_ratio);
		}

		scattered = ray(rec.p, direction);
		return true;
	}

public:
	double refractive_index_ratio_; // index of refraction

private:
	static double reflectance(double cosine, double refract_idx)
	{
		// Use Schlick's approximation for reflection
		double r0 = (1 - refract_idx) / (1 + refract_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}
};

#endif // !MATERIAL_H
