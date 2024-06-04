#include "raylib.h"
#include "rlgl.h"

#include "rt.h"
#include "primitive.h"
#include "camera.h"
#include "material.h"

#include <string.h>

#define MAX_IMAGE_WIDTH 1920
#define MAX_IMAGE_HEIGHT 1080

const int window_width = 1920;
const int window_height = 1080;

const int work_group_size_x = 8;
const int work_group_size_y = 8;

void init_scene()
{
    MaterialHandle mat{};
    mat = add_lambertian(Color3{0.5f, 0.5f, 0.5f});
    add_sphere(Point3{0, -1000, 0}, 1000, mat);

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float choose_mat = random_float();
            Point3 center(a + 0.9f*random_float(), 0.2f, b + 0.9f*random_float());

            if ((center - Point3(4, 0.2f, 0)).length() > 0.9f)
            {
                if (choose_mat < 0.8f) {
                    // diffuse
                    Color3 albedo = {
                        random_float() * random_float(), 
                        random_float() * random_float(), 
                        random_float() * random_float()
                    };
                    mat = add_lambertian(albedo);
                    add_sphere(Point3{center.x, center.y, center.z}, 0.2f, mat);
                } else if (choose_mat < 0.95f) {
                    // metal
                    Color3 albedo = {
                        random_float(0.5f, 1), 
                        random_float(0.5f, 1), 
                        random_float(0.5f, 1)
                    };
                    float fuzz = random_float(0, 0.5f);
                    mat = add_metal(albedo, fuzz);
                    add_sphere(Point3{center.x, center.y, center.z}, 0.2f, mat);
                } else {
                    // glass
                    mat = add_dielectric(Color3{1.0f, 1.0f, 1.0f}, 1.5f);
                    add_sphere(Point3{center.x, center.y, center.z}, 0.2f, mat);
                }
            }
        }
    }

    mat = add_dielectric(Color3{1.0f, 1.0f, 1.0f}, 1.5f);
    add_sphere(Point3{0, 1, 0}, 1.0f, mat);

    mat = add_lambertian(Color3{0.4f, 0.2f, 0.1f});
    add_sphere(Point3{-4, 1, 0}, 1.0f, mat);

    mat = add_metal(Color3{0.7f, 0.6f, 0.5f}, 0.0);
    add_sphere(Point3{4, 1, 0}, 1.0f, mat);
}

void init_simple_test_scene()
{
    MaterialHandle mat{};
    mat = add_lambertian(Color3{0.8f, 0.8f, 0.0f});
    add_sphere(Point3{0.0f, -100.5f, -1.0f}, 100.0f, mat);
    
    mat = add_lambertian(Color3{0.1f, 0.2f, 0.5f});
    add_sphere(Point3{0.0f, 0.0f, -1.2f}, 0.5f, mat);

    mat = add_dielectric(Color3{1.0f, 1.0f, 1.0f}, 1.5f);
    add_sphere(Point3{-1.0f, 0.0f, -1.0f}, 0.5f, mat);
    
    mat = add_dielectric(Color3{1.0f, 1.0f, 1.0f}, 1.0f / 1.5f);
    add_sphere(Point3{-1.0f, 0.0f, -1.0f}, 0.4f, mat);
    
    mat = add_metal(Color3{0.8f, 0.6f, 0.2f}, 1.0f);
    add_sphere(Point3{1.0f, 0.0f, -1.0f}, 0.5f, mat);
}

int main(void)
{
    InitWindow(window_width, window_height, "rt demo");

    init_scene();

    // compute shader
    char* rt_shader_text = LoadFileText("resources/shaders/rt.comp");
    unsigned int rt_shader = rlCompileShader(rt_shader_text, RL_COMPUTE_SHADER);
    unsigned int rt_program = rlLoadComputeShaderProgram(rt_shader);

    unsigned int ssbo_seed = rlLoadShaderBuffer(sizeof(unsigned int)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT, NULL, RL_DYNAMIC_COPY);
    unsigned int ssbo_lambertian = rlLoadShaderBuffer(sizeof(LambertianSSBO), NULL, RL_DYNAMIC_COPY);
    unsigned int ssbo_metal = rlLoadShaderBuffer(sizeof(MetalSSBO), NULL, RL_DYNAMIC_COPY);
    unsigned int ssbo_dielectric = rlLoadShaderBuffer(sizeof(DielectricSSBO), NULL, RL_DYNAMIC_COPY);
    unsigned int ssbo_sphere = rlLoadShaderBuffer(sizeof(SphereSSBO), NULL, RL_DYNAMIC_COPY);
    unsigned int ssbo_color_accumulation = rlLoadShaderBuffer(4*sizeof(float)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT, NULL, RL_DYNAMIC_COPY);

    unsigned int* seed_buffer = (unsigned int*)malloc(sizeof(unsigned int)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT);
    float* color_accumulation_buffer = (float*)malloc(4*sizeof(float)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT);

    // camera
    RtCamera rt_camera;
    //rt_camera.aspect_ratio = 16.0f / 9.0f;
    rt_camera.image_width = 1280;
    rt_camera.image_height = 720;
    rt_camera.samples_per_pixel = 500;
    rt_camera.max_depth = 50;
    rt_camera.vfov = 20;
    //rt_camera.lookfrom = Point3(-2, 2, 1); // simple_test_scene
    //rt_camera.lookat = Point3(0, 0, -1); // simple_test_scene
    rt_camera.lookfrom = Point3(13, 2, 3);
    rt_camera.lookat = Point3(0, 0, 0);
    rt_camera.vup = Vec3(0, 1, 0);
    //rt_camera.defocus_angle = 0.0f; // simple_test_scene
    //rt_camera.focus_distance = (rt_camera.lookfrom - rt_camera.lookat).length(); // simple_test_scene
    rt_camera.defocus_angle = 0.6f;
    rt_camera.focus_distance = 10.0f;
    rt_camera.update();

    for (int y = 0; y < rt_camera.image_height; y++)
    {
        for (int x = 0; x < rt_camera.image_width; x++)
        {
            int index = y * rt_camera.image_width + x;
            seed_buffer[index] = 19u * x + 47u * y + 101u;
        }
    }

    memset(color_accumulation_buffer, 0, 4*sizeof(float)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT);

    RtCameraCompute rt_camera_comp = rt_camera.get_camera_compute();

    Image black_img = GenImageColor(rt_camera.image_width, rt_camera.image_height, BLACK);
    Texture2D target = LoadTextureFromImage(black_img);

    UnloadImage(black_img);

    int sample_count = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        ClearBackground(DARKGRAY);

        if (sample_count == 0)
        {
            rlEnableShader(rt_program);
            rlBindImageTexture(target.id, 0, target.format, false);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.lookfrom"), &rt_camera_comp.lookfrom, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.pixel_00"), &rt_camera_comp.pixel_00, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.pixel_delta_u"), &rt_camera_comp.pixel_delta_u, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.pixel_delta_v"), &rt_camera_comp.pixel_delta_v, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.defocus_disk_u"), &rt_camera_comp.defocus_disk_u, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.defocus_disk_v"), &rt_camera_comp.defocus_disk_v, RL_SHADER_UNIFORM_VEC3, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.max_depth"), &rt_camera_comp.max_depth, RL_SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "camera.samples_per_pixel"), &rt_camera_comp.samples_per_pixel, RL_SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(rt_program, "sample_count"), &sample_count, RL_SHADER_UNIFORM_INT, 1);
            rlUpdateShaderBuffer(ssbo_seed, seed_buffer, sizeof(unsigned int)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT, 0);
            rlUpdateShaderBuffer(ssbo_lambertian, &lambertian_buffer, sizeof(LambertianSSBO), 0);
            rlUpdateShaderBuffer(ssbo_metal, &metal_buffer, sizeof(MetalSSBO), 0);
            rlUpdateShaderBuffer(ssbo_dielectric, &dielectric_buffer, sizeof(DielectricSSBO), 0);
            rlUpdateShaderBuffer(ssbo_sphere, &sphere_buffer, sizeof(SphereSSBO), 0);
            rlUpdateShaderBuffer(ssbo_color_accumulation, color_accumulation_buffer, sizeof(4*sizeof(float)*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT), 0);
            rlBindShaderBuffer(ssbo_seed, 1);
            rlBindShaderBuffer(ssbo_lambertian, 2);
            rlBindShaderBuffer(ssbo_metal, 3);
            rlBindShaderBuffer(ssbo_dielectric, 4);
            rlBindShaderBuffer(ssbo_sphere, 5);
            rlBindShaderBuffer(ssbo_color_accumulation, 6);
            rlDisableShader();
        }

        if (sample_count < rt_camera.samples_per_pixel)
        {
            sample_count++;
            rlEnableShader(rt_program);
            rlSetUniform(rlGetLocationUniform(rt_program, "sample_count"), &sample_count, RL_SHADER_UNIFORM_INT, 1);
            // TODO: what do we do when (image_width % work_group_size_x != 0) or (image_height % work_group_size_y != 0)
            assert(rt_camera.image_width % work_group_size_x == 0);
            assert(rt_camera.image_height % work_group_size_y == 0);
            rlComputeShaderDispatch(
                rt_camera.image_width / work_group_size_x, rt_camera.image_height / work_group_size_y, 1);
            rlDisableShader();
        }

        DrawTextureRec(
            target, Rectangle{0, 0, float(rt_camera.image_width), float(rt_camera.image_height)}, 
            Vector2{0, 0}, WHITE);

        if (sample_count < rt_camera.samples_per_pixel)
        {
            DrawText(
                TextFormat("Rendering...(%i/%i)", sample_count, rt_camera.samples_per_pixel),
                16, rt_camera.image_height + 16,
                32, WHITE
            );
        }
        else
        {
            DrawText(
                TextFormat("Done(%i/%i)", sample_count, rt_camera.samples_per_pixel),
                16, rt_camera.image_height + 16,
                32, WHITE
            );
        }

        EndDrawing();
    }

    UnloadTexture(target);

    rlUnloadShaderBuffer(ssbo_sphere);
    rlUnloadShaderBuffer(ssbo_dielectric);
    rlUnloadShaderBuffer(ssbo_metal);
    rlUnloadShaderBuffer(ssbo_lambertian);

    rlUnloadShaderProgram(rt_program);

    CloseWindow();
}