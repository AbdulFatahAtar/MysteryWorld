#include <iostream>
#include <vector>
#include <string>

using namespace std;


// entity is an abstract class because
// it contain(s) pure virtual methods.

class entity
{
    protected:
    char _symbol;
    public:
    virtual void Display() = 0;
    virtual bool CanMove() = 0;
    virtual char GetSymbol()
    {
        return _symbol;
    }
};

class animate : public entity
{
    protected:
    int _health;
    public:
    virtual bool CanMove() { return _health > 0; }
};

class human: public animate
{
    protected:
    string _name;
    public:
    human(string name, int health)
    {
        _name = name;
        _health = health;
        _symbol = '@';
    }
    
    virtual void Display()
    {
        cout << "Entity properties:" << endl;
        cout << "Type: human" << endl;
        cout << "Name: " << _name << endl;
        cout << "Health: " << _health << endl;
    }
};

class room
{
private:
    // Uses pointers. What are they?
    vector<vector<entity*>> _grid;
    int _length;
    int _width;

public:
    // Constructor
    room(int length, int width)
    {
        _grid.resize(length, vector<entity*>(width, nullptr));
        _length = length;
        _width = width;
    }

    // Destructor - What is its purpose? 
    ~room()
    {
        for(int x = 0; x < _width; x++)
            for(int y = 0; y < _length; y++)
                if (!this->IsEmpty(x,y))
                    // To Stop memory leaks - lookup and use a citation.
                    delete this->Clear(x,y);
    }

    // There is a flaw in this design - what is it? 
    void Display()
    {
        for(int x = 0; x < _width; x++)
        {
            if (x == 0)
            {
                cout << " ";
                for(int y = 0; y < _length; y++)
                    cout << y;
                cout << endl;
            }
            cout << x;

            for(int y = 0; y < _length; y++)
            {
                entity* e = _grid[y][x];
                char symbol;
                if (e == nullptr)
                    symbol = '.';
                else
                    symbol = e->GetSymbol();
                cout << symbol;
            }
            cout << endl;
        }
    }
    
    bool IsEmpty(int x, int y)
    {
        if(x < 0 || y < 0)
            return false;
            
        if(x >= _width || y >= _length)
            return false;

        return _grid[y][x] == nullptr;
    }
    
    void Set(entity* e, int x, int y)
    {
        if (this->IsEmpty(x,y))
            _grid[y][x] = e;
    }

    entity* Clear(int x, int y)
    {
        if (!this->IsEmpty(x,y))
            return _grid[y][x];

        return nullptr;
    }

    // Should use a random no. How?
    void initialise()
    {
        this->Set(new human("Donald", 100),0,0);
        this->Set(new human("Elon", 100), 1,1);
        this->Set(new human("Mark", 100), 2,2);
    }
};

int main()
{
    room r(10,10);
    r.Display();
    r.initialise();
    r.Display();
    return 0;
}