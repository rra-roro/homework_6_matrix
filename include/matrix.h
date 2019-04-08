#include <unordered_map>
#include <functional>
#include <cassert>
#include <initializer_list>
#include <tuple>
#include <array>
#include <iterator>

namespace roro_lib
{
      namespace internal
      {
            template <std::size_t Dimension>
            struct key_t
            {
                  using coordinates_t = std::array<std::size_t, Dimension>;
                  using iterator = typename coordinates_t::iterator;

                  coordinates_t coordinates = { 0 };
                  iterator it = coordinates.begin();

                  key_t(std::size_t coordinate)
                  {
                        *it = coordinate;
                        ++it;
                  };

                  key_t(const key_t& key) : coordinates(key.coordinates),
                                            it(coordinates.begin() +
                                                std::distance(const_cast<key_t&>(key).coordinates.begin(), key.it))
                  {
                  };

                  key_t(key_t&& key) : it(key.it)
                  {
                        coordinates.swap(key.coordinates);
                  };

                  void push_back(std::size_t coordinate)
                  {
                        *it = coordinate;
                        ++it;
                  };
            };

            template <std::size_t Dimension>
            bool operator==(const key_t<Dimension>& arg1, const key_t<Dimension>& arg2) noexcept
            {
                  return (arg1.coordinates == arg2.coordinates);
            }
      }
}

namespace std
{
      // custom specialization of std::hash can be injected in namespace std
      template <std::size_t Dimension>
      struct hash<roro_lib::internal::key_t<Dimension>>
      {
            using argument_type = roro_lib::internal::key_t<Dimension>;
            using result_type = std::size_t;

            result_type operator()(const argument_type& key) const noexcept
            {
                  using type_hash = typename decltype(key.coordinates)::value_type;

                  result_type h = std::hash<type_hash>{}(key.coordinates[0]);
                  for (std::size_t shift = 1; shift < Dimension; ++shift)
                  {
                        result_type h_tmp = std::hash<type_hash>{}(key.coordinates[shift]);
                        h ^= (h_tmp << shift);
                  }

                  return h;
            }
      };
}


namespace roro_lib
{
      template <typename T, T default_value = 0, std::size_t Dimension = 2>
      class matrix
      {
            static_assert(Dimension != 0, "Dimension shoudn't be zero");

        public:
            template <std::size_t I>
            struct matrix_internal;
            template <typename MapIter>
            class matrix_iter;

            using iternal_type = std::unordered_map<internal::key_t<Dimension>, T>;
            using size_type = typename iternal_type::size_type;

            using iterator = matrix_iter<typename iternal_type::iterator>;
            using const_iterator = const matrix_iter<typename iternal_type::iterator>;


            matrix() = default;
            matrix(const matrix&) = default;
            matrix(matrix&&) = default;
            matrix& operator=(const matrix&) = default;
            matrix& operator=(matrix&&) = default;


            auto operator[](std::size_t row)
            {
                  return matrix_internal<1>(um, row);
            }

            size_type size() noexcept
            {
                  return um.size();
            }

            iterator begin() noexcept
            {
                  return iterator(um.begin());
            }

            iterator end() noexcept
            {
                  return iterator(um.end());
            }

            const_iterator cbegin() noexcept
            {
                  return const_iterator(um.cbegin());
            }

            const_iterator cend() noexcept
            {
                  return const_iterator(um.cend());
            }

        private:
            iternal_type um;

        public:
            template <std::size_t I>
            struct matrix_internal
            {
                  iternal_type& um;
                  internal::key_t<Dimension> key;
                  matrix_internal(iternal_type& um, std::size_t row) : um(um), key(row) {}

                  template <std::size_t U>
                  matrix_internal(const matrix_internal<U>& arg) : um(arg.um), key(arg.key) {}

                  auto operator[](std::size_t column)
                  {
                        static_assert(I != Dimension, "Error using operator[]: class 'matrix' has less dimensions.");
                        key.push_back(column);
                        return matrix_internal<I + 1>(*this);
                  }

                  auto operator=(T value)
                  {
                        static_assert(I == Dimension, "Error using operator[]: class 'matrix' has more dimensions.");

                        if (value != default_value)
                              um[key] = value;
                        else
                        if (um.count(key) == 1)
                        {
                              um.erase(key);
                        }

                        return matrix_internal<I>(*this);
                  }

                  operator T()
                  {
                        if (um.count(key) == 0)
                        {
                              return default_value;
                        }
                        else
                        {
                              return um[key];
                        }
                  }
            };

            template <typename MapIter>
            class matrix_iter
            {
                  MapIter current_iter;

                  decltype(std::tuple_cat(current_iter->first.coordinates, std::make_tuple(current_iter->second))) current_node;

              public:
                  using iterator_category = std::forward_iterator_tag;
                  using value_type = decltype(current_node);
                  using difference_type = ptrdiff_t;
                  using pointer = value_type*;
                  using reference = value_type&;

                  matrix_iter() noexcept : matrix_iter(nullptr) {}
                  matrix_iter(MapIter it) noexcept : current_iter(it) {}
                  template <typename U>
                  matrix_iter(const matrix_iter<U>& iter) noexcept : current_iter(iter.current_iter) {}

                  template <typename U>
                  matrix_iter& operator=(const matrix_iter<U>& iter) noexcept
                  {
                        if (this == &iter)
                        {
                              return *this;
                        }
                        current_iter = iter.current_iter;
                        return *this;
                  }

                  value_type* operator->()
                  {
                        current_node = std::tuple_cat(current_iter->first.coordinates, std::make_tuple(current_iter->second));
                        return &current_node;
                  }

                  value_type& operator*() noexcept
                  {
                        current_node = std::tuple_cat(current_iter->first.coordinates, std::make_tuple(current_iter->second));
                        return current_node;
                  }

                  template <typename U>
                  bool operator==(const matrix_iter<U>& iter) const noexcept
                  {
                        return current_iter == iter.current_iter;
                  }

                  template <typename U>
                  bool operator!=(const matrix_iter<U>& iter) const noexcept
                  {
                        return current_iter != iter.current_iter;
                  }

                  matrix_iter& operator++() noexcept
                  {
                        ++current_iter;
                        return *this;
                  }

                  matrix_iter operator++(int)
                  {
                        matrix_iter old_iter = *this;
                        current_iter++;
                        return old_iter;
                  }
            };
      };
}
