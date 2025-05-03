#ifndef MIZU_INSTRUCTIONS_LOOKUP_IS_AVAILABLE
#define MIZU_INSTRUCTIONS_LOOKUP_IS_AVAILABLE

#define MIZU_REGISTER_INSTRUCTION_FUNCTION(name) mizu::detail::register_instruction(#name, name)
// NOTE: Needs to be first so that the version in opcode.hpp isn't emitted
#ifdef MIZU_IMPLEMENTATION
	#define MIZU_REGISTER_INSTRUCTION_PROTOTYPE(name) extern bool name ## _registered
	#define MIZU_REGISTER_INSTRUCTION(name) bool name ## _registered = MIZU_REGISTER_INSTRUCTION_FUNCTION(name) /* NOTE: Only emitted in implementation file*/
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
		std::unordered_map<mizu::instruction_t, id_t> reverse_function_lookup;

		#ifndef MIZU_IMPLEMENTATION
		extern
		#endif
		std::unordered_map<id_t, std::pair<std::string_view, mizu::instruction_t>> lookup;


		inline id_t register_instruction(std::string_view name, mizu::instruction_t ptr) {
			static id_t counter = 0;
			static id_t program_end = -1; if(program_end == -1) program_end = 0, program_end = register_instruction("program_end", nullptr);
			id_t id = counter++;
			reverse_name_lookup[name] = id;
			reverse_function_lookup[ptr] = id;
			lookup[id] = {name, ptr};
			return id;
		}
	}

/** \addtogroup lookup
 *  @{
 */

	/**
	 * Finds the ID of an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<detail::id_t> the id if found or std::nullopt if not
	 */
	inline std::optional<detail::id_t> lookup_id(std::string_view name) {
		if(!detail::reverse_name_lookup.contains(name)) return {};
		return detail::reverse_name_lookup[name];
	}
	/**
	 * Finds the ID of an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<detail::id_t> the id if found or std::nullopt if not
	 */
	inline std::optional<detail::id_t> lookup_id(const fp_string_view name) { return lookup_id(fp_string_view_to_std(name)); }
	/**
	 * Finds the ID of an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<detail::id_t> the id if found or std::nullopt if not
	 */
	inline std::optional<detail::id_t> lookup_id(const fp_string name) { return lookup_id(fp_string_to_view_const(name)); }

	/**
	 * Finds the ID of an instruction by its function pointer
	 * 
	 * @param ptr pointer to the instruction to find
	 * @return std::optional<detail::id_t> the id if found or std::nullopt if not
	 */
	inline std::optional<detail::id_t> lookup_id(mizu::instruction_t ptr) {
		if(!detail::reverse_function_lookup.contains(ptr)) return {};
		return detail::reverse_function_lookup[ptr];
	}

	/**
	 * Looks up the function pointer for an instruction based on its ID
	 * 
	 * @param id The ID to search for
	 * @return std::optional<mizu::instruction_t> callable function pointer if found or std::nullopt if not
	 */
	inline std::optional<mizu::instruction_t> lookup_pointer(detail::id_t id) {
		if(!detail::lookup.contains(id)) return {};
		return detail::lookup[id].second;
	}
	/**
	 * Looks up the name of an instruction based on its ID
	 * 
	 * @param id The ID to search for
	 * @return std::optional<fp_string_view> view of the instruction's name if found or std::nullopt if not
	 */
	inline std::optional<fp_string_view> lookup_name(detail::id_t id) {
		if(!detail::lookup.contains(id)) return {};
		return fp_std_to_string_view(detail::lookup[id].first);
	}

	/**
	 * Finds the function pointer for an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<mizu::instruction_t> callable function pointer if found or std::nullopt if not
	 */	
	inline std::optional<mizu::instruction_t> lookup(std::string_view name) {
		auto id = lookup_id(name);
		if(!id) return {};
		return lookup_pointer(*id);
	}
	/**
	 * Finds the function pointer for an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<mizu::instruction_t> callable function pointer if found or std::nullopt if not
	 */	
	inline std::optional<mizu::instruction_t> lookup(const fp_string_view name) { return lookup(fp_string_view_to_std(name)); }
	/**
	 * Finds the function pointer for an instruction by its name 
	 * 
	 * @param name the name of the instruction to find
	 * @return std::optional<mizu::instruction_t> callable function pointer if found or std::nullopt if not
	 */	
	inline std::optional<mizu::instruction_t> lookup(const fp_string name) { return lookup(fp_string_to_view_const(name)); }

	/**
	 * Finds the name for an instruction by its function pointer 
	 * 
	 * @param ptr pointer to the instruction to find
	 * @return std::optional<fp_string_view> view of the instruction's name if found or std::nullopt if not
	 */	
	inline std::optional<fp_string_view> lookup(mizu::instruction_t ptr) {
		auto id = lookup_id(ptr);
		if(!id) return {};
		return lookup_name(*id);
	}

	/**
	 * Releases all of the lookup tables used lookup instructions
	 * @note After lookup is no longer needed (say if the program has been loaded from disk and does not need to be written to disk again)
	 * 	this function can be called to free up memory in constrained scenarios.
	 */
	inline void release_lookup_data() {
		detail::reverse_name_lookup.clear();
		detail::reverse_function_lookup.clear();
		detail::lookup.clear();
	}
/** @}*/
}

#endif // MIZU_INSTRUCTIONS_LOOKUP_IS_AVAILABLE