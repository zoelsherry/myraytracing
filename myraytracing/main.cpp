#include "utility.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define CHANNEL_NUM 3

using namespace std;

color ray_color(const ray& r, const hittable& world, int depth)
{
	// exceed the ray bounce limit, no more light is gathered
	if (depth <= 0)
		return color(0, 0, 0);

	hit_record rec;
	if (world.hit(r, 0.001, INF, rec)) // 0.001 Fixing Shadow Acne
	{
		//point3 target = rec.p + rec.normal + random_unit_vector();
		//point3 target = rec.p + random_in_hemisphere(rec.normal);
		ray scattered;
		color attenuation;
		if (rec.material_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		else
		{
			return color(0, 0, 0);
		}

		// view normal
		// return 0.5*(rec.normal + color(1, 1, 1));
	}
	vec3 direction = normalize(r.direction());
	double t = 0.5 * (direction.y() + 1.0);
	return (1.0 - t)*color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene()
{
	hittable_list world;

	auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int i = -11; i < 11; i++)
	{
		for (int j = -11; j < 11; j++)
		{
			auto choose_material = random_double();
			point3 center(i + 0.9*random_double(), 0.2, j + 0.9*random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<Material> sphere_material;

				if (choose_material < 0.8)
				{
					// diffuse
					vec3 albedo = color::random()*color::random();
					sphere_material = make_shared<Lambertian>(albedo);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_material < 0.95)
				{
					// metal
					vec3 albedo = color::random(0.5, 1);
					double fuzzy = random_double(0, 0.5);
					sphere_material = make_shared<Metal>(albedo, fuzzy);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else
				{
					// glass
					sphere_material = make_shared<Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5);
	world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

int main(int argc, char* argv[], char* envp[])
{
	// image
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 500;
	const int max_depth = 50;
	const double gamma = 2.2;

	// World
	//hittable_list world;
	//auto material_ground = make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	////auto material_center = make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	////auto material_left = make_shared<Metal>(color(0.8, 0.8, 0.8), 0.3);
	////auto material_center = make_shared<Dielectric>(1.5);
	//auto material_center = make_shared<Lambertian>(color(0.1, 0.2, 0.5));
	//auto material_left = make_shared<Dielectric>(1.5);
	//auto material_right = make_shared<Metal>(color(0.8, 0.6, 0.2), 0.0);

	//world.add(make_shared<Sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	//world.add(make_shared<Sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	//world.add(make_shared<Sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	//world.add(make_shared<Sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left)); // hollow glass sphere
	//world.add(make_shared<Sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	hittable_list world = random_scene();

	// Camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	double focus_dist = (lookfrom - lookat).length();
	double aperture = 2.0;
	Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, focus_dist);

	// render
	unsigned char* pixels = new unsigned char[image_width*image_height*CHANNEL_NUM]();

	int index = 0;
	for (int j = image_height-1; j >= 0; j--)
	{
		cerr << "\rScanlines remaining: " << j << ' ' << flush;
		for (int i = 0; i < image_width; i++)
		{
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++)
			{
				double u = double(i + random_double()) / (image_width - 1);
				double v = double(j + random_double()) / (image_height - 1);
				ray r = camera.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			pixel_color *= 1.0 / samples_per_pixel;

			pixel_color = pow(pixel_color, vec3(1 / gamma));

			pixels[index++] = static_cast<unsigned char>(255.999 * pixel_color[0]);
			pixels[index++] = static_cast<unsigned char>(255.999 * pixel_color[1]);
			pixels[index++] = static_cast<unsigned char>(255.999 * pixel_color[2]);
		}
	}

	stbi_write_png("output.png", image_width, image_height, CHANNEL_NUM, pixels, image_width*CHANNEL_NUM);

	cout << "\nDone!" << endl;

	return 0;
}