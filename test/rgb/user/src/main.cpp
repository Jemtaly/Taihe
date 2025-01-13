#include "rgb.base.proj.hpp"
#include "rgb.show.proj.hpp"

#include <iostream>
#include <cmath>
#include <iomanip>

using namespace rgb::base;
using namespace rgb::show;
using namespace taihe::core;

class ColoredCircle {
    float r;
    std::string name;

    ColorOrRGBOrName myColor;

public:
    ColoredCircle(string_view id, float r, ColorOrRGBOrName const& color)
        : name(id), r(r), myColor(color) {
        std::cout << "new " << this << std::endl;
    }

    ~ColoredCircle() {
        std::cout << "del " << this << std::endl;
    }

    string getId() {
        return name;
    }

    float calculateArea() {
        return M_PI * r * r;
    }

    ColorOrRGBOrName getColor() {
        return myColor;
    }

    void setColor(ColorOrRGBOrName const& color) {
        myColor = color;
    }

    void show() {
        std::string content = "circle " + name + ": r = " + std::to_string(r);
        if (auto ptr = myColor.get_ptr<ColorOrRGBOrName::tag_t::color>()) {
            std::cout << "\033[" << 30 + (int)ptr->get_tag() << "m" << content << "\033[39m" << std::endl;
        } else if (auto ptr = myColor.get_ptr<ColorOrRGBOrName::tag_t::rgb>()) {
            std::cout << "\033[38;2;" << (int)ptr->r << ";" << (int)ptr->g << ";" << (int)ptr->b << "m" << content << "\033[39m" << std::endl;
        } else if (auto ptr = myColor.get_ptr<ColorOrRGBOrName::tag_t::name>()) {
            std::cout << "(" << ptr->c_str() << ") " << content << std::endl;
        } else {
            std::cout << content << std::endl;
        }
    }
};

int main() {
    Color yellow = Color::make_yellow();
    ColorOrRGBOrName color_114514 = ColorOrRGBOrName::make_rgb(RGB{0x11, 0x45, 0x14});
    ColorOrRGBOrName color_yellow = ColorOrRGBOrName::make_color(yellow);
    ColorOrRGBOrName color_my_color = ColorOrRGBOrName::make_name("My Color");
    ColorOrRGBOrName color_unknown = ColorOrRGBOrName::make_undefined();

    std::cout << toString(color_114514).c_str() << std::endl;
    std::cout << toString(color_yellow).c_str() << std::endl;
    std::cout << toString(color_my_color).c_str() << std::endl;
    std::cout << toString(color_unknown).c_str() << std::endl;

    color_my_color.emplace_name("emplace color");
    std::cout << toString(color_my_color).c_str() << std::endl;

    ColorOrRGBOrName color_ptr = ColorOrRGBOrName::make_name("Color Name");
    auto* color_ptr_rgb_ptr = color_ptr.get_rgb_ptr();
    auto* color_unsafe_ptr_rgb_ptr = color_ptr.unsafe_get_rgb_ptr();

    if(color_ptr_rgb_ptr != nullptr)
    {
        auto color_rgb = ColorOrRGBOrName::make_rgb(*color_ptr_rgb_ptr);
        std::cout << toString(color_rgb).c_str() << std::endl;
    }
    else
    {
        std::cout << "NULL" << std::endl;
    }

    if(color_ptr.holds_name())
    {
        string* color_ptr_name_ptr = color_ptr.get_name_ptr();
        std::cout << color_ptr_name_ptr->c_str() << std::endl;
    }

    rgb::base::ColorOrRGBOrName::tag_t tag = color_my_color.get_tag();
    switch (color_ptr.get_tag()) {
        case ColorOrRGBOrName::tag_t::color:
            std::cout << "color_ptr has color" << std::endl;
            break;
        case ColorOrRGBOrName::tag_t::rgb:
            std::cout << "color_ptr has rgb" << std::endl;
            break;
        case ColorOrRGBOrName::tag_t::name:
            std::cout << "color_ptr has name" << std::endl;
            break;
        default:
            break;
        }

    // User implements the interface
    auto circle = make_holder<ColoredCircle, IShowable>("A", 10, color_114514);
    auto rect = makeColoredRectangle("B", color_yellow, 5, 5);
    Color color_single = Color::make_yellow();
    weak::IShowable circle_as_showable = circle;

    circle->show();
    circle_as_showable.show();
    rect.show();
    copyColor(rect, circle);
    rect.show();
}
