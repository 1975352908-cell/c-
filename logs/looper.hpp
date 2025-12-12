/*
 实现异步工作线程模型
 */
#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include"buffer.hpp"
#include"sink.hpp"
#include<thread>
#include<mutex>
#include<condition_variable>
#include<iostream>
#include<string>
#include<memory>
#include<functional>

namespace mylog
{
    using Functor=std::function<void(Buffer&)>;
    enum class AsyncType
    {
        ASYNC_SAFE,   //安全模式，表示缓冲区满了则阻塞，防止资源耗尽；
        ASYNC_UNSAFE, //不安全模式，不考虑资源耗尽，无线扩容，常用语测试；
    };
    class AsyncLooper
    {
    public:
        using ptr=std::shared_ptr<AsyncLooper>;
       AsyncLooper(const Functor&cb, AsyncType type = AsyncType::ASYNC_SAFE)   
       :_stop(false)
       ,_callback(cb)
       ,loopper_type(type)
        {
            _thread=std::thread(&AsyncLooper::threadEntry,this);
        }
        ~AsyncLooper()
        {
            stop();
        }
        void stop()
        {
            _stop=true;
            _con_cond.notify_all();//唤醒所有工作线程;
            _thread.join();//等待工作线程退出;
        }
        void push(const char*data,size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            //条件变量判断生产者缓冲区剩余空间大于等于data的长度，则直接写入，否则阻塞；
            if(loopper_type==AsyncType::ASYNC_SAFE)
                _pro_cond.wait(lock,[&]{return _pro_buffer.writeAvailable()>=len;});
            
            //写入数据；
            _pro_buffer.push(data,len);
            //唤醒消费者;
            _con_cond.notify_one();
        }
    private:
        void threadEntry()
        {
            while(1)
            {
              { 
                //1.判断生产者缓冲区是否为空，有数据则交换，没数据则阻塞；
                std::unique_lock<std::mutex> lock(_mutex);
                _con_cond.wait(lock,[this]{return _stop || !_pro_buffer.empty();});
                //如果停止标志为真，并且生产者缓冲区为空，则退出线程,保证缓冲区数据被消费完;
                if(_stop&& _pro_buffer.empty())
                {
                    return;
                }
                _con_buffer.swap(_pro_buffer);
                //2.唤醒生产者；
                if(loopper_type==AsyncType::ASYNC_SAFE)
                {
                    _pro_cond.notify_all();
                }
              }
                //3.被唤醒后，执行回调函数；
                _callback(_con_buffer);   
                //4.重置消费者缓冲区；
                _con_buffer.reset();
            }
        }
         Functor _callback;//具体对缓冲区数据进行处理的回调函数，由异步工具的使用者提供；
    private:
        AsyncType loopper_type; //异步模式类型;
        bool _stop; //是否停止;
        std::thread _thread; //后台工作线程;
        std::mutex _mutex; //互斥锁;
        Buffer _pro_buffer; //生产者缓冲区;
        Buffer _con_buffer; //消费者缓冲区;
        std::condition_variable _pro_cond; //生产者条件变量;
        std::condition_variable _con_cond; //消费者条件变量;
    };
}
#endif