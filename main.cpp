#include <iostream>

int main() {
    while (true) {
        // 1. Print the menu
        std::cout << "1: Print help" << std::endl;
        std::cout << "2: Print exchange stats" << std::endl;
        std::cout << "3: Place an ask" << std::endl;
        std::cout << "4: Place a bid" << std::endl;
        std::cout << "5: Print wallet" << std::endl;
        std::cout << "6: Continue" << std::endl;

        // 2. Read user input
        int userOption;
        std::cout << "Type in 1-6" << std::endl;
        std::cin >> userOption;

        // 3. Print out which choice they made
        std::cout << "You chose: " << userOption << std::endl;

        // 4. Do something based on the user input using a switch statement
        switch (userOption) {
            case 1:
                std::cout << "Help - choose options from the menu" << std::endl;
                std::cout << "and follow the on screen instructions." << std::endl;
                break;
            case 2:
                std::cout << "You selected: Print exchange stats" << std::endl;
                break;
            case 3:
                std::cout << "You selected: Place an ask" << std::endl;
                break;
            case 4:
                std::cout << "You selected: Place a bid" << std::endl;
                break;
            case 5:
                std::cout << "You selected: Print wallet" << std::endl;
                break;
            case 6:
                std::cout << "You selected: Continue" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
                break;
        }
    }

    return 0;
}
