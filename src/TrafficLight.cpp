#include "TrafficLight.h"
#include <chrono>
#include <iostream>
#include <random>

double gen_random_cycle_duration()
{
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(4.0, 6.0);
    return dis(gen);
}

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lk(_mutex);
    _condition.wait(lk, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lk(_mutex);
    _queue.emplace_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        auto phase = _queue.receive();
        if (phase == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    std::thread t{&TrafficLight::cycleThroughPhases, this};
    this->threads.emplace_back(std::move(t));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto previous = std::chrono::steady_clock::now();
    double cycleDuration = gen_random_cycle_duration();

    while (true)
    {
        auto current = std::chrono::steady_clock::now();
        auto secsSincePrevious = std::chrono::duration_cast<std::chrono::seconds>(current - previous).count();

        if (secsSincePrevious >= cycleDuration)
        {
            cycleDuration = gen_random_cycle_duration();
            previous = current;

            auto nextPhase =
                _currentPhase == TrafficLightPhase::green ? TrafficLightPhase::red : TrafficLightPhase::green;
            _currentPhase = nextPhase;
            _queue.send(std::move(nextPhase));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
