#include <iostream>
#include <vector>
#include <string>
#include <random>   

using namespace std;

// Represents a row and column in the grid
struct Position {
    int row, col;
};

// Forward declaration of Room class
class Room;

// ================
// Base class for all entities on the grid
// ================
class Entity {
protected:
    char _symbol;       // symbol shown on the grid
    int _health;        // health for movable entities
    Position _pos;      // current position in the room
public:
    virtual ~Entity() = default;
    // Show entity details
    virtual void displayEntity() const = 0;
    // Return true if entity can move
    virtual bool canMove() const = 0;
    // Move the entity according to its rules
    virtual void moveEntity(Room& room) = 0;
    // Get the symbol for display on the grid
    char getSymbol() const { return _symbol; }
    // Get the current position
    Position getPosition() const { return _pos; }
    // Set a new position
    void setPosition(const Position& p) { _pos = p; }
};

// ================
// Inanimate entities (they never move)
// ================
class Inanimate : public Entity {
public:
    // Always false because inanimate objects do not move
    bool canMove() const override { return false; }
    
    // Do nothing on move
    void moveEntity(Room&) override { }
};

// Stone is a simple inanimate object
class Stone : public Inanimate {
public:
    // Constructor sets the display symbol
    Stone() { _symbol = 'S'; }
    
    // Show stone properties
    void displayEntity() const override {
        cout << "Type: Stone, Symbol: S\n";
    }
};

// Hole is an inanimate object with a depth
class Hole : public Inanimate {
    int _depth;
public:
    // Constructor sets depth and symbol
    Hole(int depth) : _depth(depth) { _symbol = 'O'; }
    
    // Show hole properties including its depth
    void displayEntity() const override {
        cout << "Type: Hole, Symbol: O, Depth: " << _depth << "\n";
    }
};

// ================
// Animate entities (they can move if health > 0)
// ================
class Animate : public Entity {
public:
    // Can move only if health is above zero
    bool canMove() const override { return _health > 0; }
};

// Human moves one step right or random adjacent if blocked
class Human : public Animate {
    string _name;
public:
    // Constructor sets name, health and symbol
    Human(const string& name) : _name(name) {
        _health = 100;
        _symbol = '@';
    }

    // Show human details
    void displayEntity() const override {
        cout << "Entity properties:\n"
             << " Type: Human\n"
             << " Name: " << _name << "\n"
             << " Health: " << _health << "\n";
    }

    // Move human according to its simple rules
    void moveEntity(Room& room) override;
};

// Dragon moves to a random adjacent cell and loses random health
class Dragon : public Animate {
public:
    // Constructor sets health and symbol
    Dragon() {
        _health = 100;
        _symbol = '#';
    }

    // Show dragon details
    void displayEntity() const override {
        cout << "Entity properties:\n"
             << " Type: Dragon\n"
             << " Health: " << _health << "\n";
    }

    // Move dragon to a random adjacent cell
    void moveEntity(Room& room) override;
};

// Monster moves by its strength or adjacent step if blocked
class Monster : public Animate {
    int _strength;
public:
    // Constructor sets strength, health and symbol
    Monster(int strength) : _strength(strength) {
        _health = 100;
        _symbol = '*';
    }

    // Show monster details
    void displayEntity() const override {
        cout << "Entity properties:\n"
             << " Type: Monster\n"
             << " Strength: " << _strength << "\n"
             << " Health: " << _health << "\n";
    }

    // Move monster according to its strength and rules
    void moveEntity(Room& room) override;
};

// =============
// Room class manages the grid and entities
// =============
class Room {
    int _rows, _cols;
    vector<vector<Entity*>> _grid;      // grid of pointers to entities
    default_random_engine _rng;         // random number generator
public:
    // Constructor sets size, grid and seeds RNG
    Room(int rows = 10, int cols = 10)
      : _rows(rows)
      , _cols(cols)
      , _grid(rows, vector<Entity*>(cols, nullptr))
      , _rng(random_device{}())
    {}

    // Destructor deletes all entities to free memory
    ~Room() {
        for (int r = 0; r < _rows; ++r)
            for (int c = 0; c < _cols; ++c)
                delete _grid[r][c];
    }

    // Return true if position is inside grid and empty
    bool isEmpty(const Position& p) const {
        return p.row >= 0 && p.col >= 0
            && p.row < _rows && p.col < _cols
            && _grid[p.row][p.col] == nullptr;
    }

    // Set an entity at a position if it is empty
    void set(Entity* e, const Position& p) {
        if (!isEmpty(p)) return;
        e->setPosition(p);
        _grid[p.row][p.col] = e;
    }

    // Show the grid with symbols for entities
    void displayRoom() const {
        cout << "  ";
        for (int c = 0; c < _cols; ++c) cout << c;
        cout << "\n";
        for (int r = 0; r < _rows; ++r) {
            cout << r << ' ';
            for (int c = 0; c < _cols; ++c)
                cout << (_grid[r][c] ? _grid[r][c]->getSymbol() : '.');
            cout << "\n";
        }
    }

    // Find entities by their symbol and return a list
    vector<Entity*> getEntitiesBySymbol(char symbol) const {
        vector<Entity*> found;
        for (int r = 0; r < _rows; ++r)
            for (int c = 0; c < _cols; ++c)
                if (_grid[r][c] && _grid[r][c]->getSymbol() == symbol)
                    found.push_back(_grid[r][c]);
        return found;
    }

    // Move all entities that can move
    void moveAllEntities() {
        vector<Entity*> movers;
        for (int r = 0; r < _rows; ++r)
            for (int c = 0; c < _cols; ++c)
                if (_grid[r][c] && _grid[r][c]->canMove())
                    movers.push_back(_grid[r][c]);

        for (Entity* e : movers) {
            Position oldPos = e->getPosition();
            _grid[oldPos.row][oldPos.col] = nullptr;
            e->moveEntity(*this);
        }
    }

    // Return a random int between min and max inclusive
    int randInt(int min, int max) {
        int range = max - min + 1;
        return min + static_cast<int>(_rng() % range);
    }

    // Pick a random empty position on the grid
    Position pickEmptyPosition() {
        Position p;
        do {
            p.row = randInt(0, _rows - 1);
            p.col = randInt(0, _cols - 1);
        } while (!isEmpty(p));
        return p;
    }

    // Remove all entities and place new ones randomly
    void resetRoom() {
        // Delete old entities
        for (int r = 0; r < _rows; ++r)
            for (int c = 0; c < _cols; ++c) {
                delete _grid[r][c];
                _grid[r][c] = nullptr;
            }

        // Place stones
        for (int i = 0; i < 2; ++i)
            set(new Stone(), pickEmptyPosition());

        // Place holes with random depth
        for (int i = 0; i < 2; ++i)
            set(new Hole(randInt(0, 20)), pickEmptyPosition());

        // Place monsters with random strength
        for (int i = 0; i < 3; ++i)
            set(new Monster(randInt(1, 5)), pickEmptyPosition());

        // Place humans with fixed names
        const string humanNames[3] = { "Harold", "David", "Clare" };
        for (int i = 0; i < 3; ++i)
            set(new Human(humanNames[i]), pickEmptyPosition());

        // Place dragons
        for (int i = 0; i < 2; ++i)
            set(new Dragon(), pickEmptyPosition());
    }
};

// ===========
// Move human one step or random adjacent
// ===========
void Human::moveEntity(Room& room) {
    Position p = getPosition();
    // try one step to the right
    Position right{ p.row, p.col + 1 };
    if (room.isEmpty(right)) {
        _health--;  // lose one health
        room.set(this, right);
        return;
    }
    // collect adjacent empty positions
    Position adj[4];
    int cnt = 0;
    int dr[4] = { -1, 1, 0, 0 }, dc[4] = { 0, 0, -1, 1 };
    for (int i = 0; i < 4; ++i) {
        Position np{ p.row + dr[i], p.col + dc[i] };
        if (room.isEmpty(np)) adj[cnt++] = np;
    }
    // pick one at random if any
    if (cnt > 0) {
        int idx = room.randInt(0, cnt - 1);
        _health--;
        room.set(this, adj[idx]);
    } else {
        // stay in place if no moves
        room.set(this, p);
    }
}

// ===========
// Move dragon to a random adjacent cell
// ===========
void Dragon::moveEntity(Room& room) {
    Position p = getPosition();
    Position adj[4];
    int cnt = 0;
    int dr[4] = { -1, 1, 0, 0 }, dc[4] = { 0, 0, -1, 1 };
    for (int i = 0; i < 4; ++i) {
        Position np{ p.row + dr[i], p.col + dc[i] };
        if (room.isEmpty(np)) adj[cnt++] = np;
    }
    if (cnt > 0) {
        int idx = room.randInt(0, cnt - 1);
        _health -= room.randInt(0, 5);  // lose random health 0-5
        room.set(this, adj[idx]);
    } else {
        room.set(this, p);  // stay if blocked
    }
}

// ===========
// Move monster by its strength or one step if blocked
// ===========
void Monster::moveEntity(Room& room) {
    Position p = getPosition();
    bool moved = false;
    // try big step equal to strength
    int drBig[4] = { -_strength, _strength, 0, 0 };
    int dcBig[4] = { 0, 0, -_strength, _strength };
    for (int i = 0; i < 4 && !moved; ++i) {
        Position np{ p.row + drBig[i], p.col + dcBig[i] };
        if (room.isEmpty(np)) {
            _health -= _strength;  // lose health equal to strength
            room.set(this, np);
            moved = true;
        }
    }
    if (!moved) {
        // try single step if big step fails
        Position adj[4];
        int cnt = 0;
        int dr[4] = { -1, 1, 0, 0 }, dc[4] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            Position np{ p.row + dr[i], p.col + dc[i] };
            if (room.isEmpty(np)) adj[cnt++] = np;
        }
        if (cnt > 0) {
            int idx = room.randInt(0, cnt - 1);
            _health -= _strength;
            room.set(this, adj[idx]);
        } else {
            room.set(this, p);  // stay if no moves
        }
    }
}

// ===========
// Main function: menu loop and actions
// ===========
int main() {
    Room room;           // create a room object
    room.resetRoom();        // fill it with entities
    cout << "\nInitial room after Reset:\n";
    room.displayRoom();      // display the grid

    const char symbols[] = {'S','O','@','#','*'};
    const string names[] = {"Stone","Hole","Human","Dragon","Monster"};
    const int typeCount = 5;
    // this while loop print the menu and take the users input
    while (true) {
        cout << "\nMenu:\n"
             << "1. Display Room\n"
             << "2. Move All Animated Entities\n"
             << "3. Display Entity Info by Type\n"
             << "4. Reset Room\n"
             << "5. Exit\n"
             << "Enter choice: ";
        int choice;
        if (!(cin >> choice)) break;  // exit on bad input
        // this swich condition checks the users input and calls the corresponding function
        switch (choice) {
            case 1:
                room.displayRoom();     // show grid
                break;
            case 2:
                room.moveAllEntities();     // move all entities
                cout << "All movable entities have moved.\n";
                break;
            case 3: {
                cout << "Select entity type:\n";
                for (int i = 0; i < typeCount; ++i)
                    cout << i << ". " << names[i] << "\n";
                int ti; cin >> ti;
                if (ti < 0 || ti >= typeCount) {
                    cout << "Invalid type.\n";
                    break;
                }
                char sym = symbols[ti];
                auto list = room.getEntitiesBySymbol(sym);
                cout << "Found " << list.size() << " " << names[ti] << "(s):\n";
                for (int i = 0; i < (int)list.size(); ++i) {
                    Position p = list[i]->getPosition();
                    cout << i << ". at (" << p.row << "," << p.col << ")\n";
                }
                if (list.empty()) break;
                cout << "Pick index: ";
                int idx; cin >> idx;
                if (idx < 0 || idx >= (int)list.size()) {
                    cout << "Invalid index.\n";
                    break;
                }
                list[idx]->displayEntity();  // show chosen entity info
                break;
            }
            case 4:
                room.resetRoom();
                cout << "Room has been reset. New layout:\n";
                room.displayRoom();
                break;
            case 5:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}
