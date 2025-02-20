#ifndef MIZU_OPERATIONS_LOOKUP_IS_AVAILABLE
#define MIZU_OPERATIONS_LOOKUP_IS_AVAILABLE

#define MIZU_REGISTER_OPERATION_FUNCTION(name) mizu::detail::register_operation(#name, name)
// NOTE: Needs to be first so that the version in opcode.hpp isn't emitted
#ifdef MIZU_IMPLEMENTATION
	#define MIZU_REGISTER_OPERATION(name) static bool name ## _registered = MIZU_REGISTER_OPERATION_FUNCTION(name) /* NOTE: Only emitted in implementation file*/
#endif


#include "../mizu/opcode.hpp"

#include <optional>
#include <string_view>
#include <unordered_map>
#include <fp/string.h>


namespace mizu {
	namespace detail {
		using id_t = size_t;

		#ifndef MIZU_IMPLEMENTATION
		extern
		#endif
		std::unordered_map<std::string_view, id_t> reverse_name_lookup;

		#ifndef MIZU_IMPLEMENTATION
		extern
		#endif
		std::unordered_map<mizu::operation_t, id_t> reverse_function_lookup;

		#ifndef MIZU_IMPLEMENTATION
		extern
		#endif
		std::unordered_map<id_t, std::pair<std::string_view, mizu::operation_t>> lookup;


		inline id_t register_operation(std::string_view name, mizu::operation_t ptr) {
			static id_t counter = 0;
			id_t id = counter++;
			reverse_name_lookup[name] = id;
			reverse_function_lookup[ptr] = id;
			lookup[id] = {name, ptr};
			return id;
		}
	}

	inline std::optional<detail::id_t> lookup_id(std::string_view name) {
		if(!detail::reverse_name_lookup.contains(name)) return {};
		return detail::reverse_name_lookup[name];
	}
	inline std::optional<detail::id_t> lookup_id(const fp_string_view name) { return lookup_id(fp_string_view_to_std(name)); }
	inline std::optional<detail::id_t> lookup_id(const fp_string name) { return lookup_id(fp_string_to_view_const(name)); }

	inline std::optional<detail::id_t> lookup_id(mizu::operation_t ptr) {
		if(!detail::reverse_function_lookup.contains(ptr)) return {};
		return detail::reverse_function_lookup[ptr];
	}

	inline std::optional<mizu::operation_t> lookup_pointer(detail::id_t id) {
		if(!detail::lookup.contains(id)) return {};
		return detail::lookup[id].second;
	}
	inline std::optional<fp_string_view> lookup_name(detail::id_t id) {
		if(!detail::lookup.contains(id)) return {};
		return fp_std_to_string_view(detail::lookup[id].first);
	}

	inline std::optional<mizu::operation_t> lookup(std::string_view name) {
		auto id = lookup_id(name);
		if(!id) return {};
		return lookup_pointer(*id);
	}
	inline std::optional<mizu::operation_t> lookup(const fp_string_view name) { return lookup(fp_string_view_to_std(name)); }
	inline std::optional<mizu::operation_t> lookup(const fp_string name) { return lookup(fp_string_to_view_const(name)); }

	inline std::optional<fp_string_view> lookup(mizu::operation_t ptr) {
		auto id = lookup_id(ptr);
		if(!id) return {};
		return lookup_name(*id);
	}


	inline void release_lookup_data() {
		detail::reverse_name_lookup.clear();
		detail::reverse_function_lookup.clear();
		detail::lookup.clear();
	}
}

#endif // MIZU_OPERATIONS_LOOKUP_IS_AVAILABLE