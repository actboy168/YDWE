#pragma once

namespace win {
	class timer {
	public:
		typedef LARGE_INTEGER    native_timer_type        ;    ///< Native timer type
		typedef long long        native_duration_type    ;    ///< Native duration type

	private:
		native_timer_type    m_nFrequency    ;
		native_timer_type    m_nStart        ;

	public:
		timer()
		{
			::QueryPerformanceFrequency( &m_nFrequency );
			current( m_nStart )    ;
		}

		static void current( native_timer_type& tmr )
		{
			::QueryPerformanceCounter( &tmr )            ;
		}

		static native_timer_type current()
		{
			native_timer_type    tmr    ;
			current(tmr)    ;
			return tmr        ;
		}

		double reset()
		{
			native_timer_type nCur                ;
			current( nCur )                        ;
			double dblRet = double(nCur.QuadPart - m_nStart.QuadPart) / m_nFrequency.QuadPart    ;
			m_nStart.QuadPart = nCur.QuadPart    ;
			return dblRet    ;
		}

		double duration( native_duration_type dur )
		{
			return double( dur ) / m_nFrequency.QuadPart    ;
		}

		double duration()
		{
			return duration( native_duration() )    ;
		}

		native_duration_type    native_duration()
		{
			native_timer_type ts    ;
			current( ts )            ;
			return native_duration( m_nStart, ts )    ;
		}

		static native_duration_type    native_duration( const native_timer_type& nStart, const native_timer_type& nEnd )
		{
			return nEnd.QuadPart - nStart.QuadPart    ;
		}
	};
}
