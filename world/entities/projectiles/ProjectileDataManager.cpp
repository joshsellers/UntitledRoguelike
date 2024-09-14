#include "ProjectileDataManager.h"
#include "ProjectileData.h"
#include "../../../inventory/Item.h"

void ProjectileDataManager::addData(ProjectileData data) {
    _projectileData.push_back(data);
}

ProjectileData ProjectileDataManager::getData(unsigned int itemId) {
    ProjectileData data{0, 0, sf::IntRect(), false, false, 0, false, 0, 0, false};

    for (ProjectileData target : _projectileData) {
        if (target.itemId == itemId) {
            return target;
        }
    }

    return data;
}

ProjectileData ProjectileDataManager::getData(std::string itemName) {
    ProjectileData data{ 0, 0, sf::IntRect(), false, false, 0, false, 0, 0, false };

    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            unsigned int itemId = item->getId();
            for (ProjectileData target : _projectileData) {
                if (target.itemId == itemId) return target;
            }
        }
    }
    return data;
}
