#include <slk/converter/AttributeMeta.hpp>

namespace slk
{
	template <> SLKLIB_API
		std::string AttributeMeta<ObjectWithOptinal>::ToSlkName(Attribute<ObjectWithOptinal> const& that) const
	{
		if (that.GetLevel() > 0)
		{
			std::string slkName;
			std::stringstream ss;
			if (meta_->field == "vertR" || meta_->field == "vertG" || meta_->field == "vertB")
				ss << meta_->field << std::setfill('0') << std::setw(2) << that.GetLevel();
			else
				ss << meta_->field << that.GetLevel();

			ss >> slkName;
			return std::move(slkName);
		}
		else
		{
			return meta_->field;
		}
	}

	template <> SLKLIB_API
		std::string AttributeMeta<ObjectWithoutOptinal>::ToSlkName(Attribute<ObjectWithoutOptinal> const& /*that*/) const
	{
		return meta_->field;
	}
}
