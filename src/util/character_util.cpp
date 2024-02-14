#include "util/character_util.hpp"
#include "util/wz_util.hpp"

namespace util
{
    CharacterUtil::CharacterUtil()
    {
        _character_node = WzUtil::current()->Character->get_root();

        _sprite_util = SpriteUtil::current();

        {
            auto body_node = _character_node->find_from_path(u"00002000.img");
            auto head_node = _character_node->find_from_path(u"00012000.img");
            for (auto stance_node : body_node->get_children())
            {
                auto ststr = stance_node.first;

                uint16_t attackdelay = 0;

                for (uint8_t frame = 0; auto frame_node = stance_node.second[0]->get_child(std::to_string(frame)); ++frame)
                {
                    if (frame_node->get_child(u"action") != nullptr)
                    {
                        /* code */
                    }
                    else
                    {
                        auto type = type_map.at(ststr);
                        auto delay_node = frame_node->get_child(u"delay");
                        auto delay = 100;
                        if (delay_node != nullptr)
                        {
                            delay = dynamic_cast<wz::Property<int> *>(delay_node)->get();
                        }
                        stance_delays[type][frame] = delay;

                        std::unordered_map<Layer, std::unordered_map<std::u16string, Point<int32_t>>> body_shift_map;

                        for (auto part_node : frame_node->get_children())
                        {
                            auto part_name = part_node.first;

                            if (part_name != u"delay" && part_name != u"face")
                            {
                                auto part = part_node.second[0];
                                if (part->type == wz::Type::UOL)
                                {
                                    part = dynamic_cast<wz::Property<wz::WzUOL> *>(part)->get_uol();
                                }

                                auto zstr = dynamic_cast<wz::Property<wz::wzstring> *>(part->get_child(u"z"))->get();
                                auto z = layer_map.at(zstr);

                                for (auto map_node : part->get_child(u"map")->get_children())
                                {
                                    auto v = dynamic_cast<wz::Property<wz::WzVec2D> *>(map_node.second[0])->get();

                                    body_shift_map[z].emplace(map_node.first, Point(v.x, v.y));
                                }
                            }
                        }
                        std::string frame_str = std::to_string(frame);
                        auto h = head_node->find_from_path(ststr + u"/" + std::u16string{frame_str.begin(), frame_str.end()} + u"/" + u"head");
                        if (h != nullptr)
                        {
                            if (h->type == wz::Type::UOL)
                            {
                                h = dynamic_cast<wz::Property<wz::WzUOL> *>(h)->get_uol();
                            }
                            for (auto map_node : h->get_child(u"map")->get_children())
                            {
                                auto v = dynamic_cast<wz::Property<wz::WzVec2D> *>(map_node.second[0])->get();

                                body_shift_map[Layer::HEAD].emplace(map_node.first, Point(v.x, v.y));
                            }
                        }

                        body_positions[type][frame] = body_shift_map[Layer::BODY][u"navel"];

                        arm_positions[type][frame] = body_shift_map.count(Layer::ARM) ? (body_shift_map[Layer::ARM][u"hand"] - body_shift_map[Layer::ARM][u"navel"] + body_shift_map[Layer::BODY][u"navel"])
                                                                                      : (body_shift_map[Layer::ARM_OVER_HAIR][u"hand"] - body_shift_map[Layer::ARM_OVER_HAIR][u"navel"] + body_shift_map[Layer::BODY][u"navel"]);
                        hand_positions[type][frame] = body_shift_map[Layer::HAND_BELOW_WEAPON][u"handMove"];
                        head_positions[type][frame] = body_shift_map[Layer::BODY][u"neck"] - body_shift_map[Layer::HEAD][u"neck"];
                        face_positions[type][frame] = body_shift_map[Layer::BODY][u"neck"] - body_shift_map[Layer::HEAD][u"neck"] + body_shift_map[Layer::HEAD][u"brow"];
                        hair_positions[type][frame] = body_shift_map[Layer::HEAD][u"brow"] - body_shift_map[Layer::HEAD][u"neck"] + body_shift_map[Layer::BODY][u"neck"];
                    }
                }
            }
        }
    }

    std::vector<Sprite> CharacterUtil::load()
    {
        std::vector<Sprite> v;
        auto a = body_positions[Type::STAND1][0];
        auto b = arm_positions[Type::STAND1][0];
        auto c = hand_positions[Type::STAND1][0];
        auto d = head_positions[Type::STAND1][0];
        auto e = face_positions[Type::STAND1][0];
        auto f = hair_positions[Type::STAND1][0];

        auto body_node = _character_node->find_from_path(u"00002000.img");
        auto head_node = _character_node->find_from_path(u"00012000.img");

        Sprite body = _sprite_util->load_sprite(body_node->find_from_path(u"stand1/0/body"));
        Sprite arm = _sprite_util->load_sprite(body_node->find_from_path(u"stand1/0/arm"), b.x() + 1, b.y() - 5);
        Sprite head = _sprite_util->load_sprite(head_node->find_from_path(u"stand1/0/head"), d.x(), d.y());
        Sprite face = _sprite_util->load_sprite(_character_node->find_from_path(u"Face/00020000.img/default/face"), e.x() + 1, e.y() + 12);
        Sprite hair = _sprite_util->load_sprite(_character_node->find_from_path(u"Hair/00030000.img/default/hair"), f.x(), f.y());
        Sprite hair_over_head = _sprite_util->load_sprite(_character_node->find_from_path(u"Hair/00030000.img/default/hairOverHead"), f.x(), f.y());
        Sprite coat = _sprite_util->load_sprite(_character_node->find_from_path(u"Coat/01040041.img/stand1/0/mail"), a.x() + 7, a.y() - 1);
        Sprite coat_arm = _sprite_util->load_sprite(_character_node->find_from_path(u"Coat/01040041.img/stand1/0/mailArm"), a.x() + 9, a.y() - 6);
        Sprite pants = _sprite_util->load_sprite(_character_node->find_from_path(u"Pants/01060000.img/stand1/0/pants"), a.x() + 6, a.y() + 7);
        Sprite shoes = _sprite_util->load_sprite(_character_node->find_from_path(u"Shoes/01070009.img/stand1/0/shoes"), a.x() + 3, a.y() + 20);

        v.push_back(body);
        v.push_back(arm);
        v.push_back(head);
        v.push_back(face);
        v.push_back(hair);
        v.push_back(hair_over_head);
        v.push_back(coat);
        v.push_back(coat_arm);
        v.push_back(pants);
        v.push_back(shoes);

        return v;
    }
}