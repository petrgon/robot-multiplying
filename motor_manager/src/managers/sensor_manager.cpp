#include "../../include/managers/sensor_manager.h"

SensorManager::SensorManager() : thread(nullptr), shouldEnd(false)
{
    ROS_INFO("SensorManager initialized");
}
SensorManager::~SensorManager()
{
    ROS_INFO("Joining thread");
    thread->join();
    delete thread;
    ROS_INFO("Thread Joined");
}

void SensorManager::subscribeToAll(State *state)
{
    subscribePuckAquiredEvent(state);
    subscribePuckLostEvent(state);
    subscribeLightDetectedEvent(state);
    subscribeLightLostEvent(state);
    subscribeLeftTouchTriggeredEvent(state);
    subscribeLeftTouchFreedEvent(state);
    subscribeRightTouchTriggeredEvent(state);
    subscribeRightTouchFreedEvent(state);
    subscribeBothTouchTriggeredEvent(state);
    subscribeBothTouchFreedEvent(state);
}
/*void SensorManager::unsubscribeFromAll(State *state)
{
    for (auto it = handlers.begin(); it != handlers.end(); it++)
    {
        it
    }
}*/

void SensorManager::subscribePuckAquiredEvent(State *state)
{
    puckAquiredEventHandlers.push_back(state);
}

void SensorManager::subscribePuckLostEvent(State *state)
{
    puckLostEventHandlers.push_back(state);
}

void SensorManager::subscribeLightDetectedEvent(State *state)
{
    lightDetectedEventHandlers.push_back(state);
}

void SensorManager::subscribeLightLostEvent(State *state)
{
    lightLostEventHandlers.push_back(state);
}

void SensorManager::subscribeLeftTouchTriggeredEvent(State *state)
{
    leftTouchTriggeredEventHandlers.push_back(state);
}

void SensorManager::subscribeLeftTouchFreedEvent(State *state)
{
    leftTouchFreedEventHandlers.push_back(state);
}

void SensorManager::subscribeRightTouchTriggeredEvent(State *state)
{
    rightTouchTriggeredEventHandlers.push_back(state);
}

void SensorManager::subscribeRightTouchFreedEvent(State *state)
{
    rightTouchFreedEventHandlers.push_back(state);
}

void SensorManager::subscribeBothTouchTriggeredEvent(State *state)
{
    bothTouchTriggeredEventHandlers.push_back(state);
}

void SensorManager::subscribeBothTouchFreedEvent(State *state)
{
    bothTouchFreedEventHandlers.push_back(state);
}

void SensorManager::start()
{
    ROS_INFO("Starting SensorManager");
    if (!thread)
    {
        ROS_INFO("Joining previous thread for SensorManager");
        shouldEnd = true;
        thread->join();
        delete thread;
    }
    thread = new std::thread(SensorManager::run, this);
}

void SensorManager::run(SensorManager *manager)
{
    ROS_INFO("SensorManager started");
    LightSensor lightSensor(LIGHT_SENSOR_PIN);
    TouchSensor puckSensor(PUCK_SENSOR_PIN);
    TouchSensor leftSensor(LEFT_SENSOR_PIN);
    TouchSensor rightSensor(RIGHT_SENSOR_PIN);
    ros::Rate loop_rate(FREQUENCE);
    while (ros::ok() && !manager->shouldEnd)
    {
        ROS_INFO("Checking sensors");
        resolveLightSensor(lightSensor, manager);
        resolveFrontSensors(leftSensor, rightSensor, manager);
        resolvePuckSensor(puckSensor, manager);
        loop_rate.sleep();
    }
    manager->shouldEnd = false;
}

void SensorManager::resolveLightSensor(LightSensor &sensor, SensorManager *manager)
{
    unsigned short prevState = sensor.getPreviousSignal();
    unsigned short state = sensor.checkSignal();
    if (prevState != state)
    {
        if (state == 1)
            callEventHandlers(manager->lightLostEventHandlers, &State::lightLostEventHandler);
        else
            callEventHandlers(manager->lightDetectedEventHandlers, &State::lightDetectedEventHandler);
    }
}

void SensorManager::resolveFrontSensors(TouchSensor &left, TouchSensor &right, SensorManager *manager)
{
    bool prevLeftState = left.getPrevious();
    bool prevRightState = right.getPrevious();
    bool leftState = left.isPushed();
    bool rightState = right.isPushed();
    if (prevLeftState != leftState && prevRightState != rightState && rightState == leftState) //both changed state
    {
        if (rightState)
            callEventHandlers(manager->bothTouchTriggeredEventHandlers, &State::bothTouchTriggeredEventHandler);
        else
            callEventHandlers(manager->bothTouchFreedEventHandlers, &State::bothTouchFreedEventHandler);
    }
    else
    {
        if (prevLeftState != leftState)
        {
            if (leftState)
                callEventHandlers(manager->leftTouchTriggeredEventHandlers, &State::leftTouchTriggeredEventHandler);
            else
                callEventHandlers(manager->leftTouchFreedEventHandlers, &State::leftTouchFreedEventHandler);
        }
        if (prevRightState != rightState)
        {
            if (rightState)
                callEventHandlers(manager->rightTouchTriggeredEventHandlers, &State::rightTouchTriggeredEventHandler);
            else
                callEventHandlers(manager->rightTouchFreedEventHandlers, &State::rightTouchFreedEventHandler);
        }
    }
}

void SensorManager::resolvePuckSensor(TouchSensor &sensor, SensorManager *manager)
{
    bool prevState = sensor.getPrevious();
    bool state = sensor.isPushed();
    if (prevState != state)
    {
        if (state)
            callEventHandlers(manager->puckAquiredEventHandlers, &State::puckAquiredEventHandler);
        else
            callEventHandlers(manager->puckLostEventHandlers, &State::puckLostEventHandler);
    }
}

void SensorManager::callEventHandlers(const std::vector<State *> &handlers, void (State::*ptr)())
{
    for (auto it = handlers.begin(); it != handlers.end(); it++)
    {
        ((*it)->*ptr)();
    }
}
