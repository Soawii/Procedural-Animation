#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include "conf.hpp"
#include "engine/Engine.hpp"
#include "util/util.hpp"
#include "sim/Snake.hpp"
#include "sim/Lizard.hpp"
#include "sim/Fish.hpp"
#include "sim/Spider.hpp"
#include "sim/ManyLegs.hpp"
#include "UI/TextElement.hpp"
#include "UI/Flex.hpp"
#include "UI/Button.hpp"
#include "UI/Slider.hpp"

int main() {
    conf::initializeConfig();

    Engine engine;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    settings.stencilBits = 8;
    engine.createWindowHandler(
        sf::VideoMode(conf::window::WIDTH, conf::window::HEIGHT),
        "Double Pendulum Balancing",
        sf::Style::Default, settings);
    engine.m_context.m_window->setFramerateLimit(conf::window::FPS);
    engine.createSimulation();
    engine.createUIManager();

    Snake *snake = new Snake(sf::Vector2f(900.0f, 450.0f));
    Lizard *lizard = new Lizard(sf::Vector2f(900.0f, 450.0f));
    Fish *fish = new Fish(sf::Vector2f(900.0f, 450.0f));
    Spider *spider = new Spider(sf::Vector2f(900.0f, 450.0f));
    ManyLegs *manylegs = new ManyLegs(sf::Vector2f(900.0f, 450.0f));
    engine.m_context.m_sim->m_entities.push_back(snake);
    engine.m_context.m_sim->m_entities.push_back(lizard);
    engine.m_context.m_sim->m_entities.push_back(fish);
    engine.m_context.m_sim->m_entities.push_back(spider);
    engine.m_context.m_sim->m_entities.push_back(manylegs);


    UIManager *m = engine.m_UIManager;
    WindowElement* we = m->m_windowElement;

    Button* open_choice =  new Button();
    m->addElement(open_choice, we, we, "open_choice", {});
    open_choice->m_vars.pos.absolute.setAll({15.0f, 15.0f});
    open_choice->m_vars.size.absolute.setAll({300.0f, 0.0f});
    open_choice->m_vars.padding.setAll(16.0f);
    open_choice->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
    open_choice->m_context->fitContentFixed = true;
    open_choice->m_vars.color.setAll(ColorHSL(245,245,245));
    open_choice->m_vars.color.setState(UIElementState::HOVER, ColorHSL(220,220,222));
    open_choice->m_vars.color.setState(UIElementState::ACTIVE, ColorHSL(220,220,222));
    open_choice->m_vars.color.setState(UIElementState::PRESSED, ColorHSL(14,14,14));
    open_choice->m_vars.color.setDuration(0.1f);
    open_choice->m_vars.borderColor.setAll(ColorHSL(245,245,245));
    open_choice->m_vars.borderWidth.setAll(2.0f);
    open_choice->m_vars.borderRadius.setAll(10.0f);
    open_choice->m_context->pressable = true;
    TextElement* text_open_choice = new TextElement("Choose Entity", conf::fonts::mono_semibold, 20);
    m->addElement(text_open_choice, open_choice, open_choice, "open_choice_text", {});
    text_open_choice->m_vars.origin.relative.setAll({0.5f, 0.0f});
    text_open_choice->m_vars.pos.relative.setAll({0.5f, 0.0f});
    text_open_choice->m_vars.color.setAll(ColorHSL(14,14,14));
    text_open_choice->m_vars.color.setState(UIElementState::PRESSED, ColorHSL(245,245,245));
    text_open_choice->m_vars.color.setDuration(0.1f);
    text_open_choice->m_vars.color.setContext(open_choice->m_context);
    text_open_choice->m_context->dontUpdateYourself = true;

    Flex* choices_and_settings = new Flex();
    m->addElement(choices_and_settings, we, open_choice, "choices_and_settings", {});
    choices_and_settings->m_vars.pos.relative.setAll({0.0f, 1.0f});
    choices_and_settings->m_vars.pos.absolute.setAll({0.0f, 10.0f});
    choices_and_settings->m_vars.size.absolute.setAll({300.0f, 0.0f});
    choices_and_settings->m_vars.padding.setAll(10.0f);
    choices_and_settings->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
    choices_and_settings->m_vars.color.setAll(ColorHSL(245,245,245));
    choices_and_settings->m_vars.borderRadius.setAll(10.0f);
    choices_and_settings->m_direction = FlexDirection::vertical;
    choices_and_settings->m_gap = 10.0f;
    choices_and_settings->m_justify = JustifyContent::start;
    choices_and_settings->m_align = AlignItems::start;
    choices_and_settings->m_vars.opacity.setState(UIElementState::DISABLED, 0.0f);
    choices_and_settings->m_vars.translate.absolute.setState(UIElementState::DISABLED, {0.0f, 40.0f});
    choices_and_settings->setState(UIElementState::DISABLED);
    choices_and_settings->m_vars.opacity.setDuration(0.2f);
    choices_and_settings->m_vars.translate.absolute.setDuration(0.2f);
    open_choice->m_context->onPressChange.push_back([open_choice, choices_and_settings]() {
        if (open_choice->m_context->pressed)
            choices_and_settings->setState(UIElementState::NORMAL);
        else
            choices_and_settings->setState(UIElementState::DISABLED);
    });

    Flex* choices = new Flex();
    m->addElement(choices, choices_and_settings, choices_and_settings, "choices", {});
    choices->m_vars.size.relative.setAll({1.0f, 0.0f});
    choices->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
    choices->m_context->fitContentFixed = true;
    choices->m_vars.color.setAll(ColorHSL(0,0,0,0));
    choices->m_direction = FlexDirection::vertical;
    choices->m_gap = 5.0f;
    choices->m_justify = JustifyContent::start;
    choices->m_align = AlignItems::start;

    Flex* e_settings = new Flex();
    e_settings->m_vars.size.relative.setAll({1.0f, 0.0f});
    e_settings->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
    e_settings->m_vars.color.setAll(ColorHSL(0,0,0,0));
    e_settings->m_direction = FlexDirection::vertical;
    e_settings->m_justify = JustifyContent::start;
    e_settings->m_align = AlignItems::start;

    std::vector<Button*>* choice_radio_group = new std::vector<Button*>();
    auto choice_generator = [e_settings, m, we, choices, choice_radio_group, &engine](std::string title, int number) {
        Button* choice =  new Button();
        m->addElement(choice, choices, choices, "choice_" + title, {});
        choice->m_radio = true;
        choice->m_radioGroup = choice_radio_group;
        choice_radio_group->push_back(choice);
        choice->m_context->pressable = true;
        choice->m_vars.size.relative.setAll({1.0f, 0.0f});
        choice->m_vars.padding.setAll(16.0f);
        choice->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
        choice->m_context->fitContentFixed = true;
        choice->m_vars.color.setAll(ColorHSL(245,245,245));
        choice->m_vars.color.setState(UIElementState::HOVER, ColorHSL(220,220,222));
        choice->m_vars.color.setState(UIElementState::ACTIVE, ColorHSL(220,220,222));
        choice->m_vars.color.setState(UIElementState::PRESSED, ColorHSL(14,14,14));
        choice->m_vars.color.setDuration(0.1f);
        choice->m_vars.borderColor.setAll(ColorHSL(180,180,190));
        choice->m_vars.borderWidth.setAll(1.0f);
        choice->m_vars.borderRadius.setAll(10.0f);

        TextElement* text_open_choice = new TextElement(title, conf::fonts::mono_semibold, 20);
        m->addElement(text_open_choice, choice, choice, "choice_text_" + title, {});
        text_open_choice->m_vars.origin.relative.setAll({0.5f, 0.0f});
        text_open_choice->m_vars.pos.relative.setAll({0.5f, 0.0f});
        text_open_choice->m_vars.color.setContext(choice->m_context);
        text_open_choice->m_vars.color.setAll(ColorHSL(14,14,14));
        text_open_choice->m_vars.color.setState(UIElementState::PRESSED, ColorHSL(245,245,245));
        text_open_choice->m_vars.color.setDuration(0.1f);

        Flex* entity_setting = new Flex();
        m->addElement(entity_setting, e_settings, e_settings, "setting_speed_row_" + title, {});
        entity_setting->m_context->z_index = 5;
        entity_setting->m_vars.size.relative.setAll({1.0f, 0.0f});
        entity_setting->m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;
        entity_setting->m_vars.color.setAll(ColorHSL(0,0,0,0));
        entity_setting->m_direction = FlexDirection::horizontal;
        entity_setting->m_justify = JustifyContent::space_between;
        entity_setting->m_align = AlignItems::center;
        entity_setting->m_gap = 8.0f;
        entity_setting->m_vars.opacity.setState(UIElementState::DISABLED, 0.0f);
        entity_setting->setState(UIElementState::DISABLED);

        TextElement* text_speed = new TextElement("Speed", conf::fonts::mono_semibold, 20);
        m->addElement(text_speed, entity_setting, entity_setting, "settings_speed_text_" + title, {});
        text_speed->m_vars.color.setAll(ColorHSL(14,14,14));
        text_speed->m_context->dontUpdateYourself = true;

        Slider* slider = new Slider(600.0f, 10.0f, 3000.0f);
        slider->m_onValueChanged = [&engine, number, slider]() {
            engine.m_context.m_sim->m_entities[engine.m_context.m_sim->m_chosen_entity]->m_velocity = slider->m_var;
        };
        slider->m_vars.size.relative.setAll({0.7f, 0.0f});
        slider->m_sliderLine->m_vars.size.absolute.setAll({0.0f, 13.0f});
        slider->m_sliderLine->m_vars.color.setAll(ColorHSL(210,210,215));
        ColorHSL fill_color(9, 121, 105);
        slider->m_sliderLine->m_fillColor.setAll(fill_color);
        slider->m_sliderLine->m_fillColor.setState(UIElementState::HOVER, ColorHSL(fill_color.h, fill_color.s, fill_color.l + 0.05f, fill_color.a));
        slider->m_sliderLine->m_fillColor.setState(UIElementState::ACTIVE, ColorHSL(fill_color.h, fill_color.s, fill_color.l + 0.1f, fill_color.a));
        slider->m_sliderLine->m_fillColor.setDuration(0.085f);
        slider->m_sliderLine->m_fillColor.setContext(slider->m_context);
        slider->m_sliderPoint->m_vars.size.absolute.setAll({25.0f, 25.0f});
        slider->m_sliderPoint->m_vars.borderWidth.setAll(8.0f);
        slider->m_sliderPoint->m_vars.borderColor.setAll(fill_color);
        slider->m_sliderPoint->m_vars.borderColor.setState(UIElementState::HOVER, ColorHSL(fill_color.h, fill_color.s, fill_color.l + 0.05f, fill_color.a));
        slider->m_sliderPoint->m_vars.borderColor.setState(UIElementState::ACTIVE, ColorHSL(fill_color.h, fill_color.s, fill_color.l + 0.1f, fill_color.a));
        slider->m_sliderPoint->m_vars.borderColor.setDuration(0.085f);
        slider->m_sliderPoint->m_vars.borderColor.setContext(slider->m_context);
        slider->m_sliderPoint->m_vars.size.absolute.setState(UIElementState::HOVER, {29.0f, 29.0f});
        slider->m_sliderPoint->m_vars.size.absolute.setState(UIElementState::ACTIVE, {34.0f, 34.0f});
        slider->m_sliderPoint->m_vars.size.absolute.setDuration(0.085f);
        slider->m_sliderPoint->m_vars.size.absolute.setContext(slider->m_context);
        slider->m_sliderPoint->m_vars.color.setAll(ColorHSL(255,255,255));
        slider->m_sliderPoint->m_vars.borderRadius.setAll(100.0f);
        m->addElement(slider, entity_setting, entity_setting, "settings_speed_slider_" + title, {});

        choice->m_context->onPressChange.push_back([entity_setting, choice, number, &engine]() {
            if (choice->m_context->pressed) {
                engine.m_context.m_sim->m_chosen_entity = number;
                entity_setting->setState(UIElementState::HOVER);
            }
            else {
                entity_setting->setState(UIElementState::DISABLED);
            }
        });
    };
    choice_generator("Snake", 0);
    choice_generator("Lizard", 1);
    choice_generator("Fish", 2);
    choice_generator("Spider", 3);
    choice_generator("Whatever this is", 4);
    m->addElement(e_settings, choices_and_settings, choices_and_settings, "entity_settings", {});
    (*choice_radio_group)[0]->click();

    while (engine.m_context.m_window->isOpen()) {
        sf::Event event;

        engine.startFrame();
        while (engine.m_context.m_window->pollEvent(event)) {
            engine.handleEvent(event);
        }

        engine.update(conf::window::dt);

        engine.draw();

        engine.endFrame();
    }
    return 0;
}