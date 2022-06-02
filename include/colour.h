#ifndef COLOUR_H
#define COLOUR_H

#include "utilities.h"
#include "vec3.h"

#include <iostream>

void write_colour(std::ostream &out, colour pixel_colour, int SAMPLES_PER_PIXEL) {
    auto r = pixel_colour.x();
    auto g = pixel_colour.y();
    auto b = pixel_colour.z();

    // Divide the colour by the number of samples and gamma-correct for gamma=2.0
    auto scale = 1.0 / SAMPLES_PER_PIXEL;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each colour component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif
