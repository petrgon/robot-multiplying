#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "ros/ros.h"

#include "../sensors/light_sensor.h"
#include "../sensors/touch_sensor.h"
#include "../sensors/infra_red_sensor.h"
#include "../states/state.h"

#include <list>
#include <thread>

#define FREQUENCE 10 //10Hz

#define LIGHT_SENSOR_PIN 7
#define LEFT_SENSOR_PIN 5
#define RIGHT_SENSOR_PIN 1
#define PUCK_SENSOR_PIN 4
#define INFRA_RED_SENSOR_PIN 4 //TODO change pin

class SensorManager
{
public:
  static SensorManager *getInstance();

  ~SensorManager();

  void subscribeToAll(State *);
  void unsubscribeFromAll(State *);

  void subscribeLightDetectedEvent(State *);
  void subscribeLightLostEvent(State *);

  void subscribeLeftTouchTriggeredEvent(State *);
  void subscribeLeftTouchFreedEvent(State *);

  void subscribeRightTouchTriggeredEvent(State *);
  void subscribeRightTouchFreedEvent(State *);

  void subscribeBothTouchTriggeredEvent(State *);
  void subscribeBothTouchFreedEvent(State *);

  void subscribePuckAquiredEvent(State *);
  void subscribePuckLostEvent(State *);
  
  void subscribeIR600FoundEvent(State *);
  void subscribeIR600LostEvent(State *);

  void subscribeIR1500FoundEvent(State *);
  void subscribeIR1500LostEvent(State *);

private:
  SensorManager();
  
  void start();

  static void resolveLightSensor(LightSensor &sensor, SensorManager *manager);

  static void resolveFrontSensors(TouchSensor &left, TouchSensor &right, SensorManager *manager);

  static void resolvePuckSensor(TouchSensor &sensor, SensorManager *manager);

  static void resolveIRSensor(InfraRedSensor &sensor, SensorManager *manager);

  static void callEventHandlers(const std::list<State *> &handlers, void (State::*ptr)());

  static void run(SensorManager *manager);

  std::list<State *> lightDetectedEventHandlers;
  std::list<State *> lightLostEventHandlers;
  std::list<State *> puckAquiredEventHandlers;
  std::list<State *> puckLostEventHandlers;
  std::list<State *> leftTouchTriggeredEventHandlers;
  std::list<State *> leftTouchFreedEventHandlers;
  std::list<State *> rightTouchTriggeredEventHandlers;
  std::list<State *> rightTouchFreedEventHandlers;
  std::list<State *> bothTouchTriggeredEventHandlers;
  std::list<State *> bothTouchFreedEventHandlers;
  std::list<State *> ir600FoundEventHandlers;
  std::list<State *> ir600LostEventHandlers;
  std::list<State *> ir1500FoundEventHandlers;
  std::list<State *> ir1500LostEventHandlers;

  std::thread *thread;
  bool shouldEnd;

  static SensorManager *instance;
};
#endif /*SENSOR_MANAGER_H*/
