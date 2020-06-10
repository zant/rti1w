#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

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

color ray_color(const ray &r, const hittable &world)
{
  hit_record rec;
  if (world.hit(r, 0, infinity, rec))
  {
    return 0.5 * (rec.normal + color(1, 1, 1));
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main()
{
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 384;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;

  std::cout << "P3\n"
            << image_width << " " << image_height << "\n255\n";

  hittable_list world;
  world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100));

  camera cam;

  for (int j = image_height - 1; j >= 0; --j)
  {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i)
    {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s)
      {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world);
      }
      pixel_color.write_color(std::cout, samples_per_pixel);
    }
  }
  std::cerr << "\nDone.\n";
}
