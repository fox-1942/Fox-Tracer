# version 460 core

layout(binding = 0, rgba32f) uniform image2D framebufferImage;
layout (local_size_x = 16, local_size_y = 16) in;