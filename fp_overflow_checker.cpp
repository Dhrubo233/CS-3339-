#include <bitset>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

static void print_usage(const char* prog) {
    std::cout << "usage:\n";
    std::cout << "    " << prog << " loop_bound loop_counter\n\n";
    std::cout << "    loop_bound is a positive floating-point value\n";
    std::cout << "    loop_counter is a positive floating-point value\n";
}

static uint32_t float_bits(float x) {
    uint32_t u = 0;
    std::memcpy(&u, &x, sizeof(float));
    return u;
}

static void print_ieee(const std::string& label, float x) {
    uint32_t u = float_bits(x);
    uint32_t sign = (u >> 31) & 1u;
    uint32_t exp  = (u >> 23) & 0xFFu;
    uint32_t frac = u & 0x7FFFFFu;

    std::bitset<8>  eb(exp);
    std::bitset<23> fb(frac);

    std::cout << label << ":  " << sign << " " << eb << " " << fb << "\n";
}

static void print_bits_only(float x) {
    uint32_t u = float_bits(x);
    uint32_t sign = (u >> 31) & 1u;
    uint32_t exp  = (u >> 23) & 0xFFu;
    uint32_t frac = u & 0x7FFFFFu;

    std::bitset<8>  eb(exp);
    std::bitset<23> fb(frac);

    std::cout << sign << " " << eb << " " << fb << "\n";
}

static int unbiased_exp(float x) {
    uint32_t u = float_bits(x);
    int bexp = int((u >> 23) & 0xFFu);
    return bexp - 127;
}

static float pow2_float(int e_unbiased) {
    int biased = e_unbiased + 127;

    if (biased < 1) biased = 1;     // smallest normal exponent field
    if (biased > 254) biased = 254; // largest normal exponent field

    uint32_t u = (uint32_t(biased) & 0xFFu) << 23; // sign=0, frac=0
    float out;
    std::memcpy(&out, &u, sizeof(float));
    return out;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 0;
    }

    float loop_bound = 0.0f;
    float loop_counter = 0.0f;

    try {
        loop_bound = std::stof(argv[1]);
        loop_counter = std::stof(argv[2]);
    } catch (...) {
        print_usage(argv[0]);
        return 0;
    }

    print_ieee("Loop bound", loop_bound);
    print_ieee("Loop counter", loop_counter);
    std::cout << "\n";

    int Ebound = unbiased_exp(loop_bound);
    int Ecnt   = unbiased_exp(loop_counter);

    bool possible_overflow = (Ebound - Ecnt >= 24);

    if (!possible_overflow) {
        std::cout << "No overflow!\n";
        return 0;
    }

    float threshold = pow2_float(Ecnt + 24);

    std::cout << "Warning: Possible overflow!\n";
    std::cout << "Overflow threshold:\n";
    std::cout << "    " << std::setprecision(7) << std::scientific << threshold << "\n";
    std::cout << "    ";
    print_bits_only(threshold);

    return 0;
}
