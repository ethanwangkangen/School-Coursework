#include <iostream>

// DO NOT MODIFY THE PROGRAM
//
// Task: There is a bad-casting vulnerability in this program.
//       Construct an input string that if provided to this program as stdin, 
//       will cause it to print "Congratulations! You have successfully modified the USER_ID!"
//
// Setup: 
//   - OnlineGDB (C++); or g++ 11.4.0 (for x64)
//   - Extra compiler flags: -fno-stack-protector -O0 -g -no-pie
//   - No Command line arguments.
//   - Standard Input: choose `text` and fill in your input string.
//
// Check it your exploit works:
//   1. click on the `Run` button in OnlineGDB 
//      NOTE: your exploit should work outside of the debugger as well.
//            It doesn't need to depend on stack addresses.
//   2. Check if 'Congratulations! You have successfully modified the USER_ID!' is printed.
// 
// Expected stdout (between markers):
//   ...
//   === START ===
//   Congratulations! You have successfully modified the USER_ID!
//   === DONE ===
    
    
class BaseInt {
public:
    int base_int;
};

class BaseFloat {
public:
    float base_float;
};

class NumberA : public BaseInt, public BaseFloat {};

class NumberB : public BaseFloat, public BaseInt {};

enum class Type {
    BaseInt,
    BaseFloat,
    NumberA,
    NumberB
};

int main() {
    int USER_ID = 3235;

    NumberA number;
    number.base_int = 15;
    number.base_float = 15;

    NumberA* numbera_ptr = &number;
    NumberB* numberb_ptr = NULL;
    BaseInt* base_int_ptr = NULL;
    BaseFloat* base_float_ptr = NULL;

    Type num_type = Type::NumberA;

    // In C++, static_cast ensures that the type conversion happens between potentially compatible types.
    // converting NumberA* to BaseInt* or BaseInt* to NumberA* are allowed,
    // but converting NumberA* to NumberB* or NumberB* to NumberA* are not allowed.
    // Thus, the following line results in compilation error:
    // 
    // NumberA* numbera_ptr = ...;
    // NumberB* _ptr = static_cast<NumberB*>(numbera_ptr);  // compile-time error
    // ^ ~~~~ ERROR: Static_cast from 'NumberA *' to 'NumberB *', which are not related by inheritance, is not allowed
    // 
    // While converting between types with inheritance relationship is allowed by static_cast, it can be dangerous, as explained in class.
    //
    // Now your job is to corrupt the user_id from 3235 to 0 and trigger the execution of the if-branch at the end of the main function.
    
    // A for loop reading commands line-by-line from the user.
    // The commands can be:
    // - `ADD <float>`
    // - `CAST <BaseInt|BaseFloat|NumberA|NumberB>`
    // - `PRINT`
    // - `EXIT`

    while (true) {
        std::string command;
        std::cin >> command;
        if (command == "ADD") {
            float value;
            std::cin >> value;
            if (num_type == Type::NumberA) {
                numbera_ptr->base_int += static_cast<int>(value);
                numbera_ptr->base_float += value;
            } else if (num_type == Type::NumberB) {
                numberb_ptr->base_int += static_cast<int>(value);
                numberb_ptr->base_float += value; 
            } else {
                std::cout << "Please convert to NumberA/B type first!" << std::endl;
                exit(17);
            }
        } else if (command == "CAST") {
            std::string new_type;
            std::cin >> new_type;
            if (new_type == "BaseInt") {
                switch (num_type) {
                    case Type::BaseInt: break;
                    case Type::BaseFloat: std::cout << "Invalid cast!" << std::endl; exit(16); break;
                    case Type::NumberA: base_int_ptr = static_cast<BaseInt*>(numbera_ptr); break;
                    case Type::NumberB: base_int_ptr = static_cast<BaseInt*>(numberb_ptr); break;
                }
                num_type = Type::BaseInt;
            } else if (new_type == "BaseFloat") {
                switch (num_type) {
                    case Type::BaseInt: std::cout << "Invalid cast!" << std::endl; exit(16); break;
                    case Type::BaseFloat: break;
                    case Type::NumberA: base_float_ptr = static_cast<BaseFloat*>(numbera_ptr); break;
                    case Type::NumberB: base_float_ptr = static_cast<BaseFloat*>(numberb_ptr); break;
                }
                num_type = Type::BaseFloat;
            }  else if (new_type == "NumberA") {
                switch (num_type) {
                    case Type::BaseInt: numbera_ptr = static_cast<NumberA*>(base_int_ptr); break;
                    case Type::BaseFloat: numbera_ptr = static_cast<NumberA*>(base_float_ptr); break;
                    case Type::NumberA: break;
                    case Type::NumberB: std::cout << "Invalid cast!" << std::endl; exit(16); break;
                }
                num_type = Type::NumberA;
            } else if (new_type == "NumberB") {
                switch (num_type) {
                    case Type::BaseInt: numberb_ptr = static_cast<NumberB*>(base_int_ptr); break;
                    case Type::BaseFloat: numberb_ptr = static_cast<NumberB*>(base_float_ptr); break;
                    case Type::NumberA: std::cout << "Invalid cast!" << std::endl; exit(16); break;
                    case Type::NumberB: break;
                }
                num_type = Type::NumberB;
            } else {
                std::cout << "CAST failed: Invalid target type!" << std::endl;
                exit(15);
            }
        } else if (command == "PRINT") {
            // print <Type> {field1: value1, field2: value2, ...}
            if (num_type == Type::BaseInt) {
                std::cout << "BaseInt @" << base_int_ptr << " {base_int: " << base_int_ptr->base_int << "}" << std::endl;
            } else if (num_type == Type::BaseFloat) {
                std::cout << "BaseFloat @" << base_float_ptr << " {base_float: " << base_float_ptr->base_float << "}" << std::endl;
            } else if (num_type == Type::NumberA) {
                std::cout << "NumberA @" << numbera_ptr << " {base_int: " << numbera_ptr->base_int << ", base_float: " << numbera_ptr->base_float << "}" << std::endl;
            } else if (num_type == Type::NumberB) {
                std::cout << "NumberB @" << numberb_ptr << " {base_float: " << numberb_ptr->base_float << ", base_int: " << numberb_ptr->base_int << "}" << std::endl;
            }
        } else if (command == "QUIT") {
            break;
        }
    }

    std::cout << "=== START ===" << std::endl;
    if (USER_ID == 0) {
        std::cout << "Congratulations! You have successfully modified the USER_ID!" << std::endl;
    }
    std::cout << "=== DONE ===" << std::endl;
    return 0;
}

