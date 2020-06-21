#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lock(_mutex);

    // wait
    _cond.wait(lock, [this]{return !_queue.empty();});

    std::cout << "entry";

    // return element
    T output = std::move(_queue.back());
    _queue.pop_back();
    return output;   
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    std::lock_guard<std::mutex> guard(_mutex);

    // add msg to Q
    _queue.push_back(std::move(msg));

    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{

}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        //run recieve
        _currentPhase = _queuePhases.receive();

        //check if green
        if (TrafficLightPhase::green == _currentPhase)
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
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    std::lock_guard<std::mutex> guard(_mutex);
    
    // call thread
      
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    // cycle duration
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<std::mt19937::result_type> duration(4, 6);

    // Infinite loop
    auto start = std::chrono::system_clock::now();
    while(true)
    {
        //time difference
        auto end = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
       std::cout << "delta: " + std::to_string(delta) + "\n";
        if (duration(rng) < delta)
        {
            //
            std::cout << "uhu\n";

            // toggle phase
            _currentPhase = static_cast<TrafficLightPhase>((static_cast<int>(_currentPhase) + 1) % 2);

            // update queue phases
            _queuePhases.send(std::move(_currentPhase));
            
            // update time
            start = end;
        }        

        // sleep time
        std::this_thread::sleep_for(std::chrono::milliseconds(1));            
    }
}

