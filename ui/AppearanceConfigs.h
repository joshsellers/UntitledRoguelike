#ifndef _APPEARANCE_CONFIG_H
#define _APPEARANCE_CONFIG_H

#include "UIApperanceConfig.h"

inline UIAppearanceConfig BASE_COMPONENT_CONFIG = {
        0.2f,
        {96, 288, 4, 6},
        {96, 295, 4, 2},
        {96, 298, 4, 5},
        {101, 288, 1, 6},
        {101, 295, 1, 2},
        {101, 298, 1, 5},
        {103, 288, 4, 6},
        {103, 295, 4, 2},
        {103, 298, 4, 5}
};

inline UIAppearanceConfig BUTTON_CONFIG = BASE_COMPONENT_CONFIG;

inline UIAppearanceConfig BUTTON_HOVER_CONFIG = BUTTON_CONFIG.offsetBy(16, 0);

inline UIAppearanceConfig BUTTON_CLICKED_CONFIG = BUTTON_CONFIG.offsetBy(32, 0);

inline UIAppearanceConfig SLIDER_HANDLE_CONFIG = BASE_COMPONENT_CONFIG;
inline UIAppearanceConfig SLIDER_RAIL_CONFIG = BASE_COMPONENT_CONFIG;

inline UIAppearanceConfig TEXTFIELD_CONFIG = BUTTON_CONFIG.offsetBy(0, 16);
inline UIAppearanceConfig TEXTFIELD_HOVER_CONFIG = TEXTFIELD_CONFIG.offsetBy(16, 0);
inline UIAppearanceConfig TEXTFIELD_ARMED_CONFIG = TEXTFIELD_CONFIG.offsetBy(32, 0);

#endif