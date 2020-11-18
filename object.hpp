#ifndef __IS_OBJECT_HPP__
#define __IS_OBJECT_HPP__
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
        shared_ptr<Object> parent_;

    public:
        Object(shared_ptr<Object> parent) : parent_(parent) {};
        virtual ~Object() {};

    };

    using ObjectPtr = shared_ptr<Object>;
}

#endif // __IS_OBJECT_HPP__