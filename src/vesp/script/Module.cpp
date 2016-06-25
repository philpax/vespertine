#include "vesp/script/Module.hpp"

#include "vesp/Log.hpp"

namespace vesp { namespace script {

	Module::Module(StringView title)
	{
		this->title_ = title.CopyToVector();
	}

	Module::~Module()
	{
	}

} }