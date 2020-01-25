#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

enum class TrafficLightPhase
{
    red = 0,
    green
};

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue
{
  public:
    T receive();
    void send(T &&msg);

  private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
  public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

  private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;

    MessageQueue<TrafficLightPhase> _queue;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
