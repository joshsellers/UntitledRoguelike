#include "ProjectileDataManager.h"
#include "ProjectileData.h"
#include "../../../inventory/Item.h"
#include "../../../core/MessageManager.h"

void ProjectileDataManager::addData(ProjectileData data) {
    if (data.itemId == 0) return;
    _projectileData.push_back(data);
}

ProjectileData ProjectileDataManager::getData(unsigned int itemId) {
    ProjectileData data;

    for (ProjectileData target : _projectileData) {
        if (target.itemId == itemId) {
            return target;
        }
    }

    return data;
}

ProjectileData ProjectileDataManager::getData(std::string itemName) {
    ProjectileData data;

    for (const auto& item : Item::ITEMS) {
        if (item->getName() == itemName) {
            unsigned int itemId = item->getId();
            return getData(itemId);
        }
    }


    MessageManager::displayMessage("Did not find a projectile type named \"" + itemName + "\"", 5, WARN);
    return data;
}
