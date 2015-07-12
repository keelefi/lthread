#pragma once

#include <exception>
#include <string>

namespace lthread
{

class exception: public std::exception
{
public:
    exception(const char *msg);
    exception(const std::string& msg);
    virtual ~exception() throw();
    virtual const char *what() throw();

private:
    std::string error_msg;
};

} // namespace lthread
