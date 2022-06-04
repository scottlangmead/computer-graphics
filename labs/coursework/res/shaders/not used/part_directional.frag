// Directional light data
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light
{
  vec4 light_colour;
  vec4 ambient_intensity;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Material data
#ifndef MATERIAL
#define MATERIAL
struct material
{
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Directional light calculation
vec4 calculate_directional(in directional_light directional, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
  // *********************************
  // Get distance between directional light and vertex
  float d = distance(50.0f, 50.0f);
  // Calculate attenuation factor : constant + (linear * d) + (quadratic * d * d)
  float attenuation = (directional.constant + (directional.linear * d) + (directional.quadratic * d * d));
  // Calculate light colour : light_colour / attenuation
  vec4 light_colour = directional.light_colour / attenuation;
  //Set colour alpha to 1.0
  light_colour.a = 1.0;
  // Calculate light dir
  vec3 light_dir = normalize(vec3(0.0f, 0.0f, 0.0f));
  // *********************************
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = (mat.diffuse_reflection * light_colour) * max(dot(normal, light_dir), 0);
  vec3 half_vector = normalize(light_dir + view_dir);
  vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0), mat.shininess);
  vec4 primary = mat.emissive + diffuse;
  vec4 colour = primary * tex_colour + specular;
  colour.a = 1.0;
  return colour;
}


