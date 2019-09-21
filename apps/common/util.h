#pragma once

float diff_image(const unsigned char *image, const unsigned char *golden, unsigned int width, unsigned int height);
void print_image(const unsigned char *image, unsigned int width, unsigned int height);
void print_diff(const unsigned char *image, const unsigned char *golden, unsigned int width, unsigned int height);
