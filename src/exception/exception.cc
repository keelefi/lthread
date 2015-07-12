#include "exception.hh"

namespace lthread
{

exception::exception(const char *msg) :
        error_msg(msg)
{
}

exception::exception(const std::string& msg) :
        error_msg(msg)
{
}

exception::~exception() throw()
{
}

const char *exception::what() throw()
{
    return error_msg.c_str();
}

} // namespace lthread
