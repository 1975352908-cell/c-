/*
    配置模块实现
    1.配置文件解析类
    2.支持多种Sink配置：TCP、UDP、MySQL、File、Stdout等
    3.支持Logger配置
*/
#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "level.hpp"

namespace mylog
{
    // 基础配置项类
    class ConfigItem
    {
    public:
        using ptr = std::shared_ptr<ConfigItem>;
        ConfigItem() = default;
        virtual ~ConfigItem() = default;
        
        void set(const std::string& key, const std::string& value)
        {
            _items[key] = value;
        }
        
        std::string get(const std::string& key, const std::string& default_value = "") const
        {
            auto it = _items.find(key);
            if (it != _items.end())
            {
                return it->second;
            }
            return default_value;
        }
        
        int getInt(const std::string& key, int default_value = 0) const
        {
            auto it = _items.find(key);
            if (it != _items.end())
            {
                try
                {
                    return std::stoi(it->second);
                }
                catch (...)
                {
                    return default_value;
                }
            }
            return default_value;
        }
        
        bool getBool(const std::string& key, bool default_value = false) const
        {
            auto it = _items.find(key);
            if (it != _items.end())
            {
                std::string value = it->second;
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                return value == "true" || value == "1" || value == "yes";
            }
            return default_value;
        }
        
        bool has(const std::string& key) const
        {
            return _items.find(key) != _items.end();
        }
        
        const std::map<std::string, std::string>& items() const
        {
            return _items;
        }
        
    private:
        std::map<std::string, std::string> _items;//键值对容器;，存储配置项;key是配置项的名称，value是配置项的值;
    };
    
    // Sink配置基类
    class SinkConfig
    {
    public:
        using ptr = std::shared_ptr<SinkConfig>;
        enum class Type
        {
            STDOUT,//标准输出;
            FILE,//文件;
            MYSQL,//数据库;
            TCP,//TCP;
            UDP,//UDP;
            UNKNOWN//未知;
        };
        
        SinkConfig(Type type) : _type(type) {}
        virtual ~SinkConfig() = default;
        
        Type type() const { return _type; }
        
        static std::string typeToString(Type type)
        {
            switch (type)
            {
            case Type::STDOUT: return "stdout";
            case Type::FILE: return "file";
            case Type::MYSQL: return "mysql";
            case Type::TCP: return "tcp";
            case Type::UDP: return "udp";
            default: return "unknown";
            }
        }
        
        static Type stringToType(const std::string& str)
        {
            if (str == "stdout") return Type::STDOUT;
            if (str == "file") return Type::FILE;
            if (str == "mysql") return Type::MYSQL;
            if (str == "tcp") return Type::TCP;
            if (str == "udp") return Type::UDP;
            return Type::UNKNOWN;
        }
        
    protected:
        Type _type;//Sink类型;
    };
    
    // 标准输出Sink配置
    class StdoutSinkConfig : public SinkConfig
    {
    public:
        using ptr = std::shared_ptr<StdoutSinkConfig>;
        StdoutSinkConfig() : SinkConfig(Type::STDOUT), _colored(true) {}
        
        void setColored(bool colored) { _colored = colored; }
        bool colored() const { return _colored; }
        
    private:
        bool _colored;
    };
    
    // 文件Sink配置
    class FileSinkConfig : public SinkConfig
    {
    public:
        using ptr = std::shared_ptr<FileSinkConfig>;
        FileSinkConfig() : SinkConfig(Type::FILE) {}
        
        void setPath(const std::string& path) { _path = path; }
        std::string path() const { return _path; }
        
    private:
        std::string _path;
    };
    
    // MySQL Sink配置
    class MysqlSinkConfig : public SinkConfig
    {
    public:
        using ptr = std::shared_ptr<MysqlSinkConfig>;
        MysqlSinkConfig() : SinkConfig(Type::MYSQL), _port(3306) {}
        
        void setHost(const std::string& host) { _host = host; }
        void setPort(int port) { _port = port; }
        void setUser(const std::string& user) { _user = user; }
        void setPassword(const std::string& password) { _password = password; }
        void setDatabase(const std::string& database) { _database = database; }
        
        std::string host() const { return _host; }
        int port() const { return _port; }
        std::string user() const { return _user; }
        std::string password() const { return _password; }
        std::string database() const { return _database; }
        
    private:
        std::string _host;
        int _port;
        std::string _user;
        std::string _password;
        std::string _database;
    };
    
    // TCP Sink配置
    class TcpSinkConfig : public SinkConfig
    {
    public:
        using ptr = std::shared_ptr<TcpSinkConfig>;
        TcpSinkConfig() : SinkConfig(Type::TCP), _port(9990) {}
        
        void setHost(const std::string& host) { _host = host; }
        void setPort(int port) { _port = port; }
        
        std::string host() const { return _host; }
        int port() const { return _port; }
        
    private:
        std::string _host;
        int _port;
    };
    
    // UDP Sink配置
    class UdpSinkConfig : public SinkConfig
    {
    public:
        using ptr = std::shared_ptr<UdpSinkConfig>;
        UdpSinkConfig() : SinkConfig(Type::UDP), _port(9991) {}
        
        void setHost(const std::string& host) { _host = host; }
        void setPort(int port) { _port = port; }
        
        std::string host() const { return _host; }
        int port() const { return _port; }
        
    private:
        std::string _host;
        int _port;
    };
    
    // Logger配置
    class LoggerConfig
    {
    public:
        using ptr = std::shared_ptr<LoggerConfig>;
        enum class LoggerType
        {
            SYNC,
            ASYNC
        };
        
        LoggerConfig() : _level(LogLevel::value::DEBUG), _type(LoggerType::SYNC) {}
        
        void setName(const std::string& name) { _name = name; }
        void setLevel(LogLevel::value level) { _level = level; }
        void setType(LoggerType type) { _type = type; }
        void setFormatter(const std::string& formatter) { _formatter = formatter; }
        void addSink(const std::string& name, SinkConfig::ptr sink)
        {
            _sinks[name] = sink;
        }
        
        std::string name() const { return _name; }
        LogLevel::value level() const { return _level; }
        LoggerType type() const { return _type; }
        std::string formatter() const { return _formatter; }
        const std::map<std::string, SinkConfig::ptr>& sinks() const { return _sinks; }
        
        static LoggerType stringToType(const std::string& str)
        {
            if (str == "ASYNC" || str == "async") return LoggerType::ASYNC;
            return LoggerType::SYNC;
        }
        
        static std::string typeToString(LoggerType type)
        {
            return type == LoggerType::ASYNC ? "ASYNC" : "SYNC";
        }
        
    private:
        std::string _name;
        LogLevel::value _level;
        LoggerType _type;
        std::string _formatter;
        std::map<std::string, SinkConfig::ptr> _sinks;//存储Sink配置;key是Sink的名称，value是Sink配置的指针;
    };
    

    // 辅助函数：字符串转日志等级
    inline LogLevel::value stringToLogLevel(const std::string& str)
    {
        if (str == "DEBUG") return LogLevel::value::DEBUG;
        if (str == "INFO") return LogLevel::value::INFO;
        if (str == "WARN" || str == "WORN") return LogLevel::value::WORN;
        if (str == "ERROR") return LogLevel::value::ERROR;
        if (str == "FATAL") return LogLevel::value::FATAL;
        if (str == "OFF") return LogLevel::value::OFF;
        return LogLevel::value::UNKNOW;
    }
    
    // 配置解析器
    class ConfigParser
    {
    public:
        using ptr = std::shared_ptr<ConfigParser>;
        
        ConfigParser() = default;
        
        // 加载配置文件
        bool load(const std::string& filename)
        {
            std::ifstream ifs(filename);//打开配置文件;
            if (!ifs.is_open())//如果文件打开失败，则返回false;
            {
                std::cerr << "Failed to open config file: " << filename << std::endl;//输出错误信息;
                return false;
            }
            
            std::string line;//读取配置文件的每一行;
            std::string current_section;//当前配置项;
            
            while (std::getline(ifs, line))//读取配置文件的每一行;
            {
                // 去除首尾空白
                line = trim(line);
                
                // 跳过空行和注释
                if (line.empty() || line[0] == '#' || line[0] == ';')
                {
                    continue;
                }
                
                // 处理配置项;
                if (line[0] == '[' && line[line.length() - 1] == ']')
                {
                    current_section = line.substr(1, line.length() - 2);//截取配置项;
                    continue;
                }
                
                // 解析key=value
                size_t pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (!current_section.empty())
                    {
                        _sections[current_section].set(key, value);
                    }
                }
            }
            
            ifs.close();
            return parseLoggers();
        }
        
        // 获取所有Logger配置
        const std::map<std::string, LoggerConfig::ptr>& loggers() const
        {
            return _loggers;
        }
        
        // 获取指定Logger配置
        LoggerConfig::ptr getLogger(const std::string& name) const
        {
            auto it = _loggers.find(name);
            if (it != _loggers.end())
            {
                return it->second;
            }
            return nullptr;
        }
        
        // 获取Section
        ConfigItem getSection(const std::string& section) const
        {
            auto it = _sections.find(section);
            if (it != _sections.end())
            {
                return it->second;
            }
            return ConfigItem();
        }
        
    private:
        // 解析Logger配置
        bool parseLoggers()
        {
            for (auto& section_pair : _sections)
            {
                const std::string& section_name = section_pair.first;
                const ConfigItem& section = section_pair.second;
                
                // 查找logger配置
                if (section_name.find("logger.") == 0)
                {
                    // 检查是否为sink配置
                    if (section_name.find(".sink.") != std::string::npos)
                    {
                        continue; // sink配置后续处理
                    }
                    
                    // 提取logger名称
                    std::string logger_name = section_name.substr(7); // "logger."之后
                    
                    LoggerConfig::ptr logger_config = std::make_shared<LoggerConfig>();
                    logger_config->setName(section.get("name", logger_name));
                    
                    // 解析日志等级
                    std::string level_str = section.get("level", "DEBUG");
                    logger_config->setLevel(stringToLogLevel(level_str));
                    
                    // 解析Logger类型
                    std::string type_str = section.get("type", "SYNC");
                    logger_config->setType(LoggerConfig::stringToType(type_str));
                    
                    // 解析格式化器
                    std::string formatter = section.get("formatter", "[%d{%H:%M:%S}][%p][%c] %m%n");
                    logger_config->setFormatter(formatter);
                    
                    // 解析Sinks
                    parseSinks(logger_name, logger_config);
                    
                    _loggers[logger_name] = logger_config;
                }
            }
            
            return true;
        }
        
        // 解析Sink配置
        void parseSinks(const std::string& logger_name, LoggerConfig::ptr logger_config)
        {
            std::string sink_prefix = "logger." + logger_name + ".sink.";
            
            for (auto& section_pair : _sections)
            {
                const std::string& section_name = section_pair.first;
                const ConfigItem& section = section_pair.second;
                
                if (section_name.find(sink_prefix) == 0)
                {
                    // 提取sink名称
                    std::string sink_name = section_name.substr(sink_prefix.length());
                    
                    // 获取sink类型
                    std::string type_str = section.get("type", "unknown");
                    SinkConfig::Type sink_type = SinkConfig::stringToType(type_str);
                    
                    SinkConfig::ptr sink_config = createSinkConfig(sink_type, section);
                    if (sink_config)
                    {
                        logger_config->addSink(sink_name, sink_config);
                    }
                }
            }
        }
        
        // 创建Sink配置
        SinkConfig::ptr createSinkConfig(SinkConfig::Type type, const ConfigItem& section)
        {
            switch (type)
            {
            case SinkConfig::Type::STDOUT:
            {
                auto config = std::make_shared<StdoutSinkConfig>();
                config->setColored(section.getBool("colored", true));
                return config;
            }
            case SinkConfig::Type::FILE:
            {
                auto config = std::make_shared<FileSinkConfig>();
                config->setPath(section.get("path", "./logs/default.log"));
                return config;
            }
            case SinkConfig::Type::MYSQL:
            {
                auto config = std::make_shared<MysqlSinkConfig>();
                config->setHost(section.get("host", "localhost"));
                config->setPort(section.getInt("port", 3306));
                config->setUser(section.get("user", "root"));
                config->setPassword(section.get("password", ""));
                config->setDatabase(section.get("database", "db_logs"));
                return config;
            }
            case SinkConfig::Type::TCP:
            {
                auto config = std::make_shared<TcpSinkConfig>();
                config->setHost(section.get("host", "127.0.0.1"));
                config->setPort(section.getInt("port", 9990));
                return config;
            }
            case SinkConfig::Type::UDP:
            {
                auto config = std::make_shared<UdpSinkConfig>();
                config->setHost(section.get("host", "127.0.0.1"));
                config->setPort(section.getInt("port", 9991));
                return config;
            }
            default:
                return nullptr;
            }
        }
        
        // 字符串去除首尾空白
        static std::string trim(const std::string& str)
        {
            size_t first = str.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                return "";
            }
            size_t last = str.find_last_not_of(" \t\r\n");
            return str.substr(first, last - first + 1);
        }
        
    private:
        std::map<std::string, ConfigItem> _sections;//存储配置项;key是section的名称，value是ConfigItem的指针;
        std::map<std::string, LoggerConfig::ptr> _loggers;//存储Logger配置;key是Logger的名称，value是Logger配置的指针;
    };
    




    // 配置管理器单例,这是单例模式中的懒汉模式;
    class ConfigManager
    {
    public:
        static ConfigManager& getInstance()//获取单例实例;
        {
            static ConfigManager instance;//定义一个静态的单例实例;
            return instance;
        }
        
        bool loadConfig(const std::string& filename)//加载配置文件;
        {
            _parser = std::make_shared<ConfigParser>();
            return _parser->load(filename);
        }
        
        ConfigParser::ptr parser() const
        {
            return _parser;
        }
        
        LoggerConfig::ptr getLogger(const std::string& name) const
        {
            if (_parser)
            {
                return _parser->getLogger(name);
            }
            return nullptr;
        }
        
        const std::map<std::string, LoggerConfig::ptr>& loggers() const
        {
            if (_parser)
            {
                return _parser->loggers();
            }
            static std::map<std::string, LoggerConfig::ptr> empty;
            return empty;
        }
        
    private:
        ConfigManager() = default;
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;
        
        ConfigParser::ptr _parser;
    };
}

#endif

