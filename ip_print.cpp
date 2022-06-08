#include <iostream>
#include <vector>
#include <list>
#include <tuple>

template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_ip(T value) {
    unsigned char* cp = reinterpret_cast<unsigned char*>(&value);
    size_t count_bytes = sizeof(value);

    while(count_bytes-- > 1){
        std::cout << +cp[count_bytes] << '.';
    }
    std::cout << +cp[count_bytes] << std::endl;
}

template <typename T>
using is_string = std::is_same<T, std::string>;

template <typename T>
typename std::enable_if<is_string<T>::value, void>::type
print_ip(const T &value) {
     std::cout << value << std::endl;
}


template <typename T>
using is_vector = std::is_same<T, std::vector<typename T::value_type,
        typename T::allocator_type>>;

template <typename T>
using is_list = std::is_same<T, std::list<typename T::value_type,
        typename T::allocator_type>>;

template<typename T>
typename std::enable_if<is_vector<T>::value || is_list<T>::value, void>::type
print_ip(const T &container) {
    auto value = container.begin();
    while(value != std::prev(container.end())) {
        std::cout << *value << '.';
        value++;
    }
    std::cout << *value << std::endl;
}

template<typename T>
auto to_unsigned(const T& t) {
    return std::make_unsigned_t<T>(t);
}

template<size_t ind, typename... Args>
struct is_same_elems {
    using type = typename std::tuple_element<ind, std::tuple<Args...> >::type;
    static constexpr bool value{std::is_same<type, typename is_same_elems<ind - 1, Args...>::type>::value
                                && is_same_elems<ind - 1, Args...>::value};
};

template<int ind, typename... Args>
struct print_ip_elem {
    void operator()(const std::tuple<Args...>& tuple, std::ostream &os = std::cout) {
        print_ip_elem<ind - 1, Args...>{}(tuple, os);
        os << "." << +(to_unsigned(std::get<ind>(tuple)));
    }
};

template<typename... Args>
struct is_same_elems<0, Args...> {
    using type = typename std::tuple_element<0, std::tuple<Args...> >::type;
    static constexpr bool value{true};
};

template<typename... Args>
struct print_ip_elem<0, Args...> {
    void operator()(const std::tuple<Args...>& tuple, std::ostream &os = std::cout) {
        os << +(to_unsigned(std::get<0>(tuple)));
    }
};

template<typename... Args>
struct is_same_tuple_elems {
    static constexpr size_t len{std::tuple_size<std::tuple<Args...>>::value};
    static constexpr bool value{is_same_elems<len - 1, Args...>::value};
};

template<typename... Args>
typename std::enable_if<is_same_tuple_elems<Args...>::value, void>::type
print_ip(const std::tuple<Args...>& tuple, std::ostream &os = std::cout) {
    const auto len = std::tuple_size<std::tuple<Args...>>::value;
    print_ip_elem<len - 1, Args...>{}(tuple, os);
}

int main() {
    print_ip( int8_t{-1} ); // 255
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::string{"Hello, World!"} ); // Hello, World!
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
}