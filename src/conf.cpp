#pragma once
#include "conf.hpp"

namespace conf {
	float constants::PI = 3.14159265359f;

	int window::WIDTH = 1600;
	int window::HEIGHT = 900;
	int window::FPS = 144;
	float window::dt = 1.0f / FPS;

	sf::Color colors::backgroundColor = sf::Color(30, 30, 30);

	sf::Font fonts::mono_semibold;

	std::chrono::steady_clock::time_point time::now;

	void initializeConfig() {
		time::now = std::chrono::steady_clock::now();
		fonts::mono_semibold.loadFromFile("../resources/fonts/mono-semibold.ttf");
	}
}