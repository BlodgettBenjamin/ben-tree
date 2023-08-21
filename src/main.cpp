#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

#include "ben/comment.h"
#include "ben/type.h"
#include "ben/memory.h"
#include "ben/string.h"
#include "ben/io.h"
#include "ben/parpack.h"
#include "ben/tree.h"

namespace user
{
	struct vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct color
	{
		float r = 1.0f;
		float g = 0.5f;
		float b = 0.0f;
		float a = 1.0f;
	};
}

int main()
{
	btl::tree<user::vec3, user::color, ben::str120> tree;

	user::vec3 vector_data[] = {
		{1.0f, 2.5f, 4.5f}, {9.5f, 2.7f, 9.2f}
	};
	user::color color_data[] = {
		{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}
	};
	user::color some_more_color_data[] = {
		{0.55f, 0.23f, 0.6f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {0.15f, 0.23f, 0.45f, 1.0f}, {0.86f, 0.17f, 0.92f, 1.0f}
	};
	ben::str120 str_data[] = {
		"stinky", "chungus", "obamanomics", "epic win!11", "yep"
	};
	tree.add(vector_data, BEN_COUNTOF(vector_data));
	tree.add(0, color_data, BEN_COUNTOF(color_data));
	tree.add(1, some_more_color_data, BEN_COUNTOF(some_more_color_data));
	tree.add(1, str_data, BEN_COUNTOF(str_data));

	ben::stru64 info_buffer_vec3;
	ben::stru64 info_buffer_color;
	ben::stru64 info_buffer_str;

	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<user::vec3>(), tree.size<user::vec3>(), sizeof(user::vec3));
	for (const auto& vector : btl::make_iterable<user::vec3>(&tree))
		info_buffer_vec3.catf("-{ %.1f, %.1f, %.1f }\n", vector.x, vector.y, vector.z);

	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<user::color>(), tree.size<user::color>(), sizeof(user::color));
	for (const auto& color : btl::make_iterable<user::color>(&tree))
	{
		const auto& vector = tree.get_parent<user::vec3>(color);

		info_buffer_color.catf("-{ %.1f, %.1f, %.1f, %.1f }\tparent : -{ %.1f, %.1f, %.1f }\n",
			color.r, color.g, color.b, color.a, vector.x, vector.y, vector.z);
	}

	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<ben::str120>(), tree.size<ben::str120>(), sizeof(ben::str120));
	for (const auto& str : btl::make_iterable<ben::str120>(&tree))
	{
		const auto& col = tree.get_parent<user::color>(str);

		info_buffer_str.catf("-%s\tparent : -{ %.1f, %.1f, %.1f, %.1f }\n",
			str, col.r, col.g, col.b, col.a);
	}

	ben::print(info_buffer_vec3);
	ben::print(info_buffer_color);
	ben::print(info_buffer_str);

	struct obama {};
	struct poop {};
	struct gaming {};
	u64 count, index;
	ben::str120 is_set;

	using pog = btl::pack<poop, obama, gaming>;
	using poo = btl::pack<poop, obama, obama>;
	using pgpgpo = btl::pack<poop, gaming, poop, gaming, poop, obama>;
	using gg = btl::pack<gaming, gaming>;

	count  = pog::count;                                                                                                       assert(count == 3);
	index  = pog::index<obama>;                                                                                                assert(index == 1);
	is_set = pog::is_set ? "true" : "false";                                                                                   assert(is_set == "true");
	ben::printf("\n<poop, obama, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = poo::count;                                                                                                       assert(count == 3);
	index  = poo::index<obama>;                                                                                                assert(index == 1);
	is_set = poo::is_set ? "true" : "false";                                                                                   assert(is_set == "false");
	ben::printf("<poop, obama, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = pgpgpo::count;                                                                                                    assert(count == 6);
	index  = pgpgpo::index<obama>;                                                                                             assert(index == 5);
	is_set = pgpgpo::is_set ? "true" : "false";                                                                                assert(is_set == "false");
	ben::printf("<poop, gaming, poop, gaming, poop, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = gg::count;                                                                                                        assert(count == 2);
	index  = gg::index<obama>;                                                                                                 assert(index == -1);
	is_set = gg::is_set ? "true" : "false";                                                                                    assert(is_set == "false");
	ben::printf("<gaming, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);

	btl::tree<user::vec3, user::color, ben::str120> good_tree;
	//btl::tree<vec3, color, ben::str120*>            bad_tree; (no layers of pointers)
	//btl::tree<vec3, color, ben::str120&>            bad_tree; (no layers of references)
	//btl::tree<vec3, color, ben::stru64>             bad_tree; (cant check for trivial serializeable at compile time so this will compile)
	//btl::tree<vec3, color, std::vector<ben::str120> bad_tree; (std::vector is clearly not trivially copyable so this wont compile)
	//struct func { void operator() () { return; } };
	//btl::tree<user::vec3, user::color, func()>      bad_tree; (functions aren't trivially copyable)
	if (std::is_trivially_copyable<ben::stru64>::value)
	{
		ben::stru64 str0, str1;
		str0.catf("A trivially copyable type is a type whose storage is contiguous");
		memcpy(&str1, &str0, sizeof(ben::stru64));                                                                             assert(memcmp(&str1, &str0, sizeof(ben::stru64)) == 0);
	}

	tree.print_conceptual();

	return 0;
}