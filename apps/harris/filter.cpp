#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "HalideRuntime.h"
#include "HalideBuffer.h"

#include "harris.h"
#include "harris_classic_auto_schedule.h"
#include "harris_auto_schedule.h"

#include "halide_benchmark.h"
#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: %s in [out [out-classic [out-auto]]]\n", argv[0]);
        return 1;
    }
    Halide::Runtime::Buffer<float> input = load_and_convert_image(argv[1]);
    Halide::Runtime::Buffer<float> output(input.width()-6, input.height()-6);

    harris(input, output);
    double min_t = benchmark(10, 10, [&]() {
        harris(input, output);
        output.device_sync();
    });
    printf("runtime (manual schedule): %gms\n", min_t * 1e3);
    if (argc >= 3) {
        convert_and_save_image(output, argv[2]);
    }

    harris_classic_auto_schedule(input, output);
    min_t = benchmark(10, 10, [&]() {
        harris_classic_auto_schedule(input, output);
        output.device_sync();
    });
    printf("runtime (classic auto scheduler): %gms\n", min_t * 1e3);
    if (argc >= 4) {
        convert_and_save_image(output, argv[3]);
    }

    harris_auto_schedule(input, output);
    min_t = benchmark(10, 10, [&]() {
        harris_auto_schedule(input, output);
        output.device_sync();
    });
    printf("runtime (new auto scheduler): %gms\n", min_t * 1e3);
    if (argc >= 5) {
        convert_and_save_image(output, argv[4]);
    }

    return 0;
}