#ifndef _TICKET_SYSTEM_EXCEPTION_H_
#define _TICKET_SYSTEM_EXCEPTION_H_

#include <cstddef>
#include <cstring>
#include <string>

namespace ticket {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
    exception(const std::string &var, const std::string &det): variant(var), detail(det) {}
	exception(const exception &e): variant(e.variant), detail(e.detail) {}
	virtual std::string what() {
		return "[" + variant + "] " + detail;
	}
};

/**
 * @brief container exceptions
 */
class container_error: public exception {
public: 
    container_error(): exception() {}
    container_error(const std::string &msg): exception("containner_error", msg) {}
};

class index_out_of_bound: public container_error {
public:
    index_out_of_bound(): container_error("index_out_of_bound") {}
    index_out_of_bound(const std::string &msg): container_error("index_out_of_bound, " + msg) {}
};

class runtime_error: public container_error {
public: 
    runtime_error(): container_error("runtime_error") {}
    runtime_error(const std::string &msg): container_error("runtime_error, " + msg) {}
};

class invalid_iterator: public container_error {
public: 
	invalid_iterator(): container_error("invalid_interator") {}
    invalid_iterator(const std::string &msg): container_error("invalid_iterator, " + msg) {}
};

class container_is_empty: public container_error {
public: 
	container_is_empty(): container_error("container_is_empty") {}
    container_is_empty(const std::string &msg): container_error("container_is_empty, " + msg) {}
};

/**
 * @brief general exceptions
 */
struct invalid_argument: public exception {
    invalid_argument(): exception("invalid_argument", "no detail") {}
    invalid_argument(const std::string &msg): exception("invalid_argument", msg) {}
};
struct length_error: public exception {
    length_error(): exception("length_error", "no detail") {}
    length_error(const std::string &msg): exception("length_error", msg) {}
};
struct out_of_range: public exception {
    out_of_range(): exception("out_of_range", "no detail") {}
    out_of_range(const std::string &msg): exception("out_of_range", msg) {}
};
struct range_error: public exception {
    range_error(): exception("range_error", "no detail") {}
    range_error(const std::string &msg): exception("range_error", msg) {}
};

/**
 * @brief user exceptions
 */
struct user_error: public exception {
    user_error(): exception("user_error", "no detail") {}
    user_error(const std::string &msg): exception("user_error", msg) {}
};
/**
 * @brief train exceptions
 */
struct train_error: public exception {
    train_error(): exception("train_error", "no detail") {}
    train_error(const std::string &msg): exception("train_error", msg) {}
};

/**
 * @brief transaction exceptions
 */
struct transaction_error: public exception {
    transaction_error(): exception("transaction_error", "no detail") {}
    transaction_error(const std::string &msg): exception("transaction_error", msg) {}
};

/**
 * @brief query exceptions
 */
struct query_exceptions: public exception {
    query_exceptions(): exception("query_exceptions", "no detail") {}
    query_exceptions(const std::string &msg): exception("query_exceptions", msg) {}
};

/**
 * @brief rollback exceptions
 */
struct rollback_error: public exception {
    rollback_error(): exception("rollback_error", "no detail") {}
    rollback_error(const std::string &msg): exception("rollback_error", msg) {}
};

}

#endif
