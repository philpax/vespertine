#pragma once

#include "vesp/String.hpp"

#pragma warning(push)
#pragma warning(disable: 4200)
#include <mruby.h>
#pragma warning(pop)

namespace vesp { namespace script {

    class Module
    {
    public:
        Module(StringView title);
        ~Module();

        mrb_state* GetState();
        String ToString(mrb_value const value) const;
        std::tuple<mrb_value, mrb_value> Execute(StringView code);

    private:
        String title_;
        mrb_state* state_;
    };

} }