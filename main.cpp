#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}


// #include <iostream>
// #include <string_view>
//
// // ---------------------------------------------------------
// // C++26 Feature 1: Deleted functions with a custom message
// // Before C++26, you could only do `= delete;`
// // ---------------------------------------------------------
// void old_function() = delete("Use new_function() instead! (C++26 feature check)");
//
// void new_function() {
//     std::cout << "1. Deleted function messages are supported!\n";
// }
//
// // ---------------------------------------------------------
// // C++26 Feature 2: Pack Indexing
// // You can now index directly into a parameter pack using `...[N]`
// // ---------------------------------------------------------
// template <std::size_t Index, typename... Args>
// auto get_nth_argument(Args... args) {
//     // Returns the Nth argument from the pack!
//     return args...[Index];
// }
//
// int main() {
//     std::cout << "--- C++26 Environment Test ---\n\n";
//
//     // ---------------------------------------------------------
//     // C++26 Feature 3: Placeholder variables with no name (_)
//     // You can declare multiple variables named `_` in the same scope
//     // without getting a "redefinition" compiler error.
//     // ---------------------------------------------------------
//     int _ = 42;
//     int _ = 100;
//     std::cout << "2. Nameless placeholder variables (_) are supported!\n";
//
//     // Testing the pack indexing function
//     // We are grabbing index 1 (the second argument, which is the double)
//     auto indexed_val = get_nth_argument<1>("String", 3.14159, 42);
//     std::cout << "3. Pack indexing works! Retrieved value: " << indexed_val << '\n';
//
//     // (Optional) Uncomment the line below to see the custom C++26 compiler error!
//     // old_function();
//
//     std::cout << "\nAll C++26 tests passed successfully!\n";
//     return 0;
// }