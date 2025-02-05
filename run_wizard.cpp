#include "catalog.hpp"

int main() {
    std::string wizard_filename, spellbook_filename;
    std::cout << "Enter the wizard info file name: ";
    std::cin >> wizard_filename;
    std::cout << "Enter the spellbook info file name: ";
    std::cin >> spellbook_filename;

    wizard* wizards = nullptr;
    int num_wizards = 0;
    if (!read_wizards(wizard_filename, wizards, num_wizards)) {
        std::cerr << "Error reading wizard file. Exiting program." << std::endl;
        return 1;
    }

    login(wizards, num_wizards);

    std::ifstream spellbookStream(spellbook_filename);
    if (!spellbookStream) {
        std::cerr << "Error opening spellbook file!" << std::endl;
        delete[] wizards;
        return 1;
    }

    int num_spellbooks;
    spellbookStream >> num_spellbooks;
    spellbook* spellbooks = create_spellbooks(num_spellbooks);

    for (int i = 0; i < num_spellbooks; ++i) {
        spellbooks[i] = read_spellbook_data(spellbookStream);
    }
    spellbookStream.close();

    int choice;
    do {
        std::cout << "1. Display all spellbooks\n";
        std::cout << "2. Search for a spellbook by name\n";
        std::cout << "3. Search for spells by effect\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                display_all_spellbooks(spellbooks, num_spellbooks, wizards[0]);
                break;
            case 2:
                search_spellbook_by_name(spellbooks, num_spellbooks);
                break;
            case 3:
                search_spells_by_effect(spellbooks, num_spellbooks, wizards[0]);
                break;
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice, Try again \n";
        }
    } while (choice != 0);

    delete_spellbooks(spellbooks, num_spellbooks);
    delete[] wizards;

    return 0;
}