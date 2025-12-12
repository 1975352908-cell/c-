/*
    日志模块性能测试;
*/
#include"../logs/mylog.h"
#include<chrono>
void bench(const std::string& logger_name,size_t thread_count,size_t message_count,size_t message_len)
{
    //1.创建日志器;
    mylog::Logger::ptr logger=mylog::getLogger(logger_name);
    if(logger==nullptr)
    {
        std::cerr<<"logger is nullptr"<<std::endl;
        return;
    }
    std::cout<<"测试日志"<<message_count<<"条"<<message_len*message_count/1024 <<"kb"<<std::endl;
    //2.组织指定长度的日志消息;
    std::string message(message_len-1,'a');
    //3.创建指定数量的线程，每个线程打印指定数量的日志消息;
    std::vector<std::thread> threads;
    size_t msg_prt_thr=message_count/thread_count; //每个线程打印的日志消息数量;
    std::vector<double> cost_array(thread_count); //存储每个线程的耗时;
    for(int i=0;i<thread_count;i++)
    {
        threads.emplace_back([&,i](){
            //4.开始计时;
            auto start=std::chrono::high_resolution_clock::now();
            for(int j=0;j<msg_prt_thr;j++)
            {
                //5.打印日志消息;
                logger->debug(message);
            }
            //6.结束计时;
            auto end=std::chrono::high_resolution_clock::now();
            //计算时间;
            std::chrono::duration<double> cost=end-start;
            cost_array[i]=cost.count();
            //打印线程id和打印时间;
            std::cout<<"thread:"<<i<<"\t输出数量:"<<msg_prt_thr<<"耗时:"<<cost.count()<<"s"<<std::endl;
        });
    }
    //5.等待所有线程完成;
    for(auto& thread:threads)
    {
        thread.join();
    }
    //6.打印总时间;
    double total_cost=0;
    for(auto& cost:cost_array)
    {
        if(cost>total_cost)
        {   
            total_cost=cost;
        }
    }
    std::cout<<"最慢线程耗时:"<<total_cost<<"s"<<std::endl;
    std::cout<<"每秒输出日志数量:"<<message_count/total_cost<<"条"<<std::endl;
    std::cout<<"每秒输出日志大小:"<<message_count*message_len/total_cost/1024<<"KB"<<std::endl;
}


void sync_bench()
{
    std::unique_ptr<mylog::LoggerBuilder> builder=std::unique_ptr<mylog::GlobalLoggerBuilder>(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildFormatter("%m%n");
    builder->buildSink<mylog::FileSink>("../log_test/sync_log.log");
    mylog::Logger::ptr logger=builder->build();
    bench("sync_logger",1,1000000,100);
}
void async_bench()
{
    std::unique_ptr<mylog::LoggerBuilder> builder=std::unique_ptr<mylog::GlobalLoggerBuilder>(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildFormatter("%m%n");
    builder->buildSink<mylog::FileSink>("../log_test/async_log.log");
    mylog::Logger::ptr logger=builder->build();
    bench("async_logger",10,1000000,100);
}
int main()
{
    async_bench();
    return 0;
}