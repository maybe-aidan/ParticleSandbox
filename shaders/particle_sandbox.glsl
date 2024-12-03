#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

uniform vec3 u_camera;
uniform mat3 u_camera_look;

uniform vec3 u_positions[100];
uniform float u_radii[100];
uniform int numParticles;

#define PI 3.14159265
#define TAU (2.0 * PI)

vec3 velocity = vec3(2.0, 0.0, 0.0);
vec3 acceleration = vec3(0.0, 1.2, 1.3);

void rotate_around_axis(inout vec2 p, float a) {
    p = cos(a) * p + sin(a) * vec2(p.y, -p.x);
}

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Float Operators

float intersectSDF(in float distA, in float distB) {
    return max(distA, distB);
}

float unionSDF(in float distA, in float distB) {
    return min(distA, distB);
}
 
float differenceSDF(in float distA, in float distB) {
    return max(distA, -distB);
}

// this function returns a smoothed maximum of two functions
// k represents a coefficient of smoothing; essentially how strong our smoothing function is.
float smoothMax(float a, float b, float k) {
    return log(exp(k * a) + exp(k * b)) / k;
}


// the inverse of the smoothMax, allowing us to get fluid mixing of SDFs when sufficiently close together
float smoothMin(float a, float b, float k) {
    return -smoothMax(-a, -b, k);
}

// Vec2 Operators
vec2 intersectVec2(in vec2 v1, in vec2 v2) {
    return (v1.x > v2.x) ? v1: v2;
}

vec2 unionVec2(in vec2 v1, in vec2 v2) {
    return (v1.x < v2.x) ? v1: v2;
}

vec2 differenceVec2(in vec2 v1, in vec2 v2) {
    return (v1.x > -v2.x) ? v1: -v2;
}

// passes the functions to the corresponding float function, retrieves material ID from union function.
vec2 smoothMinVec2(in vec2 v1, in vec2 v2, float k) {
    float x = smoothMin(v1.x, v2.x, k);
    float y = unionVec2(v1, v2).y;
    vec2 result = vec2(x, y);
    return result;
}

// ==================================
//     SIGNED DISTANCE FUNCTIONS
// ==================================

float distance_from_sphere(in vec3 p, in float r){
    return length(p) - r;
    // distance between our point and the center of the sphere, minus its radius
}

vec2 map_of_the_world(in vec3 p){
    float world = distance_from_sphere(p, 0);
    for(int i = 0; i < numParticles; i++){
        world = smoothMin(world, distance_from_sphere(p - u_positions[i], u_radii[i]), 3);
    }

    return vec2(world, 4);
}

vec3 calculate_normal(in vec3 p) {
    const vec3 small_step = vec3(0.0001, 0.0, 0.0);
    float x_gradient = map_of_the_world(p + small_step.xyy).x - map_of_the_world(p - small_step.xyy).x;
    float y_gradient = map_of_the_world(p + small_step.yxy).x - map_of_the_world(p - small_step.yxy).x;
    float z_gradient = map_of_the_world(p + small_step.yyx).x - map_of_the_world(p - small_step.yyx).x;

    vec3 normal = vec3(x_gradient, y_gradient, z_gradient);

    return normalize(normal);
}

// all functions come in a tuple, with the x component being the float value of the function, 
// and the y component being the ID for assigning a color or material to the object
vec3 getMaterial(vec3 p, float ID) {
    vec3 m;
    switch(int(ID)) {
        case 1:
            m = vec3(0.75, 0.12, 0.0); break;
        case 2:
            m = vec3(0.0, 0.7333, 0.1843); break;
        case 3:
            m = vec3(0.749, 0.0, 0.902); break;
        case 4: // plane
            m = vec3(0.2 + 0.4 * mod(floor(p.x) + floor(p.z), 2.0)); break;
        default:
            m = vec3(1.0, 1.0, 1.0); break;
    }

    return m;
}

vec3 phong(in vec3 lightDir, in vec3 viewDir, in vec3 N, in vec3 color) {
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 nLightDir = normalize(lightDir); // normalized light direction vector

    const float ambientStrength = 0.1;
    vec3 ambient = lightColor * ambientStrength;

    float diff = max(dot(N, nLightDir), 0.0);
    vec3 diffuse = lightColor * diff;

    const float specularStrength = 0.5;
    vec3 reflectDir = reflect(nLightDir, N);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lightColor * (specularStrength * spec);

    vec3 result = ambient * color + diffuse * color + specular;
    return result;
}

// improved version of the original Phong model.
vec3 blinn_phong(in vec3 lightDir, in vec3 viewDir, in vec3 N, in vec3 color) {
    const vec3 lightColor = vec3(0.8, 1.0, 1.0);
    const float ambientStrength = 0.2;
    const float specularStrength = 0.5;

    vec3 nLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = ambientStrength * color;
    // diffuse
    float diff = max(dot(nLightDir, N), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDir = reflect(nLightDir, N);
    float spec = 0.0;
    vec3 halfwayDir = normalize(nLightDir - viewDir);
    spec = pow(max(dot(N, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(1.0) * spec;

    return ambient + diffuse + specular;
}

vec3 renderStars(vec2 uv) {
    float starDensity = 0.02; // Adjust density of stars
    float starThreshold = 0.98; // Stars only appear above this random value

    // Generate a random value based on UV coordinates
    float starValue = random(uv * 500.0); // Scale UV for finer granularity

    // If the value is above the threshold, render a bright star
    if (starValue > starThreshold) {
        float brightness = pow((starValue - starThreshold) / (1.0 - starThreshold), 8.0);
        return vec3(brightness); // White star
    }
    return vec3(0.0); // No star
}

vec3 raymarch(in vec3 ro, in vec3 rd, in vec2 uv) {
    float total_dist_traveled = 0.0;

    const int NUMBER_OF_STEPS = 256;
    const float EPSILON = 0.001;
    const float MAX_DISTANCE = 500.0;

    for(int i = 0; i < NUMBER_OF_STEPS; i++) {
        vec3 current_position = ro + total_dist_traveled * rd;

        // feed the sdf with p = current_position
        // Notice each function call to map_of_the_world() must specify the x or y component now
        float march_radius = map_of_the_world(current_position).x;

        if(march_radius < EPSILON) { // Hit!
            vec3 normal = calculate_normal(current_position);

            vec3 light_position = vec3(2.0 , 5.0 , 3.0);

            // do this normalization inside the lighting model, so we can calculate shadows
            //vec3 direction_to_light = normalize(light_position - current_position);
            vec3 direction_to_light = light_position - current_position;

            vec3 lighting = blinn_phong(normalize(direction_to_light), normalize(current_position - ro), normal, getMaterial(current_position, map_of_the_world(current_position).y));
                                                         // Color based on displacement value
            return lighting; //  + displacement(current_position) * vec3(0.0, 1.0, 0.0) * 2.0;
        }

        if(total_dist_traveled > MAX_DISTANCE) { // Miss
            break;
        }
        total_dist_traveled += march_radius;
    }
                                        // creates the sort of starburst effect of the background
    vec3 bgColor = vec3(0.2, 0.1, 0.3);
    vec3 stars = renderStars(uv);
    return bgColor + stars;

}

void render(inout vec3 color, in vec2 uv) {
    vec3 ro = u_camera;
    vec3 rd = u_camera_look * normalize(vec3(uv, 1.0));
    color = raymarch(ro, rd, uv);
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;

    vec3 color = vec3(0.0, 0.0, 0.0);
    render(color, uv);

    FragColor = vec4(color, 1.0);
}