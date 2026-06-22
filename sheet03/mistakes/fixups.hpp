#define toString(...) #__VA_ARGS__

#define FUNCTION(function_name, op)     \
    int function_name(int a, int b) {   \
        return ((a) op(b) ? (a) : (b)); \
    }

#define INF (1 << 30)

#define Variable(type, name, ...) __VA_OPT__(type name = __VA_ARGS__);

#define SOME_MACRO 42

#define DEBUG(msg) log(__LINE__, __FILE__, msg)

#define foreach(container, var) for (size_t(i) = 0; (i) < (container).size(); (i)++)
