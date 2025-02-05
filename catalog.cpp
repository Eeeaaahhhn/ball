#include "catalog.hpp"
#include <iostream>
#include <fstream>

/**
 * Function: create_spells
 * Description: Allocates a dynamic array of spells of the requested size and
 *              returns a pointer that points to it.
 * Parameters:
 *      size (int): Size of dynamic array of spells to create.
 * Returns: Pointer that points to dynamically allocated array of spells.
 */
spell* create_spells(int size) {
    return new spell[size];
}

// Function to read a single spell's data from a file
spell read_spell_data(std::ifstream& file) {
    spell new_spell;
    file >> new_spell.name >> new_spell.success_rate >> new_spell.effect;
    return new_spell;
}

// Function to create an array of spellbooks
spellbook* create_spellbooks(int size) {
    return new spellbook[size];
}

// Function to read a spellbook's data from a file
spellbook read_spellbook_data(std::ifstream& file) {
    spellbook sb;
    file >> sb.title >> sb.author >> sb.num_pages >> sb.edition >> sb.num_spells;
    sb.spells = create_spells(sb.num_spells);

    float total_success_rate = 0;
    for (int i = 0; i < sb.num_spells; ++i) {
        sb.spells[i] = read_spell_data(file);
        total_success_rate += sb.spells[i].success_rate;
    }
    sb.avg_success_rate = total_success_rate / sb.num_spells;
    return sb;
}

// Function to delete an array of spells
void delete_spells(spell*& spells) {
    delete[] spells;
    spells = nullptr;
}

// Function to delete an array of spellbooks and their spells
void delete_spellbooks(spellbook*& spellbooks, int size) {
    for (int i = 0; i < size; ++i) {
        delete_spells(spellbooks[i].spells);
    }
    delete[] spellbooks;
    spellbooks = nullptr;
}

// Function to read wizard data from a file
bool read_wizards(const std::string& filename, wizard*& wizards, int& num_wizards) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open wizard file." << std::endl;
        return false;
    }
    file >> num_wizards;
    wizards = new wizard[num_wizards];
    for (int i = 0; i < num_wizards; ++i) {
        file >> wizards[i].name >> wizards[i].id >> wizards[i].password >> wizards[i].position_title >> wizards[i].beard_length;
    }
    return true;
}


// Function to handle wizard login
void login(const wizard* wizards, int num_wizards) {
    int attempts = 0;
    while (attempts < 3) {
        int id;
        std::string password;
        std::cout << "Please enter your ID: ";
        std::cin >> id;
        std::cout << "Please enter your password: ";
        std::cin >> password;

        for (int i = 0; i < num_wizards; ++i) {
            if (wizards[i].id == id && wizards[i].password == password) {
                std::cout << "Welcome " << wizards[i].name << "\n";
                std::cout << "ID: " << wizards[i].id << "\n";
                std::cout << "Status: " << wizards[i].position_title << "\n";
                std::cout << "Beard Length: " << wizards[i].beard_length << "\n";
                return;
            }
        }
        std::cout << "Incorrect ID or password.\n";
        attempts++;
    }
    std::cout << "Maximum attempts exceeded. Exiting.\n";
    exit(1);
}

//nction to display all spellbooks for a logged-in wizard
void display_all_spellbooks(const spellbook* spellbooks, int num_spellbooks, const wizard& logged_in_wizard) {
    std::ofstream ofs("output.txt", std::ios::app);
    for (int i = 0; i < num_spellbooks; ++i) {
        std::cout << "Spellbook Title: " << spellbooks[i].title << "\n";
        ofs << "Spellbook Title: " << spellbooks[i].title << "\n";
        std::cout << "Author: " << spellbooks[i].author << "\n";
        ofs << "Author: " << spellbooks[i].author << "\n";
        std::cout << "Number of Pages: " << spellbooks[i].num_pages << "\n";
        ofs << "Number of Pages: " << spellbooks[i].num_pages << "\n";
        std::cout << "Edition: " << spellbooks[i].edition << "\n";
        ofs << "Edition: " << spellbooks[i].edition << "\n";
        std::cout << "Average Success Rate: " << spellbooks[i].avg_success_rate << "\n";
        ofs << "Average Success Rate: " << spellbooks[i].avg_success_rate << "\n";
        std::cout << "Spells:\n";
        ofs << "Spells:\n";

        for (int j = 0; j < spellbooks[i].num_spells; ++j) {
            const spell& s = spellbooks[i].spells[j];
            if (logged_in_wizard.position_title == "Student" && (s.effect == "poison" || s.effect == "death")) {
                continue; // Hide certain spells from students
            }
            std::cout << " - " << s.name << " (Effect: " << s.effect << ", Success Rate: " << s.success_rate << ")\n";
            ofs << " - " << s.name << " (Effect: " << s.effect << ", Success Rate: " << s.success_rate << ")\n";
        }
        std::cout << "\n";
        ofs << "\n";
    }
    ofs.close();
}
// Function to search for a spellbook by name
void search_spellbook_by_name(const spellbook* spellbooks, int num_spellbooks) {
    std::string name;
    std::cout << "Enter the spellbook name: ";
    std::cin >> name;

    for (int i = 0; i < num_spellbooks; ++i) {
        if (spellbooks[i].title == name) {
            std::cout << "Spellbook found:\n";
            std::ofstream ofs("output.txt", std::ios::app);
            write_spellbook_info(spellbooks[i], ofs);
            ofs.close();
            std::cout << "Information appended to file.\n";
            return;
        }
    }
    std::cout << "Spellbook not found.\n";
}
// Function to search for spells by effect
void search_spells_by_effect(const spellbook* spellbooks, int num_spellbooks, const wizard& logged_in_wizard) {
    std::string effect;
    std::cout << "Enter the spell effect (fire, bubble, memory_loss, death, poison): ";
    std::cin >> effect;

    if (!is_valid_effect(effect) || (logged_in_wizard.position_title == "Student" && (effect == "death" || effect == "poison"))) {
        std::cout << "Invalid effect or access denied.\n";
        return;
    }

    std::ofstream ofs("output.txt", std::ios::app);
    for (int i = 0; i < num_spellbooks; ++i) {
        for (int j = 0; j < spellbooks[i].num_spells; ++j) {
            const spell& s = spellbooks[i].spells[j];
            if (s.effect == effect) {
                std::cout << " - " << s.name << " (Effect: " << s.effect << ", Success Rate: " << s.success_rate << ")\n";
                ofs << " - " << s.name << " (Effect: " << s.effect << ", Success Rate: " << s.success_rate << ")\n";
            }
        }
    }
    ofs.close();
}
// Function to write spellbook info to an output file
void write_spellbook_info(const spellbook& sb, std::ofstream& ofs) {
    ofs << "Spellbook Title: " << sb.title << "\n";
    ofs << "Author: " << sb.author << "\n";
    ofs << "Number of Pages: " << sb.num_pages << "\n";
    ofs << "Edition: " << sb.edition << "\n";
    ofs << "Average Success Rate: " << sb.avg_success_rate << "\n";
    ofs << "Spells:\n";

    for (int j = 0; j < sb.num_spells; ++j) {
        const spell& s = sb.spells[j];
        ofs << " - " << s.name << " (Effect: " << s.effect << ", Success Rate: " << s.success_rate << ")\n";
    }
}

// Function to validate if the spell effect is recognized
bool is_valid_effect(const std::string& effect) {
    return effect == "fire" || effect == "bubble" || effect == "memory_loss" || effect == "death" || effect == "poison";
}

