#pragma once

#include "utl/helpers/algorithm.h"

#include "nigiri/timetable.h"

namespace motis {

// The routes stopping at `l`, including those at its virtual locations:
// transfers.txt rules move trips there, but they still stop at `l`.
template <typename Fn>
void for_each_route_at(nigiri::timetable const& tt,
                       nigiri::location_idx_t const l,
                       Fn&& fn) {
  for (auto const r : tt.location_routes_[l]) {
    fn(r);
  }
  tt.locations_.for_each_virt(l, [&](nigiri::location_idx_t const v) {
    for (auto const r : tt.location_routes_[v]) {
      fn(r);
    }
  });
}

// Whether `pred` holds for one of the routes stopping at `l` (see
// for_each_route_at). No route is checked after the first match.
template <typename Pred>
bool any_route_at(nigiri::timetable const& tt,
                  nigiri::location_idx_t const l,
                  Pred&& pred) {
  auto any = utl::any_of(tt.location_routes_[l], pred);
  tt.locations_.for_each_virt(l, [&](nigiri::location_idx_t const v) {
    any = any || utl::any_of(tt.location_routes_[v], pred);
  });
  return any;
}

inline bool has_routes(nigiri::timetable const& tt,
                       nigiri::location_idx_t const l) {
  return any_route_at(tt, l, [](nigiri::route_idx_t) { return true; });
}

inline nigiri::hash_set<std::string_view> get_location_routes(
    nigiri::timetable const& tt, nigiri::location_idx_t const l) {
  auto names = nigiri::hash_set<std::string_view>{};
  for_each_route_at(tt, l, [&](nigiri::route_idx_t const r) {
    for (auto const t : tt.route_transport_ranges_[r]) {
      names.emplace(tt.transport_name(t));
    }
  });
  return names;
}

}  // namespace motis