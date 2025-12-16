# 日志打印流程交互图

```mermaid
sequenceDiagram
autonumber
participant U as 用户代码(main.cpp)
participant M as 宏/代理(bitlog.h)
participant LM as loggerManager(单例)
participant L as Logger(基类接口)
participant F as Formatter
participant S as LogSink(Stdout/File/Roll)
participant AL as AsyncLogger
participant Q as AsyncLooper(双缓冲+线程)
participant T as 异步线程(worker_loop)

U->>M: LOGI("x=%d", x)
M->>LM: rootLogger()/getLogger(name)
LM-->>M: Logger*
M->>L: info(__FILE__, __LINE__, fmt,...)
L->>L: shouldLog(level)

alt 不满足等级
  L-->>U: return
else 满足等级
  L->>L: log(level,file,line,fmt,va_list)
  L->>L: vasprintf 生成 payload 字符串
  L->>L: 构造 LogMsg(name,file,line,payload,level)
  L->>F: format(ostream, LogMsg)
  F-->>L: 拼出最终字符串 msg

  alt 同步日志器(SyncLogger)
    L->>S: sink->log(msg.c_str(), msg.size())（遍历多个sink）
  else 异步日志器(AsyncLogger)
    L->>AL: logIt(msg)
    AL->>Q: push(msg) 写入 tasks_push
    Q-->>T: notify pop_cond
    T->>Q: swap(tasks_push,tasks_pop)
    T->>AL: realLog(tasks_pop)
    AL->>S: sink->log(buffer.begin(), buffer.readAbleSize())
  end
end
