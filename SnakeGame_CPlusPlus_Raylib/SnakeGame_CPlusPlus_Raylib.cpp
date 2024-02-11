#include <raylib.h> // Include the Raylib library for graphics and input
#include <iostream> // Include the iostream library for input/output operations
#include <deque>    // Include the deque header file for using deque data structure
#include <raymath.h>    // Include the raymath header file for using raylib's math functions

Color green = { 173, 204, 96, 255 };    // Define a color named 'green' with specified RGBA values
Color darkGreen = { 43, 51, 24, 255 };  // Define another color named 'darkGreen' with different RGBA values

int cellSize = 30;  // Set the size of each cell in the grid
int cellCount = 25; // Set the total number of cells in the grid
int offset = 75;    // Define an offset value to adjust positions or coordinates in the grid

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
    std::deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} }; // Define the snake's initial body position
    Vector2 direction = { 1, 0 };   // Set the initial direction of the snake
    bool addSegment = false;    // Flag to determine whether to add a new segment to the snake's body

    void Draw() // Method to draw the snake on the screen
    {
        for (size_t i = 0; i < body.size(); i++)    // Iterate over each segment of the snake's body
        {
            float x = body[i].x;    // Get the x-coordinate of the current segment
            float y = body[i].y;    // Get the y-coordinate of the current segment
            Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};  // Define a rectangle representing the current segment's position and size
            DrawRectangleRounded(segment, 0.5f, 6.0f, darkGreen);   // Draw the current segment on the screen
        }
    }

    void Update()   // Method to update the snake's position
    {
        body.push_front(Vector2Add(body[0], direction));    // Move the snake's head in the current direction
        if (addSegment == true) // If a new segment is flagged to be added
        {            
            addSegment = false; // Reset the flag
        }
        else
        {
            body.pop_back();    // Remove the last segment of the snake's body            
        }        
    }

    void Reset()    // Method to reset the snake's position and direction
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} }; // Reset the snake's body to its initial position
        direction = { 1, 0 };   // Reset the snake's direction to its initial direction
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
        DrawTexture(texture, offset + position.x*cellSize, offset + position.y*cellSize, WHITE);  // Draw the texture at the position with white color
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
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {        
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

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
            score++;
            PlaySound(eatSound);
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
        score = 0;
        PlaySound(wallSound);
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

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");   // Create the game window with appropriate size and title
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
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5.0f, darkGreen);
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();   // End drawing operations
    }

    CloseWindow();  // Close the game window

    return 0;
}

