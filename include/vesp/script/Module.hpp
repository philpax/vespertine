#pragma once

#include "vesp/String.hpp"

namespace vesp { namespace script {

	class Module
	{
	public:
		Module(StringView title);
		~Module();
		
	private:
		String title_;
	};

} }