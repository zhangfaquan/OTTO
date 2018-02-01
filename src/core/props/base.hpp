#pragma once

#include "tag_list.hpp"

namespace otto::core::props {

  // PropertyBase /////////////////////////////////////////////////////////////

  template<typename T, typename TagList>
  struct PropertyImpl : inherits_from_mixins_t<T, TagList> {
    using value_type = T;
    using tag_list_t = TagList;
    constexpr static tag_list_t tag_list;

    template<typename Tag>
    using mixin = tag_mixin_t<Tag, T, tag_list_t>;

    using inherits_from_mixins_t<T, TagList>::inherits_from_mixins_t;
    using inherits_from_mixins_t<T, TagList>::operator=;

    /// Initialize the mixin of type `Tag` with args
    template<typename Tag, typename... Args>
    auto init(Args&&... args)
      -> std::enable_if_t<contains_tag_v<tag_list_t, Tag>,
                          decltype(mixin<Tag>::init(std::declval<Args>()...),
                                   std::declval<PropertyImpl&>())>
    {
      mixin<Tag>::init(std::forward<Args>(args)...);
      return *this;
    }

    template<typename Tag>
    constexpr static bool is =
      ::otto::core::props::contains_tag_v<tag_list_t, Tag>;

    template<typename Tag>
    constexpr auto as() -> ::std::enable_if_t<
      PropertyImpl::is<Tag>,
      ::otto::core::props::tag_mixin_t<Tag, value_type, tag_list_t>&>
    {
      return static_cast<
        ::otto::core::props::tag_mixin_t<Tag, value_type, tag_list_t>&>(*this);
    }

    template<typename Tag>
    constexpr auto as() const -> ::std::enable_if_t<
      PropertyImpl::is<Tag>,
      const ::otto::core::props::tag_mixin_t<Tag, value_type, tag_list_t>&>
    {
      return static_cast<
        const ::otto::core::props::tag_mixin_t<Tag, value_type, tag_list_t>&>(
        *this);
    }

    template<typename Hook>
    using hook = detail::hook_t<Hook, value_type>;

    template<typename Hook>
    typename hook<Hook>::arg_type run_hook(
      const typename hook<Hook>::arg_type& arg)
    {
      return ::otto::core::props::detail::run_hook<Hook>(*this, arg);
    }

    template<typename Hook>
    typename hook<Hook>::arg_type run_hook(
      const typename hook<Hook>::arg_type& arg) const
    {
      return ::otto::core::props::detail::run_hook<Hook>(*this, arg);
    }
  };

  template<typename ValueType, typename... Tags>
  using Property = PropertyImpl<ValueType, make_tag_list_t<Tags...>>;

} // namespace otto::core::props
