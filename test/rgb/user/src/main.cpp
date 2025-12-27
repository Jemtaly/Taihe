/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file is a test file.
// NOLINTBEGIN
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <taihe/callback.hpp>
#include <taihe/object.hpp>
#include <taihe/unit.hpp>
#include <unordered_map>
#include <unordered_set>

#include "rgb.base.user.hpp"
#include "rgb.show.IBase.proj.1.hpp"
#include "rgb.show.IShape.proj.1.hpp"
#include "rgb.show.user.hpp"

#include "taihe/string.hpp"
#include "tester.hpp"

using namespace rgb::base;
using namespace rgb::show;
using namespace taihe;

template<typename... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overloads(Ts...) -> overloads<Ts...>;

string toString(ColorOrRGBOrName const &color)
{
    return color.visit<string>(overloads {
        [](static_tag_t<ColorOrRGBOrName::tag_t::rgb>, const RGB &val) {
            std::ostringstream oss;
            oss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(val.r) << std::setw(2)
                << static_cast<int>(val.g) << std::setw(2) << static_cast<int>(val.b);
            return oss.str();
        },
        [](static_tag_t<ColorOrRGBOrName::tag_t::name>, Name const &val) {
            return string(val);
        },
        [](static_tag_t<ColorOrRGBOrName::tag_t::color>, Color const &val) {
            return std::to_string(val.get_value());
        },
        [](static_tag_t<ColorOrRGBOrName::tag_t::undefined>, taihe::unit) {
            return "Undefined";
        },
    });
}

struct UserType {
    static inline std::unordered_set<UserType *> registry;

private:
    string id;
    ColorOrRGBOrName myColor;

public:
    auto getId()
    {
        return concat({"UserType", "(", id, ")"});
    }

    void userMethod()
    {
        std::cout << "User Method Called;" << std::endl;
    }

    void setColor(ColorOrRGBOrName const &color)
    {
        myColor = color;
    }

    ColorOrRGBOrName getColor()
    {
        return myColor;
    }

    UserType(string_view id, ColorOrRGBOrName const &color) : id(id), myColor(color)
    {
        std::cout << getId() << " made" << std::endl;
        registry.insert(this);
    }

    ~UserType()
    {
        std::cout << getId() << " deleted" << std::endl;
        registry.erase(this);
    }

    UserType(string_view id) : UserType(id, ColorOrRGBOrName::make_undefined())
    {
    }
};

static ColorOrRGBOrName color_114514 = ColorOrRGBOrName::make_rgb(RGB {0x11, 0x45, 0x14});
static ColorOrRGBOrName color_yellow = ColorOrRGBOrName::make_color(Color::key_t::yellow);
static ColorOrRGBOrName color_miku = ColorOrRGBOrName::make_name(Name::key_t::BLUE);
static ColorOrRGBOrName color_unknown = ColorOrRGBOrName::make_undefined();

void testUnion()
{
    std::cout << toString(color_114514) << std::endl;
    std::cout << toString(color_yellow) << std::endl;
    std::cout << toString(color_miku) << std::endl;
    std::cout << toString(color_unknown) << std::endl;

    if (Name *name_ptr = color_miku.get_name_ptr()) {
        std::cout << "color_miku is holding name, name is " << *name_ptr << std::endl;
    } else {
        std::cout << "Error" << std::endl;
        Tester::assert(false, "color_miku should hold a name");
    }

    if (color_miku.holds_name()) {
        Name name_ref = color_miku.get_name_ref();
        std::cout << "color_miku is holding name, name is " << name_ref << std::endl;
    } else {
        std::cout << "Error" << std::endl;
        Tester::assert(false, "color_miku should hold a name");
    }

    color_miku.emplace_rgb(RGB {0x39, 0xC5, 0xBB});

    std::cout << toString(color_miku) << std::endl;

    switch (color_miku.get_tag()) {
        case ColorOrRGBOrName::tag_t::color:
            std::cout << "color_miku is holding color" << std::endl;
            break;
        case ColorOrRGBOrName::tag_t::rgb:
            std::cout << "color_miku is holding rgb" << std::endl;
            break;
        case ColorOrRGBOrName::tag_t::name:
            std::cout << "color_miku is holding name" << std::endl;
            break;
        default:
            std::cout << "color_miku is holding other stuff" << std::endl;
            break;
    }

    Tester::assert(color_miku.get_tag() == ColorOrRGBOrName::tag_t::rgb, "color_miku should hold rgb, got %s",
                   toString(color_miku).c_str());
}

void testInterfaceCall()
{
    IShowable colored_rect = makeColoredRectangle("Rect", color_yellow, 5, 5);

    Tester::assert(weak::IColorable(colored_rect)->getColor() == color_yellow,
                   "Colored Rectangle should have color %s, got %s", toString(color_yellow).c_str(),
                   toString(weak::IColorable(colored_rect)->getColor()).c_str());

    copyColor(colored_rect, make_holder<UserType, IColorable>("Circ", color_114514));

    Tester::assert(weak::IColorable(colored_rect)->getColor() == color_114514,
                   "Colored Rectangle should have color %s, got %s", toString(color_114514).c_str(),
                   toString(weak::IColorable(colored_rect)->getColor()).c_str());
}

void testInterfaceCast()
{
    IBase ibase_a = makeColoredRectangle("A", color_yellow, 1, 2);

    Tester::assert(!weak::IColorable(ibase_a).is_error(), "Dynamic cast from %s to IColorable should succeed",
                   ibase_a->getId().c_str());

    IBase ibase_b = makeRectangle("B", 3, 4);

    Tester::assert(weak::IColorable(ibase_b).is_error(), "Dynamic cast from %s to IColorable should fail",
                   ibase_b->getId().c_str());
}

void testArray()
{
    std::size_t m = 5;
    std::size_t n = 2;

    auto x = make_holder<UserType, IBase>("x");
    auto y = make_holder<UserType, IBase>("y");

    auto dst = array<IBase>::make(m, x);
    auto src = array<IBase>::make(n, y);

    auto res = exchangeArr(dst, src);

    Tester::assert(dst.size() == m, "dst size should be %zu, got %zu", m, dst.size());
    Tester::assert(src.size() == n, "src size should be %zu, got %zu", n, src.size());
    Tester::assert(res.size() == n, "res size should be %zu, got %zu", n, res.size());

    for (size_t i = 0; i < n; i++) {
        Tester::assert(src[i] == y, "src[%zu] should be %s, got %s", i, y->getId().c_str(), src[i]->getId().c_str());
        Tester::assert(dst[i] == y, "dst[%zu] should be %s, got %s", i, y->getId().c_str(), dst[i]->getId().c_str());
        Tester::assert(res[i] == x, "res[%zu] should be %s, got %s", i, x->getId().c_str(), res[i]->getId().c_str());
    }
    for (size_t i = n; i < m; i++) {
        Tester::assert(dst[i] == x, "dst[%zu] should be %s, got %s", i, x->getId().c_str(), dst[i]->getId().c_str());
    }
}

void testOptional()
{
    auto some = optional<IBase>(std::in_place, make_holder<UserType, IBase>("some"));
    auto some_str = getIdFromOptional(some);
    Tester::assert(some_str.has_value(), "some_str should have value");
    Tester::assert(some_str.value() == some.value()->getId(), "some_str should be %s, got %s",
                   some.value()->getId().c_str(), some_str.value().c_str());

    auto none = optional<IBase>(std::nullopt);
    auto none_str = getIdFromOptional(none);
    Tester::assert(!none_str.has_value(), "none_str should not have value");
}

void testVector()
{
    {
        array<IBase> src = {
            make_holder<UserType, IBase>("a"),
            make_holder<UserType, IBase>("b"),
            make_holder<UserType, IBase>("c"),
        };

        vector<IBase> res = makeVec(src);
        Tester::assert(res.size() == src.size(), "Vector result size should be %zu, got %zu", src.size(), res.size());
        for (size_t i = 0; i < src.size(); i++) {
            Tester::assert(res[i]->getId() == src[i]->getId(), "res[%zu] should be %s, got %s", i,
                           src[i]->getId().c_str(), res[i]->getId().c_str());
        }

        vector<IBase> buf;
        fillVec(src, buf);
        Tester::assert(buf.size() == src.size(), "Vector buffer size should be %zu, got %zu", src.size(), buf.size());
        for (size_t i = 0; i < src.size(); i++) {
            Tester::assert(buf[i]->getId() == src[i]->getId(), "buf[%zu] should be %s, got %s", i,
                           src[i]->getId().c_str(), buf[i]->getId().c_str());
        }
    }

    // empty/size + at/[]
    {
        vector<int> v;
        Tester::assert(v.empty(), "new vector should be empty");
        Tester::assert(v.size() == 0, "new vector size should be 0");

        {
            bool ok = false;
            try {
                (void)v.at(0);
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "at(0) on empty should throw out_of_range");
        }
        {
            bool ok = false;
            try {
                (void)v[0];
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "operator on empty should throw out_of_range");
        }

        v.push_back(10);
        Tester::assert(!v.empty(), "vector should not be empty after push_back");
        Tester::assert(v.size() == 1, "size should be 1 after push_back");
        Tester::assert(v.at(0) == 10, "at(0) should be 10");

        {
            bool ok = false;
            try {
                (void)v.at(1);
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "at(1) should throw out_of_range when size=1");
        }
    }

    // set
    {
        vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);

        v.set(1, 20);
        Tester::assert(v.size() == 3, "set should not change size");
        Tester::assert(v.at(0) == 1 && v.at(1) == 20 && v.at(2) == 3, "set should update element at idx");

        {
            bool ok = false;
            try {
                v.set(3, 30);
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "set(idx==size) should throw out_of_range");
        }
    }

    // insert/push_back/pop_back/remove/clear
    {
        vector<int> v;

        // pop_back on empty
        {
            bool ok = false;
            try {
                v.pop_back();
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "pop_back on empty should throw out_of_range");
        }
        // remove on empty
        {
            bool ok = false;
            try {
                v.remove(0);
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "remove(0) on empty should throw out_of_range");
        }

        // insert(0) on empty ==> push_back
        {
            bool ok = true;
            try {
                v.insert(0, 1);
            } catch (...) {
                ok = false;
            }
            Tester::assert(ok, "insert(0) on empty should succeed");
            Tester::assert(v.size() == 1 && v.at(0) == 1, "after insert(0,1), vector should be [1]");
        }

        // insert(size) ==> push_back
        {
            bool ok = true;
            try {
                v.insert(v.size(), 3);
            } catch (...) {
                ok = false;
            }
            Tester::assert(ok, "insert(size, val) should succeed");
            Tester::assert(v.size() == 2 && v.at(0) == 1 && v.at(1) == 3,
                           "after insert(size,3), vector should be [1,3]");
        }

        // insert middle
        v.insert(1, 2);  // [1,2,3]
        Tester::assert(v.size() == 3, "insert middle should increase size");
        Tester::assert(v.at(0) == 1 && v.at(1) == 2 && v.at(2) == 3, "insert should shift right");

        // insert(idx > size)
        {
            bool ok = false;
            try {
                v.insert(v.size() + 1, 4);
            } catch (std::out_of_range const &) {
                ok = true;
            } catch (...) {
            }
            Tester::assert(ok, "insert(idx>size) should throw out_of_range");
        }

        v.remove(1);  // [1,3]
        Tester::assert(v.size() == 2, "remove should decrease size");
        Tester::assert(v.at(0) == 1 && v.at(1) == 3, "remove should shift left");

        v.pop_back();  // [1]
        Tester::assert(v.size() == 1 && v.at(0) == 1, "pop_back should remove tail");

        v.clear();
        Tester::assert(v.size() == 0 && v.empty(), "clear should make vector empty");
    }

    // fill
    {
        vector<int> v;
        v.push_back(7);
        v.push_back(8);
        v.push_back(9);
        v.fill(42);
        Tester::assert(v.size() == 3, "fill should not change size");
        Tester::assert(v.at(0) == 42 && v.at(1) == 42 && v.at(2) == 42, "fill should overwrite all elements");
    }

    // find/npos
    {
        vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(2);
        v.push_back(3);

        Tester::assert(v.find(1) == 0, "find(1) should be 0");
        Tester::assert(v.find(2) == 1, "find(2) should return first match");
        Tester::assert(v.find(2, 2) == 2, "find(2,2) should be 2");
        Tester::assert(v.find(4) == vector<int>::npos, "find(4) should be npos");
        Tester::assert(v.find(1, 4) == vector<int>::npos, "find(start==size) should be npos");
        Tester::assert(v.find(1, 5) == vector<int>::npos, "find(start>size) should be npos");
    }

    // iter：LIVE + fail-fast
    {
        vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);

        {
            auto it = v.iter();
            Tester::assert(!it.is_end(), "iter() should not be end at start");
            Tester::assert(it.get() == 1, "first element should be 1");

            v.set(1, 20);
            it.move_next();
            Tester::assert(it.get() == 20, "iterator should observe updated value (LIVE)");
        }

        {
            auto it = v.iter();
            v.push_back(4);
            bool ok = false;
            try {
                (void)it.get();
            } catch (std::logic_error const &e) {
                ok = true;
            }
            Tester::assert(ok, "iterator.get after push_back should fail-fast");
        }

        {
            auto it = v.iter();
            v.remove(0);
            bool ok = false;
            try {
                it.move_next();
            } catch (std::logic_error const &e) {
                ok = true;
            }
            Tester::assert(ok, "iterator.move_next after remove should fail-fast");
        }

        {
            vector<int> w;
            w.push_back(1);
            auto it = w.iter();
            Tester::assert(!it.is_end(), "single-element iter start not end");
            Tester::assert(it.get() == 1, "single-element get should be 1");
            it.move_next();
            Tester::assert(it.is_end(), "after move_next, iterator should be end");

            {
                bool ok = false;
                try {
                    (void)it.get();
                } catch (std::out_of_range const &) {
                    ok = true;
                }
                Tester::assert(ok, "iterator.get at end should throw");
            }
            {
                bool ok = false;
                try {
                    it.move_next();
                } catch (std::out_of_range const &) {
                    ok = true;
                }
                Tester::assert(ok, "iterator.move_next at end should throw");
            }
        }
    }
}

void testMap()
{
    // empty map
    {
        taihe::map<int, taihe::string> m;

        Tester::assert(m.size() == 0, "Empty map size should be 0, got %zu", m.size());
        Tester::assert(!m.contains(42), "Empty map contains should be false");

        auto it = m.iter();
        Tester::assert(it.is_end(), "Empty map iterator should be at end");
    }

    // set + get + contains + size
    {
        taihe::map<int, taihe::string> m;

        m.set(1, "one");
        m.set(2, "two");
        m.set(3, "three");

        Tester::assert(m.size() == 3, "Map size after 3 set() should be 3, got %zu", m.size());
        Tester::assert(m.contains(1), "Map should contain key=1");
        Tester::assert(m.contains(2), "Map should contain key=2");
        Tester::assert(m.contains(3), "Map should contain key=3");
        Tester::assert(!m.contains(99), "Map should NOT contain key=99");

        taihe::string out = m[2];
        Tester::assert(out == "two", "Map[2] should return \"two\", got \"%s\"", out.c_str());

        bool threw = false;
        try {
            (void)m[99];
        } catch (std::out_of_range const &) {
            threw = true;
        }
        Tester::assert(threw, "Map get(missing_key) should throw (out_of_range)");
    }

    // insert
    {
        taihe::map<int, taihe::string> m;

        m.set(1, "one");
        m.set(2, "two");

        bool inserted = m.insert(2, "TWO");
        Tester::assert(!inserted, "Map insert(existing_key) should return false");
        Tester::assert(m[2] == "two",
                       "Map insert(existing_key) should NOT change existing value, "
                       "map[2] should be \"two\", got \"%s\"",
                       m[2].c_str());

        inserted = m.insert(3, "three");
        Tester::assert(inserted, "Map insert(new_key) should return true");
        Tester::assert(m.size() == 3,
                       "Map size after insert(new_key) should increase, "
                       "size should be 3, got %zu",
                       m.size());
        Tester::assert(m.get(3) == "three",
                       "Map insert(new_key) should write value correctly, "
                       "map[3] should be \"three\", got \"%s\"",
                       m[3].c_str());
    }

    // remove / clear
    {
        taihe::map<int, taihe::string> m;

        m.set(1, "one");
        m.set(2, "two");
        m.set(3, "three");

        bool removed = m.remove(2);
        Tester::assert(removed, "Map remove(existing_key) should return true, got false");
        Tester::assert(!m.contains(2), "Map removed key=2 should NOT be contained");
        Tester::assert(m.size() == 2,
                       "Map size after remove(existing_key) should decrease, "
                       "size should be 2, got %zu",
                       m.size());

        removed = m.remove(42);
        Tester::assert(!removed, "Map remove(missing_key) should return false, got true");
        Tester::assert(m.size() == 2,
                       "Map size after remove(missing_key) should NOT change, "
                       "size should be 2, got %zu",
                       m.size());

        m.clear();
        Tester::assert(m.size() == 0, "Map size after clear() should be 0, got %zu", m.size());
        Tester::assert(!m.contains(1) && !m.contains(3), "Map map should contain NO keys after clear()");
    }

    // Iteration
    {
        taihe::array<taihe::string> keys = {"a", "b", "c", "a"};
        taihe::array<IBase> src = {
            make_holder<UserType, IBase>("a"),
            make_holder<UserType, IBase>("b"),
            make_holder<UserType, IBase>("c"),
            make_holder<UserType, IBase>("d"),
        };

        std::unordered_map<std::string, IBase> expected;
        size_t n = std::min(keys.size(), src.size());
        for (size_t i = 0; i < n; i++) {
            expected.emplace(std::string(keys[i]), src[i]);
        }

        taihe::map<taihe::string, IBase> res = makeMap(keys, src);
        Tester::assert(res.size() == expected.size(), "Map result size should be %zu, got %zu", expected.size(),
                       res.size());
        for (auto const &[key, value] : expected) {
            bool has_val = res.contains(key);
            Tester::assert(has_val, "Map should contain key=%s", key.c_str());
            Tester::assert(res[key]->getId() == value->getId(), "Map[%s] should be %s, got %s", key.c_str(),
                           value->getId().c_str(), res[key]->getId().c_str());
        }

        taihe::map<taihe::string, IBase> buf;
        fillMap(keys, src, buf);
        Tester::assert(buf.size() == expected.size(), "Map buffer size should be %zu, got %zu", expected.size(),
                       buf.size());
        for (auto const &[key, value] : expected) {
            bool has_val = buf.contains(key);
            Tester::assert(has_val, "buffer should contain key=%s", key.c_str());
            Tester::assert(res.get(key)->getId() == value->getId(), "buffer[%s] should be %s, got %s", key.c_str(),
                           value->getId().c_str(), res[key]->getId().c_str());
        }
    }

    // iterator fail-fast
    {
        taihe::map<int, taihe::string> m;
        m.set(1, "one");
        m.set(2, "two");
        m.set(3, "three");

        auto it = m.iter();
        if (!it.is_end()) {
            bool inserted = m.insert(42, "forty-two");
            (void)inserted;

            bool threw = false;
            try {
                ++it;
            } catch (std::logic_error const &) {
                threw = true;
            }

            Tester::assert(threw, "Iterator should fail-fast after structural modification");
        }
    }

    // nested map
    {
        taihe::map<int, taihe::map<taihe::string, taihe::string>> m;

        taihe::map<taihe::string, taihe::string> inner;
        inner.set("hello", "world");
        m.set(1, inner);

        Tester::assert(m.size() == 1, "Outer map size should be 1");
        Tester::assert(m.contains(1), "Outer map should contain key=1");
        Tester::assert(!m.contains(2), "Outer map should NOT contain key=2");

        auto inner_retrieved = m[1];
        Tester::assert(inner_retrieved.size() == 1, "inner map size should be 1");
        Tester::assert(inner_retrieved.contains("hello"), "Inner map should contain key=\"hello\"");
        Tester::assert(!inner_retrieved.contains("foo"), "Inner map should NOT contain key=\"foo\"");

        taihe::string val = inner_retrieved["hello"];
        Tester::assert(val == "world", "Inner map[\"hello\"] should return \"world\", got \"%s\"", val.c_str());

        inner_retrieved.set("foo", "bar");
        Tester::assert(m[1].contains("foo"), "Modifying retrieved inner map should affect outer map");
    }
}

void testSet()
{
    // empty set
    {
        taihe::set<int> s;

        Tester::assert(s.size() == 0, "Empty set size should be 0, got %zu", s.size());
        Tester::assert(s.empty(), "Empty set empty() should be true");
        Tester::assert(!s.contains(42), "Empty set contains should be false");

        auto it = s.iter();
        Tester::assert(it.is_end(), "Empty set iterator should be at end");
    }

    // insert + contains + size
    {
        taihe::set<int> s;

        bool inserted = s.insert(1);
        Tester::assert(inserted, "Set insert(1) on empty set should return true");

        inserted = s.insert(2);
        Tester::assert(inserted, "Set insert(2) on set {1} should return true");

        inserted = s.insert(3);
        Tester::assert(inserted, "Set insert(3) on set {1,2} should return true");

        Tester::assert(s.size() == 3, "Set size after 3 insert() should be 3, got %zu", s.size());
        Tester::assert(s.contains(1), "Set should contain value=1");
        Tester::assert(s.contains(2), "Set should contain value=2");
        Tester::assert(s.contains(3), "Set should contain value=3");
        Tester::assert(!s.contains(99), "Set should NOT contain value=99");

        // duplicate insert
        inserted = s.insert(2);
        Tester::assert(!inserted, "Set insert(duplicate_value) should return false");
        Tester::assert(s.size() == 3,
                       "Set size after duplicate insert() should NOT change, "
                       "size should be 3, got %zu",
                       s.size());
    }

    // remove / clear
    {
        taihe::set<int> s;

        s.insert(1);
        s.insert(2);
        s.insert(3);

        bool removed = s.remove(2);
        Tester::assert(removed, "Set remove(existing_value) should return true, got false");
        Tester::assert(!s.contains(2), "Set removed value=2 should NOT be contained");
        Tester::assert(s.size() == 2,
                       "Set size after remove(existing_value) should decrease, "
                       "size should be 2, got %zu",
                       s.size());

        removed = s.remove(42);
        Tester::assert(!removed, "Set remove(missing_value) should return false, got true");
        Tester::assert(s.size() == 2,
                       "Set size after remove(missing_value) should NOT change, "
                       "size should be 2, got %zu",
                       s.size());

        s.clear();
        Tester::assert(s.size() == 0, "Set size after clear() should be 0, got %zu", s.size());
        Tester::assert(!s.contains(1) && !s.contains(3), "Set should contain NO values after clear()");
    }

    // Iteration
    {
        taihe::array<taihe::string> vals = {"a", "b", "c", "a"};

        std::unordered_set<std::string> expected;
        for (size_t i = 0; i < vals.size(); ++i) {
            expected.emplace(std::string(vals[i]));
        }

        taihe::set<taihe::string> res;
        for (size_t i = 0; i < vals.size(); ++i) {
            res.insert(vals[i]);
        }

        Tester::assert(res.size() == expected.size(), "Set result size should be %zu, got %zu", expected.size(),
                       res.size());

        for (auto const &v : expected) {
            Tester::assert(res.contains(v), "Set should contain value=%s", v.c_str());
        }

        std::unordered_set<std::string> actual;
        auto it = res.iter();
        while (!it.is_end()) {
            taihe::string cur = *it;
            actual.emplace(cur.c_str());
            ++it;
        }

        Tester::assert(actual == expected, "Set iteration should visit all elements exactly once");

        std::unordered_set<std::string> actual2;
        for (auto const &cur : res) {
            actual2.emplace(cur.c_str());
        }

        Tester::assert(actual2 == expected, "Set range-for iteration should visit all elements exactly once");
    }

    // iterator fail-fast
    {
        taihe::set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);

        auto it = s.iter();
        if (!it.is_end()) {
            bool inserted = s.insert(42);
            (void)inserted;

            bool threw = false;
            try {
                ++it;
            } catch (std::logic_error const &) {
                threw = true;
            }

            Tester::assert(threw,
                           "Set iterator should fail-fast after structural "
                           "modification");
        }
    }

    {
        taihe::array<taihe::string> src = {"a", "b", "c", "a"};

        std::unordered_set<std::string> expected;
        for (size_t i = 0; i < src.size(); i++) {
            expected.emplace(std::string(src[i]));
        }

        taihe::set<taihe::string> res = makeSet(src);
        Tester::assert(res.size() == expected.size(), "Set result size should be %zu, got %zu", expected.size(),
                       res.size());

        for (auto const &key : expected) {
            Tester::assert(res.contains(key), "Set should contain key %s", key.c_str());
        }

        taihe::set<taihe::string> buf;
        fillSet(src, buf);
        Tester::assert(buf.size() == expected.size(), "Set buffer size should be %zu, got %zu", expected.size(),
                       buf.size());

        for (auto const &key : expected) {
            Tester::assert(buf.contains(key), "buffer should contain key %s", key.c_str());
        }
    }
}

struct MyCallback {
    string f;

    MyCallback(string_view f) : f(f)
    {
        std::cout << "Callback " << f << " made" << std::endl;
    }

    ~MyCallback()
    {
        std::cout << "Callback " << f << " deleted" << std::endl;
    }

    string operator()(string_view a, string_view b)
    {
        std::cout << "Callback " << f << " called" << std::endl;
        return std::string(f) + "(" + a.c_str() + ", " + b.c_str() + ")";
    }
};

void testCallback()
{
    auto curried = currying(make_holder<MyCallback, callback<string(string_view, string_view)>>("f"));
    auto f = curried("abc");
    auto x = f("123");
    auto y = f("456");

    auto expected_x = MyCallback("f")("abc", "123");
    Tester::assert(x == expected_x, "x should be %s, got %s", expected_x.c_str(), x.c_str());

    auto expected_y = MyCallback("f")("abc", "456");
    Tester::assert(y == expected_y, "y should be %s, got %s", expected_y.c_str(), y.c_str());
}

struct AutoCompareType {
    string id;

    AutoCompareType(string_view id) : id(id)
    {
        std::cout << "AutoCompareType " << id << " made" << std::endl;
    }

    string getId() const
    {
        return id;
    }
};

struct UserCompareType {
    string id;

    UserCompareType(string_view id) : id(id)
    {
        std::cout << "UserCompareType " << id << " made" << std::endl;
    }

    string getId() const
    {
        return id;
    }
};

template<>
struct taihe::same_impl_t<UserCompareType> {
    bool operator()(data_view lhs, data_view rhs) const
    {
        auto lhs_with_id = weak::IBase(lhs);
        auto rhs_with_id = weak::IBase(rhs);
        if (lhs_with_id.is_error() || rhs_with_id.is_error()) {
            return same_impl<void>(lhs, rhs);
        }
        return lhs_with_id->getId() == rhs_with_id->getId();
    }
};

template<>
struct taihe::hash_impl_t<UserCompareType> {
    std::size_t operator()(data_view val) const
    {
        auto val_with_id = weak::IBase(val);
        if (val_with_id.is_error()) {
            return hash_impl<void>(val);
        }
        return std::hash<std::string_view> {}(val_with_id->getId());
    }
};

void testCompare()
{
    // AutoCompareType uses default comparison
    map<IBase, string> auto_compare_map;
    auto_compare_map.insert(make_holder<AutoCompareType, IBase>("a"), "a");
    auto_compare_map.insert(make_holder<AutoCompareType, IBase>("b"), "b");
    auto_compare_map.insert(make_holder<AutoCompareType, IBase>("a"), "c");
    std::cout << "AutoCompareMap size: " << auto_compare_map.size() << std::endl;
    for (auto const &[key, value] : auto_compare_map) {
        std::cout << "AutoCompareMap: " << key->getId() << " -> " << value << std::endl;
    }

    // UserCompareType uses custom comparison
    map<IBase, string> user_compare_map;
    user_compare_map.insert(make_holder<UserCompareType, IBase>("a"), "a");
    user_compare_map.insert(make_holder<UserCompareType, IBase>("b"), "b");
    user_compare_map.insert(make_holder<UserCompareType, IBase>("a"), "c");
    std::cout << "UserCompareMap size: " << user_compare_map.size() << std::endl;
    for (auto const &[key, value] : user_compare_map) {
        std::cout << "UserCompareMap: " << key->getId() << " -> " << value << std::endl;
    }

    Tester::assert(auto_compare_map.size() == 3, "AutoCompareMap should have 3 items, got %zu",
                   auto_compare_map.size());
    Tester::assert(user_compare_map.size() == 2, "UserCompareMap should have 2 items, got %zu",
                   user_compare_map.size());
}

void testHashAndSame()
{
    auto a = make_holder<UserType, IBase>("a");
    std::cout << "Hash of a: " << std::hash<IBase>()(a) << std::endl;
    std::cout << "Comparing a with itself: " << std::boolalpha << (a == a) << std::endl;

    taihe::array<IBase> arr = {a, a, a};
    std::cout << "Hash of array with three a's: " << std::hash<array<IBase>>()(arr) << std::endl;
    std::cout << "Comparing array with three a's with itself: " << std::boolalpha << (arr == arr) << std::endl;

    taihe::optional<IBase> opt(std::in_place, a);
    std::cout << "Hash of optional with a: " << std::hash<optional<IBase>>()(opt) << std::endl;
    std::cout << "Comparing optional with a with itself: " << std::boolalpha << (opt == opt) << std::endl;

    taihe::vector<IBase> vec;
    vec.push_back(a);
    std::cout << "Hash of vector with a: " << std::hash<vector<IBase>>()(vec) << std::endl;
    std::cout << "Comparing vector with a with itself: " << std::boolalpha << (vec == vec) << std::endl;

    taihe::set<IBase> s;
    s.insert(a);
    std::cout << "Hash of set with a: " << std::hash<set<IBase>>()(s) << std::endl;
    std::cout << "Comparing set with a with itself: " << std::boolalpha << (s == s) << std::endl;

    taihe::map<IBase, IBase> m;
    m.insert(a, a);
    std::cout << "Hash of map with a: " << std::hash<map<IBase, IBase>>()(m) << std::endl;
    std::cout << "Comparing map with a with itself: " << std::boolalpha << (m == m) << std::endl;

    taihe::callback<string(string_view, string_view)> cb(
        make_holder<MyCallback, callback<string(string_view, string_view)>>("cb"));
    std::cout << "Hash of callback: " << std::hash<callback<string(string_view, string_view)>>()(cb) << std::endl;
    std::cout << "Comparing callback with itself: " << std::boolalpha << (cb == cb) << std::endl;
}

void testMemoryLeak()
{
    size_t remaining = 0;

    for (auto *user : UserType::registry) {
        std::cout << user->getId() << " is still alive" << std::endl;
        remaining++;
    }

    Tester::assert(remaining == 0, "Memory leak detected: %zu UserType objects are still alive", remaining);
}

int main()
{
    Tester tester;

    tester.run("testUnion", testUnion);
    tester.run("testInterfaceCall", testInterfaceCall);
    tester.run("testInterfaceCast", testInterfaceCast);
    tester.run("testArray", testArray);
    tester.run("testOptional", testOptional);
    tester.run("testVector", testVector);
    tester.run("testMap", testMap);
    tester.run("testSet", testSet);
    tester.run("testCallback", testCallback);
    tester.run("testCompare", testCompare);
    tester.run("testHashAndSame", testHashAndSame);
    tester.run("testMemoryLeak", testMemoryLeak);

    return tester.report();
}

// NOLINTEND
