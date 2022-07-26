#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Rover;

int NUM_OF_DIRECTIONS;
map<int, pair<char, pair<int,int>> > direction_map;

class Direction
{

public:
    char direction_character;
    int direction_integer;

    Direction()
    {
        direction_character = 'N';
        direction_integer = 0;
    }
    Direction(char _direction_character, int _direction_integer)
    {
        direction_character = _direction_character;
        direction_integer = _direction_integer;
    }

    int leftDirectionInteger(){
        int t = direction_integer - ((direction_character / NUM_OF_DIRECTIONS) * NUM_OF_DIRECTIONS);
        if (t < 0)
            t += NUM_OF_DIRECTIONS;
        return t;
    }

    int rightDirectionInteger(){
        return (direction_integer + 1) % NUM_OF_DIRECTIONS;
    }


};

class Coordinate
{

public:
    int xCoordinate, yCoordinate;

    Coordinate()
    {
        xCoordinate = 0;
        yCoordinate = 0;
    }

    Coordinate(int _x, int _y)
    {
        xCoordinate = _x;
        yCoordinate = _y;
    }
};

class Command
{

public:
    char command_identifier;

    Command()
    {
        command_identifier = 'X';
    }
    Command(char _commandIdentifier)
    {

        command_identifier = toupper(_commandIdentifier);
    }

    virtual void runCommand(Rover *r) = 0;

};
map<char, Command *> commands;


vector<Command *> parsedCommands(string _roverPath)
{
    vector<Command *> roverCommands;
    for (int i = 0; i < _roverPath.size(); i++)
    {
        roverCommands.push_back(commands[_roverPath[i]]);
    }

    return roverCommands;
}


class Rover
{

public:
    int rover_id;
    Coordinate roverCoordinate;
    Direction roverDirection;
    string roverPath;
    bool isOnPlateau;

    Rover(int _roverId, Coordinate _roverCoordinate, Direction _roverDirection, string _roverPath)
    {
        rover_id = _roverId;
        roverCoordinate = _roverCoordinate;
        roverDirection = _roverDirection;
        isOnPlateau = true;
        roverPath = _roverPath;
    }

        void runRover(vector<Command*> roverCommands)
        {


            for (int i = 0; i < roverCommands.size(); i++)
            {
                Command *roverCommand = roverCommands[i];
                roverCommand->runCommand(this);
            }
        }

};
vector<Rover *> rovers;



class Plateau
{

    Coordinate bottomLeft, topRight;
    bool validateXCoordinate(int _x)
    {
        return bottomLeft.xCoordinate <= _x && topRight.xCoordinate >= _x;
    }
    bool validateYCoordinate(int _y)
    {
        return bottomLeft.yCoordinate <= _y && topRight.yCoordinate >= _y;
    }

public:
    Plateau()
    {
        bottomLeft = Coordinate(0, 0);
        topRight = Coordinate(0, 0);
    }
    Plateau(Coordinate _bottomLeft, Coordinate _topRight)
    {
        bottomLeft = _bottomLeft;
        topRight = _topRight;
    }

    bool validate(Coordinate new_position)
    {
        return validateXCoordinate(new_position.xCoordinate) && validateYCoordinate(new_position.yCoordinate);
    }

    void runRoversSequentially()
    {
        for (int i = 0; i < rovers.size(); i++)
        {

            rovers[i]->runRover(parsedCommands(rovers[i]->roverPath));
            cout << endl;
        }
    }


    void printFinalPositions()
    {

        for (int i = 0; i < rovers.size(); i++)
        {
            cout << endl
                 << "Position of Rover #" << i << ": " << rovers[i]->roverCoordinate.xCoordinate << "," << rovers[i]->roverCoordinate.yCoordinate;
        }
    }

};
Plateau plateau;


class RotateLeftCommand : public Command
{


public:
    RotateLeftCommand()
    {
        command_identifier = 'L';
    }
    void runCommand(Rover *r)
    {

        int new_rover_direction = r->roverDirection.leftDirectionInteger();
        r->roverDirection.direction_integer = new_rover_direction;
        r->roverDirection.direction_character = direction_map[new_rover_direction].first;
        cout << endl
             << "Rotating left, changing direction of " << r->rover_id << " to " << r->roverDirection.direction_character;
    }
};

class RotateRightCommand : public Command
{
public:
    RotateRightCommand()
    {
        command_identifier = 'R';
    }
    void runCommand(Rover *r)
    {
        int new_rover_direction = r->roverDirection.rightDirectionInteger();
        r->roverDirection.direction_integer = new_rover_direction;
        r->roverDirection.direction_character = direction_map[new_rover_direction].first;

        cout << endl
             << " Rotating right, changing direction of " << r->rover_id << " to " << r->roverDirection.direction_character;
    }
};

class MoveCommand : public Command
{

public:
    MoveCommand()
    {
        command_identifier = 'M';
    }
    void runCommand(Rover *r)
    {

        if (!r->isOnPlateau)
        {
            cout << endl
                 << "Rover is outside the plateau";
        }
        else
        {

            int current_rover_direction = r->roverDirection.direction_integer;
            r->roverCoordinate.xCoordinate += direction_map[current_rover_direction].second.first;
            r->roverCoordinate.yCoordinate += direction_map[current_rover_direction].second.second;
            cout << endl
                 << "Moving " << r->rover_id << " to " << r->roverCoordinate.xCoordinate << "," << r->roverCoordinate.yCoordinate << "," << r->roverDirection.direction_character;
            if (!plateau.validate(Coordinate(r->roverCoordinate.xCoordinate, r->roverCoordinate.yCoordinate)))
            {

                cout << endl
                     << "Rover moved outside of the plateau.";
                r->isOnPlateau = false;
            }
        }
    }
};



void constructCommands()
{

    commands['L'] = new RotateLeftCommand();
    commands['R'] = new RotateRightCommand();
    commands['M'] = new MoveCommand();
}

void constructDirections(){


    direction_map[0] = {'N',{0,1}};
    direction_map[1] = {'E',{1,0}};
    direction_map[2] = {'S',{0,-1}};
    direction_map[3] = {'W',{-1,0}};

    NUM_OF_DIRECTIONS = direction_map.size();
}

void constructPlateau()
{
    plateau = Plateau(Coordinate(0, 0), Coordinate(5, 5));
}

void constructRovers()
{

    rovers.push_back(new Rover(0, Coordinate(1, 2), Direction('N', 0), "LMLMLMLMM"));
    rovers.push_back(new Rover(1, Coordinate(3, 3), Direction('E', 1), "MMRMMRMRRM"));
    rovers.push_back(new Rover(2, Coordinate(3, 3), Direction('E', 0), "MMMMMMM"));
}

void initialize()
{

    constructCommands();
    constructDirections();
    constructPlateau();
    constructRovers();
}





int main()
{

    initialize();
    plateau.runRoversSequentially();
    plateau.printFinalPositions();

    return 0;
}
