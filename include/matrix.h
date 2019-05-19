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
            /*!   \brief  Эта структура является ключом для unordered_map, где хранятся используемые ячейки разреженной матрицы

                          Структура содержит в себе координаты одной ячейки N-мерной матрицы. Координаты задаются для всех N измерений.

            */
            template <std::size_t Dimension>
            struct key
            {
                  using coordinates_t = std::array<std::size_t, Dimension>;
                  using iterator_t = typename coordinates_t::iterator;

                  coordinates_t coordinates = { 0 };
                  iterator_t it = coordinates.begin();

                  key() = delete;

                  key(std::size_t coordinate)
                  {
                        *it = coordinate;
                        ++it;
                  };

                  key(const key& key_arg) : coordinates(key_arg.coordinates)
                  {
                        it = coordinates.begin();
                        it += std::distance(const_cast<key&>(key_arg).coordinates.begin(), key_arg.it);
                  };

                  key(key&& key_arg) : it(key_arg.it)
                  {
                        coordinates.swap(key_arg.coordinates);
                  };

                  void push_back(std::size_t coordinate)
                  {
                        *it = coordinate;
                        ++it;
                  };

                  std::size_t get_hash() const
                  {
                        std::size_t h_value = std::hash<std::size_t>{}(coordinates[0]);
                        for (std::size_t shift = 1; shift < Dimension; ++shift)
                        {
                              std::size_t h_tmp = std::hash<std::size_t>{}(coordinates[shift]);
                              h_value ^= (h_tmp << shift);
                        }

                        return h_value;
                  };
            };

            template <std::size_t Dimension>
            bool operator==(const key<Dimension>& arg1, const key<Dimension>& arg2)
            {
                  return (arg1.coordinates == arg2.coordinates);
            }
      }
}

namespace std
{
      /*!   \brief  Это специализация std::hash для нашей структуры key. 

                    Это специализация функтора std::hash, который вычисляет хеш на основании координат из структуры key.
                    Этот хэш используется в unordered_map, где хранятся используемые ячейки разреженной матрицы
                    Эта специализация может быть объявлена в пространстве имен std
      */
      template <std::size_t Dimension>
      struct hash<roro_lib::internal::key<Dimension>>
      {
            using argument_t = roro_lib::internal::key<Dimension>;
            using result_t = std::size_t;

            result_t operator()(const argument_t& key_arg) const noexcept
            {
                  return key_arg.get_hash();
            }
      };
}


namespace roro_lib
{
      /*!   \brief  Это шаблонный класс N-мерной бесконечной разряженной матрицы.

             \tparam  T             -тип данных ячейки матрицы
             \tparam  default_value -значение по умолчанию для ячеек матрицы
             \tparam  Dimension     -размерность матицы
      */
      template <typename T, T default_value = 0, std::size_t Dimension = 2>
      class matrix
      {
            static_assert(Dimension != 0, "Dimension shoudn't be zero");

        public:
            template <std::size_t>
            struct indexation_matrix;
            template <typename>
            class matrix_iterator;

            using iternal_data_t = std::unordered_map<internal::key<Dimension>, T>;

            using value_type = T;
            using size_type = typename iternal_data_t::size_type;
            using iterator = matrix_iterator<typename iternal_data_t::iterator>;
            using const_iterator = const matrix_iterator<typename iternal_data_t::iterator>;


            matrix() = default;
            matrix(const matrix&) = default;
            matrix(matrix&&) = default;
            matrix& operator=(const matrix&) = default;
            matrix& operator=(matrix&&) = default;


            auto operator[](std::size_t row)
            {
                  return indexation_matrix<1>(um, row);
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
            iternal_data_t um;

        public:
            /*!   \brief  Вложенный класс N-мерной бесконечной разряженной  матрицы.<br>
                          Класс отвечает за доступ к ячейки матрицы через индексацию многомерного массива в стиле C

                   \tparam  I -индекс текущей размерности матрицы
            */
            template <std::size_t I>
            class indexation_matrix
            {
              public:
                  indexation_matrix(iternal_data_t& um, std::size_t row) : um(um),
                                                                           key(row)
                  {
                  }

                  template <std::size_t U>
                  indexation_matrix(const indexation_matrix<U>& arg) : um(arg.um),
                                                                       key(arg.key)
                  {
                  }

                  auto operator[](std::size_t column)
                  {
                        static_assert(I != Dimension,
                            "Error using operator[]: class 'matrix' has less dimensions.");

                        key.push_back(column);
                        return indexation_matrix<I + 1>(*this);
                  }

                  auto operator=(T value)
                  {
                        static_assert(I == Dimension,
                            "Error using operator[]: class 'matrix' has more dimensions.");

                        if (value != default_value)
                        {
                              um[key] = value;
                        }
                        else if (um.count(key) == 1)
                        {
                              um.erase(key);
                        }

                        return indexation_matrix<I>(*this);
                  }

                  operator T()
                  {
                        static_assert(I == Dimension,
                            "Error using operator[]: class 'matrix' has more dimensions.");

                        if (um.count(key) == 0)
                        {
                              return default_value;
                        }
                        else
                        {
                              return um[key];
                        }
                  }

                  template <std::size_t U>
                  friend class indexation_matrix;

              private:
                  iternal_data_t& um;
                  internal::key<Dimension> key;
            };

            /*!   \brief  Вложенный класс N-мерной бесконечной разряженной  матрицы.<br>
                          Класс отвечает за итерацию всех ячеек матрицы, что заняты НЕ значениями по умолчанию

                  При разыменовании итератора получаем std::tuple содержащий координаты и значение данной ячейки матрицы.
            */
            template <typename MapIter>
            class matrix_iterator
            {
              private:
                  MapIter current_internal_iter;

                  using node_t = decltype(std::tuple_cat(current_internal_iter->first.coordinates,
                                                         std::make_tuple(current_internal_iter->second)));

                  node_t current_node;

              public:
                  using iterator_category = std::forward_iterator_tag;
                  using value_type = node_t;
                  using difference_type = ptrdiff_t;
                  using pointer = value_type*;
                  using reference = value_type&;

                  matrix_iterator() noexcept : matrix_iterator(nullptr) {}
                  matrix_iterator(MapIter it) noexcept : current_internal_iter(it) {}

                  template <typename U>
                  matrix_iterator(const matrix_iterator<U>& iter) noexcept : current_internal_iter(iter.current_internal_iter)
                  {
                  }

                  template <typename U>
                  matrix_iterator& operator=(const matrix_iterator<U>& iter) noexcept
                  {
                        if (this == &iter)
                              return *this;

                        current_internal_iter = iter.current_internal_iter;
                        return *this;
                  }

                  value_type* operator->()
                  {
                        current_node = std::tuple_cat(current_internal_iter->first.coordinates,
                                                      std::make_tuple(current_internal_iter->second));
                        return &current_node;
                  }

                  value_type& operator*()
                  {
                        current_node = std::tuple_cat(current_internal_iter->first.coordinates,
                                                      std::make_tuple(current_internal_iter->second));
                        return current_node;
                  }

                  template <typename U>
                  bool operator==(const matrix_iterator<U>& iter) const 
                  {
                        return current_internal_iter == iter.current_internal_iter;
                  }

                  template <typename U>
                  bool operator!=(const matrix_iterator<U>& iter) const 
                  {
                        return current_internal_iter != iter.current_internal_iter;
                  }

                  matrix_iterator& operator++()
                  {
                        ++current_internal_iter;
                        return *this;
                  }

                  matrix_iterator operator++(int)
                  {
                        matrix_iterator old_iter = *this;
                        current_internal_iter++;
                        return old_iter;
                  }
            };
      };
}
