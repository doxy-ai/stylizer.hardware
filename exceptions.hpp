#ifndef STYLIZER_EXCEPTION_IMPLEMENTATION
#define STYLIZER_EXCEPTION_IMPLEMENTATION

namespace stylizer {

#ifndef STYLIZER_NO_EXCEPTIONS
	struct error: public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	#define STYLIZER_THROW(x) throw stylizer::error((x))
#else
	#define STYLIZER_THROW(x) assert(((x), false)); std::exit(-22);
#endif

}

#endif // STYLIZER_EXCEPTION_IMPLEMENTATION