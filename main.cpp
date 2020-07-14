#include "utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

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

  hittable_list world;

  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, make_shared<lambertian>(color(.1, .2, .5))));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, make_shared<lambertian>(color(.8, .8, 0.))));
  world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, make_shared<metal>(color(.8, .6, .2), 0.3)));
  world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(point3(-1, 0, -1), -0.45, make_shared<dielectric>(1.5)));
  camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20, aspect_ratio);

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
