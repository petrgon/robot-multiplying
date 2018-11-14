#ifndef CALLBACK_TIME_MANAGER_H
#define CALLBACK_TIME_MANAGER_H

#include "ros/ros.h"

#include <list>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "../states/state.h"

struct SubscribedCallBack;

class CallBackTimeManager
{
  public:
    ~CallBackTimeManager();
    static CallBackTimeManager *getInstance();

    void subscribe(State *, int64_t milis);

  private:
    CallBackTimeManager();

    void start();
    static void run(CallBackTimeManager *manager);

    static CallBackTimeManager *instance;

    std::thread *thread;
    std::mutex m;
    std::condition_variable condVar;
    bool shouldEnd;

    std::list<SubscribedCallBack> callbackHandlers;
};

struct SubscribedCallBack
{
    SubscribedCallBack(State *, int64_t);
    ~SubscribedCallBack();
    SubscribedCallBack(const SubscribedCallBack &) = default;
    SubscribedCallBack(SubscribedCallBack &&) = default;
    SubscribedCallBack &operator=(const SubscribedCallBack &) = default;
    SubscribedCallBack &operator=(SubscribedCallBack &&) = default;
    bool operator>(const SubscribedCallBack &) const;
    bool operator<(const SubscribedCallBack &) const;
    bool operator>=(const SubscribedCallBack &) const;
    bool operator<=(const SubscribedCallBack &) const;
    bool operator==(const SubscribedCallBack &) const;
    bool operator!=(const SubscribedCallBack &) const;

    State *state;
    std::chrono::milliseconds time;
    std::chrono::time_point<std::chrono::system_clock> subscribed;
};
#endif /*CALLBACK_TIME_MANAGER_H*/