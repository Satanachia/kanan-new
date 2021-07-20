#include <imgui.h>

#include "Mabinogi.hpp"
#include "KCharacter.hpp"
#include "KItem.hpp"
#include "Kanan.hpp"
#include "EntityViewer.hpp"

using namespace std;

namespace kanan {
    void EntityViewer::onUI() {
        if (ImGui::CollapsingHeader("Character List")) {
            buildCharacterList();
            createCharacterTree();
        }

        if (ImGui::CollapsingHeader("Item List")) {
            buildItemList();
            createItemTree();
        }
    }

    void EntityViewer::buildCharacterList() {
        // Clear the current list.
        m_characters.clear();

        // Iterate over every character and add it to our list.
        auto game = g_kanan->getGame();
        auto entityList = game->getEntityList();

        if (entityList == nullptr) {
            ImGui::Text("Couldn't find the entity list.");
            return;
        }

        auto& characters = entityList->characters;
        auto highestIndex = characters.count;
        auto node = characters.root;

        for (uint32_t i = 0; i <= highestIndex && node != nullptr; ++i, node = node->next) {
            auto character = (KCharacter*)node->entry->character;

            if (character == nullptr || !character->getID()) {
                continue;
            }

            m_characters.emplace_front(character);
        }

        // Sort our list.
        m_characters.sort([](auto a, auto b) {
            return (a->getName().value_or("") < b->getName().value_or(""));
        });
    }

    void EntityViewer::buildItemList() {
        // Clear the current list.
        m_items.clear();

        // Iterate over every item and add it to our list.
        auto game = g_kanan->getGame();
        auto entityList = game->getEntityList();

        if (entityList == nullptr) {
            ImGui::Text("Couldn't find the entity list.");
            return;
        }

        auto& items = entityList->items;
        auto highestIndex = items.count;
        auto node = items.root;

        for (uint32_t i = 0; i <= highestIndex && node != nullptr; ++i, node = node->next) {
            auto item = (KItem*)node->entry->item;

            if (item == nullptr || !item->getID()) {
                continue;
            }

            m_items.emplace_front(item);
        }

        // Sort our list.
        m_items.sort([](auto a, auto b) {
            return (a->getName().value_or("") < b->getName().value_or(""));
        });
    }

    void EntityViewer::createCharacterTree() {
        if (m_characters.empty()) {
            ImGui::Text("There are no characters yet.");
            return;
        }

        for (auto& character : m_characters) {
            auto name = character->getName();

            if (!name) {
                continue;
            }

            if (ImGui::TreeNode(character, "%s", name->c_str())) {
                displayCharacter(character);
                ImGui::TreePop();
            }
        }
    }

    void EntityViewer::createItemTree() {
        if (m_items.empty()) {
            ImGui::Text("There are no items yet.");
            return;
        }

        for (auto& item : m_items) {
            auto name = item->getName();

            if (!name) {
                continue;
            }

            if (ImGui::TreeNode(item, "%s", name->c_str())) {
                displayItem(item);
                ImGui::TreePop();
            }
        }
    }

    void EntityViewer::displayCharacter(KCharacter* character) {
        auto id = character->getID();
        auto name = character->getName();
        auto pos = character->getPosition();
        auto parameter = character->parameter;

        if (!id || !name || !pos || parameter == nullptr) {
            ImGui::Text("This character has no data.");
            return;
        }

        auto target = g_kanan->getGame()->getCharacterByID(character->targetID);
        auto equipment = character->equipment;

        ImGui::BulletText("Address: %p", character);
        ImGui::BulletText("Name: %s", name->c_str());
        ImGui::BulletText("ID: %llX", *id);
        ImGui::BulletText("Pos: %f, %f, %f", pos->x, pos->y, pos->z);
        ImGui::BulletText("Combat Power: %f", parameter->combatPower.value);
        ImGui::BulletText("Age: %d", parameter->age.value);
        ImGui::BulletText("Health: %f/%f", parameter->life.value, parameter->lifeMaxBase.value + parameter->lifeMaxMod.value);
        ImGui::BulletText("Race: %s (%d)", raceToString(parameter->type.value), parameter->type.value);
        ImGui::BulletText("Target: %s", (!target) ? "No Target" : target->getName()->c_str());

        if (equipment != nullptr && ImGui::TreeNode(equipment, "Equipment")) {
            displayEquipment(equipment);
            ImGui::TreePop();
        }
    }

    void EntityViewer::displayEquipment(CCharacter::CEquipment* equipment) {
        static const map<int, string> equipmentNames{
        { 1, "Torso\\Armor\\Shirt" },
        { 2, "Head\\Helmet\\Hat" },
        { 4, "Hands\\Gauntlets\\Gloves" },
        { 5, "Feet\\Boots\\Shoes" },
        { 7, "Hair" },
        { 8, "Back\\Wings\\Robe" },
        { 10, "Weapon 1" },
        { 11, "Weapon 2" },
        { 12, "Arrow\\Shield 1" },
        { 13, "Arrow\\Shield 2" },
        { 15, "Face Accessory" },
        { 16, "Accessory 1" },
        { 17, "Accessory 2" },
        { 18, "Tail" }
        };

        for (auto[id, name] : equipmentNames) {
            auto& itemInfo = equipment->itemInfo[id];

            // Skip empty slots.
            if (itemInfo.classID == 0) {
                continue;
            }

            ImGui::BulletText("[%s] ID: %d Colors: #%08X #%08X #%08X", name.c_str(), itemInfo.classID, itemInfo.color1, itemInfo.color2, itemInfo.color3);
        }
    }

    void EntityViewer::displayItem(KItem* item) {
        auto id = item->getID();
        auto name = item->getName();
        auto maxStackCount = item->getMaxStackCount();

        if (!id || !name) {
            ImGui::Text("This item has no data.");
            return;
        }

        ImGui::BulletText("Address: %p", item);
        ImGui::BulletText("Name: %s", name->c_str());
        ImGui::BulletText("ID: %llX", *id);
        ImGui::BulletText("Item ID: %d", item->itemID);
        ImGui::BulletText("Inventory ID: %d", item->inventoryID);
        ImGui::BulletText("Pos: %d, %d", item->positionX, item->positionY);
        ImGui::BulletText("Color: %X %X %X %X %X %X", item->color1, item->color2, item->color3, item->color4, item->color5, item->color6);
        ImGui::BulletText("Price: %d", item->price);
        ImGui::BulletText("Sell price: %d", item->sellPrice);
        ImGui::BulletText("Durability: %0.4f/%0.4f", durabilityToDouble(item->durability, item->maxDurability), durabilityToDouble(item->maxDurability));
        ImGui::BulletText("Stack count: %d/%d", item->stackCount, *maxStackCount);
    }

    double EntityViewer::durabilityToDouble(uint32_t dura, uint32_t maxDura) {
        // Get the length of max dura to determine where to place the decimal
        if (std::to_string(maxDura).length() > 5) {
            return (double)dura * 0.0001;
        }
        else return (double)dura * 0.001;

    }

    double EntityViewer::durabilityToDouble(uint32_t maxDura) {
        // same as above, but return maxDura instead
        if (std::to_string(maxDura).length() > 5) {
            return (double)maxDura * 0.0001;
        }
        else return (double)maxDura * 0.001;
    }

    char* EntityViewer::raceToString(uint32_t raceType) {
        switch (raceType) {
        case  8001: return "Female Giant";
        case  8002: return "Male Giant";
        case  9001: return "Female Elf";
        case  9002: return "Male Elf";
        case 10001: return "Female Human";
        case 10002: return "Male Human";
        default: return "Error: Unknown Race";
        }
    }
}
