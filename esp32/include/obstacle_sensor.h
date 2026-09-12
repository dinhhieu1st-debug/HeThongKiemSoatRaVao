#ifndef OBSTACLE_SENSOR_H
#define OBSTACLE_SENSOR_H

namespace ObstacleSensor {

void begin();
void update();

bool hasObstacle();
bool stateChanged();

}

#endif