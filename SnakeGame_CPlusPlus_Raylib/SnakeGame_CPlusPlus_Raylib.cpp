#include <raylib.h> // Include the Raylib library for graphics and input
#include <iostream> // Include the iostream library for input/output operations
#include <deque>    // Include the deque header file for using deque data structure
#include <raymath.h>    // Include the raymath header file for using raylib's math functions

Color green = { 173, 204, 96, 255 };    // Define a color named 'green' with specified RGBA values
Color darkGreen = { 43, 51, 24, 255 };  // Define another color named 'darkGreen' with different RGBA values

int cellSize = 30;  // Set the size of each cell in the grid
int cellCount = 25; // Set the total number of cells in the grid

double lastUpdateTime = 0;  // Initialize a variable to store the time of the last update

// This function checks if a given Vector2 element exists in a deque container.
bool ElementInDeque(Vector2 element, std::deque<Vector2> deque)
{   
    // Iterate through each element in the deque container
    for (size_t i = 0; i < deque.size(); i++)
    {
        // Check if the current element in the deque is equal to the target element
        if (Vector2Equals(deque[i], element))
        {
            // If a match is found, return true to indicate that the element exists in the deque
            return true;
        }
    }
    return false;   // If no match is found after iterating through the entire deque, return false
}

// This function checks if a certain time interval has elapsed since the last event update.
bool EventTriggered(double interval)
{
    double currentTime = GetTime(); // Get the current time in seconds.

    // Check if the difference between the current time and the time of the last update
    // is greater than or equal to the specified interval
    if (currentTime - lastUpdateTime >= interval)
    {
        // If the condition is true, update the last update time to the current time
        lastUpdateTime = currentTime;
        // Return true to indicate that the event should be triggered
        return true;
    }
    return false;   // If the condition is false, return false to indicate that the event should not be triggered
}

class Snake
{
public:
    std::deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Draw()
    {
        for (size_t i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{x*cellSize, y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5f, 6.0f, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {            
            addSegment = false;
        }
        else
        {
            body.pop_back();            
        }        
    }

    void Reset()
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

class Food  // Class to represent the food in the game
{
public:
    Vector2 position;    
    Texture2D texture;   // Texture of the food image

    Food(std::deque<Vector2> snakeBody)  // Constructor of the Food class
    {
        Image image = LoadImage("Graphics/food.png");   // Load the food image from the file
        texture = LoadTextureFromImage(image);  // Create a texture from the image
        UnloadImage(image); // Unload the image from memory

        position = GenerateRandomPosition(snakeBody);
    }

    ~Food() // Destructor of the Food class
    {
        UnloadTexture(texture); // Unload the texture from memory
    }

    void Draw() // Function to draw the food on the screen
    {
        DrawTexture(texture, position.x*cellSize, position.y*cellSize, WHITE);  // Draw the texture at the position with white color
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        Vector2 position = { x, y };

        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPosition(std::deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();

        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }

        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running == true)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }       
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPosition(snake.body);
            snake.addSegment = true;
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPosition(snake.body);
        running = false;
    }

    void CheckCollisionWithTail()
    {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    std::cout << "Game Commence\n"; // Print a message to the console indicating game start

    InitWindow(cellSize * cellCount, cellSize * cellCount, "Snake Game");   // Create the game window with appropriate size and title
    SetTargetFPS(60);   // Set the target frames per second for smooth animation

    Game game = Game();
   

    while (WindowShouldClose() == false)    // Main game loop (runs until the window is closed)
    {
        BeginDrawing(); // Begin drawing operations

        if (EventTriggered(0.2))
        {
            game.Update();
        }   

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }

        ClearBackground(green); // Clear the screen with the light green background color        
        game.Draw();

        EndDrawing();   // End drawing operations
    }

    CloseWindow();  // Close the game window

    return 0;
}
