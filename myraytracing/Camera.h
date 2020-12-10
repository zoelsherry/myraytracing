#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"

class Camera
{
public:
	Camera(
		point3 lookfrom,
		point3 lookat,
		vec3 vup,
		double vertical_fov,
		double aspect_ratio,
		double aperture,
		double focus_dist
	)
	{
		double theta = degrees_to_radians(vertical_fov);
		double h = tan(theta / 2);
		double viewport_height = 2.0*h;
		double viewport_width = aspect_ratio * viewport_height;

		w = normalize(lookfrom - lookat);
		u = normalize(cross(vup, w));
		v = normalize(cross(w, u));

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal * 0.5 - vertical * 0.5 - focus_dist * w; // focus plane

		lens_radius = aperture / 2;
	}

	ray get_ray(double s, double t) const
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();

		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 w, u, v;
	double lens_radius;
};

#endif // !CAMERA_H
