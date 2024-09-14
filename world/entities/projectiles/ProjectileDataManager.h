#ifndef _PROJECTILE_DATA_MANAGER_H
#define _PROJECTILE_DATA_MANAGER_H

#include <vector>
#include <string>

struct ProjectileData;

class ProjectileDataManager {
public:
    static void addData(ProjectileData data);
    static ProjectileData getData(unsigned int itemId);
    static ProjectileData getData(std::string itemName);
private:
    static inline std::vector<ProjectileData> _projectileData;
};

#endif