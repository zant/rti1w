#include "utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

hittable_list random_scene()
{
  hittable_list world;

  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1090, ground_material));

  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8)
        {
          auto albedo = color::random() * color::random();
          sphere_material = make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95)
        {
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
        else
        {
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }
  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return world;
}

double hit_sphere(vec3 center, double radius, const ray &r)
{
  vec3 oc = r.origin() - center;
  auto a = dot(r.direction(), r.direction());
  auto half_b = dot(oc, r.direction());
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
  {
    return -1.0;
  }
  else
  {
    return (-half_b - sqrt(discriminant)) / a;
  }
}

color ray_color(const ray &r, const hittable &world, int depth)
{
  hit_record rec;

  if (depth <= 0)
    return color(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);

    return color(0, 0, 0);
    // point3 target = rec.p + random_in_hemisphere(rec.normal);
    // return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
    // return 0.5 * (rec.normal + color(1,1,1);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main()
{
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 600;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  std::cout << "P3\n"
            << image_width << " " << image_height << "\n255\n";

  // hittable_list world = random_scene();
  hittable_list world;

  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, make_shared<lambertian>(color(.1, .2, .5))));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, make_shared<lambertian>(color(.8, .8, 0.))));
  world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, make_shared<metal>(color(.8, .6, .2), 0.3)));
  world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(point3(-1, 0, -1), -0.45, make_shared<dielectric>(1.5)));

  point3 lookfrom(3, 3, 2);
  point3 lookat(0, 0, -1);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = (lookfrom - lookat).length();
  auto aperture = 2.0;

  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  for (int j = image_height - 1; j >= 0; --j)
  {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i)
    {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s)
      {
        auto u = (i + random_double(0, 1)) / (image_width - 1);
        auto v = (j + random_double(0, 1)) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }
      pixel_color.write_color(std::cout, samples_per_pixel);
    }
  }
  std::cerr << "\nDone.\n";
}
