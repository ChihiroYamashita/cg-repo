//
//  Light.cpp
//  gl3d_raytracing_v2
//
//  Created by Yonghao Yue on 2019/11/19.
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

#include "Light.h"

void initAreaLights(std::vector<AreaLight>& lights)
{
    AreaLight light1;
    light1.pos << -1.2, 1.2, 1.2;
    light1.arm_u << 1.0, 0.0, 0.0;
    light1.arm_v = -light1.pos.cross( light1.arm_u );
    light1.arm_v.normalize();
    light1.arm_u = light1.arm_u * 0.3;
    light1.arm_v = light1.arm_v * 0.2;

    light1.color << 1.0, 0.8, 0.3;
    //light1.color << 1.0, 1.0, 1.0;
    //light1.intensity = 64.0;
    light1.intensity = 48.0;

    AreaLight light2;
    light2.pos << 1.2, 1.2, 0.0;
    light2.arm_u << 1.0, 0.0, 0.0;
    light2.arm_v = -light2.pos.cross( light2.arm_u );
    light2.arm_v.normalize();
    light2.arm_u = light2.arm_u * 0.3;
    light2.arm_v = light2.arm_v * 0.2;

    //light2.color << 0.3, 0.3, 1.0;
    light2.color << 1.0, 1.0, 1.0;
    //light2.intensity = 64.0;
    light2.intensity = 30.0;

    lights.push_back( light1 );
    lights.push_back( light2 );
}
