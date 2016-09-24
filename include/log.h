#ifndef INCLUDE_LOG_H_
#define INCLUDE_LOG_H_

#include <nitro/log/log.hpp>

#include <nitro/log/sink/stderr.hpp>

#include <nitro/log/attribute/message.hpp>
#include <nitro/log/attribute/severity.hpp>
#include <nitro/log/attribute/timestamp.hpp>

#include <nitro/log/filter/and_filter.hpp>
#include <nitro/log/filter/severity_filter.hpp>


namespace leapDriver
{
namespace log
{

	namespace detail
	{

		typedef nitro::log::record<nitro::log::message_attribute,
								   nitro::log::severity_attribute>
			record;

		/** specifies how the output of the log shall be formatted.
		 *
		 */
		template <typename Record>
		class log_formater
		{
		public:
			std::string format(Record& r)
			{
				std::stringstream s;

				s << "LeapDriver:[" << r.severity() << "]: " << r.message();

				return s.str();
			}
		};

		template <typename Record>
		using log_filter = nitro::log::filter::severity_filter<Record>;
	}

	typedef nitro::log::logger<detail::record, detail::log_formater, nitro::log::sink::stderr,
							   detail::log_filter>
		logging;

	inline void set_min_severity_level(nitro::log::severity_level sev)
	{
		detail::log_filter<detail::record>::set_severity(sev);
	}

} // namespace log

using log::logging;
}

#endif /* INCLUDE_RRL_LOG_HPP_ */
