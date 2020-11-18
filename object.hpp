#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__
#include "pch.hpp"

namespace Is
{
    using std::string;
    using std::size_t;
    using std::shared_ptr;
    using std::map;
    using std::pair;
    
    class Object
    {
        const static size_t max_length_ = 1024;
        static HINSTANCE handler_instance_;
        shared_ptr<Object> parent_;

    public:
        Object(shared_ptr<Object> parent) : parent_(parent) {};
        virtual ~Object() {};

        static bool set_instance(HINSTANCE handler_instance)
        {
            if (handler_instance)
            {
                handler_instance_ = handler_instance;
                return true;
            }
            return false;
        }

        static HINSTANCE &instance() { return handler_instance_; }
    };

    using ObjectPtr = shared_ptr<Object>;
}

#endif // __OBJECT_HPP__