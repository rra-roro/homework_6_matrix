#include <unordered_map>
#include <functional>
#include <cassert>
#include <initializer_list>
#include <tuple>

namespace roro_lib
{
      namespace internal
      {
            struct KeyYX
            {
                  size_t row;
                  size_t column;                  
                  KeyYX(std::initializer_list<size_t> i_list) : row(*i_list.begin()),
                                                                column(*(i_list.end() - 1)){};
                  KeyYX(const KeyYX& key) : row(key.row), column(key.column){};
                  KeyYX(KeyYX&& key) : row(key.row), column(key.column){};
            };

            bool operator==(const KeyYX& arg1, const KeyYX& arg2) noexcept
            {
                  return (arg1.row == arg2.row) && (arg1.column == arg2.column);
            }            
      }
}

namespace std
{
      // custom specialization of std::hash can be injected in namespace std
      template <>
      struct hash<roro_lib::internal::KeyYX>
      {
            using argument_type = roro_lib::internal::KeyYX;
            using result_type = size_t;

            result_type operator()(const argument_type& key) const noexcept
            {
                  result_type const h1(std::hash<decltype(key.row)>{}(key.row));
                  result_type const h2(std::hash<decltype(key.column)>{}(key.column));
                  return h1 ^ (h2 << 1);
            }
      };
}


namespace roro_lib
{
      template <typename T, T default_value = 0>
      class matrix
      {
        public:
            struct matrix_internal;
            template <typename MapIter>
            class matrix_iter;

            using iternal_type = std::unordered_map<internal::KeyYX, T>;
            using size_type = typename iternal_type::size_type;

            using iterator = matrix_iter<typename iternal_type::iterator>;
            using const_iterator = const matrix_iter<typename iternal_type::iterator>;


            matrix() = default;
            matrix(const matrix&) = default;
            matrix(matrix&&) = default;
            matrix& operator=(const matrix&) = default;
            matrix& operator=(matrix&&) = default;


            matrix_internal operator[](size_t row)
            {
                  return matrix_internal(um, row);
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

            struct matrix_internal
            {
                  iternal_type& um;
                  internal::KeyYX key = { 0, 0 };
                  matrix_internal(iternal_type& um, size_t row) : um(um), key{ row, 0 } {}

                  struct item_t;

                  matrix_internal& operator[](size_t column)
                  {
                        key.column = column;
                        return *this;
                  }

                  matrix_internal& operator=(T value)
                  {
                        if (value != default_value)
                              um[key] = value;
                        else if (um.count(key) == 1)
                        {
                              um.erase(key);
                        }

                        return *this;
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
                  std::tuple<std::size_t, std::size_t, T> current_node;

              public:
                  using iterator_category = std::forward_iterator_tag;
                  using value_type = std::tuple<std::size_t, std::size_t, T>;
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
                        current_node = std::make_tuple(current_iter->first.row,
                                                       current_iter->first.column,
                                                       current_iter->second);
                        return &current_node;
                  }

                  value_type& operator*() noexcept
                  {
                        current_node = std::make_tuple(current_iter->first.row,
                                                       current_iter->first.column,
                                                       current_iter->second);
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
