#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <functional>

namespace conf {
    class constants {
    public:
        static float PI;
    };

    class window {
    public:
        static int WIDTH;
        static int HEIGHT;
        static int FPS;
        static float dt;
    };

    class fonts {
    public:
        static sf::Font mono_semibold;
    };

    class colors {
    public:
        static sf::Color backgroundColor;
    };

    class time {
    public:
        static std::chrono::steady_clock::time_point now;
    };

    void initializeConfig();
}