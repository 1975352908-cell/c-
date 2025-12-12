#ifndef __M_BUFFER_H__
#define __M_BUFFER_H__
#include<vector>
#include<cassert>
#include<cstdio>
#define DEFAULT_BUFFER_SIZE (1*1024*1024)
#define THRESHOLD_SIZE (8*1024*1024)
#define INCREMENT_SIZE (1*1024*1024)
namespace mylog
{
    class Buffer
    {
    public:
        Buffer()
        :_buffer(DEFAULT_BUFFER_SIZE),
         _reader_index(0),
         _writer_index(0)
        {
        }
        //向缓冲区中写入数据;   
        void push(const char*data,size_t len)
        {
            //空间不够则扩容；
            ensureEnoughSize(len);
            std::copy(data,data+len,&_buffer[_writer_index]);
            moveWriter(len);
        }
        size_t writeAvailable()
        {
            //此接口用于固定大小缓冲区，计算剩余空间；
            return _buffer.size()-_writer_index;
        }
        //获取缓冲区读数据的起始位置
        const char* begin() const
        {
            return &_buffer[_reader_index];
        }
        //获取缓冲区中可读取的数据大小;
        size_t readAbleSize() const
        {
            return _writer_index-_reader_index;
        }
        //判断缓冲区是否为空;
        bool empty() const
        {
            return _reader_index==_writer_index;
        }
        //交换缓冲区;
        void swap(Buffer& other)
        {
           std::swap(_buffer,other._buffer);
           std::swap(_reader_index,other._reader_index);
           std::swap(_writer_index,other._writer_index);
        }
        //重置缓冲区;
        void reset()
        {
            _reader_index=0;
            _writer_index=0;
        }
        //移动读取位置;
        void moveReader(size_t len)
        {
            assert(len+_reader_index<=_writer_index);
            _reader_index+=len;
        }
    private:
        void ensureEnoughSize(size_t len)
        {
            //空间足够，直接返回
           if(len<=writeAvailable())
           {
                return;
           }
           //空间不够，扩容
           //小于阈值，按照2倍扩容
           if(_buffer.size()<THRESHOLD_SIZE)
           {
                _buffer.resize(2*_buffer.size()+len);//+len是为了保证扩容后空间足够;
           }
           //超过阈值，按照INCREMENT_SIZE扩容
           else
           {
                _buffer.resize(_buffer.size()+INCREMENT_SIZE+len);//+len是为了保证扩容后空间足够;
           }
        }
        //移动写入位置;
        void moveWriter(size_t len)
        {   
            assert(len+_writer_index<=_buffer.size());
            _writer_index+=len;
        }
    private:
        std::vector<char> _buffer; //缓冲区;
        size_t _reader_index; //读取位置;
        size_t _writer_index; //写入位置;
    };
}


















#endif