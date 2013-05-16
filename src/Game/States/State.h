#pragma once

#include "../../System/Clock.h"
#include "../../System/Event.h"

#ifdef DEBUG
	#define stt_name_decl() const char *name() const
	#define stt_name(T) const char *T::name() const { return #T; }
#else
	#define stt_name_decl()
	#define stt_name(T)
#endif

namespace stt
{
	class State
	{
	public:
		virtual ~State() {}

		virtual void update(Time dt) = 0;
		virtual void draw(float percent) = 0;
		virtual void event(const Event &evt) = 0;

		#ifdef DEBUG
			virtual const char *name() const = 0;
		#endif
	};
}
