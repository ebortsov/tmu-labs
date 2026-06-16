#include <cmath>
#include <cstdio>

namespace {
    // check if the imaginary number z = real + i*img belongs to mandelbrot set
    bool belongs_to_mandelbrot(double real, double img) {
        double z_real = 0, z_img = 0;
        for (int i = 1; i <= 200; i++) {
            double new_z_real = z_real * z_real - z_img * z_img + real;
            double new_z_img = 2 * z_real * z_img + img;
            z_real = new_z_real;
            z_img = new_z_img;
            if (z_real * z_real + z_img * z_img > 4) {
                return false;
            }
        }
        return true;
    }
} // namespace

namespace mandelbrot {

void mandelbrot(double realCenter, double imagCenter, double width, double height) {
    int terminal_width = 200;
    int terminal_height = static_cast<int>(std::ceil(80 * (height / width)));
    double x0 = realCenter - width / 2;
    double y0 = imagCenter + height / 2;
    for (int i = 0; i < terminal_height; i++) {
        for (int j = 0; j < terminal_width; j++) {
            double x = x0 + j * (width / terminal_width);
            double y = y0 - i * (height / terminal_height);
            if (belongs_to_mandelbrot(x, y)) {
                std::putchar('*');
            } else {
                std::putchar(' ');
            }
        }
        std::putchar('\n');
    }
}
} // namespace mandelbrot