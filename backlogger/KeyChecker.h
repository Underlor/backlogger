#pragma once
#include <boost/algorithm/string/case_conv.hpp>
#include <string>
class KeyChecker
{
	bool ignore_, nowanswer_, newcfg_;
public:
	bool ignore() const
	{
		return ignore_;
	}

	bool nowanswer() const
	{
		return nowanswer_;
	}

	bool newcfg() const
	{
		return newcfg_;
	}


	//KeyChecker(KeyChecker* keys_): ignore_(keys_->ignore_), noanswer_(keys_->noanswer_), newcfg_(keys_->newcfg_){}
};
