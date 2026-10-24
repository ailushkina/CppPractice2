#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <limits>
#include <cstring>
#include <cstdlib>
#include <windows.h>
using namespace std;

enum HeroClass { Warrior, Mage, Healer };
enum ItemType { Weapon, Armor, Potion };
enum StatusEffect { None, Blessed, Cursed, Poisoned };

struct StatusFlags {
    unsigned int blessed : 1;
    unsigned int cursed : 1;
    unsigned int poisoned : 1;
};

struct WeaponStats { int damage; int durability; bool twoHanded; };
struct ArmorStats { int defense; int durability; bool magical; };
struct PotionStats { int healAmount; int manaAmount; };

union ItemProperties {
    WeaponStats weapon;
    ArmorStats  armor;
    PotionStats potion;
};

struct Item {
    string name;
    ItemType type;
    int weight;
    int value;
    ItemProperties props;
};

struct Hero {
    string name;
    HeroClass heroClass;
    int level;
    int hp, maxHp;
    int mp, maxMp;
    StatusFlags status;
    Item* equipment;
    int equippedCount;
};

struct Enemy {
    string name;
    int hp;
    int damage;
};

string getItemTypeName(ItemType type) {
    switch (type) {
    case Weapon: return "Weapon";
    case Armor:  return "Armor";
    case Potion: return "Potion";
    }
    return "Unknown";
}

int main() {
    setlocale(LC_ALL, "ru");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    auto getInt = [](const string& prompt, int minVal = -2147483647, int maxVal = 2147483647) {
        int val;
        while (true) {
            cout << prompt;
            cin >> val;
            if (cin.fail()) {
                cin.clear();
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                cout << "Ошибка! Введите число.\n";
            }
            else if (val < minVal || val > maxVal) {
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                cout << "Ошибка! Число должно быть от " << minVal << " до " << maxVal << ".\n";
            }
            else {
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                return val;
            }
        }
        };

    auto getString = [](const string& prompt) {
        string s;
        while (true) {
            cout << prompt;
            getline(cin, s);
            if (s.empty()) {
                cout << "Ошибка! Строка не может быть пустой.\n";
            }
            else {
                return s;
            }
        }
        };

    auto printStatus = [](const StatusFlags& s) {
        bool any = false;
        if (s.blessed) { cout << "Blessed ";  any = true; }
        if (s.cursed) { cout << "Cursed ";   any = true; }
        if (s.poisoned) { cout << "Poisoned "; any = true; }
        if (!any) cout << "None";
        };

    auto printItem = [](const Item& it) {
        if (it.name.empty()) { cout << "[пусто]"; return; }
        cout << it.name << " (" << getItemTypeName(it.type) << ", вес: " << it.weight << ", цена: " << it.value << ")";
        switch (it.type) {
        case Weapon:
            cout << " | урон: " << it.props.weapon.damage
                << " | прочн: " << it.props.weapon.durability
                << " | двуруч: " << (it.props.weapon.twoHanded ? "да" : "нет");
            break;
        case Armor:
            cout << " | защ: " << it.props.armor.defense
                << " | прочн: " << it.props.armor.durability
                << " | магич: " << (it.props.armor.magical ? "да" : "нет");
            break;
        case Potion:
            cout << " | лечHP: " << it.props.potion.healAmount
                << " | воспMP: " << it.props.potion.manaAmount;
            break;
        }
        };

    array<Item, 16> inventory;
    for (auto& it : inventory) { it.name = ""; it.type = Weapon; it.weight = 0; it.value = 0; }

    vector<Hero> squad;
    Enemy* enemies = nullptr;
    int enemyCount = 0;

    int choice = -1;
    while (choice != 0) {
        system("cls");
        cout << "\n=== СИСТЕМА УПРАВЛЕНИЯ ОТРЯДОМ ===\n"
            << "1. Управление отрядом (герои)\n"
            << "2. Управление инвентарём (предметы)\n"
            << "3. Экипировка героев\n"
            << "4. Управление врагами\n"
            << "5. Симуляция битвы\n"
            << "6. Показать сводку по отряду\n"
            << "0. Выход\n";
        choice = getInt("Выбор: ", 0, 6);

        switch (choice) {
        case 1: {
            int sub = -1;
            while (sub != 0) {
                system("cls");
                cout << "\n--- Управление отрядом ---\n1. Добавить героя\n2. Удалить героя\n3. Показать всех героев\n0. Назад\n";
                sub = getInt("Выбор: ", 0, 3);

                if (sub == 1) {
                    system("cls");
                    Hero h;
                    h.name = getString("Имя: ");
                    int cls = getInt("Класс (1-Warrior 2-Mage 3-Healer): ", 1, 3);
                    int lvl = getInt("Уровень (от 1 до 100): ", 1, 100);
                    int eff = getInt("Статус (0-None 1-Blessed 2-Cursed 3-Poisoned): ", 0, 3);

                    h.heroClass = (cls == 1) ? Warrior : (cls == 2) ? Mage : Healer;
                    h.level = lvl;
                    switch (h.heroClass) {
                    case Warrior: h.maxHp = 100 + lvl * 20; h.maxMp = 10 + lvl * 2;  break;
                    case Mage:    h.maxHp = 50 + lvl * 8;  h.maxMp = 100 + lvl * 25; break;
                    case Healer:  h.maxHp = 60 + lvl * 10; h.maxMp = 80 + lvl * 20; break;
                    }
                    h.hp = h.maxHp;
                    h.mp = h.maxMp;
                    h.status.blessed = h.status.cursed = h.status.poisoned = 0;
                    switch (eff) {
                    case 1: h.status.blessed = 1; break;
                    case 2: h.status.cursed = 1; break;
                    case 3: h.status.poisoned = 1; break;
                    }

                    h.equipment = new Item[4];
                    for (int i = 0; i < 4; i++) h.equipment[i].name = "";
                    h.equippedCount = 0;
                    squad.push_back(h);
                    cout << "Герой добавлен.\n";
                    system("pause");
                }
                else if (sub == 2) {
                    system("cls");
                    if (squad.empty()) { cout << "Нет героев.\n"; system("pause"); continue; }
                    int idx = getInt("Индекс: ", 0, (int)squad.size() - 1);
                    delete[] squad[idx].equipment;
                    squad.erase(squad.begin() + idx);
                    cout << "Герой удален.\n";
                    system("pause");
                }
                else if (sub == 3) {
                    system("cls");
                    if (squad.empty()) { cout << "Нет героев.\n"; system("pause"); continue; }
                    for (int i = 0; i < (int)squad.size(); i++) {
                        cout << i << ". " << squad[i].name
                            << " | " << (squad[i].heroClass == Warrior ? "Warrior" : squad[i].heroClass == Mage ? "Mage" : "Healer")
                            << " | Уровень " << squad[i].level
                            << " | HP " << squad[i].hp << "/" << squad[i].maxHp
                            << " | MP " << squad[i].mp << "/" << squad[i].maxMp
                            << " | Статус: ";
                        printStatus(squad[i].status);
                        cout << "\n";
                    }
                    system("pause");
                }
            }
        } break;

        case 2: {
            int sub = -1;
            while (sub != 0) {
                system("cls");
                cout << "\n--- Управление инвентарём ---\n1. Добавить предмет\n2. Удалить предмет\n3. Показать инвентарь\n0. Назад\n";
                sub = getInt("Выбор: ", 0, 3);

                if (sub == 1) {
                    system("cls");
                    int slot = -1;
                    for (int i = 0; i < 16; i++) if (inventory[i].name.empty()) { slot = i; break; }
                    if (slot == -1) { cout << "Инвентарь полон.\n"; system("pause"); continue; }

                    Item it;
                    it.name = getString("Имя: ");
                    int t = getInt("Тип (1-Weapon 2-Armor 3-Potion): ", 1, 3);
                    it.type = (t == 1) ? Weapon : (t == 2) ? Armor : Potion;

                    it.weight = getInt("Вес (0-100): ", 0, 100);
                    it.value = getInt("Стоимость (0-10000): ", 0, 10000);

                    switch (it.type) {
                    case Weapon:
                        it.props.weapon.damage = getInt("Урон (0-100): ", 0, 100);
                        it.props.weapon.durability = getInt("Прочность (0-1000): ", 0, 1000);
                        it.props.weapon.twoHanded = getInt("Двуручное (0-нет, 1-да): ", 0, 1);
                        break;
                    case Armor:
                        it.props.armor.defense = getInt("Защита (0-100): ", 0, 100);
                        it.props.armor.durability = getInt("Прочность (0-1000): ", 0, 1000);
                        it.props.armor.magical = getInt("Магическое (0-нет, 1-да): ", 0, 1);
                        break;
                    case Potion:
                        it.props.potion.healAmount = getInt("Лечение HP (0-500): ", 0, 500);
                        it.props.potion.manaAmount = getInt("Восстановление MP (0-500): ", 0, 500);
                        break;
                    }
                    inventory[slot] = it;
                    cout << "Добавлено в слот " << slot << "\n";
                    system("pause");
                }
                else if (sub == 2) {
                    system("cls");
                    int idx = getInt("Индекс слота (0-15): ", 0, 15);
                    inventory[idx].name = "";
                    cout << "Предмет удален.\n";
                    system("pause");
                }
                else if (sub == 3) {
                    system("cls");
                    for (int i = 0; i < 16; i++) {
                        cout << i << ". ";
                        printItem(inventory[i]);
                        cout << "\n";
                    }
                    system("pause");
                }
            }
        } break;

        case 3: {
            int sub = -1;
            while (sub != 0) {
                system("cls");
                cout << "\n--- Экипировка ---\n1. Надеть предмет\n2. Снять предмет\n3. Показать экипировку героя\n0. Назад\n";
                sub = getInt("Выбор: ", 0, 3);

                if (sub == 1) {
                    system("cls");
                    if (squad.empty()) { cout << "Нет героев.\n"; system("pause"); continue; }
                    int hi = getInt("Индекс героя: ", 0, (int)squad.size() - 1);
                    Hero& h = squad[hi];
                    if (h.equippedCount >= 4) { cout << "Все слоты экипировки заняты.\n"; system("pause"); continue; }
                    int ii = getInt("Индекс слота инвентаря (0-15): ", 0, 15);
                    if (inventory[ii].name.empty()) { cout << "Ошибка: этот слот пуст.\n"; system("pause"); continue; }
                    h.equipment[h.equippedCount++] = inventory[ii];
                    inventory[ii].name = "";
                    cout << "Предмет надет.\n";
                    system("pause");
                }
                else if (sub == 2) {
                    system("cls");
                    if (squad.empty()) { cout << "Нет героев.\n"; system("pause"); continue; }
                    int hi = getInt("Индекс героя: ", 0, (int)squad.size() - 1);
                    Hero& h = squad[hi];
                    if (h.equippedCount == 0) { cout << "Нет надетых предметов.\n"; system("pause"); continue; }
                    int slot = getInt("Номер слота экипировки: ", 0, h.equippedCount - 1);

                    int invSlot = -1;
                    for (int i = 0; i < 16; i++) if (inventory[i].name.empty()) { invSlot = i; break; }
                    if (invSlot == -1) { cout << "Инвентарь полон.\n"; system("pause"); continue; }
                    inventory[invSlot] = h.equipment[slot];
                    for (int i = slot; i < h.equippedCount - 1; i++)
                        h.equipment[i] = h.equipment[i + 1];
                    h.equippedCount--;
                    h.equipment[h.equippedCount].name = "";
                    cout << "Предмет снят.\n";
                    system("pause");
                }
                else if (sub == 3) {
                    system("cls");
                    if (squad.empty()) { cout << "Нет героев.\n"; system("pause"); continue; }
                    int hi = getInt("Индекс героя: ", 0, (int)squad.size() - 1);
                    for (int i = 0; i < squad[hi].equippedCount; i++) {
                        cout << "  " << i << ". ";
                        printItem(squad[hi].equipment[i]);
                        cout << "\n";
                    }
                    system("pause");
                }
            }
        } break;

        case 4: {
            int sub = -1;
            while (sub != 0) {
                system("cls");
                cout << "\n--- Управление врагами ---\n1. Добавить врага\n2. Удалить врага\n3. Показать всех врагов\n0. Назад\n";
                sub = getInt("Выбор: ", 0, 3);

                if (sub == 1) {
                    system("cls");
                    string tempName = getString("Имя: ");
                    char nameBuffer[100] = { 0 };
                    strncpy(nameBuffer, tempName.c_str(), sizeof(nameBuffer) - 1);

                    int hp = getInt("HP (1-10000): ", 1, 10000);
                    int dmg = getInt("Урон (1-1000): ", 1, 1000);

                    Enemy* newArr = new Enemy[enemyCount + 1];
                    for (int i = 0; i < enemyCount; i++) newArr[i] = enemies[i];
                    newArr[enemyCount].name = string(nameBuffer);
                    newArr[enemyCount].hp = hp;
                    newArr[enemyCount].damage = dmg;
                    delete[] enemies;
                    enemies = newArr;
                    enemyCount++;
                    cout << "Враг добавлен.\n";
                    system("pause");
                }
                else if (sub == 2) {
                    system("cls");
                    if (enemyCount == 0) { cout << "Нет врагов.\n"; system("pause"); continue; }
                    int idx = getInt("Индекс: ", 0, enemyCount - 1);
                    Enemy* newArr = new Enemy[enemyCount - 1];
                    for (int i = 0, j = 0; i < enemyCount; i++) {
                        if (i == idx) continue;
                        newArr[j++] = enemies[i];
                    }
                    delete[] enemies;
                    enemies = newArr;
                    enemyCount--;
                    cout << "Враг удален.\n";
                    system("pause");
                }
                else if (sub == 3) {
                    system("cls");
                    for (int i = 0; i < enemyCount; i++)
                        cout << i << ". " << enemies[i].name << " | HP: " << enemies[i].hp << " | Урон: " << enemies[i].damage << "\n";
                    system("pause");
                }
            }
        } break;

        case 5: {
            system("cls");
            if (squad.empty() || enemyCount == 0) {
                cout << "Нужен хотя бы один герой и один враг!\n";
                system("pause");
                continue;
            }

            vector<Hero> bs;
            for (const auto& h : squad) {
                Hero c = h;
                c.equipment = new Item[4];
                for (int i = 0; i < 4; i++) c.equipment[i] = h.equipment[i];
                bs.push_back(c);
            }
            Enemy* be = new Enemy[enemyCount];
            for (int i = 0; i < enemyCount; i++) be[i] = enemies[i];
            int beCount = enemyCount;

            int turn = 1;
            bool heroesAlive = true, enemiesAlive = true;

            while (heroesAlive && enemiesAlive) {
                cout << "\n--- Ход " << turn << " ---\n";

                for (auto& h : bs) {
                    if (h.hp <= 0) continue;
                    if (h.status.poisoned) {
                        h.hp -= 3;
                        cout << h.name << " получает 3 урона от яда.\n";
                        if (h.hp < 0) h.hp = 0;
                    }
                }
                heroesAlive = false;
                for (const auto& h : bs) if (h.hp > 0) heroesAlive = true;
                if (!heroesAlive) break;

                for (int i = 0; i < (int)bs.size(); i++) {
                    if (bs[i].hp <= 0) continue;
                    Hero& h = bs[i];
                    cout << "\nХод героя: " << h.name << " ("
                        << (h.heroClass == Warrior ? "Warrior" : h.heroClass == Mage ? "Mage" : "Healer")
                        << ") | HP " << h.hp << "/" << h.maxHp << " | MP " << h.mp << "/" << h.maxMp << " [";
                    printStatus(h.status);
                    cout << "]\n";

                    if (h.equippedCount >= 4 && !h.equipment[3].name.empty() && h.equipment[3].type == Potion) {
                        int d = getInt("В 4-м слоте есть зелье. Выпить? (1 - да, 0 - нет): ", 0, 1);
                        if (d == 1) {
                            int heal = h.equipment[3].props.potion.healAmount;
                            int mana = h.equipment[3].props.potion.manaAmount;
                            h.hp += heal; if (h.hp > h.maxHp) h.hp = h.maxHp;
                            h.mp += mana; if (h.mp > h.maxMp) h.mp = h.maxMp;
                            cout << "Восстановлено " << heal << " HP, " << mana << " MP.\n";
                            h.equipment[3].name = "";
                            h.equippedCount--;
                        }
                    }

                    int weaponDmg = 0;
                    for (int k = 0; k < h.equippedCount; k++) {
                        if (!h.equipment[k].name.empty() && h.equipment[k].type == Weapon) {
                            weaponDmg = h.equipment[k].props.weapon.damage;
                            break;
                        }
                    }

                    if (h.heroClass == Healer) {
                        bool useAbility = false;
                        if (h.mp >= 40) {
                            useAbility = getInt("Использовать способность «Лечение»? (1 - да, 0 - нет): ", 0, 1);
                        }
                        if (useAbility) {
                            h.mp -= 40;
                            int ai = getInt("Индекс союзника: ", 0, (int)bs.size() - 1);
                            if (bs[ai].hp <= 0) { cout << "Ошибка: неверная цель (мертв).\n"; continue; }
                            int heal = 6 + weaponDmg;
                            bs[ai].hp += heal;
                            if (bs[ai].hp > bs[ai].maxHp) bs[ai].hp = bs[ai].maxHp;
                            cout << "Вылечил " << bs[ai].name << " на " << heal << " HP.\n";
                        }
                        else {
                            int ti = getInt("Индекс врага: ", 0, beCount - 1);
                            if (be[ti].hp <= 0) { cout << "Ошибка: неверная цель (мертв).\n"; continue; }
                            int dmg = 6 + weaponDmg;
                            if (h.status.blessed) dmg *= 2;
                            if (h.status.cursed)  dmg = (dmg + 1) / 2;
                            be[ti].hp -= dmg;
                            cout << "Нанесено " << dmg << " урона цели " << be[ti].name << "\n";
                        }
                    }
                    else {
                        int ti = getInt("Индекс врага: ", 0, beCount - 1);
                        if (be[ti].hp <= 0) { cout << "Ошибка: неверная цель (мертв).\n"; continue; }
                        int base = (h.heroClass == Warrior) ? 15 : 8;
                        int cost = (h.heroClass == Warrior) ? 20 : 40;
                        double mult = (h.heroClass == Warrior) ? 1.5 : 3.0;
                        bool useAbility = false;
                        if (h.mp >= cost) {
                            useAbility = getInt("Использовать способность? (1 - да, 0 - нет): ", 0, 1);
                        }
                        int dmg;
                        if (useAbility) {
                            h.mp -= cost;
                            dmg = (int)(base * mult);
                            cout << "Способность использована!\n";
                        }
                        else {
                            dmg = base + weaponDmg;
                        }
                        if (h.status.blessed) dmg *= 2;
                        if (h.status.cursed)  dmg = (dmg + 1) / 2;
                        be[ti].hp -= dmg;
                        cout << "Нанесено " << dmg << " урона цели " << be[ti].name << "\n";
                    }
                }

                enemiesAlive = false;
                for (int i = 0; i < beCount; i++) if (be[i].hp > 0) enemiesAlive = true;
                if (!enemiesAlive) break;

                for (int i = 0; i < beCount; i++) {
                    if (be[i].hp <= 0) continue;
                    int hi = getInt("\n" + be[i].name + " атакует. Индекс героя: ", 0, (int)bs.size() - 1);
                    if (bs[hi].hp <= 0) { cout << "Ошибка: неверная цель (мертв).\n"; continue; }

                    int aDef = 0;
                    for (int k = 0; k < bs[hi].equippedCount; k++) {
                        if (!bs[hi].equipment[k].name.empty() && bs[hi].equipment[k].type == Armor) {
                            aDef += bs[hi].equipment[k].props.armor.defense;
                        }
                    }
                    int dmg = be[i].damage - aDef;
                    if (dmg < 1) dmg = 1;
                    bs[hi].hp -= dmg;
                    if (bs[hi].hp < 0) bs[hi].hp = 0;
                    cout << be[i].name << " попадает по " << bs[hi].name << " на " << dmg << " урона.\n";
                }

                cout << "\n--- Состояние ---\n";
                for (const auto& h : bs) {
                    cout << h.name << ": " << (h.hp > 0 ? "жив" : "мёртв")
                        << " | HP " << h.hp << " | MP " << h.mp << " [";
                    printStatus(h.status);
                    cout << "]\n";
                }
                for (int i = 0; i < beCount; i++) {
                    cout << be[i].name << ": " << (be[i].hp > 0 ? "жив" : "мёртв")
                        << " | HP " << be[i].hp << "\n";
                }

                turn++;
            }

            cout << "\n=== РЕЗУЛЬТАТ ===\n";
            if (!enemiesAlive) cout << "ПОБЕДА ГЕРОЕВ!\n";
            else cout << "ПОРАЖЕНИЕ... Враги победили.\n";

            for (auto& h : bs) delete[] h.equipment;
            delete[] be;

            system("pause");
        } break;

        case 6: {
            system("cls");
            cout << "\n=== СВОДКА ПО ОТРЯДУ ===\n";
            int totalLevel = 0, totalHp = 0, totalMp = 0;
            for (auto& h : squad) {
                totalLevel += h.level;
                totalHp += h.hp;
                totalMp += h.mp;
            }
            cout << "Количество героев: " << squad.size() << "\n";
            cout << "Суммарный уровень: " << totalLevel << "\n";
            cout << "Суммарное HP: " << totalHp << "\n";
            cout << "Суммарное MP: " << totalMp << "\n";
            for (auto& h : squad) {
                cout << h.name << " (" << (h.heroClass == Warrior ? "Warrior" : h.heroClass == Mage ? "Mage" : "Healer") << "): ";
                printStatus(h.status);
                cout << "\n";
                for (int i = 0; i < h.equippedCount; i++) {
                    cout << "  "; printItem(h.equipment[i]); cout << "\n";
                }
            }
            int used = 0, val = 0, wt = 0;
            for (auto& it : inventory) {
                if (!it.name.empty()) { used++; val += it.value; wt += it.weight; }
            }
            cout << "Состояние инвентаря: занято " << used << "/16, суммарная стоимость: " << val << ", суммарный вес: " << wt << "\n";
            cout << "Список врагов:\n";
            for (int i = 0; i < enemyCount; i++)
                cout << "  " << enemies[i].name << " | HP: " << enemies[i].hp << " | Урон: " << enemies[i].damage << "\n";

            system("pause");
        } break;

        case 0:
            break;
        }
    }

    for (auto& h : squad) delete[] h.equipment;
    delete[] enemies;
    return 0;
}