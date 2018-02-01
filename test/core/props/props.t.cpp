#include "testing.t.hpp"

#include "core/props/props.hpp"

namespace otto::core::props {

  // Make sure that tags are sorted and uniquified correctly

  BOOST_HANA_CONSTANT_CHECK(
    black_magic::sort_tags(tag_list<mixins::has_value, mixins::steppable>()) ==
    black_magic::sort_tags(tag_list<mixins::steppable, mixins::has_value>()));

  BOOST_HANA_CONSTANT_CHECK(
    black_magic::sort_tags(
      tag_list<mixins::has_value, mixins::steppable, mixins::has_value>()) ==
    black_magic::sort_tags(tag_list<mixins::steppable, mixins::has_value>()));

  // Make sure that requirements are pulled in, and still sorted correctly

  BOOST_HANA_CONSTANT_CHECK(
    black_magic::with_required(tag_list<mixins::steppable>()) ==
    black_magic::sort_tags(tag_list<mixins::steppable, mixins::has_value>()));

  // Test the public `make_tag_list_t` type

  BOOST_HANA_CONSTANT_CHECK(
    make_tag_list_t<mixins::steppable>() ==
    black_magic::sort_tags(tag_list<mixins::steppable, mixins::has_value>()));

  TEST_CASE("Propeties", "[props]")
  {
    SECTION("Basic mixin tests")
    {
      // Test the tags of a property

      Property<float, mixins::steppable, mixins::has_name> pf;

      BOOST_HANA_CONSTANT_CHECK(
        pf.tag_list ==
        black_magic::sort_tags(
          tag_list<mixins::steppable, mixins::has_value, mixins::has_name>()));

      // Assert that the mixin implementation is inherited by the property
      static_assert(
        std::is_base_of_v<
          tag_mixin_t<mixins::steppable, float,
                      make_tag_list_t<mixins::steppable, mixins::has_name>>,
          decltype(pf)>);

      pf.set(3.f);
      REQUIRE(pf.get() == 3.f);
      pf.init<mixins::steppable>(2.f);
      pf.init<mixins::has_name>("property");
      pf.step(1);
      REQUIRE(pf == 5.f);
      float f = pf;
    }

    struct Props {
      Property<float, mixins::steppable, mixins::has_limits, mixins::has_name, mixins::faust_link> pf1 = 0.f;
      Property<float, mixins::steppable, mixins::has_name, mixins::faust_link> pf2 = 1.f;

      Props() {
        pf1.init<mixins::steppable>(1.f)
          .init<mixins::has_limits>(-5.f, 5.f);
      };
    } props;

    static_assert(detail::has_handler<typename decltype(props.pf1)::mixin<mixins::has_limits>, mixins::has_value::hooks::on_set>::value);

    REQUIRE(props.pf1 == 0.f);
    REQUIRE(props.pf2 == 1.f);
    props.pf1 = 10.f;
    REQUIRE(props.pf1 == 5.f);
  }
} // namespace otto::core::props
