#pragma once
#include <random>

constexpr double EPSILON = 0.000001;
constexpr float GAMMA = 2.2;
constexpr int MAX_DEPTH = 50;
constexpr int PROGRESS_CHARS = 70;
constexpr int RUSSIAN_ROULETTE_MIN_DEPTH = 5;
constexpr double PI = 3.14159265358979323846;

using real_t = double;
using rng_t = std::minstd_rand;
